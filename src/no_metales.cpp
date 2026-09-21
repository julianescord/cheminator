#include "cheminator/no_metales.hpp"

#include "validacion_tablas.hpp"

namespace cheminator {
namespace {

template <typename... Enteros>
constexpr InfoNoMetal noMetal(std::string_view simbolo, std::string_view nombre, std::string_view raiz,
                               Enteros... valencias)
{
	static_assert(sizeof...(Enteros) >= 1, "Un no metal debe declarar al menos una valencia.");
	static_assert(sizeof...(Enteros) <= MAX_VALENCIAS, "Demasiadas valencias para MAX_VALENCIAS.");

	return InfoNoMetal{simbolo, nombre, raiz, {Valencia{valencias}...}, static_cast<int>(sizeof...(Enteros))};
}

// No metales tipicos de un curso basico, con las valencias que usan al
// combinarse con oxigeno (anhidridos). El nitrogeno con valencia 2 (NO) y
// otros casos irregulares quedan fuera a proposito: no siguen el patron
// regular de prefijos y sufijos.
constexpr std::array TABLA_NO_METALES = {
	noMetal("C", "Carbono", "carbon", 2, 4),
	noMetal("N", "Nitrogeno", "nitr", 1, 3, 5),
	noMetal("P", "Fosforo", "fosfor", 3, 5),
	noMetal("S", "Azufre", "sulfur", 2, 4, 6),
	noMetal("Cl", "Cloro", "clor", 1, 3, 5, 7),
	noMetal("Br", "Bromo", "brom", 1, 3, 5, 7),
	noMetal("I", "Yodo", "iod", 1, 3, 5, 7),
};

static_assert(detalle::clavesUnicas(TABLA_NO_METALES, [](const InfoNoMetal &e) { return e.simbolo; }),
              "Hay un simbolo repetido en la tabla de no metales.");

static_assert(detalle::cantidadesEnRango(
                  TABLA_NO_METALES, [](const InfoNoMetal &e) { return e.cantidadValencias; }, MAX_VALENCIAS),
              "Algun no metal declara una cantidad de valencias fuera de rango.");

static_assert(detalle::valenciasAscendentes(TABLA_NO_METALES,
                                             [](const InfoNoMetal &e) { return e.valenciasConocidas(); }),
              "Las valencias de algun no metal no estan en orden ascendente: eso romperia la asignacion "
              "de prefijos y sufijos tradicionales, que depende de la posicion.");

} // namespace

const InfoNoMetal *buscarNoMetal(std::string_view simbolo) noexcept
{
	for (const InfoNoMetal &noMetal : TABLA_NO_METALES)
	{
		if (noMetal.simbolo == simbolo)
		{
			return &noMetal;
		}
	}
	return nullptr;
}

} // namespace cheminator
