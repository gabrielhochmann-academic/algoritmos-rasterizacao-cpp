cmake_minimum_required(VERSION 3.16)
project(Trabalho1Rasterizacao LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(rasterizacao
    src/main.cpp
    src/bresenham.cpp
    src/imagem_ppm.cpp
    src/exercicio1_ponto_medio.cpp
    src/exercicio2_cohen_sutherland.cpp
    src/exercicio3_poligonos.cpp
    src/exercicio4_scanline.cpp
)

target_include_directories(rasterizacao PRIVATE include)

if(MSVC)
    target_compile_options(rasterizacao PRIVATE /W4 /utf-8)
else()
    target_compile_options(rasterizacao PRIVATE -Wall -Wextra -pedantic)
endif()
