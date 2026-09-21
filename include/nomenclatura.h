#ifndef CHEMINATOR_NOMENCLATURA_H
#define CHEMINATOR_NOMENCLATURA_H

#include "elementos.h"
#include "formula.h"
#include "no_metales.h"
#include "hidracidos.h"
#include "oxacidos.h"
#include "radicales.h"

// Máximo número de líneas de razonamiento que registra una Explicacion, y
// longitud máxima de cada línea (más holgada que TAM_MAX porque una línea de
// explicación es una oración completa, no solo un símbolo o fórmula corta).
constexpr int MAX_PASOS_EXPLICACION = 10;
constexpr int TAM_MAX_PASO = 160;

// Bitácora opcional de los pasos de razonamiento que sigue una función de
// nomenclatura (p.ej. "Se identifica el metal (Fe) y el oxigeno (O)",
// "Hierro tiene valencias conocidas: II, III"). Cada función de nomenclatura
// la recibe por puntero y, si no es nullptr, agrega una línea por cada paso
// relevante de su cálculo; si es nullptr (el valor por defecto), no se
// registra nada y el costo es cero. Pensada para el modo de detección
// automática, que muestra el razonamiento completo además del resultado.
struct Explicacion {
	char pasos[MAX_PASOS_EXPLICACION][TAM_MAX_PASO];
	int cantidadPasos = 0;
};

// Agrega una línea a la explicación si `explicacion` no es nullptr y aún hay
// espacio disponible; no hace nada en caso contrario (uso seguro con nullptr).
void agregarPaso(Explicacion *explicacion, const char *formato, ...);

enum class ResultadoNomenclatura {
	OK,
	FORMULA_INVALIDA,        // no parseó como fórmula química
	NO_ES_OXIDO,             // no tiene exactamente metal + oxígeno
	NO_ES_PEROXIDO,          // no tiene exactamente metal + grupo peroxo (O en subíndice par)
	NO_ES_ANHIDRIDO,         // no tiene exactamente no metal + oxígeno
	NO_ES_HIDRACIDO,         // no tiene exactamente H + no metal formador de hidrácido, en la proporción esperada
	NO_ES_OXACIDO,           // no tiene exactamente H + no metal + oxígeno
	NO_ES_BASE,              // no tiene exactamente metal + grupo hidroxilo (OH), en la proporción esperada
	NO_ES_SAL,               // no tiene exactamente metal + radical poliatómico conocido, en la proporción esperada
	ELEMENTO_DESCONOCIDO,    // el metal no está en la tabla de elementos
	VALENCIA_NO_DETERMINADA, // el subíndice de O no corresponde a ninguna valencia conocida del metal
};

// Calcula la nomenclatura Stock de un óxido (MetalxOy) a partir de su fórmula
// ya parseada, escribiendo el resultado (p.ej. "oxido de hierro (III)") en
// `resultado`. Requiere que `formula` tenga exactamente dos componentes:
// un metal y oxígeno ("O"), en cualquier orden.
ResultadoNomenclatura nomenclaturaStockOxido(const FormulaParseada &formula, char resultado[TAM_MAX],
                                              Explicacion *explicacion = nullptr);

// Calcula la nomenclatura Stock de un peróxido (Metal2(O2) o equivalente) a
// partir de su fórmula ya parseada, escribiendo el resultado (p.ej.
// "peroxido de sodio") en `resultado`. Un peróxido siempre contiene el grupo
// peroxo (O2)^2-, por lo que el subíndice de oxígeno en la fórmula debe ser
// el doble del subíndice del metal (p.ej. Na2O2, H2O2, BaO2).
ResultadoNomenclatura nomenclaturaStockPeroxido(const FormulaParseada &formula, char resultado[TAM_MAX],
                                                 Explicacion *explicacion = nullptr);

// Calcula la nomenclatura tradicional de un anhídrido (óxido de no metal,
// NoMetalxOy) a partir de su fórmula ya parseada, escribiendo el resultado
// (p.ej. "anhidrido sulfurico") en `resultado`. Requiere que `formula` tenga
// exactamente dos componentes: un no metal y oxígeno ("O"), en cualquier
// orden. Usa sufijos -oso/-ico y, para no metales con 3 o 4 valencias
// conocidas, también los prefijos hipo-/per- según la posición de la
// valencia deducida dentro de la lista ordenada de valencias del no metal.
ResultadoNomenclatura nomenclaturaTradicionalAnhidrido(const FormulaParseada &formula, char resultado[TAM_MAX],
                                                        Explicacion *explicacion = nullptr);

