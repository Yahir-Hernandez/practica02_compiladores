/**
 * @file main.cpp
 * @brief Programa de demostracion de la conversion NFA-DFA.
 *
 * Construye varios NFA de ejemplo, los convierte a DFA y genera un documento
 * Typst que muestra lado a lado el automata antes y despues de la conversion.
 * El documento se compila invocando `typst` desde la consola.
 */
#include <iostream>
#include <string>
#include <vector>
#include "automatas.hpp"
#include "to_string.hpp"
#include "convert.hpp"
#include "typst_doc.hpp"

using namespace std;

namespace
{
  /** @struct Options @brief Opciones de linea de comandos. */
  struct Options
  {
    /** Directorio donde se escriben el `.typ` y el documento compilado. */
    string out_dir = "salida";
    /** Formato de salida: pdf, png o svg. */
    string format = "pdf";
    /** Si es true solo se genera el `.typ`, sin invocar a typst. */
    bool no_compile = false;
    /** Si es true el programa solo imprime la ayuda. */
    bool help = false;
    /** Mensaje de error de analisis, vacio si los argumentos son validos. */
    string error;
  };

  /** Imprime el modo de uso del programa. */
  void print_usage(const char *program)
  {
    cout << "Uso: " << program << " [opciones]\n\n"
         << "  -o, --out DIR      Directorio de salida (por omision: salida)\n"
         << "  -f, --format FMT   Formato de salida: pdf, png o svg (por omision: pdf)\n"
         << "  -n, --no-compile   Solo genera el archivo .typ, sin invocar a typst\n"
         << "  -h, --help         Muestra esta ayuda\n";
  }

  /** Analiza los argumentos de linea de comandos. */
  Options parse_options(int argc, char **argv)
  {
    Options options;
    for (int i = 1; i < argc; ++i)
    {
      const string arg = argv[i];
      const bool needs_value = (arg == "-o" || arg == "--out" ||
                                arg == "-f" || arg == "--format");
      if (needs_value && i + 1 >= argc)
      {
        options.error = "la opcion " + arg + " requiere un valor";
        return options;
      }

      if (arg == "-o" || arg == "--out")
      {
        options.out_dir = argv[++i];
      }
      else if (arg == "-f" || arg == "--format")
      {
        options.format = argv[++i];
      }
      else if (arg == "-n" || arg == "--no-compile")
      {
        options.no_compile = true;
      }
      else if (arg == "-h" || arg == "--help")
      {
        options.help = true;
      }
      else
      {
        options.error = "opcion desconocida: " + arg;
        return options;
      }
    }

    if (options.format != "pdf" && options.format != "png" && options.format != "svg")
    {
      options.error = "formato no soportado: " + options.format;
    }
    return options;
  }

  /** @return Los NFA de ejemplo junto con el titulo de su seccion. */
  vector<Conversion> build_examples()
  {
    vector<Conversion> cases;

    // Acepta las cadenas sobre {a, b} que contienen la subcadena "ab".
    cases.push_back(convert_and_trace(
        "Ejemplo 1: cadenas que contienen la subcadena ab",
        NFA(
            {
                {{{'a', {1}}, {'b', {0}}}, {}, false},
                {{{'a', {1}}, {'b', {2}}}, {}, false},
                {{{'a', {2}}, {'b', {2}}}, {}, true},
            },
            0,
            {'a', 'b'})));

    // Transicion epsilon desde el estado inicial y un estado inalcanzable.
    cases.push_back(convert_and_trace(
        "Ejemplo 2: transición epsilon y estados inalcanzables",
        NFA(
            {
                {{{'a', {0}}, {'b', {0}}}, {2}, false},
                {{{'c', {2}}}, {}, true},
                {{}, {}, true},
            },
            0,
            {'a', 'b', 'c'})));

    // Un mismo simbolo lleva a varios destinos desde el mismo estado.
    cases.push_back(convert_and_trace(
        "Ejemplo 3: un símbolo con varios destinos",
        NFA(
            {
                {{{'a', {1}}}, {}, true},
                {{{'a', {2}}, {'b', {0, 2}}}, {}, false},
                {{{'a', {3}}}, {}, false},
                {{{'a', {2}}}, {0}, false},
            },
            0,
            {'a', 'b'})));

    // No determinismo y transiciones epsilon combinados.
    cases.push_back(convert_and_trace(
        "Ejemplo 4: no determinismo y transiciones epsilon combinados",
        NFA(
            {
                {{{'a', {0, 3}}}, {1}, false},
                {{{'b', {2}}}, {2}, false},
                {{}, {}, true},
                {{{'a', {3}}}, {}, false},
            },
            0,
            {'a', 'b'})));

    return cases;
  }

  /** Imprime en consola el resumen de un caso y las cadenas generadas. */
  void print_case(const Conversion &conversion)
  {
    cout << "==== " << conversion.title << " ====\n";
    cout << "Alfabeto: " << alphabet_to_string(conversion.nfa.get_alphabet()) << "\n";
    cout << "NFA (" << conversion.nfa.get_num_states() << " estados):\n";
    cout << nfa_to_string(conversion.nfa) << "\n";
    cout << "DFA (" << conversion.dfa.get_num_states() << " estados):\n";
    cout << dfa_to_string(conversion.dfa) << "\n";
    cout << "Correspondencia de subconjuntos:\n";
    for (size_t i = 0; i < conversion.subsets.size(); ++i)
    {
      cout << "  " << state_name(DFA_PREFIX, static_cast<IDstate>(i)) << " = "
           << state_set_to_string(conversion.subsets[i], NFA_PREFIX) << "\n";
    }
    cout << "\n";
  }
} // namespace

int main(int argc, char **argv)
{
  const Options options = parse_options(argc, argv);
  if (options.help)
  {
    print_usage(argv[0]);
    return 0;
  }
  if (!options.error.empty())
  {
    cerr << "Error: " << options.error << "\n\n";
    print_usage(argv[0]);
    return 2;
  }

  const vector<Conversion> cases = build_examples();
  for (const Conversion &conversion : cases)
  {
    print_case(conversion);
  }

  const string typ_path = options.out_dir + "/nfa_to_dfa.typ";
  if (!write_document(typ_path, build_typst_document(cases)))
  {
    cerr << "Error: no se pudo escribir " << typ_path << "\n";
    return 1;
  }
  cout << "Documento Typst generado en " << typ_path << "\n";

  if (options.no_compile)
  {
    return 0;
  }

  const string typst = typst_command();
  if (typst.empty())
  {
    cerr << "Aviso: no hay forma de invocar a typst, el documento no se compilo.\n"
            "       Opciones: instalarlo desde https://github.com/typst/typst/releases,\n"
            "       o ejecutar el programa desde el directorio que contiene\n"
            "       typst-docker.sh, que lo corre dentro de un contenedor.\n";
    return 1;
  }
  cout << "Compilando con: " << typst << "\n";

  // typst exige una plantilla con el numero de pagina cuando la salida son
  // imagenes, porque escribe un archivo por pagina.
  const string out_path = options.format == "pdf"
                              ? options.out_dir + "/nfa_to_dfa.pdf"
                              : options.out_dir + "/nfa_to_dfa-{p}." + options.format;

  const int status = compile_document(typ_path, out_path);
  if (status != 0)
  {
    cerr << "Error: `typst compile` termino con codigo " << status << "\n";
    return 1;
  }
  cout << "Documento compilado en " << out_path << "\n";
  return 0;
}
