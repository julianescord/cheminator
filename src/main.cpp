#include "menu.h"
#include <cstdio>
#include <iostream>

int main()
{
	int opcion;
	bool repetir = true;

	while (repetir)
	{
		dibujarMenu();
		std::printf("=>");
		std::scanf("%d", &opcion);

		switch (opcion)
		{
			case 1: // Oxidos
				oxido();
				break;

			case 2: // Peroxidos
				peroxido();
				break;

			case 0: // Salir
				repetir = false;
				break;

			default:
				std::cout << "\nLa formula escrita no pertenece a un compuesto quimico inorganico";
				break;
		}
	}

	return 0;
}
