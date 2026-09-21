#include "formula.h"
#include <cctype>
#include <cstring>

ResultadoParseo parsearFormula(const char formula[], FormulaParseada &resultado)
{
	resultado.cantidadComponentes = 0;

	if (formula == nullptr || formula[0] == '\0')
	{
		return ResultadoParseo::FORMULA_VACIA;
	}

	int i = 0;
	int longitud = static_cast<int>(std::strlen(formula));

	while (i < longitud)
	{
		// Un símbolo válido empieza en mayúscula y puede tener una segunda
		// letra minúscula (p.ej. "Fe", "Na"), como en la notación química real.
		if (!std::isupper(static_cast<unsigned char>(formula[i])))
		{
			return ResultadoParseo::SIMBOLO_INVALIDO;
		}

		if (resultado.cantidadComponentes >= MAX_COMPONENTES)
		{
			return ResultadoParseo::DEMASIADOS_COMPONENTES;
		}

		char simbolo[TAM_MAX];
		int j = 0;
		simbolo[j++] = formula[i++];

		if (i < longitud && std::islower(static_cast<unsigned char>(formula[i])))
		{
			simbolo[j++] = formula[i++];
		}
		simbolo[j] = '\0';

		// Subíndice: uno o más dígitos consecutivos; si no hay dígitos, es 1.
		int subindice = 0;
		int digitos = 0;
		while (i < longitud && std::isdigit(static_cast<unsigned char>(formula[i])))
		{
			subindice = subindice * 10 + (formula[i] - '0');
			i++;
			digitos++;
		}

		if (digitos == 0)
		{
			subindice = 1;
		}
		else if (subindice == 0)
		{
			// Se escribieron dígitos pero el valor es 0 (p.ej. "H0"), inválido.
			return ResultadoParseo::SUBINDICE_INVALIDO;
		}

		std::strcpy(resultado.componentes[resultado.cantidadComponentes].simbolo, simbolo);
		resultado.componentes[resultado.cantidadComponentes].subindice = subindice;
		resultado.cantidadComponentes++;
	}

	return ResultadoParseo::OK;
}

const char *mensajeError(ResultadoParseo resultado)
{
	switch (resultado)
	{
		case ResultadoParseo::OK:
			return "Sin errores.";
		case ResultadoParseo::FORMULA_VACIA:
			return "La formula esta vacia.";
		case ResultadoParseo::SIMBOLO_INVALIDO:
			return "La formula contiene un simbolo invalido (debe iniciar con mayuscula, ej. \"Fe2O3\").";
		case ResultadoParseo::SUBINDICE_INVALIDO:
			return "La formula contiene un subindice invalido.";
		case ResultadoParseo::DEMASIADOS_COMPONENTES:
			return "La formula tiene mas elementos distintos de los soportados.";
	}
	return "Error desconocido.";
}
