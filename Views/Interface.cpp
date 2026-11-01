#include "Interface.h"
#include "Quadros.h"
#include "Graficos.h"
#include "../Models/FuncHorner.h"
#include "../Models/FuncaoPendulo.h"
#include "../Models/NewtonRaphson.h"
#include "../Models/NewtonFL.h"
#include "../Models/NewtonDerivadaNumerica.h"
#include "../Models/NewtonHorner.h"
#include "../Models/Secante.h"
#include "../Models/Bissecao.h"
#include "../Models/PosicaoFalsa.h"
#include "../Models/AnaliseG.h"
#include "../Models/constantes.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

using namespace std;

namespace {

const NewtonRaphson NEWTON;
const NewtonFL NEWTON_FL;
const NewtonDerivadaNumerica NEWTON_NUM;
const NewtonHorner NEWTON_HORNER;
const Secante SECANTE;
const Bissecao BISSECAO;
const PosicaoFalsa POSICAO_FALSA;

// mesma lista no menu e nas tabelas: o nome mostrado e sempre metodo.nome()
const int NUM_METODOS = 7;
const InterfaceMetodo* const METODOS[NUM_METODOS] = {
    &NEWTON, &NEWTON_FL, &NEWTON_NUM, &NEWTON_HORNER, &SECANTE, &BISSECAO, &POSICAO_FALSA
};
const char* FAMILIAS[NUM_METODOS] = {"Newton, item a", "Newton, item b", "Newton, item c", "Newton",
                                     "Aberto", "Intervalo", "Intervalo"};

const double PASSO_DERIVADA_PADRAO = 1e-5;   // h da derivada numerica (~ raiz cubica do eps da maquina)
const double DISTANCIA_SECANTE = 0.1;        // x1 padrao da secante = d0 + 0,1

struct Config {
    double d0, x1, h, precisao;
    int maxIter;
};

Config configPadrao() {
    return {0.5, 0.5 + DISTANCIA_SECANTE, PASSO_DERIVADA_PADRAO, 0.001, 100};
}

string textoDoisPontos(double x0, double x1) {
    char buf[64];
    snprintf(buf, sizeof buf, "x0=%g;x1=%g", x0, x1);
    return buf;
}

void rodarCombinacao(vector<LinhaQuadro>& linhas, double a3, double a2,
                     double lambda, const Config& c, const vector<bool>& usar) {
    FuncaoPendulo direta(a3, a2);  // formula direta: todos os metodos menos o Newton com Horner
    FuncHorner poli(a3, a2);       // coeficientes: so o Newton com Horner

    //  métodos abertos: partem de d0 (a secante, de d0 e x1)
    Parametros pAberto(c.d0, c.precisao, lambda, c.h, c.maxIter);
    for (int i = 0; i < 3; i++)
        if (usar[i]) linhas.push_back(executar(*METODOS[i], direta, pAberto, a3, a2, textoPonto(c.d0)));
    if (usar[3]) linhas.push_back(executar(NEWTON_HORNER, poli, pAberto, a3, a2, textoPonto(c.d0)));

    if (usar[4]) {
        Parametros pSecante(c.d0, c.precisao, lambda, c.h, c.maxIter, c.x1);
        linhas.push_back(executar(SECANTE, direta, pSecante, a3, a2, textoDoisPontos(c.d0, c.x1)));
    }

    // métodos de intervalo: precisam do isolamento (Bolzano)
    if (!(usar[5] || usar[6])) return;
    auto intervalos = isolarRaizes(direta, 0.0, 5.0, PASSO_ISOLAMENTO);
    if (intervalos.empty()) {
        cout << "a3=" << a3 << " a2=" << a2 << ": " << mensagemSemIntervalo(0.0, 5.0, PASSO_ISOLAMENTO)
             << " (" << BISSECAO.nome() << " e " << POSICAO_FALSA.nome() << " ignoradas)\n";
        return;
    }
    double a = intervalos[0].first, b = intervalos[0].second;   // menor raiz positiva
    Parametros pIntervalo(a, c.precisao, lambda, c.h, c.maxIter, b);
    string iso = textoIntervalo(a, b);

    for (int i = 5; i < 7; i++)
        if (usar[i]) linhas.push_back(executar(*METODOS[i], direta, pIntervalo, a3, a2, iso));
}

//leitura
struct FimDaEntrada {};  

string lerLinha(const string& msg) {
    cout << msg << flush;
    string s;
    if (!getline(cin, s)) throw FimDaEntrada{};
    return s;
}

string textoNumero(double v) {
    char buf[32];
    snprintf(buf, sizeof buf, "%g", v);
    return buf;
}

// Aceita vírgula decimal ("0,05"). Enter = valor padrão.
double lerDouble(const string& msg, double padrao, bool positivo = false) {
    while (true) {
        string s = lerLinha(msg + " [" + textoNumero(padrao) + "]: ");
        if (s.empty()) return padrao;
        for (char& c : s) if (c == ',') c = '.';
        try {
            size_t pos;
            double v = stod(s, &pos);
            if (pos != s.size()) throw 1;
            if (positivo && v <= 0) { cout << "  O valor deve ser maior que zero.\n"; continue; }
            return v;
        } catch (...) {
            cout << "  Entrada invalida, tente de novo.\n";
        }
    }
}

int lerInt(const string& msg, int padrao, int minimo, int maximo) {
    while (true) {
        string s = lerLinha(msg + " [" + to_string(padrao) + "]: ");
        if (s.empty()) return padrao;
        try {
            size_t pos;
            int v = stoi(s, &pos);
            if (pos == s.size() && v >= minimo && v <= maximo) return v;
        } catch (...) {}
        cout << "  Digite um inteiro entre " << minimo << " e " << maximo << ".\n";
    }
}

// Enter = resposta padrão.
bool lerSimNao(const string& msg, bool padrao) {
    while (true) {
        string s = lerLinha(msg + (padrao ? " (S/n): " : " (s/N): "));
        if (s.empty()) return padrao;
        char c = (char)tolower((unsigned char)s[0]);
        if (c == 's') return true;
        if (c == 'n') return false;
        cout << "  Responda s ou n.\n";
    }
}

vector<bool> escolherMetodos() {
    cout << "\nMetodos disponiveis:\n";
    for (int i = 0; i < NUM_METODOS; i++)
        printf("  %d) %-28s (%s)\n", i + 1, METODOS[i]->nome().c_str(), FAMILIAS[i]);
    while (true) {
        string s = lerLinha("Escolha (ex: 1,2,5 | Enter = todos): ");
        vector<bool> usar(NUM_METODOS, false);
        if (s.empty() || s == "0") return vector<bool>(NUM_METODOS, true);
        for (char& c : s) if (c == ',') c = ' ';
        istringstream in(s);
        int v; bool ok = true, algum = false;
        while (in >> v) {
            if (v < 1 || v > NUM_METODOS) ok = false;
            else { usar[v - 1] = true; algum = true; }
        }
        if (ok && algum && in.eof()) return usar;
        cout << "  Selecao invalida.\n";
    }
}

}

