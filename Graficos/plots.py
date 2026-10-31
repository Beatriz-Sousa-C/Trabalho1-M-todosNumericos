#!/usr/bin/env python3
import os
import re
import sys

import numpy as np
import pandas as pd
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator

PASTA = sys.argv[1] if len(sys.argv) > 1 else "."
FIGS = sys.argv[2] if len(sys.argv) > 2 else "figuras"
LIMITE = 0.3  # o pêndulo rompe se d > 0,3 cm
os.makedirs(FIGS, exist_ok=True)


def ler_csv(nome, obrigatorio=True):
    caminho = os.path.join(PASTA, nome)
    if not os.path.exists(caminho):
        if obrigatorio:
            sys.exit(f"Arquivo {caminho} nao encontrado. Rode o programa (opcao 1 ou 2) antes.")
        return None
    return pd.read_csv(caminho)


def raizes_reais(a3, a2):
    """Raízes reais de a3 d^3 - 9 a2 d + 3 (referência para medir o erro)."""
    r = np.roots([a3, 0, -9 * a2, 3])
    return np.sort(r[np.abs(r.imag) < 1e-9].real)


def f(d, a3, a2):
    return a3 * d**3 - 9 * a2 * d + 3


def familia(nome):
    """Família do método, deduzida do nome (não dependemos de como cada colega escreveu)."""
    n = nome.lower()
    if "bisse" in n or "falsa" in n:
        return "Intervalo"
    if "newton" in n:
        return "Newton"
    return "Abertos"  # secante, ponto fixo...


PALETA = ["#1f77b4", "#ff7f0e", "#2ca02c", "#9467bd", "#d62728", "#8c564b", "#17becf", "#7f7f7f", "#e377c2"]
MARCAS = ["o", "s", "^", "D", "v", "P", "X", "*", "h"]
_estilos = {}


def estilo(metodo):
    """Cor e marcador fixos por método (mesmo método = mesma aparência em todos os gráficos)."""
    if metodo not in _estilos:
        k = len(_estilos) % len(PALETA)
        _estilos[metodo] = (PALETA[k], MARCAS[k])
    return _estilos[metodo]


def salvar(fig, nome):
    caminho = os.path.join(FIGS, nome)
    fig.tight_layout()
    fig.savefig(caminho, dpi=160)
    plt.close(fig)
    print("  gerado", caminho)


res_all = ler_csv("resultados.csv")
hist_all = ler_csv("historico.csv")
grade = ler_csv("resultados_grade.csv", obrigatorio=False)

for m in res_all.metodo.unique():  # fixa o estilo na ordem de aparição
    estilo(m)

if len(sys.argv) >= 6:
    a3, a2, lam = float(sys.argv[3]), float(sys.argv[4]), float(sys.argv[5])
else:
    a3, a2, lam = (float(res_all.a3.iloc[0]), float(res_all.a2.iloc[0]), float(res_all["lambda"].iloc[0]))


def filtrar(df):
    return df[np.isclose(df.a3, a3) & np.isclose(df.a2, a2) & np.isclose(df["lambda"], lam)]


res = filtrar(res_all)
hist = filtrar(hist_all)
if res.empty:
    sys.exit(f"Combinacao a3={a3}, a2={a2}, lambda={lam} nao existe em resultados.csv")
raizes = raizes_reais(a3, a2)
rotulo = f"a3={a3:g}, a2={a2:g}, λ={lam:g}"
print(f"Combinação: {rotulo}; raízes de referência = {np.round(raizes, 6)}")


def serie(metodo):
    """Histórico x_k do método (só valores finitos)."""
    d = hist[hist.metodo == metodo].sort_values("k")
    d = d[np.isfinite(d.x_k)]
    return d.k.to_numpy(), d.x_k.to_numpy()


# 1) convergência
def erro_serie(xs):
    """|x_k - r|, com r = raiz de referência mais próxima do último x_k.
    (Não usamos a coluna 'erro' do CSV: ela significa coisas diferentes em cada método.)"""
    r = raizes[np.argmin(np.abs(raizes - xs[-1]))]
    return np.maximum(np.abs(xs - r), 1e-17)  # piso para o log não explodir em 0


def plot_convergencia(metodos, titulo, nome):
    fig, ax = plt.subplots(figsize=(7, 4.4))
    desenhou = False
    for m in metodos:
        k, xs = serie(m)
        if len(xs) == 0:
            continue
        c, mk = estilo(m)
        ax.semilogy(k, erro_serie(xs), marker=mk, color=c, label=m, lw=1.6, ms=6)
        desenhou = True
    if not desenhou:
        plt.close(fig)
        return
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel("iteração k")
    ax.set_ylabel(r"erro $|x_k - r|$ (escala log)")
    ax.set_title(titulo)
    ax.grid(True, which="both", alpha=0.3)
    ax.legend(fontsize=8)
    salvar(fig, nome)


