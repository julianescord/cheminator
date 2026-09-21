#ifndef CHEMINATOR_HIDRACIDOS_HPP
#define CHEMINATOR_HIDRACIDOS_HPP

#include "cheminator/tipos.hpp"

#include <string_view>

namespace cheminator {

// Un no metal que forma acido hidracido con el hidrogeno (HxE).
//
// Esta tabla es independiente de la de anhidridos a proposito: alli el no
// metal actua con valencia positiva (enlazado a oxigeno) y aqui con valencia
// negativa (enlazado a hidrogeno), y el conjunto de elementos que forman
// hidracidos comunes tampoco coincide (no incluye N, P ni C).
struct InfoHidracido {
	std::string_view simbolo;
	// Raiz para armar "acido <raiz>hidrico" ("clor" -> "acido clorhidrico").
	std::string_view raiz;
	// Cuantos hidrogenos lleva la formula, que es el valor absoluto de la
	// valencia negativa del no metal (Cl actua -1 -> HCl; S actua -2 -> H2S).
	Subindice hidrogenos;
};

// Busca un no metal formador de hidracido. Devuelve nullptr si no forma uno
// de los hidracidos comunes que cubre este programa.
const InfoHidracido *buscarHidracido(std::string_view simbolo) noexcept;

} // namespace cheminator

#endif // CHEMINATOR_HIDRACIDOS_HPP
