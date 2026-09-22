#ifndef CHEMINATOR_FORMULACION_HPP
#define CHEMINATOR_FORMULACION_HPP

#include "cheminator/nomenclatura.hpp"
#include "cheminator/tipos.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace cheminator {

// Formulacion: el camino inverso a la nomenclatura. Dado un nombre en espanol
// ("oxido de hierro (III)"), devuelve la formula ("Fe2O3").
//
// Es el reverso exacto de nombrar(): lo que nomenclatura.cpp compone, aqui se
// descompone. Por eso reutiliza las mismas tablas en vez de duplicarlas, y por
// eso las pruebas comprueban el viaje de ida y vuelta.

enum class ErrorFormulacion {
	VACIO,                  // no se escribio nada
	CATEGORIA_DESCONOCIDA,  // no empieza por "oxido", "acido", "hidroxido"...
	ELEMENTO_DESCONOCIDO,   // el metal o no metal no esta en las tablas
	RADICAL_DESCONOCIDO,    // el radical de la sal no esta tabulado
	FALTA_VALENCIA,         // el elemento tiene varias valencias y no se indico
	VALENCIA_INVALIDA,      // el numero romano no corresponde a ese elemento
	NOMBRE_MAL_FORMADO,     // falta el "de", sobran palabras, etc.
	COMPLEJO_NO_SOPORTADO,  // es un compuesto de coordinacion, que todavia
	                        // solo va de formula a nombre
};

std::string_view mensajeError(ErrorFormulacion error) noexcept;

// El resultado de formular: la formula y el razonamiento que llevo a ella,
// igual que Nomenclatura pero en el otro sentido.
struct Formulacion {
	std::string formula;
	CategoriaCompuesto categoria;
	std::vector<std::string> pasos;
};

using ResultadoFormulacion = Resultado<Formulacion, ErrorFormulacion>;

// Obtiene la formula a partir del nombre. Tolera mayusculas, tildes y espacios
// de mas: "Óxido de Hierro (III)", "oxido de hierro (iii)" y
// "  OXIDO  DE  HIERRO  (III)  " dan todos "Fe2O3".
ResultadoFormulacion formular(std::string_view nombre);

// Normaliza un nombre para compararlo: pasa a minusculas, quita tildes y
// colapsa los espacios. Se expone porque tambien sirve para buscar en tablas.
std::string normalizarNombre(std::string_view texto);

} // namespace cheminator

#endif // CHEMINATOR_FORMULACION_HPP
