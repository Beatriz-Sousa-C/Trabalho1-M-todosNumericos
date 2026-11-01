#include "CalculoController.h"
#include "../Models/FuncHorner.h"
#include "../Models/NewtonDerivadaNumerica.h"
#include "../Models/NewtonHorner.h"
#include "../Models/Secante.h"
#include "../Models/Bissecao.h"
#include "../Models/PosicaoFalsa.h"
#include "../Models/AnaliseG.h"
#include "../Models/EstudoPassoH.h"
#include "../Models/constantes.h"
#include <iostream>

static const int MAXIMO_ITERACOES_PADRAO = 10000;

Resultado CalculoController::calcularNewtonRaphson(
    double aproximacaoInicial,
    double precisao,
    double a3,
    double a2) {
    Parametros parametros(aproximacaoInicial, precisao, 0.0, 0.0, MAXIMO_ITERACOES_PADRAO);

    FuncaoPendulo funcoes = FuncaoPendulo(a3,a2);

    NewtonRaphson nr;
    return nr.calcular(funcoes, parametros);
}

Resultado CalculoController::calcularNewtonFL(
    double aproximacaoInicial,
    double precisao,
    double lambda,
    double a3,
    double a2) {
    Parametros parametros(aproximacaoInicial, precisao, lambda, 0.0, MAXIMO_ITERACOES_PADRAO);

    FuncaoPendulo funcoes = FuncaoPendulo(a3,a2);

    NewtonFL nfl;
    return nfl.calcular(funcoes, parametros);
}

Resultado CalculoController::calcularNewtonNumerico(
    double aproximacaoInicial,
    double precisao,
    double passoDerivada,
    double a3,
    double a2) {
    Parametros parametros(aproximacaoInicial, precisao, 0.0, passoDerivada, MAXIMO_ITERACOES_PADRAO);

    FuncaoPendulo funcoes = FuncaoPendulo(a3,a2);

    NewtonDerivadaNumerica nn;
    return nn.calcular(funcoes, parametros);
}

void CalculoController::rodarCombinacao(double a3, double a2, double lambda,
                                        double d0, double precisao, int maxIter) {
    FuncHorner f(a3, a2);
    Parametros pAberto(d0, precisao, lambda, 1e-5, maxIter);

    NewtonRaphson newton;
    NewtonFL newtonFL;
    NewtonDerivadaNumerica newtonNum;
    NewtonHorner newtonHorner;
    for (const InterfaceMetodo* m : std::vector<const InterfaceMetodo*>{&newton, &newtonFL, &newtonNum, &newtonHorner})
        linhas.push_back(executar(*m, f, pAberto, a3, a2, textoPonto(d0)));

    auto intervalos = isolarRaizes(f, 0.0, 5.0, PASSO_ISOLAMENTO);
    if (intervalos.empty()) {
        std::cout << "a3=" << a3 << " a2=" << a2 << ": sem raiz positiva em [0, 5]\n";
        return;
    }
    double a = intervalos[0].first, b = intervalos[0].second;
    Parametros pIntervalo(a, precisao, lambda, 1e-5, maxIter, b);
    std::string iso = textoIntervalo(a, b);

    Secante secante;
    Bissecao bissecao;
    PosicaoFalsa posicaoFalsa;
    for (const InterfaceMetodo* m : std::vector<const InterfaceMetodo*>{&secante, &bissecao, &posicaoFalsa})
        linhas.push_back(executar(*m, f, pIntervalo, a3, a2, iso));
}

void CalculoController::estudarPassoH(double a3, double a2, double x) {
    FuncHorner f(a3, a2);
    imprimirEstudoPassoH(estudoPassoH(f, x), a3, a2, x);
}

void CalculoController::apresentarResultados() {
    imprimirQuadroResposta(linhas);
    imprimirQuadroComparativo(linhas);

    exportarCSV(linhas, "resultados.csv", "historico.csv");
    exportarMarkdown(linhas, "quadro.md");
    exportarLatex(linhas, "quadro.tex");
    std::cout << "\nArquivos gerados: resultados.csv, historico.csv, quadro.md, quadro.tex\n";
}
