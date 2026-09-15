/**
 * @file to_string.hpp
 * @brief Declaraciones para serializar automatas en formato textual.
 */
#pragma once
#include <string>
#include "automatas.hpp"

/**
 * @brief Serializa un conjunto de simbolos.
 * @param alphabet Alfabeto que se serializara.
 * @return Texto con el formato usado por Typst.
 */
std::string alphabet_to_string(const std::set<char> &alphabet);

/**
 * @brief Serializa los estados finales.
 * @param f_states Estados finales que se serializaran.
 * @return Texto con la lista de estados finales.
 */
std::string final_states_to_string(const std::set<IDstate> &f_states);

/**
 * @brief Serializa un NFA, incluyendo transiciones epsilon.
 * @param nfa Automata que se serializara.
 * @return Representacion textual del NFA.
 */
std::string nfa_to_string(const NFA &nfa);

/**
 * @brief Serializa un DFA y sus transiciones deterministas.
 * @param dfa Automata que se serializara.
 * @return Representacion textual del DFA.
 */
std::string dfa_to_string(const DFA &dfa);