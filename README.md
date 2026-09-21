# Cheminator

Programa de consola en C++ para practicar la nomenclatura de compuestos químicos inorgánicos (óxidos, peróxidos, anhídridos, ácidos hidrácidos y oxácidos, bases y sales oxisal), obteniendo su nomenclatura Stock a partir de la fórmula.

> Proyecto educativo en desarrollo. El menú principal y el flujo de óxidos funcionan; el resto de opciones (peróxidos, anhídridos, ácidos, bases, sales) están planificadas pero aún no implementadas.

## Estructura del proyecto

```
cheminator/
├── include/          # Declaraciones (headers)
│   ├── elementos.h
│   └── menu.h
├── src/              # Implementación
│   ├── elementos.cpp # Reconocimiento de elementos a partir de su símbolo
│   ├── menu.cpp       # Menú y flujo de cada opción de nomenclatura
│   └── main.cpp       # Punto de entrada del programa
├── Makefile
└── README.md
```

## Compilación

Requiere un compilador de C++ (g++, clang++ o MSVC) compatible con C++17.

```bash
make        # compila el binario ./cheminator
make run    # compila y ejecuta
make clean  # elimina el binario
```

También se puede compilar manualmente:

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude -o cheminator src/*.cpp
./cheminator
```

## Estado / Roadmap

- [x] Menú principal
- [x] Flujo de entrada de fórmula para óxidos
- [x] Reconocimiento básico de elementos (tabla en `elementos.cpp`, por ahora solo Hidrógeno)
- [ ] Lógica de nomenclatura Stock completa para óxidos (subíndices, valencias)
- [ ] Peróxidos
- [ ] Anhídridos
- [ ] Ácidos hidrácidos y oxácidos
- [ ] Bases
- [ ] Sales oxisal

## Licencia

Distribuido bajo la licencia MIT. Ver [LICENSE](LICENSE).
