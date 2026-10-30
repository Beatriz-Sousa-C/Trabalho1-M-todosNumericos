#include "CalculoController.h"

Resultado CalculoController::calcularNewtonRaphson() {
    Parametros parametros;
    parametros.aproximacaoInicial = 1;
    parametros.maximoIteracoes = 10000;
    parametros.precisao = 0.0001;

    FuncaoPendulo funcoes = FuncaoPendulo(1,1);

    NewtonRaphson nr;
    return nr.calcular(funcoes, parametros);
}
