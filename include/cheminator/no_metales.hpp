#ifndef CHEMINATOR_NO_METALES_H
#define CHEMINATOR_NO_METALES_H

#include "cheminator/elementos.hpp"

// Máximo número de valencias que puede tener un no metal en esta tabla.
constexpr int MAX_VALENCIAS_NO_METAL = 4;

struct InfoNoMetal {
	const char *simbolo;
	const char *nombre;
	// Raíz usada para los sufijos -oso/-ico (p.ej. "sulfur" para el azufre,
	// que da "sulfuroso"/"sulfurico"; no siempre coincide con el nombre).
	const char *raiz;
	// Valencias en orden ascendente. El orden importa: la nomenclatura
	// tradicional asigna el sufijo/prefijo según la posición de la valencia
	// usada dentro de esta lista (ver nomenclaturaTradicionalAnhidrido).
	int valencias[MAX_VALENCIAS_NO_METAL];
	int cantidadValencias;
};

// Busca la información (nombre, raíz y valencias) del no metal con el
// símbolo dado. Devuelve `nullptr` si el símbolo no está en la tabla.
const InfoNoMetal *buscarNoMetal(const char simbolo[]);

#endif // CHEMINATOR_NO_METALES_H
