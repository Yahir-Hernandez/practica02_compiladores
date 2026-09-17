/**
 * @file to_string.cpp
 * @brief Implementacion de la serializacion de automatas al formato `finite`.
 */
#include "to_string.hpp"

#include <map>
#include <vector>

namespace
{
  /** Escapa los caracteres que no pueden aparecer dentro de una cadena Typst. */
  std::string escape(const std::string &text)
  {
    std::string result;
    for (char c : text)
    {
      if (c == '\\' || c == '"')
      {
        result += '\\';
      }
      result += c;
    }
    return result;
  }

  /** Envuelve un texto entre comillas, escapandolo antes. */
  std::string quote(const std::string &text)
  {
    return "\"" + escape(text) + "\"";
  }

  /**
   * Serializa un arreglo de Typst. Se agrega coma final siempre porque en Typst
   * `("a")` es solo un parentesis y `("a",)` es el arreglo de un elemento.
   */
  std::string to_array(const std::vector<std::string> &items)
  {
    std::string result = "(";
    for (const auto &item : items)
    {
      result += quote(item) + ", ";
    }
    result += ")";
    return result;
  }

  /** Serializa la lista de nombres de estado `("q0", "q1", ...)`. */
  std::string states_to_string(size_t num_states, const std::string &prefix)
  {
    std::vector<std::string> names;
    for (size_t i = 0; i < num_states; ++i)
    {
      names.push_back(state_name(prefix, static_cast<IDstate>(i)));
    }
    return to_array(names);
  }

  /**
   * Serializa el diccionario de destinos de un estado.
   *
   * `finite` agrupa por estado destino, mientras que los automatas de esta
   * practica agrupan por simbolo, asi que hay que invertir la relacion.
   *
   * @param targets Destinos del estado, ya agrupados por identificador.
   * @param prefix Prefijo con el que se nombran los estados.
   * @return Texto de la forma `(q1: ("a", "b"), q2: ("e",))`, o `(:)` si no hay
   *         transiciones salientes.
   */
  std::string targets_to_string(const std::map<IDstate, std::vector<std::string>> &targets,
                                const std::string &prefix)
  {
    if (targets.empty())
    {
      return "(:)";
    }
    std::string result = "(";
    for (auto it = targets.begin(); it != targets.end(); ++it)
    {
      result += state_name(prefix, it->first) + ": " + to_array(it->second);
      if (std::next(it) != targets.end())
      {
        result += ", ";
      }
    }
    result += ")";
    return result;
  }
} // namespace

std::string state_name(const std::string &prefix, IDstate state)
{
  return prefix + std::to_string(state);
}

// Serializa el alfabeto conservando el orden definido por std::set.
std::string alphabet_to_string(const std::set<char> &alphabet)
{
  std::vector<std::string> symbols;
  for (char symbol : alphabet)
  {
    symbols.push_back(std::string(1, symbol));
  }
  return to_array(symbols);
}

// Serializa la lista de estados finales.
std::string final_states_to_string(const std::set<IDstate> &f_states,
                                   const std::string &prefix)
{
  std::vector<std::string> names;
  for (IDstate state : f_states)
  {
    names.push_back(state_name(prefix, state));
  }
  return to_array(names);
}

// Serializa un conjunto de estados en notacion matematica.
std::string state_set_to_string(const std::set<IDstate> &st, const std::string &prefix)
{
  std::string result = "{";
  for (auto it = st.begin(); it != st.end(); ++it)
  {
    result += state_name(prefix, *it);
    if (std::next(it) != st.end())
    {
      result += ", ";
    }
  }
  result += "}";
  return result;
}

// Serializa todos los componentes observables de un NFA.
std::string nfa_to_string(const NFA &nfa, const std::string &indent)
{
  const std::string in1 = indent + "  ";
  const std::string in2 = indent + "    ";

  std::string result = "(\n";
  result += in1 + "states: " + states_to_string(nfa.get_num_states(), NFA_PREFIX) + ",\n";
  result += in1 + "transitions: (\n";
  for (size_t i = 0; i < nfa.get_num_states(); ++i)
  {
    const stateNFA &state = nfa.get_state(static_cast<IDstate>(i));
    // Invierte "simbolo -> destinos" a "destino -> simbolos".
    std::map<IDstate, std::vector<std::string>> targets;
    for (const auto &transition : state.transitions)
    {
      for (IDstate to_state : transition.second)
      {
        targets[to_state].push_back(std::string(1, transition.first));
      }
    }
    for (IDstate to_state : state.epsilon)
    {
      targets[to_state].push_back(EPSILON);
    }
    result += in2 + state_name(NFA_PREFIX, static_cast<IDstate>(i)) + ": " +
              targets_to_string(targets, NFA_PREFIX) + ",\n";
  }
  result += in1 + "),\n";
  result += in1 + "initial: " + quote(state_name(NFA_PREFIX, nfa.get_initial_state())) + ",\n";
  result += in1 + "final: " + final_states_to_string(nfa.get_final_states(), NFA_PREFIX) + ",\n";
  result += indent + ")";
  return result;
}

// Serializa todos los componentes observables de un DFA.
std::string dfa_to_string(const DFA &dfa, const std::string &indent)
{
  const std::string in1 = indent + "  ";
  const std::string in2 = indent + "    ";

  std::string result = "(\n";
  result += in1 + "states: " + states_to_string(dfa.get_num_states(), DFA_PREFIX) + ",\n";
  result += in1 + "transitions: (\n";
  for (size_t i = 0; i < dfa.get_num_states(); ++i)
  {
    std::map<IDstate, std::vector<std::string>> targets;
    for (const auto &transition : dfa.get_transition(static_cast<IDstate>(i)))
    {
      targets[transition.second].push_back(std::string(1, transition.first));
    }
    result += in2 + state_name(DFA_PREFIX, static_cast<IDstate>(i)) + ": " +
              targets_to_string(targets, DFA_PREFIX) + ",\n";
  }
  result += in1 + "),\n";
  result += in1 + "initial: " + quote(state_name(DFA_PREFIX, dfa.get_initial_state())) + ",\n";
  result += in1 + "final: " + final_states_to_string(dfa.get_final_states(), DFA_PREFIX) + ",\n";
  result += indent + ")";
  return result;
}
