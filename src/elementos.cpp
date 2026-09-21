#include "elementos.h"
#include <cstring>

// Tabla de símbolos conocidos y su nombre. Se irá ampliando a medida que se
// implementen más compuestos (por ahora solo hidrógeno, base del prototipo original).
struct Elemento {
	const char *simbolo;
	const char *nombre;
};

static const Elemento TABLA_ELEMENTOS[] = {
	{"H", "Hidrogeno"},
};

void obtenerNombreElemento(const char simbolo[], char resultado[TAM_MAX])
{
	resultado[0] = '\0';

	for (const Elemento &elemento : TABLA_ELEMENTOS)
	{
		if (std::strcmp(simbolo, elemento.simbolo) == 0)
		{
			std::strncpy(resultado, elemento.nombre, TAM_MAX - 1);
			resultado[TAM_MAX - 1] = '\0';
			return;
		}
	}
}
