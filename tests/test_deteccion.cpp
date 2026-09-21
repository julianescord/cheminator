#include "cheminator/formula.hpp"
#include "cheminator/nomenclatura.hpp"
#include "test_runner.h"

using namespace cheminator;

namespace {

void verificarDeteccion(std::string_view texto, CategoriaCompuesto categoriaEsperada, std::string_view nombreEsperado)
{
	const auto analizada = parsearFormula(texto);
	ASSERT_TRUE(analizada.ok());
	if (!analizada.ok())
	{
		return;
	}

	const auto resultado = nombrar(analizada.valor());
	ASSERT_TRUE(resultado.ok());
	if (!resultado.ok())
	{
		std::fprintf(stderr, "  (al detectar \"%.*s\")\n", static_cast<int>(texto.size()), texto.data());
		return;
	}

	ASSERT_TRUE(resultado.valor().categoria == categoriaEsperada);
	ASSERT_EQ_STR(resultado.valor().nombre, nombreEsperado);
}

} // namespace

void test_deteccion()
{
	// Una formula representativa de cada categoria, sin decirle cual es.
	verificarDeteccion("Fe2O3", CategoriaCompuesto::OXIDO, "oxido de Hierro (III)");
	verificarDeteccion("Na2O2", CategoriaCompuesto::PEROXIDO, "peroxido de Sodio");
	verificarDeteccion("SO3", CategoriaCompuesto::ANHIDRIDO, "anhidrido sulfurico");
	verificarDeteccion("HCl", CategoriaCompuesto::ACIDO_HIDRACIDO, "acido clorhidrico");
	verificarDeteccion("H2SO4", CategoriaCompuesto::ACIDO_OXACIDO, "acido sulfurico");
	verificarDeteccion("Ca(OH)2", CategoriaCompuesto::BASE, "hidroxido de Calcio");
	verificarDeteccion("Al2(SO4)3", CategoriaCompuesto::SAL_OXISAL, "sulfato de Aluminio");

	// Formas sin parentesis de base y sal, que el parser entrega distinto.
	verificarDeteccion("NaOH", CategoriaCompuesto::BASE, "hidroxido de Sodio");
	verificarDeteccion("CaCO3", CategoriaCompuesto::SAL_OXISAL, "carbonato de Calcio");

	// Caso ambiguo real: CuO encaja como oxido de cobre (II) y tambien,
	// formalmente, como peroxido de cobre con valencia 1. Se prefiere la
	// lectura de oxido, que es la estandar.
	verificarDeteccion("CuO", CategoriaCompuesto::OXIDO, "oxido de Cobre (II)");

	// NaCl es un compuesto valido, pero de una categoria fuera del alcance
	// (sal binaria): el error debe decir eso, no inventar otra cosa.
	{
		const auto analizada = parsearFormula("NaCl");
		ASSERT_TRUE(analizada.ok());
		if (analizada.ok())
		{
			const auto resultado = nombrar(analizada.valor());
			ASSERT_TRUE(!resultado.ok());
			if (!resultado.ok())
			{
				ASSERT_TRUE(resultado.error() == ErrorNomenclatura::NINGUNA_CATEGORIA);
			}
		}
	}

	// El razonamiento acompana siempre al resultado.
	{
		const auto analizada = parsearFormula("Fe2O3");
		ASSERT_TRUE(analizada.ok());
		if (analizada.ok())
		{
			const auto resultado = nombrar(analizada.valor());
			ASSERT_TRUE(resultado.ok());
			if (resultado.ok())
			{
				ASSERT_TRUE(resultado.valor().pasos.size() >= 3);
			}
		}
	}

	ASSERT_EQ_STR(nombreCategoria(CategoriaCompuesto::SAL_OXISAL), "sal oxisal");
}
