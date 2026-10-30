#include "NewtonDerivadaNumerica.h"
#include "DerivadaNumerica.h"
#include <cmath>
#include "constantes.h"

using namespace std;

Resultado NewtonDerivadaNumerica::calcular(const Funcao& f,const Parametros& p) const {
    Resultado r{};
    double x1 = p.aproximacaoInicial;
    double epsilon = p.precisao;
    double h = p.passoDerivada;
    int maximoIteracoes = p.maximoIteracoes;

    r.raiz = x1;
    r.convergiu = false;
    r.iteracoes = 0;
    r.erro = NAN;

    if (!isfinite(x1) ||!isfinite(epsilon) || !isfinite(h) ||
        maximoIteracoes <= 0 || epsilon <= 0 || h <= 0) {
        r.mensagem = "Parametro invalido identificado!";
        return r;
    }

    r.historico.push_back(x1);
    for(int i =0; i<maximoIteracoes; i++){
        double fx = f.avaliar(x1);
        double dfx = derivadaNumerica(f, x1, h);

        if (!isfinite(fx) || !isfinite(dfx)) {
            r.mensagem = "Valor nao finito durante o calculo. Nao foi possivel completar a analise.";
            return r;
        }

        if (fabs(dfx) < LIMITE_DENOMINADOR) {
            r.mensagem = "Derivada numerica muito pequena, não converge";
            return r;
        }

        double xProx = x1 - (fx / dfx);
        r.historico.push_back(xProx);

        if (!isfinite(xProx)) {
            r.mensagem = "Proxima aproximacao eh nao finita";
            return r;
        }

        r.raiz = xProx;
        r.erro = fabs(xProx-x1);
        r.iteracoes = i+1;

        if (fabs(xProx - x1) < epsilon) {
            r.convergiu = true;
            r.mensagem = "Raiz calculada com sucesso!\n";
            if (fabs(r.raiz) > DESLOCAMENTO_LIMITE) {
                r.mensagem += "ATENCAO: Deslocamento maior que 0,3 cm!";
            }
            return r;
        }

        x1 = xProx;
    }

    r.mensagem = "Limite maximo de iteracoes atingido.";
    return r;
}


string NewtonDerivadaNumerica::nome() const {
    return "Newton com derivada numerica";
}