plot_convergencia(list(res.metodo), f"Convergência de todos os métodos ({rotulo})", "01_convergencia_geral.png")
for fam in ["Newton", "Intervalo", "Abertos"]:
    ms = [m for m in res.metodo if familia(m) == fam]
    if ms:
        plot_convergencia(ms, f"Convergência – família {fam}", f"01_convergencia_{fam.lower()}.png")

# 2) f(d) com as raízes
fig, ax = plt.subplots(figsize=(7, 4.4))
d = np.linspace(raizes.min() - 0.8, raizes.max() + 0.8, 600)
ax.plot(d, f(d, a3, a2), color="#1f77b4", lw=2, label=r"$f(d)=a_3d^3-9a_2d+3$")
ax.axhline(0, color="k", lw=0.8)
ax.axvline(LIMITE, color="#d62728", ls="--", lw=1.5, label="limite d = 0,3 cm")
for i, r in enumerate(raizes):
    ax.plot(r, 0, "o", color="#ff7f0e", ms=8, zorder=5, label="raízes" if i == 0 else None)
    ax.annotate(f"{r:.3f}", (r, 0), textcoords="offset points", xytext=(0, 10), ha="center", fontsize=9)
ax.set_ylim(-25, 25)
ax.set_xlabel("d (cm)")
ax.set_ylabel("f(d)")
ax.set_title(f"f(d) e suas raízes ({rotulo})")
ax.grid(alpha=0.3)
ax.legend(fontsize=8)
salvar(fig, "02_f_raizes.png")


# 3) intervalos encolhendo
def reconstruir_intervalos(row, xs):
    """Bisseção e posição falsa não guardam [a,b]: refazemos a partir do isolamento inicial
    (coluna 'isolamento', ex.: "[0.33;0.34]") e do sinal de f em cada x_k do histórico."""
    m = re.match(r"\[\s*([-\d.eE+]+)\s*;\s*([-\d.eE+]+)\s*\]", str(row.isolamento))
    if not m:
        return None
    a, b = float(m.group(1)), float(m.group(2))
    fa = f(a, a3, a2)
    out = [(a, b)]
    for x in xs:
        fx = f(x, a3, a2)
        if fx == 0:
            a = b = x
        elif (fa > 0) == (fx > 0):
            a, fa = x, fx
        else:
            b = x
        out.append((a, b))
    return np.array(out)


alvos = [(r.metodo, r) for r in res.itertuples() if familia(r.metodo) == "Intervalo"]
if alvos:
    fig, axs = plt.subplots(1, len(alvos), figsize=(5 * len(alvos), 4), sharey=True, squeeze=False)
    for ax, (m, row) in zip(axs[0], alvos):
        _, xs = serie(m)
        iv = reconstruir_intervalos(row, xs)
        if iv is None or len(xs) == 0:
            ax.set_title(f"{m}: sem dados")
            continue
        c, _ = estilo(m)
        k = np.arange(len(iv))
        ax.fill_between(k, iv[:, 0], iv[:, 1], color=c, alpha=0.25, label="intervalo [a_k, b_k]")
        ax.plot(k, iv[:, 0], color=c, marker="o", ms=4)
        ax.plot(k, iv[:, 1], color=c, marker="o", ms=4)
        ax.axhline(raizes[np.argmin(np.abs(raizes - xs[-1]))], color="k", ls=":", label="raiz")
        ax.set_title(f"{m}: largura {iv[0,1]-iv[0,0]:.3g} → {iv[-1,1]-iv[-1,0]:.3g}", fontsize=10)
        ax.set_xlabel("iteração k")
        ax.xaxis.set_major_locator(MaxNLocator(integer=True))
        ax.grid(alpha=0.3)
        ax.legend(fontsize=8)
    axs[0][0].set_ylabel("d (cm)")
    salvar(fig, "03_intervalos.png")

# 4) teia de aranha (ponto fixo)
phi1 = lambda x: (a3 * x**3 + 3) / (9 * a2)
phi2 = lambda x: np.cbrt((9 * a2 * x - 3) / a3)
dphi1 = lambda x: 3 * a3 * x**2 / (9 * a2)
dphi2 = lambda x: (9 * a2 / a3) / (3 * np.cbrt((9 * a2 * x - 3) / a3) ** 2 + 1e-12)


