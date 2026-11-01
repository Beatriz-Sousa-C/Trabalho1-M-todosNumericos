#include "Graficos.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace {

const double LIMITE = 0.3;   // o pêndulo rompe se d > 0,3 cm

// CSV
struct Tabela {
    vector<string> cab;
    vector<vector<string>> linhas;
    int col(const string& nome) const {
        for (size_t i = 0; i < cab.size(); i++) if (cab[i] == nome) return (int)i;
        return -1;
    }
    double num(size_t i, const string& nome) const {
        int c = col(nome);
        if (c < 0 || c >= (int)linhas[i].size()) return NAN;
        try { return stod(linhas[i][c]); } catch (...) { return NAN; }
    }
    const string& txt(size_t i, const string& nome) const {
        static const string vazio;
        int c = col(nome);
        return (c < 0 || c >= (int)linhas[i].size()) ? vazio : linhas[i][c];
    }
};

vector<string> dividir(const string& s) {
    vector<string> r; string cur; stringstream ss(s);
    while (getline(ss, cur, ',')) { if (!cur.empty() && cur.back() == '\r') cur.pop_back(); r.push_back(cur); }
    return r;
}

bool lerCSV(const string& caminho, Tabela& t) {
    ifstream in(caminho);
    if (!in) return false;
    string l;
    if (!getline(in, l)) return false;
    t.cab = dividir(l);
    while (getline(in, l)) if (!l.empty()) t.linhas.push_back(dividir(l));
    return true;
}

// matemática
double f(double d, double a3, double a2) { return a3 * d * d * d - 9 * a2 * d + 3; }

// Raízes reais de f: varre o eixo e refina por bisseção (referência para medir erros).
vector<double> raizesReais(double a3, double a2) {
    vector<double> r;
    double passo = 0.001, x = -10;
    while (x < 10) {
        double fa = f(x, a3, a2), fb = f(x + passo, a3, a2);
        if (fa == 0) r.push_back(x);
        else if (fa * fb < 0) {
            double lo = x, hi = x + passo;
            for (int i = 0; i < 100; i++) {
                double m = (lo + hi) / 2;
                if (f(lo, a3, a2) * f(m, a3, a2) <= 0) hi = m; else lo = m;
            }
            r.push_back((lo + hi) / 2);
        }
        x += passo;
    }
    return r;
}

// Raiz mais próxima de x0 (referência do erro de um método).
double raizReferencia(double x0, double a3, double a2) {
    vector<double> rs = raizesReais(a3, a2);
    if (rs.empty()) return x0;
    return *min_element(rs.begin(), rs.end(),
                        [&](double a, double b) { return fabs(a - x0) < fabs(b - x0); });
}

// SVG
const char* PALETA[] = {"#0072B2", "#D55E00", "#009E73", "#CC79A7", "#E69F00", "#56B4E9", "#444444"};
const int NPAL = 7;
const char* COR_TEXTO = "#2B3A4F";
const char* COR_GRADE = "#E3E6EB";

string esc(const string& s) {
    string r;
    for (char c : s) {
        if (c == '&') r += "&amp;"; else if (c == '<') r += "&lt;";
        else if (c == '>') r += "&gt;"; else r += c;
    }
    return r;
}

string fmt(double v, int dig = 3) {
    char b[64]; snprintf(b, sizeof b, "%.*g", dig, v);
    string r = b;
    replace(r.begin(), r.end(), '.', ',');   // vírgula decimal
    return r;
}

class Svg {
public:
    Svg(double w, double h) : W(w), H(h) {
        o << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << w << "\" height=\"" << h
          << "\" viewBox=\"0 0 " << w << " " << h << "\" font-family=\"Arial, Helvetica, sans-serif\">\n"
          << "<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n";
    }
    void linha(double x1, double y1, double x2, double y2, const string& cor,
               double larg = 1, const string& tracejado = "") {
        o << "<line x1=\"" << x1 << "\" y1=\"" << y1 << "\" x2=\"" << x2 << "\" y2=\"" << y2
          << "\" stroke=\"" << cor << "\" stroke-width=\"" << larg << "\""
          << (tracejado.empty() ? "" : " stroke-dasharray=\"" + tracejado + "\"") << "/>\n";
    }
    void retangulo(double x, double y, double w, double h, const string& cor) {
        o << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << w << "\" height=\"" << h
          << "\" fill=\"" << cor << "\"/>\n";
    }
    void texto(double x, double y, const string& s, double tam = 14, const string& ancora = "start",
               const string& cor = COR_TEXTO, bool negrito = false, double giro = 0) {
        o << "<text x=\"" << x << "\" y=\"" << y << "\" font-size=\"" << tam << "\" text-anchor=\""
          << ancora << "\" fill=\"" << cor << "\"" << (negrito ? " font-weight=\"bold\"" : "");
        if (giro != 0) o << " transform=\"rotate(" << giro << " " << x << " " << y << ")\"";
        o << ">" << esc(s) << "</text>\n";
    }
    void polilinha(const vector<pair<double, double>>& p, const string& cor, double larg = 2.5) {
        if (p.size() < 2) return;
        o << "<polyline fill=\"none\" stroke=\"" << cor << "\" stroke-width=\"" << larg
          << "\" stroke-linejoin=\"round\" points=\"";
        for (auto& q : p) o << q.first << "," << q.second << " ";
        o << "\"/>\n";
    }

