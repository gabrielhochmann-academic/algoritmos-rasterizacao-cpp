#include <exception>
#include <iostream>
#include <string>

#include "exercicios.hpp"

namespace {

void exibirUso(const char* nomePrograma) {
    std::cout << "Uso: " << nomePrograma << " [1|2|3|4|todos]\n"
              << "  1     Executa Bresenham (ponto médio).\n"
              << "  2     Executa Cohen-Sutherland.\n"
              << "  3     Executa desenho de polígonos com Bresenham.\n"
              << "  4     Executa preenchimento Scanline.\n"
              << "  todos Executa os quatro exercícios em sequência.\n";
}

int executarTodos() {
    int resultado = 0;
    resultado |= executarExercicio1();
    std::cout << '\n';
    resultado |= executarExercicio2();
    std::cout << '\n';
    resultado |= executarExercicio3();
    std::cout << '\n';
    resultado |= executarExercicio4();
    return resultado;
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::string opcao = (argc >= 2) ? argv[1] : "todos";

    try {
        if (opcao == "1") {
            return executarExercicio1();
        }
        if (opcao == "2") {
            return executarExercicio2();
        }
        if (opcao == "3") {
            return executarExercicio3();
        }
        if (opcao == "4") {
            return executarExercicio4();
        }
        if (opcao == "todos") {
            return executarTodos();
        }

        exibirUso(argv[0]);
        return 1;
    } catch (const std::exception& erro) {
        std::cerr << "Erro durante a execução: " << erro.what() << '\n';
        return 1;
    }
}
