#include "oxacidos.h"
#include <cstring>

// Fórmulas y nombres verificados manualmente contra la nomenclatura
// tradicional estándar de un curso de química inorgánica básica (no
// derivados por fórmula genérica; ver la nota en oxacidos.h). El ácido
// nítrico(I)/HNO y el hiponitroso de N no se incluyen por ser inusuales en
// este nivel; se cubre el resto de valencias tabuladas para anhídridos.
static const InfoOxacido TABLA_OXACIDOS[] = {
	{"C",  2, 1, 3, "acido carbonico"},

	{"N",  1, 1, 2, "acido nitroso"},
	{"N",  1, 1, 3, "acido nitrico"},

	{"P",  3, 1, 3, "acido fosforoso"},
	{"P",  3, 1, 4, "acido fosforico"},

	{"S",  2, 1, 2, "acido hiposulfuroso"},
	{"S",  2, 1, 3, "acido sulfuroso"},
	{"S",  2, 1, 4, "acido sulfurico"},

	{"Cl", 1, 1, 1, "acido hipocloroso"},
	{"Cl", 1, 1, 2, "acido cloroso"},
	{"Cl", 1, 1, 3, "acido clorico"},
	{"Cl", 1, 1, 4, "acido perclorico"},

	{"Br", 1, 1, 1, "acido hipobromoso"},
	{"Br", 1, 1, 2, "acido bromoso"},
	{"Br", 1, 1, 3, "acido bromico"},
	{"Br", 1, 1, 4, "acido perbromico"},

	{"I",  1, 1, 1, "acido hipoiodoso"},
	{"I",  1, 1, 2, "acido iodoso"},
	{"I",  1, 1, 3, "acido iodico"},
	{"I",  1, 1, 4, "acido periodico"},
};

static constexpr int CANTIDAD_OXACIDOS = sizeof(TABLA_OXACIDOS) / sizeof(TABLA_OXACIDOS[0]);

const InfoOxacido *buscarOxacido(const char simboloNoMetal[], int subindiceH, int subindiceNoMetal, int subindiceO)
{
	for (int i = 0; i < CANTIDAD_OXACIDOS; i++)
	{
		const InfoOxacido &o = TABLA_OXACIDOS[i];
		if (std::strcmp(simboloNoMetal, o.simboloNoMetal) == 0 &&
		    subindiceH == o.subindiceH &&
		    subindiceNoMetal == o.subindiceNoMetal &&
		    subindiceO == o.subindiceO)
		{
			return &o;
		}
	}
	return nullptr;
}
