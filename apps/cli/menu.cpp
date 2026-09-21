#include "menu.hpp"
#include "cheminator/elementos.hpp"
#include "cheminator/formula.hpp"
#include "cheminator/nomenclatura.hpp"
#include <cstdio>
#include <iostream>
#include <limits>
#include <string>

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
	std::printf("8)  Detectar automaticamente (con explicacion paso a paso)\n");
	std::printf("0)  Salir\n");
}

// Lee una fórmula desde stdin y la devuelve como std::string, que crece sola
// según lo que escriba el usuario. Antes esto leía sobre un char[] usando
// `std::cin >> std::setw(n) >> puntero`, pero C++20 eliminó esa sobrecarga
// de operator>> para punteros crudos (P0487R1) precisamente porque no puede
// conocer el tamaño del buffer y es una fuente clásica de desbordamiento.
static std::string leerFormula()
{
	std::string destino;
	std::cin >> destino;

	if (std::cin.fail())
	{
		std::cin.clear();
		destino.clear();
	}
	// Descarta cualquier resto de la línea (p.ej. espacios extra u otro token)
	// para que la siguiente lectura del menú no herede entrada sobrante.
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	return destino;
}

// Firma común de las funciones nomenclaturaStock*/nomenclaturaTradicional*.
using CalculadoraNomenclatura = ResultadoNomenclatura (*)(const FormulaParseada &, char[TAM_MAX], Explicacion *);

// Flujo compartido por las distintas opciones del menú: pide una fórmula,
// la parsea, calcula su nomenclatura con la función indicada y reporta el
// resultado o el error correspondiente.
static void pedirFormulaYNombrar(const char *mensajeEntrada, CalculadoraNomenclatura calcular)
{
	std::cout << "\n" << mensajeEntrada;
	const std::string formula = leerFormula();

	if (formula.empty())
	{
		std::cout << "\nNo se ingreso ninguna formula.\n";
		return;
	}

	std::cout << "La formula es: " << formula;

	FormulaParseada parseada;
	ResultadoParseo resultadoParseo = parsearFormula(formula.c_str(), parseada);
	if (resultadoParseo != ResultadoParseo::OK)
	{
		std::cout << "\nError: " << mensajeError(resultadoParseo) << "\n";
		return;
	}

	char nomenclatura[TAM_MAX];
	ResultadoNomenclatura resultadoNomenclatura = calcular(parseada, nomenclatura, nullptr);
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

void base()
{
	pedirFormulaYNombrar(
		"Introduzca la formula de la base del que desea conocer su nomenclatura stock (ej. Ca(OH)2 o NaOH):",
		nomenclaturaStockBase);
}

void salOxisal()
{
	pedirFormulaYNombrar(
		"Introduzca la formula de la sal oxisal de la que desea conocer su nomenclatura (ej. Al2(SO4)3 o CaCO3):",
		nomenclaturaTradicionalSal);
}

void detectarAutomaticamente()
{
	std::cout << "\nIntroduzca la formula del compuesto (sin indicar el tipo, ej. Fe2O3, HCl, Al2(SO4)3):";
	const std::string formula = leerFormula();

	if (formula.empty())
	{
		std::cout << "\nNo se ingreso ninguna formula.\n";
		return;
	}

	std::cout << "La formula es: " << formula << "\n";

	FormulaParseada parseada;
	ResultadoParseo resultadoParseo = parsearFormula(formula.c_str(), parseada);
	if (resultadoParseo != ResultadoParseo::OK)
	{
		std::cout << "Error: " << mensajeError(resultadoParseo) << "\n";
		return;
	}

	char nomenclatura[TAM_MAX];
	CategoriaCompuesto categoria;
	Explicacion explicacion;
	ResultadoNomenclatura resultadoNomenclatura = detectarYNombrar(parseada, nomenclatura, categoria, &explicacion);

	if (resultadoNomenclatura != ResultadoNomenclatura::OK)
	{
		std::cout << "No se pudo determinar el tipo de compuesto.\n";
		std::cout << "Error: " << mensajeError(resultadoNomenclatura) << "\n";
		return;
	}

	std::cout << "Tipo de compuesto detectado: " << nombreCategoria(categoria) << "\n";
	std::cout << "\nRazonamiento:\n";
	for (int i = 0; i < explicacion.cantidadPasos; i++)
	{
		std::cout << "  " << (i + 1) << ". " << explicacion.pasos[i] << "\n";
	}
	std::cout << "\nNomenclatura del compuesto: " << nomenclatura << "\n";
}
