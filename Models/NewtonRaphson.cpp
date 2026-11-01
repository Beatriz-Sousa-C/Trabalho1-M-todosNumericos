#include "NewtonRaphson.h"
#include "constantes.h"

Resultado NewtonRaphson::calcular(const Funcao &funcao, const Parametros &parametros) const {
    double f_x;// vai guardar o valor de f(x)
    double f_derivada;// vai guardar o valor de f'(x)
    Resultado r = Resultado(); //Classe resultado
    //Definindo o x inical
    double x = parametros.aproximacaoInicial;
    double x_prox;
    int iteracoes = 0;
    r.raiz = x;
    r.erro = NAN;
    r.convergiu = false;

    if (!std::isfinite(x) || !std::isfinite(parametros.precisao) ||
        parametros.precisao <= 0 || parametros.maximoIteracoes <= 0) {
        r.mensagem = "Parametro invalido identificado!";
        return r;
    }

    r.historico.push_back(x);
    while(iteracoes < parametros.maximoIteracoes) {
        f_x = funcao.avaliar(x);
        f_derivada = funcao.avaliarDerivada(x);
        if (!std::isfinite(f_x) || !std::isfinite(f_derivada)) {
            r.mensagem = "Valor nao finito durante o calculo.";
            return r;
        }
        if (std::abs(f_derivada) <= LIMITE_DENOMINADOR) {
            r.mensagem = "Derivada muito proxima de zero.";
            return r;
        }

        x_prox = x - f_x/f_derivada;
        if (!std::isfinite(x_prox)) {
            r.mensagem = "Proxima aproximacao eh nao finita.";
            return r;
        }
        r.historico.push_back(x_prox);

        ++iteracoes;
        r.raiz = x_prox;
        r.iteracoes = iteracoes;
        r.erro = std::abs(x_prox - x);
        if (r.erro < parametros.precisao) {
            r.convergiu = true;
            r.mensagem = "A raiz foi encontrada com sucesso.";
            avisarDeslocamento(r);
            return r;
        }
        x = x_prox;
    }
    r.mensagem = "O metodo excedeu o limite de iteracoes.";
    return r;
}

std::string NewtonRaphson::nome() const {
    return "Newton original";
}
