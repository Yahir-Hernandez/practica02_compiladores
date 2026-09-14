// convierte un automata a string para poder ser parseado a Typst
#include "to_string.hpp"
/**
 * (
    states: states,
    alphabet: alphabet,
    transitions: transitions,
    start-state: actual-start,
    final-states: final-states,
  )
 */
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
  result += " alphabet: (";
  const auto &alphabet = nfa.get_alphabet();
  for (auto it = alphabet.begin(); it != alphabet.end(); ++it)
  {
    result += "\"" + std::string(1, *it) + "\"";
    if (std::next(it) != alphabet.end())
    {
      result += ", ";
    }
  }
  result += "),\n";
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
  result += " final-states: (";
  for (const auto &state : nfa.get_final_states())
  {
    result += "\"" + std::to_string(state) + "\"";
    if (std::next(nfa.get_final_states().begin()) != nfa.get_final_states().end())
    {
      result += ", ";
    }
  }
  result += "),\n";
  result += ")";
  return result;
};