//  Interface

Interface::Interface() {}

void Interface::mostrarEExportar(const vector<LinhaQuadro>& linhas) {
    if (linhas.empty()) { cout << "\nNenhum resultado para mostrar.\n"; return; }
    imprimirQuadroResposta(linhas);
    imprimirQuadroComparativo(linhas);
    exportarCSV(linhas, "resultados.csv", "historico.csv");
    exportarMarkdown(linhas, "quadro.md");
    exportarLatex(linhas, "quadro.tex");
    cout << "\nArquivos gerados: resultados.csv, historico.csv, quadro.md, quadro.tex\n"
         << "(use a opcao 4 do menu para gerar os graficos)\n";
}

void Interface::novaExecucao() {
    cout << "\n--- Nova execucao ---\n";
    int n = lerInt("n (numero de opcoes)", 1, 1, 50);
    double precisao = lerDouble("Precisao e", 0.001, true);
    double d0 = lerDouble("Aproximacao inicial d0", 0.5);
    int maxIter = lerInt("Maximo de iteracoes", 100, 1, 100000);
    vector<bool> usar = escolherMetodos();

    // h só importa para a derivada numérica (método 3); x1 só para a secante (método 5)
    double h = PASSO_DERIVADA_PADRAO;
    if (usar[2] && lerSimNao("Alterar o passo h da derivada numerica (padrao " +
                             textoNumero(PASSO_DERIVADA_PADRAO) + ")?", false))
        h = lerDouble("Passo h da derivada numerica", PASSO_DERIVADA_PADRAO, true);
    double x1 = d0 + DISTANCIA_SECANTE;
    if (usar[4]) {
        x1 = lerDouble("Segunda aproximacao x1 da secante", d0 + DISTANCIA_SECANTE);
        if (x1 == d0) {
            x1 = d0 + DISTANCIA_SECANTE;
            cout << "  A secante precisa de x1 diferente de d0; usando x1 = " << x1 << ".\n";
        }
    }
    Config c{d0, x1, h, precisao, maxIter};

    struct Opcao { double lambda, a3, a2; };
    vector<Opcao> opcoes;
    double lamAnt = 0.05, a3Ant = 1.0, a2Ant = 1.0;
    for (int i = 1; i <= n; i++) {
        cout << "\nOpcao " << i << " de " << n << ":\n";
        Opcao o;
        o.lambda = lerDouble("  lambda (positivo, perto de 0)", lamAnt, true);
        o.a3 = lerDouble("  a3 (diferente de 0)", a3Ant);
        if (o.a3 == 0) { cout << "  a3 = 0 deixa a equacao linear; usando a3 = 1.\n"; o.a3 = 1; }
        o.a2 = lerDouble("  a2", a2Ant);
        lamAnt = o.lambda; a3Ant = o.a3; a2Ant = o.a2;   // o último vira sugestão da próxima
        opcoes.push_back(o);
    }

    vector<LinhaQuadro> linhas;
    for (const Opcao& o : opcoes)
        rodarCombinacao(linhas, o.a3, o.a2, o.lambda, c, usar);
    mostrarEExportar(linhas);
}

