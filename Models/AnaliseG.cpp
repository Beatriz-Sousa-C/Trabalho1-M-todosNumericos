#include "AnaliseG.h"
#include "FuncHorner.h"
#include "FuncaoPendulo.h"
#include "NewtonHorner.h"
#include "PosicaoFalsa.h"
#include "constantes.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

using namespace std;

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

string mensagemSemIntervalo(double xMin, double xMax, double passo) {
    char buf[200];
    snprintf(buf, sizeof buf,
             "Sem troca de sinal em [%g, %g] com passo %g: ou nao ha raiz positiva, "
             "ou ha duas raizes a menos de %g uma da outra.", xMin, xMax, passo, passo);
    return buf;
}

static void imprimirLinha(double a3, double a2, const string& metodo,
                          const Resultado& r, double limite) {
    bool passou = r.convergiu && r.raiz > limite;
    const char* obs = !r.convergiu ? r.mensagem.c_str()
                    : semSentidoFisico(r) ? "raiz <= 0: sem sentido fisico" : "";
    printf("%5.2f %5.2f  %-28s %10.6f %10.2e %5d   %-3s   %-3s   %s\n",
           a3, a2, metodo.c_str(), r.raiz, r.erro, r.iteracoes,
           r.convergiu ? "sim" : "nao", passou ? "SIM" : "nao", obs);
}

void executarLote(const vector<double>& valoresA3,
                  const vector<double>& valoresA2,
                  const Parametros& par,
                  const vector<const InterfaceMetodo*>& metodosAbertos,
                  double limite) {
    printf("   a3    a2  %-28s %10s %10s %5s  conv  d>%.1f  obs\n",
           "metodo", "raiz", "erro", "iter", limite);
    printf("----------------------------------------------------------------------------------------\n");

    PosicaoFalsa pf;
    for (double a3 : valoresA3) {
        for (double a2 : valoresA2) {
            FuncHorner poli(a3, a2);       // só o Newton com Horner usa os coeficientes
            FuncaoPendulo direta(a3, a2);  // os demais avaliam a fórmula direta

            for (const InterfaceMetodo* m : metodosAbertos) {
                const Funcao& f = dynamic_cast<const NewtonHorner*>(m) ? (const Funcao&)poli : direta;
                imprimirLinha(a3, a2, m->nome(), m->calcular(f, par), limite);
            }

            auto intervalos = isolarRaizes(direta, 0.0, 5.0, PASSO_ISOLAMENTO);
            if (!intervalos.empty()) {
                Parametros parIntervalo(intervalos[0].first, par.precisao, par.lambda,
                                        par.passoDerivada, par.maximoIteracoes,
                                        intervalos[0].second);
                imprimirLinha(a3, a2, pf.nome(), pf.calcular(direta, parIntervalo), limite);
            } else {
                Resultado semRaiz{};
                semRaiz.mensagem = mensagemSemIntervalo(0.0, 5.0, PASSO_ISOLAMENTO);
                imprimirLinha(a3, a2, pf.nome(), semRaiz, limite);
            }
        }
    }
}
