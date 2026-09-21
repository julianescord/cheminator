#include "test_runner.h"
#include <cstdio>

void test_formula();
void test_elementos();
void test_nomenclatura();
void test_nomenclatura_peroxidos();

int main()
{
	test_formula();
	test_elementos();
	test_nomenclatura();
	test_nomenclatura_peroxidos();

	if (g_pruebasFallidas == 0)
	{
		std::printf("Todas las pruebas pasaron.\n");
		return 0;
	}

	std::printf("%d prueba(s) fallaron.\n", g_pruebasFallidas);
	return 1;
}
