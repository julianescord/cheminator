#include "cheminator/radicales.hpp"
#include <cstring>

// Radicales (aniones poliatómicos) derivados de los ácidos oxácidos
// tabulados en oxacidos.cpp, verificados manualmente contra la nomenclatura
// tradicional estándar (no derivados por regla genérica raíz+sufijo, que
// falla para azufre y fósforo; ver la nota en radicales.h).
static const InfoRadical TABLA_RADICALES[] = {
	{"CO3",  2, "carbonato"},

	{"NO2",  1, "nitrito"},
	{"NO3",  1, "nitrato"},

	{"PO3",  3, "fosfito"},
	{"PO4",  3, "fosfato"},

	{"SO2",  2, "hiposulfito"},
	{"SO3",  2, "sulfito"},
	{"SO4",  2, "sulfato"},

	{"ClO",  1, "hipoclorito"},
	{"ClO2", 1, "clorito"},
	{"ClO3", 1, "clorato"},
	{"ClO4", 1, "perclorato"},

	{"BrO",  1, "hipobromito"},
	{"BrO2", 1, "bromito"},
	{"BrO3", 1, "bromato"},
	{"BrO4", 1, "perbromato"},

	{"IO",   1, "hipoiodito"},
	{"IO2",  1, "iodito"},
	{"IO3",  1, "iodato"},
	{"IO4",  1, "periodato"},
};

static constexpr int CANTIDAD_RADICALES = sizeof(TABLA_RADICALES) / sizeof(TABLA_RADICALES[0]);

const InfoRadical *buscarRadical(const char formula[])
{
	for (int i = 0; i < CANTIDAD_RADICALES; i++)
	{
		if (std::strcmp(formula, TABLA_RADICALES[i].formula) == 0)
		{
			return &TABLA_RADICALES[i];
		}
	}
	return nullptr;
}