def teia(ax, phi, dphi, xi, alvo, titulo, xmin, xmax, n=12):
    x = np.linspace(xmin, xmax, 400)
    ax.plot(x, phi(x), color="#1f77b4", label=r"$\varphi(d)$")
    ax.plot(x, x, color="k", lw=1, label="y = d")
    cur, pts = xi, [(xi, 0)]
    for _ in range(n):
        nxt = phi(cur)
        if not np.isfinite(nxt) or abs(nxt) > 50:
            break
        pts += [(cur, nxt), (nxt, nxt)]
        cur = nxt
    px, py = zip(*pts)
    ax.plot(px, py, color="#d62728", lw=1, marker=".", ms=3, label="iteração")
    ax.plot(alvo, alvo, "o", color="#ff7f0e", ms=8, zorder=5)
    g = abs(dphi(alvo))
    ax.set_title(f"{titulo}\n|φ'({alvo:.3f})| = {g:.2f} → " + ("converge (|φ'| < 1)" if g < 1 else "diverge (|φ'| > 1)"), fontsize=9)
    ax.set_xlim(xmin, xmax)
    ax.set_ylim(xmin, xmax)
    ax.set_xlabel("d")
    ax.grid(alpha=0.3)
    ax.legend(fontsize=7, loc="upper left")


if len(raizes) == 3:
    r_neg, r_peq, r_gde = raizes
    fig, axs = plt.subplots(2, 2, figsize=(9, 8))
    teia(axs[0, 0], phi1, dphi1, r_peq + 0.15, r_peq, r"$\varphi_1=(a_3d^3+3)/(9a_2)$, d0 perto da raiz pequena", r_peq - 0.4, r_peq + 0.9)
    teia(axs[0, 1], phi1, dphi1, r_gde - 0.05, r_gde, r"$\varphi_1$, d0 perto da raiz grande", r_peq - 0.5, r_gde + 0.6)
    teia(axs[1, 0], phi2, dphi2, r_gde - 0.3, r_gde, r"$\varphi_2=\sqrt[3]{(9a_2d-3)/a_3}$, d0 perto da raiz grande", r_peq - 0.5, r_gde + 0.6)
    teia(axs[1, 1], phi2, dphi2, r_peq + 0.05, r_peq, r"$\varphi_2$, d0 perto da raiz pequena", r_peq - 0.4, r_peq + 0.9)
    salvar(fig, "04_teia_ponto_fixo.png")

# 5) barras de custo
cols = [("iteracoes", "Iterações"), ("aval_f", "Avaliações de f"), ("aval_df", "Avaliações de f'"), ("tempo_us", "Tempo (µs)")]
fig, axs = plt.subplots(1, 4, figsize=(15, 4))
for ax, (col, tit) in zip(axs, cols):
    ax.barh(res.metodo, res[col], color=[estilo(m)[0] for m in res.metodo])
    ax.invert_yaxis()
    ax.set_title(tit)
    ax.grid(axis="x", alpha=0.3)
    for y, v in enumerate(res[col]):
        ax.text(v, y, f" {v:.3g}", va="center", fontsize=8)
for ax in axs[1:]:
    ax.set_yticklabels([])
salvar(fig, "05_custo.png")


# 6) raiz × a3 e raiz × a2 (análise g)
def raiz_vs(param, valores, fixo, nome):
    """Esquerda: as 3 raízes. Direita: zoom na raiz pequena contra o limite de 0,3 cm."""
    fig, (ax, az) = plt.subplots(1, 2, figsize=(11, 4.4))
    curvas = [raizes_reais(v, fixo) if param == "a3" else raizes_reais(fixo, v) for v in valores]
    for k, rot in enumerate(["raiz negativa", "raiz pequena (pêndulo)", "raiz grande"]):
        ys = [c[k] if len(c) == 3 else np.nan for c in curvas]
        ax.plot(valores, ys, lw=2, label=rot)
        if k == 1:
            az.plot(valores, ys, lw=2, color="#ff7f0e", label=rot)
    for a_ in (ax, az):
        a_.axhline(LIMITE, color="#d62728", ls="--", label="limite 0,3 cm")
    if grade is not None:  # sobrepõe o que os métodos realmente acharam
        outro = "a2" if param == "a3" else "a3"
        g = grade[np.isclose(grade[outro], fixo) & (grade.convergiu == 1)]
        for m, gm in g.groupby("metodo"):
            c, mk = estilo(m)
            for a_ in (ax, az):
                a_.scatter(gm[param], gm.raiz, color=c, marker=mk, s=28, alpha=0.8, label=m if a_ is ax else None, zorder=5)
    outro = "a2" if param == "a3" else "a3"
    ax.set_title(f"Raízes × {param} ({outro} = {fixo:g})")
    az.set_title("Zoom: raiz pequena vs limite 0,3 cm")
    az.set_ylim(0.1, 0.6)
    for a_ in (ax, az):
        a_.set_xlabel(param)
        a_.set_ylabel("raiz d")
        a_.grid(alpha=0.3)
    ax.legend(fontsize=7, ncol=2)
    salvar(fig, nome)


valores = np.linspace(0.3, 3, 120)
raiz_vs("a3", valores, a2, "06_raiz_vs_a3.png")
raiz_vs("a2", valores, a3, "06_raiz_vs_a2.png")
print("Pronto.")
