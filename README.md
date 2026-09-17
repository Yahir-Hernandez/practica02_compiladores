# Práctica 2 - Compiladores

Este repositorio implementa en C++ las algoritmos de move, ε-closure y el algoritmo de construcción de subconjuntos para autómatas finitos no deterministas (NFA).

Los algoritmos se prueban mediante la conversión de 4 NFA en sus DFA equivalentes. El antes y el después de cada conversión puede visualizarse en un archivo PDF generado con Typst.

## Requisitos

Para compilar la práctica utilizamos CMake dentro de Docker, por lo que solo necesitas tener instalados **Make y Docker**. Docker debe estar en ejecución y disponible para tu usuario.

## Uso

Desde el directorio `nfa_to_dfa/`, puedes ejecutar:

```bash
make run
```
Este comando transforma los cuatro NFA de ejemplo y genera en la carpeta `salida/` un PDF con la visualización de cada NFA original y su DFA equivalente.

```bash
make test
```
Para ejecutar las 18 pruebas que validan la implementación.

```bash
make clean
```
Para eliminar la carpeta `salida/` y los archivos generados.

También implementamos una GitHub Action que ejecuta `make test` con cada nuevo *push* al repositorio. Los resultados de los tests automáticos pueden consultarse en el apartado **Actions >> Tests**.
