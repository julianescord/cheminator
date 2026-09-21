#include "cheminator/coordinacion.hpp"

#include "cheminator/elementos.hpp"
#include "cheminator/hidracidos.hpp"
#include "cheminator/radicales.hpp"

#include <algorithm>
#include <cctype>
#include <format>
#include <string>
#include <utility>

namespace cheminator {
namespace {

// Dentro de la esfera, los ligandos pueden venir agrupados entre parentesis
// ("(CN)6") o sueltos ("Cl4"). En ambos casos el nodo guarda la formula del
// ligando en `simbolo` y cuantos hay en `subindice`, asi que basta buscar esa
// formula en la tabla.
Resultado<LigandoContado, ErrorCoordinacion> comoLigando(const NodoFormula &nodo)
{
	if (const InfoLigando *info = buscarLigando(nodo.simbolo))
	{
		return LigandoContado{info, nodo.subindice.valor()};
	}
	return ErrorCoordinacion::LIGANDO_DESCONOCIDO;
}

} // namespace

std::string_view mensajeError(ErrorCoordinacion error) noexcept
{
	switch (error)
	{
		case ErrorCoordinacion::NO_ES_COMPLEJO:
			return "La formula no tiene una esfera de coordinacion entre corchetes.";
		case ErrorCoordinacion::SIN_ATOMO_CENTRAL:
			return "Dentro de los corchetes no se reconoce un atomo central que este en las tablas.";
		case ErrorCoordinacion::LIGANDO_DESCONOCIDO:
			return "Alguno de los ligandos no esta en la tabla de la libreria.";
		case ErrorCoordinacion::CONTRAION_DESCONOCIDO:
			return "El ion que acompana al complejo no esta en las tablas.";
		case ErrorCoordinacion::CARGA_INCONSISTENTE:
			return "La carga del complejo no cuadra con la de sus ligandos.";
		case ErrorCoordinacion::ESTADO_INVALIDO:
			return "El estado de oxidacion que resulta no es uno de los que admite ese metal.";
	}
	return "Error desconocido.";
}

Resultado<Complejo, ErrorCoordinacion> analizarComplejo(const Formula &formula)
{
	const NodoFormula *esfera = formula.esferaDeCoordinacion();
	if (esfera == nullptr)
	{
		return ErrorCoordinacion::NO_ES_COMPLEJO;
	}
	if (esfera->hijos.empty())
	{
		return ErrorCoordinacion::SIN_ATOMO_CENTRAL;
	}

	Complejo complejo;

	// El atomo central es el primero dentro del corchete, por convencion de
	// escritura.
	const NodoFormula &central = esfera->hijos.front();
	const InfoElemento *infoCentral = buscarElemento(central.simbolo);
	if (infoCentral == nullptr)
	{
		return ErrorCoordinacion::SIN_ATOMO_CENTRAL;
	}
	complejo.simboloCentral = infoCentral->simbolo;

	// El resto de la esfera son ligandos.
	int cargaLigandos = 0;
	for (std::size_t i = 1; i < esfera->hijos.size(); ++i)
	{
		const auto resultado = comoLigando(esfera->hijos[i]);
		if (!resultado)
		{
			return resultado.error();
		}

		const LigandoContado &contado = resultado.valor();
		complejo.ligandos.push_back(contado);
		complejo.indiceCoordinacion += contado.cantidad;
		cargaLigandos += contado.info->carga.valor() * contado.cantidad;
	}

	if (complejo.ligandos.empty())
	{
		return ErrorCoordinacion::LIGANDO_DESCONOCIDO;
	}

	// Lo que hay fuera de los corchetes, si lo hay. Puede ir delante (un
	// cation, "K3[...]") o detras (un anion, "[...]Cl3").
	// Se recorre por indice para saber si el contraion va antes o despues de
	// la esfera, que es lo que decide si el complejo es el anion o el cation.
	const std::span<const NodoFormula> raices = formula.raices();
	std::size_t posicionEsfera = 0;
	for (std::size_t i = 0; i < raices.size(); ++i)
	{
		if (&raices[i] == esfera)
		{
			posicionEsfera = i;
			break;
		}
	}

	for (std::size_t i = 0; i < raices.size(); ++i)
	{
		if (i == posicionEsfera)
		{
			continue;
		}
		complejo.contraion = raices[i].simbolo;
		complejo.cantidadContraion = raices[i].subindice;
		complejo.contraionDelante = (i < posicionEsfera);
	}

	// La carga de la esfera: la escrita como sufijo si el complejo va suelto,
	// o la que compensa al contraion si va acompanado.
	if (complejo.contraion.empty())
	{
		complejo.cargaEsfera = formula.carga();
	}
	else
	{
		const int cantidad = complejo.cantidadContraion.valor();

		// Un contraion tiene su propia carga: si es un metal, la valencia con
		// que actua; si es un radical, la que tenga tabulada.
		int cargaContraion = 0;
		if (const InfoElemento *metal = buscarElemento(complejo.contraion))
		{
			// Se asume la valencia menor, que es lo que se hace al escribir
			// una sal sin indicar nada.
			cargaContraion = metal->valenciasConocidas().front().valor();
		}
		else if (const InfoRadical *radical = buscarRadical(complejo.contraion))
		{
			cargaContraion = -radical->carga.valor();
		}
		else if (const InfoHidracido *hidracido = buscarHidracido(complejo.contraion))
		{
			// Un anion monoatomico como el cloruro, cuya carga negativa es el
			// numero de hidrogenos que lleva su hidracido (HCl -> Cl(1-)).
			cargaContraion = -hidracido->hidrogenos.valor();
		}
		else
		{
			return ErrorCoordinacion::CONTRAION_DESCONOCIDO;
		}

		// El conjunto es neutro, asi que la esfera lleva la carga opuesta a la
		// suma de los contraiones.
		complejo.cargaEsfera = Carga{-cargaContraion * cantidad};
	}

	// Estado de oxidacion del metal: la carga de la esfera menos la que
	// aportan los ligandos. Es el balance que hace falta el arbol para poder
	// calcular, porque exige saber que esta dentro del corchete.
	const int estado = complejo.cargaEsfera.valor() - cargaLigandos;
	if (estado < 0)
	{
		return ErrorCoordinacion::CARGA_INCONSISTENTE;
	}
	complejo.estadoOxidacion = Valencia{estado};

	// El estado cero es valido y no se comprueba contra la tabla: los
	// carbonilos como [Ni(CO)4] tienen el metal sin oxidar, y ninguna tabla de
	// valencias incluye el cero porque no interviene en compuestos ionicos.
	if (estado > 0 && !infoCentral->admite(complejo.estadoOxidacion))
	{
		return ErrorCoordinacion::ESTADO_INVALIDO;
	}

	return complejo;
}

namespace {

// Nombres latinos de los metales que cambian al formar un anion complejo. Los
// que no aparecen usan su nombre en espanol sin mas ("cobaltato", "cromato").
constexpr std::pair<std::string_view, std::string_view> RAICES_ANIONICAS[] = {
	{"Fe", "ferr"},   {"Cu", "cupr"},   {"Ag", "argent"}, {"Au", "aur"},
	{"Pb", "plumb"},  {"Sn", "estann"}, {"Mn", "mangan"}, {"Ni", "niquel"},
	{"Zn", "zinc"},   {"Co", "cobalt"}, {"Cr", "crom"},   {"Pt", "platin"},
	{"Al", "alumin"}, {"Ti", "titan"},  {"Hg", "mercur"},
};

// Prefijos multiplicadores. La segunda lista es para los ligandos cuyo nombre
// ya contiene un numero, donde di- y tri- se leerian como parte del nombre.
constexpr std::string_view PREFIJOS[] = {"", "", "di", "tri", "tetra", "penta", "hexa"};
constexpr std::string_view PREFIJOS_ALTERNOS[] = {"", "", "bis", "tris", "tetrakis", "pentakis", "hexakis"};

std::string prefijoPara(int cantidad, bool alterno)
{
	const auto &tabla = alterno ? PREFIJOS_ALTERNOS : PREFIJOS;
	if (cantidad >= 1 && cantidad < static_cast<int>(std::size(PREFIJOS)))
	{
		return std::string{tabla[static_cast<std::size_t>(cantidad)]};
	}
	return std::to_string(cantidad) + "-";
}

// El nombre del contraion tal como aparece en el nombre del compuesto.
std::string nombreContraion(std::string_view simbolo)
{
	if (const InfoElemento *metal = buscarElemento(simbolo))
	{
		std::string nombre{metal->nombre};
		// Los nombres de los metales estan capitalizados en la tabla, pero
		// dentro de un nombre compuesto van en minuscula.
		if (!nombre.empty())
		{
			nombre[0] = static_cast<char>(std::tolower(static_cast<unsigned char>(nombre[0])));
		}
		return nombre;
	}
	if (const InfoHidracido *hidracido = buscarHidracido(simbolo))
	{
		return std::string{hidracido->raiz} + "uro";
	}
	if (const InfoRadical *radical = buscarRadical(simbolo))
	{
		return std::string{radical->nombre};
	}
	return std::string{simbolo};
}

} // namespace

std::string_view nombreAnionico(std::string_view simbolo) noexcept
{
	for (const auto &[clave, raiz] : RAICES_ANIONICAS)
	{
		if (clave == simbolo)
		{
			return raiz;
		}
	}
	return {};
}

Resultado<NomenclaturaComplejo, ErrorCoordinacion> nombrarComplejo(const Formula &formula)
{
	const auto analisis = analizarComplejo(formula);
	if (!analisis)
	{
		return analisis.error();
	}

	const Complejo &complejo = analisis.valor();
	const InfoElemento *central = buscarElemento(complejo.simboloCentral);
	if (central == nullptr)
	{
		return ErrorCoordinacion::SIN_ATOMO_CENTRAL;
	}

	NomenclaturaComplejo salida;
	auto paso = [&salida](std::string texto) { salida.pasos.push_back(std::move(texto)); };

	paso(std::format("El atomo central es {} ({}), dentro de los corchetes.", central->nombre,
	                 central->simbolo));

	// Los ligandos se nombran en orden alfabetico, sin contar el prefijo
	// multiplicador. Se ordena una copia para no alterar el analisis.
	std::vector<LigandoContado> ordenados = complejo.ligandos;
	std::sort(ordenados.begin(), ordenados.end(), [](const LigandoContado &a, const LigandoContado &b) {
		return a.info->nombre < b.info->nombre;
	});

	std::string parteLigandos;
	for (const LigandoContado &contado : ordenados)
	{
		parteLigandos += prefijoPara(contado.cantidad, contado.info->usaPrefijoAlterno);
		parteLigandos += contado.info->nombre;
	}

	{
		std::string detalle;
		for (const LigandoContado &contado : ordenados)
		{
			if (!detalle.empty()) detalle += ", ";
			detalle += std::format("{} x{}", contado.info->nombre, contado.cantidad);
		}
		paso(std::format("Ligandos en orden alfabetico: {}.", detalle));
		paso(std::format("Indice de coordinacion: {}.", complejo.indiceCoordinacion));
	}

	// El estado de oxidacion sale del balance de cargas, que es lo que obliga
	// a conocer la estructura y no solo la lista de atomos.
	paso(std::format("La esfera tiene carga {:+}, y los ligandos suman {:+}.", complejo.cargaEsfera.valor(),
	                 complejo.cargaEsfera.valor() - complejo.estadoOxidacion.valor()));
	paso(std::format("Por diferencia, {} actua con estado de oxidacion {}.", central->nombre,
	                 complejo.estadoOxidacion.valor()));

	// Un complejo anionico lleva el metal con raiz latina y sufijo -ato.
	const bool esAnion = complejo.cargaEsfera.valor() < 0;

	std::string nombreCentral;
	if (esAnion)
	{
		const std::string_view raiz = nombreAnionico(central->simbolo);
		nombreCentral = raiz.empty() ? std::string{central->nombre} : std::string{raiz};
		if (!nombreCentral.empty())
		{
			nombreCentral[0] = static_cast<char>(std::tolower(static_cast<unsigned char>(nombreCentral[0])));
		}
		nombreCentral += "ato";
		paso(std::format("La esfera es un anion, asi que el metal toma el sufijo -ato: \"{}\".", nombreCentral));
	}
	else
	{
		nombreCentral = central->nombre;
		if (!nombreCentral.empty())
		{
			nombreCentral[0] = static_cast<char>(std::tolower(static_cast<unsigned char>(nombreCentral[0])));
		}
	}

	// El estado cero se escribe "0" y no en numeros romanos, que no tienen
	// cero: "tetracarboniloniquel (0)".
	const std::string estadoEscrito = complejo.estadoOxidacion == Valencia{0}
	                                       ? std::string{"0"}
	                                       : std::string{aRomano(complejo.estadoOxidacion)};

	salida.nombre = parteLigandos + nombreCentral + " (" + estadoEscrito + ")";

	// El contraion se nombra despues, unido con "de".
	if (!complejo.contraion.empty())
	{
		const std::string nombreIon = nombreContraion(complejo.contraion);
		if (complejo.contraionDelante)
		{
			// El contraion es el cation: "...ferrato (III) de potasio".
			salida.nombre += " de " + nombreIon;
		}
		else
		{
			// El complejo es el cation: "cloruro de hexaamincobalto (III)".
			salida.nombre = nombreIon + " de " + salida.nombre;
		}
		paso(std::format("Fuera de la esfera esta {}, que completa el nombre.", nombreIon));
	}

	return salida;
}

} // namespace cheminator
