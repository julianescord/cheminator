CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
SRC := $(wildcard src/*.cpp)
LIB_SRC := $(filter-out src/main.cpp,$(SRC))
TEST_SRC := $(wildcard tests/*.cpp)
BIN := cheminator
TEST_BIN := run_tests

.PHONY: all test clean run

all: $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(SRC)

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(LIB_SRC) $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -Itests -o $(TEST_BIN) $(LIB_SRC) $(TEST_SRC)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN) $(TEST_BIN)
