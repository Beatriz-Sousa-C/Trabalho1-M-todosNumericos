cmake_minimum_required(VERSION 3.16)

project(TrabalhoMetodos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
        main.exe
        main.cpp
        Views/Interface.cpp
        Models/FuncHorner.cpp
        Models/NewtonHorner.cpp
        Models/PosicaoFalsa.cpp
        Models/AnaliseG.cpp
        Models/NewtonFL.cpp
        Models/FuncaoPendulo.cpp
        Models/InterfaceMetodo.cpp
        Models/NewtonRaphson.cpp
        Models/Bissecao.cpp
        Models/Secante.cpp
        Models/DerivadaNumerica.cpp
        Models/NewtonDerivadaNumerica.cpp
        Models/EstudoPassoH.cpp
        Views/Quadros.cpp
        Views/Graficos.cpp
)
