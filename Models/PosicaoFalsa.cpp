#include "PosicaoFalsa.h"
#include "constantes.h"
#include <bits/stdc++.h>
using namespace std;

static double escolha(const Funcao& f, double a, double b) {
    if (abs(f.avaliar(a)) < abs(f.avaliar(b))) return a;
    return b;
}

static void avisarLimite(Resultado& r) {
    if (abs(r.raiz) > DESLOCAMENTO_LIMITE)
        r.mensagem += "\nATENCAO: Deslocamento maior que 0,3 cm!";
}

Resultado PosicaoFalsa::calcular(const Funcao& f, const Parametros& par) const {
    Resultado r{};

    double a = par.aproximacaoInicial, b = par.b;
    double erro1 = par.precisao;
    int iterMax = par.maximoIteracoes;

    r.raiz = a;
    r.erro = NAN;

    if (!isfinite(a) || !isfinite(b) || !isfinite(erro1) || erro1 <= 0 || iterMax <= 0) {
        r.mensagem = "Parametros invalidos: precisao e maximo de iteracoes devem ser > 0.";
        return r;
    }
    if (a == b) {
        r.mensagem = "Posicao falsa precisa de um intervalo [a, b] com a != b.";
        return r;
    }
    if (a > b) swap(a, b);

    double fa = f.avaliar(a);
    double fb = f.avaliar(b);

    if (!isfinite(fa) || !isfinite(fb)) {
        r.mensagem = "Valor nao finito nos extremos do intervalo.";
        return r;
    }
    if (fa * fb > 0) {   
        r.mensagem = "Intervalo invalido: f(a) e f(b) tem o mesmo sinal.";
        return r;
    }

    r.erro = abs(b - a);
    if (abs(b - a) < erro1) {
        r.raiz = escolha(f, a, b); r.convergiu = true;
        r.mensagem = "Intervalo inicial ja menor que e.";
        avisarLimite(r);
        return r;
    }
    if (fa == 0) { r.raiz = a; r.erro = 0; r.convergiu = true; r.mensagem = "a ja e raiz (f(a) = 0)."; avisarLimite(r); return r; }
    if (fb == 0) { r.raiz = b; r.erro = 0; r.convergiu = true; r.mensagem = "b ja e raiz (f(b) = 0)."; avisarLimite(r); return r; }

    double xAnterior = NAN;
    for (int i = 1; i <= iterMax; i++) {
        double x  = (a * fb - b * fa) / (fb - fa);
        r.historico.push_back(x);
        double fx = f.avaliar(x);

        if (!isfinite(fx)) {
            r.mensagem = "Valor nao finito durante o calculo.";
            return r;
        }

        if (fa * fx > 0) { a = x; fa = fx; } 
        else             { b = x; fb = fx; }

        r.raiz = x;
        r.iteracoes = i;
        
        r.erro = (i == 1) ? abs(b - a) : abs(x - xAnterior);
        xAnterior = x;

        
        if (fx == 0 || r.erro < erro1) {
            if (fx == 0) r.erro = 0;
            r.convergiu = true;
            r.mensagem = (fx == 0) ? "Convergiu: f(x) = 0." : "Convergiu: |x_k - x_{k-1}| < e.";
            avisarLimite(r);
            return r;
        }
        if (abs(b - a) < erro1) {
            r.raiz = escolha(f, a, b);
            r.erro = abs(b - a);
            r.convergiu = true;
            r.mensagem = "Convergiu: |b - a| < e.";
            avisarLimite(r);
            return r;
        }
    }
    r.mensagem = "Atingiu o maximo de iteracoes sem convergir.";
    return r;
}
