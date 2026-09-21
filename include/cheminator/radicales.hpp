#ifndef CHEMINATOR_RADICALES_HPP
#define CHEMINATOR_RADICALES_HPP

#include "cheminator/tipos.hpp"

#include <string_view>

namespace cheminator {

// Un radical (anion poliatomico) de los que forman sales oxisal, derivado de
// un acido oxacido al quitarle los hidrogenos (H2SO4 -> SO4, carga 2-).
//
// Igual que los oxacidos, se tabula explicitamente y no se deriva con una
// regla generica de raiz + sufijo, porque esa regla falla justo en los casos
// mas comunes: el azufre da "sulfato" (no "sulfurato") y el fosforo "fosfato"
// (no "fosforato"), pese a que como anhidridos usan las raices "sulfur" y
// "fosfor".
struct InfoRadical {
	// Formula tal como aparece entre parentesis en la sal ("SO4").
	std::string_view formula;
	// Carga negativa en valor absoluto (el sulfato, SO4(2-), lleva Carga{2}).
	Carga carga;
	std::string_view nombre;
};

// Busca un radical por su formula. Devuelve nullptr si no esta en la tabla.
const InfoRadical *buscarRadical(std::string_view formula) noexcept;

} // namespace cheminator

#endif // CHEMINATOR_RADICALES_HPP
