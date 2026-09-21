#ifndef CHEMINATOR_C_API_H
#define CHEMINATOR_C_API_H

/*
 * Frontera en C de la libreria.
 *
 * El nucleo esta escrito en C++, pero casi ningun lenguaje sabe enlazar
 * contra C++ directamente: los nombres mangleados, las excepciones y el
 * layout de std::string son especificos del compilador. Una capa en C con
 * tipos opacos es la interfaz que si pueden consumir Python (ctypes/cffi),
 * C#, Java (JNI), Rust, Go y cualquier otro lenguaje con FFI.
 *
 * Convencion de memoria: chem_nombrar() entrega un objeto que pertenece a
 * quien llama y se libera con chem_resultado_liberar(). Las cadenas que
 * devuelven los accesores apuntan a memoria interna de ese objeto y dejan de
 * ser validas al liberarlo; quien las necesite despues debe copiarlas.
 */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Resultado de nombrar un compuesto. Tipo opaco: su contenido se consulta
 * con los accesores de abajo, nunca accediendo a campos. */
typedef struct chem_resultado chem_resultado;

/* Analiza la formula y determina la categoria y el nombre del compuesto.
 * Devuelve NULL solo si no hubo memoria disponible. En cualquier otro caso
 * devuelve un objeto valido, que puede representar un exito o un error;
 * eso se consulta con chem_resultado_ok(). */
chem_resultado *chem_nombrar(const char *formula);

/* Libera el objeto. Acepta NULL sin hacer nada. */
void chem_resultado_liberar(chem_resultado *resultado);

/* 1 si se pudo nombrar el compuesto, 0 si hubo error. */
int chem_resultado_ok(const chem_resultado *resultado);

/* Nombre del compuesto ("oxido de Hierro (III)"). Cadena vacia si hubo error. */
const char *chem_resultado_nombre(const chem_resultado *resultado);

/* Categoria detectada ("oxido", "sal oxisal"...). Vacia si hubo error. */
const char *chem_resultado_categoria(const chem_resultado *resultado);

/* Mensaje de error legible. Cadena vacia si no hubo error. */
const char *chem_resultado_error(const chem_resultado *resultado);

/* Cantidad de pasos del razonamiento; 0 si hubo error. */
size_t chem_resultado_cantidad_pasos(const chem_resultado *resultado);

/* Paso numero `indice` del razonamiento, o cadena vacia si esta fuera de rango. */
const char *chem_resultado_paso(const chem_resultado *resultado, size_t indice);

/* Version de la libreria, con formato "mayor.menor.parche". */
const char *chem_version(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CHEMINATOR_C_API_H */
