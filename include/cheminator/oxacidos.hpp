#ifndef CHEMINATOR_OXACIDOS_HPP
#define CHEMINATOR_OXACIDOS_HPP

#include "cheminator/tipos.hpp"

#include <string_view>

namespace cheminator {

// Un acido oxacido conocido: la formula HxEyOz en su forma reducida estandar
// y el nombre tradicional que le corresponde.
//
// Se tabula explicitamente en vez de derivarse sumando anhidrido + H2O y
// reduciendo, porque esa derivacion no siempre da la formula real: el fosforo
// con valencias 3 y 5 forma H3PO3 y H3PO4, no HPO2 y HPO3 como resultaria de
// aplicar la regla simple de intercambio.
struct InfoOxacido {
	std::string_view simboloNoMetal;
	Subindice hidrogenos;
	Subindice atomosNoMetal;
	Subindice oxigenos;
	std::string_view nombre;
};

// Busca el oxacido cuya formula coincide exactamente con los subindices dados.
// Devuelve nullptr si no hay coincidencia.
const InfoOxacido *buscarOxacido(std::string_view simboloNoMetal, Subindice hidrogenos, Subindice atomosNoMetal,
                                  Subindice oxigenos) noexcept;

} // namespace cheminator

#endif // CHEMINATOR_OXACIDOS_HPP
