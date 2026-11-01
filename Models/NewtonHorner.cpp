#include "NewtonHorner.h"
#include "FuncHorner.h"
#include "constantes.h"
#include <cmath>
using namespace std;

Resultado NewtonHorner::calcular(const Funcao& funcao,
                                 const Parametros& par) const {
    Resultado r{};
    r.raiz = par.aproximacaoInicial;
    r.erro = NAN;

    // só funciona com polinômio, porque precisa dos coeficientes
    const auto* poli = dynamic_cast<const FuncHorner*>(&funcao);
    if (!poli) {
        r.mensagem = "Newton com Horner exige uma FuncHorner (polinomio).";
        return r;
    }
    if (!isfinite(par.aproximacaoInicial) || !isfinite(par.precisao) ||
        par.precisao <= 0 || par.maximoIteracoes <= 0) {
        r.mensagem = "Parametro invalido identificado!";
        return r;
    }

    double x = par.aproximacaoInicial;
    r.historico.push_back(x);
    for (int k = 1; k <= par.maximoIteracoes; k++) {
        double p, dp;
        poli->avaliarComDerivada(x, p, dp); // p e p' numa iteração só

        if (!isfinite(p) || !isfinite(dp)) {
            r.mensagem = "Valor nao finito durante o calculo.";
            return r;
        }
        if (fabs(dp) <= LIMITE_DENOMINADOR) { // derivada nula
            r.mensagem = "Derivada muito proxima de zero.";
            return r;
        }

        double xNovo = x - p / dp;
        if (!isfinite(xNovo)) {
            r.mensagem = "Proxima aproximacao eh nao finita.";
            return r;
        }
        r.historico.push_back(xNovo);
        r.erro = fabs(xNovo - x);
        x = xNovo;
        r.raiz = x; r.iteracoes = k;

        if (r.erro < par.precisao) {
            r.convergiu = true;
            r.mensagem = "Convergiu: |x_{k+1} - x_k| < e.";
            avisarDeslocamento(r);
            return r;
        }
    }
    r.mensagem = "Atingiu o maximo de iteracoes sem convergir.";
    return r;
}
