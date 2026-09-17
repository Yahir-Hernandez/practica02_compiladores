# Alternativa a CMake para compilar el proyecto con un solo comando.
CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Isrc

SOURCES := src/main.cpp src/to_string.cpp src/convert.cpp src/typst_doc.cpp
HEADERS := $(wildcard src/*.hpp)
BIN     := build/nfa_to_dfa_exec

.PHONY: all run test clean

all: $(BIN)

$(BIN): $(SOURCES) $(HEADERS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@

## Genera el documento y lo compila con typst (requiere typst en el PATH).
run: $(BIN)
	./$(BIN)

## Corre la suite de pruebas (CTest con sanitizadores) dentro del contenedor.
test:
	docker build -t nfa-to-dfa .
	docker run --rm nfa-to-dfa

clean:
	rm -rf build salida
