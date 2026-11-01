#include "AnaliseG.h"
#include "FuncHorner.h"
#include "PosicaoFalsa.h"

vector<pair<double, double>>
isolarRaizes(const Funcao& f, double xMin, double xMax, double passo) {
    vector<pair<double, double>> intervalos;
    int n = (int)ceil((xMax - xMin) / passo - 1e-9);
    double a = xMin, fa = f.avaliar(a);
    for (int i = 1; i <= n; i++) {
        double b = min(xMin + i * passo, xMax);
        double fb = f.avaliar(b);
       
        if (fa * fb < 0 || fb == 0 || (i == 1 && fa == 0))
            intervalos.emplace_back(a, b);
        a = b; fa = fb;
    }
    return intervalos;
}

static void imprimirLinha(double a3, double a2, const string& metodo,
                          const Resultado& r, double limite) {
    bool passou = r.convergiu && fabs(r.raiz) > limite;
    printf("%5.2f %5.2f  %-18s %10.6f %10.2e %5d   %-3s   %-3s   %s\n",
           a3, a2, metodo.c_str(), r.raiz, r.erro, r.iteracoes,
           r.convergiu ? "sim" : "nao", passou ? "SIM" : "nao",
           r.convergiu ? "" : r.mensagem.c_str());
}

void executarLote(const vector<double>& valoresA3,
                  const vector<double>& valoresA2,
                  const Parametros& par,
                  const vector<const InterfaceMetodo*>& metodosAbertos,
                  double limite) {
    printf("   a3    a2  %-18s %10s %10s %5s  conv  d>%.1f  obs\n",
           "metodo", "raiz", "erro", "iter", limite);
    printf("------------------------------------------------------------------------------\n");

    PosicaoFalsa pf;
    for (double a3 : valoresA3) {
        for (double a2 : valoresA2) {
            FuncHorner f(a3, a2);

            for (const InterfaceMetodo* m : metodosAbertos)
                imprimirLinha(a3, a2, m->nome(), m->calcular(f, par), limite);

            auto intervalos = isolarRaizes(f, 0.0, 5.0, 0.5);
            if (!intervalos.empty()) {
                Parametros parIntervalo(intervalos[0].first, par.precisao, par.lambda,
                                        par.passoDerivada, par.maximoIteracoes,
                                        intervalos[0].second);
                imprimirLinha(a3, a2, pf.nome(), pf.calcular(f, parIntervalo), limite);
            } else {
                Resultado semRaiz{};
                semRaiz.mensagem = "Sem raiz positiva em [0, 5]: nao ha intervalo.";
                imprimirLinha(a3, a2, "Posicao falsa", semRaiz, limite);
            }
        }
    }
}
