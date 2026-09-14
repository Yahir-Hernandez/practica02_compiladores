#pragma once
#include <map>
#include <set>
#include <vector>
#include <queue>

// Los estados del NFA y DFA se identifican por un entero (IDstate)
// es mas facil de trebajar porque los ids de los estados serán indices en sus vector correpspondientes
using IDstate = int;

struct stateNFA
{

  // Transición simbolo (char) -> conjunto de estados (conjunto de IDstate)
  std::map<char, std::set<IDstate>> transitions;

  // Transición epsilon -> conjunto de estados (conjunto de IDstate)
  std::set<IDstate> epsilon;

  // Indica si el estado es final o no
  bool is_final;
};

class NFA
{
private:
  // Constructor privado para inicializar el NFA con un conjunto de estados, un estado inicial y un alfabeto

  // Estados del NFA, el indice en el vetor corresponde a la id del estado
  std::vector<stateNFA> states;
  // Estado inicial del NFA
  IDstate initial_state;
  // Conjunto de Simbolo
  std::set<char> alphabet;

public:
  NFA(std::vector<stateNFA> st, IDstate init_st, std::set<char> alph)
      : states(st), initial_state(init_st), alphabet(alph) {}
  IDstate get_initial_state() const
  {
    return initial_state;
  }
  void set_initial_state(IDstate state)
  {
    initial_state = state;
  }

  const stateNFA &get_state(IDstate state) const
  {
    return states[state];
  }

  IDstate add_state(bool is_final)
  {
    states.push_back(stateNFA{{}, {}, is_final});
    return states.size() - 1;
  }

  void add_transition(IDstate from, char symbol, IDstate to)
  {
    states[from].transitions[symbol].insert(to);
    alphabet.insert(symbol);
  }

  void add_epsilon_transition(IDstate from, IDstate to)
  {
    states[from].epsilon.insert(to);
  }

  const std::set<char> &get_alphabet() const
  {
    return alphabet;
  }

  const std::set<IDstate> &get_final_states() const
  {
    static std::set<IDstate> final_states;
    final_states.clear();
    for (size_t i = 0; i < states.size(); ++i)
    {
      if (states[i].is_final)
      {
        final_states.insert(i);
      }
    }
    return final_states;
  }
  size_t get_num_states() const { return states.size(); }
};

class DFA
{
private:
  // transiciones del DFA, el indice en el vector corresponde a la id del estado
  // la transicion es (indice en vector, simbolo) -> (id del estado destino)
  std::vector<std::map<char, IDstate>> transitions;
  std::set<IDstate> final_states;
  std::set<char> alphabet;

public:
  IDstate add_state(bool is_final)
  {
    transitions.push_back({});
    if (is_final)
      final_states.insert(transitions.size() - 1);
    return transitions.size() - 1;
  }

  void add_transition(IDstate from, char symbol, IDstate to)
  {
    transitions[from][symbol] = to;
  }

  IDstate get_initial_state() const
  {
    return 0; 
  }

  const std::map<char, IDstate> &get_transitions(IDstate state) const
  {
    return transitions[state];
  }
};
