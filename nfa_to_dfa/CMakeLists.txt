cmake_minimum_required(VERSION 3.14)
project(nfa_to_dfa CXX)

# El proyecto requiere C++17 por las operaciones de la biblioteca estandar.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(
    NFA_DFA_ENABLE_SANITIZERS
    "Compila los ejecutables con AddressSanitizer y UndefinedBehaviorSanitizer"
    OFF
)

if(NFA_DFA_ENABLE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    else()
        message(WARNING "Los sanitizadores solo estan configurados para GCC y Clang")
    endif()
endif()

# Biblioteca reutilizable con los algoritmos de conversion.
add_library(nfa_to_dfa_core STATIC
    src/convert.cpp
)
target_include_directories(nfa_to_dfa_core PUBLIC src)
target_compile_options(nfa_to_dfa_core PRIVATE -Wall -Wextra -Wpedantic)

# Genera el ejecutable que imprime los NFA y DFA en formato Typst.
add_executable(nfa_to_dfa_exec
    src/main.cpp
    src/to_string.cpp
    src/typst_doc.cpp
)
target_link_libraries(nfa_to_dfa_exec PRIVATE nfa_to_dfa_core)
target_compile_options(nfa_to_dfa_exec PRIVATE -Wall -Wextra -Wpedantic)

include(CTest)

if(BUILD_TESTING)
    add_executable(closuretests test/closure.cpp)
    add_executable(movetests test/move.cpp)
    add_executable(subsettests test/subset.cpp)

    foreach(test_target closuretests movetests subsettests)
        target_link_libraries(${test_target} PRIVATE nfa_to_dfa_core)
        target_compile_options(${test_target} PRIVATE -Wall -Wextra -Wpedantic)
    endforeach()

    function(register_algorithm_test test_name test_target case_name)
        add_test(NAME ${test_name} COMMAND ${test_target} ${case_name})
        set_tests_properties(${test_name} PROPERTIES TIMEOUT 5)
    endfunction()

    register_algorithm_test(closure.cycle closuretests cycle)
    register_algorithm_test(closure.empty closuretests empty)
    register_algorithm_test(closure.isolated closuretests isolated)
    register_algorithm_test(closure.convergent closuretests convergent)
    register_algorithm_test(closure.labeled closuretests labeled)

    register_algorithm_test(move.symbol movetests symbol)
    register_algorithm_test(move.multisource movetests multisource)
    register_algorithm_test(move.empty movetests empty)
    register_algorithm_test(move.missing movetests missing)
    register_algorithm_test(move.epsilon movetests epsilon)

    register_algorithm_test(subset.structure subsettests structure)
    register_algorithm_test(subset.equivalence subsettests equivalence)
    register_algorithm_test(subset.initialfinal subsettests initialfinal)
    register_algorithm_test(subset.emptyalphabet subsettests emptyalphabet)
    register_algorithm_test(subset.unreachable subsettests unreachable)
    register_algorithm_test(subset.cycles subsettests cycles)
    register_algorithm_test(subset.partial subsettests partial)
    register_algorithm_test(subset.postclosure subsettests postclosure)
endif()