    void marcador(int forma, double x, double y, const string& cor, double r = 5) {
        switch (forma % 4) {
            case 0: o << "<circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"" << r << "\" fill=\"" << cor << "\"/>\n"; break;
            case 1: retangulo(x - r, y - r, 2 * r, 2 * r, cor); break;
            case 2: o << "<polygon points=\"" << x << "," << y - r - 1 << " " << x - r - 1 << "," << y + r
                      << " " << x + r + 1 << "," << y + r << "\" fill=\"" << cor << "\"/>\n"; break;
            default: o << "<polygon points=\"" << x << "," << y - r - 1 << " " << x - r - 1 << "," << y
                       << " " << x << "," << y + r + 1 << " " << x + r + 1 << "," << y << "\" fill=\"" << cor << "\"/>\n";
        }
    }
    void poligono(const vector<pair<double, double>>& p, const string& cor, double opac) {
        if (p.size() < 3) return;
        o << "<polygon fill=\"" << cor << "\" fill-opacity=\"" << opac << "\" stroke=\"none\" points=\"";
        for (auto& q : p) o << q.first << "," << q.second << " ";
        o << "\"/>\n";
    }
    void caixa(double x, double y, double w, double h) {   
        o << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << w << "\" height=\"" << h
          << "\" fill=\"#FFFFFF\" fill-opacity=\"0.85\" stroke=\"#C9CFD8\"/>\n";
    }
    void inicioClip(const string& id, double x, double y, double w, double h) {
        o << "<clipPath id=\"" << id << "\"><rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << w
          << "\" height=\"" << h << "\"/></clipPath>\n<g clip-path=\"url(#" << id << ")\">\n";
    }
    void fimClip() { o << "</g>\n"; }
    void linhaPts(const vector<pair<double, double>>& p, const string& cor, double larg,
                  const string& tracejado) {
        if (p.size() < 2) return;
        o << "<polyline fill=\"none\" stroke=\"" << cor << "\" stroke-width=\"" << larg << "\""
          << (tracejado.empty() ? "" : " stroke-dasharray=\"" + tracejado + "\"") << " points=\"";
        for (auto& q : p) o << q.first << "," << q.second << " ";
        o << "\"/>\n";
    }
    bool salvar(const string& caminho) {
        ofstream out(caminho);
        if (!out) return false;
        out << o.str() << "</svg>\n";
        return true;
    }
    double W, H;
private:
    ostringstream o;
};

vector<double> marcas(double lo, double hi, int n = 6) {
    vector<double> r;
    double bruto = (hi - lo) / n;
    if (bruto <= 0) { r.push_back(lo); return r; }
    double mag = pow(10, floor(log10(bruto)));
    double passo = mag;
    for (double m : {1.0, 2.0, 2.5, 5.0, 10.0}) { passo = m * mag; if (passo >= bruto) break; }
    for (double v = ceil(lo / passo - 1e-9) * passo; v <= hi + 1e-9; v += passo)
        r.push_back(fabs(v) < passo * 1e-9 ? 0.0 : v);
    return r;
}

struct Area {
    double L, T, Wd, Hd;          // retângulo do gráfico em pixels
    double x0, x1, y0, y1;        // limites dos dados 
    bool logY = false;

