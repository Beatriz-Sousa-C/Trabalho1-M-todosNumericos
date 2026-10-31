#include "CalculoController.h"

Resultado CalculoController::calcularNewtonRaphson(
    double aproximacaoInicial,
    double precisao,
    double a3,
    double a2) {
    Parametros parametros(aproximacaoInicial, precisao, 0.05, 1e-5, 10000);

    FuncaoPendulo funcoes = FuncaoPendulo(a3,a2);

    NewtonRaphson nr;
    return nr.calcular(funcoes, parametros);
}
