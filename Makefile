CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -Iinclude
BUILD_DIR := build
EXECUTAVEL := $(BUILD_DIR)/rasterizacao
FONTES := $(wildcard src/*.cpp)

.PHONY: todos executar exercicio1 exercicio2 exercicio3 exercicio4 limpar

todos: $(EXECUTAVEL)

$(EXECUTAVEL): $(FONTES) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(FONTES) -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

executar: todos
	./$(EXECUTAVEL) todos

exercicio1: todos
	./$(EXECUTAVEL) 1

exercicio2: todos
	./$(EXECUTAVEL) 2

exercicio3: todos
	./$(EXECUTAVEL) 3

exercicio4: todos
	./$(EXECUTAVEL) 4

limpar:
	rm -rf $(BUILD_DIR) saidas resultados
