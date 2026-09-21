#ifndef CHEMINATOR_NO_METALES_HPP
#define CHEMINATOR_NO_METALES_HPP

#include "cheminator/elementos.hpp"
#include "cheminator/tipos.hpp"

#include <array>
#include <cstddef>
#include <span>
#include <string_view>

namespace cheminator {

struct InfoNoMetal {
	std::string_view simbolo;
	std::string_view nombre;
	// Raiz para los sufijos -oso/-ico ("sulfur" para el azufre, que da
	// "sulfuroso"/"sulfurico"); no siempre coincide con el nombre.
	std::string_view raiz;
	// Casi siempre la raiz es la misma para los dos sufijos, pero el arsenico
	// y el selenio llevan una i ante -oso y no ante -ico: "arsenioso" pero
	// "arsenico", no "arseniico". Cuando difieren, esta es la de -ico.
	std::string_view raizIco;
	// En orden ascendente: la nomenclatura tradicional asigna prefijo y
	// sufijo segun la posicion que ocupa la valencia usada en esta lista.
	std::array<Valencia, MAX_VALENCIAS> valencias;
	int cantidadValencias;

	constexpr std::span<const Valencia> valenciasConocidas() const noexcept
	{
		return {valencias.data(), static_cast<std::size_t>(cantidadValencias)};
	}

	// La raiz que corresponde al sufijo que se va a usar. Se consulta por aqui
	// en vez de leer los campos directamente, para no tener que recordar en
	// cada sitio que algunos elementos cambian de raiz segun el sufijo.
	constexpr std::string_view raizPara(std::string_view sufijo) const noexcept
	{
		if (sufijo == "ico" && !raizIco.empty())
		{
			return raizIco;
		}
		return raiz;
	}

	// Posicion (0-indexada) de una valencia dentro de la lista ordenada, que
	// es lo que determina el prefijo y sufijo tradicionales. Devuelve -1 si
	// el no metal no admite esa valencia.
	constexpr int posicionDe(Valencia valencia) const noexcept
	{
		for (int i = 0; i < cantidadValencias; ++i)
		{
			if (valencias[static_cast<std::size_t>(i)] == valencia)
			{
				return i;
			}
		}
		return -1;
	}
};

// Busca un no metal por su simbolo. Devuelve nullptr si no esta en la tabla.
const InfoNoMetal *buscarNoMetal(std::string_view simbolo) noexcept;

// La tabla completa, para recorrerla buscando por nombre en vez de por
// simbolo, que es lo que necesita la formulacion inversa.
std::span<const InfoNoMetal> todosLosNoMetales() noexcept;

} // namespace cheminator

#endif // CHEMINATOR_NO_METALES_HPP
