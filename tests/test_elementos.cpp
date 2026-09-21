#include "cheminator/elementos.hpp"
#include "cheminator/hidracidos.hpp"
#include "cheminator/no_metales.hpp"
#include "cheminator/oxacidos.hpp"
#include "cheminator/radicales.hpp"
#include "test_runner.h"

using namespace cheminator;

void test_elementos()
{
	const InfoElemento *hierro = buscarElemento("Fe");
	ASSERT_TRUE(hierro != nullptr);
	if (hierro != nullptr)
	{
		ASSERT_EQ_STR(hierro->nombre, "Hierro");
		ASSERT_EQ_INT(hierro->cantidadValencias, 2);
		ASSERT_TRUE(!hierro->tieneUnicaValencia());
		ASSERT_TRUE(hierro->admite(Valencia{2}));
		ASSERT_TRUE(hierro->admite(Valencia{3}));
		ASSERT_TRUE(!hierro->admite(Valencia{5}));
		ASSERT_EQ_INT(static_cast<int>(hierro->valenciasConocidas().size()), 2);
	}

	const InfoElemento *sodio = buscarElemento("Na");
	ASSERT_TRUE(sodio != nullptr);
	if (sodio != nullptr)
	{
		ASSERT_TRUE(sodio->tieneUnicaValencia());
	}

	ASSERT_TRUE(buscarElemento("Xx") == nullptr);

	// La tabla es sensible a mayusculas, como la notacion quimica real.
	ASSERT_TRUE(buscarElemento("fe") == nullptr);

	const InfoNoMetal *azufre = buscarNoMetal("S");
	ASSERT_TRUE(azufre != nullptr);
	if (azufre != nullptr)
	{
		ASSERT_EQ_STR(azufre->raiz, "sulfur");
		ASSERT_EQ_INT(azufre->cantidadValencias, 3);
		// El orden de la lista define el prefijo/sufijo tradicional.
		ASSERT_EQ_INT(azufre->posicionDe(Valencia{2}), 0);
		ASSERT_EQ_INT(azufre->posicionDe(Valencia{4}), 1);
		ASSERT_EQ_INT(azufre->posicionDe(Valencia{6}), 2);
		ASSERT_EQ_INT(azufre->posicionDe(Valencia{5}), -1);
	}

	ASSERT_TRUE(buscarNoMetal("Fe") == nullptr);

	const InfoHidracido *cloro = buscarHidracido("Cl");
	ASSERT_TRUE(cloro != nullptr);
	if (cloro != nullptr)
	{
		ASSERT_EQ_STR(cloro->raiz, "clor");
		ASSERT_TRUE(cloro->hidrogenos == Subindice{1});
	}
	ASSERT_TRUE(buscarHidracido("N") == nullptr);

	const InfoOxacido *sulfurico = buscarOxacido("S", Subindice{2}, Subindice{1}, Subindice{4});
	ASSERT_TRUE(sulfurico != nullptr);
	if (sulfurico != nullptr)
	{
		ASSERT_EQ_STR(sulfurico->nombre, "acido sulfurico");
	}
	ASSERT_TRUE(buscarOxacido("S", Subindice{2}, Subindice{1}, Subindice{9}) == nullptr);

	const InfoRadical *sulfato = buscarRadical("SO4");
	ASSERT_TRUE(sulfato != nullptr);
	if (sulfato != nullptr)
	{
		ASSERT_EQ_STR(sulfato->nombre, "sulfato");
		ASSERT_TRUE(sulfato->carga == Carga{2});
	}
	ASSERT_TRUE(buscarRadical("SO9") == nullptr);
}
