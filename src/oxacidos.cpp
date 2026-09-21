#include "cheminator/oxacidos.hpp"

#include "validacion_tablas.hpp"

#include <array>
#include <tuple>

namespace cheminator {
namespace {

constexpr InfoOxacido oxacido(std::string_view simbolo, int hidrogenos, int atomos, int oxigenos,
                               std::string_view nombre)
{
	return InfoOxacido{simbolo, Subindice{hidrogenos}, Subindice{atomos}, Subindice{oxigenos}, nombre};
}

// Formulas y nombres verificados manualmente contra la nomenclatura
// tradicional estandar. El acido de nitrogeno con valencia 1 (HNO) queda
// fuera por inestable e inusual en este nivel.
constexpr std::array TABLA_OXACIDOS = {
	oxacido("C", 2, 1, 3, "acido carbonico"),

	oxacido("N", 1, 1, 2, "acido nitroso"),
	oxacido("N", 1, 1, 3, "acido nitrico"),

	oxacido("P", 3, 1, 3, "acido fosforoso"),
	oxacido("P", 3, 1, 4, "acido fosforico"),

	oxacido("S", 2, 1, 2, "acido hiposulfuroso"),
	oxacido("S", 2, 1, 3, "acido sulfuroso"),
	oxacido("S", 2, 1, 4, "acido sulfurico"),

	oxacido("Cl", 1, 1, 1, "acido hipocloroso"),
	oxacido("Cl", 1, 1, 2, "acido cloroso"),
	oxacido("Cl", 1, 1, 3, "acido clorico"),
	oxacido("Cl", 1, 1, 4, "acido perclorico"),

	oxacido("Br", 1, 1, 1, "acido hipobromoso"),
	oxacido("Br", 1, 1, 2, "acido bromoso"),
	oxacido("Br", 1, 1, 3, "acido bromico"),
	oxacido("Br", 1, 1, 4, "acido perbromico"),

	oxacido("I", 1, 1, 1, "acido hipoiodoso"),
	oxacido("I", 1, 1, 2, "acido iodoso"),
	oxacido("I", 1, 1, 3, "acido iodico"),
	oxacido("I", 1, 1, 4, "acido periodico"),
};

// La clave util aqui es la formula completa, no el simbolo: un mismo no metal
// forma varios oxacidos. Dos entradas con identicos subindices harian que la
// segunda quedara inalcanzable.
static_assert(detalle::clavesUnicas(TABLA_OXACIDOS,
                                     [](const InfoOxacido &o) {
                                         return std::tuple{o.simboloNoMetal, o.hidrogenos, o.atomosNoMetal,
                                                           o.oxigenos};
                                     }),
              "Hay dos oxacidos con la misma formula en la tabla.");

static_assert(detalle::clavesUnicas(TABLA_OXACIDOS, [](const InfoOxacido &o) { return o.nombre; }),
              "Hay dos oxacidos con el mismo nombre en la tabla.");

} // namespace

const InfoOxacido *buscarOxacido(std::string_view simboloNoMetal, Subindice hidrogenos, Subindice atomosNoMetal,
                                  Subindice oxigenos) noexcept
{
	for (const InfoOxacido &oxacido : TABLA_OXACIDOS)
	{
		if (oxacido.simboloNoMetal == simboloNoMetal && oxacido.hidrogenos == hidrogenos &&
		    oxacido.atomosNoMetal == atomosNoMetal && oxacido.oxigenos == oxigenos)
		{
			return &oxacido;
		}
	}
	return nullptr;
}

} // namespace cheminator
