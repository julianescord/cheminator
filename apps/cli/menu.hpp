#ifndef CHEMINATOR_CLI_MENU_HPP
#define CHEMINATOR_CLI_MENU_HPP

// Interfaz de consola. Vive fuera de la libreria a proposito: el nucleo no
// hace ninguna entrada ni salida, que es lo que permite embeberlo.
namespace cheminator::cli {

// Muestra el menu principal con las opciones disponibles.
void dibujarMenu();

// Cada opcion pide una formula y muestra el resultado.
void oxido();
void peroxido();
void anhidrido();
void acidoHidracido();
void acidoOxacido();
void base();
void salOxisal();

// Pide una formula sin que el usuario indique la categoria, la detecta y
// muestra el razonamiento paso a paso ademas del nombre.
void detectarAutomaticamente();

} // namespace cheminator::cli

#endif // CHEMINATOR_CLI_MENU_HPP
