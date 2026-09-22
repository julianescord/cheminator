# Registro de cambios

Todas las versiones publicadas de Cheminator, con los cambios que afectan a
quien usa la librería.

El proyecto sigue [versionado semántico](https://semver.org/lang/es/). Mientras
esté en `0.x`, una versión menor puede romper la compatibilidad, y por eso
`find_package(cheminator)` exige coincidencia de versión menor: pedir `0.1`
contra una instalación `0.2.0` falla en vez de enlazar en silencio.

## [0.2.0] — 2026-09-21

La librería pasa a funcionar en las dos direcciones y suma una octava
categoría de compuesto.

### Novedades

- **Formulación inversa** ([`formulacion.hpp`](include/cheminator/formulacion.hpp)).
  A partir del nombre en español se obtiene la fórmula: `"sulfato de Aluminio"`
  → `Al2(SO4)3`. Tolera mayúsculas, tildes y espacios de más. Disponible en las
  cuatro caras de la librería (C++, C, WebAssembly y consola).
- **Compuestos de coordinación** ([`coordinacion.hpp`](include/cheminator/coordinacion.hpp)).
  `K3[Fe(CN)6]` → `hexacianoferrato (III) de potasio`, con el estado de
  oxidación deducido del balance de cargas. Incluye una tabla de 18 ligandos.
- **Frontera en C** ([`cheminator.h`](include/cheminator/cheminator.h)) con
  tipos opacos, para embeber la librería desde Python, C#, Java, Rust o Go.
- **WebAssembly**: el mismo núcleo corre en el navegador sin servidor, con una
  demo publicada en [julianescord.github.io/cheminator](https://julianescord.github.io/cheminator/).
- **Tablas ampliadas**: los metales pasan de 15 a 29 y los no metales de 7 a
  13, con sus oxácidos y radicales correspondientes.
- El parser construye un **árbol** en vez de una lista plana, admite grupos
  anidados y corchetes, y lee la carga escrita como sufijo (`[Fe(CN)6]3-`).

### Cambios incompatibles

Quien venga de la 0.1.0 debe revisar tres cosas:

1. **`CategoriaCompuesto` tiene un valor nuevo**, `COMPLEJO`. Un `switch`
   exhaustivo sobre ese enum deja de compilar con `-Werror=switch`.
2. **`ErrorFormula` tiene dos valores nuevos**, `DEMASIADO_ANIDADO` y
   `CARGA_MAL_FORMADA`, y `ErrorFormulacion` uno, `COMPLEJO_NO_SOPORTADO`,
   con el mismo efecto sobre un `switch` exhaustivo.
3. **`Ca((OH))2` ya no es un error.** El parser admite anidamiento, que es lo
   que permite analizar la esfera de coordinación. Lo que sigue siendo
   inválido es un cierre sin apertura (`Ca(OH))2`) y los delimitadores
   cruzados (`Ca[OH)2`).

Además, `find_package(cheminator)` pasa a exigir coincidencia de versión
menor: antes `SameMajorVersion` daba por compatibles la 0.1 y la 0.2, que no
lo son. Un `find_package(cheminator 0.1 REQUIRED)` que antes pasaba en
silencio ahora falla con un mensaje que nombra la versión instalada.

### Correcciones

- **Peróxidos de metales divalentes.** Al formular, el subíndice del metal
  estaba fijo en 2, lo que daba `Ca2O4` en vez de `CaO2`. Ahora se cruza la
  valencia del metal con la carga del grupo peroxo.
- **Raíz de arsénico y selenio.** Llevan una i ante `-oso` pero no ante
  `-ico`, así que con una sola raíz salían `arseniico` y `seleniico`, que no
  existen.
- **Complejos neutros.** Se rechazaba el estado de oxidación cero, que es
  válido en carbonilos como `[Ni(CO)4]`.
- **Mensaje de error del parser.** Un cierre huérfano se reportaba como
  símbolo inválido, lo que mandaba a buscar el problema donde no estaba.
- **Nombre de un complejo al formular.** Escribir
  `"hexacianoferrato (III) de potasio"` respondía «el radical no está en la
  tabla», cuando el problema es que la formulación inversa todavía no cubre
  los complejos. Ahora devuelve `COMPLEJO_NO_SOPORTADO` y lo dice.
- **Copia de la página de ejemplo.** Al ser un `POST_BUILD` del módulo de
  WebAssembly, solo se ejecutaba cuando el módulo se reenlazaba: editar solo
  el HTML dejaba servida una versión vieja.

### Limitaciones conocidas

- La formulación inversa cubre las siete categorías simples, pero **no los
  compuestos de coordinación**: `K3[Fe(CN)6]` da su nombre, y el nombre no
  devuelve la fórmula. La librería lo detecta y lo dice en vez de fallar con
  un error engañoso.

### Verificación

- 177 aserciones automatizadas, incluida una prueba de ida y vuelta que nombra
  cada fórmula y vuelve a formular el nombre resultante.
- Barrido de las tablas completas: cada elemento con cada una de sus
  valencias, 144 casos sin fallos.
- Integración continua en tres frentes: compilación nativa, solo-librería con
  un consumidor en C, y WebAssembly ejecutado en Node.

## [0.1.0]

Primera versión con el núcleo separado como librería reutilizable.

- Las siete categorías de compuesto inorgánico simple: óxidos, peróxidos,
  anhídridos, ácidos hidrácidos y oxácidos, bases y sales oxisal.
- Detección automática de la categoría a partir de la fórmula.
- Explicación del razonamiento paso a paso.
- Migración de Makefile a CMake, con instalación y `find_package`.
- Tipos fuertes para valencia, subíndice y carga; tablas `constexpr`
  verificadas en tiempo de compilación.

[0.2.0]: https://github.com/julianescord/cheminator/releases/tag/v0.2.0
[0.1.0]: https://github.com/julianescord/cheminator/commits/master
