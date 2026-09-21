#ifndef CHEMINATOR_FORMULA_H
#define CHEMINATOR_FORMULA_H

#include "elementos.h"

// Un componente de una fórmula: un símbolo de elemento (o, si viene entre
// paréntesis, el contenido completo del grupo, p.ej. "OH") y su subíndice.
// Ej. en "Fe2O3", los componentes son {"Fe", 2} y {"O", 3}; en "Ca(OH)2",
// son {"Ca", 1} y {"OH", 2}.
struct ComponenteFormula {
	char simbolo[TAM_MAX];
	int subindice;
};

// Máximo número de componentes distintos que se aceptan en una fórmula
// (suficiente para los compuestos inorgánicos simples de este programa).
constexpr int MAX_COMPONENTES = 4;

enum class ResultadoParseo {
	OK,
	FORMULA_VACIA,
	SIMBOLO_INVALIDO,   // no empieza con mayúscula, o tiene caracteres no alfanuméricos
	SUBINDICE_INVALIDO, // subíndice es 0 o no numérico
	DEMASIADOS_COMPONENTES,
	GRUPO_MAL_FORMADO,  // paréntesis sin cerrar, vacío, o anidado
};

struct FormulaParseada {
	ComponenteFormula componentes[MAX_COMPONENTES];
	int cantidadComponentes = 0;
};

// Analiza una fórmula química simple (p.ej. "Fe2O3", "H2O", "NaCl") y la
// descompone en pares (símbolo, subíndice). Un símbolo sin subíndice
// explícito se interpreta con subíndice 1. También reconoce un grupo entre
// paréntesis con su propio subíndice (p.ej. "Ca(OH)2" -> {"Ca",1}, {"OH",2}),
// útil para radicales como el hidroxilo o los aniones poliatómicos de sales.
ResultadoParseo parsearFormula(const char formula[], FormulaParseada &resultado);

// Devuelve un mensaje de error legible para un ResultadoParseo distinto de OK.
const char *mensajeError(ResultadoParseo resultado);

#endif // CHEMINATOR_FORMULA_H
