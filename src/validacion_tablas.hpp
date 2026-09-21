#ifndef CHEMINATOR_VALIDACION_TABLAS_HPP
#define CHEMINATOR_VALIDACION_TABLAS_HPP

#include <cstddef>

// Verificaciones que se ejecutan en tiempo de compilacion sobre las tablas de
// datos quimicos. No son pruebas unitarias: si una tabla esta mal formada, el
// proyecto directamente no compila y el mensaje del static_assert dice por que.
//
// Cubren errores que de otro modo serian silenciosos: un simbolo duplicado
// hace que la segunda entrada quede inalcanzable, y una lista de valencias
// desordenada rompe la nomenclatura tradicional, que asigna los prefijos y
// sufijos segun la posicion que ocupa la valencia dentro de la lista.
namespace cheminator::detalle {

// Ninguna entrada comparte clave con otra.
template <typename Tabla, typename ObtenerClave>
constexpr bool clavesUnicas(const Tabla &tabla, ObtenerClave clave)
{
	for (std::size_t i = 0; i < tabla.size(); ++i)
	{
		for (std::size_t j = i + 1; j < tabla.size(); ++j)
		{
			if (clave(tabla[i]) == clave(tabla[j]))
			{
				return false;
			}
		}
	}
	return true;
}

// Toda entrada declara al menos una valencia y no mas de las que caben.
template <typename Tabla, typename ObtenerCantidad>
constexpr bool cantidadesEnRango(const Tabla &tabla, ObtenerCantidad cantidad, int maximo)
{
	for (std::size_t i = 0; i < tabla.size(); ++i)
	{
		const int n = cantidad(tabla[i]);
		if (n < 1 || n > maximo)
		{
			return false;
		}
	}
	return true;
}

// Las valencias de cada entrada estan en orden estrictamente ascendente y son
// positivas.
template <typename Tabla, typename ObtenerValencias>
constexpr bool valenciasAscendentes(const Tabla &tabla, ObtenerValencias valencias)
{
	for (std::size_t i = 0; i < tabla.size(); ++i)
	{
		const auto lista = valencias(tabla[i]);
		int anterior = 0;
		for (const auto valencia : lista)
		{
			if (valencia.valor() <= anterior)
			{
				return false;
			}
			anterior = valencia.valor();
		}
	}
	return true;
}

} // namespace cheminator::detalle

#endif // CHEMINATOR_VALIDACION_TABLAS_HPP
