#include "cheminator/formulacion.hpp"

#include "afijos.hpp"
#include "cheminator/elementos.hpp"
#include "cheminator/hidracidos.hpp"
#include "cheminator/no_metales.hpp"
#include "cheminator/oxacidos.hpp"
#include "cheminator/radicales.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cheminator {
namespace {

// ---------------------------------------------------------------------------
// Texto
// ---------------------------------------------------------------------------

// Las tablas guardan los nombres sin tildes ("Hierro", "oxido"), asi que
// normalizar consiste en dejar la entrada en esa misma forma. Se trabaja sobre
// los bytes de UTF-8 porque las vocales acentuadas del espanol ocupan dos.
std::string quitarTildes(std::string_view texto)
{
	static constexpr std::pair<std::string_view, char> EQUIVALENCIAS[] = {
		{"á", 'a'}, {"é", 'e'}, {"í", 'i'}, {"ó", 'o'}, {"ú", 'u'}, {"ü", 'u'},
		{"Á", 'a'}, {"É", 'e'}, {"Í", 'i'}, {"Ó", 'o'}, {"Ú", 'u'}, {"Ü", 'u'},
		{"ñ", 'n'}, {"Ñ", 'n'},
	};

	std::string salida;
	salida.reserve(texto.size());

	for (std::size_t i = 0; i < texto.size();)
	{
		bool reemplazado = false;

		// Los caracteres multibyte se comparan antes que los de un solo byte.
		if (static_cast<unsigned char>(texto[i]) >= 0x80)
		{
			for (const auto &[acentuada, llana] : EQUIVALENCIAS)
			{
				if (texto.compare(i, acentuada.size(), acentuada) == 0)
				{
					salida.push_back(llana);
					i += acentuada.size();
					reemplazado = true;
					break;
				}
			}
		}

		if (!reemplazado)
		{
			salida.push_back(texto[i]);
			++i;
		}
	}

	return salida;
}

} // namespace

std::string normalizarNombre(std::string_view texto)
{
	const std::string sinTildes = quitarTildes(texto);

	std::string salida;
	salida.reserve(sinTildes.size());

	bool espacioPendiente = false;
	for (const unsigned char caracter : sinTildes)
	{
		if (std::isspace(caracter))
		{
			// Los espacios de mas se colapsan, y los de los extremos se
			// descartan por no llegar a escribirse nunca.
			espacioPendiente = !salida.empty();
			continue;
		}

		if (espacioPendiente)
		{
			salida.push_back(' ');
			espacioPendiente = false;
		}
		salida.push_back(static_cast<char>(std::tolower(caracter)));
	}

	return salida;
}