    double X(double x) const { return L + Wd * (x - x0) / (x1 - x0); }
    double Y(double y) const {
        if (logY) y = log10(max(y, 1e-300));
        return T + Hd * (1 - (y - y0) / (y1 - y0));
    }
    void desenhar(Svg& s, const string& titulo, const string& rotX, const string& rotY,
                  bool xInteiro = false, double tam = 18, const string& sub = "") const {
        if (sub.empty()) s.texto(L, T - 18, titulo, tam, "start", COR_TEXTO, true);
        else {
            s.texto(L, T - 36, titulo, tam, "start", COR_TEXTO, true);
            s.texto(L, T - 16, sub, tam - 1, "start", "#4A5870");
        }
        if (logY) {
            for (int e = (int)ceil(y0 - 1e-9); e <= (int)floor(y1 + 1e-9); e++) {
                double py = T + Hd * (1 - (e - y0) / (y1 - y0));
                s.linha(L, py, L + Wd, py, COR_GRADE);
                s.texto(L - 8, py + 5, "1e" + to_string(e), 13, "end");
            }
        } else {
            for (double v : marcas(y0, y1)) {
                double py = Y(v);
                s.linha(L, py, L + Wd, py, COR_GRADE);
                s.texto(L - 8, py + 5, fmt(v), 13, "end");
            }
        }
        for (double v : marcas(x0, x1, xInteiro ? (int)min(10.0, x1 - x0) : 7)) {
            if (xInteiro && fabs(v - round(v)) > 1e-9) continue;
            double px = X(v);
            s.linha(px, T, px, T + Hd, COR_GRADE);
            s.texto(px, T + Hd + 20, fmt(v), 13, "middle");
        }
        s.linha(L, T + Hd, L + Wd, T + Hd, "#8A94A3", 1.5);
        s.linha(L, T, L, T + Hd, "#8A94A3", 1.5);
        s.texto(L + Wd / 2, T + Hd + 46, rotX, 15, "middle");
        if (!rotY.empty()) s.texto(L - 62, T + Hd / 2, rotY, 15, "middle", COR_TEXTO, false, -90);
    }
};

void legenda(Svg& s, double x, double y, const vector<string>& nomes, const map<string, int>& idx) {
    for (size_t i = 0; i < nomes.size(); i++) {
        int k = idx.at(nomes[i]);
        double yy = y + 28.0 * i;
        s.linha(x, yy, x + 30, yy, PALETA[k % NPAL], 3);
        s.marcador(k, x + 15, yy, PALETA[k % NPAL], 5);
        s.texto(x + 40, yy + 5, nomes[i], 14);
    }
}

// dados comuns
struct Contexto {
    Tabela res, hist, grade;
    bool temGrade = false;
    double a3 = 1, a2 = 1, lam = 0.05;     
    vector<string> metodos;                
    map<string, int> idx;                  
};

bool perto(double a, double b) { return fabs(a - b) < 1e-9; }

string familia(const string& nome) {
    string n;
    for (char ch : nome) n += (char)tolower((unsigned char)ch);
    if (n.find("bisse") != string::npos || n.find("falsa") != string::npos) return "Intervalo";
    if (n.find("newton") != string::npos) return "Newton";
    return "Abertos";   
}

// Histórico x_k de um método na combinação mostrada (só valores finitos).
vector<double> serieDe(const Contexto& c, const string& metodo) {
    vector<double> xs;
    for (size_t i = 0; i < c.hist.linhas.size(); i++)
        if (c.hist.txt(i, "metodo") == metodo && perto(c.hist.num(i, "a3"), c.a3) &&
            perto(c.hist.num(i, "a2"), c.a2) && perto(c.hist.num(i, "lambda"), c.lam)) {
            double x = c.hist.num(i, "x_k");
            if (isfinite(x)) xs.push_back(x);
        }
    return xs;
}

// Desenha y = fn(x) em [xa, xb]; interrompe o traço onde fn não é finita.
template <class F>
void curva(Svg& s, const Area& a, F fn, double xa, double xb, const string& cor, double larg = 2.5,
           const string& tracejado = "", int n = 400) {
    vector<pair<double, double>> seg;
    for (int i = 0; i <= n; i++) {
        double x = xa + (xb - xa) * i / n, y = fn(x);
        if (!isfinite(y)) { s.linhaPts(seg, cor, larg, tracejado); seg.clear(); continue; }
        double py = max(-5000.0, min(5000.0, a.Y(y)));   
        seg.push_back({a.X(x), py});
    }
    s.linhaPts(seg, cor, larg, tracejado);
}

