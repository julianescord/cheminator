# Cheminator

Librería C++ y programa de consola para la nomenclatura de compuestos químicos inorgánicos en español (óxidos, peróxidos, anhídridos, ácidos hidrácidos y oxácidos, bases y sales oxisal), en los sistemas Stock y tradicional.

Además de dar el nombre a partir de la fórmula, Cheminator puede **detectar automáticamente** a qué categoría pertenece un compuesto y **explicar su razonamiento paso a paso**, en vez de limitarse a devolver una respuesta.

> Proyecto educativo en desarrollo. El núcleo está separado como librería reutilizable (`libcheminator`); el programa de consola es solo uno de sus clientes.

## Estructura del proyecto

```
cheminator/
├── include/cheminator/   # API publica de la libreria
│   ├── elementos.hpp     # Tabla de metales: simbolo, nombre, valencias
│   ├── no_metales.hpp    # Tabla de no metales (para anhidridos)
│   ├── hidracidos.hpp    # No metales formadores de acidos hidracidos
│   ├── oxacidos.hpp      # Formulas y nombres de acidos oxacidos conocidos
│   ├── radicales.hpp     # Radicales (aniones poliatomicos) para sales
│   ├── formula.hpp       # Parseo de formulas (incluye grupos entre parentesis)
│   └── nomenclatura.hpp  # Reglas de nomenclatura, deteccion y explicacion
├── src/                  # Implementacion de la libreria (sin entrada/salida)
├── apps/cli/             # Programa de consola: un cliente de la libreria
│   ├── main.cpp
│   ├── menu.hpp
│   └── menu.cpp
├── tests/                # Pruebas automatizadas (framework propio, sin dependencias)
├── .github/workflows/    # CI: compila, prueba y verifica el build de solo-libreria
├── CMakeLists.txt
├── Makefile              # Envoltorio fino sobre CMake
└── README.md
```

La librería no contiene ninguna llamada de entrada/salida por consola: todo eso vive en `apps/cli/`. Esa separación es lo que permite embeberla en otro software.

## Compilación

Requiere CMake 3.16+ y un compilador con soporte de C++20 (g++ 10+, clang 12+, MSVC 2019+).

```bash
make        # configura y compila todo
make run    # compila y ejecuta el programa de consola
make test   # compila y corre las pruebas automatizadas
make clean  # elimina el directorio build/
```

O directamente con CMake:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Para compilar **solo la librería**, sin el programa de consola ni las pruebas:

```bash
cmake -S . -B build -DCHEMINATOR_BUILD_CLI=OFF -DCHEMINATOR_BUILD_TESTS=OFF
cmake --build build
cmake --install build --prefix /donde/instalar
```

## Usar como librería

Una vez instalada, otro proyecto CMake la consume así:

```cmake
find_package(cheminator REQUIRED)
target_link_libraries(mi_programa PRIVATE cheminator::cheminator)
```

```cpp
#include <cheminator/formula.hpp>
#include <cheminator/nomenclatura.hpp>

FormulaParseada f;
if (parsearFormula("Fe2O3", f) == ResultadoParseo::OK)
{
    char nombre[TAM_MAX];
    CategoriaCompuesto categoria;
    Explicacion explicacion;

    if (detectarYNombrar(f, nombre, categoria, &explicacion) == ResultadoNomenclatura::OK)
    {
        // nombre    -> "oxido de Hierro (III)"
        // categoria -> CategoriaCompuesto::OXIDO
        // explicacion.pasos -> el razonamiento, linea por linea
    }
}
```

## Ejemplo de uso

```
=> 8
Introduzca la formula del compuesto (sin indicar el tipo, ej. Fe2O3, HCl, Al2(SO4)3): Fe2O3
La formula es: Fe2O3
Tipo de compuesto detectado: oxido

Razonamiento:
  1. Se identifica el metal (Fe, subindice 2) y el oxigeno (subindice 3).
  2. Hierro (Fe) tiene 2 valencia(s) conocida(s) para oxidos.
  3. La proporcion 2:3 (metal:oxigeno) corresponde a la valencia 3.
  4. Hierro tiene mas de una valencia -> se indica con numero romano (III).

Nomenclatura del compuesto: oxido de Hierro (III)
```

## Alcance actual

Metales: H, Li, Na, K, Ca, Mg, Al, Zn, Ag, Cu, Fe, Au, Pb, Sn (ver [`src/elementos.cpp`](src/elementos.cpp)).
No metales para anhídridos y oxácidos: C, N, P, S, Cl, Br, I (ver [`src/no_metales.cpp`](src/no_metales.cpp), [`src/oxacidos.cpp`](src/oxacidos.cpp)).
No metales para ácidos hidrácidos: F, Cl, Br, I, S, Se, Te (ver [`src/hidracidos.cpp`](src/hidracidos.cpp)).
Radicales para sales oxisal: carbonato, nitrito/nitrato, fosfito/fosfato, (hipo)sulfito/sulfato, y los 4 oxianiones de Cl/Br/I (ver [`src/radicales.cpp`](src/radicales.cpp)).

## Estado / Roadmap

- [x] Parseo de fórmulas, incluidos grupos entre paréntesis (`Ca(OH)2`, `Al2(SO4)3`)
- [x] Las 7 categorías de compuesto, en nomenclatura Stock o tradicional según corresponda
- [x] Detección automática de la categoría a partir de la fórmula
- [x] Explicación del razonamiento paso a paso
- [x] Núcleo separado como librería reutilizable, instalable vía CMake
- [x] Pruebas automatizadas y CI
- [ ] Frontera `extern "C"` para embeber desde otros lenguajes
- [ ] Compilación a WebAssembly (Emscripten) para uso en navegador
- [ ] Modernización: tipos fuertes para valencia/subíndice/carga, `std::string_view`, tablas `constexpr` verificadas en compilación
- [ ] Formulación inversa: escribir el nombre en español y obtener la fórmula
- [ ] Ampliar tablas de metales, no metales y radicales

## Licencia

Distribuido bajo la licencia MIT. Ver [LICENSE](LICENSE).
