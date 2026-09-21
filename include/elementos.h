#ifndef CHEMINATOR_ELEMENTOS_H
#define CHEMINATOR_ELEMENTOS_H

// Tamaño máximo esperado para símbolos/nombres de elementos y fórmulas.
constexpr int TAM_MAX = 50;

// Busca el nombre del elemento correspondiente al símbolo dado (p.ej. "H" -> "Hidrogeno").
// Escribe el resultado en `resultado`. Si el símbolo no se reconoce, escribe una cadena vacía.
void obtenerNombreElemento(const char simbolo[], char resultado[TAM_MAX]);

#endif // CHEMINATOR_ELEMENTOS_H
