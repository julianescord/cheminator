#include "menu.hpp"

#include <iostream>
#include <limits>

int main()
{
	using namespace cheminator::cli;

	bool repetir = true;

	while (repetir)
	{
		dibujarMenu();
		std::cout << "=> ";

		int opcion = 0;
		if (!(std::cin >> opcion))
		{
			// Dos casos que la version anterior ignoraba, porque usaba scanf
			// sin revisar su resultado: con la entrada agotada, `opcion`
			// quedaba sin tocar y el bucle giraba para siempre; con texto no
			// numerico, el token invalido nunca se consumia y volvia a fallar
			// en cada vuelta, con el mismo efecto.
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
			case 1: oxido(); break;
			case 2: peroxido(); break;
			case 3: anhidrido(); break;
			case 4: acidoHidracido(); break;
			case 5: acidoOxacido(); break;
			case 6: base(); break;
			case 7: salOxisal(); break;
			case 8: detectarAutomaticamente(); break;

			case 0:
				repetir = false;
				break;

			default:
				std::cout << "\nEsa opcion no existe en el menu.\n";
				break;
		}
	}

	return 0;
}
