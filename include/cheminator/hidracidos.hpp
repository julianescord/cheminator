#ifndef CHEMINATOR_HIDRACIDOS_H
#define CHEMINATOR_HIDRACIDOS_H

#include "cheminator/elementos.hpp"

// Un no metal que forma ácido hidrácido con el hidrógeno (HxE), con la
// cantidad de átomos de H que le corresponde por su valencia como anión
// (p.ej. Cl actúa como -1 -> HCl; S actúa como -2 -> H2S). Esta tabla es
// independiente de la de anhídridos: ahí el no metal actúa con valencia
// positiva (formando enlace con oxígeno); aquí actúa con valencia negativa
// (formando enlace con hidrógeno), y el conjunto de no metales que forman
// hidrácidos comunes es distinto (halógenos + S, Se, Te; no incluye N, P, C).
struct InfoHidracido {
	const char *simbolo;
	// Raíz para armar el nombre "acido <raiz>hidrico" (p.ej. "clor" -> "acido clorhidrico").
	const char *raiz;
	// Subíndice de hidrógeno esperado en la fórmula reducida (= |valencia negativa|).
	int subindiceHidrogeno;
};

// Busca la información de un no metal que forma ácido hidrácido.
// Devuelve `nullptr` si el símbolo no forma un hidrácido común.
const InfoHidracido *buscarHidracido(const char simbolo[]);

#endif // CHEMINATOR_HIDRACIDOS_H
