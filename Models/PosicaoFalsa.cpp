#include "PosicaoFalsa.h"
#include <bits/stdc++.h>
using namespace std;

// Entre a e b, devolve o que tem |f| menor (mais perto da raiz).
static double escolha(const Funcao& f, double a, double b) {
    if (abs(f.avaliar(a)) < abs(f.avaliar(b))) return a;
    return b;
}

Resultado PosicaoFalsa::calcular(const Funcao& f, const Parametros& par) const {
    Resultado r{};

    double a = a_, b = b_;
    double erro1 = par.precisao;   // tolerância no tamanho do intervalo
    double erro2 = par.precisao;   // tolerância em |f(x)|
    int iterMax = par.maximoIteracoes;

    if (erro1 <= 0 || iterMax <= 0) {
        r.mensagem = "Parametros invalidos: precisao e maximo de iteracoes devem ser > 0.";
        return r;
    }

    double fa = f.avaliar(a);
    double fb = f.avaliar(b);

    if (fa * fb > 0) {   // intervalo inválido: f não troca de sinal
        r.mensagem = "Intervalo invalido: f(a) e f(b) tem o mesmo sinal.";
        return r;
    }

    // casos em que já dá para responder sem iterar
    r.erro = abs(b - a);
    if (abs(b - a) < erro1) {
        r.raiz = escolha(f, a, b); r.convergiu = true;
        r.mensagem = "Intervalo inicial ja menor que e.";
        return r;
    }
    if (abs(fa) < erro2) { r.raiz = a; r.convergiu = true; r.mensagem = "a ja e raiz (|f(a)| < e)."; return r; }
    if (abs(fb) < erro2) { r.raiz = b; r.convergiu = true; r.mensagem = "b ja e raiz (|f(b)| < e)."; return r; }

    for (int i = 1; i <= iterMax; i++) {
        double x  = (a * fb - b * fa) / (fb - fa);
        double fx = f.avaliar(x);

        if (fa * fx > 0) { a = x; fa = fx; } // raiz está em [x, b]
        else             { b = x; fb = fx; } // raiz está em [a, x]

        r.raiz = x;
        r.iteracoes = i;
        r.erro = abs(b - a);   // tamanho do intervalo atual

        if (abs(fx) < erro2) {
            r.convergiu = true;
            r.mensagem = "Convergiu: |f(x)| < e.";
            return r;
        }
        if (abs(b - a) < erro1) {
            r.raiz = escolha(f, a, b);
            r.convergiu = true;
            r.mensagem = "Convergiu: |b - a| < e.";
            return r;
        }
    }
    r.mensagem = "Atingiu o maximo de iteracoes sem convergir.";
    return r;
}