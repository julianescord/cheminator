#include "cheminator/cheminator.h"

#include "cheminator/formula.hpp"
#include "cheminator/nomenclatura.hpp"

#include <new>
#include <string>
#include <vector>

// CMake define la version a partir del project(); el respaldo evita que el
// archivo deje de compilar si alguien lo construye fuera de CMake.
#ifndef CHEMINATOR_VERSION
#define CHEMINATOR_VERSION "desconocida"
#endif

namespace {

// Lo que hay detras del tipo opaco de la API en C. Guarda las cadenas ya
// materializadas para poder devolver punteros estables mientras el objeto
// viva, sin que quien llama tenga que gestionar nada mas que liberarlo.
struct ResultadoInterno {
	bool ok = false;
	std::string nombre;
	std::string categoria;
	std::string error;
	std::vector<std::string> pasos;
};

constexpr const char *VACIA = "";

} // namespace

// La definicion del tipo opaco declarado en el header.
struct chem_resultado {
	ResultadoInterno datos;
};

extern "C" {

chem_resultado *chem_nombrar(const char *formula)
{
	// La API en C no propaga excepciones: cualquier fallo de asignacion se
	// convierte en NULL, que es lo que el contrato del header promete.
	chem_resultado *salida = new (std::nothrow) chem_resultado{};
	if (salida == nullptr)
	{
		return nullptr;
	}

	try
	{
		const std::string texto = formula == nullptr ? std::string{} : std::string{formula};

		const auto analizada = cheminator::parsearFormula(texto);
		if (!analizada)
		{
			salida->datos.error = cheminator::mensajeError(analizada.error());
			return salida;
		}

		const auto resultado = cheminator::nombrar(analizada.valor());
		if (!resultado)
		{
			salida->datos.error = cheminator::mensajeError(resultado.error());
			return salida;
		}

		const cheminator::Nomenclatura &nomenclatura = resultado.valor();
		salida->datos.ok = true;
		salida->datos.nombre = nomenclatura.nombre;
		salida->datos.categoria = cheminator::nombreCategoria(nomenclatura.categoria);
		salida->datos.pasos = nomenclatura.pasos;
	}
	catch (const std::bad_alloc &)
	{
		delete salida;
		return nullptr;
	}
	catch (...)
	{
		salida->datos.ok = false;
		salida->datos.error = "Error interno al procesar la formula.";
	}

	return salida;
}

void chem_resultado_liberar(chem_resultado *resultado)
{
	delete resultado;
}

int chem_resultado_ok(const chem_resultado *resultado)
{
	return (resultado != nullptr && resultado->datos.ok) ? 1 : 0;
}

const char *chem_resultado_nombre(const chem_resultado *resultado)
{
	return resultado == nullptr ? VACIA : resultado->datos.nombre.c_str();
}

const char *chem_resultado_categoria(const chem_resultado *resultado)
{
	return resultado == nullptr ? VACIA : resultado->datos.categoria.c_str();
}

const char *chem_resultado_error(const chem_resultado *resultado)
{
	return resultado == nullptr ? VACIA : resultado->datos.error.c_str();
}

size_t chem_resultado_cantidad_pasos(const chem_resultado *resultado)
{
	return resultado == nullptr ? 0 : resultado->datos.pasos.size();
}

const char *chem_resultado_paso(const chem_resultado *resultado, size_t indice)
{
	if (resultado == nullptr || indice >= resultado->datos.pasos.size())
	{
		return VACIA;
	}
	return resultado->datos.pasos[indice].c_str();
}

const char *chem_version(void)
{
	return CHEMINATOR_VERSION;
}

} // extern "C"
