#include <algorithm>
#include <cmath>
#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "bresenham.hpp"
#include "imagem_ppm.hpp"
#include "tipos.hpp"

namespace {

using Poligono = std::vector<cg::Ponto>;
using AcaoPintar = std::function<void(int, int, int)>;

struct Aresta {
    int yMax;
    double x;
    double invM;

    bool operator<(const Aresta& outra) const {
        return x < outra.x;
    }
};

class ScanlineRenderer {
public:
    static void preencherPoligono(const Poligono& poligono, const AcaoPintar& pintar) {
        if (poligono.size() < 3) {
            return;
        }

        int yMinGlobal = poligono[0].y;
        int yMaxGlobal = poligono[0].y;
        for (const cg::Ponto& ponto : poligono) {
            yMinGlobal = std::min(yMinGlobal, ponto.y);
            yMaxGlobal = std::max(yMaxGlobal, ponto.y);
        }

        std::vector<std::vector<Aresta>> tabelaDeArestas(
            static_cast<std::size_t>(yMaxGlobal - yMinGlobal + 1));

        for (std::size_t i = 0; i < poligono.size(); ++i) {
            const cg::Ponto p1 = poligono[i];
            const cg::Ponto p2 = poligono[(i + 1) % poligono.size()];
            if (p1.y == p2.y) {
                continue;
            }

            const cg::Ponto& pontoInferior = (p1.y < p2.y) ? p1 : p2;
            const cg::Ponto& pontoSuperior = (p1.y < p2.y) ? p2 : p1;
            const double invM = static_cast<double>(p2.x - p1.x) /
                                static_cast<double>(p2.y - p1.y);

            tabelaDeArestas[static_cast<std::size_t>(pontoInferior.y - yMinGlobal)].push_back(
                {pontoSuperior.y, static_cast<double>(pontoInferior.x), invM});
        }

        std::vector<Aresta> tabelaDeArestasAtivas;
        for (int y = yMinGlobal; y <= yMaxGlobal; ++y) {
            const std::size_t indice = static_cast<std::size_t>(y - yMinGlobal);
            for (const Aresta& aresta : tabelaDeArestas[indice]) {
                tabelaDeArestasAtivas.push_back(aresta);
            }

            std::sort(tabelaDeArestasAtivas.begin(), tabelaDeArestasAtivas.end());
            tabelaDeArestasAtivas.erase(
                std::remove_if(tabelaDeArestasAtivas.begin(), tabelaDeArestasAtivas.end(),
                               [y](const Aresta& aresta) { return aresta.yMax == y; }),
                tabelaDeArestasAtivas.end());

            for (std::size_t i = 0; i + 1 < tabelaDeArestasAtivas.size(); i += 2) {
                const int xInicial = static_cast<int>(std::lround(tabelaDeArestasAtivas[i].x));
                const int xFinal = static_cast<int>(std::lround(tabelaDeArestasAtivas[i + 1].x));
                pintar(xInicial, xFinal, y);
            }

            for (Aresta& aresta : tabelaDeArestasAtivas) {
                aresta.x += aresta.invM;
            }
        }
    }
};

class GraficosConsole {
public:
    static constexpr int LARGURA = 16;
    static constexpr int ALTURA = 13;

    static void limpar() {
        grade_ = std::vector<std::vector<char>>(ALTURA, std::vector<char>(LARGURA, '.'));
    }

    static void pintarNaGrade(int xInicial, int xFinal, int y) {
        if (y < 0 || y >= ALTURA) {
            return;
        }
        for (int x = xInicial; x <= xFinal; ++x) {
            if (x >= 0 && x < LARGURA) {
                grade_[ALTURA - 1 - y][x] = '#';
            }
        }
    }

    static void imprimirDesenho() {
        for (const auto& linha : grade_) {
            for (const char caractere : linha) {
                std::cout << caractere;
            }
            std::cout << '\n';
        }
    }

