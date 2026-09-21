#include "cheminator/no_metales.hpp"
#include <cstring>

// Tabla de no metales típicos de un curso de nomenclatura inorgánica básica,
// con sus valencias más usadas para anhídridos (óxidos de no metal) en orden
// ascendente. La raíz es la que se usa para los sufijos -oso/-ico, que no
// siempre coincide con el nombre del elemento (p.ej. "azufre" -> "sulfur-").
static const InfoNoMetal TABLA_NO_METALES[] = {
	{"C",  "Carbono",  "carbon",   {2, 4},       2},
	{"N",  "Nitrogeno", "nitr",    {1, 3, 5},    3},
	{"P",  "Fosforo",  "fosfor",  {3, 5},        2},
	{"S",  "Azufre",   "sulfur",  {2, 4, 6},     3},
	{"Cl", "Cloro",    "clor",    {1, 3, 5, 7},  4},
	{"Br", "Bromo",    "brom",    {1, 3, 5, 7},  4},
	{"I",  "Yodo",     "iod",     {1, 3, 5, 7},  4},
};

static constexpr int CANTIDAD_NO_METALES = sizeof(TABLA_NO_METALES) / sizeof(TABLA_NO_METALES[0]);

const InfoNoMetal *buscarNoMetal(const char simbolo[])
{
	for (int i = 0; i < CANTIDAD_NO_METALES; i++)
	{
		if (std::strcmp(simbolo, TABLA_NO_METALES[i].simbolo) == 0)
		{
			return &TABLA_NO_METALES[i];
		}
	}
	return nullptr;
}
