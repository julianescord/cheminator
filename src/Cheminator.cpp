#include <iostream>
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
using namespace std;

char formula[50];
char nomenclatura[50];
														//FUNCION ELEMENTOS
//dasdasda

														//FUNCION OXIDO
void oxido()
{
	cout << "\nIntroduzca la formula del oxido del que desea conocer su nomenclatura stock:";
	cin >> formula;

	cout << "La formula es:" << formula;
	cout << "\nNomenclatura Stock del compuesto:";
}//fin de funcion de oxido



//Menu principal
void dibujarMenu(void) {
	cout << "Bienvenidos a Cheminator";

	cout << "\nIntroduzca la formula del compuesto del que desea conocer en base a las siguientes opciones.";


	printf("\n\nOpciones:\n");
	printf("1)  Ingresar formula de oxido\n");
	printf("2)  Ingresar formula de peroxido\n");
	printf("3)  Ingresar formula de anhidrido\n");
	printf("4)  Ingresar formula de acido hidracido\n");
	printf("5)  Ingresar formula de acido oxacido\n");
	printf("6)  Ingresar formula de base\n");
	printf("7)  Ingresar formula de sal oxisal\n");
	printf("0)  Salir\n");
}
void pausa(void)
{
	printf("\nPresione la barra espaciadora y enter para continuar");
	while (getchar() != ' ');
}
														//PROGRAMA  PRINCIPAL
int main(void)
{
	int opcion;
	bool repetir = true;
	while (repetir)
	{
		dibujarMenu();
		printf("=>");
		scanf("%d", &opcion);
		switch (opcion)
		{
			case 1: //Oxidos
				oxido();
				break;

			case 0: //Salir
				repetir = false;
				break;

			default:
				cout << "\nLa formula escrita no pertenece a un compuesto quimico inorganico";
				break;
		}//fin switch
	}//fin while
	return 0;
}//fin main
