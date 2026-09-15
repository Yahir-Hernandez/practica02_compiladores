cmake_minimum_required(VERSION 3.14)
project(nfa_to_dfa CXX)

# El proyecto requiere C++17 por las operaciones de la biblioteca estandar.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Permite incluir los encabezados del directorio fuente.
include_directories(src)

# Fuentes del ejecutable de demostracion.
set(SOURCES
    src/main.cpp
    src/to_string.cpp
    src/convert.cpp
)

# Genera el ejecutable que imprime los NFA y DFA en formato Typst.
add_executable(nfa_to_dfa_exec ${SOURCES})