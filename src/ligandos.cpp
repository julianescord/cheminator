#include "cheminator/ligandos.hpp"

#include "validacion_tablas.hpp"

#include <array>

namespace cheminator {
namespace {

constexpr InfoLigando ligando(std::string_view formula, std::string_view nombre, int carga,
                               bool prefijoAlterno = false)
{
	return InfoLigando{formula, nombre, Carga{carga}, prefijoAlterno};
}

// Ligandos de uso comun en un curso de quimica inorganica.
//
// La IUPAC fija estos nombres uno por uno y no se derivan de la formula: el
// cianuro como ligando es "ciano", el agua es "acua" y el amoniaco "amin".
// Por eso la tabla es explicita, igual que la de oxacidos.
constexpr std::array TABLA_LIGANDOS = {
	// Anionicos: el nombre del anion terminado en -o.
	ligando("CN", "ciano", -1),
	ligando("OH", "hidroxo", -1),
	ligando("Cl", "cloro", -1),
	ligando("Br", "bromo", -1),
	ligando("I", "iodo", -1),
	ligando("F", "fluoro", -1),
	ligando("NO2", "nitro", -1),
	ligando("SCN", "tiociano", -1),
	ligando("O", "oxo", -2),
	ligando("S", "tio", -2),
	ligando("C2O4", "oxalato", -2, /*prefijoAlterno=*/true),
	ligando("SO4", "sulfato", -2, /*prefijoAlterno=*/true),
	ligando("S2O3", "tiosulfato", -2, /*prefijoAlterno=*/true),

	// Neutros: nombres especiales que no siguen ninguna regla general.
	ligando("H2O", "acua", 0),
	ligando("NH3", "amin", 0),
	ligando("CO", "carbonilo", 0),
	ligando("NO", "nitrosilo", 0),
	ligando("en", "etilendiamino", 0, /*prefijoAlterno=*/true),
};

static_assert(detalle::clavesUnicas(TABLA_LIGANDOS, [](const InfoLigando &l) { return l.formula; }),
              "Hay una formula repetida en la tabla de ligandos: la segunda entrada seria inalcanzable.");

static_assert(detalle::clavesUnicas(TABLA_LIGANDOS, [](const InfoLigando &l) { return l.nombre; }),
              "Hay un nombre repetido en la tabla de ligandos: la formulacion inversa seria ambigua.");

} // namespace

const InfoLigando *buscarLigando(std::string_view formula) noexcept
{
	for (const InfoLigando &ligando : TABLA_LIGANDOS)
	{
		if (ligando.formula == formula)
		{
			return &ligando;
		}
	}
	return nullptr;
}

std::span<const InfoLigando> todosLosLigandos() noexcept
{
	return TABLA_LIGANDOS;
}

} // namespace cheminator
