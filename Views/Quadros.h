#ifndef TB1_QUADROS_H
#define TB1_QUADROS_H

#include <string>
#include <vector>
#include "../Models/InterfaceMetodo.h"
#include "../Models/EstudoPassoH.h"
using namespace std;

struct LinhaQuadro {
    double a3{}, a2{}, lambda{};
    bool usaLambda{};   // so o Newton com FL usa lambda
    string metodo;
    string isolamento;
    Resultado r;
    int avaliacoesF{};
    int avaliacoesDf{};
    double tempoUs{};
};

class FuncaoContadora : public Funcao {
public:
    explicit FuncaoContadora(const Funcao& f) : f_(f) {}
    double avaliar(double x) const override { chamadasF++; return f_.avaliar(x); }
    double avaliarDerivada(double x) const override { chamadasDf++; return f_.avaliarDerivada(x); }

    mutable int chamadasF = 0;
    mutable int chamadasDf = 0;
private:
    const Funcao& f_;
};

LinhaQuadro executar(const InterfaceMetodo& metodo, const Funcao& funcao,
                     const Parametros& par, double a3, double a2,
                     const string& isolamento);

string textoPonto(double x0);
string textoIntervalo(double a, double b);

bool passouLimite(const Resultado& r);

// mensagem do metodo em uma linha so (para tabelas e CSV)
string mensagemEmLinha(const Resultado& r);

void imprimirEstudoPassoH(const vector<PontoEstudoH>& pontos, double a3, double a2, double x);

void imprimirQuadroResposta(const vector<LinhaQuadro>& linhas);
void imprimirQuadroComparativo(const vector<LinhaQuadro>& linhas);

void exportarCSV(const vector<LinhaQuadro>& linhas,
                 const string& arquivoResultados,
                 const string& arquivoHistorico);
void exportarMarkdown(const vector<LinhaQuadro>& linhas, const string& arquivo);
void exportarLatex(const vector<LinhaQuadro>& linhas, const string& arquivo);

#endif