# Envoltorio fino sobre CMake, para que los comandos de siempre sigan
# funcionando sin tener que recordar la invocacion de CMake.
# La configuracion real del proyecto vive en CMakeLists.txt.

BUILD_DIR := build
WASM_DIR := build-wasm

# Se resuelve la ruta absoluta de cmake en vez de confiar en el PATH. Al
# activar Emscripten, su emsdk_env.sh antepone al PATH un directorio que
# contiene una *carpeta* llamada "cmake" (emscripten/cmake/Modules), y el
# shell que usa make la encuentra antes que el ejecutable real, fallando con
# "permiso denegado".
CMAKE := $(shell command -v cmake)
CTEST := $(shell command -v ctest)

.PHONY: all configure test run wasm servir-wasm clean

all: configure
	@$(CMAKE) --build $(BUILD_DIR)

configure:
	@$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release

test: all
	@$(CTEST) --test-dir $(BUILD_DIR) --output-on-failure

run: all
	@./$(BUILD_DIR)/cheminator

# Compila la libreria a WebAssembly. Requiere tener Emscripten activo en el
# entorno: source <ruta-de-emsdk>/emsdk_env.sh
wasm:
	@command -v emcmake >/dev/null 2>&1 || { \
		echo "Falta Emscripten. Ejecute primero:"; \
		echo "  source /ruta/a/emsdk/emsdk_env.sh"; exit 1; }
	@emcmake $(CMAKE) -S . -B $(WASM_DIR) -DCMAKE_BUILD_TYPE=Release -DCHEMINATOR_BUILD_TESTS=OFF
	@$(CMAKE) --build $(WASM_DIR)
	@echo ""
	@echo "Listo. Para probar la pagina: make servir-wasm"

# Los modulos ES y WebAssembly no se pueden cargar desde file://, hace falta
# servirlos por HTTP aunque sea en local.
servir-wasm: wasm
	@echo "Abra http://localhost:8000 (Ctrl+C para detener)"
	@cd $(WASM_DIR) && python3 -m http.server 8000

clean:
	@rm -rf $(BUILD_DIR) $(WASM_DIR)