namespace {

// ---------------------------------------------------------------------------
// Numeros romanos
// ---------------------------------------------------------------------------

// Solo hacen falta del I al VII: ninguna valencia de las tablas llega mas
// alto, y aceptar mas seria admitir nombres que no se pueden formular.
constexpr std::string_view ROMANOS[] = {"i", "ii", "iii", "iv", "v", "vi", "vii"};

std::optional<Valencia> desdeRomano(std::string_view romano)
{
	for (std::size_t i = 0; i < std::size(ROMANOS); ++i)
	{
		if (ROMANOS[i] == romano)
		{
			return Valencia{static_cast<int>(i) + 1};
		}
	}
	return std::nullopt;
}

// Separa el numero romano entre parentesis del resto del nombre. De
// "hierro (iii)" devuelve {"hierro", Valencia{3}}.
struct NombreYValencia {
	std::string nombre;
	std::optional<Valencia> valencia;
	bool parentesisMalFormado = false;
};

NombreYValencia separarValencia(std::string_view texto)
{
	const std::size_t apertura = texto.find('(');
	if (apertura == std::string_view::npos)
	{
		return NombreYValencia{std::string{texto}, std::nullopt, false};
	}

	const std::size_t cierre = texto.find(')', apertura);
	if (cierre == std::string_view::npos || cierre != texto.size() - 1)
	{
		return NombreYValencia{std::string{texto}, std::nullopt, true};
	}

	const std::string_view interior = texto.substr(apertura + 1, cierre - apertura - 1);
	const std::optional<Valencia> valencia = desdeRomano(interior);
	if (!valencia)
	{
		return NombreYValencia{std::string{texto}, std::nullopt, true};
	}

	// Se recorta tambien el espacio que precede al parentesis.
	std::string_view sinValencia = texto.substr(0, apertura);
	while (!sinValencia.empty() && sinValencia.back() == ' ')
	{
		sinValencia.remove_suffix(1);
	}

	return NombreYValencia{std::string{sinValencia}, valencia, false};
}

// ---------------------------------------------------------------------------
// Busqueda en las tablas, por nombre en vez de por simbolo
// ---------------------------------------------------------------------------

// Las tablas se recorren enteras porque son pequenas (15 metales, 7 no
// metales, 20 radicales) y porque buscar por nombre no es la direccion para la
// que estan indexadas.
const InfoElemento *elementoPorNombre(std::string_view nombre)
{
	for (const InfoElemento &elemento : todosLosElementos())
	{
		if (normalizarNombre(elemento.nombre) == nombre)
		{
			return &elemento;
		}
	}
	return nullptr;
}

const InfoRadical *radicalPorNombre(std::string_view nombre)
{
	for (const InfoRadical &radical : todosLosRadicales())
	{
		if (normalizarNombre(radical.nombre) == nombre)
		{
			return &radical;
		}
	}
	return nullptr;
}

const InfoOxacido *oxacidoPorNombre(std::string_view nombre)
{
	for (const InfoOxacido &oxacido : todosLosOxacidos())
	{
		if (normalizarNombre(oxacido.nombre) == nombre)
		{
			return &oxacido;
		}
	}
	return nullptr;
}

// ---------------------------------------------------------------------------
// Construccion de la formula
// ---------------------------------------------------------------------------

// Escribe un simbolo con su subindice, omitiendo el 1 por convencion: "Fe2",
// pero "Na" y no "Na1".
std::string conSubindice(std::string_view simbolo, Subindice subindice)
{
	if (subindice == Subindice{1})
	{
		return std::string{simbolo};
	}
	return std::format("{}{}", simbolo, subindice.valor());
}

// Igual, pero envolviendo el grupo en parentesis cuando lleva subindice, que
// es como se escriben los radicales y el hidroxilo: "(OH)2", pero "OH".
std::string grupoConSubindice(std::string_view grupo, Subindice subindice)
{
	if (subindice == Subindice{1})
	{
		return std::string{grupo};
	}
	return std::format("({}){}", grupo, subindice.valor());
}

class Pasos {
public:
	template <typename... Args>
	void agregar(std::format_string<Args...> formato, Args &&...args)
	{
		lista_.push_back(std::format(formato, std::forward<Args>(args)...));
	}

