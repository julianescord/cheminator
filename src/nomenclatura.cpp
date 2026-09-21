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

// Separa una fórmula de dos componentes en su parte metálica y su oxígeno.
// Devuelve false si la fórmula no tiene exactamente esa forma (metal + "O").
static bool separarMetalYOxigeno(const FormulaParseada &formula,
                                  const ComponenteFormula *&metal,
                                  const ComponenteFormula *&oxigeno)
{
	metal = nullptr;
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
			metal = &formula.componentes[i];
		}
	}

	return metal != nullptr && oxigeno != nullptr;
}

ResultadoNomenclatura nomenclaturaStockOxido(const FormulaParseada &formula, char resultado[TAM_MAX])
{
	resultado[0] = '\0';

	const ComponenteFormula *metal = nullptr;
	const ComponenteFormula *oxigeno = nullptr;
	if (!separarMetalYOxigeno(formula, metal, oxigeno))
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
	if (!separarMetalYOxigeno(formula, metal, oxigeno))
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
		case ResultadoNomenclatura::ELEMENTO_DESCONOCIDO:
			return "El elemento metalico de la formula no esta en la tabla de elementos soportados.";
		case ResultadoNomenclatura::VALENCIA_NO_DETERMINADA:
			return "La proporcion de la formula no corresponde a ninguna valencia conocida del metal.";
	}
	return "Error desconocido.";
}
