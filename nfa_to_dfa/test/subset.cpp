/**
 * @file subset.cpp
 * @brief Pruebas de la construccion de subconjuntos NFA-DFA.
 */
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "convert.hpp"

namespace
{

class TestFailure : public std::runtime_error
{
public:
  explicit TestFailure(const std::string &message) : std::runtime_error(message) {}
};

using TestFunction = void (*)();

struct TestCase
{
  std::string id;
  std::string name;
  TestFunction function;
};

void expect(bool condition, const std::string &message)
{
  if (!condition)
  {
    throw TestFailure(message);
  }
}

std::string states_to_string(const std::set<IDstate> &states)
{
  std::string output = "{";
  bool first = true;
  for (IDstate state : states)
  {
    if (!first)
    {
      output += ", ";
    }
    output += std::to_string(state);
    first = false;
  }
  return output + "}";
}

void expect_states(
    const std::set<IDstate> &actual,
    const std::set<IDstate> &expected,
    const std::string &message)
{
  if (actual != expected)
  {
    throw TestFailure(
        message + ": se obtuvo " + states_to_string(actual) +
        " y se esperaba " + states_to_string(expected));
  }
}

stateNFA make_state(
    std::map<char, std::set<IDstate>> transitions = {},
    std::set<IDstate> epsilon = {},
    bool is_final = false)
{
  return stateNFA{std::move(transitions), std::move(epsilon), is_final};
}

NFA make_reference_nfa()
{
  return NFA(
      {
          make_state({}, {1, 2}),
          make_state({{'a', {1}}, {'b', {3}}}),
          make_state({{'a', {2}}, {'b', {3}}}),
          make_state({}, {}, true),
      },
      0,
      {'a', 'b'});
}

void expect_transition(
    const DFA &dfa,
    IDstate from,
    char symbol,
    IDstate expected_destination)
{
  const auto &transitions = dfa.get_transition(from);
  const auto transition = transitions.find(symbol);
  expect(
      transition != transitions.end(),
      "Falta la transicion desde " + std::to_string(from) + " con " + symbol);
  expect(
      transition->second == expected_destination,
      "Destino incorrecto desde " + std::to_string(from) + " con " + symbol);
}

void expect_no_transition(const DFA &dfa, IDstate from, char symbol)
{
  expect(
      dfa.get_transition(from).count(symbol) == 0,
      "No debe existir una transicion desde " + std::to_string(from) + " con " + symbol);
}

std::set<IDstate> independent_epsilon_closure(
    const NFA &nfa,
    const std::set<IDstate> &initial_states)
{
  std::set<IDstate> closure = initial_states;
  std::vector<IDstate> pending(initial_states.begin(), initial_states.end());

  while (!pending.empty())
  {
    const IDstate state = pending.back();
    pending.pop_back();
    for (IDstate destination : nfa.get_state(state).epsilon)
    {
      if (closure.insert(destination).second)
      {
        pending.push_back(destination);
      }
    }
  }

  return closure;
}

bool nfa_accepts(const NFA &nfa, const std::string &word)
{
  std::set<IDstate> current = independent_epsilon_closure(
      nfa, {nfa.get_initial_state()});

  for (char symbol : word)
  {
    std::set<IDstate> destinations;
    for (IDstate state : current)
    {
      const std::set<IDstate> next = nfa.delta(state, symbol);
      destinations.insert(next.begin(), next.end());
    }
    current = independent_epsilon_closure(nfa, destinations);
  }

  const std::set<IDstate> final_states = nfa.get_final_states();
  for (IDstate state : current)
  {
    if (final_states.count(state) != 0)
    {
      return true;
    }
  }
  return false;
}

bool dfa_accepts(const DFA &dfa, const std::string &word)
{
  IDstate current = dfa.get_initial_state();
  for (char symbol : word)
  {
    const auto &transitions = dfa.get_transition(current);
    const auto transition = transitions.find(symbol);
    if (transition == transitions.end())
    {
      return false;
    }
    current = transition->second;
  }
  return dfa.get_final_states().count(current) != 0;
}

void append_words(
    std::vector<std::string> &words,
    const std::vector<char> &alphabet,
    const std::string &prefix,
    std::size_t remaining_length)
{
  words.push_back(prefix);
  if (remaining_length == 0)
  {
    return;
  }

  for (char symbol : alphabet)
  {
    append_words(words, alphabet, prefix + symbol, remaining_length - 1);
  }
}

std::vector<std::string> words_up_to(
    const std::set<char> &alphabet,
    std::size_t maximum_length)
{
  std::vector<std::string> words;
  append_words(
      words,
      std::vector<char>(alphabet.begin(), alphabet.end()),
      "",
      maximum_length);
  return words;
}

/**
 * @brief Comprueba estados, alfabeto, finales y transiciones exactas del DFA resultante.
 */
void test_structure()
{
  const NFA nfa = make_reference_nfa();
  const DFA dfa = nfa_to_dfa(nfa);

  expect(dfa.get_num_states() == 3, "La conversion debe producir tres estados DFA");
  expect(dfa.get_initial_state() == 0, "El estado inicial DFA debe ser cero");
  expect(dfa.get_alphabet() == std::set<char>({'a', 'b'}), "El alfabeto debe conservarse");
  expect_states(dfa.get_final_states(), {2}, "Solo el tercer estado DFA debe ser final");
  expect(dfa.get_transition(0).size() == 2, "El estado cero debe tener dos simbolos distintos");
  expect_transition(dfa, 0, 'a', 1);
  expect_transition(dfa, 0, 'b', 2);
  expect(dfa.get_transition(1).size() == 2, "El estado uno debe tener dos simbolos distintos");
  expect_transition(dfa, 1, 'a', 1);
  expect_transition(dfa, 1, 'b', 2);
  expect(dfa.get_transition(2).empty(), "El estado final no debe inventar transiciones");
}

/**
 * @brief Comprueba equivalencia de aceptacion para todas las palabras de longitud cero a cuatro.
 */
void test_equivalence()
{
  const NFA nfa = make_reference_nfa();
  const DFA dfa = nfa_to_dfa(nfa);
  const std::vector<std::string> words = words_up_to(nfa.get_alphabet(), 4);

  expect(words.size() == 31, "Deben generarse las 31 palabras binarias esperadas");
  for (const std::string &word : words)
  {
    const std::string shown_word = word.empty() ? "epsilon" : word;
    expect(
        nfa_accepts(nfa, word) == dfa_accepts(dfa, word),
        "El NFA y el DFA discrepan para la palabra " + shown_word);
  }
}

/**
 * @brief Comprueba que el cierre inicial vuelva final al primer estado DFA cuando contiene aceptacion.
 */
void test_initialfinal()
{
  const NFA nfa(
      {
          make_state({}, {1}),
          make_state({}, {}, true),
      },
      0,
      {});
  const DFA dfa = nfa_to_dfa(nfa);

  expect(dfa.get_num_states() == 1, "El cierre inicial debe formar un solo estado");
  expect_states(dfa.get_final_states(), {0}, "El estado inicial DFA debe ser final");
  expect(dfa_accepts(dfa, ""), "El DFA debe aceptar la palabra vacia");
}

/**
 * @brief Comprueba que un NFA con alfabeto vacio produzca solamente su cierre inicial.
 */
void test_emptyalphabet()
{
  const NFA nfa(
      {
          make_state({}, {}, true),
          make_state(),
      },
      0,
      {});
  const DFA dfa = nfa_to_dfa(nfa);

  expect(dfa.get_num_states() == 1, "El alfabeto vacio debe producir un solo estado DFA");
  expect(dfa.get_alphabet().empty(), "El DFA debe conservar el alfabeto vacio");
  expect(dfa.get_transition(0).empty(), "No deben existir transiciones sin alfabeto");
  expect_states(dfa.get_final_states(), {0}, "El estado inicial final debe conservarse");
}

/**
 * @brief Comprueba que los estados NFA inalcanzables no creen estados ni finales en el DFA.
 */
void test_unreachable()
{
  const NFA nfa(
      {
          make_state({{'a', {0}}}),
          make_state({}, {}, true),
          make_state({{'a', {1}}}),
      },
      0,
      {'a'});
  const DFA dfa = nfa_to_dfa(nfa);

  expect(dfa.get_num_states() == 1, "Solo el estado NFA alcanzable debe aparecer en el DFA");
  expect(dfa.get_final_states().empty(), "El final inalcanzable no debe propagarse");
  expect_transition(dfa, 0, 'a', 0);
}

/**
 * @brief Comprueba que los ciclos reutilicen un subconjunto existente y no generen estados infinitos.
 */
void test_cycles()
{
  const NFA nfa(
      {
          make_state({}, {1}),
          make_state({{'a', {0, 1}}}, {}, true),
      },
      0,
      {'a'});
  const DFA dfa = nfa_to_dfa(nfa);

  expect(dfa.get_num_states() == 1, "El ciclo debe reutilizar el subconjunto inicial");
  expect_transition(dfa, 0, 'a', 0);
  expect_states(dfa.get_final_states(), {0}, "El subconjunto ciclico debe conservar su estado final");
}

/**
 * @brief Comprueba la semantica parcial actual sin crear un estado sumidero para destinos vacios.
 */
void test_partial()
{
  const NFA nfa(
      {
          make_state({{'a', {1}}}),
          make_state({}, {}, true),
      },
      0,
      {'a', 'b'});
  const DFA dfa = nfa_to_dfa(nfa);

  expect(dfa.get_num_states() == 2, "No debe agregarse un tercer estado sumidero");
  expect_transition(dfa, 0, 'a', 1);
  expect_no_transition(dfa, 0, 'b');
  expect(dfa.get_transition(1).empty(), "El estado final debe conservar transiciones parciales");
  expect(!dfa_accepts(dfa, "b"), "Una transicion ausente debe interpretarse como rechazo");
}

/**
 * @brief Comprueba el cierre epsilon posterior a move y la aceptacion heredada por su destino.
 */
void test_postclosure()
{
  const NFA nfa(
      {
          make_state({{'a', {1}}}),
          make_state({}, {2}),
          make_state({}, {}, true),
      },
      0,
      {'a'});
  const DFA dfa = nfa_to_dfa(nfa);

  expect(dfa.get_num_states() == 2, "La transicion a debe crear un unico subconjunto destino");
  expect_transition(dfa, 0, 'a', 1);
  expect_states(dfa.get_final_states(), {1}, "El destino debe ser final por su cierre epsilon");
  expect(!dfa_accepts(dfa, ""), "La palabra vacia no debe aceptarse");
  expect(dfa_accepts(dfa, "a"), "La palabra a debe aceptarse despues del cierre epsilon");
}

const std::vector<TestCase> &test_cases()
{
  static const std::vector<TestCase> cases = {
      {"structure", "Subconjuntos: estructura exacta", test_structure},
      {"equivalence", "Subconjuntos: equivalencia acotada", test_equivalence},
      {"initialfinal", "Subconjuntos: inicial final por epsilon", test_initialfinal},
      {"emptyalphabet", "Subconjuntos: alfabeto vacio", test_emptyalphabet},
      {"unreachable", "Subconjuntos: estados inalcanzables", test_unreachable},
      {"cycles", "Subconjuntos: ciclos finitos", test_cycles},
      {"partial", "Subconjuntos: DFA parcial", test_partial},
      {"postclosure", "Subconjuntos: cierre posterior a move", test_postclosure},
  };
  return cases;
}

} // namespace

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cerr << "Uso: subsettests <caso>\n";
    return 2;
  }

  for (const TestCase &test_case : test_cases())
  {
    if (test_case.id != argv[1])
    {
      continue;
    }

    try
    {
      test_case.function();
      std::cout << "[PASS] " << test_case.name << '\n';
      return 0;
    }
    catch (const std::exception &error)
    {
      std::cerr << "[FAIL] " << test_case.name << ": " << error.what() << '\n';
      return 1;
    }
  }

  std::cerr << "Caso de subconjuntos desconocido: " << argv[1] << '\n';
  return 2;
}
