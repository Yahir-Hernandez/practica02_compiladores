# Sistema base: Debian Bookworm Slim
FROM debian:bookworm-slim

# Instalar dependencias y descargar Typst
RUN apt-get update && apt-get install -y \
    curl xz-utils build-essential g++ cmake \
    && curl -fsSL https://github.com/typst/typst/releases/latest/download/typst-x86_64-unknown-linux-musl.tar.xz -o typst.tar.xz \
    && tar -xf typst.tar.xz \
    && mv typst-x86_64-unknown-linux-musl/typst /usr/local/bin/ \
    && rm -rf typst.tar.xz typst-x86_64-unknown-linux-musl \
    && apt-get purge -y curl xz-utils && rm -rf /var/lib/apt/lists/*

# Pre-descargar finite y sus dependencias (cetz, t4t, oxifmt, diagraph-layout),
# que son los paquetes con los que se dibujan los automatas. 
ENV XDG_CACHE_HOME=/opt/cache
RUN mkdir -p /tmp/warmup && cd /tmp/warmup \
    && printf '%s\n' \
        '#import "@preview/finite:0.5.1": automaton, layout' \
        '#automaton((q0: (q1: "a"), q1: (:)), layout: layout.diagraph)' \
        > warmup.typ \
    && typst compile warmup.typ warmup.svg --format svg \
    && rm -rf /tmp/warmup \
    && chmod -R a+rwX /opt/cache

WORKDIR /app

# Copiar el código fuente
COPY . .

# Configurar y compilar el programa junto con la suite de pruebas.
RUN cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTING=ON \
    -DNFA_DFA_ENABLE_SANITIZERS=OFF \
    && cmake --build build --parallel

# `make test` usa esta orden; `make run` la sustituye por el programa.
CMD ["ctest", "--test-dir", "build", "--output-on-failure"]
