#include "elementos.h"
#include <cstring>

// Tabla de elementos conocidos: símbolo, nombre y valencias positivas típicas
// (las usadas para nomenclatura de óxidos). Se amplía a medida que se cubran
// más compuestos. Fuente: valencias de uso más común en cursos de nomenclatura
// inorgánica básica, no la lista completa de estados de oxidación posibles.
static const InfoElemento TABLA_ELEMENTOS[] = {
	{"H",  "Hidrogeno", {1},       1},
	{"Li", "Litio",     {1},       1},
	{"Na", "Sodio",     {1},       1},
	{"K",  "Potasio",   {1},       1},
	{"Ca", "Calcio",    {2},       1},
	{"Mg", "Magnesio",  {2},       1},
	{"Al", "Aluminio",  {3},       1},
	{"Zn", "Zinc",      {2},       1},
	{"Ag", "Plata",     {1},       1},
	{"Cu", "Cobre",     {1, 2},    2},
	{"Fe", "Hierro",    {2, 3},    2},
	{"Au", "Oro",       {1, 3},    2},
	{"Pb", "Plomo",     {2, 4},    2},
	{"Sn", "Estaño",    {2, 4},    2},
	{"O",  "Oxigeno",   {2},       1},
};

static constexpr int CANTIDAD_ELEMENTOS = sizeof(TABLA_ELEMENTOS) / sizeof(TABLA_ELEMENTOS[0]);

const InfoElemento *buscarElemento(const char simbolo[])
{
	for (int i = 0; i < CANTIDAD_ELEMENTOS; i++)
	{
		if (std::strcmp(simbolo, TABLA_ELEMENTOS[i].simbolo) == 0)
		{
			return &TABLA_ELEMENTOS[i];
		}
	}
	return nullptr;
}

void obtenerNombreElemento(const char simbolo[], char resultado[TAM_MAX])
{
	resultado[0] = '\0';

	const InfoElemento *elemento = buscarElemento(simbolo);
	if (elemento != nullptr)
	{
		std::strncpy(resultado, elemento->nombre, TAM_MAX - 1);
		resultado[TAM_MAX - 1] = '\0';
	}
}
