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
char oxido[50] = "";
char H[20] = "H";

void elementos(char formula[], char resultado[]);

//Menu principal
int main()
{
	using namespace std;
	char nombre[50] = "";
	cout << "\nIntroduzca la formula del compuesto del que desea conocer su nomenclatura stock:";
	cin >> formula;
	cout << "La formula es:" << formula;
	elementos(formula, nombre);
	cout << "\nNomenclatura Stock del compuesto:" << nombre;
	return 0;
}

void elementos(char formula[], char resultado[])
{
	using namespace std;
	if (strcmp(formula, H) == 0)
	{
		strcpy(resultado, "Hidrogeno");
	}
}