// convergência
bool figConvergencia(const Contexto& c, const string& pasta, const vector<string>& metodos,
                     const string& titulo, const string& arquivo) {
    struct Serie { string nome; vector<double> k, e; };
    vector<Serie> series;
    double kmax = 1, emax = 0, emin = 1e300;
    const double PISO = 1e-17;  
    for (const string& m : metodos) {
        vector<double> xs = serieDe(c, m);
        if (xs.empty()) continue;
        Serie s; s.nome = m;
        double ref = raizReferencia(xs.back(), c.a3, c.a2);   
        for (size_t k = 0; k < xs.size(); k++) {
            double e = max(fabs(xs[k] - ref), PISO);
            s.k.push_back((double)k); s.e.push_back(e);
            emin = min(emin, e); emax = max(emax, e);
        }
        kmax = max(kmax, s.k.back());
        series.push_back(s);
    }
    if (series.empty()) return false;
    double lo = floor(log10(emin)), hi = ceil(log10(emax));
    if (hi - lo < 2) { lo = hi - 2; }

    Svg svg(1150, 620);
    Area a{90, 70, 800, 440, 0, kmax, lo, hi};
    a.logY = true;
    a.desenhar(svg, titulo, "iteração k", "erro |x_k − r| (escala log)", true);
    double tol = 1e-3;   
    if (log10(tol) > a.y0 && log10(tol) < a.y1) {
        svg.linha(a.L, a.Y(tol), a.L + a.Wd, a.Y(tol), "#B54A00", 1.5, "6,5");
        svg.texto(a.L + a.Wd - 6, a.Y(tol) - 6, "precisão 1e-3", 13, "end", "#B54A00");
    }
    vector<string> nomes;
    for (auto& s : series) {
        int k = c.idx.at(s.nome);
        vector<pair<double, double>> pts;
        for (size_t i = 0; i < s.k.size(); i++) pts.push_back({a.X(s.k[i]), a.Y(s.e[i])});
        svg.polilinha(pts, PALETA[k % NPAL]);
        for (auto& p : pts) svg.marcador(k, p.first, p.second, PALETA[k % NPAL]);
        nomes.push_back(s.nome);
    }
    legenda(svg, 920, 90, nomes, c.idx);
    svg.texto(a.L, 600, "Reta descendente mais inclinada = convergência mais rápida. Erro 1e-17 = zero numérico.",
              13, "start", "#6B7788");
    return svg.salvar(pasta + "/" + arquivo);
}

// f(d) e raízes
bool figFuncao(const Contexto& c, const string& pasta) {
    vector<double> rs = raizesReais(c.a3, c.a2);
    double xa = -4.5, xb = 4.5;
    if (!rs.empty()) { xa = rs.front() - 0.8; xb = rs.back() + 0.8; }
    double ya = -25, yb = 25;
    Svg svg(1000, 620);
    Area a{90, 70, 800, 440, xa, xb, ya, yb};
    a.desenhar(svg, "f(d) = a3·d³ − 9·a2·d + 3  (a3=" + fmt(c.a3) + ", a2=" + fmt(c.a2) + ", λ=" + fmt(c.lam) + ")",
               "d (cm)", "f(d)");
    svg.inicioClip("c02", a.L, a.T, a.Wd, a.Hd);
    svg.linha(a.L, a.Y(0), a.L + a.Wd, a.Y(0), COR_TEXTO, 1.5);
    svg.linha(a.X(LIMITE), a.T, a.X(LIMITE), a.T + a.Hd, "#B54A00", 1.5, "6,5");
    curva(svg, a, [&](double x) { return f(x, c.a3, c.a2); }, xa, xb, PALETA[0], 3, "", 600);
    svg.fimClip();
    svg.texto(a.X(LIMITE) + 8, a.T + 16, "limite d = 0,3 cm", 13, "start", "#B54A00");
    for (double r : rs) {
        svg.marcador(0, a.X(r), a.Y(0), "#D55E00", 7);
        bool pendulo = (r > 0 && r < 1);   
        svg.texto(a.X(r) + (pendulo ? 14 : 0), a.Y(0) + (pendulo ? -14 : 24), "d = " + fmt(r, 4), 14,
                  pendulo ? "start" : "middle", "#D55E00", true);
    }
    svg.texto(a.L, 600, "A raiz entre 0 e 1 é o deslocamento do pêndulo; se passar de 0,3 cm, ele rompe.",
              13, "start", "#6B7788");
    return svg.salvar(pasta + "/02_f_raizes.svg");
}

