#include "CalculoController.h"

Resultado CalculoController::calcularNewtonRaphson(
    double aproximacaoInicial,
    double precisao,
    double a3,
    double a2) {
    Parametros parametros;
    parametros.aproximacaoInicial = aproximacaoInicial;
    parametros.maximoIteracoes = 10000;
    parametros.precisao = precisao;

    FuncaoPendulo funcoes = FuncaoPendulo(a3,a2);

    NewtonRaphson nr;
    return nr.calcular(funcoes, parametros);
}
