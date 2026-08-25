#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "bresenham.hpp"
#include "imagem_ppm.hpp"

namespace {

struct Poligono {
    std::string nome;
    std::vector<cg::Ponto> vertices;
};

std::vector<std::vector<cg::Ponto>> rasterizarLados(const Poligono& poligono) {
    std::vector<std::vector<cg::Ponto>> lados;
    if (poligono.vertices.size() < 3) {
        return lados;
    }

    lados.reserve(poligono.vertices.size());
    for (std::size_t i = 0; i < poligono.vertices.size(); ++i) {
        const cg::Ponto& inicio = poligono.vertices[i];
        const cg::Ponto& fim = poligono.vertices[(i + 1) % poligono.vertices.size()];
        lados.push_back(cg::pontoMedio(inicio.x, inicio.y, fim.x, fim.y));
    }
    return lados;
}

bool poligonoEstaFechado(const Poligono& poligono, const std::vector<std::vector<cg::Ponto>>& lados) {
    if (poligono.vertices.size() < 3 || lados.size() != poligono.vertices.size()) {
        return false;
    }

    for (std::size_t i = 0; i < lados.size(); ++i) {
        if (lados[i].empty()) {
            return false;
        }
        const cg::Ponto& inicioEsperado = poligono.vertices[i];
        const cg::Ponto& fimEsperado = poligono.vertices[(i + 1) % poligono.vertices.size()];
        const cg::Ponto& inicioObtido = lados[i].front();
        const cg::Ponto& fimObtido = lados[i].back();
        if (inicioObtido.x != inicioEsperado.x || inicioObtido.y != inicioEsperado.y ||
            fimObtido.x != fimEsperado.x || fimObtido.y != fimEsperado.y) {
            return false;
        }
    }
    return true;
}

void salvarPoligono(const Poligono& poligono, const std::string& caminho) {
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

    const std::vector<std::vector<cg::Ponto>> lados = rasterizarLados(poligono);
    for (const std::vector<cg::Ponto>& lado : lados) {
        for (const cg::Ponto& ponto : lado) {
            imagem.desenharBloco((ponto.x - menorX) * escala + escala / 2,
                                 (ponto.y - menorY) * escala + escala / 2,
                                 5,
                                 cg::PRETO);
        }
    }

    for (const cg::Ponto& vertice : poligono.vertices) {
        imagem.desenharBloco((vertice.x - menorX) * escala + escala / 2,
                             (vertice.y - menorY) * escala + escala / 2,
                             11,
                             cg::VERMELHO);
    }
    imagem.salvar(caminho);
}

void exibirResumo(const Poligono& poligono, const std::string& caminho) {
    const std::vector<std::vector<cg::Ponto>> lados = rasterizarLados(poligono);
    std::cout << poligono.nome << '\n' << "  Vértices: ";
    for (std::size_t i = 0; i < poligono.vertices.size(); ++i) {
        std::cout << '(' << poligono.vertices[i].x << ", " << poligono.vertices[i].y << ')';
        if (i + 1 < poligono.vertices.size()) {
            std::cout << " -> ";
        }
    }
    std::cout << " -> primeiro vértice\n";

    std::size_t pixelsNosLados = 0;
    for (std::size_t i = 0; i < lados.size(); ++i) {
        pixelsNosLados += lados[i].size();
        std::cout << "  Lado " << i + 1 << ": " << lados[i].size() << " pixels\n";
    }
    std::cout << "  Pixels gerados nos lados (com repetição dos vértices): " << pixelsNosLados << '\n'
              << "  Fechamento: " << (poligonoEstaFechado(poligono, lados) ? "APROVADO" : "REPROVADO") << '\n'
              << "  Evidência: " << caminho << "\n\n";
}

}  // namespace

int executarExercicio3() {
    const Poligono hexagonoDoEnunciado{
        "Polígono 1 — hexágono do enunciado (A-B-C-D-E-F)",
        {{2, 3}, {7, 1}, {13, 5}, {13, 11}, {7, 7}, {2, 9}}
    };
    const Poligono pentagonoDeEscolhaLivre{
        "Polígono 2 — pentágono convexo de escolha livre",
        {{3, 2}, {8, 1}, {13, 4}, {11, 10}, {5, 11}}
    };

    std::filesystem::create_directories("saidas/exercicio3");
    salvarPoligono(hexagonoDoEnunciado, "saidas/exercicio3/poligono_enunciado.ppm");
    salvarPoligono(pentagonoDeEscolhaLivre, "saidas/exercicio3/poligono_escolha_livre.ppm");

    std::cout << "=== Exercício 3 — Desenho de polígonos com Bresenham ===\n\n";
    exibirResumo(hexagonoDoEnunciado, "saidas/exercicio3/poligono_enunciado.ppm");
    exibirResumo(pentagonoDeEscolhaLivre, "saidas/exercicio3/poligono_escolha_livre.ppm");
    return 0;
}
