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
			return "La formula tiene un grupo entre parentesis mal formado (sin cerrar, vacio o anidado).";
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

Resultado<Formula, ErrorFormula> parsearFormula(std::string_view texto)
{
	if (texto.empty())
	{
		return ErrorFormula::VACIA;
	}

	std::vector<ComponenteFormula> componentes;
	std::size_t i = 0;

	while (i < texto.size())
	{
		std::string simbolo;

		if (texto[i] == '(')
		{
			// Grupo entre parentesis ("(OH)", "(SO4)"): su contenido literal
			// pasa a ser el simbolo de un unico componente, que es como la
			// nomenclatura de bases y sales trata a un radical.
			++i;
			const std::size_t inicio = i;
			while (i < texto.size() && texto[i] != ')')
			{
				if (texto[i] == '(')
				{
					return ErrorFormula::GRUPO_MAL_FORMADO; // no se admite anidamiento
				}
				++i;
			}
			if (i >= texto.size() || i == inicio)
			{
				return ErrorFormula::GRUPO_MAL_FORMADO; // sin cerrar, o vacio
			}
			simbolo.assign(texto.substr(inicio, i - inicio));
			++i; // saltar ')'
		}
		else if (esMayuscula(texto[i]))
		{
			// Un simbolo empieza en mayuscula y puede llevar una segunda
			// letra minuscula ("Fe", "Na"), como en la notacion real.
			const std::size_t inicio = i;
			++i;
			if (i < texto.size() && esMinuscula(texto[i]))
			{
				++i;
			}
			simbolo.assign(texto.substr(inicio, i - inicio));
		}
		else
		{
			return ErrorFormula::SIMBOLO_INVALIDO;
		}

		const LecturaSubindice lectura = leerSubindice(texto, i);
		if (!lectura.valido)
		{
			return ErrorFormula::SUBINDICE_INVALIDO;
		}
		i = lectura.siguiente;

		if (componentes.size() >= MAX_COMPONENTES)
		{
			return ErrorFormula::DEMASIADOS_COMPONENTES;
		}
		componentes.push_back(ComponenteFormula{std::move(simbolo), lectura.subindice});
	}

	return Formula{std::move(componentes)};
}

} // namespace cheminator
