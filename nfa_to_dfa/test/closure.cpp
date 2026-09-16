/**
 * @file closure.cpp
 * @brief Pruebas unitarias del algoritmo de cierre epsilon.
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

/**
 * @brief Comprueba que eClosure incluya el origen y termine ante un ciclo epsilon.
 */
void test_cycle()
{
  const NFA nfa(
      {
          make_state({}, {1}),
          make_state({}, {2}),
          make_state({}, {1}),
          make_state(),
      },
      0,
      {});

  expect_states(eClosure({0}, nfa), {0, 1, 2}, "El cierre epsilon ciclico es incorrecto");
}

/**
 * @brief Comprueba que el cierre epsilon de un conjunto vacio permanezca vacio.
 */
void test_empty()
{
  const NFA nfa({make_state({}, {1}), make_state()}, 0, {});
  expect_states(eClosure({}, nfa), {}, "El cierre de un conjunto vacio debe ser vacio");
}

/**
 * @brief Comprueba que un estado sin transiciones epsilon se incluya solamente a si mismo.
 */
void test_isolated()
{
  const NFA nfa({make_state(), make_state()}, 0, {});
  expect_states(eClosure({1}, nfa), {1}, "El estado aislado debe conservarse como unico resultado");
}

/**
 * @brief Comprueba la union transitiva de varias raices epsilon con destinos convergentes.
 */
void test_convergent()
{
  const NFA nfa(
      {
          make_state({}, {2}),
          make_state({}, {2, 4}),
          make_state({}, {3}),
          make_state(),
          make_state({}, {3}),
      },
      0,
      {});

  expect_states(
      eClosure({0, 1}, nfa),
      {0, 1, 2, 3, 4},
      "El cierre debe reunir las ramas epsilon convergentes");
}

/**
 * @brief Comprueba que eClosure ignore transiciones que consumen simbolos del alfabeto.
 */
void test_labeled()
{
  const NFA nfa(
      {
          make_state({{'a', {2}}}, {1}),
          make_state({{'b', {2}}}),
          make_state(),
      },
      0,
      {'a', 'b'});

  expect_states(eClosure({0}, nfa), {0, 1}, "El cierre epsilon no debe consumir a ni b");
}

const std::vector<TestCase> &test_cases()
{
  static const std::vector<TestCase> cases = {
      {"cycle", "eClosure: ciclo epsilon", test_cycle},
      {"empty", "eClosure: conjunto vacio", test_empty},
      {"isolated", "eClosure: estado aislado", test_isolated},
      {"convergent", "eClosure: raices convergentes", test_convergent},
      {"labeled", "eClosure: ignora simbolos", test_labeled},
  };
  return cases;
}

} // namespace

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cerr << "Uso: closuretests <caso>\n";
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

  std::cerr << "Caso de eClosure desconocido: " << argv[1] << '\n';
  return 2;
}