    static void imprimirLog(int xInicial, int xFinal, int y) {
        std::cout << "  y=" << y << ": x=" << xInicial << ".." << xFinal << '\n';
    }

private:
    static std::vector<std::vector<char>> grade_;
};

std::vector<std::vector<char>> GraficosConsole::grade_;

// Evidência visual do preenchimento Scanline.
void salvarPreenchimentoEmImagem(const Poligono& poligono, const std::string& caminho) {
    constexpr int menorX = 0;
    constexpr int maiorX = 16;
    constexpr int menorY = 0;
    constexpr int maiorY = 13;
    constexpr int escala = 28;

    cg::ImagemPPM imagem((maiorX - menorX + 1) * escala,
                          (maiorY - menorY + 1) * escala,
                          cg::BRANCO);

    for (int y = menorY; y <= maiorY; ++y) {
        for (int x = menorX; x <= maiorX; ++x) {
            const int imagemX = (x - menorX) * escala + escala / 2;
            const int imagemY = (y - menorY) * escala + escala / 2;
            if (x == 0 || y == 0) {
                imagem.desenharBloco(imagemX, imagemY, 3, cg::CINZA_MEDIO);
            } else if (x % 2 == 0 || y % 2 == 0) {
                imagem.definirPixel(imagemX, imagemY, cg::CINZA_CLARO);
            }
        }
    }

    ScanlineRenderer::preencherPoligono(
        poligono,
        [&imagem](int xInicial, int xFinal, int y) {
            for (int x = xInicial; x <= xFinal; ++x) {
                imagem.desenharBloco(x * escala + escala / 2,
                                     y * escala + escala / 2,
                                     escala - 2,
                                     cg::AZUL);
            }
        });

    for (std::size_t i = 0; i < poligono.size(); ++i) {
        const cg::Ponto& inicio = poligono[i];
        const cg::Ponto& fim = poligono[(i + 1) % poligono.size()];
        const std::vector<cg::Ponto> lado = cg::pontoMedio(inicio.x, inicio.y, fim.x, fim.y);
        for (const cg::Ponto& pixel : lado) {
            imagem.desenharBloco(pixel.x * escala + escala / 2,
                                 pixel.y * escala + escala / 2,
                                 9,
                                 cg::PRETO);
        }
    }

    for (const cg::Ponto& vertice : poligono) {
        imagem.desenharBloco(vertice.x * escala + escala / 2,
                             vertice.y * escala + escala / 2,
                             11,
                             cg::VERMELHO);
    }
    imagem.salvar(caminho);
}

}  // namespace

int executarExercicio4() {
    const Poligono hexagono = {
        {2, 3}, {7, 1}, {13, 5}, {13, 11}, {7, 7}, {2, 9}
    };
    const Poligono quadrado = {
        {3, 2}, {10, 2}, {10, 9}, {3, 9}
    };

    std::filesystem::create_directories("saidas/exercicio4");
    salvarPreenchimentoEmImagem(hexagono, "saidas/exercicio4/hexagono_preenchido.ppm");
    salvarPreenchimentoEmImagem(quadrado, "saidas/exercicio4/quadrado_preenchido.ppm");

    std::cout << "Exercício 4: Scanline\n";
    std::cout << "Hexágono do enunciado\n";
    ScanlineRenderer::preencherPoligono(hexagono, GraficosConsole::imprimirLog);
    GraficosConsole::limpar();
    ScanlineRenderer::preencherPoligono(hexagono, GraficosConsole::pintarNaGrade);
    std::cout << "Grade:\n";
    GraficosConsole::imprimirDesenho();

    std::cout << "Quadrado de escolha livre\n";
    ScanlineRenderer::preencherPoligono(quadrado, GraficosConsole::imprimirLog);
    GraficosConsole::limpar();
    ScanlineRenderer::preencherPoligono(quadrado, GraficosConsole::pintarNaGrade);
    std::cout << "Grade:\n";
    GraficosConsole::imprimirDesenho();
    return 0;
}
