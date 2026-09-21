#ifndef CHEMINATOR_NOMENCLATURA_H
#define CHEMINATOR_NOMENCLATURA_H

#include "elementos.h"
#include "formula.h"

enum class ResultadoNomenclatura {
	OK,
	FORMULA_INVALIDA,        // no parseó como fórmula química
	NO_ES_OXIDO,             // no tiene exactamente metal + oxígeno
	ELEMENTO_DESCONOCIDO,    // el metal no está en la tabla de elementos
	VALENCIA_NO_DETERMINADA, // el subíndice de O no corresponde a ninguna valencia conocida del metal
};

// Calcula la nomenclatura Stock de un óxido (MetalxOy) a partir de su fórmula
// ya parseada, escribiendo el resultado (p.ej. "oxido de hierro (III)") en
// `resultado`. Requiere que `formula` tenga exactamente dos componentes:
// un metal y oxígeno ("O"), en cualquier orden.
ResultadoNomenclatura nomenclaturaStockOxido(const FormulaParseada &formula, char resultado[TAM_MAX]);

// Devuelve un mensaje de error legible para un ResultadoNomenclatura distinto de OK.
const char *mensajeError(ResultadoNomenclatura resultado);

#endif // CHEMINATOR_NOMENCLATURA_H
