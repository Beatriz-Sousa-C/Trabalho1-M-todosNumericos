#include <iostream>
#include <vector>
#include "Models/FuncHorner.h"
#include "Models/NewtonRaphson.h"
#include "Models/NewtonFL.h"
#include "Models/NewtonDerivadaNumerica.h"
#include "Models/NewtonHorner.h"
#include "Models/Secante.h"
#include "Models/Bissecao.h"
#include "Models/PosicaoFalsa.h"
#include "Models/AnaliseG.h"
#include "Views/Quadros.h"
#include "Views/Interface.h"
#include "Controllers/CalculoController.h"
using namespace std;

void rodarCombinacao(vector<LinhaQuadro>& linhas, double a3, double a2,
                     double lambda, double d0, double precisao, int maxIter) {
    FuncHorner f(a3, a2);
    Parametros pAberto(d0, precisao, lambda, 1e-5, maxIter);

    NewtonRaphson newton;
    NewtonFL newtonFL;
    NewtonDerivadaNumerica newtonNum;
    NewtonHorner newtonHorner;
    for (const InterfaceMetodo* m : vector<const InterfaceMetodo*>{&newton, &newtonFL, &newtonNum, &newtonHorner})
        linhas.push_back(executar(*m, f, pAberto, a3, a2, textoPonto(d0)));

    auto intervalos = isolarRaizes(f, 0.0, 5.0, 0.01);
    if (intervalos.empty()) {
        cout << "a3=" << a3 << " a2=" << a2 << ": sem raiz positiva em [0, 5]\n";
        return;
    }
    double a = intervalos[0].first, b = intervalos[0].second;
    Parametros pIntervalo(a, precisao, lambda, 1e-5, maxIter, b);
    string iso = textoIntervalo(a, b);

    Secante secante;
    Bissecao bissecao;
    PosicaoFalsa posicaoFalsa(a, b);
    for (const InterfaceMetodo* m : vector<const InterfaceMetodo*>{&secante, &bissecao, &posicaoFalsa})
        linhas.push_back(executar(*m, f, pIntervalo, a3, a2, iso));
}

int main() {
    vector<LinhaQuadro> linhas;

    rodarCombinacao(linhas, 1.0, 1.0, 0.05, 0.5, 0.001, 100);
    rodarCombinacao(linhas, 2.0, 1.0, 0.05, 0.5, 0.001, 100);
    rodarCombinacao(linhas, 1.0, 0.5, 0.05, 0.5, 0.001, 100);

    imprimirQuadroResposta(linhas);
    imprimirQuadroComparativo(linhas);

    exportarCSV(linhas, "resultados.csv", "historico.csv");
    exportarMarkdown(linhas, "quadro.md");
    exportarLatex(linhas, "quadro.tex");
    cout << "\nArquivos gerados: resultados.csv, historico.csv, quadro.md, quadro.tex\n";
    return 0;
}
