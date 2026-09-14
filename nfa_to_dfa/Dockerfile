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

# Pre-descargar el paquete fletcher (librería de Typst)
ENV XDG_CACHE_HOME=/root/.cache
RUN mkdir -p /tmp/warmup && cd /tmp/warmup && \
    echo '#import "@preview/fletcher:0.5.7" as fletcher' > warmup.typ && \
    echo '#fletcher.diagram()' >> warmup.typ && \
    typst compile warmup.typ warmup.svg --format svg || true && \
    rm -rf /tmp/warmup

WORKDIR /app

# Copiar el código fuente
COPY . .

# Compilar con CMake en contenedor
RUN mkdir -p build && cd build && cmake .. && make

# Comando por defecto para ejecutar tu programa
CMD ["./build/nfa_to_dfa_exec"]