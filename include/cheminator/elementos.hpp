#ifndef CHEMINATOR_ELEMENTOS_H
#define CHEMINATOR_ELEMENTOS_H

// Tamaño máximo esperado para símbolos/nombres de elementos y fórmulas.
constexpr int TAM_MAX = 50;

// Máximo número de valencias que puede tener un elemento en esta tabla.
constexpr int MAX_VALENCIAS = 4;

struct InfoElemento {
	const char *simbolo;
	const char *nombre;
	int valencias[MAX_VALENCIAS];
	int cantidadValencias;
};

// Busca la información (nombre y valencias) del elemento con el símbolo dado.
// Devuelve `nullptr` si el símbolo no está en la tabla.
const InfoElemento *buscarElemento(const char simbolo[]);

// Busca el nombre del elemento correspondiente al símbolo dado (p.ej. "H" -> "Hidrogeno").
// Escribe el resultado en `resultado`. Si el símbolo no se reconoce, escribe una cadena vacía.
void obtenerNombreElemento(const char simbolo[], char resultado[TAM_MAX]);

#endif // CHEMINATOR_ELEMENTOS_H
