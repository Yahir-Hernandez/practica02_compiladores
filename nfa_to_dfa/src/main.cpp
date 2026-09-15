/**
 * @file main.cpp
 * @brief Programa de demostracion de la conversion NFA-DFA.
 */
#include <iostream>
#include <string>
#include "automatas.hpp"
#include "to_string.hpp"
#include "convert.hpp"

using namespace std;

int main()
{
  cout << "==== NFA 1: =====" << endl;
  NFA nfa1(
      {
          {{{'a', {1}}, {'b', {0}}}, {}, false},
          {{{'a', {1}}, {'b', {2}}}, {}, false},
          {{{'a', {2}}, {'b', {2}}}, {}, true},
      },
      0,
      {'a', 'b'});
  cout << "NFA: " << endl;
  cout << nfa_to_string(nfa1) << endl;
  DFA dfa1 = nfa_to_dfa(nfa1);
  cout << "DFA: " << endl;
  cout << dfa_to_string(dfa1) << endl;
  cout << "==== NFA 2: =====" << endl;
  NFA nfa2(
      {
          {{{'a', {0}}, {'b', {0}}}, {2}, false},
          {{{'c', {2}}}, {}, true},
          {{}, {}, true},
      },
      0,
      {'a', 'b', 'c'});
  cout << "NFA: " << endl;
  cout << nfa_to_string(nfa2) << endl;
  DFA dfa2 = nfa_to_dfa(nfa2);
  cout << "DFA: " << endl;
  cout << dfa_to_string(dfa2) << endl;
  cout << "==== NFA 3: =====" << endl;
  NFA nfa3(
      {
          {{{'a', {1}}}, {}, true},
          {{{'a', {2}}, {'b', {0, 2}}}, {}, false},
          {{{'a', {3}}}, {}, false},
          {{{'a', {2}}}, {0}, false},
      },
      0,
      {'a', 'b'});
  cout << "NFA: " << endl;
  cout << nfa_to_string(nfa3) << endl;
  DFA dfa3 = nfa_to_dfa(nfa3);
  cout << "DFA: " << endl;
  cout << dfa_to_string(dfa3) << endl;
  cout << "==== NFA 4: =====" << endl;
  NFA nfa4(
      {
          {{{'a', {0, 3}}}, {1}, false},
          {{{'b', {2}}}, {2}, false},
          {{}, {}, true},
          {{{'a', {3}}}, {}, false},
      },
      0,
      {'a', 'b'});
  cout << "NFA: " << endl;
  cout << nfa_to_string(nfa4) << endl;
  DFA dfa4 = nfa_to_dfa(nfa4);
  cout << "DFA: " << endl;
  cout << dfa_to_string(dfa4) << endl;
  return 0;
}