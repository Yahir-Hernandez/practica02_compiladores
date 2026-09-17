/**
 * @file typst_doc.cpp
 * @brief Implementacion del generador del documento Typst.
 */
#include "typst_doc.hpp"
#include "to_string.hpp"

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <set>
#include <system_error>

namespace
{
  /** Version del paquete `finite` contra la que se genera el documento. */
  constexpr const char *FINITE_VERSION = "0.5.1";

  /**
   * Preambulo del documento: importa `finite` y define los ayudantes de estilo
   * que reutiliza cada seccion. `{FINITE}` se sustituye por FINITE_VERSION.
   */
  constexpr const char *PREAMBLE = R"TYPST(#import "@preview/finite:{FINITE}": automaton, create-automaton, transition-table, layout as fin-layout

#set page(
  paper: "a4",
  margin: (x: 1.8cm, top: 2cm, bottom: 1.6cm),
  footer: context align(center, text(8pt, fill: luma(45%), counter(page).display("1"))),
)
#set text(font: ("Libertinus Serif", "New Computer Modern", "DejaVu Serif"), size: 10.5pt, lang: "es")
#set par(justify: true)

#let nfa-color = rgb("#2f6f9f")
#let dfa-color = rgb("#c06014")

#show heading.where(level: 1): it => block(above: 1.6em, below: 0.9em, breakable: false)[
  #text(13pt, weight: "bold", fill: rgb("#14304d"))[#it.body]
  #v(-0.5em)
  #line(length: 100%, stroke: 0.6pt + rgb("#14304d"))
]

// Normaliza la especificacion y ordena los simbolos de entrada; al quedar
// marcada como `finite-spec` los comandos de `finite` la usan tal cual.
#let prep(spec) = {
  let s = create-automaton(spec)
  s.inputs = s.inputs.sorted()
  s
}

// "q0" -> q con subindice 0.
#let st(name) = {
  let m = name.match(regex(`^(\D+)(\d+)$`.text))
  if m != none { [#m.captures.at(0)#sub(m.captures.at(1))] } else { [#name] }
}

// ("q0", "q1") -> {q0, q1}
#let stset(names) = {
  if names.len() == 0 { [$emptyset$] } else { [\{#names.map(st).join(", ")\}] }
}

// Contenido de una celda de la tabla de transiciones.
#let cell(value) = {
  let parts = value.split(",").map(p => p.trim()).filter(p => p != "")
  if parts.len() == 0 { [] } else if parts.len() == 1 { st(parts.first()) } else { stset(parts) }
}

#let aut-style(color) = (
  state: (
    stroke: 0.8pt + color,
    fill: color.lighten(93%),
    label: (size: 0.95em),
    initial: (
      stroke: 0.8pt + color,
      label: (text: "inicio", size: 0.7em, dist: 0.22),
    ),
  ),
  transition: (stroke: 0.7pt + luma(30%), label: (size: 0.85em)),
)

// Acomodo de Graphviz (motor `dot`) transpuesto para que el automata crezca de
// izquierda a derecha en lugar de hacia abajo.
#let dot-lr(spec, style: (:)) = {
  let (positions, anchors) = fin-layout.diagraph(spec, engine: "dot", style: style)
  let rotated = (:)
  for (name, point) in positions {
    rotated.insert(name, (-point.at(1), point.at(0)))
  }
  (rotated, anchors)
}

// Reduce el contenido si no cabe en el ancho disponible. El margen evita que el
// diagrama quede pegado al borde del recuadro.
#let fit(body, margin: 0.94) = layout(size => {
  let available = size.width * margin
  let measured = measure(body)
  let factor = if measured.width > available { available / measured.width } else { 1.0 }
  if factor == 1.0 { body } else {
    scale(body, x: factor * 100%, y: factor * 100%, origin: top + left, reflow: true)
  }
})

// Diagrama del automata, garantizado a la medida de la columna.
#let diagram(spec, color) = fit(automaton(
  spec,
  layout: dot-lr,
  style: aut-style(color),
  length: 0.82cm,
))

#let table-of(spec) = transition-table(
  spec,
  format: (col, row, v) => if col == 0 or row == 0 { strong(cell(v)) } else { cell(v) },
  format-list: states => states.join(","),
  stroke: 0.4pt + luma(60%),
  inset: (x: 6pt, y: 3.5pt),
)

#let panel(title, color, body) = block(
  width: 100%,
  radius: 4pt,
  inset: (x: 8pt, y: 9pt),
  stroke: 0.6pt + color.lighten(35%),
  fill: color.lighten(96%),
)[
  #align(center)[#text(9.5pt, weight: "bold", fill: color, tracking: 0.05em)[#upper(title)]]
  #v(4pt)
  #align(center)[#body]
]

#let captioned(title, body) = align(center)[
  #text(9pt, weight: "bold")[#title]
  #v(4pt)
  #body
]

