#include "cheminator/elementos.hpp"
#include "test_runner.h"
#include <cstring>

void test_elementos()
{
	const InfoElemento *fe = buscarElemento("Fe");
	ASSERT_TRUE(fe != nullptr);
	ASSERT_TRUE(std::strcmp(fe->nombre, "Hierro") == 0);
	ASSERT_EQ_INT(fe->cantidadValencias, 2);

	ASSERT_TRUE(buscarElemento("Xx") == nullptr);

	char nombre[TAM_MAX];
	obtenerNombreElemento("H", nombre);
	ASSERT_TRUE(std::strcmp(nombre, "Hidrogeno") == 0);

	obtenerNombreElemento("Xx", nombre);
	ASSERT_TRUE(nombre[0] == '\0');
}
