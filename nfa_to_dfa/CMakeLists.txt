cmake_minimum_required(VERSION 3.14)
project(nfa_to_dfa CXX)

# Usar estándar C++17 o posterior
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluir la carpeta src para buscar headers
include_directories(src)

# Definir los archivos fuente
set(SOURCES
    src/main.cpp
    src/to_string.cpp
)

# Crear el ejecutable llamado 'nfa_to_dfa_exec'
add_executable(nfa_to_dfa_exec ${SOURCES})