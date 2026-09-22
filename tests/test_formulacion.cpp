#include "test_runner.h"

#include "cheminator/formula.hpp"
#include "cheminator/formulacion.hpp"
#include "cheminator/nomenclatura.hpp"

#include <string>

using namespace cheminator;

namespace {

// Comprueba que un nombre da exactamente la formula esperada.
bool formulaEs(const char *nombre, const char *esperada)
{
	const ResultadoFormulacion resultado = formular(nombre);
	if (!resultado)
	{
		return false;
	}
	return resultado.valor().formula == esperada;
}

bool errorEs(const char *nombre, ErrorFormulacion esperado)
{
	const ResultadoFormulacion resultado = formular(nombre);
	return !resultado && resultado.error() == esperado;
}

} // namespace

void test_formulacion()
{
	// --- Una por categoria, en el mismo orden que el menu --------------------

	ASSERT_TRUE(formulaEs("oxido de Hierro (III)", "Fe2O3"));  // oxido con valencia indicada
	ASSERT_TRUE(formulaEs("oxido de Sodio", "Na2O"));  // oxido de valencia unica
	ASSERT_TRUE(formulaEs("peroxido de Sodio", "Na2O2"));  // peroxido de metal monovalente
	ASSERT_TRUE(formulaEs("anhidrido sulfurico", "SO3"));  // anhidrido
	ASSERT_TRUE(formulaEs("acido clorhidrico", "HCl"));  // acido hidracido
	ASSERT_TRUE(formulaEs("acido sulfurico", "H2SO4"));  // acido oxacido
	ASSERT_TRUE(formulaEs("hidroxido de Calcio", "Ca(OH)2"));  // base
	ASSERT_TRUE(formulaEs("sulfato de Aluminio", "Al2(SO4)3"));  // sal oxisal

	// El grupo peroxido se cruza con la valencia del metal en vez de fijar el
	// metal en 2. Con un 2 fijo, CaO2 salia como Ca2O4.
	ASSERT_TRUE(formulaEs("peroxido de Calcio", "CaO2"));  // peroxido de metal divalente

	// --- Normalizacion de la entrada ----------------------------------------

	ASSERT_TRUE(formulaEs("Óxido de Hierro (III)", "Fe2O3"));  // se aceptan tildes
	ASSERT_TRUE(formulaEs("OXIDO DE HIERRO (III)", "Fe2O3"));  // se aceptan mayusculas
	ASSERT_TRUE(formulaEs("oxido de hierro (iii)", "Fe2O3"));  // se acepta el romano en minuscula
	ASSERT_TRUE(formulaEs("   oxido   de   Hierro   (III)   ", "Fe2O3"));  // se colapsan los espacios

	// --- Errores -------------------------------------------------------------

	ASSERT_TRUE(errorEs("", ErrorFormulacion::VACIO));  // nombre vacio
	ASSERT_TRUE(errorEs("   ", ErrorFormulacion::VACIO));  // solo espacios

	// Sin numero romano no se puede saber de que oxido de hierro se habla, y
	// adivinar daria una formula distinta de la que espera quien pregunta.
	ASSERT_TRUE(errorEs("oxido de Hierro", ErrorFormulacion::FALTA_VALENCIA));  // falta la valencia en un metal con varias
	ASSERT_TRUE(errorEs("oxido de Hierro (VII)", ErrorFormulacion::VALENCIA_INVALIDA));  // valencia que el elemento no admite
	ASSERT_TRUE(errorEs("oxido de Kryptonita", ErrorFormulacion::ELEMENTO_DESCONOCIDO));  // elemento fuera de las tablas
	ASSERT_TRUE(errorEs("fosgenato de Sodio", ErrorFormulacion::RADICAL_DESCONOCIDO));  // radical fuera de la tabla
	ASSERT_TRUE(errorEs("compuesto raro", ErrorFormulacion::CATEGORIA_DESCONOCIDA));  // nombre que no encaja en ninguna categoria

	// --- Raiz que cambia segun el sufijo -------------------------------------

	// El arsenico y el selenio llevan una i ante -oso y no ante -ico. Con una
	// sola raiz, la valencia mayor salia como "arseniico" y "seleniico".
	ASSERT_TRUE(formulaEs("anhidrido arsenioso", "As2O3"));
	ASSERT_TRUE(formulaEs("anhidrido arsenico", "As2O5"));
	ASSERT_TRUE(formulaEs("anhidrido selenioso", "SeO2"));
	ASSERT_TRUE(formulaEs("anhidrido selenico", "SeO3"));

	// El azufre comparte raiz para los dos sufijos, que es el caso normal.
	ASSERT_TRUE(formulaEs("anhidrido sulfuroso", "SO2"));
	ASSERT_TRUE(formulaEs("anhidrido sulfurico", "SO3"));

	// --- Complejos: se reconocen, aunque todavia no se formulan --------------

	// Decir "radical desconocido" ante el nombre de un complejo mandaba a
	// buscar el problema donde no estaba.
	ASSERT_TRUE(errorEs("hexacianoferrato (III) de potasio", ErrorFormulacion::COMPLEJO_NO_SOPORTADO));
	ASSERT_TRUE(errorEs("tetraamincobre (II)", ErrorFormulacion::COMPLEJO_NO_SOPORTADO));

	// Una sal con numero romano no es un complejo y debe seguir formulandose:
	// es lo que distingue el prefijo multiplicador pegado a un ligando de un
	// radical tabulado.
	ASSERT_TRUE(formulaEs("sulfato de Hierro (III)", "Fe2(SO4)3"));
	ASSERT_TRUE(formulaEs("carbonato de Bario", "BaCO3"));

	// --- Normalizacion, en aislado -------------------------------------------

	// El resultado se guarda en una variable antes de comparar: pasar
	// normalizarNombre(...).c_str() directo a la macro deja un puntero a un
	// temporal ya destruido.
	const std::string conEspacios = normalizarNombre("  Óxido   DE  Hierro  ");
	ASSERT_EQ_STR(conEspacios.c_str(), "oxido de hierro");

	const std::string conTildes = normalizarNombre("Ácido Sulfúrico");
	ASSERT_EQ_STR(conTildes.c_str(), "acido sulfurico");
}

