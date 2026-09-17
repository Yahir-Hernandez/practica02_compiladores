/**
 * @file convert.hpp
 * @brief Operaciones para convertir un NFA en un DFA equivalente.
 */
#pragma once
#include <set>
#include <vector>
#include "automatas.hpp"

/**
 * @brief Trazabilidad de la construccion de subconjuntos.
 *
 * El elemento i contiene el subconjunto de estados del NFA que dio origen al
 * estado i del DFA. Es la informacion que permite explicar el "antes y despues"
 * de la conversion.
 */
using SubsetTable = std::vector<std::set<IDstate>>;

/**
 * @brief Calcula la operacion move del algoritmo de subconjuntos.
 * @param st Conjunto de estados origen.
 * @param symbol Simbolo que se consume.
 * @param nfa Automata sobre el que se realiza la consulta.
 * @return Union de todos los destinos alcanzables con symbol.
 */
std::set<IDstate> move(std::set<IDstate> st, char symbol, const NFA &nfa);

/**
 * @brief Calcula el cierre epsilon de un conjunto de estados.
 * @param st Conjunto inicial de estados.
 * @param nfa Automata cuyas transiciones epsilon se recorren.
 * @return Estados de st y todos los alcanzables usando solo epsilon.
 */
std::set<IDstate> eClosure(std::set<IDstate> st, const NFA &nfa);

/**
 * @brief Convierte un NFA en un DFA equivalente.
 * @param nfa Automata no determinista de entrada.
 * @param subsets Salida opcional con el subconjunto asociado a cada estado del DFA.
 * @return DFA construido mediante la construccion de subconjuntos.
 */
DFA nfa_to_dfa(const NFA &nfa, SubsetTable *subsets = nullptr);
