#ifndef CHEMINATOR_NOMENCLATURA_HPP
#define CHEMINATOR_NOMENCLATURA_HPP

#include "cheminator/elementos.hpp"
#include "cheminator/formula.hpp"
#include "cheminator/hidracidos.hpp"
#include "cheminator/no_metales.hpp"
#include "cheminator/oxacidos.hpp"
#include "cheminator/radicales.hpp"
#include "cheminator/tipos.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace cheminator {

// Las categorias de compuesto que reconoce la libreria.
enum class CategoriaCompuesto {
	OXIDO,
	PEROXIDO,
	ANHIDRIDO,
	ACIDO_HIDRACIDO,
	ACIDO_OXACIDO,
	BASE,
	SAL_OXISAL,
	COMPLEJO,
};

std::string_view nombreCategoria(CategoriaCompuesto categoria) noexcept;

// El resultado de nombrar un compuesto: ademas del nombre, la categoria a la
// que resulto pertenecer y el razonamiento que llevo hasta el.
//
// Los pasos no son un extra decorativo: son el motivo por el que existe esta
// libreria. Las herramientas equivalentes devuelven el nombre y nada mas, lo
// que sirve para consultar pero no para aprender.
struct Nomenclatura {
	std::string nombre;
	CategoriaCompuesto categoria;
	std::vector<std::string> pasos;
};

enum class ErrorNomenclatura {
	NINGUNA_CATEGORIA,       // no coincide con ninguna de las categorias conocidas
	NO_ES_OXIDO,
	NO_ES_PEROXIDO,
	NO_ES_ANHIDRIDO,
	NO_ES_HIDRACIDO,
	NO_ES_OXACIDO,
	NO_ES_BASE,
	NO_ES_SAL,
	ELEMENTO_DESCONOCIDO,    // algun elemento no esta en las tablas
	VALENCIA_NO_DETERMINADA, // los subindices no cuadran con ninguna valencia conocida
};

std::string_view mensajeError(ErrorNomenclatura error) noexcept;

using ResultadoNomenclatura = Resultado<Nomenclatura, ErrorNomenclatura>;

// Nomenclatura Stock: "oxido de Hierro (III)". El numero romano solo aparece
// cuando el metal admite mas de una valencia.
ResultadoNomenclatura nombrarOxido(const Formula &formula);

// Nomenclatura Stock: "peroxido de Sodio". El grupo peroxo (O2)2- hace que
// cada oxigeno aporte valencia -1 y que su subindice no se reduzca.
ResultadoNomenclatura nombrarPeroxido(const Formula &formula);

// Nomenclatura tradicional: "anhidrido sulfurico". Usa los sufijos -oso/-ico
// y, para no metales con 3 o 4 valencias, los prefijos hipo-/per-.
ResultadoNomenclatura nombrarAnhidrido(const Formula &formula);

// Nomenclatura tradicional: "acido clorhidrico" (H + no metal).
ResultadoNomenclatura nombrarHidracido(const Formula &formula);

// Nomenclatura tradicional: "acido sulfurico" (H + no metal + oxigeno).
ResultadoNomenclatura nombrarOxacido(const Formula &formula);

// Nomenclatura Stock: "hidroxido de Calcio". Acepta tanto "Ca(OH)2" como
// "NaOH", que el parser entrega de formas distintas.
ResultadoNomenclatura nombrarBase(const Formula &formula);

// Nomenclatura tradicional: "sulfato de Aluminio". Acepta tanto "Al2(SO4)3"
// como "Na2SO4".
ResultadoNomenclatura nombrarSal(const Formula &formula);

// Prueba la formula contra las categorias conocidas y devuelve la primera que
// la reconoce, sin que haga falta decirle de antemano que tipo de compuesto es.
ResultadoNomenclatura nombrar(const Formula &formula);

} // namespace cheminator

#endif // CHEMINATOR_NOMENCLATURA_HPP
