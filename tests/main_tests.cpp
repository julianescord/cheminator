#include "test_runner.h"

#include <cstdio>

void test_tipos();
void test_formula();
void test_elementos();
void test_nomenclatura_oxidos();
void test_nomenclatura_peroxidos();
void test_nomenclatura_anhidridos();
void test_nomenclatura_hidracidos();
void test_nomenclatura_oxacidos();
void test_nomenclatura_bases();
void test_nomenclatura_sales();
void test_deteccion();
void test_formulacion();
void test_ida_y_vuelta();

int main()
{
	test_tipos();
	test_formula();
	test_elementos();
	test_nomenclatura_oxidos();
	test_nomenclatura_peroxidos();
	test_nomenclatura_anhidridos();
	test_nomenclatura_hidracidos();
	test_nomenclatura_oxacidos();
	test_nomenclatura_bases();
	test_nomenclatura_sales();
	test_deteccion();
	test_formulacion();
	test_ida_y_vuelta();

	if (g_pruebasFallidas == 0)
	{
		std::printf("Todas las pruebas pasaron.\n");
		return 0;
	}

	std::printf("%d prueba(s) fallaron.\n", g_pruebasFallidas);
	return 1;
}
