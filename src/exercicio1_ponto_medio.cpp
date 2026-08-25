#include <algorithm>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "bresenham.hpp"
#include "imagem_ppm.hpp"

namespace {

struct CasoDeTeste {
    int xInicial;
    int yInicial;
    int xFinal;
    int yFinal;
};

std::string nomeDoArquivo(std::size_t indice, const CasoDeTeste& teste) {
    std::ostringstream nome;
    nome << "saidas/exercicio1/teste_";
    if (indice < 10) {
        nome << '0';
    }
    nome << indice << "_de_(" << teste.xInicial << ',' << teste.yInicial << ")_a_("
         << teste.xFinal << ',' << teste.yFinal << ").ppm";
    return nome.str();
}

void salvarEvidenciaVisual(const std::vector<cg::Ponto>& reta, const CasoDeTeste& teste,
                           const std::string& caminho) {
    const int margemLogica = 4;
    const int escala = 3;
    const int menorX = std::min(teste.xInicial, teste.xFinal) - margemLogica;
    const int maiorX = std::max(teste.xInicial, teste.xFinal) + margemLogica;
    const int menorY = std::min(teste.yInicial, teste.yFinal) - margemLogica;
    const int maiorY = std::max(teste.yInicial, teste.yFinal) + margemLogica;

    cg::ImagemPPM imagem((maiorX - menorX + 1) * escala,
                          (maiorY - menorY + 1) * escala,
                          cg::BRANCO);

    for (int y = menorY; y <= maiorY; ++y) {
        for (int x = menorX; x <= maiorX; ++x) {
            if (x % 10 == 0 || y % 10 == 0) {
                imagem.definirPixel((x - menorX) * escala + escala / 2,
                                    (y - menorY) * escala + escala / 2,
                                    cg::CINZA_CLARO);
            }
        }
    }

    for (const cg::Ponto& ponto : reta) {
        imagem.desenharBloco((ponto.x - menorX) * escala + escala / 2,
                             (ponto.y - menorY) * escala + escala / 2,
                             escala,
                             cg::PRETO);
    }

    imagem.desenharBloco((teste.xInicial - menorX) * escala + escala / 2,
                         (teste.yInicial - menorY) * escala + escala / 2,
                         escala,
                         cg::CINZA_MEDIO);
    imagem.desenharBloco((teste.xFinal - menorX) * escala + escala / 2,
                         (teste.yFinal - menorY) * escala + escala / 2,
                         escala,
                         cg::CINZA_MEDIO);
    imagem.salvar(caminho);
}

bool validarResultado(const std::vector<cg::Ponto>& reta, const CasoDeTeste& teste) {
    if (reta.empty()) {
        return false;
    }

    const cg::Ponto primeiro = reta.front();
    const cg::Ponto ultimo = reta.back();
    if (primeiro.x != teste.xInicial || primeiro.y != teste.yInicial ||
        ultimo.x != teste.xFinal || ultimo.y != teste.yFinal) {
        return false;
    }

    for (std::size_t i = 1; i < reta.size(); ++i) {
        const int passoX = std::abs(reta[i].x - reta[i - 1].x);
        const int passoY = std::abs(reta[i].y - reta[i - 1].y);
        if (passoX > 1 || passoY > 1 || (passoX == 0 && passoY == 0)) {
            return false;
        }
    }
    return true;
}

}  // namespace

int executarExercicio1() {
    const std::vector<CasoDeTeste> testes = {
        {10, 100, 100, 10},
        {100, 10, 10, 100},
        {200, 100, 1, 1},
        {1, 1, 200, 100},
        {200, 100, 200, 10},
        {200, 10, 200, 100},
        {50, 100, 200, 100},
        {200, 100, 50, 100}
    };

    std::filesystem::create_directories("saidas/exercicio1");
    bool todosOsTestesPassaram = true;

    std::cout << "Exercício 1: Bresenham\n";
    for (std::size_t indice = 0; indice < testes.size(); ++indice) {
        const CasoDeTeste& teste = testes[indice];
        const std::vector<cg::Ponto> reta = cg::pontoMedio(
            teste.xInicial, teste.yInicial, teste.xFinal, teste.yFinal);
        const bool passou = validarResultado(reta, teste);
        const std::string caminho = nomeDoArquivo(indice + 1, teste);
        salvarEvidenciaVisual(reta, teste, caminho);

        std::cout << "Teste " << indice + 1 << ": ("
                  << teste.xInicial << ',' << teste.yInicial << ") -> ("
                  << teste.xFinal << ',' << teste.yFinal << ") | pixels: "
                  << reta.size() << " | " << (passou ? "ok" : "falhou") << '\n';
        todosOsTestesPassaram = todosOsTestesPassaram && passou;
    }

    std::cout << (todosOsTestesPassaram
                      ? "8 de 8 testes aprovados.\n"
                      : "Há teste com falha.\n");
    return todosOsTestesPassaram ? 0 : 1;
}
