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

# Configurar y compilar el programa junto con la suite de pruebas.
RUN cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTING=ON \
    -DNFA_DFA_ENABLE_SANITIZERS=ON \
    && cmake --build build --parallel

# LeakSanitizer se desactiva por compatibilidad con contenedores restringidos.
ENV ASAN_OPTIONS=detect_leaks=0:halt_on_error=1
ENV UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1

# Ejecutar la suite registrada en CTest al iniciar el contenedor.
CMD ["ctest", "--test-dir", "build", "--output-on-failure", "-V"]
