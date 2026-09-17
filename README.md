# Práctica 2 - Compiladores

Conversión de un autómata finito no determinista (NFA) a uno determinista (DFA)
mediante la construcción de subconjuntos, con visualización del **antes y
después** de la transformación.

El programa de C++ construye los autómatas, ejecuta la conversión y emite una
cadena con el código fuente de un documento [Typst](https://typst.app) que usa el
paquete [`finite`](https://typst.app/universe/package/finite) para dibujar los
autómatas. Después invoca `typst compile` desde la consola para producir el PDF.

Typst no hace falta instalarlo: el `Dockerfile` arma una imagen que sólo lo
contiene y `typst-docker.sh` lo ejecuta ahí dentro. El C++ se compila y corre de
forma nativa.

```
NFA (C++)  ──nfa_to_dfa──▶  DFA (C++)
    │                          │
    └────── to_string ─────────┘      especificación del paquete `finite`
                 │
          typst_doc: documento .typ
                 │
          system("typst compile ...")
                 │
             salida/nfa_to_dfa.pdf
```

Cada ejemplo ocupa una página con:

- el diagrama del NFA original y el del DFA resultante,
- las tablas de transiciones de ambos,
- la correspondencia entre cada estado del DFA y el subconjunto de estados del
  NFA que lo originó.

## Ejecución

Sólo hace falta `g++` con C++17 y Docker.

```bash
cd nfa_to_dfa
make run                      # equivale a: make && ./build/nfa_to_dfa_exec
```

La primera vez se construye la imagen con Typst (~210 MB); a partir de ahí se
reutiliza. El resultado queda en `salida/nfa_to_dfa.pdf`.

O con CMake, si se prefiere:

```bash
cd nfa_to_dfa
cmake -B build && cmake --build build
./build/nfa_to_dfa_exec
```

### Cómo se elige el Typst a usar

El programa resuelve la orden en este orden:

1. la variable de entorno `TYPST_CMD`, si está definida;
2. el ejecutable `typst` del `PATH`, si está instalado;
3. `./typst-docker.sh`, que lo corre dentro del contenedor.

Imprime cuál eligió antes de compilar. El envoltorio también sirve a mano, igual
que el ejecutable real:

```bash
./typst-docker.sh compile salida/nfa_to_dfa.typ salida/nfa_to_dfa.pdf
./typst-docker.sh --version
```

Monta el directorio actual en `/work`, así que las rutas que reciba deben ser
relativas a él.

### Opciones

```
-o, --out DIR      Directorio de salida (por omisión: salida)
-f, --format FMT   pdf, png o svg (por omisión: pdf)
-n, --no-compile   Sólo genera el archivo .typ, sin invocar a typst
-h, --help         Muestra la ayuda
```

Con `--no-compile` se obtiene `salida/nfa_to_dfa.typ`, que puede abrirse en
<https://typst.app> o compilarse a mano con `typst compile salida/nfa_to_dfa.typ`.

## Estructura

| Archivo | Contenido |
| --- | --- |
| `src/automatas.hpp` | Modelos `NFA` y `DFA`. |
| `src/convert.*` | `move`, `eClosure` y la construcción de subconjuntos. |
| `src/to_string.*` | Serialización de los autómatas al formato del paquete `finite`. |
| `src/typst_doc.*` | Armado del documento Typst y llamada a `typst compile`. |
| `src/main.cpp` | Autómatas de ejemplo y línea de comandos. |
| `Dockerfile` | Imagen con Typst y el paquete `finite` precargado. |
| `typst-docker.sh` | Envoltorio que ejecuta `typst` dentro del contenedor. |
