/**
 * @file main.cpp
 * @brief Convierte cuatro NFA de ejemplo y genera su documento Typst y PDF.
 */
#include <iostream>
#include <string>
#include <vector>
#include "automatas.hpp"
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

} // namespace

int main()
{
  const vector<Conversion> cases = build_examples();
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
