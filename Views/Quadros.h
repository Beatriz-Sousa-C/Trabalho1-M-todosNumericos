#ifndef TB1_QUADROS_H
#define TB1_QUADROS_H

#include <string>
#include <vector>
#include "../Models/InterfaceMetodo.h"
#include "../Models/EstudoPassoH.h"

struct LinhaQuadro {
    double a3{}, a2{}, lambda{};
    bool usaLambda{};   // so o Newton com FL usa lambda
    std::string metodo;
    std::string isolamento;
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
                     const std::string& isolamento);

std::string textoPonto(double x0);
std::string textoIntervalo(double a, double b);

// mensagem do metodo em uma linha so (para tabelas e CSV)
std::string mensagemEmLinha(const Resultado& r);

void imprimirEstudoPassoH(const std::vector<PontoEstudoH>& pontos, double a3, double a2, double x);

void imprimirQuadroResposta(const std::vector<LinhaQuadro>& linhas);
void imprimirQuadroComparativo(const std::vector<LinhaQuadro>& linhas);

void exportarCSV(const std::vector<LinhaQuadro>& linhas,
                 const std::string& arquivoResultados,
                 const std::string& arquivoHistorico);
void exportarMarkdown(const std::vector<LinhaQuadro>& linhas, const std::string& arquivo);
void exportarLatex(const std::vector<LinhaQuadro>& linhas, const std::string& arquivo);

#endif