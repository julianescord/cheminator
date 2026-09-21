#include "cheminator/hidracidos.hpp"

#include "validacion_tablas.hpp"

#include <array>

namespace cheminator {
namespace {

// Ningun hidracido comun lleva mas hidrogenos que esto; sirve de cota para
// la verificacion en compilacion.
constexpr int MAX_HIDROGENOS = 3;

constexpr InfoHidracido hidracido(std::string_view simbolo, std::string_view raiz, int hidrogenos)
{
	return InfoHidracido{simbolo, raiz, Subindice{hidrogenos}};
}

// Halogenos (valencia -1, un hidrogeno) y calcogenos (valencia -2, dos), que
// son los hidracidos que se ven en un curso de nomenclatura basica.
constexpr std::array TABLA_HIDRACIDOS = {
	hidracido("F", "fluor", 1),
	hidracido("Cl", "clor", 1),
	hidracido("Br", "brom", 1),
	hidracido("I", "iod", 1),
	hidracido("S", "sulf", 2),
	hidracido("Se", "selen", 2),
	hidracido("Te", "telur", 2),
};

static_assert(detalle::clavesUnicas(TABLA_HIDRACIDOS, [](const InfoHidracido &h) { return h.simbolo; }),
              "Hay un simbolo repetido en la tabla de hidracidos.");

static_assert(detalle::cantidadesEnRango(
                  TABLA_HIDRACIDOS, [](const InfoHidracido &h) { return h.hidrogenos.valor(); }, MAX_HIDROGENOS),
              "Algun hidracido declara una cantidad de hidrogenos fuera de rango.");

} // namespace

const InfoHidracido *buscarHidracido(std::string_view simbolo) noexcept
{
	for (const InfoHidracido &hidracido : TABLA_HIDRACIDOS)
	{
		if (hidracido.simbolo == simbolo)
		{
			return &hidracido;
		}
	}
	return nullptr;
}

std::span<const InfoHidracido> todosLosHidracidos() noexcept
{
	return TABLA_HIDRACIDOS;
}

} // namespace cheminator
