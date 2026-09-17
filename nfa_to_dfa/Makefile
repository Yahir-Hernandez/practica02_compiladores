# CMake compila dentro de Docker; Make coordina la ejecucion y la limpieza.
IMAGE := nfa-to-dfa

.PHONY: run test clean image

# Genera el documento Typst y el PDF con el propietario del usuario local.
run: image
	mkdir -p salida
	docker run --rm --network none \
		--user "$$(id -u):$$(id -g)" \
		--volume "$$(pwd)/salida:/app/salida" \
		$(IMAGE) ./build/nfa_to_dfa_exec

# Ejecuta CTest; Docker y Make propagan cualquier error de las pruebas.
test: image
	docker run --rm --network none $(IMAGE)

# Elimina los documentos generados, sin depender de Docker.
clean:
	rm -rf salida

# Docker reutiliza las capas sin cambios y recompila cuando cambia el codigo.
image:
	docker build -t $(IMAGE) .