	std::vector<std::string> extraer() { return std::move(lista_); }

private:
	std::vector<std::string> lista_;
};

Formulacion armar(std::string formula, CategoriaCompuesto categoria, Pasos &pasos)
{
	return Formulacion{std::move(formula), categoria, pasos.extraer()};
}

// Elige la valencia con la que trabajar: la que se indico entre parentesis, o
// la unica que tenga el elemento si no hay ambiguedad.
Resultado<Valencia, ErrorFormulacion> resolverValencia(const InfoElemento &elemento,
                                                        std::optional<Valencia> indicada, Pasos &pasos)
{
	if (indicada)
	{
		if (!elemento.admite(*indicada))
		{
			return ErrorFormulacion::VALENCIA_INVALIDA;
		}
		pasos.agregar("La valencia indicada entre parentesis es {}.", indicada->valor());
		return *indicada;
	}

	if (!elemento.tieneUnicaValencia())
	{
		// Sin numero romano no hay forma de saber de cual de las valencias se
		// habla, y adivinar daria una formula distinta de la esperada.
		return ErrorFormulacion::FALTA_VALENCIA;
	}

	const Valencia unica = elemento.valenciasConocidas().front();
	pasos.agregar("{} tiene una unica valencia ({}), asi que no hace falta indicarla.", elemento.nombre,
	              unica.valor());
	return unica;
}

// ---------------------------------------------------------------------------
// Una funcion por categoria, en el mismo orden que en nomenclatura.cpp
// ---------------------------------------------------------------------------

// "oxido de hierro (iii)" -> Fe2O3
ResultadoFormulacion formularOxido(std::string_view resto)
{
	Pasos pasos;

	const NombreYValencia partes = separarValencia(resto);
	if (partes.parentesisMalFormado)
	{
		return ErrorFormulacion::NOMBRE_MAL_FORMADO;
	}

	const InfoElemento *elemento = elementoPorNombre(partes.nombre);
	if (elemento == nullptr)
	{
		return ErrorFormulacion::ELEMENTO_DESCONOCIDO;
	}
	pasos.agregar("El metal es {} ({}).", elemento->nombre, elemento->simbolo);

	const Resultado<Valencia, ErrorFormulacion> valencia = resolverValencia(*elemento, partes.valencia, pasos);
	if (!valencia)
	{
		return valencia.error();
	}

	// El oxigeno actua con valencia 2 y se aplica la regla de intercambio.
	const ParSubindices subindices = intercambiarValencias(valencia.valor(), Valencia{2});
	pasos.agregar("Se intercambian las valencias con el oxigeno (valencia 2) y se reduce: {}{} y O{}.",
	              elemento->simbolo, subindices.primero.valor(), subindices.segundo.valor());

	const std::string formula =
		conSubindice(elemento->simbolo, subindices.primero) + conSubindice("O", subindices.segundo);

	return armar(formula, CategoriaCompuesto::OXIDO, pasos);
}

// "peroxido de sodio" -> Na2O2
ResultadoFormulacion formularPeroxido(std::string_view resto)
{
	Pasos pasos;

	const NombreYValencia partes = separarValencia(resto);
	if (partes.parentesisMalFormado)
	{
		return ErrorFormulacion::NOMBRE_MAL_FORMADO;
	}

	const InfoElemento *elemento = elementoPorNombre(partes.nombre);
	if (elemento == nullptr)
	{
		return ErrorFormulacion::ELEMENTO_DESCONOCIDO;
	}
	pasos.agregar("El metal es {} ({}).", elemento->nombre, elemento->simbolo);

	const Resultado<Valencia, ErrorFormulacion> valencia = resolverValencia(*elemento, partes.valencia, pasos);
	if (!valencia)
	{
		return valencia.error();
	}

	// El grupo peroxido (O2) actua como una unidad de carga 2-, asi que se
	// cruza con la valencia del metal igual que un radical. Lo que NO se hace
	// es reducir despues el oxigeno: el grupo es indivisible, y por eso el
	// sodio da Na2O2 y no NaO.
	//
	// Cruzar en vez de fijar el metal en 2 es lo que distingue Na2O2 (valencia
	// 1) de CaO2 (valencia 2), que con un 2 fijo habria dado Ca2O4.
	const ParSubindices cruce = intercambiarValencias(valencia.valor(), Valencia{2});
	const Subindice subMetal = cruce.primero;
	const Subindice gruposPeroxo = cruce.segundo;
	const Subindice subOxigeno{2 * gruposPeroxo.valor()};
	pasos.agregar("El grupo peroxido (O2)(2-) se cruza con la valencia {}: {}{} y {} grupo(s) O2 = O{}.",
	              valencia.valor().valor(), elemento->simbolo, subMetal.valor(), gruposPeroxo.valor(),
	              subOxigeno.valor());

	const std::string formula = conSubindice(elemento->simbolo, subMetal) + conSubindice("O", subOxigeno);

	return armar(formula, CategoriaCompuesto::PEROXIDO, pasos);
}

// "hidroxido de calcio" -> Ca(OH)2
ResultadoFormulacion formularBase(std::string_view resto)
{
	Pasos pasos;

	const NombreYValencia partes = separarValencia(resto);
	if (partes.parentesisMalFormado)
	{
		return ErrorFormulacion::NOMBRE_MAL_FORMADO;
	}

	const InfoElemento *elemento = elementoPorNombre(partes.nombre);
	if (elemento == nullptr)
	{
		return ErrorFormulacion::ELEMENTO_DESCONOCIDO;
	}
	pasos.agregar("El metal es {} ({}).", elemento->nombre, elemento->simbolo);

	const Resultado<Valencia, ErrorFormulacion> valencia = resolverValencia(*elemento, partes.valencia, pasos);
	if (!valencia)
	{
		return valencia.error();
	}

	// El hidroxilo tiene carga 1-, asi que hacen falta tantos como indique la
	// valencia del metal.
	const Subindice hidroxilos{valencia.valor().valor()};
	pasos.agregar("El grupo hidroxilo (OH)(1-) se repite {} vez/veces.", hidroxilos.valor());

	const std::string formula = std::string{elemento->simbolo} + grupoConSubindice("OH", hidroxilos);

	return armar(formula, CategoriaCompuesto::BASE, pasos);
}

// "acido clorhidrico" -> HCl, y "acido sulfurico" -> H2SO4
ResultadoFormulacion formularAcido(std::string_view resto)
{
	Pasos pasos;

	// Los oxacidos estan tabulados con su nombre completo, asi que se prueban
	// primero: es una busqueda exacta y no deja lugar a ambiguedad.
	const std::string nombreCompleto = std::format("acido {}", resto);
	if (const InfoOxacido *oxacido = oxacidoPorNombre(nombreCompleto))
	{
		pasos.agregar("\"{}\" esta en la tabla de acidos oxacidos.", nombreCompleto);
		pasos.agregar("Su formula combina hidrogeno, {} y oxigeno.", oxacido->simboloNoMetal);

		const std::string formula = conSubindice("H", oxacido->hidrogenos) +
		                            conSubindice(oxacido->simboloNoMetal, oxacido->atomosNoMetal) +
		                            conSubindice("O", oxacido->oxigenos);

		return armar(formula, CategoriaCompuesto::ACIDO_OXACIDO, pasos);
	}

	// Un hidracido se nombra "acido <raiz>hidrico".
	constexpr std::string_view SUFIJO = "hidrico";
	if (resto.size() > SUFIJO.size() && resto.ends_with(SUFIJO))
	{
		const std::string_view raiz = resto.substr(0, resto.size() - SUFIJO.size());

		for (const InfoHidracido &hidracido : todosLosHidracidos())
		{
			if (normalizarNombre(hidracido.raiz) != raiz)
			{
				continue;
			}

			pasos.agregar("La raiz \"{}\" con el sufijo \"-hidrico\" corresponde a {}.", raiz,
			              hidracido.simbolo);
			pasos.agregar("{} lleva {} hidrogeno(s) en el hidracido.", hidracido.simbolo,
			              hidracido.hidrogenos.valor());

			const std::string formula =
				conSubindice("H", hidracido.hidrogenos) + std::string{hidracido.simbolo};

			return armar(formula, CategoriaCompuesto::ACIDO_HIDRACIDO, pasos);
		}

		return ErrorFormulacion::ELEMENTO_DESCONOCIDO;
	}

	return ErrorFormulacion::CATEGORIA_DESCONOCIDA;
}

// "anhidrido sulfurico" -> SO3
ResultadoFormulacion formularAnhidrido(std::string_view resto)
{
	Pasos pasos;

	// El nombre lleva prefijo y sufijo segun la posicion de la valencia, asi
	// que se genera el nombre que tendria cada no metal con cada una de sus
	// valencias y se compara. Se usa la MISMA funcion de afijos que
	// nomenclatura.cpp, de modo que invertir no puede desincronizarse de
	// nombrar.
	for (const InfoNoMetal &noMetal : todosLosNoMetales())
	{
		const std::span<const Valencia> valencias = noMetal.valenciasConocidas();

		for (std::size_t i = 0; i < valencias.size(); ++i)
		{
			const detalle::Afijos afijos =
				detalle::afijosTradicionales(static_cast<int>(i), noMetal.cantidadValencias);
			const std::string candidato =
				normalizarNombre(std::format("{}{}{}", afijos.prefijo, noMetal.raizPara(afijos.sufijo), afijos.sufijo));

			if (candidato != resto)
			{
				continue;
			}

			const Valencia valencia = valencias[i];
			pasos.agregar("El no metal es {} ({}), con valencia {}.", noMetal.nombre, noMetal.simbolo,
			              valencia.valor());

			const ParSubindices subindices = intercambiarValencias(valencia, Valencia{2});
			pasos.agregar("Se intercambian las valencias con el oxigeno (valencia 2) y se reduce: {}{} y O{}.",
			              noMetal.simbolo, subindices.primero.valor(), subindices.segundo.valor());

			const std::string formula =
				conSubindice(noMetal.simbolo, subindices.primero) + conSubindice("O", subindices.segundo);

			return armar(formula, CategoriaCompuesto::ANHIDRIDO, pasos);
		}
	}

	return ErrorFormulacion::ELEMENTO_DESCONOCIDO;
}

// "sulfato de aluminio" -> Al2(SO4)3
ResultadoFormulacion formularSal(std::string_view radicalTexto, std::string_view metalTexto)
{
	Pasos pasos;

	const InfoRadical *radical = radicalPorNombre(radicalTexto);
	if (radical == nullptr)
	{
		return ErrorFormulacion::RADICAL_DESCONOCIDO;
	}
	pasos.agregar("El radical \"{}\" es {} con carga {}-.", radical->nombre, radical->formula,
	              radical->carga.valor());

	const NombreYValencia partes = separarValencia(metalTexto);
	if (partes.parentesisMalFormado)
	{
		return ErrorFormulacion::NOMBRE_MAL_FORMADO;
	}

	const InfoElemento *elemento = elementoPorNombre(partes.nombre);
	if (elemento == nullptr)
	{
		return ErrorFormulacion::ELEMENTO_DESCONOCIDO;
	}
	pasos.agregar("El metal es {} ({}).", elemento->nombre, elemento->simbolo);

	const Resultado<Valencia, ErrorFormulacion> valencia = resolverValencia(*elemento, partes.valencia, pasos);
	if (!valencia)
	{
		return valencia.error();
	}

	// Se cruzan la valencia del metal y la carga del radical, reduciendo por el
	// maximo comun divisor igual que en los oxidos.
	const ParSubindices subindices =
		intercambiarValencias(valencia.valor(), Valencia{radical->carga.valor()});
	pasos.agregar("Se cruzan la valencia del metal ({}) y la carga del radical ({}): {}{} y {}{}.",
	              valencia.valor().valor(), radical->carga.valor(), elemento->simbolo,
	              subindices.primero.valor(), radical->formula, subindices.segundo.valor());

	const std::string formula =
		conSubindice(elemento->simbolo, subindices.primero) + grupoConSubindice(radical->formula, subindices.segundo);

	return armar(formula, CategoriaCompuesto::SAL_OXISAL, pasos);
}

// Separa "<algo> de <algo>" en sus dos mitades.
struct DosPartes {
	std::string_view primera;
	std::string_view segunda;
	bool valido = false;
};

DosPartes separarPorDe(std::string_view texto)
{
	constexpr std::string_view SEPARADOR = " de ";
	const std::size_t posicion = texto.find(SEPARADOR);
	if (posicion == std::string_view::npos)
	{
		return DosPartes{};
	}
	return DosPartes{texto.substr(0, posicion), texto.substr(posicion + SEPARADOR.size()), true};
}

} // namespace

