#include "cheminator/formula.hpp"
#include "cheminator/nomenclatura.hpp"
#include "test_runner.h"

using namespace cheminator;

namespace {

using Nombrador = ResultadoNomenclatura (*)(const Formula &);

// Comprueba que una formula recibe el nombre esperado con el nombrador dado.
void verificar(Nombrador nombrar, std::string_view texto, std::string_view nombreEsperado)
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
		std::fprintf(stderr, "  (al nombrar \"%.*s\")\n", static_cast<int>(texto.size()), texto.data());
		return;
	}

	ASSERT_EQ_STR(resultado.valor().nombre, nombreEsperado);
	// Todo nombre correcto viene acompanado de su razonamiento.
	ASSERT_TRUE(!resultado.valor().pasos.empty());
}

void verificarError(Nombrador nombrar, std::string_view texto, ErrorNomenclatura esperado)
{
	const auto analizada = parsearFormula(texto);
	ASSERT_TRUE(analizada.ok());
	if (!analizada.ok())
	{
		return;
	}

	const auto resultado = nombrar(analizada.valor());
	ASSERT_TRUE(!resultado.ok());
	if (!resultado.ok() && resultado.error() != esperado)
	{
		const std::string_view obtenido = mensajeError(resultado.error());
		std::fprintf(stderr, "FALLO: \"%.*s\" dio el error \"%.*s\", que no es el esperado.\n",
		             static_cast<int>(texto.size()), texto.data(), static_cast<int>(obtenido.size()),
		             obtenido.data());
		g_pruebasFallidas++;
	}
}

} // namespace

void test_nomenclatura_oxidos()
{
	// Metales de una sola valencia: sin numero romano.
	verificar(nombrarOxido, "Na2O", "oxido de Sodio");
	verificar(nombrarOxido, "CaO", "oxido de Calcio");
	verificar(nombrarOxido, "Al2O3", "oxido de Aluminio");

	// Metales de valencia multiple: con numero romano.
	verificar(nombrarOxido, "FeO", "oxido de Hierro (II)");
	verificar(nombrarOxido, "Fe2O3", "oxido de Hierro (III)");
	verificar(nombrarOxido, "CuO", "oxido de Cobre (II)");
	verificar(nombrarOxido, "Cu2O", "oxido de Cobre (I)");
	verificar(nombrarOxido, "PbO2", "oxido de Plomo (IV)");

	verificarError(nombrarOxido, "NaCl", ErrorNomenclatura::NO_ES_OXIDO);
	verificarError(nombrarOxido, "XxO", ErrorNomenclatura::ELEMENTO_DESCONOCIDO);
	verificarError(nombrarOxido, "Fe5O2", ErrorNomenclatura::VALENCIA_NO_DETERMINADA);
}

void test_nomenclatura_peroxidos()
{
	// Metales de valencia 1: el grupo peroxo no se reduce, subindices 2:2.
	verificar(nombrarPeroxido, "Na2O2", "peroxido de Sodio");
	verificar(nombrarPeroxido, "H2O2", "peroxido de Hidrogeno");
	verificar(nombrarPeroxido, "K2O2", "peroxido de Potasio");

	// Metales de valencia 2: subindices 1:2.
	verificar(nombrarPeroxido, "CaO2", "peroxido de Calcio");

	// El cobre forma peroxido con cualquiera de sus dos valencias.
	verificar(nombrarPeroxido, "Cu2O2", "peroxido de Cobre");
	verificar(nombrarPeroxido, "CuO2", "peroxido de Cobre");

	verificarError(nombrarPeroxido, "NaCl", ErrorNomenclatura::NO_ES_PEROXIDO);
	verificarError(nombrarPeroxido, "CaO", ErrorNomenclatura::VALENCIA_NO_DETERMINADA);
	verificarError(nombrarPeroxido, "XxO2", ErrorNomenclatura::ELEMENTO_DESCONOCIDO);
}

void test_nomenclatura_anhidridos()
{
	verificar(nombrarAnhidrido, "CO2", "anhidrido carbonico");
	verificar(nombrarAnhidrido, "P2O3", "anhidrido fosforoso");
	verificar(nombrarAnhidrido, "P2O5", "anhidrido fosforico");

	// Tres valencias: hipo-/-oso, -oso, -ico.
	verificar(nombrarAnhidrido, "N2O", "anhidrido hiponitroso");
	verificar(nombrarAnhidrido, "N2O3", "anhidrido nitroso");
	verificar(nombrarAnhidrido, "N2O5", "anhidrido nitrico");
	verificar(nombrarAnhidrido, "SO2", "anhidrido sulfuroso");
	verificar(nombrarAnhidrido, "SO3", "anhidrido sulfurico");

	// Cuatro valencias: se suma per-/-ico.
	verificar(nombrarAnhidrido, "Cl2O", "anhidrido hipocloroso");
	verificar(nombrarAnhidrido, "Cl2O3", "anhidrido cloroso");
	verificar(nombrarAnhidrido, "Cl2O5", "anhidrido clorico");
	verificar(nombrarAnhidrido, "Cl2O7", "anhidrido perclorico");

	verificarError(nombrarAnhidrido, "NaCl", ErrorNomenclatura::NO_ES_ANHIDRIDO);
	verificarError(nombrarAnhidrido, "FeO", ErrorNomenclatura::ELEMENTO_DESCONOCIDO);
	// El nitrogeno con valencia 2 (NO) no esta tabulado a proposito.
	verificarError(nombrarAnhidrido, "NO", ErrorNomenclatura::VALENCIA_NO_DETERMINADA);
}