// Calcula la nomenclatura tradicional de un ácido hidrácido (HxE, con E un no
// metal formador de hidrácido) a partir de su fórmula ya parseada, escribiendo
// el resultado (p.ej. "acido clorhidrico") en `resultado`. Requiere que
// `formula` tenga exactamente dos componentes: hidrógeno ("H") y un no metal
// de la tabla de hidrácidos, con el subíndice de H igual a la valencia
// negativa de ese no metal (p.ej. HCl, H2S).
ResultadoNomenclatura nomenclaturaTradicionalHidracido(const FormulaParseada &formula, char resultado[TAM_MAX],
                                                        Explicacion *explicacion = nullptr);

// Calcula la nomenclatura tradicional de un ácido oxácido (HxEyOz, con E un
// no metal) a partir de su fórmula ya parseada, escribiendo el resultado
// (p.ej. "acido sulfurico") en `resultado`. Requiere que `formula` tenga
// exactamente tres componentes: hidrógeno ("H"), un no metal y oxígeno
// ("O"). La fórmula se busca tal cual (ya reducida) en una tabla de
// oxácidos conocidos, en vez de derivarse aritméticamente de anhídrido+H2O
// (ver la nota en oxacidos.h sobre por qué esa derivación no siempre es
// correcta).
ResultadoNomenclatura nomenclaturaTradicionalOxacido(const FormulaParseada &formula, char resultado[TAM_MAX],
                                                      Explicacion *explicacion = nullptr);

// Calcula la nomenclatura Stock de una base/hidróxido (Metal(OH)n) a partir
// de su fórmula ya parseada, escribiendo el resultado (p.ej. "hidroxido de
// hierro (III)") en `resultado`. Acepta tanto la forma con paréntesis
// (p.ej. "Ca(OH)2", que el parser entrega como componentes {"Ca",1} y
// {"OH",2}) como la forma sin paréntesis para un solo grupo hidroxilo
// (p.ej. "NaOH", que el parser entrega como {"Na",1},{"O",1},{"H",1}): esta
// función normaliza ambos patrones antes de deducir la valencia del metal.
ResultadoNomenclatura nomenclaturaStockBase(const FormulaParseada &formula, char resultado[TAM_MAX],
                                             Explicacion *explicacion = nullptr);

// Calcula la nomenclatura tradicional de una sal oxisal (Metal_x(Radical)_y)
// a partir de su fórmula ya parseada, escribiendo el resultado (p.ej.
// "sulfato de aluminio") en `resultado`. Acepta tanto la forma con
// paréntesis para varios grupos (p.ej. "Al2(SO4)3", que el parser entrega
// como {"Al",2},{"SO4",3}) como la forma sin paréntesis para un solo grupo
// (p.ej. "Na2SO4", que el parser entrega como {"Na",2},{"S",1},{"O",4}):
// esta función normaliza ambos patrones antes de identificar el radical y
// verificar que la carga del radical y la valencia del metal se equilibran.
ResultadoNomenclatura nomenclaturaTradicionalSal(const FormulaParseada &formula, char resultado[TAM_MAX],
                                                  Explicacion *explicacion = nullptr);

// Devuelve un mensaje de error legible para un ResultadoNomenclatura distinto de OK.
const char *mensajeError(ResultadoNomenclatura resultado);

// Una de las 7 categorías de compuesto que este programa reconoce.
enum class CategoriaCompuesto {
	OXIDO,
	PEROXIDO,
	ANHIDRIDO,
	ACIDO_HIDRACIDO,
	ACIDO_OXACIDO,
	BASE,
	SAL_OXISAL,
};

// Nombre legible de una categoría (p.ej. "oxido", "acido oxacido"), para
// mostrarla en el modo de detección automática.
const char *nombreCategoria(CategoriaCompuesto categoria);

// Prueba la fórmula ya parseada contra las 7 categorías de compuesto, en un
// orden que evita ambigüedades (ver la implementación para el porqué de ese
// orden), y devuelve la primera que reconoce la fórmula con éxito. Escribe
// el nombre resultante en `resultado`, la categoría detectada en
// `categoriaDetectada`, y opcionalmente el razonamiento paso a paso en
// `explicacion`. Si ninguna categoría reconoce la fórmula, devuelve el
// ResultadoNomenclatura de la categoría cuyo fallo se considera más
// informativo (ver implementación) y `categoriaDetectada` queda sin definir.
ResultadoNomenclatura detectarYNombrar(const FormulaParseada &formula, char resultado[TAM_MAX],
                                        CategoriaCompuesto &categoriaDetectada,
                                        Explicacion *explicacion = nullptr);

#endif // CHEMINATOR_NOMENCLATURA_H