bool figIntervalos(const Contexto& c, const string& pasta) {
    struct Painel { string nome; vector<double> A, B; double raiz; };
    vector<Painel> ps;
    for (const string& m : c.metodos) {
        if (familia(m) != "Intervalo") continue;
        double a0 = NAN, b0 = NAN;
        for (size_t i = 0; i < c.res.linhas.size(); i++)
            if (c.res.txt(i, "metodo") == m && perto(c.res.num(i, "a3"), c.a3) &&
                perto(c.res.num(i, "a2"), c.a2) && perto(c.res.num(i, "lambda"), c.lam)) {
                if (sscanf(c.res.txt(i, "isolamento").c_str(), "[%lf;%lf]", &a0, &b0) != 2) a0 = NAN;
                break;
            }
        vector<double> xs = serieDe(c, m);
        if (isnan(a0) || xs.empty()) continue;
        Painel p; p.nome = m; p.raiz = raizReferencia(xs.back(), c.a3, c.a2);
        double a = a0, b = b0, fa = f(a, c.a3, c.a2);
        p.A.push_back(a); p.B.push_back(b);
        for (double x : xs) {
            double fx = f(x, c.a3, c.a2);
            if (fx == 0) { a = b = x; }
            else if ((fa > 0) == (fx > 0)) { a = x; fa = fx; }
            else b = x;
            p.A.push_back(a); p.B.push_back(b);
        }
        ps.push_back(p);
    }
    if (ps.empty()) return false;
    double ymin = 1e300, ymax = -1e300, kmax = 1;
    for (auto& p : ps) {
        for (double v : p.A) ymin = min(ymin, v);
        for (double v : p.B) ymax = max(ymax, v);
        kmax = max(kmax, (double)p.A.size() - 1);
    }
    double mg = max((ymax - ymin) * 0.08, 1e-9);
    int n = (int)ps.size();
    Svg svg(110 + n * 560, 600);
    for (int i = 0; i < n; i++) {
        Painel& p = ps[i];
        Area a{90.0 + i * 560, 80, 470, 380, 0, kmax, ymin - mg, ymax + mg};
        const string cor = PALETA[c.idx.at(p.nome) % NPAL];
        double w0 = p.B.front() - p.A.front(), w1 = p.B.back() - p.A.back();
        a.desenhar(svg, p.nome + ": largura " + fmt(w0) + " → " + fmt(w1), "iteração k", i == 0 ? "d (cm)" : "",
                   true, 15);
        vector<pair<double, double>> topo, base, area;
        for (size_t k = 0; k < p.A.size(); k++) {
            base.push_back({a.X((double)k), a.Y(p.A[k])});
            topo.push_back({a.X((double)k), a.Y(p.B[k])});
        }
        area = topo;
        for (auto it = base.rbegin(); it != base.rend(); ++it) area.push_back(*it);
        svg.poligono(area, cor, 0.25);
        svg.polilinha(topo, cor, 2); svg.polilinha(base, cor, 2);
        for (auto& q : topo) svg.marcador(0, q.first, q.second, cor, 4);
        for (auto& q : base) svg.marcador(0, q.first, q.second, cor, 4);
        svg.linha(a.L, a.Y(p.raiz), a.L + a.Wd, a.Y(p.raiz), "#111111", 1.5, "2,4");
    }
    svg.texto(90, 580, "Área colorida = intervalo [a_k, b_k] que contém a raiz; linha pontilhada = raiz.",
              13, "start", "#6B7788");
    return svg.salvar(pasta + "/03_intervalos.svg");
}

