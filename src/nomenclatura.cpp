#include "nomenclatura.h"
#include <cstdio>
#include <cstring>

static int maximoComunDivisor(int a, int b)
{
	while (b != 0)
	{
		int temp = b;
		b = a % b;
		a = temp;
	}
	return a;
}

// Convierte un entero de 1 a 4 a número romano (suficiente para valencias químicas usuales).
static const char *aRomano(int numero)
{
	switch (numero)
	{
		case 1: return "I";
		case 2: return "II";
		case 3: return "III";
		case 4: return "IV";
		case 5: return "V";
		case 6: return "VI";
		case 7: return "VII";
		default: return "?";
	}
}

// Separa una fórmula de dos componentes en un elemento y su oxígeno.
// Devuelve false si la fórmula no tiene exactamente esa forma (elemento + "O").
// Sirve tanto para metal+O (óxidos, peróxidos) como para no metal+O (anhídridos).
static bool separarElementoYOxigeno(const FormulaParseada &formula,
                                     const ComponenteFormula *&elemento,
                                     const ComponenteFormula *&oxigeno)
{
	elemento = nullptr;
	oxigeno = nullptr;

	if (formula.cantidadComponentes != 2)
	{
		return false;
	}

	for (int i = 0; i < formula.cantidadComponentes; i++)
	{
		if (std::strcmp(formula.componentes[i].simbolo, "O") == 0)
		{
			oxigeno = &formula.componentes[i];
		}
		else
		{
			elemento = &formula.componentes[i];
		}
	}

	return elemento != nullptr && oxigeno != nullptr;
}

