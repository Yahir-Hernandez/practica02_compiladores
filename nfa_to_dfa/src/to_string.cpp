/**
 * @file to_string.cpp
 * @brief Implementacion de la serializacion textual de automatas.
 */
#include "to_string.hpp"

// Serializa el alfabeto conservando el orden definido por std::set.
std::string alphabet_to_string(const std::set<char> &alphabet)
{
  std::string result = " alphabet: (";
  for (auto it = alphabet.begin(); it != alphabet.end(); ++it)
  {
    result += "\"" + std::string(1, *it) + "\"";
    if (std::next(it) != alphabet.end())
    {
      result += ", ";
    }
  }
  result += "),\n";
  return result;
}
// Serializa la lista de estados finales.
std::string final_states_to_string(const std::set<IDstate> &f_states)
{
  std::string result = " final-states: (";
  for (const auto &state : f_states)
  {
    result += "\"" + std::to_string(state) + "\"";
    if (std::next(f_states.begin()) != f_states.end())
    {
      result += ", ";
    }
  }
  result += "),\n";
  return result;
}

// Serializa todos los componentes observables de un NFA.
std::string nfa_to_string(const NFA &nfa)
{
  std::string result = "( \n";
  result += " states: (";
  for (size_t i = 0; i < nfa.get_num_states(); ++i)
  {
    result += "\"" + std::to_string(i) + "\"";
    if (i != nfa.get_num_states() - 1)
    {
      result += ", ";
    }
  }
  result += "),\n";
  result += alphabet_to_string(nfa.get_alphabet());
  result += " transitions: (\n";
  for (size_t i = 0; i < nfa.get_num_states(); ++i)
  {
    const auto &state = nfa.get_state(i);
    for (const auto &transition : state.transitions)
    {
      char symbol = transition.first;
      result += " \"(" + std::to_string(i) + ", " + std::string(1, symbol) + ")\": (";
      for (const auto &to_state : transition.second)
      {
        result += "\"" + std::to_string(to_state) + "\"";
        result += ", ";
      }
      result += "),\n";
    }
    if (!state.epsilon.empty())
    {
      result += " \"(" + std::to_string(i) + ", ε)\": (";
      for (const auto &to_state : state.epsilon)
      {
        result += std::to_string(to_state);
        result += ", ";
      }
      result += "),\n";
    }
  }
  result += " ),\n";
  result += " start-state: \"" + std::to_string(nfa.get_initial_state()) + "\",\n";
  result += final_states_to_string(nfa.get_final_states());
  result += ")";
  return result;
};

// Serializa todos los componentes observables de un DFA.
std::string dfa_to_string(const DFA &dfa)
{
  std::string result = "( \n";
  result += " states: (";
  for (size_t i = 0; i < dfa.get_num_states(); ++i)
  {
    result += "\"" + std::to_string(i) + "\"";
    if (i != dfa.get_num_states() - 1)
    {
      result += ", ";
    }
  }
  result += "),\n";
  result += alphabet_to_string(dfa.get_alphabet());
  result += " transitions: (\n";
  for (size_t i = 0; i < dfa.get_num_states(); ++i)
  {
    const auto &transitions = dfa.get_transition(i);
    for (const auto &transition : transitions)
    {
      char symbol = transition.first;
      IDstate to_state = transition.second;
      result += " \"(" + std::to_string(i) + ", " + std::string(1, symbol) + ")\": (\"" + std::to_string(to_state) + "\"),\n";
    }
  }
  result += " ),\n";
  result += " start-state: \"" + std::to_string(dfa.get_initial_state()) + "\",\n";
  result += final_states_to_string(dfa.get_final_states());
  result += ")";
  return result;
}