// Tabla que explica que subconjunto del NFA representa cada estado del DFA.
#let subset-table(rows) = captioned(
  "Correspondencia de estados (construcción de subconjuntos)",
  table(
    columns: 3,
    align: (center + horizon, left + horizon, center + horizon),
    inset: (x: 8pt, y: 4pt),
    stroke: 0.4pt + luma(60%),
    fill: (c, r) => if r == 0 { luma(240) },
    table.header([*Estado del DFA*], [*Subconjunto del NFA*], [*Final*]),
    ..rows.map(r => (st(r.at(0)), stset(r.at(1)), if r.at(2) { [sí] } else { [—] })).flatten(),
  ),
)

// Bloque completo de un ejemplo: diagramas, tablas y correspondencia.
#let ejemplo(nfa-spec, dfa-spec, subsets) = {
  let n = prep(nfa-spec)
  let d = prep(dfa-spec)
  panel("NFA — antes de la conversión", nfa-color, diagram(n, nfa-color))
  v(8pt)
  panel("DFA — después de la conversión", dfa-color, diagram(d, dfa-color))
  v(10pt)
  grid(
    columns: (1fr, 1fr),
    column-gutter: 10pt,
    captioned("Tabla de transiciones del NFA", table-of(n)),
    captioned("Tabla de transiciones del DFA", table-of(d)),
  )
  v(10pt)
  subset-table(subsets)
}

#align(center)[
  #text(19pt, weight: "bold")[Conversión de NFA a DFA]
  #v(-4pt)
  #text(11pt, fill: luma(40%))[Construcción de subconjuntos · Práctica 2 de Compiladores]
]
)TYPST";

  /** Sustituye todas las apariciones de @p needle dentro de @p text. */
  std::string replace_all(std::string text, const std::string &needle,
                          const std::string &value)
  {
    std::size_t pos = 0;
    while ((pos = text.find(needle, pos)) != std::string::npos)
    {
      text.replace(pos, needle.size(), value);
      pos += value.size();
    }
    return text;
  }

  /** Serializa las filas de la tabla de correspondencia de subconjuntos. */
  std::string subsets_to_typst(const Conversion &conversion)
  {
    const std::set<IDstate> &finals = conversion.dfa.get_final_states();
    std::string result = "(\n";
    for (std::size_t i = 0; i < conversion.subsets.size(); ++i)
    {
      const IDstate id = static_cast<IDstate>(i);
      result += "    (\"" + state_name(DFA_PREFIX, id) + "\", (";
      for (IDstate nfa_state : conversion.subsets[i])
      {
        result += "\"" + state_name(NFA_PREFIX, nfa_state) + "\", ";
      }
      result += "), ";
      result += finals.count(id) ? "true" : "false";
      result += "),\n";
    }
    result += "  )";
    return result;
  }

  /** Escapa los caracteres con significado especial en el modo markup de Typst. */
  std::string escape_markup(const std::string &text)
  {
    std::string result;
    for (char c : text)
    {
      if (c == '#' || c == '$' || c == '*' || c == '_' || c == '@' || c == '\\' ||
          c == '<' || c == '>' || c == '[' || c == ']' || c == '{' || c == '}')
      {
        result += '\\';
      }
      result += c;
    }
    return result;
  }
} // namespace

Conversion convert_and_trace(const std::string &title, const NFA &nfa)
{
  Conversion conversion{title, nfa, DFA(), SubsetTable()};
  conversion.dfa = nfa_to_dfa(nfa, &conversion.subsets);
  return conversion;
}

std::string build_typst_document(const std::vector<Conversion> &cases)
{
  std::string document = replace_all(PREAMBLE, "{FINITE}", FINITE_VERSION);

  for (const Conversion &conversion : cases)
  {
    if (&conversion != &cases.front())
    {
      document += "\n\n#pagebreak()";
    }
    document += "\n\n= " + escape_markup(conversion.title) + "\n\n";
    document += "#ejemplo(\n";
    document += "  " + nfa_to_string(conversion.nfa, "  ") + ",\n";
    document += "  " + dfa_to_string(conversion.dfa, "  ") + ",\n";
    document += "  " + subsets_to_typst(conversion) + ",\n";
    document += ")\n";
  }

  return document;
}

bool write_document(const std::string &path, const std::string &document)
{
  const std::filesystem::path file(path);
  std::error_code error;
  if (file.has_parent_path())
  {
    std::filesystem::create_directories(file.parent_path(), error);
  }

  std::ofstream out(file);
  if (!out)
  {
    return false;
  }
  out << document;
  return out.good();
}

int compile_document(const std::string &typ_path, const std::string &out_path)
{
  const std::string command =
      "typst compile \"" + typ_path + "\" \"" + out_path + "\"";
  return std::system(command.c_str());
}
