#include <iostream>
#include <string>
#include "automatas.hpp"
#include "to_string.hpp"

using namespace std;

int main() {
  NFA nfa(
    {
      { {{'a', {1}} }, {}, true },
      { {{'a', {2}},{'b', {0,2}} }, {}, false},
      { {{'a', {3}} }, {}, false},
      { {{'a', {2}} }, {0}, false},
    },
    0,
    {'a', 'b'}
  );

  cout << nfa_to_string(nfa) << endl;
}