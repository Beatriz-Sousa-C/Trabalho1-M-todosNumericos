#include "Bissecao.h"
#include "constantes.h"
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

    r.raiz = (a + b) / 2;
    r.convergiu = false;
    r.erro = NAN;
    r.iteracoes = 0;

    if (!isfinite(a) || !isfinite(b) || !isfinite(epsilon) ||
        maximoIteracoes <= 0 || epsilon <= 0 || a == b) {
        r.mensagem = "Parametro invalido identificado!";
        return r;
    }

    if (a > b) swap(a, b);

    double fa = funcao.avaliar(a);
    double fb = funcao.avaliar(b);

    if (!isfinite(fa) || !isfinite(fb)) {
        r.mensagem = "Valor nao finito nos extremos do intervalo.";
        return r;
    }

    if (fa == 0 || fb == 0) {
        r.raiz = (fa == 0) ? a : b;
        r.erro = 0;
        r.convergiu = true;
        r.mensagem = "Extremo do intervalo ja e raiz.";
        avisarDeslocamento(r);
        return r;
    }

    if ((fa > 0) == (fb > 0)) {
        r.mensagem = "Nao ha troca de sinal em [a, b]: bissecao nao se aplica.";
        return r;
    }

    if (b - a < epsilon) {
        r.raiz = (a + b) / 2;
        r.erro = (b - a) / 2;
        r.convergiu = true;
        r.mensagem = "Intervalo menor que a precisao.";
        avisarDeslocamento(r);
        return r;
    }

    // k > log2((b - a) / e): numero de iteracoes para que b - a < e
    int necessarias = (int)floor(log2((b - a) / epsilon)) + 1;
    string avisoIteracoes;
    if (maximoIteracoes < necessarias)
        avisoIteracoes = "\nAVISO: maximo de iteracoes (" + to_string(maximoIteracoes) +
                         ") menor que o necessario (" + to_string(necessarias) +
                         "); a precisao nao sera atingida.";
    else if (maximoIteracoes == necessarias)
        avisoIteracoes = "\nMaximo de iteracoes igual ao necessario (" + to_string(necessarias) + ").";
    else
        avisoIteracoes = "\nMaximo de iteracoes (" + to_string(maximoIteracoes) +
                         ") maior que o necessario (" + to_string(necessarias) + ").";

    for (int i = 0; i < maximoIteracoes; i++) {
        double m = (a + b) / 2;
        double fm = funcao.avaliar(m);
        if (!isfinite(fm)) {
            r.mensagem = "Valor nao finito durante o calculo." + avisoIteracoes;
            return r;
        }
        r.historico.push_back(m);

        if (fm == 0) {
            a = m;
            b = m;
        } else if ((fa > 0) == (fm > 0)) {
            a = m;
            fa = fm;
        } else {
            b = m;
            fb = fm;
        }

        // m e um dos extremos do novo intervalo, logo |m - raiz| <= b - a
        r.raiz = m;
        r.erro = b - a;
        r.iteracoes = i + 1;

        if (b - a < epsilon) {
            r.convergiu = true;
            r.mensagem = "Raiz calculada com sucesso!";
            avisarDeslocamento(r);
            r.mensagem += avisoIteracoes;
            return r;
        }
    }

    r.mensagem = "Limite maximo de iteracoes atingido." + avisoIteracoes;
    return r;
}

string Bissecao::nome() const {
    return "Bissecao";
}
