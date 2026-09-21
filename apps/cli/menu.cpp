#include "menu.hpp"

#include "cheminator/formula.hpp"
#include "cheminator/formulacion.hpp"
#include "cheminator/nomenclatura.hpp"

#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace cheminator::cli {
namespace {

// Lee una formula desde stdin. Se usa std::string y no un buffer fijo porque
// C++20 elimino la sobrecarga de operator>> para punteros crudos (P0487R1),
// justamente por ser una fuente clasica de desbordamiento.
std::string leerFormula()
{
	std::string entrada;
	std::cin >> entrada;

	if (std::cin.fail())
	{
		std::cin.clear();
		entrada.clear();
	}
	// Descarta el resto de la linea para que la siguiente lectura del menu no
	// herede entrada sobrante.
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	return entrada;
}

// Un nombre lleva espacios ("oxido de hierro (III)"), asi que no sirve la
// lectura con >>, que cortaria en el primer espacio y dejaria solo "oxido".
std::string leerLinea()
{
	std::string entrada;
	if (!std::getline(std::cin, entrada))
	{
		std::cin.clear();
		entrada.clear();
	}

	// Se recortan los espacios de los extremos; los interiores los colapsa
	// despues normalizarNombre().
	const std::size_t primero = entrada.find_first_not_of(" \t\r\n");
	if (primero == std::string::npos)
	{
		return {};
	}
	const std::size_t ultimo = entrada.find_last_not_of(" \t\r\n");

	return entrada.substr(primero, ultimo - primero + 1);
}

using Nombrador = ResultadoNomenclatura (*)(const Formula &);

// Flujo comun a las opciones que ya saben de que categoria es el compuesto.
void pedirFormulaYNombrar(std::string_view mensaje, Nombrador nombrador)
{
	std::cout << "\n" << mensaje;
	const std::string texto = leerFormula();

	if (texto.empty())
	{
		std::cout << "\nNo se ingreso ninguna formula.\n";
		return;
	}

	std::cout << "La formula es: " << texto << "\n";

	const auto analizada = parsearFormula(texto);
	if (!analizada)
	{
		std::cout << "Error: " << mensajeError(analizada.error()) << "\n";
		return;
	}

	const auto resultado = nombrador(analizada.valor());
	if (!resultado)
	{
		std::cout << "Error: " << mensajeError(resultado.error()) << "\n";
		return;
	}

	std::cout << "Nomenclatura del compuesto: " << resultado.valor().nombre << "\n";
}

} // namespace

void dibujarMenu()
{
	std::cout << "\nBienvenidos a Cheminator\n";
	std::cout << "Introduzca la formula del compuesto del que desea conocer su nomenclatura.\n";
	std::cout << "\nOpciones:\n";
	std::cout << "1)  Ingresar formula de oxido\n";
	std::cout << "2)  Ingresar formula de peroxido\n";
	std::cout << "3)  Ingresar formula de anhidrido\n";
	std::cout << "4)  Ingresar formula de acido hidracido\n";
	std::cout << "5)  Ingresar formula de acido oxacido\n";
	std::cout << "6)  Ingresar formula de base\n";
	std::cout << "7)  Ingresar formula de sal oxisal\n";
	std::cout << "8)  Detectar automaticamente (formula -> nombre, con explicacion)\n";
	std::cout << "9)  Obtener la formula a partir del nombre (nombre -> formula)\n";
	std::cout << "0)  Salir\n";
}

void oxido()
{
	pedirFormulaYNombrar("Introduzca la formula del oxido (ej. Fe2O3): ", nombrarOxido);
}

void peroxido()
{
	pedirFormulaYNombrar("Introduzca la formula del peroxido (ej. Na2O2): ", nombrarPeroxido);
}

void anhidrido()
{
	pedirFormulaYNombrar("Introduzca la formula del anhidrido (ej. SO3): ", nombrarAnhidrido);
}

void acidoHidracido()
{
	pedirFormulaYNombrar("Introduzca la formula del acido hidracido (ej. HCl): ", nombrarHidracido);
}

void acidoOxacido()
{
	pedirFormulaYNombrar("Introduzca la formula del acido oxacido (ej. H2SO4): ", nombrarOxacido);
}

void base()
{
	pedirFormulaYNombrar("Introduzca la formula de la base (ej. Ca(OH)2 o NaOH): ", nombrarBase);
}

void salOxisal()
{
	pedirFormulaYNombrar("Introduzca la formula de la sal oxisal (ej. Al2(SO4)3 o CaCO3): ", nombrarSal);
}

void detectarAutomaticamente()
{
	std::cout << "\nIntroduzca la formula del compuesto, sin indicar el tipo (ej. Fe2O3, HCl, Al2(SO4)3): ";
	const std::string texto = leerFormula();

	if (texto.empty())
	{
		std::cout << "\nNo se ingreso ninguna formula.\n";
		return;
	}

	std::cout << "La formula es: " << texto << "\n";

	const auto analizada = parsearFormula(texto);
	if (!analizada)
	{
		std::cout << "Error: " << mensajeError(analizada.error()) << "\n";
		return;
	}

	const auto resultado = nombrar(analizada.valor());
	if (!resultado)
	{
		std::cout << "No se pudo determinar el tipo de compuesto.\n";
		std::cout << "Error: " << mensajeError(resultado.error()) << "\n";
		return;
	}

	const Nomenclatura &nomenclatura = resultado.valor();

	std::cout << "Tipo de compuesto detectado: " << nombreCategoria(nomenclatura.categoria) << "\n";
	std::cout << "\nRazonamiento:\n";
	for (std::size_t i = 0; i < nomenclatura.pasos.size(); ++i)
	{
		std::cout << "  " << (i + 1) << ". " << nomenclatura.pasos[i] << "\n";
	}
	std::cout << "\nNomenclatura del compuesto: " << nomenclatura.nombre << "\n";
}

// El camino inverso: se escribe el nombre y sale la formula.
void formularDesdeNombre()
{
	std::cout << "\nIntroduzca el nombre del compuesto (ej. oxido de Hierro (III), acido sulfurico): ";
	const std::string texto = leerLinea();

	if (texto.empty())
	{
		std::cout << "\nNo se ingreso ningun nombre.\n";
		return;
	}

	std::cout << "El nombre es: " << texto << "\n";

	const auto resultado = formular(texto);
	if (!resultado)
	{
		std::cout << "No se pudo obtener la formula.\n";
		std::cout << "Error: " << mensajeError(resultado.error()) << "\n";
		return;
	}

	const Formulacion &formulacion = resultado.valor();

	std::cout << "Tipo de compuesto: " << nombreCategoria(formulacion.categoria) << "\n";
	std::cout << "\nRazonamiento:\n";
	for (std::size_t i = 0; i < formulacion.pasos.size(); ++i)
	{
		std::cout << "  " << (i + 1) << ". " << formulacion.pasos[i] << "\n";
	}
	std::cout << "\nFormula del compuesto: " << formulacion.formula << "\n";
}

} // namespace cheminator::cli
