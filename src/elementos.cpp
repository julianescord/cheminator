#include "cheminator/elementos.hpp"

#include "validacion_tablas.hpp"

namespace cheminator {
namespace {

// Construye una entrada de la tabla a partir de los enteros de sus valencias.
// Antes la cantidad se escribia a mano junto a la lista ({1, 2}, 2), lo que
// permitia que ambas se desincronizaran en silencio al agregar una valencia y
// olvidar actualizar el numero; aqui la cuenta la deduce el compilador.
template <typename... Enteros>
constexpr InfoElemento elemento(std::string_view simbolo, std::string_view nombre, Enteros... valencias)
{
	static_assert(sizeof...(Enteros) >= 1, "Un elemento debe declarar al menos una valencia.");
	static_assert(sizeof...(Enteros) <= MAX_VALENCIAS, "Demasiadas valencias para MAX_VALENCIAS.");

	return InfoElemento{simbolo, nombre, {Valencia{valencias}...}, static_cast<int>(sizeof...(Enteros))};
}

// Metales y sus valencias positivas de uso mas comun en nomenclatura
// inorganica basica (no la lista completa de estados de oxidacion posibles).
constexpr std::array TABLA_ELEMENTOS = {
	// Alcalinos y el hidrogeno: valencia 1.
	elemento("H", "Hidrogeno", 1),
	elemento("Li", "Litio", 1),
	elemento("Na", "Sodio", 1),
	elemento("K", "Potasio", 1),
	elemento("Rb", "Rubidio", 1),
	elemento("Cs", "Cesio", 1),

	// Alcalinoterreos: valencia 2.
	elemento("Be", "Berilio", 2),
	elemento("Mg", "Magnesio", 2),
	elemento("Ca", "Calcio", 2),
	elemento("Sr", "Estroncio", 2),
	elemento("Ba", "Bario", 2),
	elemento("Ra", "Radio", 2),

	// Valencia fija, de otros grupos.
	elemento("Al", "Aluminio", 3),
	elemento("Zn", "Zinc", 2),
	elemento("Cd", "Cadmio", 2),
	elemento("Ag", "Plata", 1),
	elemento("O", "Oxigeno", 2),

	// De valencia variable. El orden ascendente importa: la nomenclatura
	// tradicional elige el sufijo -oso/-ico segun la posicion en esta lista.
	elemento("Cu", "Cobre", 1, 2),
	elemento("Hg", "Mercurio", 1, 2),
	elemento("Fe", "Hierro", 2, 3),
	elemento("Co", "Cobalto", 2, 3),
	elemento("Ni", "Niquel", 2, 3),
	elemento("Cr", "Cromo", 2, 3, 6),
	elemento("Mn", "Manganeso", 2, 3, 4, 7),
	elemento("Au", "Oro", 1, 3),
	elemento("Pb", "Plomo", 2, 4),
	elemento("Sn", "Estano", 2, 4),
	elemento("Pt", "Platino", 2, 4),
	elemento("Ti", "Titanio", 3, 4),
};

static_assert(detalle::clavesUnicas(TABLA_ELEMENTOS, [](const InfoElemento &e) { return e.simbolo; }),
              "Hay un simbolo repetido en la tabla de elementos: la segunda entrada seria inalcanzable.");

static_assert(detalle::cantidadesEnRango(
                  TABLA_ELEMENTOS, [](const InfoElemento &e) { return e.cantidadValencias; }, MAX_VALENCIAS),
              "Algun elemento declara una cantidad de valencias fuera de rango.");

static_assert(detalle::valenciasAscendentes(TABLA_ELEMENTOS,
                                             [](const InfoElemento &e) { return e.valenciasConocidas(); }),
              "Las valencias de algun elemento no estan en orden ascendente estricto.");

} // namespace

const InfoElemento *buscarElemento(std::string_view simbolo) noexcept
{
	for (const InfoElemento &elemento : TABLA_ELEMENTOS)
	{
		if (elemento.simbolo == simbolo)
		{
			return &elemento;
		}
	}
	return nullptr;
}

std::span<const InfoElemento> todosLosElementos() noexcept
{
	return TABLA_ELEMENTOS;
}

} // namespace cheminator
