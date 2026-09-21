#ifndef CHEMINATOR_FORMULA_HPP
#define CHEMINATOR_FORMULA_HPP

#include "cheminator/tipos.hpp"

#include <initializer_list>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace cheminator {

// Un componente de una formula: un simbolo de elemento o, si venia entre
// parentesis, el contenido completo del grupo ("OH", "SO4"), con su subindice.
// En "Fe2O3" los componentes son {"Fe",2} y {"O",3}; en "Ca(OH)2", {"Ca",1} y
// {"OH",2}.
struct ComponenteFormula {
	std::string simbolo;
	Subindice subindice;
};

// Tope de componentes distintos que se aceptan, suficiente para los
// compuestos inorganicos simples que cubre la libreria.
inline constexpr std::size_t MAX_COMPONENTES = 4;

enum class ErrorFormula {
	VACIA,
	SIMBOLO_INVALIDO,       // no empieza en mayuscula, o tiene caracteres raros
	SUBINDICE_INVALIDO,     // se escribio un subindice igual a cero
	DEMASIADOS_COMPONENTES,
	GRUPO_MAL_FORMADO,      // parentesis sin cerrar, vacio o anidado
};

std::string_view mensajeError(ErrorFormula error) noexcept;

// Una formula ya analizada, como lista de componentes.
class Formula {
public:
	Formula() = default;
	explicit Formula(std::vector<ComponenteFormula> componentes) : componentes_{std::move(componentes)} {}

	std::span<const ComponenteFormula> componentes() const noexcept { return componentes_; }
	std::size_t cantidad() const noexcept { return componentes_.size(); }

	// El componente con ese simbolo exacto, o nullptr si no aparece.
	const ComponenteFormula *componente(std::string_view simbolo) const noexcept;

	// El unico componente cuyo simbolo no esta entre los dados. Devuelve
	// nullptr si hay mas de uno o ninguno, lo que permite escribir
	// "el metal es lo que no es oxigeno" sin repetir el bucle en cada
	// funcion de nomenclatura.
	const ComponenteFormula *unicoDistintoDe(std::initializer_list<std::string_view> simbolos) const noexcept;

private:
	std::vector<ComponenteFormula> componentes_;
};

// Analiza una formula quimica simple ("Fe2O3", "NaCl", "Ca(OH)2",
// "Al2(SO4)3"). Un simbolo sin subindice explicito vale 1, y un grupo entre
// parentesis se guarda como un unico componente con el contenido del grupo
// como simbolo.
Resultado<Formula, ErrorFormula> parsearFormula(std::string_view texto);

} // namespace cheminator

#endif // CHEMINATOR_FORMULA_HPP
