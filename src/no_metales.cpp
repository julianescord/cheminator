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

	return InfoNoMetal{simbolo,         nombre, raiz, /*raizIco=*/{}, {Valencia{valencias}...},
	                   static_cast<int>(sizeof...(Enteros))};
}

// Variante para los pocos elementos cuya raiz cambia segun el sufijo. Es una
// funcion aparte porque las valencias son un paquete variadico y no admiten
// nada detras.
template <typename... Enteros>
constexpr InfoNoMetal noMetalDobleRaiz(std::string_view simbolo, std::string_view nombre, std::string_view raizOso,
                                        std::string_view raizIco, Enteros... valencias)
{
	static_assert(sizeof...(Enteros) >= 1, "Un no metal debe declarar al menos una valencia.");
	static_assert(sizeof...(Enteros) <= MAX_VALENCIAS, "Demasiadas valencias para MAX_VALENCIAS.");

	return InfoNoMetal{simbolo,         nombre, raizOso, raizIco, {Valencia{valencias}...},
	                   static_cast<int>(sizeof...(Enteros))};
}

// No metales tipicos de un curso basico, con las valencias que usan al
// combinarse con oxigeno (anhidridos). El nitrogeno con valencia 2 (NO) y
// otros casos irregulares quedan fuera a proposito: no siguen el patron
// regular de prefijos y sufijos.
// El fluor no aparece: al ser el elemento mas electronegativo, no forma
// oxidos acidos, solo compuestos con el oxigeno en estado positivo.
constexpr std::array TABLA_NO_METALES = {
	noMetal("B", "Boro", "bor", 3),
	noMetal("C", "Carbono", "carbon", 2, 4),
	noMetal("Si", "Silicio", "silic", 4),
	noMetal("N", "Nitrogeno", "nitr", 1, 3, 5),
	noMetal("P", "Fosforo", "fosfor", 3, 5),
	noMetalDobleRaiz("As", "Arsenico", "arseni", "arsen", 3, 5),
	noMetalDobleRaiz("Sb", "Antimonio", "antimoni", "antimon", 3, 5),
	noMetal("S", "Azufre", "sulfur", 2, 4, 6),
	noMetalDobleRaiz("Se", "Selenio", "seleni", "selen", 2, 4, 6),
	noMetal("Te", "Teluro", "telur", 2, 4, 6),
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

std::span<const InfoNoMetal> todosLosNoMetales() noexcept
{
	return TABLA_NO_METALES;
}

} // namespace cheminator
