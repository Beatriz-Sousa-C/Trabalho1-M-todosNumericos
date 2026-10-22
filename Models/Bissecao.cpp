#include "Bissecao.h"
#include <cmath>
#include <string>
#include <utility>

using namespace std;

Resultado Bissecao::calcular(const Funcao& funcao, const Parametros& parametros) const {
    Resultado r{};

    double a = parametros.aproximacaoInicial;
    double b = parametros.b;
    double epsilon = parametros.precisao;
    int maximoIteracoes = parametros.maximoIteracoes;

    r.raiz = (a+b)/2;
    r.convergiu = false;
    r.erro = NAN;
    r.iteracoes =0;





    if (!isfinite(a) || !isfinite(b) || !isfinite(epsilon) ||
        maximoIteracoes <= 0 || epsilon <= 0 || a == b) {
        resultado.mensagem = "Parametro invalido identificado!";
        return r;
    }

    if (a > b) swap(a, b);

    double f(a) = funcao.avaliar(a);
    double f(b) = funcao.avaliar(b);

    if (!isfinite(f(a)) || !isfinite(f(b))) {
        resultado.mensagem = "Valor nao finito nos extremos do intervalo.";
        return r;
    }

        if ((f(a) > 0) == (f(b) > 0) && f(a) != 0 && f(b) != 0) {
        r.mensagem = "Nao ha troca de sinal em [a, b]: bissecao nao se aplica.";
        return r;
        }

        if (b - a < epsilon) {
        r.raiz = (a + b) / 2;
        r.erro = b - a;
        r.convergiu = true;
        r.mensagem = "Intervalo menor que a precisao.";
        return r;
    }

        int necessarias = (int)floor(log2((b - a) / epsilon)) + 1;
        string avisoIteracoes;

        if (maximoIteracoes < necessarias)
        avisoIteracoes = "\nAVISO: maximo de iteracoes (" to_string(maximoIteracoes) + ") menor que o necessario (" + to_string(necessarias) + "); a precisao nao sera atingida.";
        else if (maximoIteracoes == necessarias)
        avisoIteracoes = "\nMaximo de iteracoes igual ao necessario (" + to_string(necessarias) + ").";
        else
        avisoIteracoes = "\nMaximo de iteracoes (" + to_string(maximoIteracoes) + ") maior que o necessario (" + to_string(necessarias) + ").";


        for (int i = 0; i < maximoIteracoes; i++) {
        double m = (a + b) / 2;
        double f(m) = funcao.avaliar(m);
        if (!isfinite(f(m))) {
            resultado.mensagem = "Valor nao finito durante o calculo." + avisoIteracoes;
            return resultado;
        }

        if(f(m) == 0){
            a=m;
            b=m;
        }
        else if((fa > 0) == (fm > 0)){
            a = m;
            f(a) = f(m);
        }
        else{

            b=m;
            f(b) = f(m);
        }


        r.raiz = (a + b) / 2;
        r.erro = b - a;
        r.iteracoes = i + 1;

        if (b - a < epsilon) {
            r.convergiu = true;
            r.mensagem = "Raiz calculada com sucesso!";
            if (fabs(r.raiz) > 0.3) {
                r.mensagem += "\nATENCAO: Deslocamento maior que 0,3 cm!";
            }
            r.mensagem += avisoIteracoes;
            return r;
        }

        r.mensagem = "Limite maximo de iteracoes atingido." + avisoIteracoes;
        return r;
    }
    




