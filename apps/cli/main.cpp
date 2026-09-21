#include "menu.hpp"
#include <cstdio>
#include <iostream>
#include <limits>

int main()
{
	bool repetir = true;

	while (repetir)
	{
		dibujarMenu();
		std::printf("=>");

		int opcion = 0;
		if (!(std::cin >> opcion))
		{
			// Dos casos distintos, ambos ignorados por la version anterior
			// (que usaba scanf y no revisaba su resultado): si la entrada se
			// agota, scanf/cin dejan `opcion` sin tocar y el bucle gira para
			// siempre; si el usuario escribe algo que no es un numero, el
			// token invalido nunca se consume y vuelve a fallar en cada
			// vuelta, con el mismo resultado.
			if (std::cin.eof())
			{
				break;
			}

			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "\nOpcion invalida: escriba el numero de una de las opciones del menu.\n";
			continue;
		}

		// Descarta el resto de la linea para que la siguiente lectura de
		// formula no herede lo que quedo despues del numero.
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch (opcion)
		{
			case 1: // Oxidos
				oxido();
				break;

			case 2: // Peroxidos
				peroxido();
				break;

			case 3: // Anhidridos
				anhidrido();
				break;

			case 4: // Acidos hidracidos
				acidoHidracido();
				break;

			case 5: // Acidos oxacidos
				acidoOxacido();
				break;

			case 6: // Bases
				base();
				break;

			case 7: // Sales oxisal
				salOxisal();
				break;

			case 8: // Deteccion automatica con explicacion
				detectarAutomaticamente();
				break;

			case 0: // Salir
				repetir = false;
				break;

			default:
				std::cout << "\nEsa opcion no existe en el menu.\n";
				break;
		}
	}

	return 0;
}
