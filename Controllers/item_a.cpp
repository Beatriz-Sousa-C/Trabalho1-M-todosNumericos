#include "item_a.h"
//testando o item a
double FuncaoRaiz::avaliar(double x) const {
    return 25 - x*x;
}

double FuncaoRaiz::avaliarDerivada(double x_raiz) const {
    return -2*x_raiz;
}

double raiz_quadrada(double x) {
    NewtonRaphson n = NewtonRaphson();
    FuncaoRaiz f =  FuncaoRaiz();
    Parametros p = Parametros();
    p.aproximacaoInicial = 1.0;
    p.precisao = 0.0001;
    p.lambda = 1.0;
    p.passoDerivada = 0.00001;
    p.maximoIteracoes = 10000;
    return n.calcular(f,p).raiz;
}