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

void oxido()
{
	char formula[TAM_MAX];

	std::cout << "\nIntroduzca la formula del oxido del que desea conocer su nomenclatura stock (ej. Fe2O3):";
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
	ResultadoNomenclatura resultadoNomenclatura = nomenclaturaStockOxido(parseada, nomenclatura);
	if (resultadoNomenclatura != ResultadoNomenclatura::OK)
	{
		std::cout << "\nError: " << mensajeError(resultadoNomenclatura) << "\n";
		return;
	}

	std::cout << "\nNomenclatura Stock del compuesto: " << nomenclatura << "\n";
}
