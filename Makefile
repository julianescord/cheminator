# Envoltorio fino sobre CMake, para que los comandos de siempre sigan
# funcionando sin tener que recordar la invocacion de CMake.
# La configuracion real del proyecto vive en CMakeLists.txt.

BUILD_DIR := build

.PHONY: all configure test run clean

all: configure
	@cmake --build $(BUILD_DIR)

configure:
	@cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release

test: all
	@ctest --test-dir $(BUILD_DIR) --output-on-failure

run: all
	@./$(BUILD_DIR)/cheminator

clean:
	@rm -rf $(BUILD_DIR)