std::string_view mensajeError(ErrorFormulacion error) noexcept
{
	switch (error)
	{
	case ErrorFormulacion::VACIO:
		return "No se escribio ningun nombre.";
	case ErrorFormulacion::CATEGORIA_DESCONOCIDA:
		return "El nombre no corresponde a ninguna de las categorias que reconoce la libreria.";
	case ErrorFormulacion::ELEMENTO_DESCONOCIDO:
		return "El elemento nombrado no esta en las tablas de la libreria.";
	case ErrorFormulacion::RADICAL_DESCONOCIDO:
		return "El radical nombrado no esta en la tabla de radicales.";
	case ErrorFormulacion::FALTA_VALENCIA:
		return "El elemento admite varias valencias: hay que indicarla con un numero romano, por ejemplo "
		       "\"oxido de Hierro (III)\".";
	case ErrorFormulacion::VALENCIA_INVALIDA:
		return "La valencia indicada no es una de las que admite ese elemento.";
	case ErrorFormulacion::NOMBRE_MAL_FORMADO:
		return "El nombre no tiene la forma esperada.";
	}
	return "Error desconocido.";
}

ResultadoFormulacion formular(std::string_view nombre)
{
	const std::string normalizado = normalizarNombre(nombre);
	if (normalizado.empty())
	{
		return ErrorFormulacion::VACIO;
	}

	// El orden importa: "peroxido" tiene que probarse antes que "oxido",
	// porque termina en esa misma palabra.
	if (normalizado.starts_with("peroxido de "))
	{
		return formularPeroxido(std::string_view{normalizado}.substr(12));
	}

	if (normalizado.starts_with("oxido de "))
	{
		return formularOxido(std::string_view{normalizado}.substr(9));
	}

	if (normalizado.starts_with("hidroxido de "))
	{
		return formularBase(std::string_view{normalizado}.substr(13));
	}

	if (normalizado.starts_with("anhidrido "))
	{
		return formularAnhidrido(std::string_view{normalizado}.substr(10));
	}

	if (normalizado.starts_with("acido "))
	{
		return formularAcido(std::string_view{normalizado}.substr(6));
	}

	// Lo que queda con forma "<radical> de <metal>" se intenta como sal, que
	// es la unica categoria que no lleva palabra inicial propia.
	const DosPartes partes = separarPorDe(normalizado);
	if (partes.valido)
	{
		return formularSal(partes.primera, partes.segunda);
	}

	return ErrorFormulacion::CATEGORIA_DESCONOCIDA;
}

} // namespace cheminator
