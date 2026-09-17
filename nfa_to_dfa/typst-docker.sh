#!/usr/bin/env bash
# Envoltorio que ejecuta `typst` dentro del contenedor del proyecto, para no
# tener que instalarlo en el equipo. Se usa igual que el ejecutable typst:
#
#   ./typst-docker.sh compile salida/nfa_to_dfa.typ salida/nfa_to_dfa.pdf
#
# El programa de C++ lo invoca automaticamente cuando no encuentra typst en el
# PATH. La imagen se construye sola la primera vez.
#
# Es la misma imagen que corre la suite de pruebas (ver Dockerfile): aqui se
# sobreescribe su CMD para llamar a typst en lugar de a ctest.
#
# El directorio actual se monta en /work, asi que las rutas deben ser relativas
# a el.
set -euo pipefail

IMAGE=nfa-to-dfa
DOCKERFILE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if ! docker image inspect "$IMAGE" > /dev/null 2>&1; then
  echo "Construyendo la imagen $IMAGE (solo la primera vez)..." >&2
  docker build -t "$IMAGE" "$DOCKERFILE_DIR" >&2
fi

exec docker run --rm \
  --user "$(id -u):$(id -g)" \
  --network none \
  --volume "$PWD:/work" \
  --workdir /work \
  "$IMAGE" typst "$@"