ResultadoNomenclatura nomenclaturaStockOxido(const FormulaParseada &formula, char resultado[TAM_MAX])
{
	resultado[0] = '\0';

	const ComponenteFormula *metal = nullptr;
	const ComponenteFormula *oxigeno = nullptr;
	if (!separarElementoYOxigeno(formula, metal, oxigeno))
	{
		return ResultadoNomenclatura::NO_ES_OXIDO;
	}

	const InfoElemento *infoMetal = buscarElemento(metal->simbolo);
	if (infoMetal == nullptr)
	{
		return ResultadoNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	// El oxígeno siempre actúa con valencia 2 en óxidos. Por notación de
	// intercambio de valencias, MxOy con valencia(M) = v produce, antes de
	// reducir, subíndices (x=2, y=v) que luego se dividen por su mcd. Se
	// prueba cada valencia conocida del metal y se acepta la que reproduzca
	// exactamente los subíndices de la fórmula ingresada (ya reducida o no).
	constexpr int VALENCIA_OXIGENO = 2;
	int valenciaDeducida = -1;

	for (int i = 0; i < infoMetal->cantidadValencias; i++)
	{
		int v = infoMetal->valencias[i];
		int mcd = maximoComunDivisor(VALENCIA_OXIGENO, v);
		int xEsperado = VALENCIA_OXIGENO / mcd; // subíndice del metal
		int yEsperado = v / mcd;                // subíndice del oxígeno

		if (metal->subindice == xEsperado && oxigeno->subindice == yEsperado)
		{
			valenciaDeducida = v;
			break;
		}
	}

	if (valenciaDeducida == -1)
	{
		return ResultadoNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	if (infoMetal->cantidadValencias == 1)
	{
		// Elementos con una sola valencia no llevan número romano (convención Stock).
		std::snprintf(resultado, TAM_MAX, "oxido de %s", infoMetal->nombre);
	}
	else
	{
		std::snprintf(resultado, TAM_MAX, "oxido de %s (%s)", infoMetal->nombre, aRomano(valenciaDeducida));
	}

	return ResultadoNomenclatura::OK;
}

ResultadoNomenclatura nomenclaturaStockPeroxido(const FormulaParseada &formula, char resultado[TAM_MAX])
{
	resultado[0] = '\0';

	const ComponenteFormula *metal = nullptr;
	const ComponenteFormula *oxigeno = nullptr;
	if (!separarElementoYOxigeno(formula, metal, oxigeno))
	{
		return ResultadoNomenclatura::NO_ES_PEROXIDO;
	}

	const InfoElemento *infoMetal = buscarElemento(metal->simbolo);
	if (infoMetal == nullptr)
	{
		return ResultadoNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	// Un peróxido siempre contiene el grupo peroxo (O2)^2-, cada oxígeno del
	// grupo aporta valencia -1 (no -2 como en un óxido normal). Por lo tanto
	// la carga se equilibra cuando subindice_metal * valencia_metal =
	// subindice_oxigeno * 1, es decir subindice_O = subindice_metal * v.
	// A diferencia del óxido, aquí el subíndice de O no se reduce junto con
	// el del metal porque el grupo (O2) es una unidad indivisible: por eso se
	// compara la fórmula "tal cual" contra ese producto, no una versión
	// reducida por mcd.
	int valenciaDeducida = -1;

	for (int i = 0; i < infoMetal->cantidadValencias; i++)
	{
		int v = infoMetal->valencias[i];
		if (oxigeno->subindice == metal->subindice * v)
		{
			valenciaDeducida = v;
			break;
		}
	}

	if (valenciaDeducida == -1)
	{
		return ResultadoNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	std::snprintf(resultado, TAM_MAX, "peroxido de %s", infoMetal->nombre);
	return ResultadoNomenclatura::OK;
}

// Arma el nombre tradicional ("hipo-...-oso", "-oso", "-ico", "per-...-ico")
// a partir de la raíz del no metal y la posición (0-indexada) que ocupa la
// valencia usada dentro de la lista ordenada de valencias del no metal.
static void formatearNombreTradicional(const char *raiz, int posicion, int totalValencias, char resultado[TAM_MAX])
{
	const char *prefijo = "";
	const char *sufijo = "ico";

	if (totalValencias == 1)
	{
		// Un no metal con una sola valencia siempre usa el sufijo -ico.
		sufijo = "ico";
	}
	else if (totalValencias == 2)
	{
		sufijo = (posicion == 0) ? "oso" : "ico";
	}
	else if (totalValencias == 3)
	{
		if (posicion == 0) { prefijo = "hipo"; sufijo = "oso"; }
		else if (posicion == 1) { sufijo = "oso"; }
		else { sufijo = "ico"; }
	}
	else // 4 valencias
	{
		if (posicion == 0) { prefijo = "hipo"; sufijo = "oso"; }
		else if (posicion == 1) { sufijo = "oso"; }
		else if (posicion == 2) { sufijo = "ico"; }
		else { prefijo = "per"; sufijo = "ico"; }
	}

	std::snprintf(resultado, TAM_MAX, "anhidrido %s%s%s", prefijo, raiz, sufijo);
}

ResultadoNomenclatura nomenclaturaTradicionalAnhidrido(const FormulaParseada &formula, char resultado[TAM_MAX])
{
	resultado[0] = '\0';

	const ComponenteFormula *noMetalComp = nullptr;
	const ComponenteFormula *oxigeno = nullptr;
	if (!separarElementoYOxigeno(formula, noMetalComp, oxigeno))
	{
		return ResultadoNomenclatura::NO_ES_ANHIDRIDO;
	}

	const InfoNoMetal *infoNoMetal = buscarNoMetal(noMetalComp->simbolo);
	if (infoNoMetal == nullptr)
	{
		return ResultadoNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	// Mismo principio de intercambio de valencias que en óxidos: el oxígeno
	// actúa con valencia 2, y se prueba cada valencia conocida del no metal
	// hasta encontrar la que reproduce los subíndices de la fórmula ingresada.
	constexpr int VALENCIA_OXIGENO = 2;
	int posicionValencia = -1;

	for (int i = 0; i < infoNoMetal->cantidadValencias; i++)
	{
		int v = infoNoMetal->valencias[i];
		int mcd = maximoComunDivisor(VALENCIA_OXIGENO, v);
		int xEsperado = VALENCIA_OXIGENO / mcd; // subíndice del no metal
		int yEsperado = v / mcd;                // subíndice del oxígeno

		if (noMetalComp->subindice == xEsperado && oxigeno->subindice == yEsperado)
		{
			posicionValencia = i;
			break;
		}
	}

	if (posicionValencia == -1)
	{
		return ResultadoNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	formatearNombreTradicional(infoNoMetal->raiz, posicionValencia, infoNoMetal->cantidadValencias, resultado);
	return ResultadoNomenclatura::OK;
}

ResultadoNomenclatura nomenclaturaTradicionalHidracido(const FormulaParseada &formula, char resultado[TAM_MAX])
{
	resultado[0] = '\0';

	if (formula.cantidadComponentes != 2)
	{
		return ResultadoNomenclatura::NO_ES_HIDRACIDO;
	}

	const ComponenteFormula *hidrogeno = nullptr;
	const ComponenteFormula *noMetalComp = nullptr;

	for (int i = 0; i < formula.cantidadComponentes; i++)
	{
		if (std::strcmp(formula.componentes[i].simbolo, "H") == 0)
		{
			hidrogeno = &formula.componentes[i];
		}
		else
		{
			noMetalComp = &formula.componentes[i];
		}
	}

	if (hidrogeno == nullptr || noMetalComp == nullptr)
	{
		return ResultadoNomenclatura::NO_ES_HIDRACIDO;
	}

	const InfoHidracido *infoHidracido = buscarHidracido(noMetalComp->simbolo);
	if (infoHidracido == nullptr)
	{
		return ResultadoNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	// A diferencia de óxidos/anhídridos, un hidrácido no tiene varias
	// valencias posibles: el no metal siempre actúa con su única valencia
	// negativa como anión, así que el subíndice de H debe coincidir
	// exactamente (sin reducir, ya que H2S no se simplifica a HS).
	if (hidrogeno->subindice != infoHidracido->subindiceHidrogeno || noMetalComp->subindice != 1)
	{
		return ResultadoNomenclatura::NO_ES_HIDRACIDO;
	}

	std::snprintf(resultado, TAM_MAX, "acido %shidrico", infoHidracido->raiz);
	return ResultadoNomenclatura::OK;
}

ResultadoNomenclatura nomenclaturaTradicionalOxacido(const FormulaParseada &formula, char resultado[TAM_MAX])
{
	resultado[0] = '\0';

	if (formula.cantidadComponentes != 3)
	{
		return ResultadoNomenclatura::NO_ES_OXACIDO;
	}

	const ComponenteFormula *hidrogeno = nullptr;
	const ComponenteFormula *noMetalComp = nullptr;
	const ComponenteFormula *oxigeno = nullptr;

	for (int i = 0; i < formula.cantidadComponentes; i++)
	{
		const char *simbolo = formula.componentes[i].simbolo;
		if (std::strcmp(simbolo, "H") == 0)
		{
			hidrogeno = &formula.componentes[i];
		}
		else if (std::strcmp(simbolo, "O") == 0)
		{
			oxigeno = &formula.componentes[i];
		}
		else
		{
			noMetalComp = &formula.componentes[i];
		}
	}

	if (hidrogeno == nullptr || noMetalComp == nullptr || oxigeno == nullptr)
	{
		return ResultadoNomenclatura::NO_ES_OXACIDO;
	}

	if (buscarNoMetal(noMetalComp->simbolo) == nullptr)
	{
		return ResultadoNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	const InfoOxacido *infoOxacido = buscarOxacido(noMetalComp->simbolo, hidrogeno->subindice,
	                                                noMetalComp->subindice, oxigeno->subindice);
	if (infoOxacido == nullptr)
	{
		return ResultadoNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	std::strncpy(resultado, infoOxacido->nombre, TAM_MAX - 1);
	resultado[TAM_MAX - 1] = '\0';
	return ResultadoNomenclatura::OK;
}

const char *mensajeError(ResultadoNomenclatura resultado)
{
	switch (resultado)
	{
		case ResultadoNomenclatura::OK:
			return "Sin errores.";
		case ResultadoNomenclatura::FORMULA_INVALIDA:
			return "La formula no es quimicamente valida.";
		case ResultadoNomenclatura::NO_ES_OXIDO:
			return "La formula no corresponde a un oxido (se esperaba metal + oxigeno).";
		case ResultadoNomenclatura::NO_ES_PEROXIDO:
			return "La formula no corresponde a un peroxido (se esperaba metal + grupo peroxo O2).";
		case ResultadoNomenclatura::NO_ES_ANHIDRIDO:
			return "La formula no corresponde a un anhidrido (se esperaba no metal + oxigeno).";
		case ResultadoNomenclatura::NO_ES_HIDRACIDO:
			return "La formula no corresponde a un acido hidracido (se esperaba H + no metal en la proporcion correcta).";
		case ResultadoNomenclatura::NO_ES_OXACIDO:
			return "La formula no corresponde a un acido oxacido (se esperaba H + no metal + oxigeno).";
		case ResultadoNomenclatura::ELEMENTO_DESCONOCIDO:
			return "El elemento de la formula no esta en la tabla de elementos/no metales soportados.";
		case ResultadoNomenclatura::VALENCIA_NO_DETERMINADA:
			return "La proporcion de la formula no corresponde a ninguna valencia conocida del metal.";
	}
	return "Error desconocido.";
}
