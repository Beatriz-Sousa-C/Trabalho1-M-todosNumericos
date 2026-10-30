#include "AnaliseG.h"
#include "FuncHorner.h"
#include "PosicaoFalsa.h"

vector<pair<double, double>>
isolarRaizes(const Funcao& f, double xMin, double xMax, double passo) {
    vector<pair<double, double>> intervalos;
    double a = xMin, fa = f.avaliar(a);
    for (double b = xMin + passo; b <= xMax + 1e-12; b += passo) {
        double fb = f.avaliar(b);
        if (fa * fb <= 0) intervalos.emplace_back(a, b);   // f trocou de sinal
        a = b; fa = fb;
    }
    return intervalos;
}

static void imprimirLinha(double a3, double a2, const string& metodo,
                          const Resultado& r, double limite) {
    bool passou = r.convergiu && r.raiz > limite;
    printf("%5.2f %5.2f  %-18s %10.6f %10.2e %5d   %-3s   %-3s   %s\n",
           a3, a2, metodo.c_str(), r.raiz, r.erro, r.iteracoes,
           r.convergiu ? "sim" : "nao", passou ? "SIM" : "nao",
           r.convergiu ? "" : r.mensagem.c_str());   // mostra o motivo só quando falha
}

void executarLote(const vector<double>& valoresA3,
                  const vector<double>& valoresA2,
                  const Parametros& par,
                  const vector<const InterfaceMetodo*>& metodosAbertos,
                  double limite) {
    printf("   a3    a2  %-18s %10s %10s %5s  conv  d>%.1f  obs\n",
           "metodo", "raiz", "erro", "iter", limite);
    printf("------------------------------------------------------------------------------\n");

    for (double a3 : valoresA3) {
        for (double a2 : valoresA2) {
            FuncHorner f(a3, a2);

            for (const InterfaceMetodo* m : metodosAbertos)
                imprimirLinha(a3, a2, m->nome(), m->calcular(f, par), limite);

            // intervalo da menor raiz positiva (deslocamento físico)
            auto intervalos = isolarRaizes(f, 0.0, 5.0, 0.01);
            if (!intervalos.empty()) {
                PosicaoFalsa pf(intervalos[0].first, intervalos[0].second);
                imprimirLinha(a3, a2, pf.nome(), pf.calcular(f, par), limite);
            } else {
                Resultado semRaiz{};
                semRaiz.mensagem = "Sem raiz positiva em [0, 5]: nao ha intervalo.";
                imprimirLinha(a3, a2, "Posicao falsa", semRaiz, limite);
            }
        }
    }
}