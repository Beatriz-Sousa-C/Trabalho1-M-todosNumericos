#include "NewtonRaphson.h"

Resultado NewtonRaphson::calcular(const Funcao &funcao, const Parametros &parametros) const {
    double f_x;// vai guardar o valor de f(x)
    double f_derivada;// vai guardar o valor de f'(x)
    Resultado r = Resultado(); //Classe resultado
    //Definindo o x inical
    double x = parametros.aproximacaoInicial;
    double x_prox;
    int iteracoes = 0;
    while(iteracoes < parametros.maximoIteracoes) {
        f_x = funcao.avaliar(x);
        f_derivada = funcao.avaliarDerivada(x);
        if (std::abs(f_derivada) <= 1e-12) {
            r.raiz = x;
            r.iteracoes = iteracoes;
            r.convergiu = false;
            r.mensagem = "Derivada muito proxima de zero.";
            return r;
        }

        x_prox = x - parametros.lambda*f_x/f_derivada;

        ++iteracoes;
        if (std::abs(x_prox - x) < parametros.precisao) {
            r.iteracoes = iteracoes;
            r.convergiu = true;
            r.erro = std::abs(x-x_prox);
            r.mensagem = "A raiz foi encontrada com sucesso.";
            r.raiz = x_prox;
            return r;
        }
        x = x_prox;
    }
    r.iteracoes = iteracoes;
    r.convergiu = false;
    r.mensagem = "O método excedeu o limite de iterações.";
    r.raiz = x;


    return r;
}

std::string NewtonRaphson::nome() const {
    return "Metodo de Newton Raphson";
}
