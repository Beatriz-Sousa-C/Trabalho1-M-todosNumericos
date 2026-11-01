#include <cmath>
#include "InterfaceMetodo.h"
#include "Secante.h"
#include "constantes.h"

using namespace std;

static void avisarLimite(Resultado& r) {
    if (fabs(r.raiz) > DESLOCAMENTO_LIMITE)
        r.mensagem += "\nATENCAO: Deslocamento maior que 0,3 cm!";
}

Resultado Secante::calcular(const Funcao& funcao, const Parametros& p) const {
    Resultado r{};

    double x0 = p.aproximacaoInicial;
    double x1 = p.b;
    double epsilon = p.precisao;
    int maximoIteracoes = p.maximoIteracoes;

    r.raiz = x0;
    r.iteracoes = 0;
    r.convergiu = false;
    r.erro = NAN;

    if (!isfinite(x0) || !isfinite(x1) || !isfinite(epsilon) ||
        maximoIteracoes <= 0 || epsilon <= 0) {
        r.mensagem = "Parametro invalido identificado!";
        return r;
    }

    if (x0 == x1) {
        r.mensagem = "Secante precisa de dois pontos distintos.";
        return r;
    }

    double f0 = funcao.avaliar(x0);
    double f1 = funcao.avaliar(x1);
    r.historico.push_back(x0);
    r.historico.push_back(x1);

    for (int i = 0; i < maximoIteracoes; i++) {
        if (!isfinite(f0) || !isfinite(f1)) {
            r.raiz = x1;
            r.iteracoes = i;
            r.mensagem = "Valor nao finito durante o calculo.";
            return r;
        }

        if (f1 == 0) {
            r.raiz = x1;
            r.erro = 0;
            r.iteracoes = i;
            r.convergiu = true;
            r.mensagem = "Raiz calculada com sucesso! (f(x) = 0)";
            avisarLimite(r);
            return r;
        }

        double denom = f1 - f0;
        if (fabs(denom) < LIMITE_DENOMINADOR) {
            r.raiz = x1;
            r.iteracoes = i;
            r.mensagem = "Denominador f(x1) - f(x0) muito pequeno: secante quase horizontal.";
            return r;
        }

        // x1 - f1*(x1 - x0)/denom em vez de (x0*f1 - x1*f0)/denom: menos cancelamento perto da raiz
        double x2 = x1 - f1 * (x1 - x0) / denom;
        if (!isfinite(x2)) {
            r.raiz = x1;
            r.iteracoes = i;
            r.mensagem = "Proxima aproximacao eh nao finita.";
            return r;
        }
        r.historico.push_back(x2);

        r.raiz = x2;
        r.erro = fabs(x2 - x1);
        r.iteracoes = i + 1;

        if (r.erro < epsilon) {
            r.convergiu = true;
            r.mensagem = "Raiz calculada com sucesso!";
            avisarLimite(r);
            return r;
        }

        x0 = x1;
        f0 = f1;
        x1 = x2;
        f1 = funcao.avaliar(x1);
    }

    r.mensagem = "Limite maximo de iteracoes atingido.";
    return r;
}

string Secante::nome() const {
    return "Secante";
}
