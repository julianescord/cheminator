# Cheminator

Programa de consola en C++ para practicar la nomenclatura de compuestos químicos inorgánicos (óxidos, peróxidos, anhídridos, ácidos hidrácidos y oxácidos, bases y sales oxisal), obteniendo su nomenclatura Stock a partir de la fórmula.

> Proyecto educativo en desarrollo. El menú principal y el flujo de óxidos funcionan; el resto de opciones (peróxidos, anhídridos, ácidos, bases, sales) están planificadas pero aún no implementadas.

## Contenido

- [`src/Cheminator.cpp`](src/Cheminator.cpp): menú principal interactivo con las distintas opciones de nomenclatura.
- [`src/Chemin2.cpp`](src/Chemin2.cpp): prototipo independiente para el reconocimiento de elementos a partir de la fórmula (punto de partida para completar la lógica de `Cheminator.cpp`).

## Compilación

Requiere un compilador de C++ (g++, clang++ o MSVC).

```bash
g++ -Wall -o cheminator src/Cheminator.cpp
./cheminator
```

```bash
g++ -Wall -o chemin2 src/Chemin2.cpp
./chemin2
```

## Estado / Roadmap

- [x] Menú principal
- [x] Flujo de entrada de fórmula para óxidos
- [ ] Lógica de nomenclatura Stock real (actualmente no calcula el nombre, solo repite la fórmula)
- [ ] Peróxidos
- [ ] Anhídridos
- [ ] Ácidos hidrácidos y oxácidos
- [ ] Bases
- [ ] Sales oxisal
- [ ] Unificar `Chemin2.cpp` dentro del menú de `Cheminator.cpp`

## Licencia

Distribuido bajo la licencia MIT. Ver [LICENSE](LICENSE).
