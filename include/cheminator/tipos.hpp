#ifndef CHEMINATOR_TIPOS_HPP
#define CHEMINATOR_TIPOS_HPP

#include <compare>
#include <string_view>
#include <variant>

namespace cheminator {

// Un entero con etiqueta: dos magnitudes distintas no se pueden mezclar por
// accidente aunque ambas se representen internamente con un entero.
//
// El motivo es concreto, no teorico. Durante el desarrollo de este proyecto
// se introdujeron tres bugs distintos por confundir una valencia con un
// subindice y un subindice con una carga, todos invisibles para el
// compilador mientras las tres magnitudes eran `int`. Con tipos separados,
// esas confusiones dejan de compilar.
template <typename Etiqueta>
class Magnitud {
public:
	constexpr Magnitud() = default;
	constexpr explicit Magnitud(int valor) noexcept : valor_{valor} {}

	constexpr int valor() const noexcept { return valor_; }

	friend constexpr bool operator==(Magnitud, Magnitud) noexcept = default;
	friend constexpr std::strong_ordering operator<=>(Magnitud, Magnitud) noexcept = default;

private:
	int valor_ = 0;
};

namespace etiquetas {
struct Valencia {};
struct Subindice {};
struct Carga {};
} // namespace etiquetas

// Con cuantos enlaces actua un elemento. Siempre positiva en este programa:
// el signo lo determina el contexto (un no metal frente al oxigeno actua
// positivo; frente al hidrogeno, negativo).
using Valencia = Magnitud<etiquetas::Valencia>;

// Cuantos atomos o grupos de una misma especie aparecen en la formula.
using Subindice = Magnitud<etiquetas::Subindice>;

// Carga de un radical poliatomico, en valor absoluto (el sulfato, SO4(2-),
// tiene Carga{2}).
using Carga = Magnitud<etiquetas::Carga>;

// Carga total aportada por un conjunto de atomos o grupos: cuantos hay, por
// cuanto aporta cada uno. Las dos sobrecargas permiten escribir el balance
// "lo positivo iguala a lo negativo" tal como se razona en quimica, y
// devuelven un entero simple porque lo unico que se hace con el resultado es
// compararlo contra el otro lado de la ecuacion.
constexpr int cargaTotal(Subindice cantidad, Valencia valencia) noexcept
{
	return cantidad.valor() * valencia.valor();
}

constexpr int cargaTotal(Subindice cantidad, Carga carga) noexcept
{
	return cantidad.valor() * carga.valor();
}

constexpr int maximoComunDivisor(int a, int b) noexcept
{
	while (b != 0)
	{
		const int resto = a % b;
		a = b;
		b = resto;
	}
	return a;
}

// Los subindices con que dos especies se combinan en una formula.
struct ParSubindices {
	Subindice primero;
	Subindice segundo;

	friend constexpr bool operator==(ParSubindices, ParSubindices) noexcept = default;
};

// Regla de intercambio de valencias: dos elementos A y B de valencias `a` y
// `b` se combinan como A(b) B(a), con los subindices reducidos por su maximo
// comun divisor. Es la regla que rige oxidos y anhidridos.
//
// Esta escrita como funcion con nombre, y no como aritmetica suelta en cada
// sitio donde hace falta, porque es justamente donde se cometieron los
// errores de calculo: teniendola en un solo lugar se prueba una sola vez.
constexpr ParSubindices intercambiarValencias(Valencia a, Valencia b) noexcept
{
	const int mcd = maximoComunDivisor(a.valor(), b.valor());
	return ParSubindices{Subindice{b.valor() / mcd}, Subindice{a.valor() / mcd}};
}

// Numero romano de una valencia, para la nomenclatura Stock.
constexpr std::string_view aRomano(Valencia valencia) noexcept
{
	switch (valencia.valor())
	{
		case 1: return "I";
		case 2: return "II";
		case 3: return "III";
		case 4: return "IV";
		case 5: return "V";
		case 6: return "VI";
		case 7: return "VII";
		default: return "?";
	}
}

// Un valor o un error, sin excepciones y sin parametros de salida.
// Equivale a std::expected (C++23), reimplementado aqui para no exigir esa
// version del estandar: la libreria apunta a compilarse tambien a
// WebAssembly, donde conviene no depender de lo mas reciente.
template <typename T, typename E>
class Resultado {
public:
	constexpr Resultado(T valor) : contenido_{std::move(valor)} {}
	constexpr Resultado(E error) : contenido_{error} {}

	constexpr bool ok() const noexcept { return std::holds_alternative<T>(contenido_); }
	constexpr explicit operator bool() const noexcept { return ok(); }

	constexpr const T &valor() const { return std::get<T>(contenido_); }
	constexpr T &valor() { return std::get<T>(contenido_); }
	constexpr E error() const { return std::get<E>(contenido_); }

private:
	std::variant<T, E> contenido_;
};

} // namespace cheminator

#endif // CHEMINATOR_TIPOS_HPP
