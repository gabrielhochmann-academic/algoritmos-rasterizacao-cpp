#include "bresenham.hpp"

#include <cstdlib>

namespace cg {

std::vector<Ponto> pontoMedio(int xInicial, int yInicial, int xFinal, int yFinal) {
    std::vector<Ponto> pixels;

    int x = xInicial;
    int y = yInicial;
    const int deltaX = std::abs(xFinal - xInicial);
    const int deltaY = std::abs(yFinal - yInicial);
    const int passoX = (xInicial < xFinal) ? 1 : ((xInicial > xFinal) ? -1 : 0);
    const int passoY = (yInicial < yFinal) ? 1 : ((yInicial > yFinal) ? -1 : 0);
    int erro = deltaX - deltaY;

    while (true) {
        pixels.push_back({x, y});

        if (x == xFinal && y == yFinal) {
            break;
        }

        const int erroDuplicado = 2 * erro;
        if (erroDuplicado > -deltaY) {
            erro -= deltaY;
            x += passoX;
        }
        if (erroDuplicado < deltaX) {
            erro += deltaX;
            y += passoY;
        }
    }

    return pixels;
}

}  // namespace cg
