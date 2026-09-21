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
}
