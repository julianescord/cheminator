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

// Profundidad maxima de anidamiento de grupos. Dos alcanzan para la esfera de
// coordinacion, que es un corchete con parentesis dentro: [Fe(CN)6].
inline constexpr int MAX_PROFUNDIDAD = 3;

enum class ErrorFormula {
	VACIA,
	SIMBOLO_INVALIDO,       // no empieza en mayuscula, o tiene caracteres raros
	SUBINDICE_INVALIDO,     // se escribio un subindice igual a cero
	DEMASIADOS_COMPONENTES,
	GRUPO_MAL_FORMADO,      // delimitador sin cerrar, vacio o mal cruzado
	DEMASIADO_ANIDADO,      // mas niveles de los que admite MAX_PROFUNDIDAD
	CARGA_MAL_FORMADA,      // el sufijo de carga no es de la forma "3-" o "2+"
};

// Como venia delimitado un grupo dentro de la formula. El corchete marca la
// esfera de coordinacion de un complejo, y esa distincion es la que permite
// separar [Fe(CN)6] de un radical cualquiera entre parentesis.
enum class Delimitador {
	NINGUNO,     // un simbolo de elemento suelto
	PARENTESIS,  // (OH), (SO4)
	CORCHETE,    // [Fe(CN)6]
};

// Un nodo del arbol de la formula. Las hojas son elementos y los nodos con
// hijos son grupos. "Fe2O3" da dos hojas; "K3[Fe(CN)6]" da la hoja K3 y un
// nodo corchete que a su vez contiene la hoja Fe y un nodo parentesis (CN)6.
struct NodoFormula {
	std::string simbolo;  // en un grupo, su contenido literal ("OH", "Fe(CN)6")
	Subindice subindice;
	Delimitador delimitador = Delimitador::NINGUNO;
	std::vector<NodoFormula> hijos;

	bool esGrupo() const noexcept { return delimitador != Delimitador::NINGUNO; }
};

std::string_view mensajeError(ErrorFormula error) noexcept;

// Una formula ya analizada.
//
// Guarda dos vistas de lo mismo: la lista plana de componentes, que es la que
// usan las siete categorias de compuesto simple, y el arbol con el anidamiento
// real, que hace falta para los complejos de coordinacion. La vista plana es
// la primera capa del arbol, asi que "Ca(OH)2" sigue dando {Ca,1} y {OH,2}
// como antes, y ademas conserva que OH era un grupo con dos hijos.
class Formula {
public:
	Formula() = default;
	explicit Formula(std::vector<ComponenteFormula> componentes) : componentes_{std::move(componentes)} {}

	Formula(std::vector<ComponenteFormula> componentes, std::vector<NodoFormula> raices, Carga carga)
		: componentes_{std::move(componentes)}, raices_{std::move(raices)}, carga_{carga}
	{
	}

	std::span<const ComponenteFormula> componentes() const noexcept { return componentes_; }
	std::size_t cantidad() const noexcept { return componentes_.size(); }

	// El arbol completo, con el anidamiento que la vista plana aplana.
	std::span<const NodoFormula> raices() const noexcept { return raices_; }

	// Carga global escrita como sufijo ("3-" en "[Fe(CN)6]3-"). Vale 0 cuando
	// la formula es neutra, que es el caso de todo compuesto simple.
	Carga carga() const noexcept { return carga_; }

	// Si la formula es un ion suelto, con carga distinta de cero.
	bool esIon() const noexcept { return carga_ != Carga{0}; }

	// El primer nodo delimitado por corchetes, que es la esfera de
	// coordinacion de un complejo. nullptr si la formula no tiene ninguno.
	const NodoFormula *esferaDeCoordinacion() const noexcept;

	// El componente con ese simbolo exacto, o nullptr si no aparece.
	const ComponenteFormula *componente(std::string_view simbolo) const noexcept;

	// El unico componente cuyo simbolo no esta entre los dados. Devuelve
	// nullptr si hay mas de uno o ninguno, lo que permite escribir
	// "el metal es lo que no es oxigeno" sin repetir el bucle en cada
	// funcion de nomenclatura.
	const ComponenteFormula *unicoDistintoDe(std::initializer_list<std::string_view> simbolos) const noexcept;

private:
	std::vector<ComponenteFormula> componentes_;
	std::vector<NodoFormula> raices_;
	Carga carga_{0};
};

// Analiza una formula quimica ("Fe2O3", "NaCl", "Ca(OH)2", "Al2(SO4)3",
// "K3[Fe(CN)6]", "[Fe(CN)6]3-"). Un simbolo sin subindice explicito vale 1, y
// un grupo delimitado se guarda como un unico componente con el contenido del
// grupo como simbolo, ademas de conservar sus hijos en el arbol.
Resultado<Formula, ErrorFormula> parsearFormula(std::string_view texto);

} // namespace cheminator

#endif // CHEMINATOR_FORMULA_HPP
