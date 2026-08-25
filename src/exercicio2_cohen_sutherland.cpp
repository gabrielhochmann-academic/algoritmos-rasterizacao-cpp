#include <array>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "bresenham.hpp"
#include "imagem_ppm.hpp"

namespace {

constexpr int INSIDE = 0;
constexpr int LEFT = 1;
constexpr int RIGHT = 2;
constexpr int BOTTOM = 4;
constexpr int TOP = 8;

struct ClipRect {
    double xmin;
    double ymin;
    double xmax;
    double ymax;
};

struct Segmento {
    std::string nome;
    double x0;
    double y0;
    double x1;
    double y1;
};

void desenharReta(cg::ImagemPPM& imagem, int x0, int y0, int x1, int y1, cg::Cor cor) {
    const std::vector<cg::Ponto> pixels = cg::pontoMedio(x0, y0, x1, y1);
    for (const cg::Ponto& pixel : pixels) {
        imagem.definirPixel(pixel.x, pixel.y, cor);
    }
}

void desenharRetangulo(cg::ImagemPPM& imagem, const ClipRect& recorte, cg::Cor cor) {
    desenharReta(imagem, static_cast<int>(recorte.xmin), static_cast<int>(recorte.ymin),
                 static_cast<int>(recorte.xmax), static_cast<int>(recorte.ymin), cor);
    desenharReta(imagem, static_cast<int>(recorte.xmax), static_cast<int>(recorte.ymin),
                 static_cast<int>(recorte.xmax), static_cast<int>(recorte.ymax), cor);
    desenharReta(imagem, static_cast<int>(recorte.xmax), static_cast<int>(recorte.ymax),
                 static_cast<int>(recorte.xmin), static_cast<int>(recorte.ymax), cor);
    desenharReta(imagem, static_cast<int>(recorte.xmin), static_cast<int>(recorte.ymax),
                 static_cast<int>(recorte.xmin), static_cast<int>(recorte.ymin), cor);
}

int outCode(double x, double y, const ClipRect& recorte) {
    int code = INSIDE;
    if (x < recorte.xmin) {
        code |= LEFT;
    } else if (x > recorte.xmax) {
        code |= RIGHT;
    }
    if (y < recorte.ymin) {
        code |= BOTTOM;
    } else if (y > recorte.ymax) {
        code |= TOP;
    }
    return code;
}

bool cohenSutherland(double& x0, double& y0, double& x1, double& y1, const ClipRect& recorte) {
    int code0 = outCode(x0, y0, recorte);
    int code1 = outCode(x1, y1, recorte);
    bool accept = false;

    while (true) {
        if ((code0 | code1) == 0) {
            accept = true;
            break;
        }
        if (code0 & code1) {
            break;
        }

        double x = 0.0;
        double y = 0.0;
        const int codeOut = code0 ? code0 : code1;

        if (codeOut & TOP) {
            x = x0 + (x1 - x0) * (recorte.ymax - y0) / (y1 - y0);
            y = recorte.ymax;
        } else if (codeOut & BOTTOM) {
            x = x0 + (x1 - x0) * (recorte.ymin - y0) / (y1 - y0);
            y = recorte.ymin;
        } else if (codeOut & RIGHT) {
            y = y0 + (y1 - y0) * (recorte.xmax - x0) / (x1 - x0);
            x = recorte.xmax;
        } else if (codeOut & LEFT) {
            y = y0 + (y1 - y0) * (recorte.xmin - x0) / (x1 - x0);
            x = recorte.xmin;
        }

        if (codeOut == code0) {
            x0 = x;
            y0 = y;
            code0 = outCode(x0, y0, recorte);
        } else {
            x1 = x;
            y1 = y;
            code1 = outCode(x1, y1, recorte);
        }
    }
    return accept;
}

bool pontoDentroOuNaBorda(double x, double y, const ClipRect& recorte) {
    constexpr double tolerancia = 1e-9;
    return x >= recorte.xmin - tolerancia && x <= recorte.xmax + tolerancia &&
           y >= recorte.ymin - tolerancia && y <= recorte.ymax + tolerancia;
}

void desenharSegmentosOriginais(cg::ImagemPPM& imagem, const std::vector<Segmento>& segmentos) {
    for (const Segmento& segmento : segmentos) {
        desenharReta(imagem,
                     static_cast<int>(std::lround(segmento.x0)),
                     static_cast<int>(std::lround(segmento.y0)),
                     static_cast<int>(std::lround(segmento.x1)),
                     static_cast<int>(std::lround(segmento.y1)),
                     cg::CINZA_CLARO);
    }
}

}  // namespace

