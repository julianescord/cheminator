#include "cheminator/formula.hpp"
#include "test_runner.h"

using namespace cheminator;

namespace {

// Comprueba que la formula se descompone en los componentes esperados.
void verificarComponentes(std::string_view texto, std::initializer_list<std::pair<std::string_view, int>> esperados)
{
	const auto resultado = parsearFormula(texto);
	ASSERT_TRUE(resultado.ok());
	if (!resultado.ok())
	{
		return;
	}

	const Formula &formula = resultado.valor();
	ASSERT_EQ_INT(static_cast<int>(formula.cantidad()), static_cast<int>(esperados.size()));
	if (formula.cantidad() != esperados.size())
	{
		return;
	}

	std::size_t i = 0;
	for (const auto &[simbolo, subindice] : esperados)
	{
		ASSERT_EQ_STR(formula.componentes()[i].simbolo, simbolo);
		ASSERT_EQ_INT(formula.componentes()[i].subindice.valor(), subindice);
		++i;
	}
}

void verificarError(std::string_view texto, ErrorFormula esperado)
{
	const auto resultado = parsearFormula(texto);
	ASSERT_TRUE(!resultado.ok());
	if (!resultado.ok())
	{
		ASSERT_TRUE(resultado.error() == esperado);
	}
}

} // namespace

void test_formula()
{
	verificarComponentes("Fe2O3", {{"Fe", 2}, {"O", 3}});

	// Un simbolo sin subindice explicito vale 1.
	verificarComponentes("NaCl", {{"Na", 1}, {"Cl", 1}});

	// Grupo entre parentesis: queda como un unico componente.
	verificarComponentes("Ca(OH)2", {{"Ca", 1}, {"OH", 2}});
	verificarComponentes("Al2(SO4)3", {{"Al", 2}, {"SO4", 3}});
	verificarComponentes("Na(OH)", {{"Na", 1}, {"OH", 1}});

	// Subindices de mas de un digito.
	verificarComponentes("C12H22", {{"C", 12}, {"H", 22}});

	verificarError("", ErrorFormula::VACIA);
	verificarError("fe2O3", ErrorFormula::SIMBOLO_INVALIDO);
	verificarError("123Fe", ErrorFormula::SIMBOLO_INVALIDO);
	verificarError("H0", ErrorFormula::SUBINDICE_INVALIDO);
	verificarError("Ca(OH", ErrorFormula::GRUPO_MAL_FORMADO);
	verificarError("Ca()2", ErrorFormula::GRUPO_MAL_FORMADO);
	verificarError("Ca((OH))2", ErrorFormula::GRUPO_MAL_FORMADO);
	verificarError("HHeLiBeB", ErrorFormula::DEMASIADOS_COMPONENTES);

	// Consultas sobre una formula ya analizada.
	const auto resultado = parsearFormula("Fe2O3");
	ASSERT_TRUE(resultado.ok());
	if (resultado.ok())
	{
		const Formula &formula = resultado.valor();
		ASSERT_TRUE(formula.componente("O") != nullptr);
		ASSERT_TRUE(formula.componente("Xx") == nullptr);

		const ComponenteFormula *metal = formula.unicoDistintoDe({"O"});
		ASSERT_TRUE(metal != nullptr);
		if (metal != nullptr)
		{
			ASSERT_EQ_STR(metal->simbolo, "Fe");
		}

		// Si queda mas de un componente sin excluir, no hay "unico".
		ASSERT_TRUE(formula.unicoDistintoDe({}) == nullptr);
	}
}
