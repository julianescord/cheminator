# Cheminator

Programa de consola en C++ para practicar la nomenclatura de compuestos químicos inorgánicos (óxidos, peróxidos, anhídridos, ácidos hidrácidos y oxácidos, bases y sales oxisal), obteniendo su nomenclatura Stock a partir de la fórmula.

> Proyecto educativo en desarrollo. El menú principal y la nomenclatura Stock de óxidos funcionan de punta a punta; el resto de opciones (peróxidos, anhídridos, ácidos, bases, sales) están planificadas pero aún no implementadas.

## Estructura del proyecto

```
cheminator/
├── include/              # Declaraciones (headers)
│   ├── elementos.h       # Tabla de elementos: símbolo, nombre, valencias
│   ├── formula.h         # Parseo de fórmulas químicas
│   ├── menu.h            # Menú y flujo de cada opción
│   └── nomenclatura.h    # Reglas de nomenclatura Stock
├── src/                  # Implementación
│   ├── elementos.cpp
│   ├── formula.cpp
│   ├── menu.cpp
│   ├── nomenclatura.cpp
│   └── main.cpp          # Punto de entrada del programa
├── tests/                # Pruebas automatizadas (framework propio, sin dependencias)
│   ├── test_runner.h
│   ├── main_tests.cpp
│   ├── test_formula.cpp
│   ├── test_elementos.cpp
│   └── test_nomenclatura.cpp
├── .github/workflows/build.yml  # CI: compila y corre los tests en cada push/PR
├── Makefile
└── README.md
```

## Compilación

Requiere un compilador de C++ (g++, clang++ o MSVC) compatible con C++17.

```bash
make        # compila el binario ./cheminator
make run    # compila y ejecuta
make test   # compila y corre las pruebas automatizadas
make clean  # elimina los binarios generados
```

También se puede compilar manualmente:

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude -o cheminator src/*.cpp
./cheminator
```

## Ejemplo de uso

```
=> 1
Introduzca la formula del oxido del que desea conocer su nomenclatura stock (ej. Fe2O3): Fe2O3
La formula es: Fe2O3
Nomenclatura Stock del compuesto: oxido de Hierro (III)
```

Elementos soportados actualmente: H, Li, Na, K, Ca, Mg, Al, Zn, Ag, Cu, Fe, Au, Pb, Sn, O (ver [`src/elementos.cpp`](src/elementos.cpp)).

## Estado / Roadmap

- [x] Menú principal
- [x] Parseo de fórmulas químicas (símbolo + subíndice, con validación de errores)
- [x] Tabla de elementos con valencias
- [x] Nomenclatura Stock completa para óxidos (deducción de valencia a partir de la fórmula)
- [x] Pruebas automatizadas y CI en GitHub Actions
- [ ] Peróxidos
- [ ] Anhídridos
- [ ] Ácidos hidrácidos y oxácidos
- [ ] Bases
- [ ] Sales oxisal

## Licencia

Distribuido bajo la licencia MIT. Ver [LICENSE](LICENSE).