void Interface::casoPadrao() {
    cout << "\nCaso padrao do enunciado: a3=1, a2=1, d0=0,5, lambda=0,05, e=0,001\n"
         << "(derivada numerica com h=1e-5; secante com x0=0,5 e x1=0,6)\n";
    vector<LinhaQuadro> linhas;
    rodarCombinacao(linhas, 1.0, 1.0, 0.05, configPadrao(), vector<bool>(NUM_METODOS, true));
    mostrarEExportar(linhas);
}

void Interface::analiseGrade() {
    cout << "\nAnalise (g): a3 e a2 em {0,5; 1; 1,5; 2; 2,5; 3}, lambda=0,05, d0=0,5, e=0,001\n"
         << "(derivada numerica com h=1e-5; secante com x0=0,5 e x1=0,6)\n";
    vector<double> valores{0.5, 1.0, 1.5, 2.0, 2.5, 3.0};
    vector<LinhaQuadro> linhas;
    for (double a3 : valores)
        for (double a2 : valores)
            rodarCombinacao(linhas, a3, a2, 0.05, configPadrao(), vector<bool>(NUM_METODOS, true));

    int naoConv = 0, passou = 0, semSentido = 0;
    for (const LinhaQuadro& l : linhas) {
        if (!l.r.convergiu) naoConv++;
        if (passouLimite(l.r)) passou++;
        if (semSentidoFisico(l.r)) semSentido++;
    }
    exportarCSV(linhas, "resultados_grade.csv", "historico_grade.csv");
    cout << linhas.size() << " execucoes: " << naoConv << " nao convergiram, "
         << passou << " passaram de 0,3 cm, "
         << semSentido << " convergiram para raiz d <= 0 (sem sentido fisico).\n"
         << "Arquivos gerados: resultados_grade.csv, historico_grade.csv\n";
}

void Interface::estudoPasso() {
    cout << "\n--- Estudo do passo h da derivada numerica ---\n";
    double a3 = lerDouble("a3", 1.0);
    double a2 = lerDouble("a2", 1.0);
    double x = lerDouble("Ponto x", 0.5);
    FuncaoPendulo f(a3, a2);
    vector<PontoEstudoH> pontos = estudoPassoH(f, x);
    imprimirEstudoPassoH(pontos, a3, a2, x);
    if (gerarFiguraEstudoPassoH(pontos, a3, a2, x, PASSO_DERIVADA_PADRAO, "figuras"))
        cout << "\nGrafico gerado: figuras/07_estudo_passo_h.svg\n";
    else
        cout << "\nNao foi possivel gerar o grafico do estudo do passo h.\n";
}

void Interface::gerarGraficos() {
    ifstream teste("resultados.csv");
    if (!teste) {
        cout << "\nAinda nao ha resultados.csv: rode a opcao 1 ou 2 primeiro.\n";
        return;
    }
    cout << "\n";
    if (gerarFiguras(".", "figuras"))
        cout << "Pronto: abra os arquivos .svg da pasta figuras/ (VS Code ou navegador).\n";
    else
        cout << "Nao foi possivel gerar os graficos.\n";
}

void Interface::executar() {
    cout << "==============================================\n"
         << " Raizes de equacoes - Tema 3 (pendulo)\n"
         << " f(d) = a3*d^3 - 9*a2*d + 3\n"
         << "==============================================\n"
         << "(Enter aceita o valor entre colchetes; virgula decimal e aceita)\n";
    try {
        while (true) {
            cout << "\n--- MENU ---\n"
                 << "1) Nova execucao (voce digita os dados)\n"
                 << "2) Caso padrao do enunciado\n"
                 << "3) Analise (g): grade de a3 e a2\n"
                 << "4) Gerar graficos\n"
                 << "5) Estudo do passo h da derivada numerica\n"
                 << "0) Sair\n";
            int op = lerInt("Opcao", 1, 0, 5);
            if (op == 0) break;
            if (op == 1) novaExecucao();
            else if (op == 2) casoPadrao();
            else if (op == 3) analiseGrade();
            else if (op == 4) gerarGraficos();
            else estudoPasso();
        }
    } catch (const FimDaEntrada&) {
        cout << "\n";
    }
    cout << "Ate logo!\n";
}
