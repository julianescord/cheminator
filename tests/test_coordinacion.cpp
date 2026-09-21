#include "test_runner.h"

#include "cheminator/coordinacion.hpp"
#include "cheminator/formula.hpp"
#include "cheminator/nomenclatura.hpp"

#include <string>

using namespace cheminator;

namespace {

// Comprueba que una formula da exactamente ese nombre de complejo.
bool nombreEs(const char *formula, const char *esperado)
{
	const auto analizada = parsearFormula(formula);
	if (!analizada) return false;

	const auto resultado = nombrarComplejo(analizada.valor());
	if (!resultado) return false;

	return resultado.valor().nombre == esperado;
}

bool errorEs(const char *formula, ErrorCoordinacion esperado)
{
	const auto analizada = parsearFormula(formula);
	if (!analizada) return false;

	const auto resultado = nombrarComplejo(analizada.valor());
	return !resultado && resultado.error() == esperado;
}

} // namespace

void test_coordinacion_analisis()
{
	// El estado de oxidacion sale del balance de cargas, que exige saber que
	// hay dentro del corchete: es lo que el arbol del parser hace posible.
	{
		const auto f = parsearFormula("K3[Fe(CN)6]");
		ASSERT_TRUE(f.ok());
		if (!f.ok()) return;

		const auto c = analizarComplejo(f.valor());
		ASSERT_TRUE(c.ok());
		if (!c.ok()) return;

		const Complejo &x = c.valor();
		ASSERT_EQ_STR(x.simboloCentral, "Fe");
		ASSERT_EQ_INT(x.estadoOxidacion.valor(), 3);
		ASSERT_EQ_INT(x.indiceCoordinacion, 6);
		ASSERT_EQ_INT(x.cargaEsfera.valor(), -3);
		ASSERT_EQ_INT(static_cast<int>(x.ligandos.size()), 1);
		ASSERT_EQ_STR(x.contraion, "K");
		ASSERT_TRUE(x.contraionDelante);
	}

	// El ferrocianuro solo se distingue del ferricianuro por el balance: los
	// mismos atomos con un potasio mas dan hierro (II) en vez de (III).
	{
		const auto f = parsearFormula("K4[Fe(CN)6]");
		ASSERT_TRUE(f.ok());
		if (!f.ok()) return;

		const auto c = analizarComplejo(f.valor());
		ASSERT_TRUE(c.ok());
		if (c.ok())
		{
			ASSERT_EQ_INT(c.valor().estadoOxidacion.valor(), 2);
		}
	}

	// Contraion detras: el complejo es el cation.
	{
		const auto f = parsearFormula("[Co(NH3)6]Cl3");
		ASSERT_TRUE(f.ok());
		if (!f.ok()) return;

		const auto c = analizarComplejo(f.valor());
		ASSERT_TRUE(c.ok());
		if (c.ok())
		{
			ASSERT_EQ_INT(c.valor().estadoOxidacion.valor(), 3);
			ASSERT_EQ_INT(c.valor().cargaEsfera.valor(), 3);
			ASSERT_TRUE(!c.valor().contraionDelante);
		}
	}
}

void test_coordinacion_nombres()
{
	// Anion: el metal toma la raiz latina y el sufijo -ato.
	ASSERT_TRUE(nombreEs("[Fe(CN)6]3-", "hexacianoferrato (III)"));
	ASSERT_TRUE(nombreEs("K3[Fe(CN)6]", "hexacianoferrato (III) de potasio"));
	ASSERT_TRUE(nombreEs("K4[Fe(CN)6]", "hexacianoferrato (II) de potasio"));
	ASSERT_TRUE(nombreEs("[PtCl4]2-", "tetracloroplatinato (II)"));
	ASSERT_TRUE(nombreEs("[Al(OH)4]-", "tetrahidroxoaluminato (III)"));

	// Cation: el metal conserva su nombre en espanol.
	ASSERT_TRUE(nombreEs("[Cu(NH3)4]2+", "tetraamincobre (II)"));
	ASSERT_TRUE(nombreEs("[Ag(NH3)2]+", "diaminplata (I)"));
	ASSERT_TRUE(nombreEs("[Fe(H2O)6]3+", "hexaacuahierro (III)"));
	ASSERT_TRUE(nombreEs("[Cr(H2O)6]3+", "hexaacuacromo (III)"));

	// Con contraion detras, el nombre empieza por el anion.
	ASSERT_TRUE(nombreEs("[Co(NH3)6]Cl3", "cloruro de hexaamincobalto (III)"));

	// Complejo neutro: el metal queda sin oxidar y el estado se escribe "0",
	// porque los numeros romanos no tienen cero.
	ASSERT_TRUE(nombreEs("[Ni(CO)4]", "tetracarboniloniquel (0)"));

	// --- Errores -------------------------------------------------------------

	ASSERT_TRUE(errorEs("Fe2O3", ErrorCoordinacion::NO_ES_COMPLEJO));
	ASSERT_TRUE(errorEs("[Xx(CN)6]3-", ErrorCoordinacion::SIN_ATOMO_CENTRAL));
	ASSERT_TRUE(errorEs("[Fe(ZZ)6]3-", ErrorCoordinacion::LIGANDO_DESCONOCIDO));

	// El hierro no actua con valencia 5, asi que la carga no puede ser esa.
	ASSERT_TRUE(errorEs("[Fe(CN)6]-", ErrorCoordinacion::ESTADO_INVALIDO));
}

// La deteccion automatica debe reconocer un complejo por los corchetes y, a la
// vez, no cambiar en nada como clasifica los compuestos simples.
void test_coordinacion_deteccion()
{
	const auto conCorchetes = parsearFormula("K3[Fe(CN)6]");
	ASSERT_TRUE(conCorchetes.ok());
	if (conCorchetes.ok())
	{
		const auto n = nombrar(conCorchetes.valor());
		ASSERT_TRUE(n.ok());
		if (n.ok())
		{
			ASSERT_TRUE(n.valor().categoria == CategoriaCompuesto::COMPLEJO);
			ASSERT_EQ_STR(n.valor().nombre.c_str(), "hexacianoferrato (III) de potasio");
			ASSERT_TRUE(!n.valor().pasos.empty());
		}
	}

	// Un compuesto simple sigue clasificandose igual que antes.
	const auto simple = parsearFormula("Fe2O3");
	ASSERT_TRUE(simple.ok());
	if (simple.ok())
	{
		const auto n = nombrar(simple.valor());
		ASSERT_TRUE(n.ok());
		if (n.ok())
		{
			ASSERT_TRUE(n.valor().categoria == CategoriaCompuesto::OXIDO);
		}
	}
}
