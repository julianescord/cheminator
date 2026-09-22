# Cheminator

**[▶ Probar la demo en el navegador](https://julianescord.github.io/cheminator/)** — sin instalar nada.

Librería C++ y programa de consola para la nomenclatura de compuestos químicos inorgánicos en español (óxidos, peróxidos, anhídridos, ácidos hidrácidos y oxácidos, bases, sales oxisal y compuestos de coordinación), en los sistemas Stock y tradicional.

Funciona en **las dos direcciones**: de la fórmula al nombre y del nombre a la fórmula. Además **detecta automáticamente** a qué categoría pertenece un compuesto y **explica su razonamiento paso a paso**, en vez de limitarse a devolver una respuesta.

> Versión 0.2.0 · Proyecto educativo en desarrollo. El núcleo está separado como librería reutilizable (`libcheminator`); el programa de consola es solo uno de sus clientes.

## Estructura del proyecto

```
cheminator/
├── include/cheminator/   # API publica de la libreria
│   ├── cheminator.h      # Frontera en C, para embeber desde otros lenguajes
│   ├── tipos.hpp         # Valencia, Subindice, Carga y la regla de intercambio
│   ├── elementos.hpp     # Tabla de metales: simbolo, nombre, valencias
│   ├── no_metales.hpp    # Tabla de no metales (para anhidridos)
│   ├── hidracidos.hpp    # No metales formadores de acidos hidracidos
│   ├── oxacidos.hpp      # Formulas y nombres de acidos oxacidos conocidos
│   ├── radicales.hpp     # Radicales (aniones poliatomicos) para sales
│   ├── formula.hpp       # Parseo de formulas (arbol, grupos anidados y carga)
│   ├── ligandos.hpp      # Ligandos para compuestos de coordinacion
│   ├── nomenclatura.hpp  # Reglas de nomenclatura, deteccion y explicacion
│   ├── formulacion.hpp   # El camino inverso: del nombre a la formula
│   └── coordinacion.hpp  # Complejos: esfera, ligandos y estado de oxidacion
├── src/                  # Implementacion de la libreria (sin entrada/salida)
├── apps/cli/             # Programa de consola: un cliente de la libreria
│   ├── main.cpp
│   ├── menu.hpp
│   └── menu.cpp
├── wasm/                 # Enlace con JavaScript y pagina de ejemplo
│   ├── bindings.cpp
│   └── demo.html
├── tests/                # Pruebas automatizadas (framework propio, sin dependencias)
├── .github/workflows/    # CI: nativo, solo-libreria, consumidor en C y WebAssembly
├── CMakeLists.txt
├── Makefile              # Envoltorio fino sobre CMake
├── CHANGELOG.md          # Cambios de cada version publicada
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

## WebAssembly

El mismo núcleo se compila para el navegador con Emscripten, sin servidor ni
instalación: todo el análisis ocurre en la máquina de quien lo usa.

La demo está publicada en
**[julianescord.github.io/cheminator](https://julianescord.github.io/cheminator/)**,
desplegada automáticamente desde este repositorio en cada push a `master`
([`.github/workflows/pages.yml`](.github/workflows/pages.yml)). Para compilarla
en local:

```bash
source /ruta/a/emsdk/emsdk_env.sh
make wasm          # genera build-wasm/cheminator.js + .wasm + index.html
make servir-wasm   # y lo sirve en http://localhost:8000
```

Desde JavaScript:

```js
import crearCheminator from './cheminator.js';

const Cheminator = await crearCheminator();
// De la formula al nombre...
const resultado = Cheminator.nombrar('Al2(SO4)3');
// { ok: true, nombre: "sulfato de Aluminio", categoria: "sal oxisal", pasos: [...] }

// ...y del nombre a la formula.
const inverso = Cheminator.formular('sulfato de Aluminio');
// { ok: true, formula: "Al2(SO4)3", categoria: "sal oxisal", pasos: [...] }
```

`pasos` llega como el vector de C++ enlazado por embind: se recorre con
`size()` y `get(i)`, no como un array de JavaScript.

## Usar como librería

Una vez instalada, otro proyecto CMake la consume así:

```cmake
find_package(cheminator 0.2 REQUIRED)
target_link_libraries(mi_programa PRIVATE cheminator::cheminator)
```

Mientras el proyecto siga en `0.x`, la versión pedida debe coincidir en el
número menor: una versión menor puede romper la compatibilidad, así que pedir
`0.1` contra una instalación `0.2` falla en vez de enlazar en silencio. Los
cambios de cada versión están en [CHANGELOG.md](CHANGELOG.md).

### Desde otros lenguajes

Además del API en C++, la librería expone una frontera en C
([`include/cheminator/cheminator.h`](include/cheminator/cheminator.h)) con tipos
opacos, que es lo que pueden consumir Python (ctypes/cffi), C#, Java, Rust, Go
y cualquier otro lenguaje con FFI:

```c
#include <cheminator/cheminator.h>

chem_resultado *r = chem_nombrar("Fe2O3");
if (chem_resultado_ok(r)) {
    chem_resultado_nombre(r);            /* "oxido de Hierro (III)" */
    chem_resultado_categoria(r);         /* "oxido"                 */
    chem_resultado_cantidad_pasos(r);    /* 4                       */
    chem_resultado_paso(r, 0);           /* primer paso del razonamiento */
}
chem_resultado_liberar(r);

/* El camino inverso usa el mismo tipo y se libera igual. */
chem_resultado *i = chem_formular("oxido de hierro (III)");
chem_resultado_formula(i);               /* "Fe2O3" */
chem_resultado_liberar(i);
```

### Desde C++

```cpp
#include <cheminator/formula.hpp>
#include <cheminator/nomenclatura.hpp>

using namespace cheminator;

if (const auto formula = parsearFormula("Fe2O3"))
{
    if (const auto resultado = nombrar(formula.valor()))
    {
        const Nomenclatura &n = resultado.valor();
        n.nombre;     // "oxido de Hierro (III)"
        n.categoria;  // CategoriaCompuesto::OXIDO
        n.pasos;      // el razonamiento, linea por linea
    }
    else
    {
        mensajeError(resultado.error());
    }
}
```

### Formulación inversa

El camino contrario: del nombre a la fórmula. Tolera mayúsculas, tildes y
espacios de más, así que `"Óxido de Hierro (III)"` y `"oxido de hierro (iii)"`
dan lo mismo.

```cpp
#include <cheminator/formulacion.hpp>

if (const auto resultado = cheminator::formular("sulfato de Aluminio"))
{
    resultado.valor().formula;    // "Al2(SO4)3"
    resultado.valor().categoria;  // CategoriaCompuesto::SAL_OXISAL
    resultado.valor().pasos;      // el razonamiento, linea por linea
}
```

Cuando un metal admite varias valencias hay que indicarla con número romano:
`"oxido de Hierro"` devuelve `FALTA_VALENCIA` en vez de adivinar entre `FeO` y
`Fe2O3`.

### Compuestos de coordinación

Es la categoría que justifica que el parser construya un árbol en vez de una
lista: para deducir el estado de oxidación del átomo central hay que saber
**qué está dentro de la esfera** y qué fuera, algo que una lista plana de
`{símbolo, subíndice}` no puede representar.

```cpp
#include <cheminator/coordinacion.hpp>

if (const auto f = parsearFormula("K3[Fe(CN)6]"))
{
    const auto complejo = analizarComplejo(f.valor());
    complejo.valor().simboloCentral;      // "Fe"
    complejo.valor().estadoOxidacion;     // Valencia{3}, deducido del balance
    complejo.valor().indiceCoordinacion;  // 6
    complejo.valor().cargaEsfera;         // Carga{-3}

    nombrarComplejo(f.valor()).valor().nombre;
    // "hexacianoferrato (III) de potasio"
}
```

`K3[Fe(CN)6]` y `K4[Fe(CN)6]` tienen los mismos elementos y solo se distinguen
por el balance de cargas: el primero es hierro (III) y el segundo hierro (II).

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

Y en la dirección contraria:

```
=> 9
Introduzca el nombre del compuesto (ej. oxido de Hierro (III), acido sulfurico): sulfato de Aluminio
El nombre es: sulfato de Aluminio
Tipo de compuesto: sal oxisal

Razonamiento:
  1. El radical "sulfato" es SO4 con carga 2-.
  2. El metal es Aluminio (Al).
  3. Aluminio tiene una unica valencia (3), asi que no hace falta indicarla.
  4. Se cruzan la valencia del metal (3) y la carga del radical (2): Al2 y SO43.

Formula del compuesto: Al2(SO4)3
```

Y con un compuesto de coordinación:

```
=> 8
Introduzca la formula del compuesto, sin indicar el tipo (ej. Fe2O3, Al2(SO4)3, K3[Fe(CN)6]): K3[Fe(CN)6]
La formula es: K3[Fe(CN)6]
Tipo de compuesto detectado: compuesto de coordinacion

Razonamiento:
  1. El atomo central es Hierro (Fe), dentro de los corchetes.
  2. Ligandos en orden alfabetico: ciano x6.
  3. Indice de coordinacion: 6.
  4. La esfera tiene carga -3, y los ligandos suman -6.
  5. Por diferencia, Hierro actua con estado de oxidacion 3.
  6. La esfera es un anion, asi que el metal toma el sufijo -ato: "ferrato".
  7. Fuera de la esfera esta potasio, que completa el nombre.

Nomenclatura del compuesto: hexacianoferrato (III) de potasio
```

## Alcance actual

**Metales (29)** — H, Li, Na, K, Rb, Cs, Be, Mg, Ca, Sr, Ba, Ra, Al, Zn, Cd, Ag, O, Cu, Hg, Fe, Co, Ni, Cr, Mn, Au, Pb, Sn, Pt, Ti (ver [`src/elementos.cpp`](src/elementos.cpp)).

**No metales para anhídridos y oxácidos (13)** — B, C, Si, N, P, As, Sb, S, Se, Te, Cl, Br, I (ver [`src/no_metales.cpp`](src/no_metales.cpp), [`src/oxacidos.cpp`](src/oxacidos.cpp)). El flúor no aparece: al ser el elemento más electronegativo, no forma óxidos ácidos.

**No metales para ácidos hidrácidos (7)** — F, Cl, Br, I, S, Se, Te (ver [`src/hidracidos.cpp`](src/hidracidos.cpp)).

**Radicales para sales oxisal (30)** — borato, carbonato, silicato, nitrito/nitrato, fosfito/fosfato, arsenito/arseniato, antimonito/antimoniato, (hipo)sulfito/sulfato, selenito/selenato, telurito/telurato, y los 4 oxianiones de Cl/Br/I (ver [`src/radicales.cpp`](src/radicales.cpp)).

**Ligandos para compuestos de coordinación (18)** — aniónicos (ciano, hidroxo, cloro, bromo, iodo, fluoro, nitro, tiociano, oxo, tio, oxalato, sulfato, tiosulfato) y neutros (acua, amin, carbonilo, nitrosilo, etilendiamino), ver [`src/ligandos.cpp`](src/ligandos.cpp).

Cada fila de cada tabla está verificada en las dos direcciones: se genera el
compuesto, se nombra y se vuelve a formular, comprobando que sale la fórmula
original.

## Estado / Roadmap

- [x] Parseo de fórmulas, incluidos grupos entre paréntesis (`Ca(OH)2`, `Al2(SO4)3`)
- [x] Las 7 categorías de compuesto, en nomenclatura Stock o tradicional según corresponda
- [x] Detección automática de la categoría a partir de la fórmula
- [x] Explicación del razonamiento paso a paso
- [x] Núcleo separado como librería reutilizable, instalable vía CMake
- [x] Tipos fuertes para valencia/subíndice/carga, tablas `constexpr` verificadas en compilación, `std::string_view`
- [x] Frontera `extern "C"` para embeber desde otros lenguajes
- [x] Compilación a WebAssembly con página de ejemplo
- [x] Pruebas automatizadas y CI (nativo, solo-librería, consumidor en C y WebAssembly)
- [x] Formulación inversa: escribir el nombre en español y obtener la fórmula
- [x] Ampliar tablas de metales, no metales y radicales
- [x] Compuestos de coordinación (`[Fe(CN)6]³⁻`), con esfera de coordinación y estado de oxidación deducido
- [ ] Formulación inversa de complejos: hoy van solo de fórmula a nombre

## Licencia

Distribuido bajo la licencia MIT. Ver [LICENSE](LICENSE).
