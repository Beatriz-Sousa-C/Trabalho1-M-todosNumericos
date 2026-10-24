#include "NewtonHorner.h"
#include "FuncHorner.h"
#include <bits/stdc++.h>
using namespace std;

Resultado NewtonHorner::calcular(const Funcao& funcao,
                                 const Parametros& par) const {
    Resultado r{};
    r.raiz = par.aproximacaoInicial;

    // só funciona com polinômio, porque precisa dos coeficientes
    const auto* poli = dynamic_cast<const FuncHorner*>(&funcao);
    if (!poli) {
        r.mensagem = "Newton com Horner exige uma FuncHorner (polinomio).";
        return r;
    }
    if (par.precisao <= 0 || par.maximoIteracoes <= 0) {
        r.mensagem = "Parametros invalidos: precisao e maximo de iteracoes devem ser > 0.";
        return r;
    }

    double x = par.aproximacaoInicial;
    for (int k = 1; k <= par.maximoIteracoes; k++) {
        double p, dp;
        poli->avaliarComDerivada(x, p, dp); // p e p' numa iteração só

        if (fabs(dp) < 1e-14) { // derivada nula
            r.raiz = x; r.iteracoes = k - 1;
            r.mensagem = "Derivada nula: Newton nao pode continuar.";
            return r;
        }

        double xNovo = x - p / dp;
        r.erro = fabs(xNovo - x);
        x = xNovo;
        r.raiz = x; r.iteracoes = k;

        if (!isfinite(x)) {
            r.mensagem = "Divergiu (valor infinito ou NaN).";
            return r;
        }
        if (r.erro < par.precisao) {
            r.convergiu = true;
            r.mensagem = "Convergiu: |x_{k+1} - x_k| < e.";
            return r;
        }
    }
    r.mensagem = "Atingiu o maximo de iteracoes sem convergir.";
    return r;
}