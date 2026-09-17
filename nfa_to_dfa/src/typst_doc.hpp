/**
 * @file typst_doc.hpp
 * @brief Generacion y compilacion del documento Typst que muestra el "antes y
 *        despues" de la conversion de un NFA a un DFA.
 *
 * El flujo es: se serializan los automatas al formato del paquete `finite`
 * (ver to_string.hpp), se arma con ellos una cadena con el documento Typst
 * completo, se escribe en disco y se invoca `typst compile` desde la consola.
 */
#pragma once
#include <string>
#include <vector>
#include "automatas.hpp"
#include "convert.hpp"

/**
 * @struct Conversion
 * @brief Un caso de estudio: el NFA original, el DFA obtenido y su trazabilidad.
 */
struct Conversion
{
  /** Titulo descriptivo que encabeza la seccion del documento. */
  std::string title;
  /** Automata no determinista de partida. */
  NFA nfa;
  /** Automata determinista equivalente. */
  DFA dfa;
  /** Subconjunto del NFA que dio origen a cada estado del DFA. */
  SubsetTable subsets;
};

/**
 * @brief Ejecuta la conversion de un NFA y empaqueta el resultado.
 * @param title Titulo de la seccion.
 * @param nfa Automata de entrada.
 * @return Caso listo para incluirse en el documento.
 */
Conversion convert_and_trace(const std::string &title, const NFA &nfa);

/**
 * @brief Construye el documento Typst completo.
 * @param cases Casos que se mostraran, uno por seccion.
 * @return Codigo fuente Typst.
 */
std::string build_typst_document(const std::vector<Conversion> &cases);

/**
 * @brief Escribe el documento en disco, creando los directorios necesarios.
 * @param path Ruta del archivo `.typ`.
 * @param document Contenido que se escribira.
 * @return true si el archivo se escribio correctamente.
 */
bool write_document(const std::string &path, const std::string &document);

/**
 * @brief Determina con que orden se invocara a Typst.
 *
 * Se prueba, en orden: la variable de entorno `TYPST_CMD`, el ejecutable
 * `typst` del PATH y, por ultimo, el envoltorio `./typst-docker.sh`, que corre
 * typst dentro de un contenedor para no tener que instalarlo en el equipo.
 *
 * @return La orden que debe usarse, o una cadena vacia si no hay ninguna.
 */
std::string typst_command();

/** @return true si hay alguna forma disponible de invocar a Typst. */
bool typst_available();

/**
 * @brief Invoca `typst compile` sobre un archivo ya escrito.
 * @param typ_path Ruta del archivo `.typ` de entrada.
 * @param out_path Ruta de salida; su extension determina el formato.
 * @return Codigo de salida del proceso `typst` (0 indica exito).
 */
int compile_document(const std::string &typ_path, const std::string &out_path);