bool figTeia(const Contexto& c, const string& pasta) {
    vector<double> rs = raizesReais(c.a3, c.a2);
    if (rs.size() != 3) return false;
    const double a3 = c.a3, a2 = c.a2;
    auto phi1 = [&](double x) { return (a3 * x * x * x + 3) / (9 * a2); };
    auto phi2 = [&](double x) { return cbrt((9 * a2 * x - 3) / a3); };
    auto dphi1 = [&](double x) { return 3 * a3 * x * x / (9 * a2); };
    auto dphi2 = [&](double x) { double r = cbrt((9 * a2 * x - 3) / a3); return (9 * a2 / a3) / (3 * r * r + 1e-12); };
    const double rp = rs[1], rg = rs[2];

    Svg svg(1300, 1160);
    svg.texto(90, 36, "Ponto fixo: teia de aranha de φ1 e φ2 (a3=" + fmt(a3) + ", a2=" + fmt(a2) + ")", 20, "start",
              COR_TEXTO, true);
    struct P { int ci, li; bool usa1; double xi, alvo, xmin, xmax; string titulo, onde; };
    vector<P> paineis = {
        {0, 0, true,  rp + 0.15, rp, rp - 0.4, rp + 0.9, "φ1 = (a3·d³ + 3) / (9·a2)", "d0 perto da raiz pequena"},
        {1, 0, true,  rg - 0.05, rg, rp - 0.5, rg + 0.6, "φ1 = (a3·d³ + 3) / (9·a2)", "d0 perto da raiz grande"},
        {0, 1, false, rg - 0.3,  rg, rp - 0.5, rg + 0.6, "φ2 = raiz cúbica de (9·a2·d − 3) / a3", "d0 perto da raiz grande"},
        {1, 1, false, rp + 0.05, rp, rp - 0.4, rp + 0.9, "φ2 = raiz cúbica de (9·a2·d − 3) / a3", "d0 perto da raiz pequena"}};
    int id = 0;
    for (auto& p : paineis) {
        auto phi = [&](double x) { return p.usa1 ? phi1(x) : phi2(x); };
        Area a{90.0 + p.ci * 630, 120.0 + p.li * 510, 500, 400, p.xmin, p.xmax, p.xmin, p.xmax};
        double g = fabs(p.usa1 ? dphi1(p.alvo) : dphi2(p.alvo));
        a.desenhar(svg, p.titulo, "d", "", false, 14,
                   p.onde + " · |φ'(" + fmt(p.alvo, 4) + ")| = " + fmt(g, 3) + (g < 1 ? " → converge" : " → diverge") +
                   (g < 1 ? " (|φ'| < 1)" : " (|φ'| > 1)"));
        svg.inicioClip("c04_" + to_string(id++), a.L, a.T, a.Wd, a.Hd);
        curva(svg, a, phi, p.xmin, p.xmax, PALETA[0], 2.5);
        svg.linha(a.X(p.xmin), a.Y(p.xmin), a.X(p.xmax), a.Y(p.xmax), "#111111", 1.2);
        double cur = p.xi;
        vector<pair<double, double>> pts = {{a.X(cur), a.Y(0)}};
        for (int i = 0; i < 12; i++) {
            double nxt = phi(cur);
            if (!isfinite(nxt) || fabs(nxt) > 50) break;
            pts.push_back({a.X(cur), a.Y(nxt)});
            pts.push_back({a.X(nxt), a.Y(nxt)});
            cur = nxt;
        }
        svg.polilinha(pts, "#D62728", 1.5);
        svg.fimClip();
        svg.marcador(0, a.X(p.alvo), a.Y(p.alvo), "#E8710A", 7);
        svg.caixa(a.L + 8, a.T + 8, 120, 66);
        svg.linha(a.L + 16, a.T + 24, a.L + 40, a.T + 24, PALETA[0], 2.5);   svg.texto(a.L + 46, a.T + 28, "φ(d)", 12);
        svg.linha(a.L + 16, a.T + 42, a.L + 40, a.T + 42, "#111111", 1.2);    svg.texto(a.L + 46, a.T + 46, "y = d", 12);
        svg.linha(a.L + 16, a.T + 60, a.L + 40, a.T + 60, "#D62728", 1.5);    svg.texto(a.L + 46, a.T + 64, "iteração", 12);
    }
    return svg.salvar(pasta + "/04_teia_ponto_fixo.svg");
}

// custo
bool figCusto(const Contexto& c, const string& pasta) {
    struct Painel { string titulo, coluna; };
    vector<Painel> paineis = {{"Iterações", "iteracoes"}, {"Avaliações de f", "aval_f"},
                              {"Avaliações de f'", "aval_df"}, {"Tempo (µs)", "tempo_us"}};
    Svg svg(1500, 880);
    svg.texto(40, 40, "Custo de cada método (a3=" + fmt(c.a3) + ", a2=" + fmt(c.a2) + ", λ=" + fmt(c.lam) + ")",
              22, "start", COR_TEXTO, true);
    int n = (int)c.metodos.size();
    for (size_t p = 0; p < paineis.size(); p++) {
        double px = 40 + (p % 2) * 740, py = 90 + (p / 2) * 390;
        svg.texto(px, py, paineis[p].titulo, 18, "start", COR_TEXTO, true);
        double rotulo = 190, larg = 450, altBarra = 28, passo = 46;
        vector<double> v(n, 0);
        double vmax = 0;
        for (int m = 0; m < n; m++)
            for (size_t i = 0; i < c.res.linhas.size(); i++)
                if (c.res.txt(i, "metodo") == c.metodos[m] && perto(c.res.num(i, "a3"), c.a3) &&
                    perto(c.res.num(i, "a2"), c.a2) && perto(c.res.num(i, "lambda"), c.lam)) {
                    v[m] = c.res.num(i, paineis[p].coluna);
                    break;
                }
        for (double x : v) vmax = max(vmax, isnan(x) ? 0 : x);
        if (vmax <= 0) vmax = 1;
        for (int m = 0; m < n; m++) {
            double y = py + 24 + m * passo;
            int k = c.idx.at(c.metodos[m]);
            svg.texto(px + rotulo - 10, y + 20, c.metodos[m], 14, "end");
            double w = max(2.0, larg * (isnan(v[m]) ? 0 : v[m]) / vmax);
            svg.retangulo(px + rotulo, y, w, altBarra, PALETA[k % NPAL]);
            svg.texto(px + rotulo + w + 8, y + 20, fmt(v[m], 3), 14, "start");
        }
    }
    svg.texto(40, 860, "Tempos em µs variam de uma execução para outra (ruído do sistema); compare só ordens de grandeza.",
              13, "start", "#6B7788");
    return svg.salvar(pasta + "/05_custo.svg");
}

