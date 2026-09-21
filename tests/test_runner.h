#ifndef CHEMINATOR_TEST_RUNNER_H
#define CHEMINATOR_TEST_RUNNER_H

#include <cstdio>
#include <cstdlib>

// Framework de pruebas mínimo, sin dependencias externas (para no exigir
// instalar una librería de testing solo para compilar el proyecto).
// Cada ASSERT_* imprime y aborta con código de error en el primer fallo,
// indicando el archivo y la línea donde ocurrió.

inline int g_pruebasFallidas = 0;

#define ASSERT_TRUE(condicion)                                                          \
	do                                                                                    \
	{                                                                                     \
		if (!(condicion))                                                                \
		{                                                                                 \
			std::fprintf(stderr, "FALLO %s:%d: se esperaba que fuera verdadero: %s\n",    \
			             __FILE__, __LINE__, #condicion);                                 \
			g_pruebasFallidas++;                                                         \
		}                                                                                 \
	} while (0)

#define ASSERT_EQ_INT(actual, esperado)                                                 \
	do                                                                                    \
	{                                                                                     \
		int _a = (actual);                                                               \
		int _e = (esperado);                                                             \
		if (_a != _e)                                                                    \
		{                                                                                 \
			std::fprintf(stderr, "FALLO %s:%d: %s == %d, se esperaba %d\n",              \
			             __FILE__, __LINE__, #actual, _a, _e);                            \
			g_pruebasFallidas++;                                                         \
		}                                                                                 \
	} while (0)

#endif // CHEMINATOR_TEST_RUNNER_H
