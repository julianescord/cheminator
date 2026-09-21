#ifndef CHEMINATOR_SRC_AFIJOS_HPP
#define CHEMINATOR_SRC_AFIJOS_HPP

#include <string_view>

// Cabecera privada: la regla de prefijos y sufijos de la nomenclatura
// tradicional, compartida por nomenclatura.cpp (que la aplica) y
// formulacion.cpp (que la invierte). Vive aqui para que ambas direcciones
// usen la MISMA regla: si cambia, cambia en los dos sentidos a la vez.

namespace cheminator::detalle {

struct Afijos {
	std::string_view prefijo;
	std::string_view sufijo;
};

// Prefijo y sufijo segun la posicion que ocupa la valencia usada dentro de la
// lista ordenada de valencias del no metal.
constexpr Afijos afijosTradicionales(int posicion, int totalValencias) noexcept
{
	if (totalValencias <= 1)
	{
		return {"", "ico"};
	}
	if (totalValencias == 2)
	{
		return posicion == 0 ? Afijos{"", "oso"} : Afijos{"", "ico"};
	}
	if (totalValencias == 3)
	{
		if (posicion == 0) return {"hipo", "oso"};
		if (posicion == 1) return {"", "oso"};
		return {"", "ico"};
	}
	if (posicion == 0) return {"hipo", "oso"};
	if (posicion == 1) return {"", "oso"};
	if (posicion == 2) return {"", "ico"};
	return {"per", "ico"};
}

} // namespace cheminator::detalle

#endif // CHEMINATOR_SRC_AFIJOS_HPP
