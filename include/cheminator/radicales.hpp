#ifndef CHEMINATOR_RADICALES_H
#define CHEMINATOR_RADICALES_H

#include "cheminator/elementos.hpp"

// Un radical (anión poliatómico) conocido, derivado de un ácido oxácido al
// quitarle los hidrógenos (p.ej. H2SO4 -> SO4, carga -2 -> "sulfato").
// Se tabula explícitamente, igual que los oxácidos en oxacidos.h, porque el
// nombre no siempre sigue la raíz usada en anhídridos/oxácidos: el azufre
// usa "sulf" (sulfato, no "sulfurato") y el fósforo usa "fosf" (fosfato, no
// "fosforato"), aunque anhídridos use "sulfur"/"fosfor" como raíz.
struct InfoRadical {
	// Fórmula del radical tal como aparece entre paréntesis en la sal
	// (p.ej. "SO4" para el sulfato), usada para reconocerlo en la fórmula
	// parseada de la sal.
	const char *formula;
	// Carga negativa del radical (p.ej. 2 para sulfato, SO4^2-).
	int carga;
	// Nombre del radical/sal, p.ej. "sulfato".
	const char *nombre;
};

// Busca el radical cuya fórmula coincide exactamente con la dada.
// Devuelve `nullptr` si no hay coincidencia.
const InfoRadical *buscarRadical(const char formula[]);

#endif // CHEMINATOR_RADICALES_H
