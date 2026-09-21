#include "cheminator/radicales.hpp"

#include "validacion_tablas.hpp"

#include <array>

namespace cheminator {
namespace {

// Ningun radical de esta tabla supera esta carga; cota para la verificacion.
constexpr int MAX_CARGA = 4;

constexpr InfoRadical radical(std::string_view formula, int carga, std::string_view nombre)
{
	return InfoRadical{formula, Carga{carga}, nombre};
}

// Radicales derivados de los oxacidos tabulados en oxacidos.cpp.
constexpr std::array TABLA_RADICALES = {
	radical("CO3", 2, "carbonato"),

	radical("NO2", 1, "nitrito"),
	radical("NO3", 1, "nitrato"),

	radical("PO3", 3, "fosfito"),
	radical("PO4", 3, "fosfato"),

	radical("SO2", 2, "hiposulfito"),
	radical("SO3", 2, "sulfito"),
	radical("SO4", 2, "sulfato"),

	radical("ClO", 1, "hipoclorito"),
	radical("ClO2", 1, "clorito"),
	radical("ClO3", 1, "clorato"),
	radical("ClO4", 1, "perclorato"),

	radical("BrO", 1, "hipobromito"),
	radical("BrO2", 1, "bromito"),
	radical("BrO3", 1, "bromato"),
	radical("BrO4", 1, "perbromato"),

	radical("IO", 1, "hipoiodito"),
	radical("IO2", 1, "iodito"),
	radical("IO3", 1, "iodato"),
	radical("IO4", 1, "periodato"),
};

static_assert(detalle::clavesUnicas(TABLA_RADICALES, [](const InfoRadical &r) { return r.formula; }),
              "Hay una formula de radical repetida en la tabla.");

static_assert(detalle::clavesUnicas(TABLA_RADICALES, [](const InfoRadical &r) { return r.nombre; }),
              "Hay dos radicales con el mismo nombre en la tabla.");

static_assert(detalle::cantidadesEnRango(
                  TABLA_RADICALES, [](const InfoRadical &r) { return r.carga.valor(); }, MAX_CARGA),
              "Algun radical declara una carga fuera de rango.");

} // namespace

const InfoRadical *buscarRadical(std::string_view formula) noexcept
{
	for (const InfoRadical &radical : TABLA_RADICALES)
	{
		if (radical.formula == formula)
		{
			return &radical;
		}
	}
	return nullptr;
}

} // namespace cheminator
