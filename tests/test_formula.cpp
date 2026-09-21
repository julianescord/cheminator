#include "formula.h"
#include "test_runner.h"
#include <cstring>

void test_formula()
{
	FormulaParseada f;

	ASSERT_TRUE(parsearFormula("Fe2O3", f) == ResultadoParseo::OK);
	ASSERT_EQ_INT(f.cantidadComponentes, 2);
	ASSERT_TRUE(std::strcmp(f.componentes[0].simbolo, "Fe") == 0);
	ASSERT_EQ_INT(f.componentes[0].subindice, 2);
	ASSERT_TRUE(std::strcmp(f.componentes[1].simbolo, "O") == 0);
	ASSERT_EQ_INT(f.componentes[1].subindice, 3);

	// Símbolo sin subíndice explícito implica subíndice 1.
	ASSERT_TRUE(parsearFormula("NaCl", f) == ResultadoParseo::OK);
	ASSERT_EQ_INT(f.cantidadComponentes, 2);
	ASSERT_EQ_INT(f.componentes[0].subindice, 1);
	ASSERT_EQ_INT(f.componentes[1].subindice, 1);

	ASSERT_TRUE(parsearFormula("", f) == ResultadoParseo::FORMULA_VACIA);
	ASSERT_TRUE(parsearFormula("fe2O3", f) == ResultadoParseo::SIMBOLO_INVALIDO);
	ASSERT_TRUE(parsearFormula("123Fe", f) == ResultadoParseo::SIMBOLO_INVALIDO);
	ASSERT_TRUE(parsearFormula("H0", f) == ResultadoParseo::SUBINDICE_INVALIDO);

	// Grupo entre paréntesis: se guarda como un único componente con el
	// contenido literal del grupo como símbolo.
	ASSERT_TRUE(parsearFormula("Ca(OH)2", f) == ResultadoParseo::OK);
	ASSERT_EQ_INT(f.cantidadComponentes, 2);
	ASSERT_TRUE(std::strcmp(f.componentes[0].simbolo, "Ca") == 0);
	ASSERT_EQ_INT(f.componentes[0].subindice, 1);
	ASSERT_TRUE(std::strcmp(f.componentes[1].simbolo, "OH") == 0);
	ASSERT_EQ_INT(f.componentes[1].subindice, 2);

	// Grupo entre paréntesis con radical de varias letras (sales oxisal).
	ASSERT_TRUE(parsearFormula("Al2(SO4)3", f) == ResultadoParseo::OK);
	ASSERT_EQ_INT(f.cantidadComponentes, 2);
	ASSERT_TRUE(std::strcmp(f.componentes[1].simbolo, "SO4") == 0);
	ASSERT_EQ_INT(f.componentes[1].subindice, 3);

	// Un grupo sin subíndice explícito implica subíndice 1.
	ASSERT_TRUE(parsearFormula("Na(OH)", f) == ResultadoParseo::OK);
	ASSERT_EQ_INT(f.componentes[1].subindice, 1);

	// Errores de formación de grupos.
	ASSERT_TRUE(parsearFormula("Ca(OH", f) == ResultadoParseo::GRUPO_MAL_FORMADO);   // sin cerrar
	ASSERT_TRUE(parsearFormula("Ca()2", f) == ResultadoParseo::GRUPO_MAL_FORMADO);   // vacío
	ASSERT_TRUE(parsearFormula("Ca((OH))2", f) == ResultadoParseo::GRUPO_MAL_FORMADO); // anidado
}
