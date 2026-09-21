#include "cheminator/formula.hpp"

#include <algorithm>
#include <cctype>

namespace cheminator {
namespace {

constexpr bool esMayuscula(char c) noexcept { return c >= 'A' && c <= 'Z'; }
constexpr bool esMinuscula(char c) noexcept { return c >= 'a' && c <= 'z'; }
constexpr bool esDigito(char c) noexcept { return c >= '0' && c <= '9'; }

// Lee los digitos que siguen a la posicion dada y devuelve el subindice que
// representan. Sin digitos, el subindice es 1, como en la notacion quimica.
struct LecturaSubindice {
	Subindice subindice;
	std::size_t siguiente;
	bool valido;
};

LecturaSubindice leerSubindice(std::string_view texto, std::size_t desde)
{
	int valor = 0;
	std::size_t i = desde;
	while (i < texto.size() && esDigito(texto[i]))
	{
		valor = valor * 10 + (texto[i] - '0');
		++i;
	}

	if (i == desde)
	{
		return {Subindice{1}, i, true};
	}
	// Se escribieron digitos pero suman cero ("H0"), que no es una formula valida.
	return {Subindice{valor}, i, valor > 0};
}

} // namespace

std::string_view mensajeError(ErrorFormula error) noexcept
{
	switch (error)
	{
		case ErrorFormula::VACIA:
			return "La formula esta vacia.";
		case ErrorFormula::SIMBOLO_INVALIDO:
			return "La formula contiene un simbolo invalido (debe iniciar con mayuscula, ej. \"Fe2O3\").";
		case ErrorFormula::SUBINDICE_INVALIDO:
			return "La formula contiene un subindice invalido.";
		case ErrorFormula::DEMASIADOS_COMPONENTES:
			return "La formula tiene mas elementos distintos de los soportados.";
		case ErrorFormula::GRUPO_MAL_FORMADO:
			return "La formula tiene un grupo mal formado (sin cerrar, vacio o con los delimitadores cruzados).";
		case ErrorFormula::DEMASIADO_ANIDADO:
			return "La formula anida mas grupos de los que la libreria admite.";
		case ErrorFormula::CARGA_MAL_FORMADA:
			return "La carga al final de la formula no tiene la forma esperada (ej. \"3-\" o \"2+\").";
	}
	return "Error desconocido.";
}

const ComponenteFormula *Formula::componente(std::string_view simbolo) const noexcept
{
	const auto it = std::find_if(componentes_.begin(), componentes_.end(),
	                              [simbolo](const ComponenteFormula &c) { return c.simbolo == simbolo; });
	return it == componentes_.end() ? nullptr : &*it;
}

const ComponenteFormula *Formula::unicoDistintoDe(std::initializer_list<std::string_view> simbolos) const noexcept
{
	const ComponenteFormula *encontrado = nullptr;

	for (const ComponenteFormula &c : componentes_)
	{
		const bool excluido = std::find(simbolos.begin(), simbolos.end(), std::string_view{c.simbolo}) != simbolos.end();
		if (excluido)
		{
			continue;
		}
		if (encontrado != nullptr)
		{
			return nullptr; // hay mas de uno
		}
		encontrado = &c;
	}

	return encontrado;
}

const NodoFormula *Formula::esferaDeCoordinacion() const noexcept
{
	for (const NodoFormula &nodo : raices_)
	{
		if (nodo.delimitador == Delimitador::CORCHETE)
		{
			return &nodo;
		}
	}
	return nullptr;
}

namespace {

// Analiza una secuencia de nodos hasta agotar el texto o encontrar el cierre
// que se espera. Se llama a si misma para el contenido de cada grupo, que es
// lo que da el anidamiento: en "[Fe(CN)6]" el corchete se analiza con esta
// misma funcion, y dentro encuentra el parentesis.
//
// `cierre` es el caracter que termina este nivel ('\0' en el nivel de arriba).
struct ResultadoNivel {
	std::vector<NodoFormula> nodos;
	std::size_t siguiente = 0;
	ErrorFormula error{};
	bool ok = false;
};

ResultadoNivel analizarNivel(std::string_view texto, std::size_t desde, char cierre, int profundidad)
{
	ResultadoNivel salida;

	if (profundidad > MAX_PROFUNDIDAD)
	{
		salida.error = ErrorFormula::DEMASIADO_ANIDADO;
		return salida;
	}

	std::size_t i = desde;

	while (i < texto.size() && texto[i] != cierre)
	{
		NodoFormula nodo;

		const char apertura = texto[i];
		if (apertura == '(' || apertura == '[')
		{
			const char esperado = (apertura == '(') ? ')' : ']';
			const std::size_t inicio = i + 1;

			// El contenido del grupo se analiza con esta misma funcion, un
			// nivel mas abajo.
			ResultadoNivel dentro = analizarNivel(texto, inicio, esperado, profundidad + 1);
			if (!dentro.ok)
			{
				salida.error = dentro.error;
				return salida;
			}
			if (dentro.siguiente >= texto.size() || texto[dentro.siguiente] != esperado)
			{
				salida.error = ErrorFormula::GRUPO_MAL_FORMADO; // sin cerrar, o cruzado
				return salida;
			}
			if (dentro.nodos.empty())
			{
				salida.error = ErrorFormula::GRUPO_MAL_FORMADO; // grupo vacio
				return salida;
			}

			// El simbolo del grupo es su contenido literal, que es lo que
			// espera la nomenclatura de bases y sales ("OH", "SO4").
			nodo.simbolo.assign(texto.substr(inicio, dentro.siguiente - inicio));
			nodo.delimitador = (apertura == '(') ? Delimitador::PARENTESIS : Delimitador::CORCHETE;
			nodo.hijos = std::move(dentro.nodos);

			i = dentro.siguiente + 1; // saltar el cierre
		}
		else if (esMayuscula(apertura))
		{
			// Un simbolo empieza en mayuscula y puede llevar una segunda
			// letra minuscula ("Fe", "Na"), como en la notacion real.
			const std::size_t inicio = i;
			++i;
			if (i < texto.size() && esMinuscula(texto[i]))
			{
				++i;
			}
			nodo.simbolo.assign(texto.substr(inicio, i - inicio));
		}
		else if (apertura == ')' || apertura == ']')
		{
			// Un cierre que no es el que este nivel esperaba: o sobra, o los
			// delimitadores estan cruzados ("Ca[OH)2"). Se distingue de un
			// simbolo invalido porque el problema es el grupo, y decir
			// "simbolo invalido" mandaria a buscar en el sitio equivocado.
			salida.error = ErrorFormula::GRUPO_MAL_FORMADO;
			return salida;
		}
		else
		{
			salida.error = ErrorFormula::SIMBOLO_INVALIDO;
			return salida;
		}

		const LecturaSubindice lectura = leerSubindice(texto, i);
		if (!lectura.valido)
		{
			salida.error = ErrorFormula::SUBINDICE_INVALIDO;
			return salida;
		}
		i = lectura.siguiente;
		nodo.subindice = lectura.subindice;

		salida.nodos.push_back(std::move(nodo));
	}

	salida.siguiente = i;
	salida.ok = true;
	return salida;
}

// Separa el sufijo de carga del final ("3-", "2+", "-"). Devuelve el texto sin
// el sufijo y la carga leida, con signo.
struct TextoYCarga {
	std::string_view texto;
	Carga carga{0};
	bool valido = true;
};

TextoYCarga separarCarga(std::string_view texto)
{
	if (texto.empty())
	{
		return TextoYCarga{texto, Carga{0}, true};
	}

	const char signo = texto.back();
	if (signo != '+' && signo != '-')
	{
		return TextoYCarga{texto, Carga{0}, true};
	}

	std::string_view resto = texto.substr(0, texto.size() - 1);

	// Los digitos que preceden al signo son la magnitud; sin ellos vale 1.
	std::size_t fin = resto.size();
	while (fin > 0 && esDigito(resto[fin - 1]))
	{
		--fin;
	}

	int magnitud = 1;
	if (fin < resto.size())
	{
		magnitud = 0;
		for (std::size_t k = fin; k < resto.size(); ++k)
		{
			magnitud = magnitud * 10 + (resto[k] - '0');
		}
		if (magnitud == 0)
		{
			return TextoYCarga{texto, Carga{0}, false};
		}
	}

	resto = resto.substr(0, fin);
	if (resto.empty())
	{
		return TextoYCarga{texto, Carga{0}, false}; // solo el signo, sin formula
	}

	return TextoYCarga{resto, Carga{signo == '-' ? -magnitud : magnitud}, true};
}

} // namespace

Resultado<Formula, ErrorFormula> parsearFormula(std::string_view texto)
{
	if (texto.empty())
	{
		return ErrorFormula::VACIA;
	}

	const TextoYCarga conCarga = separarCarga(texto);
	if (!conCarga.valido)
	{
		return ErrorFormula::CARGA_MAL_FORMADA;
	}
	if (conCarga.texto.empty())
	{
		return ErrorFormula::VACIA;
	}

	ResultadoNivel nivel = analizarNivel(conCarga.texto, 0, '\0', 1);
	if (!nivel.ok)
	{
		return nivel.error;
	}
	// Si sobro texto es porque aparecio un cierre sin su apertura.
	if (nivel.siguiente != conCarga.texto.size())
	{
		return ErrorFormula::GRUPO_MAL_FORMADO;
	}
	if (nivel.nodos.empty())
	{
		return ErrorFormula::VACIA;
	}
	if (nivel.nodos.size() > MAX_COMPONENTES)
	{
		return ErrorFormula::DEMASIADOS_COMPONENTES;
	}

	// La vista plana es la primera capa del arbol, que es exactamente lo que
	// devolvia el parser anterior.
	std::vector<ComponenteFormula> componentes;
	componentes.reserve(nivel.nodos.size());
	for (const NodoFormula &nodo : nivel.nodos)
	{
		componentes.push_back(ComponenteFormula{nodo.simbolo, nodo.subindice});
	}

	return Formula{std::move(componentes), std::move(nivel.nodos), conCarga.carga};
}

} // namespace cheminator
