/**
 * @file automatas.hpp
 * @brief Modelos de automatas finitos no deterministas y deterministas.
 */
#pragma once
#include <map>
#include <set>
#include <vector>


/** @brief Identificador de estado, equivalente al indice dentro del vector. */
using IDstate = int;

/**
 * @brief Datos de un estado NFA.
 *
 * Un NFA puede tener varios destinos para un mismo simbolo. Las transiciones
 * epsilon no consumen entrada y se almacenan en un conjunto independiente.
 */
struct stateNFA
{

  std::map<char, std::set<IDstate>> transitions;

  std::set<IDstate> epsilon;

  bool is_final;
};

/**
 * @class NFA
 * @brief Representacion de un automata finito no determinista.
 */
class NFA
{
private:
  std::vector<stateNFA> states;
  IDstate initial_state;
  std::set<char> alphabet;

public:
  /**
   * @brief Inicializa el NFA con sus estados, estado inicial y alfabeto.
   * @param st Estados iniciales del automata.
   * @param init_st Identificador del estado inicial.
   * @param alph Simbolos que pueden consumirse.
   */
  NFA(std::vector<stateNFA> st, IDstate init_st, std::set<char> alph)
      : states(st), initial_state(init_st), alphabet(alph) {}
  /** @return Identificador del estado inicial. */
  IDstate get_initial_state() const
  {
    return initial_state;
  }
  void set_initial_state(IDstate state)
  {
    initial_state = state;
  }

  /**
   * @brief Devuelve los datos de un estado.
   * @param state Identificador valido del estado consultado.
   * @return Referencia constante a los datos del estado.
   */
  const stateNFA &get_state(IDstate state) const
  {
    return states[state];
  }

  /**
   * @brief Agrega un estado.
   * @param is_final Indica si el estado acepta cadenas.
   * @return Identificador asignado al nuevo estado.
   */
  IDstate add_state(bool is_final)
  {
    states.push_back(stateNFA{{}, {}, is_final});
    return states.size() - 1;
  }

  /**
   * @brief Agrega una transicion etiquetada.
   * @param from Estado origen.
   * @param symbol Simbolo consumido.
   * @param to Estado destino.
   */
  void add_transition(IDstate from, char symbol, IDstate to)
  {
    states[from].transitions[symbol].insert(to);
    alphabet.insert(symbol);
  }

  /**
   * @brief Agrega una transicion epsilon.
   * @param from Estado origen.
   * @param to Estado destino.
   */
  void add_epsilon_transition(IDstate from, IDstate to)
  {
    states[from].epsilon.insert(to);
  }

  /**
   * @brief Consulta la funcion de transicion del NFA.
   * @param state Estado origen.
   * @param symbol Simbolo consumido.
   * @return Conjunto de estados destino, posiblemente vacio.
   */
  std::set<IDstate> delta(IDstate state, char symbol) const
  {
    auto it = states[state].transitions.find(symbol);
    if (it != states[state].transitions.end())
    {
      return it->second;
    }
    return {};
  }

  /** @return Alfabeto del NFA. */
  const std::set<char> &get_alphabet() const
  {
    return alphabet;
  }

  /** Calcula y devuelve el conjunto de estados finales del NFA. */
  std::set<IDstate> get_final_states() const
  {
    std::set<IDstate> final_states;
    for (size_t i = 0; i < states.size(); ++i)
    {
      if (states[i].is_final)
      {
        final_states.insert(i);
      }
    }
    return final_states;
  }
  /** @return Cantidad total de estados del NFA. */
  size_t get_num_states() const { return states.size(); }
};

/**
 * @class DFA
 * @brief Representacion de un automata finito determinista.
 */
class DFA
{
private:
  std::vector<std::map<char, IDstate>> transitions;
  std::set<IDstate> final_states;
  std::set<char> alphabet;

public:
  /** @return Identificador del nuevo estado agregado. */
  IDstate add_state()
  {
    transitions.push_back({});
    return transitions.size() - 1;
  }

  /**
   * @brief Agrega o reemplaza una transicion determinista.
   * @param from Estado origen.
   * @param symbol Simbolo consumido.
   * @param to Estado destino unico.
   */
  void add_transition(IDstate from, char symbol, IDstate to)
  {
    transitions[from][symbol] = to;
  }

  /** @return Estado inicial, que la conversion crea con el identificador cero. */
  IDstate get_initial_state() const
  {
    return 0; 
  }

  /** @return Conjunto de identificadores de estados finales. */
  const std::set<IDstate> &get_final_states() const
  {
    return final_states;
  }

  /** @return Alfabeto del DFA. */
  const std::set<char> &get_alphabet() const
  {
    return alphabet;
  }

  /** @param state Estado que se marcara como final. */
  void add_final_state(IDstate state)
  {
    final_states.insert(state);
  }

  /** @param alph Alfabeto que se copiara al DFA. */
  void set_alphabet(const std::set<char> &alph)
  {
    alphabet = alph;
  }

  /**
   * @brief Devuelve las transiciones que salen de un estado.
   * @param state Estado consultado.
   * @return Mapa de simbolos a estados destino.
   */
  const std::map<char, IDstate> &get_transition(IDstate state) const
  {
    return transitions[state];
  }

  /** @return Cantidad total de estados del DFA. */
  size_t get_num_states() const { return transitions.size(); }
};
