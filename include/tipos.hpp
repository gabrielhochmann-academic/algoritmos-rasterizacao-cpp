#pragma once

#include <cstdint>

namespace cg {

struct Ponto {
    int x;
    int y;
};

struct Cor {
    std::uint8_t vermelho;
    std::uint8_t verde;
    std::uint8_t azul;
};

constexpr Cor BRANCO{255, 255, 255};
constexpr Cor PRETO{0, 0, 0};
constexpr Cor CINZA_CLARO{210, 210, 210};
constexpr Cor CINZA_MEDIO{145, 145, 145};
constexpr Cor VERMELHO{210, 30, 30};
constexpr Cor VERDE{60, 180, 75};
constexpr Cor AZUL{0, 130, 200};
constexpr Cor LARANJA{245, 130, 48};
constexpr Cor ROXO{145, 30, 180};

}  // namespace cg
