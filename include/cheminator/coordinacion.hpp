#ifndef CHEMINATOR_COORDINACION_HPP
#define CHEMINATOR_COORDINACION_HPP

#include "cheminator/formula.hpp"
#include "cheminator/ligandos.hpp"
#include "cheminator/tipos.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace cheminator {

// Compuestos de coordinacion: un atomo central rodeado de ligandos dentro de
// una esfera que se escribe entre corchetes, con un posible contraion fuera.
//
// Es la categoria que justifica el arbol del parser. Un compuesto simple se
// describe con una lista de pares; aqui hace falta saber que esta dentro de la
// esfera y que fuera, cuantos ligandos hay de cada clase y con que carga, para
// poder deducir el estado de oxidacion del metal.
//
//     K3[Fe(CN)6]
//     |  |  |  |
//     |  |  |  +-- indice de coordinacion: 6
//     |  |  +----- ligando ciano, carga 1- cada uno
//     |  +-------- atomo central, hierro
//     +----------- contraion, fuera de la esfera

// Un ligando con cuantas veces aparece.
struct LigandoContado {
	const InfoLigando *info;
	int cantidad;
};

// El analisis de un complejo, antes de ponerle nombre.
struct Complejo {
	std::string_view simboloCentral;
	Valencia estadoOxidacion{0};
	std::vector<LigandoContado> ligandos;
	// Suma de los ligandos, contando repeticiones: el indice de coordinacion.
	int indiceCoordinacion = 0;
	// Carga de la esfera entre corchetes. Negativa, positiva o cero.
	Carga cargaEsfera{0};
	// Contraion fuera de los corchetes, si lo hay ("K3" en "K3[Fe(CN)6]").
	std::string contraion;
	Subindice cantidadContraion{0};
	// Si el contraion va delante (es un cation, el complejo es el anion).
	bool contraionDelante = false;
};

enum class ErrorCoordinacion {
	NO_ES_COMPLEJO,          // la formula no tiene esfera entre corchetes
	SIN_ATOMO_CENTRAL,       // el corchete no empieza por un metal conocido
	LIGANDO_DESCONOCIDO,     // alguna especie de la esfera no esta tabulada
	CONTRAION_DESCONOCIDO,   // el ion de fuera no esta en las tablas
	CARGA_INCONSISTENTE,     // la carga no cuadra con los ligandos y el metal
	ESTADO_INVALIDO,         // el estado de oxidacion no lo admite el metal
};

std::string_view mensajeError(ErrorCoordinacion error) noexcept;

// Analiza la estructura de un complejo a partir de la formula ya parseada.
Resultado<Complejo, ErrorCoordinacion> analizarComplejo(const Formula &formula);

// Nombre en espanol de un metal cuando actua como atomo central de un anion
// complejo: ahi la IUPAC usa la raiz latina mas el sufijo -ato ("ferrato" y no
// "hierrato"). Devuelve una cadena vacia si el metal no esta tabulado.
std::string_view nombreAnionico(std::string_view simbolo) noexcept;

// Nombra un compuesto de coordinacion siguiendo las reglas de la IUPAC:
// ligandos en orden alfabetico con su prefijo multiplicador, luego el atomo
// central con su estado de oxidacion en numero romano, y el sufijo -ato si el
// complejo es un anion.
//
//     K3[Fe(CN)6] -> hexacianoferrato (III) de potasio
//
// Devuelve tambien los pasos del razonamiento, igual que nombrar().
struct NomenclaturaComplejo {
	std::string nombre;
	std::vector<std::string> pasos;
};

Resultado<NomenclaturaComplejo, ErrorCoordinacion> nombrarComplejo(const Formula &formula);

} // namespace cheminator

#endif // CHEMINATOR_COORDINACION_HPP
