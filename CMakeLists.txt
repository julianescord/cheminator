cmake_minimum_required(VERSION 3.16)

project(cheminator
	VERSION 0.1.0
	DESCRIPTION "Libreria de nomenclatura quimica inorganica en espanol"
	LANGUAGES CXX)

# El proyecto se consume de dos formas: como librería embebida en otro
# software (el caso principal) y como programa de consola. Por eso el CLI y
# las pruebas son opcionales: quien solo quiere la librería los apaga.
option(CHEMINATOR_BUILD_CLI "Compilar el programa de consola" ON)
option(CHEMINATOR_BUILD_TESTS "Compilar las pruebas automatizadas" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Librería: el núcleo reutilizable, sin ninguna dependencia de entrada/salida
# por consola. Es lo que se embebe en otro software o se compila a WebAssembly.
# ---------------------------------------------------------------------------
add_library(cheminator
	src/elementos.cpp
	src/no_metales.cpp
	src/hidracidos.cpp
	src/oxacidos.cpp
	src/radicales.cpp
	src/formula.cpp
	src/nomenclatura.cpp
	src/c_api.cpp)

add_library(cheminator::cheminator ALIAS cheminator)

target_include_directories(cheminator PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>)

# La version vive en un solo lugar (el project() de arriba) y llega al codigo
# desde aqui, en vez de repetirse escrita a mano en algun archivo fuente.
target_compile_definitions(cheminator PRIVATE CHEMINATOR_VERSION="${PROJECT_VERSION}")

if(MSVC)
	target_compile_options(cheminator PRIVATE /W4)
else()
	target_compile_options(cheminator PRIVATE -Wall -Wextra)
endif()

set_target_properties(cheminator PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR})

# ---------------------------------------------------------------------------
# Programa de consola: un cliente más de la librería, no parte de ella.
# ---------------------------------------------------------------------------
if(CHEMINATOR_BUILD_CLI AND NOT EMSCRIPTEN)
	add_executable(cheminator_cli
		apps/cli/main.cpp
		apps/cli/menu.cpp)

	target_link_libraries(cheminator_cli PRIVATE cheminator)
	set_target_properties(cheminator_cli PROPERTIES OUTPUT_NAME cheminator)

	if(MSVC)
		target_compile_options(cheminator_cli PRIVATE /W4)
	else()
		target_compile_options(cheminator_cli PRIVATE -Wall -Wextra)
	endif()
endif()

# ---------------------------------------------------------------------------
# WebAssembly: el mismo núcleo, compilado para el navegador. Este bloque solo
# aplica al configurar con `emcmake cmake`; en un build nativo no existe.
# ---------------------------------------------------------------------------
if(EMSCRIPTEN)
	add_executable(cheminator_wasm wasm/bindings.cpp)

	target_link_libraries(cheminator_wasm PRIVATE cheminator)
	target_compile_definitions(cheminator_wasm PRIVATE CHEMINATOR_VERSION="${PROJECT_VERSION}")
	target_compile_options(cheminator_wasm PRIVATE -Wall -Wextra)

	set_target_properties(cheminator_wasm PROPERTIES
		OUTPUT_NAME cheminator
		# MODULARIZE entrega una fabrica en vez de contaminar el ambito
		# global, para poder incrustar el modulo en una pagina que ya tenga
		# su propio codigo.
		LINK_FLAGS "--bind -sMODULARIZE=1 -sEXPORT_NAME=crearCheminator -sALLOW_MEMORY_GROWTH=1 -sEXPORT_ES6=1")

	# La pagina de ejemplo se copia junto a los artefactos para poder servir
	# el directorio de build tal cual.
	add_custom_command(TARGET cheminator_wasm POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${CMAKE_CURRENT_SOURCE_DIR}/wasm/demo.html"
			"$<TARGET_FILE_DIR:cheminator_wasm>/index.html")
endif()

# ---------------------------------------------------------------------------
# Pruebas: enlazan solo contra la librería (antes arrastraban el código de
# consola sin necesitarlo).
# ---------------------------------------------------------------------------
if(CHEMINATOR_BUILD_TESTS)
	enable_testing()

	add_executable(run_tests
		tests/main_tests.cpp
		tests/test_tipos.cpp
		tests/test_formula.cpp
		tests/test_elementos.cpp
		tests/test_nomenclatura.cpp
		tests/test_deteccion.cpp)

	target_link_libraries(run_tests PRIVATE cheminator)
	target_include_directories(run_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests)

	if(MSVC)
		target_compile_options(run_tests PRIVATE /W4)
	else()
		target_compile_options(run_tests PRIVATE -Wall -Wextra)
	endif()

	add_test(NAME cheminator_tests COMMAND run_tests)
endif()

# ---------------------------------------------------------------------------
# Instalación: permite que otro proyecto haga find_package(cheminator).
# ---------------------------------------------------------------------------
include(GNUInstallDirs)

install(TARGETS cheminator
	EXPORT cheminatorTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY include/cheminator DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT cheminatorTargets
	FILE cheminatorTargets.cmake
	NAMESPACE cheminator::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cheminator)

# find_package(cheminator) busca un cheminatorConfig.cmake: exportar solo los
# targets no alcanza, hay que generar tambien ese archivo de configuracion.
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/cheminatorConfigVersion.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cheminatorConfig.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cheminatorConfig.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cheminator)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/cheminatorConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/cheminatorConfigVersion.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cheminator)