void test_nomenclatura_hidracidos()
{
	verificar(nombrarHidracido, "HCl", "acido clorhidrico");
	verificar(nombrarHidracido, "HF", "acido fluorhidrico");
	verificar(nombrarHidracido, "HBr", "acido bromhidrico");
	verificar(nombrarHidracido, "HI", "acido iodhidrico");

	verificar(nombrarHidracido, "H2S", "acido sulfhidrico");
	verificar(nombrarHidracido, "H2Se", "acido selenhidrico");
	verificar(nombrarHidracido, "H2Te", "acido telurhidrico");

	verificarError(nombrarHidracido, "NaCl", ErrorNomenclatura::NO_ES_HIDRACIDO);
	verificarError(nombrarHidracido, "H2O", ErrorNomenclatura::ELEMENTO_DESCONOCIDO);
	verificarError(nombrarHidracido, "H3S", ErrorNomenclatura::NO_ES_HIDRACIDO);
	verificarError(nombrarHidracido, "HCl2", ErrorNomenclatura::NO_ES_HIDRACIDO);
}

void test_nomenclatura_oxacidos()
{
	verificar(nombrarOxacido, "H2SO4", "acido sulfurico");
	verificar(nombrarOxacido, "H2SO3", "acido sulfuroso");
	verificar(nombrarOxacido, "HNO3", "acido nitrico");
	verificar(nombrarOxacido, "HNO2", "acido nitroso");
	verificar(nombrarOxacido, "H2CO3", "acido carbonico");
	verificar(nombrarOxacido, "HClO", "acido hipocloroso");
	verificar(nombrarOxacido, "HClO4", "acido perclorico");
	verificar(nombrarOxacido, "HBrO3", "acido bromico");
	verificar(nombrarOxacido, "HIO2", "acido iodoso");

	// El fosforo no sigue la regla simple de intercambio (H3PO3/H3PO4, no
	// HPO2/HPO3): por eso los oxacidos estan tabulados y no derivados.
	verificar(nombrarOxacido, "H3PO3", "acido fosforoso");
	verificar(nombrarOxacido, "H3PO4", "acido fosforico");

	verificarError(nombrarOxacido, "HCl", ErrorNomenclatura::NO_ES_OXACIDO);
	// Sin hidrogeno no hay oxacido, por mas que el resto encaje.
	verificarError(nombrarOxacido, "NaClO3", ErrorNomenclatura::NO_ES_OXACIDO);
	verificarError(nombrarOxacido, "H4SO4", ErrorNomenclatura::VALENCIA_NO_DETERMINADA);
}

void test_nomenclatura_bases()
{
	// Forma con parentesis.
	verificar(nombrarBase, "Ca(OH)2", "hidroxido de Calcio");
	verificar(nombrarBase, "Al(OH)3", "hidroxido de Aluminio");
	verificar(nombrarBase, "Fe(OH)2", "hidroxido de Hierro (II)");
	verificar(nombrarBase, "Fe(OH)3", "hidroxido de Hierro (III)");
	verificar(nombrarBase, "Cu(OH)2", "hidroxido de Cobre (II)");

	// Forma sin parentesis, que el parser entrega como tres componentes.
	verificar(nombrarBase, "NaOH", "hidroxido de Sodio");
	verificar(nombrarBase, "KOH", "hidroxido de Potasio");

	verificarError(nombrarBase, "NaCl", ErrorNomenclatura::NO_ES_BASE);
	verificarError(nombrarBase, "Na(OH)2", ErrorNomenclatura::VALENCIA_NO_DETERMINADA);
	verificarError(nombrarBase, "Fe(OH)5", ErrorNomenclatura::VALENCIA_NO_DETERMINADA);
	verificarError(nombrarBase, "XxOH", ErrorNomenclatura::ELEMENTO_DESCONOCIDO);
}

void test_nomenclatura_sales()
{
	// Forma con parentesis.
	verificar(nombrarSal, "Al2(SO4)3", "sulfato de Aluminio");
	verificar(nombrarSal, "Ca(NO3)2", "nitrato de Calcio");
	verificar(nombrarSal, "Ca3(PO4)2", "fosfato de Calcio");
	verificar(nombrarSal, "Fe2(SO4)3", "sulfato de Hierro (III)");

	// Forma sin parentesis (un solo grupo del radical).
	verificar(nombrarSal, "Na2SO4", "sulfato de Sodio");
	verificar(nombrarSal, "CaCO3", "carbonato de Calcio");
	verificar(nombrarSal, "NaNO3", "nitrato de Sodio");
	verificar(nombrarSal, "Na2CO3", "carbonato de Sodio");
	verificar(nombrarSal, "Na2SO3", "sulfito de Sodio");
	verificar(nombrarSal, "FeSO4", "sulfato de Hierro (II)");

	// Radical con un solo oxigeno: se escribe "ClO", no "ClO1".
	verificar(nombrarSal, "NaClO", "hipoclorito de Sodio");
	verificar(nombrarSal, "KClO", "hipoclorito de Potasio");

	// "Cl" es un elemento, no un radical poliatomico: la formula simplemente
	// no es una sal oxisal (es una sal binaria, fuera del alcance).
	verificarError(nombrarSal, "NaCl", ErrorNomenclatura::NO_ES_SAL);
	verificarError(nombrarSal, "CaO", ErrorNomenclatura::NO_ES_SAL);
	// SO5 si tiene forma de radical, pero no esta en la tabla.
	verificarError(nombrarSal, "Na2SO5", ErrorNomenclatura::ELEMENTO_DESCONOCIDO);
	verificarError(nombrarSal, "Fe2(SO4)5", ErrorNomenclatura::VALENCIA_NO_DETERMINADA);
}
