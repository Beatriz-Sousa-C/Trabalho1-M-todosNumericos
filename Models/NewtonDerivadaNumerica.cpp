#include "NewtonDerivadaNumerica.h"
#include "DerivadaNumerica.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include "constantes.h"

using namespace std;

Resultado NewtonDerivadaNumerica::calcular(const Funcao& f, const Parametros& p) const {
    Resultado r{};
    double x = p.aproximacaoInicial;
    double epsilon = p.precisao;
    double h = p.passoDerivada;
    int maximoIteracoes = p.maximoIteracoes;

    r.raiz = x;
    r.convergiu = false;
    r.iteracoes = 0;
    r.erro = NAN;

    if (!isfinite(x) || !isfinite(epsilon) || !isfinite(h) ||
        maximoIteracoes <= 0 || epsilon <= 0 || h <= 0) {
        r.mensagem = "Parametro invalido identificado!";
        return r;
    }

    r.historico.push_back(x);
    for (int i = 0; i < maximoIteracoes; i++) {
        double fx = f.avaliar(x);
        double dfx = derivadaNumerica(f, x, h);

        if (!isfinite(fx) || !isfinite(dfx)) {
            r.mensagem = "Valor nao finito durante o calculo. Nao foi possivel completar a analise.";
            return r;
        }

        // Erro de arredondamento da diferenca central ~ eps_maquina*|f|/h: abaixo disso
        // a derivada calculada e so ruido, e o passo de Newton nao tem significado.
        double ruido = 100 * numeric_limits<double>::epsilon() * max(1.0, fabs(fx)) /
                       (h * max(1.0, fabs(x)));
        if (fabs(dfx) < max(LIMITE_DENOMINADOR, ruido)) {
            r.mensagem = "Derivada numerica proxima de zero (ponto critico): metodo falhou.";
            return r;
        }

        double xProx = x - (fx / dfx);
        if (!isfinite(xProx)) {
            r.mensagem = "Proxima aproximacao eh nao finita.";
            return r;
        }
        r.historico.push_back(xProx);

        r.raiz = xProx;
        r.erro = fabs(xProx - x);
        r.iteracoes = i + 1;

        if (r.erro < epsilon) {
            r.convergiu = true;
            r.mensagem = "Raiz calculada com sucesso!";
            avisarDeslocamento(r);
            return r;
        }

        x = xProx;
    }

    r.mensagem = "Limite maximo de iteracoes atingido.";
    return r;
}


string NewtonDerivadaNumerica::nome() const {
    return "Newton com derivada numerica";
}
