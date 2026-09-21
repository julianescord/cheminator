#ifndef CHEMINATOR_OXACIDOS_H
#define CHEMINATOR_OXACIDOS_H

#include "cheminator/elementos.hpp"

// Un ácido oxácido conocido: la fórmula HxEyOz (ya en su forma reducida
// estándar, no derivada aritméticamente) que corresponde a un no metal con
// una valencia específica, junto con el nombre tradicional resultante.
//
// Se tabula explícitamente (en vez de calcularse sumando el anhídrido + H2O
// y reduciendo) porque esa suma no siempre da la fórmula real: por ejemplo,
// el fósforo con valencia 3 y 5 da H3PO3 y H3PO4 (no HPO2/HPO3 como
// resultaría de la regla simple de intercambio de valencias), por la
// estructura molecular real de esos ácidos.
struct InfoOxacido {
	const char *simboloNoMetal;
	int subindiceH;
	int subindiceNoMetal;
	int subindiceO;
	// Nombre completo tradicional, p.ej. "acido sulfurico".
	const char *nombre;
};

// Busca el oxácido cuya fórmula coincide exactamente con los subíndices
// dados para un no metal. Devuelve `nullptr` si no hay coincidencia.
const InfoOxacido *buscarOxacido(const char simboloNoMetal[], int subindiceH, int subindiceNoMetal, int subindiceO);

#endif // CHEMINATOR_OXACIDOS_H
