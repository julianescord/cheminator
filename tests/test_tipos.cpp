#include "cheminator/tipos.hpp"
#include "test_runner.h"

using namespace cheminator;

namespace {
// Solo para ejercitar Resultado con un tipo de error cualquiera.
enum class ErrorTipoPrueba { ALGO };
} // namespace

// La regla de intercambio de valencias merece pruebas propias: es donde se
// cometieron los errores de calculo del proyecto, y ahora que esta en una
// sola funcion con nombre se puede verificar de forma aislada.
void test_tipos()
{
	// Valencias iguales se cancelan y dan subindices 1:1 (CaO, del calcio de
	// valencia 2 con el oxigeno de valencia 2).
	ASSERT_TRUE(intercambiarValencias(Valencia{2}, Valencia{2}) == (ParSubindices{Subindice{1}, Subindice{1}}));

	// Sodio (1) con oxigeno (2) -> Na2O.
	ASSERT_TRUE(intercambiarValencias(Valencia{1}, Valencia{2}) == (ParSubindices{Subindice{2}, Subindice{1}}));

	// Hierro (3) con oxigeno (2) -> Fe2O3.
	ASSERT_TRUE(intercambiarValencias(Valencia{3}, Valencia{2}) == (ParSubindices{Subindice{2}, Subindice{3}}));

	// Aluminio (3) con oxigeno (2) -> Al2O3.
	ASSERT_TRUE(intercambiarValencias(Valencia{3}, Valencia{2}) == (ParSubindices{Subindice{2}, Subindice{3}}));

	// Azufre (6) con oxigeno (2) -> SO3, ya reducido por el mcd.
	ASSERT_TRUE(intercambiarValencias(Valencia{6}, Valencia{2}) == (ParSubindices{Subindice{1}, Subindice{3}}));

	// Cloro (7) con oxigeno (2) -> Cl2O7.
	ASSERT_TRUE(intercambiarValencias(Valencia{7}, Valencia{2}) == (ParSubindices{Subindice{2}, Subindice{7}}));

	// El balance de cargas es simetrico: 2 atomos de valencia 3 aportan lo
	// mismo que 3 grupos de carga 2 (Al2(SO4)3).
	ASSERT_EQ_INT(cargaTotal(Subindice{2}, Valencia{3}), cargaTotal(Subindice{3}, Carga{2}));

	ASSERT_EQ_INT(maximoComunDivisor(2, 3), 1);
	ASSERT_EQ_INT(maximoComunDivisor(4, 2), 2);
	ASSERT_EQ_INT(maximoComunDivisor(6, 2), 2);

	ASSERT_EQ_STR(aRomano(Valencia{1}), "I");
	ASSERT_EQ_STR(aRomano(Valencia{4}), "IV");
	ASSERT_EQ_STR(aRomano(Valencia{7}), "VII");

	// Resultado: lleva un valor o un error, nunca ambos.
	const Resultado<int, ErrorTipoPrueba> bueno{42};
	ASSERT_TRUE(bueno.ok());
	ASSERT_EQ_INT(bueno.valor(), 42);

	const Resultado<int, ErrorTipoPrueba> malo{ErrorTipoPrueba::ALGO};
	ASSERT_TRUE(!malo.ok());
	ASSERT_TRUE(malo.error() == ErrorTipoPrueba::ALGO);
}
