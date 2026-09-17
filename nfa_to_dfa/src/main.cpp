/**
 * @file main.cpp
 * @brief Convierte cuatro NFA de ejemplo y genera su documento Typst y PDF.
 */
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include "automatas.hpp"
#include "to_string.hpp"
#include "typst_doc.hpp"

using namespace std;

namespace
{
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

  /** Imprime los automatas y la correspondencia de estados de cada ejemplo. */
  void print_case(const Conversion &conversion)
  {
    cout << "==== " << conversion.title << " ====\n";
    cout << "Alfabeto: " << alphabet_to_string(conversion.nfa.get_alphabet()) << "\n";
    cout << "NFA (" << conversion.nfa.get_num_states() << " estados):\n";
    cout << nfa_to_string(conversion.nfa) << "\n";
    cout << "DFA (" << conversion.dfa.get_num_states() << " estados):\n";
    cout << dfa_to_string(conversion.dfa) << "\n";
    cout << "Correspondencia de subconjuntos:\n";
    for (std::size_t i = 0; i < conversion.subsets.size(); ++i)
    {
      cout << "  " << state_name(DFA_PREFIX, static_cast<IDstate>(i)) << " = "
           << state_set_to_string(conversion.subsets[i], NFA_PREFIX) << "\n";
    }
    cout << "\n";
  }
} // namespace

int main()
{
  const vector<Conversion> cases = build_examples();
  for (const Conversion &conversion : cases)
  {
    print_case(conversion);
  }

  const string typ_path = "salida/nfa_to_dfa.typ";
  const string pdf_path = "salida/nfa_to_dfa.pdf";

  if (!write_document(typ_path, build_typst_document(cases)))
  {
    cerr << "Error: no se pudo escribir " << typ_path << "\n";
    return 1;
  }
  cout << "Documento Typst generado en " << typ_path << "\n";

  if (compile_document(typ_path, pdf_path) != 0)
  {
    cerr << "Error: no se pudo compilar el documento con Typst.\n";
    return 1;
  }
  cout << "Documento compilado en " << pdf_path << "\n";
  return 0;
}
