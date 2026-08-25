#include "imagem_ppm.hpp"

#include <fstream>
#include <stdexcept>

namespace cg {

ImagemPPM::ImagemPPM(int largura, int altura, Cor fundo)
    : largura_(largura), altura_(altura), pixels_(static_cast<std::size_t>(largura) * altura, fundo) {
    if (largura <= 0 || altura <= 0) {
        throw std::invalid_argument("A imagem deve possuir largura e altura positivas.");
    }
}

void ImagemPPM::definirPixel(int x, int y, Cor cor) {
    if (x < 0 || x >= largura_ || y < 0 || y >= altura_) {
        return;
    }

    const int yArquivo = altura_ - 1 - y;
    pixels_[static_cast<std::size_t>(yArquivo) * largura_ + x] = cor;
}

void ImagemPPM::desenharBloco(int xCentral, int yCentral, int tamanho, Cor cor) {
    const int deslocamento = tamanho / 2;
    for (int y = yCentral - deslocamento; y <= yCentral + deslocamento; ++y) {
        for (int x = xCentral - deslocamento; x <= xCentral + deslocamento; ++x) {
            definirPixel(x, y, cor);
        }
    }
}

void ImagemPPM::salvar(const std::string& caminho) const {
    std::ofstream arquivo(caminho, std::ios::binary);
    if (!arquivo) {
        throw std::runtime_error("Não foi possível criar a imagem: " + caminho);
    }

    arquivo << "P6\n" << largura_ << ' ' << altura_ << "\n255\n";
    for (const Cor& cor : pixels_) {
        arquivo.put(static_cast<char>(cor.vermelho));
        arquivo.put(static_cast<char>(cor.verde));
        arquivo.put(static_cast<char>(cor.azul));
    }
}

int ImagemPPM::largura() const {
    return largura_;
}

int ImagemPPM::altura() const {
    return altura_;
}

}  // namespace cg
