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
	// "Ca((OH))2" ya no es un error: el parser paso a ser recursivo para poder
	// analizar la esfera de coordinacion de los complejos, y el anidamiento es
	// justo lo que eso habilita. Lo que sigue siendo invalido es un cierre sin
	// su apertura y un grupo sin cerrar, que es lo que comprueban las lineas
	// de alrededor.
	verificarError("Ca(OH))2", ErrorFormula::GRUPO_MAL_FORMADO);
	verificarError("Ca[OH)2", ErrorFormula::GRUPO_MAL_FORMADO);
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

// El arbol y la carga son lo que agrega el parser recursivo. La vista plana
// que usan las siete categorias simples se comprueba arriba; aqui se verifica
// que ademas queda registrado el anidamiento.
void test_formula_arbol()
{
	// Un compuesto simple: dos hojas, sin grupos ni carga.
	{
		const auto r = parsearFormula("Fe2O3");
		ASSERT_TRUE(r.ok());
		if (!r.ok()) return;

		const Formula &f = r.valor();
		ASSERT_EQ_INT(static_cast<int>(f.raices().size()), 2);
		ASSERT_TRUE(!f.raices()[0].esGrupo());
		ASSERT_TRUE(!f.esIon());
		ASSERT_TRUE(f.esferaDeCoordinacion() == nullptr);
	}

	// Un grupo entre parentesis conserva sus hijos, ademas de aplanarse a
	// "OH" en la vista plana.
	{
		const auto r = parsearFormula("Ca(OH)2");
		ASSERT_TRUE(r.ok());
		if (!r.ok()) return;

		const Formula &f = r.valor();
		ASSERT_EQ_INT(static_cast<int>(f.raices().size()), 2);
		ASSERT_EQ_STR(f.componentes()[1].simbolo, "OH");
		ASSERT_TRUE(f.raices()[1].esGrupo());
		ASSERT_EQ_INT(static_cast<int>(f.raices()[1].hijos.size()), 2);
		ASSERT_EQ_STR(f.raices()[1].hijos[0].simbolo, "O");
		ASSERT_EQ_STR(f.raices()[1].hijos[1].simbolo, "H");
	}

	// Corchete con un parentesis dentro: el anidamiento que el parser anterior
	// rechazaba.
	{
		const auto r = parsearFormula("K3[Fe(CN)6]");
		ASSERT_TRUE(r.ok());
		if (!r.ok()) return;

		const Formula &f = r.valor();
		ASSERT_EQ_INT(static_cast<int>(f.raices().size()), 2);
		ASSERT_EQ_STR(f.raices()[0].simbolo, "K");
		ASSERT_EQ_INT(f.raices()[0].subindice.valor(), 3);

		const NodoFormula *esfera = f.esferaDeCoordinacion();
		ASSERT_TRUE(esfera != nullptr);
		if (esfera == nullptr) return;

		ASSERT_EQ_STR(esfera->simbolo, "Fe(CN)6");
		ASSERT_EQ_INT(static_cast<int>(esfera->hijos.size()), 2);
		ASSERT_EQ_STR(esfera->hijos[0].simbolo, "Fe");
		ASSERT_EQ_STR(esfera->hijos[1].simbolo, "CN");
		ASSERT_EQ_INT(esfera->hijos[1].subindice.valor(), 6);
		ASSERT_EQ_INT(static_cast<int>(esfera->hijos[1].hijos.size()), 2);
	}

	// Carga como sufijo, con y sin magnitud explicita.
	{
		const auto r = parsearFormula("[Fe(CN)6]3-");
		ASSERT_TRUE(r.ok());
		if (r.ok())
		{
			ASSERT_EQ_INT(r.valor().carga().valor(), -3);
			ASSERT_TRUE(r.valor().esIon());
		}

		const auto amonio = parsearFormula("[NH4]+");
		ASSERT_TRUE(amonio.ok());
		if (amonio.ok())
		{
			ASSERT_EQ_INT(amonio.valor().carga().valor(), 1);
		}
	}

	verificarError("3-", ErrorFormula::CARGA_MAL_FORMADA);
	verificarError("Fe0-", ErrorFormula::CARGA_MAL_FORMADA);
	verificarError("[Fe(CN)6", ErrorFormula::GRUPO_MAL_FORMADO);
}
