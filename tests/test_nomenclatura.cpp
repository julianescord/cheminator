#include "formula.h"
#include "nomenclatura.h"
#include "test_runner.h"
#include <cstring>

static void verificarOxido(const char *formulaTexto, const char *esperado)
{
	FormulaParseada f;
	ResultadoParseo rp = parsearFormula(formulaTexto, f);
	ASSERT_TRUE(rp == ResultadoParseo::OK);

	char resultado[TAM_MAX];
	ResultadoNomenclatura rn = nomenclaturaStockOxido(f, resultado);
	ASSERT_TRUE(rn == ResultadoNomenclatura::OK);
	ASSERT_TRUE(std::strcmp(resultado, esperado) == 0);
}

static void verificarPeroxido(const char *formulaTexto, const char *esperado)
{
	FormulaParseada f;
	ResultadoParseo rp = parsearFormula(formulaTexto, f);
	ASSERT_TRUE(rp == ResultadoParseo::OK);

	char resultado[TAM_MAX];
	ResultadoNomenclatura rn = nomenclaturaStockPeroxido(f, resultado);
	ASSERT_TRUE(rn == ResultadoNomenclatura::OK);
	ASSERT_TRUE(std::strcmp(resultado, esperado) == 0);
}

static void verificarAnhidrido(const char *formulaTexto, const char *esperado)
{
	FormulaParseada f;
	ResultadoParseo rp = parsearFormula(formulaTexto, f);
	ASSERT_TRUE(rp == ResultadoParseo::OK);

	char resultado[TAM_MAX];
	ResultadoNomenclatura rn = nomenclaturaTradicionalAnhidrido(f, resultado);
	ASSERT_TRUE(rn == ResultadoNomenclatura::OK);
	ASSERT_TRUE(std::strcmp(resultado, esperado) == 0);
}

void test_nomenclatura()
{
	// Metales con una sola valencia: sin número romano.
	verificarOxido("Na2O", "oxido de Sodio");
	verificarOxido("CaO", "oxido de Calcio");
	verificarOxido("Al2O3", "oxido de Aluminio");

	// Metales con valencia múltiple: con número romano.
	verificarOxido("FeO", "oxido de Hierro (II)");
	verificarOxido("Fe2O3", "oxido de Hierro (III)");
	verificarOxido("CuO", "oxido de Cobre (II)");
	verificarOxido("Cu2O", "oxido de Cobre (I)");
	verificarOxido("PbO2", "oxido de Plomo (IV)");

	// Casos de error.
	FormulaParseada f;
	char resultado[TAM_MAX];

	parsearFormula("NaCl", f); // no es un óxido (no tiene O)
	ASSERT_TRUE(nomenclaturaStockOxido(f, resultado) == ResultadoNomenclatura::NO_ES_OXIDO);

	parsearFormula("XxO", f); // elemento inexistente
	ASSERT_TRUE(nomenclaturaStockOxido(f, resultado) == ResultadoNomenclatura::ELEMENTO_DESCONOCIDO);

	parsearFormula("Fe5O2", f); // proporción que no corresponde a ninguna valencia de Fe
	ASSERT_TRUE(nomenclaturaStockOxido(f, resultado) == ResultadoNomenclatura::VALENCIA_NO_DETERMINADA);
}

void test_nomenclatura_peroxidos()
{
	// Metales de valencia 1: el grupo peroxo (O2) no se reduce, subíndices 2:2.
	verificarPeroxido("Na2O2", "peroxido de Sodio");
	verificarPeroxido("H2O2", "peroxido de Hidrogeno");
	verificarPeroxido("K2O2", "peroxido de Potasio");

	// Metales de valencia 2: subíndices 1:2.
	verificarPeroxido("CaO2", "peroxido de Calcio");

	// Cobre: ambas valencias (I y II) dan peróxidos válidos con distinta forma.
	verificarPeroxido("Cu2O2", "peroxido de Cobre");
	verificarPeroxido("CuO2", "peroxido de Cobre");

	// Casos de error.
	FormulaParseada f;
	char resultado[TAM_MAX];

	parsearFormula("NaCl", f); // no tiene oxigeno
	ASSERT_TRUE(nomenclaturaStockPeroxido(f, resultado) == ResultadoNomenclatura::NO_ES_PEROXIDO);

	parsearFormula("CaO", f); // es oxido normal, no peroxido (proporcion 1:1 no es 1:2)
	ASSERT_TRUE(nomenclaturaStockPeroxido(f, resultado) == ResultadoNomenclatura::VALENCIA_NO_DETERMINADA);

	parsearFormula("XxO2", f); // elemento inexistente
	ASSERT_TRUE(nomenclaturaStockPeroxido(f, resultado) == ResultadoNomenclatura::ELEMENTO_DESCONOCIDO);
}

void test_nomenclatura_anhidridos()
{
	// No metal con 2 valencias: -oso (menor) / -ico (mayor).
	verificarAnhidrido("CO2", "anhidrido carbonico"); // C solo tiene valencia 4 en la tabla -> unica opcion es -ico
	verificarAnhidrido("P2O3", "anhidrido fosforoso");
	verificarAnhidrido("P2O5", "anhidrido fosforico");

	// No metal con 3 valencias: hipo-...-oso / -oso / -ico.
	verificarAnhidrido("N2O", "anhidrido hiponitroso");
	verificarAnhidrido("N2O3", "anhidrido nitroso");
	verificarAnhidrido("N2O5", "anhidrido nitrico");
	verificarAnhidrido("SO2", "anhidrido sulfuroso");
	verificarAnhidrido("SO3", "anhidrido sulfurico");

	// No metal con 4 valencias: hipo-...-oso / -oso / -ico / per-...-ico.
	verificarAnhidrido("Cl2O", "anhidrido hipocloroso");
	verificarAnhidrido("Cl2O3", "anhidrido cloroso");
	verificarAnhidrido("Cl2O5", "anhidrido clorico");
	verificarAnhidrido("Cl2O7", "anhidrido perclorico");

	// Casos de error.
	FormulaParseada f;
	char resultado[TAM_MAX];

	parsearFormula("NaCl", f); // no tiene oxigeno
	ASSERT_TRUE(nomenclaturaTradicionalAnhidrido(f, resultado) == ResultadoNomenclatura::NO_ES_ANHIDRIDO);

	parsearFormula("FeO", f); // Fe es metal, no esta en la tabla de no metales
	ASSERT_TRUE(nomenclaturaTradicionalAnhidrido(f, resultado) == ResultadoNomenclatura::ELEMENTO_DESCONOCIDO);

	parsearFormula("NO", f); // N valencia 2 no esta en la tabla de valencias de anhidridos
	ASSERT_TRUE(nomenclaturaTradicionalAnhidrido(f, resultado) == ResultadoNomenclatura::VALENCIA_NO_DETERMINADA);
}