bool figRaizVs(const Contexto& c, const string& pasta, const string& varia) {
    const string outro = (varia == "a3") ? "a2" : "a3";
    const double fixo = (varia == "a3") ? c.a2 : c.a3;
    auto raizesPara = [&](double v) { return varia == "a3" ? raizesReais(v, fixo) : raizesReais(fixo, v); };

    const int N = 120;
    vector<double> vs(N + 1);
    vector<vector<double>> r(N + 1);
    for (int i = 0; i <= N; i++) { vs[i] = 0.3 + 2.7 * i / N; r[i] = raizesPara(vs[i]); }

    map<string, vector<pair<double, double>>> pts;
    vector<string> nomes;
    if (c.temGrade)
        for (size_t i = 0; i < c.grade.linhas.size(); i++) {
            if (!perto(c.grade.num(i, outro), fixo) || c.grade.num(i, "convergiu") < 0.5) continue;
            const string& m = c.grade.txt(i, "metodo");
            if (!c.idx.count(m)) continue;
            double x = c.grade.num(i, varia), y = c.grade.num(i, "raiz");
            if (!isfinite(x) || !isfinite(y)) continue;
            if (!pts.count(m)) nomes.push_back(m);
            pts[m].push_back({x, y});
        }

    double ymin = LIMITE, ymax = LIMITE;
    for (auto& v : r) for (double y : v) { ymin = min(ymin, y); ymax = max(ymax, y); }
    for (auto& kv : pts) for (auto& q : kv.second) { ymin = min(ymin, q.second); ymax = max(ymax, q.second); }
    double mg = (ymax - ymin) * 0.06;

    Svg svg(1620, 620);
    Area a{80, 80, 560, 420, 0.3, 3.1, ymin - mg, ymax + mg};
    Area z{740, 80, 560, 420, 0.3, 3.1, 0.1, 0.6};
    a.desenhar(svg, "Raízes × " + varia + "  (" + outro + " = " + fmt(fixo) + ")", varia, "raiz d (cm)");
    z.desenhar(svg, "Zoom: raiz pequena × limite de 0,3 cm", varia, "raiz d (cm)");

    const char* COR_R[3] = {"#7A8699", "#111111", "#7A8699"};
    const char* DASH_R[3] = {"8,4", "", "2,4"};
    const double LARG_R[3] = {2, 3.5, 2};
    for (Area* ar : {&a, &z}) {
        svg.inicioClip(ar == &a ? "c06a" : "c06z", ar->L, ar->T, ar->Wd, ar->Hd);
        svg.linha(ar->L, ar->Y(LIMITE), ar->L + ar->Wd, ar->Y(LIMITE), "#B54A00", 1.5, "6,5");
        for (int k = 0; k < 3; k++) {
            if (ar == &z && k != 1) continue;
            vector<pair<double, double>> seg;
            for (int i = 0; i <= N; i++) {
                if (r[i].size() == 3) seg.push_back({ar->X(vs[i]), max(-5000.0, min(5000.0, ar->Y(r[i][k])))});
                else { svg.linhaPts(seg, COR_R[k], LARG_R[k], DASH_R[k]); seg.clear(); }
            }
            svg.linhaPts(seg, COR_R[k], LARG_R[k], DASH_R[k]);
        }
        for (const string& m : nomes) {
            int k = c.idx.at(m);
            for (auto& q : pts[m]) svg.marcador(k, ar->X(q.first), ar->Y(q.second), PALETA[k % NPAL], 4.5);
        }
        svg.fimClip();
    }
  
    double lx = 1330, ly = 100;
    auto item = [&](const string& cor, const string& dash, double larg, int forma, const string& txt) {
        svg.linha(lx, ly, lx + 30, ly, cor, larg, dash);
        if (forma >= 0) svg.marcador(forma, lx + 15, ly, cor, 5);
        svg.texto(lx + 40, ly + 5, txt, 14);
        ly += 26;
    };
    item(COR_R[0], DASH_R[0], 2, -1, "raiz negativa");
    item(COR_R[1], DASH_R[1], 3.5, -1, "raiz pequena (pêndulo)");
    item(COR_R[2], DASH_R[2], 2, -1, "raiz grande");
    item("#B54A00", "6,5", 1.5, -1, "limite 0,3 cm");
    for (const string& m : nomes) { int k = c.idx.at(m); item(PALETA[k % NPAL], "", 0.01, k, m); }
    if (!c.temGrade) svg.texto(lx, ly + 20, "(sem grade: rode a opção 3)", 13, "start", "#6B7788");
    svg.texto(80, 600, "Linhas = raízes exatas de f; marcadores = o que cada método achou na grade. "
                       "Acima da linha tracejada, o pêndulo passa de 0,3 cm.", 13, "start", "#6B7788");
    return svg.salvar(pasta + "/06_raiz_vs_" + varia + ".svg");
}

} 