int executarExercicio2() {
    const ClipRect clip{200, 150, 400, 350};
    const std::vector<Segmento> segmentos = {
        {"EF", 40, 120, 150, 420},
        {"AB", 260, 200, 300, 260},
        {"CD", 320, 220, 380, 430},
        {"GH", 230, 100, 420, 320},
        {"IJ", 350, 60, 430, 250}
    };
    const std::array<cg::Cor, 5> cores = {cg::VERMELHO, cg::VERDE, cg::AZUL, cg::LARANJA, cg::ROXO};

    std::filesystem::create_directories("saidas/exercicio2");
    cg::ImagemPPM entrada(650, 500, cg::BRANCO);
    cg::ImagemPPM resultado(650, 500, cg::BRANCO);
    cg::ImagemPPM comparacao(650, 500, cg::BRANCO);
    desenharRetangulo(entrada, clip, cg::PRETO);
    desenharRetangulo(resultado, clip, cg::PRETO);
    desenharRetangulo(comparacao, clip, cg::PRETO);
    desenharSegmentosOriginais(entrada, segmentos);
    desenharSegmentosOriginais(comparacao, segmentos);

    bool todosOsTestesPassaram = true;
    std::cout << "=== Exercício 2 — Cohen–Sutherland ===\n"
              << "Janela de recorte: [" << clip.xmin << ", " << clip.xmax << "] x ["
              << clip.ymin << ", " << clip.ymax << "]\n\n";

    for (std::size_t i = 0; i < segmentos.size(); ++i) {
        const Segmento& segmento = segmentos[i];
        double cx0 = segmento.x0;
        double cy0 = segmento.y0;
        double cx1 = segmento.x1;
        double cy1 = segmento.y1;
        const bool aceita = cohenSutherland(cx0, cy0, cx1, cy1, clip);

        std::cout << "Segmento " << segmento.nome << " (" << segmento.x0 << ',' << segmento.y0
                  << ") - (" << segmento.x1 << ',' << segmento.y1 << ")\n";
        if (!aceita) {
            std::cout << "  Resultado: REJEITADO (totalmente fora da janela)\n\n";
            continue;
        }

        const bool dentroDaJanela = pontoDentroOuNaBorda(cx0, cy0, clip) &&
                                      pontoDentroOuNaBorda(cx1, cy1, clip);
        todosOsTestesPassaram = todosOsTestesPassaram && dentroDaJanela;
        std::cout << "  Parte visível: (" << cx0 << ',' << cy0 << ") - ("
                  << cx1 << ',' << cy1 << ")\n"
                  << "  Verificação dos extremos: " << (dentroDaJanela ? "APROVADO" : "REPROVADO")
                  << "\n\n";

        desenharReta(resultado,
                     static_cast<int>(std::lround(cx0)), static_cast<int>(std::lround(cy0)),
                     static_cast<int>(std::lround(cx1)), static_cast<int>(std::lround(cy1)),
                     cores[i]);
        desenharReta(comparacao,
                     static_cast<int>(std::lround(cx0)), static_cast<int>(std::lround(cy0)),
                     static_cast<int>(std::lround(cx1)), static_cast<int>(std::lround(cy1)),
                     cores[i]);
    }

    entrada.salvar("saidas/exercicio2/entrada_sem_recorte.ppm");
    resultado.salvar("saidas/exercicio2/resultado_recortado.ppm");
    comparacao.salvar("saidas/exercicio2/comparacao_entrada_e_resultado.ppm");

    std::cout << "Evidências geradas:\n"
              << "  - saidas/exercicio2/entrada_sem_recorte.ppm\n"
              << "  - saidas/exercicio2/resultado_recortado.ppm\n"
              << "  - saidas/exercicio2/comparacao_entrada_e_resultado.ppm\n"
              << "Resultado final: " << (todosOsTestesPassaram ? "APROVADO" : "REPROVADO") << '\n';
    return todosOsTestesPassaram ? 0 : 1;
}
