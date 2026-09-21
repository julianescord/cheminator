#ifndef CHEMINATOR_NOMENCLATURA_H
#define CHEMINATOR_NOMENCLATURA_H

#include "elementos.h"
#include "formula.h"
#include "no_metales.h"
#include "hidracidos.h"
#include "oxacidos.h"

enum class ResultadoNomenclatura {
	OK,
	FORMULA_INVALIDA,        // no parseó como fórmula química
	NO_ES_OXIDO,             // no tiene exactamente metal + oxígeno
	NO_ES_PEROXIDO,          // no tiene exactamente metal + grupo peroxo (O en subíndice par)
	NO_ES_ANHIDRIDO,         // no tiene exactamente no metal + oxígeno
	NO_ES_HIDRACIDO,         // no tiene exactamente H + no metal formador de hidrácido, en la proporción esperada
	NO_ES_OXACIDO,           // no tiene exactamente H + no metal + oxígeno
	ELEMENTO_DESCONOCIDO,    // el metal no está en la tabla de elementos
	VALENCIA_NO_DETERMINADA, // el subíndice de O no corresponde a ninguna valencia conocida del metal
};

// Calcula la nomenclatura Stock de un óxido (MetalxOy) a partir de su fórmula
// ya parseada, escribiendo el resultado (p.ej. "oxido de hierro (III)") en
// `resultado`. Requiere que `formula` tenga exactamente dos componentes:
// un metal y oxígeno ("O"), en cualquier orden.
ResultadoNomenclatura nomenclaturaStockOxido(const FormulaParseada &formula, char resultado[TAM_MAX]);

// Calcula la nomenclatura Stock de un peróxido (Metal2(O2) o equivalente) a
// partir de su fórmula ya parseada, escribiendo el resultado (p.ej.
// "peroxido de sodio") en `resultado`. Un peróxido siempre contiene el grupo
// peroxo (O2)^2-, por lo que el subíndice de oxígeno en la fórmula debe ser
// el doble del subíndice del metal (p.ej. Na2O2, H2O2, BaO2).
ResultadoNomenclatura nomenclaturaStockPeroxido(const FormulaParseada &formula, char resultado[TAM_MAX]);

// Calcula la nomenclatura tradicional de un anhídrido (óxido de no metal,
// NoMetalxOy) a partir de su fórmula ya parseada, escribiendo el resultado
// (p.ej. "anhidrido sulfurico") en `resultado`. Requiere que `formula` tenga
// exactamente dos componentes: un no metal y oxígeno ("O"), en cualquier
// orden. Usa sufijos -oso/-ico y, para no metales con 3 o 4 valencias
// conocidas, también los prefijos hipo-/per- según la posición de la
// valencia deducida dentro de la lista ordenada de valencias del no metal.
ResultadoNomenclatura nomenclaturaTradicionalAnhidrido(const FormulaParseada &formula, char resultado[TAM_MAX]);

// Calcula la nomenclatura tradicional de un ácido hidrácido (HxE, con E un no
// metal formador de hidrácido) a partir de su fórmula ya parseada, escribiendo
// el resultado (p.ej. "acido clorhidrico") en `resultado`. Requiere que
// `formula` tenga exactamente dos componentes: hidrógeno ("H") y un no metal
// de la tabla de hidrácidos, con el subíndice de H igual a la valencia
// negativa de ese no metal (p.ej. HCl, H2S).
ResultadoNomenclatura nomenclaturaTradicionalHidracido(const FormulaParseada &formula, char resultado[TAM_MAX]);

// Calcula la nomenclatura tradicional de un ácido oxácido (HxEyOz, con E un
// no metal) a partir de su fórmula ya parseada, escribiendo el resultado
// (p.ej. "acido sulfurico") en `resultado`. Requiere que `formula` tenga
// exactamente tres componentes: hidrógeno ("H"), un no metal y oxígeno
// ("O"). La fórmula se busca tal cual (ya reducida) en una tabla de
// oxácidos conocidos, en vez de derivarse aritméticamente de anhídrido+H2O
// (ver la nota en oxacidos.h sobre por qué esa derivación no siempre es
// correcta).
ResultadoNomenclatura nomenclaturaTradicionalOxacido(const FormulaParseada &formula, char resultado[TAM_MAX]);

// Devuelve un mensaje de error legible para un ResultadoNomenclatura distinto de OK.
const char *mensajeError(ResultadoNomenclatura resultado);

#endif // CHEMINATOR_NOMENCLATURA_H
