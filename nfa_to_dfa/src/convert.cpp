/**
 * @file convert.cpp
 * @brief Implementacion de las operaciones de conversion NFA-DFA.
 */
#include "convert.hpp"

#include <map>
#include <queue>
#include <stack>

std::set<IDstate> move(
    std::set<IDstate> st,
    char symbol,
    const NFA &nfa)
{
  std::set<IDstate> result;
  for (const auto &state : st)
  {
    result.merge(nfa.delta(state, symbol));
  }
  return result;
}

std::set<IDstate> eClosure(std::set<IDstate> st, const NFA &nfa)
{
  std::stack<IDstate> stack;
  std::set<IDstate> result = st;
  for (IDstate state : st)
  {
    stack.push(state);
  }

  while (!stack.empty())
  {
    IDstate state = stack.top();
    stack.pop();
    for (const auto &next_state : nfa.get_state(state).epsilon)
    {
      if (result.insert(next_state).second)
      {
        stack.push(next_state);
      }
    }
  }
  return result;
}

DFA nfa_to_dfa(const NFA &nfa, SubsetTable *subsets) {
  DFA dfa;
  std::queue<std::set<IDstate>> qe;
  std::set<IDstate> init_state = eClosure({nfa.get_initial_state()}, nfa);
  // Mapea cada subconjunto NFA a su identificador en el DFA.
  std::map<std::set<IDstate>, IDstate> state_map;
  state_map[init_state] = dfa.add_state();
  const std::set<char> &alphabet = nfa.get_alphabet();
  dfa.set_alphabet(alphabet);
  qe.push(init_state);

  while (!qe.empty()) {
    std::set<IDstate> tempU = qe.front();
    qe.pop();
    for (char symbol : alphabet) {
      std::set<IDstate> tempV = eClosure(move(tempU, symbol, nfa), nfa);
      if (!tempV.empty()) {
        if (state_map.find(tempV) == state_map.end()) {
          state_map[tempV] = dfa.add_state();
          qe.push(tempV);
        }
        dfa.add_transition(state_map[tempU], symbol, state_map[tempV]);
      }
    }
  }

  // Un subconjunto es final si contiene al menos un estado final del NFA.
  const std::set<IDstate> final_states = nfa.get_final_states();
  for (const auto &entry : state_map) {
    for (IDstate state : entry.first) {
      if (final_states.count(state)) {
        dfa.add_final_state(entry.second);
        break;
      }
    }
  }

  // Publica la correspondencia estado del DFA -> subconjunto del NFA.
  if (subsets != nullptr) {
    subsets->assign(dfa.get_num_states(), {});
    for (const auto &entry : state_map) {
      (*subsets)[entry.second] = entry.first;
    }
  }

  return dfa;
}
