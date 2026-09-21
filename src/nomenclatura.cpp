#include "cheminator/nomenclatura.hpp"

#include "afijos.hpp"

#include <array>
#include <format>
#include <optional>
#include <utility>

namespace cheminator {
namespace {

constexpr Valencia VALENCIA_OXIGENO{2};
constexpr std::string_view OXIGENO = "O";
constexpr std::string_view HIDROGENO = "H";
constexpr std::string_view HIDROXILO = "OH";

// Acumula las lineas del razonamiento mientras se nombra un compuesto.
class Pasos {
public:
	template <typename... Args>
	void agregar(std::format_string<Args...> formato, Args &&...args)
	{
		lista_.push_back(std::format(formato, std::forward<Args>(args)...));
	}

	std::vector<std::string> extraer() noexcept { return std::move(lista_); }

private:
	std::vector<std::string> lista_;
};

Nomenclatura armar(std::string nombre, CategoriaCompuesto categoria, Pasos &pasos)
{
	return Nomenclatura{std::move(nombre), categoria, pasos.extraer()};
}

// Nombre en notacion Stock: el numero romano solo aparece cuando el elemento
// admite mas de una valencia, porque con una sola no hay ambiguedad.
std::string nombreStock(std::string_view tipo, const InfoElemento &elemento, Valencia valencia)
{
	if (elemento.tieneUnicaValencia())
	{
		return std::format("{} de {}", tipo, elemento.nombre);
	}
	return std::format("{} de {} ({})", tipo, elemento.nombre, aRomano(valencia));
}

// Prefijo y sufijo tradicionales segun la posicion que ocupa la valencia usada
// dentro de la lista ordenada de valencias del no metal. La regla vive en
// afijos.hpp porque formulacion.cpp la necesita para invertirla.
using detalle::Afijos;
using detalle::afijosTradicionales;

// Busca, entre las valencias del elemento, la que reproduce los subindices de
// la formula segun la regla de intercambio. Comun a oxidos y anhidridos.
std::optional<Valencia> valenciaQueCuadraPorIntercambio(std::span<const Valencia> valencias, Subindice subElemento,
                                                         Subindice subOxigeno)
{
	for (const Valencia valencia : valencias)
	{
		const ParSubindices esperados = intercambiarValencias(valencia, VALENCIA_OXIGENO);
		if (esperados == ParSubindices{subElemento, subOxigeno})
		{
			return valencia;
		}
	}
	return std::nullopt;
}

// Un metal acompanado de un grupo repetido (hidroxilos o radicales), que es
// la forma comun de bases y sales.
struct MetalConGrupos {
	const ComponenteFormula *metal;
	std::string grupo;
	Subindice cantidadGrupos;
};

// Reconoce las dos formas equivalentes en que el parser puede entregar una
// base: "Ca(OH)2" llega como dos componentes con el grupo ya agrupado, y
// "NaOH" como tres sueltos. Sin parentesis solo puede haber un grupo, de ahi
// que se exija que los subindices de O y H coincidan.
std::optional<MetalConGrupos> normalizarBase(const Formula &formula)
{
	if (formula.cantidad() == 2)
	{
		const ComponenteFormula *grupo = formula.componente(HIDROXILO);
		const ComponenteFormula *metal = formula.unicoDistintoDe({HIDROXILO});
		if (grupo == nullptr || metal == nullptr)
		{
			return std::nullopt;
		}
		return MetalConGrupos{metal, std::string{HIDROXILO}, grupo->subindice};
	}

	if (formula.cantidad() == 3)
	{
		const ComponenteFormula *oxigeno = formula.componente(OXIGENO);
		const ComponenteFormula *hidrogeno = formula.componente(HIDROGENO);
		const ComponenteFormula *metal = formula.unicoDistintoDe({OXIGENO, HIDROGENO});
		if (oxigeno == nullptr || hidrogeno == nullptr || metal == nullptr)
		{
			return std::nullopt;
		}
		if (oxigeno->subindice != hidrogeno->subindice)
		{
			return std::nullopt;
		}
		return MetalConGrupos{metal, std::string{HIDROXILO}, oxigeno->subindice};
	}

	return std::nullopt;
}

// Lo mismo para sales: "Al2(SO4)3" llega agrupado y "Na2SO4" suelto. En la
// forma suelta hay que reconstruir la formula del radical a partir del no
// metal y el oxigeno, sin escribir el subindice cuando vale 1 ("ClO", no
// "ClO1"), que es como esta tabulado.
std::optional<MetalConGrupos> normalizarSal(const Formula &formula)
{
	if (formula.cantidad() == 2)
	{
		const ComponenteFormula *metal = nullptr;
		const ComponenteFormula *radical = nullptr;

		for (const ComponenteFormula &componente : formula.componentes())
		{
			if (buscarElemento(componente.simbolo) != nullptr)
			{
				if (metal != nullptr) return std::nullopt;
				metal = &componente;
			}
			else if (buscarNoMetal(componente.simbolo) != nullptr)
			{
				// Un simbolo de elemento suelto nunca es un radical, que por
				// definicion es poliatomico. Sin esta comprobacion, "NaCl" se
				// leeria como sodio mas un supuesto radical "Cl" y el error
				// resultante hablaria de un radical desconocido en vez de
				// decir que la formula no es una sal oxisal.
				return std::nullopt;
			}
			else
			{
				if (radical != nullptr) return std::nullopt;
				radical = &componente;
			}
		}

		if (metal == nullptr || radical == nullptr)
		{
			return std::nullopt;
		}
		return MetalConGrupos{metal, radical->simbolo, radical->subindice};
	}

	if (formula.cantidad() == 3)
	{
		const ComponenteFormula *oxigeno = formula.componente(OXIGENO);
		if (oxigeno == nullptr)
		{
			return std::nullopt;
		}

		const ComponenteFormula *noMetal = nullptr;
		const ComponenteFormula *metal = nullptr;

		for (const ComponenteFormula &componente : formula.componentes())
		{
			if (&componente == oxigeno)
			{
				continue;
			}
			if (buscarNoMetal(componente.simbolo) != nullptr)
			{
				if (noMetal != nullptr) return std::nullopt;
				noMetal = &componente;
			}
			else if (buscarElemento(componente.simbolo) != nullptr)
			{
				if (metal != nullptr) return std::nullopt;
				metal = &componente;
			}
			else
			{
				return std::nullopt;
			}
		}

		if (noMetal == nullptr || metal == nullptr || noMetal->subindice != Subindice{1})
		{
			return std::nullopt;
		}

		std::string formulaRadical = noMetal->simbolo;
		formulaRadical += OXIGENO;
		if (oxigeno->subindice != Subindice{1})
		{
			formulaRadical += std::to_string(oxigeno->subindice.valor());
		}

		return MetalConGrupos{metal, std::move(formulaRadical), Subindice{1}};
	}

	return std::nullopt;
}

} // namespace

std::string_view nombreCategoria(CategoriaCompuesto categoria) noexcept
{
	switch (categoria)
	{
		case CategoriaCompuesto::OXIDO: return "oxido";
		case CategoriaCompuesto::PEROXIDO: return "peroxido";
		case CategoriaCompuesto::ANHIDRIDO: return "anhidrido";
		case CategoriaCompuesto::ACIDO_HIDRACIDO: return "acido hidracido";
		case CategoriaCompuesto::ACIDO_OXACIDO: return "acido oxacido";
		case CategoriaCompuesto::BASE: return "base";
		case CategoriaCompuesto::SAL_OXISAL: return "sal oxisal";
	}
	return "desconocido";
}

std::string_view mensajeError(ErrorNomenclatura error) noexcept
{
	switch (error)
	{
		case ErrorNomenclatura::NINGUNA_CATEGORIA:
			return "La formula no corresponde a ninguna de las categorias de compuesto que reconoce la libreria.";
		case ErrorNomenclatura::NO_ES_OXIDO:
			return "La formula no corresponde a un oxido (se esperaba metal + oxigeno).";
		case ErrorNomenclatura::NO_ES_PEROXIDO:
			return "La formula no corresponde a un peroxido (se esperaba metal + grupo peroxo O2).";
		case ErrorNomenclatura::NO_ES_ANHIDRIDO:
			return "La formula no corresponde a un anhidrido (se esperaba no metal + oxigeno).";
		case ErrorNomenclatura::NO_ES_HIDRACIDO:
			return "La formula no corresponde a un acido hidracido (se esperaba H + no metal en la proporcion correcta).";
		case ErrorNomenclatura::NO_ES_OXACIDO:
			return "La formula no corresponde a un acido oxacido (se esperaba H + no metal + oxigeno).";
		case ErrorNomenclatura::NO_ES_BASE:
			return "La formula no corresponde a una base (se esperaba metal + grupo hidroxilo OH, ej. Ca(OH)2 o NaOH).";
		case ErrorNomenclatura::NO_ES_SAL:
			return "La formula no corresponde a una sal oxisal (se esperaba metal + radical conocido, ej. Al2(SO4)3 o CaCO3).";
		case ErrorNomenclatura::ELEMENTO_DESCONOCIDO:
			return "Algun elemento o radical de la formula no esta en las tablas soportadas.";
		case ErrorNomenclatura::VALENCIA_NO_DETERMINADA:
			return "La proporcion de la formula no corresponde a ninguna valencia conocida.";
	}
	return "Error desconocido.";
}

ResultadoNomenclatura nombrarOxido(const Formula &formula)
{
	if (formula.cantidad() != 2)
	{
		return ErrorNomenclatura::NO_ES_OXIDO;
	}

	const ComponenteFormula *oxigeno = formula.componente(OXIGENO);
	const ComponenteFormula *metal = formula.unicoDistintoDe({OXIGENO});
	if (oxigeno == nullptr || metal == nullptr)
	{
		return ErrorNomenclatura::NO_ES_OXIDO;
	}

	Pasos pasos;
	pasos.agregar("Se identifica el metal ({}, subindice {}) y el oxigeno (subindice {}).", metal->simbolo,
	              metal->subindice.valor(), oxigeno->subindice.valor());

	const InfoElemento *info = buscarElemento(metal->simbolo);
	if (info == nullptr)
	{
		return ErrorNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	pasos.agregar("{} ({}) tiene {} valencia(s) conocida(s) para oxidos.", info->nombre, metal->simbolo,
	              info->cantidadValencias);

	const auto valencia = valenciaQueCuadraPorIntercambio(info->valenciasConocidas(), metal->subindice,
	                                                       oxigeno->subindice);
	if (!valencia)
	{
		return ErrorNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	pasos.agregar("El oxigeno actua con valencia 2; cruzando valencias, la proporcion {}:{} corresponde a la valencia {}.",
	              metal->subindice.valor(), oxigeno->subindice.valor(), valencia->valor());

	if (info->tieneUnicaValencia())
	{
		pasos.agregar("{} tiene una unica valencia, asi que no se indica numero romano.", info->nombre);
	}
	else
	{
		pasos.agregar("{} admite varias valencias, asi que se indica con numero romano ({}).", info->nombre,
		              aRomano(*valencia));
	}

	return armar(nombreStock("oxido", *info, *valencia), CategoriaCompuesto::OXIDO, pasos);
}

ResultadoNomenclatura nombrarPeroxido(const Formula &formula)
{
	if (formula.cantidad() != 2)
	{
		return ErrorNomenclatura::NO_ES_PEROXIDO;
	}

	const ComponenteFormula *oxigeno = formula.componente(OXIGENO);
	const ComponenteFormula *metal = formula.unicoDistintoDe({OXIGENO});
	if (oxigeno == nullptr || metal == nullptr)
	{
		return ErrorNomenclatura::NO_ES_PEROXIDO;
	}

	Pasos pasos;
	pasos.agregar("Se identifica el metal ({}, subindice {}) y el oxigeno (subindice {}).", metal->simbolo,
	              metal->subindice.valor(), oxigeno->subindice.valor());

	const InfoElemento *info = buscarElemento(metal->simbolo);
	if (info == nullptr)
	{
		return ErrorNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	pasos.agregar("En un peroxido actua el grupo peroxo (O2)2-, donde cada oxigeno aporta -1 y no -2 como en un oxido.");

	// Como el grupo (O2) no se reduce junto con el subindice del metal, aqui
	// no se aplica la regla de intercambio sino el balance directo de cargas.
	std::optional<Valencia> deducida;
	for (const Valencia valencia : info->valenciasConocidas())
	{
		if (oxigeno->subindice.valor() == cargaTotal(metal->subindice, valencia))
		{
			deducida = valencia;
			break;
		}
	}

	if (!deducida)
	{
		return ErrorNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	pasos.agregar("Con valencia {}, {} atomo(s) de metal equilibran {} oxigeno(s): {} x {} = {}.", deducida->valor(),
	              metal->subindice.valor(), oxigeno->subindice.valor(), metal->subindice.valor(), deducida->valor(),
	              oxigeno->subindice.valor());

	return armar(std::format("peroxido de {}", info->nombre), CategoriaCompuesto::PEROXIDO, pasos);
}

ResultadoNomenclatura nombrarAnhidrido(const Formula &formula)
{
	if (formula.cantidad() != 2)
	{
		return ErrorNomenclatura::NO_ES_ANHIDRIDO;
	}

	const ComponenteFormula *oxigeno = formula.componente(OXIGENO);
	const ComponenteFormula *noMetal = formula.unicoDistintoDe({OXIGENO});
	if (oxigeno == nullptr || noMetal == nullptr)
	{
		return ErrorNomenclatura::NO_ES_ANHIDRIDO;
	}

	Pasos pasos;
	pasos.agregar("Se identifica el no metal ({}, subindice {}) y el oxigeno (subindice {}).", noMetal->simbolo,
	              noMetal->subindice.valor(), oxigeno->subindice.valor());

	const InfoNoMetal *info = buscarNoMetal(noMetal->simbolo);
	if (info == nullptr)
	{
		return ErrorNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	pasos.agregar("{} ({}) tiene {} valencia(s) conocida(s) para anhidridos.", info->nombre, noMetal->simbolo,
	              info->cantidadValencias);

	const auto valencia = valenciaQueCuadraPorIntercambio(info->valenciasConocidas(), noMetal->subindice,
	                                                       oxigeno->subindice);
	if (!valencia)
	{
		return ErrorNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	const int posicion = info->posicionDe(*valencia);
	pasos.agregar("La proporcion {}:{} corresponde a la valencia {}, la {} de {} en orden ascendente.",
	              noMetal->subindice.valor(), oxigeno->subindice.valor(), valencia->valor(), posicion + 1,
	              info->cantidadValencias);

	const Afijos afijos = afijosTradicionales(posicion, info->cantidadValencias);
	pasos.agregar("Por esa posicion le corresponden el prefijo \"{}\" y el sufijo \"{}\".",
	              afijos.prefijo.empty() ? std::string_view{"(ninguno)"} : afijos.prefijo, afijos.sufijo);

	return armar(std::format("anhidrido {}{}{}", afijos.prefijo, info->raiz, afijos.sufijo),
	             CategoriaCompuesto::ANHIDRIDO, pasos);
}

ResultadoNomenclatura nombrarHidracido(const Formula &formula)
{
	if (formula.cantidad() != 2)
	{
		return ErrorNomenclatura::NO_ES_HIDRACIDO;
	}

	const ComponenteFormula *hidrogeno = formula.componente(HIDROGENO);
	const ComponenteFormula *noMetal = formula.unicoDistintoDe({HIDROGENO});
	if (hidrogeno == nullptr || noMetal == nullptr)
	{
		return ErrorNomenclatura::NO_ES_HIDRACIDO;
	}

	Pasos pasos;
	pasos.agregar("Se identifica el hidrogeno (subindice {}) y el no metal ({}, subindice {}).",
	              hidrogeno->subindice.valor(), noMetal->simbolo, noMetal->subindice.valor());

	const InfoHidracido *info = buscarHidracido(noMetal->simbolo);
	if (info == nullptr)
	{
		return ErrorNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	pasos.agregar("{} forma hidracido con {} hidrogeno(s): su valencia negativa es fija, no hay varias opciones.",
	              noMetal->simbolo, info->hidrogenos.valor());

	if (hidrogeno->subindice != info->hidrogenos || noMetal->subindice != Subindice{1})
	{
		return ErrorNomenclatura::NO_ES_HIDRACIDO;
	}

	pasos.agregar("El nombre se arma como \"acido\" + raiz \"{}\" + sufijo fijo \"hidrico\".", info->raiz);

	return armar(std::format("acido {}hidrico", info->raiz), CategoriaCompuesto::ACIDO_HIDRACIDO, pasos);
}

ResultadoNomenclatura nombrarOxacido(const Formula &formula)
{
	if (formula.cantidad() != 3)
	{
		return ErrorNomenclatura::NO_ES_OXACIDO;
	}

	const ComponenteFormula *hidrogeno = formula.componente(HIDROGENO);
	const ComponenteFormula *oxigeno = formula.componente(OXIGENO);
	const ComponenteFormula *noMetal = formula.unicoDistintoDe({HIDROGENO, OXIGENO});
	if (hidrogeno == nullptr || oxigeno == nullptr || noMetal == nullptr)
	{
		return ErrorNomenclatura::NO_ES_OXACIDO;
	}

	Pasos pasos;
	pasos.agregar("Se identifican H({}), {}({}) y O({}) en la formula.", hidrogeno->subindice.valor(),
	              noMetal->simbolo, noMetal->subindice.valor(), oxigeno->subindice.valor());

	if (buscarNoMetal(noMetal->simbolo) == nullptr)
	{
		return ErrorNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	const InfoOxacido *info = buscarOxacido(noMetal->simbolo, hidrogeno->subindice, noMetal->subindice,
	                                         oxigeno->subindice);
	if (info == nullptr)
	{
		return ErrorNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	pasos.agregar("Esa formula coincide con un oxacido conocido de {}, tabulado y no derivado por regla general.",
	              noMetal->simbolo);

	return armar(std::string{info->nombre}, CategoriaCompuesto::ACIDO_OXACIDO, pasos);
}

ResultadoNomenclatura nombrarBase(const Formula &formula)
{
	const auto normalizada = normalizarBase(formula);
	if (!normalizada)
	{
		return ErrorNomenclatura::NO_ES_BASE;
	}

	Pasos pasos;
	pasos.agregar("Se identifica el metal ({}) acompanado de {} grupo(s) hidroxilo (OH).",
	              normalizada->metal->simbolo, normalizada->cantidadGrupos.valor());

	const InfoElemento *info = buscarElemento(normalizada->metal->simbolo);
	if (info == nullptr)
	{
		return ErrorNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	// El hidroxilo actua con valencia 1, asi que la cantidad de grupos es
	// directamente la valencia del metal, sin reduccion posible.
	const Valencia valencia{normalizada->cantidadGrupos.valor()};
	if (!info->admite(valencia))
	{
		return ErrorNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	pasos.agregar("El grupo OH actua con valencia 1, asi que {} grupo(s) implican valencia {} para {}.",
	              normalizada->cantidadGrupos.valor(), valencia.valor(), info->nombre);

	if (!info->tieneUnicaValencia())
	{
		pasos.agregar("{} admite varias valencias, asi que se indica con numero romano ({}).", info->nombre,
		              aRomano(valencia));
	}

	return armar(nombreStock("hidroxido", *info, valencia), CategoriaCompuesto::BASE, pasos);
}

ResultadoNomenclatura nombrarSal(const Formula &formula)
{
	const auto normalizada = normalizarSal(formula);
	if (!normalizada)
	{
		return ErrorNomenclatura::NO_ES_SAL;
	}

	Pasos pasos;
	pasos.agregar("Se identifica el metal ({}) y {} grupo(s) del radical {}.", normalizada->metal->simbolo,
	              normalizada->cantidadGrupos.valor(), normalizada->grupo);

	const InfoElemento *infoMetal = buscarElemento(normalizada->metal->simbolo);
	if (infoMetal == nullptr)
	{
		return ErrorNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	const InfoRadical *infoRadical = buscarRadical(normalizada->grupo);
	if (infoRadical == nullptr)
	{
		return ErrorNomenclatura::ELEMENTO_DESCONOCIDO;
	}

	pasos.agregar("El radical {} es el {}, con carga -{}.", normalizada->grupo, infoRadical->nombre,
	              infoRadical->carga.valor());

	// La formula equilibra cuando la carga positiva total del metal iguala a
	// la carga negativa total que aportan los radicales.
	const int cargaNegativa = cargaTotal(normalizada->cantidadGrupos, infoRadical->carga);

	std::optional<Valencia> deducida;
	for (const Valencia valencia : infoMetal->valenciasConocidas())
	{
		if (cargaTotal(normalizada->metal->subindice, valencia) == cargaNegativa)
		{
			deducida = valencia;
			break;
		}
	}

	if (!deducida)
	{
		return ErrorNomenclatura::VALENCIA_NO_DETERMINADA;
	}

	pasos.agregar("Para equilibrar {} carga(s) negativa(s) con {} atomo(s) de metal, {} debe actuar con valencia {}.",
	              cargaNegativa, normalizada->metal->subindice.valor(), infoMetal->nombre, deducida->valor());

	if (!infoMetal->tieneUnicaValencia())
	{
		pasos.agregar("{} admite varias valencias, asi que se indica con numero romano ({}).", infoMetal->nombre,
		              aRomano(*deducida));
	}

	return armar(nombreStock(infoRadical->nombre, *infoMetal, *deducida), CategoriaCompuesto::SAL_OXISAL, pasos);
}

ResultadoNomenclatura nombrar(const Formula &formula)
{
	// El orden no es arbitrario. Oxido va antes que peroxido porque una misma
	// formula puede encajar en ambos: CuO es "oxido de cobre (II)" y tambien,
	// formalmente, un peroxido de cobre con valencia 1. En ausencia de mas
	// contexto se prefiere la lectura de oxido, que es la estandar.
	using Candidato = ResultadoNomenclatura (*)(const Formula &);
	constexpr std::array<Candidato, 7> candidatos = {
		&nombrarOxido, &nombrarPeroxido, &nombrarAnhidrido, &nombrarHidracido,
		&nombrarOxacido, &nombrarBase, &nombrarSal,
	};

	ErrorNomenclatura ultimoError = ErrorNomenclatura::NINGUNA_CATEGORIA;

	for (const Candidato candidato : candidatos)
	{
		ResultadoNomenclatura resultado = candidato(formula);
		if (resultado)
		{
			return resultado;
		}

		// Que una formula "no sea un oxido" no dice nada util cuando todavia
		// quedan categorias por probar. En cambio un elemento desconocido o
		// una valencia que no cuadra si son informativos, asi que se guardan
		// para reportarlos si al final no encaja en ninguna categoria.
		const ErrorNomenclatura error = resultado.error();
		const bool esRechazoDeForma =
		    error == ErrorNomenclatura::NO_ES_OXIDO || error == ErrorNomenclatura::NO_ES_PEROXIDO ||
		    error == ErrorNomenclatura::NO_ES_ANHIDRIDO || error == ErrorNomenclatura::NO_ES_HIDRACIDO ||
		    error == ErrorNomenclatura::NO_ES_OXACIDO || error == ErrorNomenclatura::NO_ES_BASE ||
		    error == ErrorNomenclatura::NO_ES_SAL;

		if (!esRechazoDeForma)
		{
			ultimoError = error;
		}
	}

	return ultimoError;
}

} // namespace cheminator
