#include "Quadros.h"
#include "../Models/NewtonHorner.h"
#include "../Models/NewtonFL.h"
#include "../Models/FuncHorner.h"
#include "../Models/constantes.h"
#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

static const int REPETICOES_TEMPO = 1000;

LinhaQuadro executar(const InterfaceMetodo& metodo, const Funcao& funcao,
                     const Parametros& par, double a3, double a2,
                     const string& isolamento) {
    LinhaQuadro l;
    l.a3 = a3;
    l.a2 = a2;
    l.lambda = par.lambda;
    l.usaLambda = dynamic_cast<const NewtonFL*>(&metodo) != nullptr;
    l.metodo = metodo.nome();
    l.isolamento = isolamento;

    const auto* poli = dynamic_cast<const FuncHorner*>(&funcao);
    if (dynamic_cast<const NewtonHorner*>(&metodo) && poli) {
        int antes = poli->passadasHorner();
        l.r = metodo.calcular(funcao, par);
        l.avaliacoesF = poli->passadasHorner() - antes;
        l.avaliacoesDf = l.avaliacoesF;
    } else {
        FuncaoContadora contadora(funcao);
        l.r = metodo.calcular(contadora, par);
        l.avaliacoesF = contadora.chamadasF;
        l.avaliacoesDf = contadora.chamadasDf;
    }

    auto inicio = chrono::steady_clock::now();
    for (int i = 0; i < REPETICOES_TEMPO; i++) metodo.calcular(funcao, par);
    auto fim = chrono::steady_clock::now();
    l.tempoUs = chrono::duration<double, micro>(fim - inicio).count() / REPETICOES_TEMPO;
    return l;
}

string textoPonto(double x0) {
    char buf[64];
    snprintf(buf, sizeof buf, "d0=%g", x0);
    return buf;
}

string textoIntervalo(double a, double b) {
    char buf[64];
    snprintf(buf, sizeof buf, "[%g;%g]", a, b);
    return buf;
}

string mensagemEmLinha(const Resultado& r) {
    string s;
    for (char c : r.mensagem) {
        if (c == '\n') s += " / ";
        else s += c;
    }
    return s;
}

// virgula viraria separador de coluna no CSV
static string mensagemCSV(const Resultado& r) {
    string s = mensagemEmLinha(r);
    for (char& c : s) if (c == ',') c = ';';
    return s;
}

// raiz d <= 0 nao e deslocamento do pendulo: nem "passou" nem "nao passou"
static const char* textoPassou(const Resultado& r) {
    if (semSentidoFisico(r)) return "d<=0";
    return passouLimite(r) ? "SIM" : "nao";
}

static string textoLambda(const LinhaQuadro& l) {
    if (!l.usaLambda) return "-";
    char buf[32];
    snprintf(buf, sizeof buf, "%g", l.lambda);
    return buf;
}

void imprimirEstudoPassoH(const vector<PontoEstudoH>& pontos, double a3, double a2, double x) {
    printf("\nESTUDO DO PASSO h DA DERIVADA NUMERICA (a3 = %g, a2 = %g, x = %g)\n", a3, a2, x);
    printf("f'(x) ~ [f(x+h) - f(x-h)] / 2h   (diferenca central, erro de truncamento O(h^2))\n");
    printf("%10s %22s %14s\n", "h", "f'(x) aproximada", "|erro|");
    printf("%s\n", string(48, '-').c_str());
    for (const PontoEstudoH& p : pontos)
        printf("%10.0e %22.15f %14.3e\n", p.h, p.derivadaAproximada, p.erro);
}

void imprimirQuadroResposta(const vector<LinhaQuadro>& linhas) {
    printf("\nQUADRO RESPOSTA (item e)\n");
    printf("%5s %5s %6s  %-30s %12s %10s  %-4s  %-8s  %s\n",
           "a3", "a2", "lambda", "metodo", "d (cm)", "erro", "conv", "d > 0,3?", "obs");
    printf("%s\n", string(110, '-').c_str());
    for (const LinhaQuadro& l : linhas) {
        printf("%5.2f %5.2f %6s  %-30s %12.6f %10.2e  %-4s  %-8s  %s\n",
               l.a3, l.a2, textoLambda(l).c_str(), l.metodo.c_str(), l.r.raiz, l.r.erro,
               l.r.convergiu ? "sim" : "nao",
               textoPassou(l.r), mensagemEmLinha(l.r).c_str());
    }
}

