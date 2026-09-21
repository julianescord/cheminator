#include "cheminator/hidracidos.hpp"
#include <cstring>

// No metales que forman ácidos hidrácidos comunes en un curso de
// nomenclatura inorgánica básica: halógenos (valencia -1, un H) y algunos
// calcógenos (valencia -2, dos H). El nombre del ácido es "acido <nombre
// sin sufijo>hidrico", p.ej. "acido clorhidrico", "acido sulfhidrico".
static const InfoHidracido TABLA_HIDRACIDOS[] = {
	{"F",  "fluor",   1},
	{"Cl", "clor",    1},
	{"Br", "brom",    1},
	{"I",  "iod",     1},
	{"S",  "sulf",    2},
	{"Se", "selen",   2},
	{"Te", "telur",   2},
};

static constexpr int CANTIDAD_HIDRACIDOS = sizeof(TABLA_HIDRACIDOS) / sizeof(TABLA_HIDRACIDOS[0]);

const InfoHidracido *buscarHidracido(const char simbolo[])
{
	for (int i = 0; i < CANTIDAD_HIDRACIDOS; i++)
	{
		if (std::strcmp(simbolo, TABLA_HIDRACIDOS[i].simbolo) == 0)
		{
			return &TABLA_HIDRACIDOS[i];
		}
	}
	return nullptr;
}
