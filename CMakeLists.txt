cmake_minimum_required(VERSION 3.16)

project(TrabalhoMetodos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
        main.exe
        main.cpp
        Views/Interface.cpp
        Models/InterfaceMetodo.cpp
        Models/NewtonRaphson.cpp
        Models/NewtonFL.cpp
        Models/FuncaoPendulo.cpp
        Controllers/CalculoController.cpp
)
