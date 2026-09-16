/**
 * @file move.cpp
 * @brief Pruebas unitarias del algoritmo move.
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
 * @brief Comprueba que move recupere todos los destinos del simbolo solicitado y excluya los demas.
 */
void test_symbol()
{
  const NFA nfa(
      {
          make_state({{'a', {1, 2}}, {'b', {3}}}),
          make_state(),
          make_state(),
          make_state(),
      },
      0,
      {'a', 'b'});

  expect_states(move({0}, 'a', nfa), {1, 2}, "Move({0}, 'a') debe devolver 1 y 2");
}

/**
 * @brief Comprueba que move una varios origenes sin repetir destinos compartidos.
 */
void test_multisource()
{
  const NFA nfa(
      {
          make_state({{'a', {2, 3}}}),
          make_state({{'a', {2, 4}}}),
          make_state(),
          make_state(),
          make_state(),
      },
      0,
      {'a'});

  expect_states(move({0, 1}, 'a', nfa), {2, 3, 4}, "Move debe unir y deduplicar destinos");
}

/**
 * @brief Comprueba que move no produzca destinos cuando el conjunto origen esta vacio.
 */
void test_empty()
{
  const NFA nfa({make_state({{'a', {1}}}), make_state()}, 0, {'a'});
  expect_states(move({}, 'a', nfa), {}, "Move del conjunto vacio debe ser vacio");
}

/**
 * @brief Comprueba que move devuelva vacio cuando ningun origen reconoce el simbolo indicado.
 */
void test_missing()
{
  const NFA nfa(
      {
          make_state({{'a', {1}}}),
          make_state({{'b', {0}}}),
      },
      0,
      {'a', 'b', 'c'});

  expect_states(move({0, 1}, 'c', nfa), {}, "El simbolo c no debe producir destinos");
}

/**
 * @brief Comprueba que move no aplique cierres epsilon antes ni despues de consumir el simbolo.
 */
void test_epsilon()
{
  const NFA nfa(
      {
          make_state({{'a', {1}}}, {3}),
          make_state({}, {2}),
          make_state(),
          make_state({{'a', {4}}}),
          make_state(),
      },
      0,
      {'a'});

  expect_states(move({0}, 'a', nfa), {1}, "Move debe limitarse a la transicion directa con a");
}

const std::vector<TestCase> &test_cases()
{
  static const std::vector<TestCase> cases = {
      {"symbol", "Move: simbolo solicitado", test_symbol},
      {"multisource", "Move: multiples origenes", test_multisource},
      {"empty", "Move: origen vacio", test_empty},
      {"missing", "Move: simbolo ausente", test_missing},
      {"epsilon", "Move: no recorre epsilon", test_epsilon},
  };
  return cases;
}

} // namespace

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cerr << "Uso: movetests <caso>\n";
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

  std::cerr << "Caso de move desconocido: " << argv[1] << '\n';
  return 2;
}