void imprimirQuadroComparativo(const vector<LinhaQuadro>& linhas) {
    printf("\nQUADRO COMPARATIVO (item f)\n");
    for (size_t i = 0; i < linhas.size(); i++) {
        const LinhaQuadro& l = linhas[i];
        bool novaCombinacao = i == 0 || l.a3 != linhas[i-1].a3 ||
                              l.a2 != linhas[i-1].a2 || l.lambda != linhas[i-1].lambda;
        if (novaCombinacao) {
            printf("\na3 = %g, a2 = %g, lambda = %g\n", l.a3, l.a2, l.lambda);
            printf("%-30s %-16s %12s %10s %5s %6s %6s %10s\n",
                   "metodo", "isolamento", "raiz", "erro", "iter",
                   "f(x)", "f'(x)", "tempo(us)");
            printf("%s\n", string(103, '-').c_str());
        }
        printf("%-30s %-16s %12.6f %10.2e %5d %6d %6d %10.3f\n",
               l.metodo.c_str(), l.isolamento.c_str(), l.r.raiz, l.r.erro,
               l.r.iteracoes, l.avaliacoesF, l.avaliacoesDf, l.tempoUs);
    }
}


void exportarCSV(const vector<LinhaQuadro>& linhas,
                 const string& arquivoResultados,
                 const string& arquivoHistorico) {
    ofstream res(arquivoResultados);
    if (!res) { cerr << "Nao foi possivel criar " << arquivoResultados << '\n'; return; }
    res << setprecision(10);
    res << "a3,a2,lambda,metodo,isolamento,raiz,erro,iteracoes,"
           "aval_f,aval_df,tempo_us,convergiu,passou_limite,sem_sentido_fisico,mensagem\n";
    for (const LinhaQuadro& l : linhas) {
        res << l.a3 << ',' << l.a2 << ',' << l.lambda << ','
            << l.metodo << ',' << l.isolamento << ','
            << l.r.raiz << ',' << l.r.erro << ',' << l.r.iteracoes << ','
            << l.avaliacoesF << ',' << l.avaliacoesDf << ',' << l.tempoUs << ','
            << l.r.convergiu << ',' << passouLimite(l.r) << ',' << semSentidoFisico(l.r) << ','
            << mensagemCSV(l.r) << '\n';
    }

    ofstream hist(arquivoHistorico);
    if (!hist) { cerr << "Nao foi possivel criar " << arquivoHistorico << '\n'; return; }
    hist << setprecision(15);
    hist << "metodo,a3,a2,lambda,k,x_k\n";
    for (const LinhaQuadro& l : linhas) {
        for (size_t k = 0; k < l.r.historico.size(); k++) {
            hist << l.metodo << ',' << l.a3 << ',' << l.a2 << ',' << l.lambda << ','
                 << k << ',' << l.r.historico[k] << '\n';
        }
    }
}

void exportarMarkdown(const vector<LinhaQuadro>& linhas, const string& arquivo) {
    ofstream out(arquivo);
    if (!out) { cerr << "Nao foi possivel criar " << arquivo << '\n'; return; }
    out << "| a3 | a2 | λ | Método | Isolamento | d (cm) | Erro | Iter. | Aval. f | Tempo (µs) | d > 0,3? |\n";
    out << "|---|---|---|---|---|---|---|---|---|---|---|\n";
    char buf[512];
    for (const LinhaQuadro& l : linhas) {
        snprintf(buf, sizeof buf,
                 "| %g | %g | %s | %s | %s | %.6f | %.2e | %d | %d | %.3f | %s |\n",
                 l.a3, l.a2, textoLambda(l).c_str(), l.metodo.c_str(), l.isolamento.c_str(),
                 l.r.raiz, l.r.erro, l.r.iteracoes, l.avaliacoesF, l.tempoUs,
                 semSentidoFisico(l.r) ? "d ≤ 0" : passouLimite(l.r) ? "sim" : "não");
        out << buf;
    }
}

void exportarLatex(const vector<LinhaQuadro>& linhas, const string& arquivo) {
    ofstream out(arquivo);
    if (!out) { cerr << "Nao foi possivel criar " << arquivo << '\n'; return; }
    out << "\\begin{tabular}{rrrllrrrrrc}\n\\hline\n";
    out << "$a_3$ & $a_2$ & $\\lambda$ & M\\'etodo & Isolamento & $d$ (cm) & Erro"
           " & Iter. & Aval. $f$ & Tempo ($\\mu$s) & $d > 0{,}3$ \\\\\n\\hline\n";
    char buf[512];
    for (const LinhaQuadro& l : linhas) {
        snprintf(buf, sizeof buf,
                 "%g & %g & %s & %s & %s & %.6f & %.2e & %d & %d & %.3f & %s \\\\\n",
                 l.a3, l.a2, textoLambda(l).c_str(), l.metodo.c_str(), l.isolamento.c_str(),
                 l.r.raiz, l.r.erro, l.r.iteracoes, l.avaliacoesF, l.tempoUs,
                 semSentidoFisico(l.r) ? "$d \\le 0$" : passouLimite(l.r) ? "sim" : "n\\~ao");
        out << buf;
    }
    out << "\\hline\n\\end{tabular}\n";
}
