#include "menu.h"
#include "elementos.h"
#include "formula.h"
#include "nomenclatura.h"
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <limits>

void dibujarMenu()
{
	std::cout << "Bienvenidos a Cheminator";
	std::cout << "\nIntroduzca la formula del compuesto del que desea conocer en base a las siguientes opciones.";

	std::printf("\n\nOpciones:\n");
	std::printf("1)  Ingresar formula de oxido\n");
	std::printf("2)  Ingresar formula de peroxido\n");
	std::printf("3)  Ingresar formula de anhidrido\n");
	std::printf("4)  Ingresar formula de acido hidracido\n");
	std::printf("5)  Ingresar formula de acido oxacido\n");
	std::printf("6)  Ingresar formula de base\n");
	std::printf("7)  Ingresar formula de sal oxisal\n");
	std::printf("0)  Salir\n");
}

// Lee una fórmula desde stdin de forma segura: acota la lectura al tamaño del
// buffer y descarta el resto de la línea si el usuario escribió de más.
static void leerFormula(char destino[TAM_MAX])
{
	std::cin >> std::setw(TAM_MAX) >> destino;

	if (std::cin.fail())
	{
		std::cin.clear();
		destino[0] = '\0';
	}
	// Descarta cualquier resto de la línea (p.ej. espacios extra u otro token)
	// para que la siguiente lectura del menú no herede entrada sobrante.
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Firma común de las funciones nomenclaturaStock*/nomenclaturaTradicional*.
using CalculadoraNomenclatura = ResultadoNomenclatura (*)(const FormulaParseada &, char[TAM_MAX]);

// Flujo compartido por las distintas opciones del menú: pide una fórmula,
// la parsea, calcula su nomenclatura con la función indicada y reporta el
// resultado o el error correspondiente.
static void pedirFormulaYNombrar(const char *mensajeEntrada, CalculadoraNomenclatura calcular)
{
	char formula[TAM_MAX];

	std::cout << "\n" << mensajeEntrada;
	leerFormula(formula);

	if (formula[0] == '\0')
	{
		std::cout << "\nNo se ingreso ninguna formula.\n";
		return;
	}

	std::cout << "La formula es: " << formula;

	FormulaParseada parseada;
	ResultadoParseo resultadoParseo = parsearFormula(formula, parseada);
	if (resultadoParseo != ResultadoParseo::OK)
	{
		std::cout << "\nError: " << mensajeError(resultadoParseo) << "\n";
		return;
	}

	char nomenclatura[TAM_MAX];
	ResultadoNomenclatura resultadoNomenclatura = calcular(parseada, nomenclatura);
	if (resultadoNomenclatura != ResultadoNomenclatura::OK)
	{
		std::cout << "\nError: " << mensajeError(resultadoNomenclatura) << "\n";
		return;
	}

	std::cout << "\nNomenclatura del compuesto: " << nomenclatura << "\n";
}

void oxido()
{
	pedirFormulaYNombrar(
		"Introduzca la formula del oxido del que desea conocer su nomenclatura stock (ej. Fe2O3):",
		nomenclaturaStockOxido);
}

void peroxido()
{
	pedirFormulaYNombrar(
		"Introduzca la formula del peroxido del que desea conocer su nomenclatura stock (ej. Na2O2):",
		nomenclaturaStockPeroxido);
}

void anhidrido()
{
	pedirFormulaYNombrar(
		"Introduzca la formula del anhidrido del que desea conocer su nomenclatura tradicional (ej. SO3):",
		nomenclaturaTradicionalAnhidrido);
}

void acidoHidracido()
{
	pedirFormulaYNombrar(
		"Introduzca la formula del acido hidracido del que desea conocer su nomenclatura (ej. HCl):",
		nomenclaturaTradicionalHidracido);
}

void acidoOxacido()
{
	pedirFormulaYNombrar(
		"Introduzca la formula del acido oxacido del que desea conocer su nomenclatura (ej. H2SO4):",
		nomenclaturaTradicionalOxacido);
}
