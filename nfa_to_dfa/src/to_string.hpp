/**
 * @file to_string.hpp
 * @brief Serializacion de automatas al formato de especificacion del paquete
 *        Typst `finite` (https://typst.app/universe/package/finite).
 *
 * `finite` describe un automata con una tabla de transiciones indexada por
 * estado origen y luego por estado destino:
 *
 * @code
 * (
 *   states: ("q0", "q1"),
 *   transitions: (
 *     q0: (q0: ("b",), q1: ("a", "e")),
 *     q1: (:),
 *   ),
 *   initial: "q0",
 *   final: ("q1",),
 * )
 * @endcode
 */
#pragma once
#include <set>
#include <string>
#include "automatas.hpp"

/** Prefijo de los nombres de estado del NFA (q0, q1, ...). */
inline constexpr const char *NFA_PREFIX = "q";

/** Prefijo de los nombres de estado del DFA (D0, D1, ...). */
inline constexpr const char *DFA_PREFIX = "D";

/** Etiqueta con la que se representa una transicion epsilon. */
inline constexpr const char *EPSILON = "ε";

/**
 * @brief Construye el nombre visible de un estado.
 * @param prefix Prefijo del automata (NFA_PREFIX o DFA_PREFIX).
 * @param state Identificador numerico del estado.
 * @return Nombre del estado, por ejemplo "q3".
 */
std::string state_name(const std::string &prefix, IDstate state);

/**
 * @brief Serializa los estados finales como arreglo de Typst.
 * @param f_states Estados finales que se serializaran.
 * @param prefix Prefijo con el que se nombran los estados.
 * @return Texto de la forma `("q1", "q2")`.
 */
std::string final_states_to_string(const std::set<IDstate> &f_states,
                                   const std::string &prefix);

/**
 * @brief Serializa un NFA, incluyendo transiciones epsilon.
 * @param nfa Automata que se serializara.
 * @param indent Sangria que antecede a cada linea generada.
 * @return Especificacion de `finite` lista para pasarse a `#automaton`.
 */
std::string nfa_to_string(const NFA &nfa, const std::string &indent = "");

/**
 * @brief Serializa un DFA y sus transiciones deterministas.
 * @param dfa Automata que se serializara.
 * @param indent Sangria que antecede a cada linea generada.
 * @return Especificacion de `finite` lista para pasarse a `#automaton`.
 */
std::string dfa_to_string(const DFA &dfa, const std::string &indent = "");
