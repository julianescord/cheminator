#include "nomenclatura.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>

void agregarPaso(Explicacion *explicacion, const char *formato, ...)
{
	if (explicacion == nullptr || explicacion->cantidadPasos >= MAX_PASOS_EXPLICACION)
	{
		return;
	}

	va_list argumentos;
	va_start(argumentos, formato);
	std::vsnprintf(explicacion->pasos[explicacion->cantidadPasos], TAM_MAX_PASO, formato, argumentos);
	va_end(argumentos);

	explicacion->cantidadPasos++;
}

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

ResultadoNomenclatura nomenclaturaStockOxido(const FormulaParseada &formula, char resultado[TAM_MAX],
                                              Explicacion *explicacion)
{
	resultado[0] = '\0';

	const ComponenteFormula *metal = nullptr;
	const ComponenteFormula *oxigeno = nullptr;
	if (!separarElementoYOxigeno(formula, metal, oxigeno))
	{
		return ResultadoNomenclatura::NO_ES_OXIDO;
	}

	agregarPaso(explicacion, "Se identifica el metal (%s, subindice %d) y el oxigeno (subindice %d).",
	            metal->simbolo, metal->subindice, oxigeno->subindice);

	const InfoElemento *infoMetal = buscarElemento(metal->simbolo);
	if (infoMetal == nullptr)
	{
		return ResultadoNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	agregarPaso(explicacion, "%s (%s) tiene %d valencia(s) conocida(s) para oxidos.",
	            infoMetal->nombre, metal->simbolo, infoMetal->cantidadValencias);

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
		agregarPaso(explicacion, "Ninguna valencia conocida de %s reproduce la proporcion %d:%d (metal:oxigeno).",
		            infoMetal->nombre, metal->subindice, oxigeno->subindice);
		return ResultadoNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	agregarPaso(explicacion, "La proporcion %d:%d (metal:oxigeno) corresponde a la valencia %d.",
	            metal->subindice, oxigeno->subindice, valenciaDeducida);

	if (infoMetal->cantidadValencias == 1)
	{
		// Elementos con una sola valencia no llevan número romano (convención Stock).
		agregarPaso(explicacion, "%s tiene una unica valencia -> no se indica numero romano.", infoMetal->nombre);
		std::snprintf(resultado, TAM_MAX, "oxido de %s", infoMetal->nombre);
	}
	else
	{
		agregarPaso(explicacion, "%s tiene mas de una valencia -> se indica con numero romano (%s).",
		            infoMetal->nombre, aRomano(valenciaDeducida));
		std::snprintf(resultado, TAM_MAX, "oxido de %s (%s)", infoMetal->nombre, aRomano(valenciaDeducida));
	}

	return ResultadoNomenclatura::OK;
}

ResultadoNomenclatura nomenclaturaStockPeroxido(const FormulaParseada &formula, char resultado[TAM_MAX],
                                                 Explicacion *explicacion)
{
	resultado[0] = '\0';

	const ComponenteFormula *metal = nullptr;
	const ComponenteFormula *oxigeno = nullptr;
	if (!separarElementoYOxigeno(formula, metal, oxigeno))
	{
		return ResultadoNomenclatura::NO_ES_PEROXIDO;
	}

	agregarPaso(explicacion, "Se identifica el metal (%s, subindice %d) y el oxigeno (subindice %d).",
	            metal->simbolo, metal->subindice, oxigeno->subindice);

	const InfoElemento *infoMetal = buscarElemento(metal->simbolo);
	if (infoMetal == nullptr)
	{
		return ResultadoNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	agregarPaso(explicacion, "Se asume el grupo peroxo (O2)2-, donde cada oxigeno aporta valencia -1 (no -2 como en un oxido).");

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
		agregarPaso(explicacion, "Ninguna valencia conocida de %s reproduce subindice_O = subindice_metal * valencia.",
		            infoMetal->nombre);
		return ResultadoNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	agregarPaso(explicacion, "%d = %d * %d confirma la valencia %d para %s.",
	            oxigeno->subindice, metal->subindice, valenciaDeducida, valenciaDeducida, infoMetal->nombre);

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

ResultadoNomenclatura nomenclaturaTradicionalAnhidrido(const FormulaParseada &formula, char resultado[TAM_MAX],
                                                        Explicacion *explicacion)
{
	resultado[0] = '\0';

	const ComponenteFormula *noMetalComp = nullptr;
	const ComponenteFormula *oxigeno = nullptr;
	if (!separarElementoYOxigeno(formula, noMetalComp, oxigeno))
	{
		return ResultadoNomenclatura::NO_ES_ANHIDRIDO;
	}

	agregarPaso(explicacion, "Se identifica el no metal (%s, subindice %d) y el oxigeno (subindice %d).",
	            noMetalComp->simbolo, noMetalComp->subindice, oxigeno->subindice);

	const InfoNoMetal *infoNoMetal = buscarNoMetal(noMetalComp->simbolo);
	if (infoNoMetal == nullptr)
	{
		return ResultadoNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	agregarPaso(explicacion, "%s (%s) tiene %d valencia(s) conocida(s) para anhidridos.",
	            infoNoMetal->nombre, noMetalComp->simbolo, infoNoMetal->cantidadValencias);

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
		agregarPaso(explicacion, "Ninguna valencia conocida de %s reproduce la proporcion %d:%d (no metal:oxigeno).",
		            infoNoMetal->nombre, noMetalComp->subindice, oxigeno->subindice);
		return ResultadoNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	agregarPaso(explicacion, "La proporcion %d:%d corresponde a la valencia %d (posicion %d de %d en la lista ordenada).",
	            noMetalComp->subindice, oxigeno->subindice, infoNoMetal->valencias[posicionValencia],
	            posicionValencia + 1, infoNoMetal->cantidadValencias);

	formatearNombreTradicional(infoNoMetal->raiz, posicionValencia, infoNoMetal->cantidadValencias, resultado);

	agregarPaso(explicacion, "Segun la cantidad de valencias y la posicion, se arma el nombre con prefijo/sufijo tradicional: %s.",
	            resultado);

	return ResultadoNomenclatura::OK;
}

ResultadoNomenclatura nomenclaturaTradicionalHidracido(const FormulaParseada &formula, char resultado[TAM_MAX],
                                                        Explicacion *explicacion)
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

	agregarPaso(explicacion, "Se identifica el hidrogeno (subindice %d) y el no metal (%s, subindice %d).",
	            hidrogeno->subindice, noMetalComp->simbolo, noMetalComp->subindice);

	const InfoHidracido *infoHidracido = buscarHidracido(noMetalComp->simbolo);
	if (infoHidracido == nullptr)
	{
		return ResultadoNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	agregarPaso(explicacion, "%s forma hidracido con %d hidrogeno(s) (valencia negativa fija, sin otras opciones).",
	            noMetalComp->simbolo, infoHidracido->subindiceHidrogeno);

	// A diferencia de óxidos/anhídridos, un hidrácido no tiene varias
	// valencias posibles: el no metal siempre actúa con su única valencia
	// negativa como anión, así que el subíndice de H debe coincidir
	// exactamente (sin reducir, ya que H2S no se simplifica a HS).
	if (hidrogeno->subindice != infoHidracido->subindiceHidrogeno || noMetalComp->subindice != 1)
	{
		agregarPaso(explicacion, "La formula no coincide con la proporcion esperada (H%d%s).",
		            infoHidracido->subindiceHidrogeno, noMetalComp->simbolo);
		return ResultadoNomenclatura::NO_ES_HIDRACIDO;
	}

	std::snprintf(resultado, TAM_MAX, "acido %shidrico", infoHidracido->raiz);
	agregarPaso(explicacion, "El nombre se arma como 'acido' + raiz ('%s') + sufijo fijo 'hidrico'.", infoHidracido->raiz);

	return ResultadoNomenclatura::OK;
}

ResultadoNomenclatura nomenclaturaTradicionalOxacido(const FormulaParseada &formula, char resultado[TAM_MAX],
                                                      Explicacion *explicacion)
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

	agregarPaso(explicacion, "Se identifican H(%d), %s(%d) y O(%d) en la formula.",
	            hidrogeno->subindice, noMetalComp->simbolo, noMetalComp->subindice, oxigeno->subindice);

	if (buscarNoMetal(noMetalComp->simbolo) == nullptr)
	{
		return ResultadoNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	const InfoOxacido *infoOxacido = buscarOxacido(noMetalComp->simbolo, hidrogeno->subindice,
	                                                noMetalComp->subindice, oxigeno->subindice);
	if (infoOxacido == nullptr)
	{
		agregarPaso(explicacion, "La formula H%d%s%dO%d no coincide con ningun oxacido tabulado de %s.",
		            hidrogeno->subindice, noMetalComp->simbolo, noMetalComp->subindice, oxigeno->subindice,
		            noMetalComp->simbolo);
		return ResultadoNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	agregarPaso(explicacion, "Esa formula coincide exactamente con un oxacido conocido de %s (tabulado, no derivado).",
	            noMetalComp->simbolo);

	std::strncpy(resultado, infoOxacido->nombre, TAM_MAX - 1);
	resultado[TAM_MAX - 1] = '\0';
	return ResultadoNomenclatura::OK;
}

// Intenta reducir la fórmula ya parseada de una base a exactamente dos
// componentes lógicos: el metal y el grupo hidroxilo con su subíndice.
// Reconoce dos formas equivalentes que el parser puede entregar:
//   - Con paréntesis:    {"Ca",1}, {"OH",2}           (2 componentes)
//   - Sin paréntesis:    {"Na",1}, {"O",1}, {"H",1}    (3 componentes,
//     solo válida cuando hay un único grupo OH, es decir cuando el
//     subíndice de O y de H coinciden entre sí).
// Devuelve false si la fórmula no coincide con ninguno de los dos patrones.
static bool normalizarBase(const FormulaParseada &formula,
                            const ComponenteFormula *&metal,
                            int &subindiceHidroxilo)
{
	metal = nullptr;
	subindiceHidroxilo = -1;

	if (formula.cantidadComponentes == 2)
	{
		const ComponenteFormula *grupoOH = nullptr;
		for (int i = 0; i < formula.cantidadComponentes; i++)
		{
			if (std::strcmp(formula.componentes[i].simbolo, "OH") == 0)
			{
				grupoOH = &formula.componentes[i];
			}
			else
			{
				metal = &formula.componentes[i];
			}
		}

		if (metal != nullptr && grupoOH != nullptr)
		{
			subindiceHidroxilo = grupoOH->subindice;
			return true;
		}
		return false;
	}

	if (formula.cantidadComponentes == 3)
	{
		const ComponenteFormula *oxigeno = nullptr;
		const ComponenteFormula *hidrogeno = nullptr;
		metal = nullptr;

		for (int i = 0; i < formula.cantidadComponentes; i++)
		{
			const char *simbolo = formula.componentes[i].simbolo;
			if (std::strcmp(simbolo, "O") == 0)
			{
				oxigeno = &formula.componentes[i];
			}
			else if (std::strcmp(simbolo, "H") == 0)
			{
				hidrogeno = &formula.componentes[i];
			}
			else
			{
				metal = &formula.componentes[i];
			}
		}

		// Un único grupo OH exige que O y H tengan el mismo subíndice: en
		// "NaOH" ambos son 1; una fórmula como "Na2OH" no representa un
		// hidróxido válido (no forma dos grupos OH sin paréntesis).
		if (metal != nullptr && oxigeno != nullptr && hidrogeno != nullptr &&
		    oxigeno->subindice == hidrogeno->subindice)
		{
			subindiceHidroxilo = oxigeno->subindice;
			return true;
		}
		return false;
	}

	return false;
}

ResultadoNomenclatura nomenclaturaStockBase(const FormulaParseada &formula, char resultado[TAM_MAX],
                                             Explicacion *explicacion)
{
	resultado[0] = '\0';

	const ComponenteFormula *metal = nullptr;
	int subindiceHidroxilo = -1;
	if (!normalizarBase(formula, metal, subindiceHidroxilo))
	{
		return ResultadoNomenclatura::NO_ES_BASE;
	}

	agregarPaso(explicacion, "Se identifica el metal (%s) y %d grupo(s) hidroxilo (OH).",
	            metal->simbolo, subindiceHidroxilo);

	const InfoElemento *infoMetal = buscarElemento(metal->simbolo);
	if (infoMetal == nullptr)
	{
		return ResultadoNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	// El grupo hidroxilo (OH) siempre actúa con valencia 1. Igual que en
	// óxidos, la fórmula reducida Metal(OH)n corresponde a la valencia v del
	// metal cuando n = v (un metal de valencia v necesita v grupos OH de
	// valencia 1 para neutralizar su carga; no hay reducción posible porque
	// el grupo OH ya tiene valencia 1).
	int valenciaDeducida = -1;

	for (int i = 0; i < infoMetal->cantidadValencias; i++)
	{
		if (infoMetal->valencias[i] == subindiceHidroxilo)
		{
			valenciaDeducida = subindiceHidroxilo;
			break;
		}
	}

	if (valenciaDeducida == -1)
	{
		agregarPaso(explicacion, "Ninguna valencia conocida de %s es igual a %d grupos OH.",
		            infoMetal->nombre, subindiceHidroxilo);
		return ResultadoNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	agregarPaso(explicacion, "El grupo OH tiene valencia 1, asi que %d grupos = valencia %d de %s.",
	            subindiceHidroxilo, valenciaDeducida, infoMetal->nombre);

	if (infoMetal->cantidadValencias == 1)
	{
		std::snprintf(resultado, TAM_MAX, "hidroxido de %s", infoMetal->nombre);
	}
	else
	{
		agregarPaso(explicacion, "%s tiene mas de una valencia -> se indica con numero romano (%s).",
		            infoMetal->nombre, aRomano(valenciaDeducida));
		std::snprintf(resultado, TAM_MAX, "hidroxido de %s (%s)", infoMetal->nombre, aRomano(valenciaDeducida));
	}

	return ResultadoNomenclatura::OK;
}

// Intenta reducir la fórmula ya parseada de una sal a exactamente dos
// componentes lógicos: el metal y el radical con su subíndice (número de
// grupos). Reconoce dos formas equivalentes que el parser puede entregar:
//   - Con paréntesis: {"Al",2}, {"SO4",3}                (2 componentes)
//   - Sin paréntesis: {"Na",2}, {"S",1}, {"O",4}          (3 componentes,
//     solo válida para un único grupo de radical, ya que sin paréntesis
//     no puede escribirse un multiplicador de grupo).
// Para la forma sin paréntesis, reconstruye la fórmula del radical
// ("S" + subíndice de O, p.ej. "SO4") a partir del no metal y el oxígeno.
// Devuelve false si la fórmula no coincide con ninguno de los dos patrones.
static bool normalizarSal(const FormulaParseada &formula,
                           const ComponenteFormula *&metal,
                           char formulaRadical[TAM_MAX],
                           int &subindiceRadical)
{
	metal = nullptr;
	formulaRadical[0] = '\0';
	subindiceRadical = -1;

	if (formula.cantidadComponentes == 2)
	{
		const ComponenteFormula *radicalComp = nullptr;
		for (int i = 0; i < formula.cantidadComponentes; i++)
		{
			// Un radical siempre viene de un grupo entre paréntesis, cuyo
			// símbolo el parser entrega con más de un carácter (p.ej. "SO4",
			// "OH"); un símbolo de elemento simple tiene a lo sumo 2.
			if (std::strlen(formula.componentes[i].simbolo) > 2)
			{
				radicalComp = &formula.componentes[i];
			}
			else
			{
				metal = &formula.componentes[i];
			}
		}

		if (metal != nullptr && radicalComp != nullptr)
		{
			std::strcpy(formulaRadical, radicalComp->simbolo);
			subindiceRadical = radicalComp->subindice;
			return true;
		}
		return false;
	}

	if (formula.cantidadComponentes == 3)
	{
		const ComponenteFormula *noMetalComp = nullptr;
		const ComponenteFormula *oxigeno = nullptr;
		metal = nullptr;

		for (int i = 0; i < formula.cantidadComponentes; i++)
		{
			const char *simbolo = formula.componentes[i].simbolo;
			if (std::strcmp(simbolo, "O") == 0)
			{
				oxigeno = &formula.componentes[i];
			}
			else if (buscarNoMetal(simbolo) != nullptr || buscarElemento(simbolo) == nullptr)
			{
				// El no metal del radical: se distingue del metal porque no
				// está en la tabla de metales (buscarElemento), o porque sí
				// figura como no metal conocido.
				noMetalComp = &formula.componentes[i];
			}
			else
			{
				metal = &formula.componentes[i];
			}
		}

		if (metal != nullptr && noMetalComp != nullptr && oxigeno != nullptr && noMetalComp->subindice == 1)
		{
			// Se arma "<simbolo>O<subindice>" (p.ej. "S"+"O"+"4" -> "SO4"),
			// igual a como el parser entrega un grupo con paréntesis, copiando
			// el símbolo, la "O" fija y el número por separado (en vez de un
			// snprintf con "%s" de tamaño no verificable en compilación, ya
			// que el compilador no puede acotar la longitud de simbolo[TAM_MAX]).
			// Un subíndice de O igual a 1 no se escribe (p.ej. "ClO", no
			// "ClO1"), igual que en la notación química estándar tabulada.
			std::strncpy(formulaRadical, noMetalComp->simbolo, TAM_MAX - 1);
			formulaRadical[TAM_MAX - 1] = '\0';
			size_t longitudSimbolo = std::strlen(formulaRadical);
			if (oxigeno->subindice == 1)
			{
				std::snprintf(formulaRadical + longitudSimbolo, TAM_MAX - longitudSimbolo, "O");
			}
			else
			{
				std::snprintf(formulaRadical + longitudSimbolo, TAM_MAX - longitudSimbolo, "O%d", oxigeno->subindice);
			}
			subindiceRadical = 1;
			return true;
		}
		return false;
	}

	return false;
}

ResultadoNomenclatura nomenclaturaTradicionalSal(const FormulaParseada &formula, char resultado[TAM_MAX],
                                                  Explicacion *explicacion)
{
	resultado[0] = '\0';

	const ComponenteFormula *metal = nullptr;
	char formulaRadical[TAM_MAX];
	int subindiceRadical = -1;
	if (!normalizarSal(formula, metal, formulaRadical, subindiceRadical))
	{
		return ResultadoNomenclatura::NO_ES_SAL;
	}

	agregarPaso(explicacion, "Se identifica el metal (%s) y el radical %s (subindice %d).",
	            metal->simbolo, formulaRadical, subindiceRadical);

	const InfoElemento *infoMetal = buscarElemento(metal->simbolo);
	if (infoMetal == nullptr)
	{
		return ResultadoNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	const InfoRadical *infoRadical = buscarRadical(formulaRadical);
	if (infoRadical == nullptr)
	{
		return ResultadoNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	agregarPaso(explicacion, "El radical %s es %s, con carga -%d.", formulaRadical, infoRadical->nombre, infoRadical->carga);

	// La fórmula equilibra cargas cuando subindice_metal * valencia_metal =
	// subindice_radical * carga_radical (misma idea de intercambio de
	// valencias que en óxidos, pero con el radical como unidad completa).
	int valenciaDeducida = -1;
	for (int i = 0; i < infoMetal->cantidadValencias; i++)
	{
		int v = infoMetal->valencias[i];
		if (metal->subindice * v == subindiceRadical * infoRadical->carga)
		{
			valenciaDeducida = v;
			break;
		}
	}

	if (valenciaDeducida == -1)
	{
		agregarPaso(explicacion, "Ninguna valencia conocida de %s equilibra %d*valencia = %d*%d.",
		            infoMetal->nombre, metal->subindice, subindiceRadical, infoRadical->carga);
		return ResultadoNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	agregarPaso(explicacion, "%d*%d = %d*%d confirma la valencia %d para %s.",
	            metal->subindice, valenciaDeducida, subindiceRadical, infoRadical->carga, valenciaDeducida, infoMetal->nombre);

	if (infoMetal->cantidadValencias == 1)
	{
		std::snprintf(resultado, TAM_MAX, "%s de %s", infoRadical->nombre, infoMetal->nombre);
	}
	else
	{
		agregarPaso(explicacion, "%s tiene mas de una valencia -> se indica con numero romano (%s).",
		            infoMetal->nombre, aRomano(valenciaDeducida));
		std::snprintf(resultado, TAM_MAX, "%s de %s (%s)", infoRadical->nombre, infoMetal->nombre, aRomano(valenciaDeducida));
	}

	return ResultadoNomenclatura::OK;
}

const char *nombreCategoria(CategoriaCompuesto categoria)
{
	switch (categoria)
	{
		case CategoriaCompuesto::OXIDO:           return "oxido";
		case CategoriaCompuesto::PEROXIDO:        return "peroxido";
		case CategoriaCompuesto::ANHIDRIDO:       return "anhidrido";
		case CategoriaCompuesto::ACIDO_HIDRACIDO: return "acido hidracido";
		case CategoriaCompuesto::ACIDO_OXACIDO:   return "acido oxacido";
		case CategoriaCompuesto::BASE:            return "base";
		case CategoriaCompuesto::SAL_OXISAL:      return "sal oxisal";
	}
	return "desconocido";
}

ResultadoNomenclatura detectarYNombrar(const FormulaParseada &formula, char resultado[TAM_MAX],
                                        CategoriaCompuesto &categoriaDetectada, Explicacion *explicacion)
{
	// Orden de prueba: metal+O antes que no metal+O evita que un mismo par
	// de símbolos se intente primero como anhídrido cuando en realidad es
	// óxido (buscarElemento/buscarNoMetal no se solapan en la práctica, pero
	// el orden por especificidad general reduce falsos positivos).
	//
	// Óxido antes que peróxido: para un metal con valencia v que también
	// tiene valencia 1, la fórmula reducida de "óxido con esa v" puede
	// coincidir con la de "peróxido con valencia 1" (p.ej. CuO es tanto
	// "oxido de cobre (II)" como, formalmente, "peroxido de cobre" si Cu
	// actuara con valencia 1) — se prioriza la lectura de óxido normal,
	// que es la interpretación estándar en ausencia de más contexto.
	struct Candidato {
		CategoriaCompuesto categoria;
		ResultadoNomenclatura (*calcular)(const FormulaParseada &, char[TAM_MAX], Explicacion *);
	};

	static const Candidato candidatos[] = {
		{CategoriaCompuesto::OXIDO, nomenclaturaStockOxido},
		{CategoriaCompuesto::PEROXIDO, nomenclaturaStockPeroxido},
		{CategoriaCompuesto::ANHIDRIDO, nomenclaturaTradicionalAnhidrido},
		{CategoriaCompuesto::ACIDO_HIDRACIDO, nomenclaturaTradicionalHidracido},
		{CategoriaCompuesto::ACIDO_OXACIDO, nomenclaturaTradicionalOxacido},
		{CategoriaCompuesto::BASE, nomenclaturaStockBase},
		{CategoriaCompuesto::SAL_OXISAL, nomenclaturaTradicionalSal},
	};

	ResultadoNomenclatura ultimoError = ResultadoNomenclatura::FORMULA_INVALIDA;

	for (const Candidato &candidato : candidatos)
	{
		char intento[TAM_MAX];
		ResultadoNomenclatura r = candidato.calcular(formula, intento, explicacion);
		if (r == ResultadoNomenclatura::OK)
		{
			categoriaDetectada = candidato.categoria;
			std::strncpy(resultado, intento, TAM_MAX - 1);
			resultado[TAM_MAX - 1] = '\0';
			return ResultadoNomenclatura::OK;
		}

		// Un error "no es esta categoria" (el nombre no calzo con el patron)
		// se descarta en silencio y se prueba la siguiente. Un error mas
		// especifico (elemento desconocido, valencia no determinada) es mas
		// informativo para el usuario si ninguna categoria termina calzando,
		// así que se conserva como el mensaje final a reportar.
		bool esRechazoDePatron = (r == ResultadoNomenclatura::NO_ES_OXIDO ||
		                          r == ResultadoNomenclatura::NO_ES_PEROXIDO ||
		                          r == ResultadoNomenclatura::NO_ES_ANHIDRIDO ||
		                          r == ResultadoNomenclatura::NO_ES_HIDRACIDO ||
		                          r == ResultadoNomenclatura::NO_ES_OXACIDO ||
		                          r == ResultadoNomenclatura::NO_ES_BASE ||
		                          r == ResultadoNomenclatura::NO_ES_SAL);
		if (!esRechazoDePatron)
		{
			ultimoError = r;
		}

		if (explicacion != nullptr)
		{
			explicacion->cantidadPasos = 0; // se descartan los pasos del intento fallido
		}
	}

	resultado[0] = '\0';
	return ultimoError;
}

const char *mensajeError(ResultadoNomenclatura resultado)
{
	switch (resultado)
	{
		case ResultadoNomenclatura::OK:
			return "Sin errores.";
		case ResultadoNomenclatura::FORMULA_INVALIDA:
			return "La formula no corresponde a ninguna de las 7 categorias de compuesto que este programa reconoce.";
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
		case ResultadoNomenclatura::NO_ES_BASE:
			return "La formula no corresponde a una base (se esperaba metal + grupo hidroxilo OH, ej. Ca(OH)2 o NaOH).";
		case ResultadoNomenclatura::NO_ES_SAL:
			return "La formula no corresponde a una sal oxisal (se esperaba metal + radical conocido, ej. Al2(SO4)3 o CaCO3).";
		case ResultadoNomenclatura::ELEMENTO_DESCONOCIDO:
			return "El elemento de la formula no esta en la tabla de elementos/no metales soportados.";
		case ResultadoNomenclatura::VALENCIA_NO_DETERMINADA:
			return "La proporcion de la formula no corresponde a ninguna valencia conocida del metal.";
	}
	return "Error desconocido.";
}
