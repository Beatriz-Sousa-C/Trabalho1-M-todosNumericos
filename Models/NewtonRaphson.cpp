#include "NewtonRaphson.h"

Resultado NewtonRaphson::calcular(const Funcao &funcao, const Parametros &parametros) const {
    double f_x = funcao.avaliar(parametros.aproximacaoInicial);
    double f_derivada = funcao.avaliarDerivada(parametros.aproximacaoInicial);

    double x_prox = parametros.aproximacaoInicial - f_x/f_derivada;
    if (std::abs(funcao.avaliar(x_prox)) <= parametros.precisao) {
        Resultado r = Resultado();
        r.raiz = x_prox;
        return r;
    }

    Parametros n_parametros = Parametros();
    n_parametros.precisao = parametros.precisao;
    n_parametros.aproximacaoInicial = x_prox;
    n_parametros.lambda = parametros.lambda;
    n_parametros.passoDerivada = parametros.passoDerivada;
    n_parametros.maximoIteracoes = parametros.maximoIteracoes;

    return calcular(funcao, n_parametros);
}

std::string NewtonRaphson::nome() const {
    return "Metodo de Newton Raphson";
}
