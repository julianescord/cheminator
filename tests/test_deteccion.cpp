#include "cheminator/formula.hpp"
#include "cheminator/nomenclatura.hpp"
#include "test_runner.h"
#include <cstring>

static void verificarDeteccion(const char *formulaTexto, CategoriaCompuesto categoriaEsperada, const char *nombreEsperado)
{
	FormulaParseada f;
	ResultadoParseo rp = parsearFormula(formulaTexto, f);
	ASSERT_TRUE(rp == ResultadoParseo::OK);

	char resultado[TAM_MAX];
	CategoriaCompuesto categoria;
	ResultadoNomenclatura rn = detectarYNombrar(f, resultado, categoria);
	ASSERT_TRUE(rn == ResultadoNomenclatura::OK);
	ASSERT_TRUE(categoria == categoriaEsperada);
	ASSERT_TRUE(std::strcmp(resultado, nombreEsperado) == 0);
}

void test_deteccion()
{
	// Una fórmula representativa de cada una de las 7 categorías, sin que
	// el llamador indique cuál es.
	verificarDeteccion("Fe2O3", CategoriaCompuesto::OXIDO, "oxido de Hierro (III)");
	verificarDeteccion("Na2O2", CategoriaCompuesto::PEROXIDO, "peroxido de Sodio");
	verificarDeteccion("SO3", CategoriaCompuesto::ANHIDRIDO, "anhidrido sulfurico");
	verificarDeteccion("HCl", CategoriaCompuesto::ACIDO_HIDRACIDO, "acido clorhidrico");
	verificarDeteccion("H2SO4", CategoriaCompuesto::ACIDO_OXACIDO, "acido sulfurico");
	verificarDeteccion("Ca(OH)2", CategoriaCompuesto::BASE, "hidroxido de Calcio");
	verificarDeteccion("Al2(SO4)3", CategoriaCompuesto::SAL_OXISAL, "sulfato de Aluminio");

	// Caso ambiguo real: CuO calza tanto con "oxido de cobre (II)" como,
	// formalmente, con un peroxido de cobre de valencia 1. Se prioriza la
	// lectura de oxido normal (ver la nota en detectarYNombrar).
	verificarDeteccion("CuO", CategoriaCompuesto::OXIDO, "oxido de Cobre (II)");

	// Una fórmula que no corresponde a ninguna categoría reconocida.
	FormulaParseada f;
	char resultado[TAM_MAX];
	CategoriaCompuesto categoria;
	parsearFormula("NaCl", f);
	ASSERT_TRUE(detectarYNombrar(f, resultado, categoria) == ResultadoNomenclatura::FORMULA_INVALIDA);

	// La explicación se llena cuando se pasa un puntero no nulo.
	Explicacion explicacion;
	parsearFormula("Fe2O3", f);
	detectarYNombrar(f, resultado, categoria, &explicacion);
	ASSERT_TRUE(explicacion.cantidadPasos > 0);
}
