#include "menu.h"
#include "elementos.h"
#include <cstdio>
#include <iostream>

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

void oxido()
{
	char formula[TAM_MAX];
	char nomenclatura[TAM_MAX];

	std::cout << "\nIntroduzca la formula del oxido del que desea conocer su nomenclatura stock:";
	std::cin >> formula;

	obtenerNombreElemento(formula, nomenclatura);

	std::cout << "La formula es:" << formula;
	std::cout << "\nNomenclatura Stock del compuesto:";
	if (nomenclatura[0] != '\0')
	{
		std::cout << nomenclatura;
	}
	else
	{
		std::cout << "(elemento no reconocido todavia)";
	}
}
