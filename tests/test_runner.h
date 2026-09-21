#ifndef CHEMINATOR_TEST_RUNNER_H
#define CHEMINATOR_TEST_RUNNER_H

#include <cstdio>
#include <string_view>

// Framework de pruebas minimo, sin dependencias externas, para no exigir
// instalar una libreria de testing solo para compilar el proyecto.
// Cada ASSERT_* informa el archivo y la linea del fallo y sigue adelante, de
// modo que una corrida reporta todos los problemas y no solo el primero.

inline int g_pruebasFallidas = 0;

#define ASSERT_TRUE(condicion)                                                                    \
	do                                                                                              \
	{                                                                                               \
		if (!(condicion))                                                                          \
		{                                                                                           \
			std::fprintf(stderr, "FALLO %s:%d: se esperaba que fuera verdadero: %s\n", __FILE__,    \
			             __LINE__, #condicion);                                                     \
			g_pruebasFallidas++;                                                                   \
		}                                                                                           \
	} while (0)

#define ASSERT_EQ_INT(actual, esperado)                                                           \
	do                                                                                              \
	{                                                                                               \
		const int _a = (actual);                                                                   \
		const int _e = (esperado);                                                                 \
		if (_a != _e)                                                                              \
		{                                                                                           \
			std::fprintf(stderr, "FALLO %s:%d: %s == %d, se esperaba %d\n", __FILE__, __LINE__,     \
			             #actual, _a, _e);                                                          \
			g_pruebasFallidas++;                                                                   \
		}                                                                                           \
	} while (0)

#define ASSERT_EQ_STR(actual, esperado)                                                           \
	do                                                                                              \
	{                                                                                               \
		const std::string_view _a{actual};                                                         \
		const std::string_view _e{esperado};                                                       \
		if (_a != _e)                                                                              \
		{                                                                                           \
			std::fprintf(stderr, "FALLO %s:%d: %s == \"%.*s\", se esperaba \"%.*s\"\n", __FILE__,   \
			             __LINE__, #actual, static_cast<int>(_a.size()), _a.data(),                 \
			             static_cast<int>(_e.size()), _e.data());                                   \
			g_pruebasFallidas++;                                                                   \
		}                                                                                           \
	} while (0)

#endif // CHEMINATOR_TEST_RUNNER_H
