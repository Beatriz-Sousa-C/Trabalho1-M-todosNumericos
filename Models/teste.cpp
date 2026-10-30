// Testes da parte P4 (caso padrão: a3 = 1, a2 = 1, d0 = 0,5, e = 0,001).
#include <bits/stdc++.h>
#include "../Models/FuncHorner.h"
#include "../Models/NewtonHorner.h"
#include "../Models/PosicaoFalsa.h"
#include "../Models/AnaliseG.h"

using namespace std;

static bool perto(double a, double b, double tol) { return fabs(a - b) < tol; }

int main() {
    FuncHorner f(1.0, 1.0);
    Parametros par{0.5, 0.001, 0.05, 1e-5, 100};

    // 1) Horner confere com a fórmula direta
    for (double x : {-3.0, 0.0, 0.5, 1.7, 2.8}) {
        assert(perto(f.avaliar(x), x*x*x - 9*x + 3, 1e-12));
        assert(perto(f.avaliarDerivada(x), 3*x*x - 9, 1e-12));
    }
    cout << "[ok] Horner = fórmula direta\n";

    // 2) Newton com Horner no caso padrão
    NewtonHorner nh;
    Resultado rn = nh.calcular(f, par);
    assert(rn.convergiu);
    cout << "[ok] " << nh.nome() << ": d = " << rn.raiz
              << " em " << rn.iteracoes << " iterações\n";

    // 3) Briot-Ruffini: resto ~ 0 na raiz
    vector<double> q;
    double resto = f.briotRuffini(rn.raiz, q);
    assert(fabs(resto) < 1e-6);
    cout << "[ok] Briot-Ruffini: resto = " << resto << "\n";

    // 4) Posição falsa no intervalo isolado
    auto intervalos = isolarRaizes(f, -5, 5, 0.1);
    assert(intervalos.size() == 3);
    PosicaoFalsa pf(intervalos[1].first, intervalos[1].second);
    Resultado rp = pf.calcular(f, par);
    assert(rp.convergiu && perto(rp.raiz, rn.raiz, 1e-3));
    cout << "[ok] Posição falsa: d = " << rp.raiz
              << " em " << rp.iteracoes << " iterações\n";

    // 5) Intervalo sem troca de sinal deve falhar
    PosicaoFalsa ruim(1.0, 2.0);
    assert(!ruim.calcular(f, par).convergiu);
    cout << "[ok] Posição falsa recusa intervalo sem troca de sinal\n";

    // 6) Lote do item g
    vector<double> grade{0.5, 1.0, 1.5, 2.0, 2.5, 3.0};
    cout << "\nAnalise (g):\n";
    executarLote(grade, grade, par, {&nh});
}