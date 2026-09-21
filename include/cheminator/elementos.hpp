#ifndef CHEMINATOR_ELEMENTOS_HPP
#define CHEMINATOR_ELEMENTOS_HPP

#include "cheminator/tipos.hpp"

#include <array>
#include <cstddef>
#include <span>
#include <string_view>

namespace cheminator {

// Maximo numero de valencias que puede declarar un elemento en estas tablas.
inline constexpr int MAX_VALENCIAS = 4;

struct InfoElemento {
	std::string_view simbolo;
	std::string_view nombre;
	// En orden ascendente; el orden importa para la nomenclatura tradicional,
	// que elige prefijo y sufijo segun la posicion de la valencia usada.
	std::array<Valencia, MAX_VALENCIAS> valencias;
	int cantidadValencias;

	// Solo las valencias realmente declaradas, sin el relleno del array.
	constexpr std::span<const Valencia> valenciasConocidas() const noexcept
	{
		return {valencias.data(), static_cast<std::size_t>(cantidadValencias)};
	}

	constexpr bool tieneUnicaValencia() const noexcept { return cantidadValencias == 1; }

	constexpr bool admite(Valencia valencia) const noexcept
	{
		for (const Valencia conocida : valenciasConocidas())
		{
			if (conocida == valencia)
			{
				return true;
			}
		}
		return false;
	}
};

// Busca un metal por su simbolo. Devuelve nullptr si no esta en la tabla.
const InfoElemento *buscarElemento(std::string_view simbolo) noexcept;

// La tabla completa. Hace falta para recorrerla en la direccion contraria,
// buscando por nombre en vez de por simbolo, que es lo que necesita la
// formulacion inversa.
std::span<const InfoElemento> todosLosElementos() noexcept;

} // namespace cheminator

#endif // CHEMINATOR_ELEMENTOS_HPP
