// Enlace de la libreria con JavaScript, para compilarla a WebAssembly.
//
// Es el motivo mas concreto por el que el nucleo esta en C++: el mismo codigo
// que corre nativo se compila a WASM con Emscripten y funciona en el navegador
// sin servidor ni instalacion, que es justo lo que necesita una herramienta
// educativa para llegar a un aula.

#include "cheminator/formula.hpp"
#include "cheminator/formulacion.hpp"
#include "cheminator/nomenclatura.hpp"

#include <emscripten/bind.h>

#include <string>
#include <vector>

#ifndef CHEMINATOR_VERSION
#define CHEMINATOR_VERSION "desconocida"
#endif

namespace {

// Lo que ve JavaScript como objeto plano. Sirve para las dos direcciones:
// al nombrar se llena `nombre` y al formular `formula`, y la que no aplica
// queda vacia.
struct ResultadoJS {
	bool ok = false;
	std::string nombre;
	std::string formula;
	std::string categoria;
	std::string error;
	std::vector<std::string> pasos;
};

ResultadoJS nombrarDesdeJS(const std::string &texto)
{
	ResultadoJS salida;

	const auto analizada = cheminator::parsearFormula(texto);
	if (!analizada)
	{
		salida.error = cheminator::mensajeError(analizada.error());
		return salida;
	}

	const auto resultado = cheminator::nombrar(analizada.valor());
	if (!resultado)
	{
		salida.error = cheminator::mensajeError(resultado.error());
		return salida;
	}

	const cheminator::Nomenclatura &nomenclatura = resultado.valor();
	salida.ok = true;
	salida.nombre = nomenclatura.nombre;
	salida.categoria = cheminator::nombreCategoria(nomenclatura.categoria);
	salida.pasos = nomenclatura.pasos;
	return salida;
}

// El camino inverso: del nombre en espanol a la formula.
ResultadoJS formularDesdeJS(const std::string &texto)
{
	ResultadoJS salida;

	const auto resultado = cheminator::formular(texto);
	if (!resultado)
	{
		salida.error = cheminator::mensajeError(resultado.error());
		return salida;
	}

	const cheminator::Formulacion &formulacion = resultado.valor();
	salida.ok = true;
	salida.formula = formulacion.formula;
	salida.categoria = cheminator::nombreCategoria(formulacion.categoria);
	salida.pasos = formulacion.pasos;
	return salida;
}

std::string version()
{
	return CHEMINATOR_VERSION;
}

} // namespace

EMSCRIPTEN_BINDINGS(cheminator)
{
	emscripten::register_vector<std::string>("VectorDeTexto");

	emscripten::value_object<ResultadoJS>("Resultado")
		.field("ok", &ResultadoJS::ok)
		.field("nombre", &ResultadoJS::nombre)
		.field("formula", &ResultadoJS::formula)
		.field("categoria", &ResultadoJS::categoria)
		.field("error", &ResultadoJS::error)
		.field("pasos", &ResultadoJS::pasos);

	emscripten::function("nombrar", &nombrarDesdeJS);
	emscripten::function("formular", &formularDesdeJS);
	emscripten::function("version", &version);
}