// Ida y vuelta: se parte de una formula, se la nombra, y el nombre se vuelve a
// formular. Es la prueba mas valiosa del modulo, porque recorre las dos
// direcciones y detecta sola cualquier desincronizacion entre ellas: si se
// toca una tabla o la regla de afijos y solo se actualiza un sentido, esto
// falla sin que nadie tenga que acordarse de escribir un caso nuevo.
void test_ida_y_vuelta()
{
	static constexpr const char *FORMULAS[] = {
		// oxidos
		"Fe2O3", "Na2O", "CuO", "Al2O3", "CaO", "PbO2", "SnO2", "Au2O3",
		// peroxidos
		"Na2O2", "CaO2", "H2O2",
		// anhidridos
		"SO3", "SO2", "CO2", "N2O5", "Cl2O7", "P2O5",
		// hidracidos
		"HCl", "H2S", "HBr", "HI", "HF",
		// oxacidos
		"H2SO4", "HNO3", "H3PO4", "H2CO3", "HClO4",
		// bases
		"NaOH", "Ca(OH)2", "Fe(OH)3", "Al(OH)3",
		// sales oxisal
		"Na2SO4", "Al2(SO4)3", "CaCO3", "KNO3", "Fe2(SO4)3", "NaClO",

		// Elementos anadidos despues de la primera version de las tablas. Se
		// incluyen aqui y no en una prueba aparte para que cualquier fila
		// nueva quede cubierta en las dos direcciones desde el principio.
		"BaO", "SrO", "BeO", "Rb2O", "Cs2O", "RaO", "CdO",
		"NiO", "Ni2O3", "CoO", "Co2O3", "CrO", "Cr2O3", "CrO3",
		"MnO", "Mn2O7", "HgO", "Hg2O", "PtO", "PtO2", "TiO2", "Ti2O3",
		"B2O3", "SiO2", "As2O3", "As2O5", "Sb2O3", "SeO2", "SeO3", "TeO3",
		"H3BO3", "H4SiO4", "H3AsO4", "H3SbO3", "H2SeO4", "H2TeO3",
		"Ba(OH)2", "Ni(OH)2", "Cr(OH)3", "Mn(OH)2",
		"BaCO3", "Ni2(SO4)3", "K3AsO4", "Na2SeO4",
	};

	for (const char *original : FORMULAS)
	{
		const auto analizada = parsearFormula(original);
		ASSERT_TRUE(static_cast<bool>(analizada));
		if (!analizada) continue;

		const ResultadoNomenclatura nombrada = nombrar(analizada.valor());
		ASSERT_TRUE(static_cast<bool>(nombrada));
		if (!nombrada) continue;

		const ResultadoFormulacion vuelta = formular(nombrada.valor().nombre);
		ASSERT_TRUE(static_cast<bool>(vuelta));
		if (!vuelta) continue;

		ASSERT_EQ_STR(vuelta.valor().formula.c_str(), original);
	}
}