bool gerarFiguras(const string& pastaCsv, const string& pastaFiguras) {
    Contexto c;
    if (!lerCSV(pastaCsv + "/resultados.csv", c.res) || c.res.linhas.empty()) {
        cout << "Nao achei " << pastaCsv << "/resultados.csv: rode a opcao 1 ou 2 antes.\n";
        return false;
    }
    if (!lerCSV(pastaCsv + "/historico.csv", c.hist)) c.hist = Tabela{};
    c.temGrade = lerCSV(pastaCsv + "/resultados_grade.csv", c.grade) && !c.grade.linhas.empty();

    c.a3 = c.res.num(0, "a3"); c.a2 = c.res.num(0, "a2"); c.lam = c.res.num(0, "lambda");

    auto registrar = [&](const Tabela& t) {
        for (size_t i = 0; i < t.linhas.size(); i++) {
            const string& m = t.txt(i, "metodo");
            if (!c.idx.count(m)) { int k = (int)c.idx.size(); c.idx[m] = k; }
        }
    };
    registrar(c.res);
    for (size_t i = 0; i < c.res.linhas.size(); i++) {
        const string& m = c.res.txt(i, "metodo");
        if (perto(c.res.num(i, "a3"), c.a3) && perto(c.res.num(i, "a2"), c.a2) &&
            perto(c.res.num(i, "lambda"), c.lam) &&
            find(c.metodos.begin(), c.metodos.end(), m) == c.metodos.end())
            c.metodos.push_back(m);
    }
    if (c.temGrade) registrar(c.grade);

    error_code ec;
    filesystem::create_directories(pastaFiguras, ec);

    int feitas = 0;
    auto tenta = [&](const string& nome, bool ok) {
        cout << (ok ? "  [ok] " : "  [--] ") << nome << (ok ? "" : "  (sem dados)") << "\n";
        if (ok) feitas++;
    };
    const string rotulo = "(a3=" + fmt(c.a3) + ", a2=" + fmt(c.a2) + ", λ=" + fmt(c.lam) + ")";
    cout << "Gerando graficos (SVG) em " << pastaFiguras << "/\n";
    tenta("01_convergencia_geral.svg", figConvergencia(c, pastaFiguras, c.metodos,
          "Convergência de todos os métodos " + rotulo, "01_convergencia_geral.svg"));
    for (const char* fam : {"Newton", "Intervalo", "Abertos"}) {
        vector<string> ms;
        for (const string& m : c.metodos) if (familia(m) == fam) ms.push_back(m);
        if (ms.empty()) continue;
        string arq = string("01_convergencia_") + (string(fam) == "Newton" ? "newton" :
                     string(fam) == "Intervalo" ? "intervalo" : "abertos") + ".svg";
        tenta(arq, figConvergencia(c, pastaFiguras, ms, string("Convergência – família ") + fam, arq));
    }
    tenta("02_f_raizes.svg", figFuncao(c, pastaFiguras));
    tenta("03_intervalos.svg", figIntervalos(c, pastaFiguras));
    tenta("04_teia_ponto_fixo.svg", figTeia(c, pastaFiguras));
    tenta("05_custo.svg", figCusto(c, pastaFiguras));
    if (c.temGrade) {
        tenta("06_raiz_vs_a3.svg", figRaizVs(c, pastaFiguras, "a3"));
        tenta("06_raiz_vs_a2.svg", figRaizVs(c, pastaFiguras, "a2"));
    } else
        cout << "  (rode a opcao 3 antes para gerar os graficos 06)\n";
    return feitas > 0;
}