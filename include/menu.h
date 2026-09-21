#ifndef CHEMINATOR_MENU_H
#define CHEMINATOR_MENU_H

// Muestra el menú principal con las opciones de nomenclatura disponibles.
void dibujarMenu();

// Pide una fórmula de óxido y muestra su nomenclatura Stock.
void oxido();

// Pide una fórmula de peróxido y muestra su nomenclatura Stock.
void peroxido();

// Pide una fórmula de anhídrido y muestra su nomenclatura tradicional.
void anhidrido();

// Pide una fórmula de ácido hidrácido y muestra su nomenclatura tradicional.
void acidoHidracido();

// Pide una fórmula de ácido oxácido y muestra su nomenclatura tradicional.
void acidoOxacido();

// Pide una fórmula de base (hidróxido) y muestra su nomenclatura Stock.
void base();

// Pide una fórmula de sal oxisal y muestra su nomenclatura tradicional.
void salOxisal();

// Pide una fórmula sin que el usuario indique la categoría, detecta
// automáticamente a qué tipo de compuesto corresponde, y muestra el
// resultado junto con el razonamiento paso a paso.
void detectarAutomaticamente();

#endif // CHEMINATOR_MENU_H
