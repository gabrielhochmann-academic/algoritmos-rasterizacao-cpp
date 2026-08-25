#pragma once

#include <string>
#include <vector>

#include "tipos.hpp"

namespace cg {

class ImagemPPM {
public:
    ImagemPPM(int largura, int altura, Cor fundo = BRANCO);

    void definirPixel(int x, int y, Cor cor);
    void desenharBloco(int xCentral, int yCentral, int tamanho, Cor cor);
    void salvar(const std::string& caminho) const;

    int largura() const;
    int altura() const;

private:
    int largura_;
    int altura_;
    std::vector<Cor> pixels_;
};

}  // namespace cg
