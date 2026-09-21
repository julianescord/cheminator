#ifndef CHEMINATOR_LIGANDOS_HPP
#define CHEMINATOR_LIGANDOS_HPP

#include "cheminator/tipos.hpp"

#include <span>
#include <string_view>

namespace cheminator {

// Un ligando: la especie que se une al atomo central en un complejo de
// coordinacion.
//
// Los nombres no se derivan del elemento porque la IUPAC los fija uno por uno
// y varios no se parecen a su formula: CN- es "ciano", H2O es "acua" y NH3 es
// "amin", tres formas distintas de nombrar tres moleculas corrientes.
struct InfoLigando {
	// Formula tal como aparece dentro de la esfera de coordinacion ("CN",
	// "H2O", "NH3").
	std::string_view formula;
	// Nombre que toma como ligando, sin prefijo multiplicador ("ciano").
	std::string_view nombre;
	// Carga del ligando, 0 si es neutro. Determina el estado de oxidacion del
	// atomo central junto con la carga global del complejo.
	Carga carga;
	// Los ligandos cuyo nombre ya lleva un numero ("etilendiamino") usan los
	// prefijos bis/tris en vez de di/tri, para no leerse como parte del
	// nombre. Ninguno de los tabulados hoy lo necesita, pero el campo existe
	// porque la regla es parte de la nomenclatura y no del ligando concreto.
	bool usaPrefijoAlterno = false;
};

// Busca un ligando por su formula. Devuelve nullptr si no esta en la tabla.
const InfoLigando *buscarLigando(std::string_view formula) noexcept;

// La tabla completa, para recorrerla buscando por nombre en vez de por
// formula, que es lo que necesita la formulacion inversa.
std::span<const InfoLigando> todosLosLigandos() noexcept;

} // namespace cheminator

#endif // CHEMINATOR_LIGANDOS_HPP
