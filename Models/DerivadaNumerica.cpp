#include "DerivadaNumerica.h"
#include <algorithm>
#include <cmath>


double derivadaNumerica(const Funcao& f, double x, double h) {
    double passo = h * std::max(1.0, std::fabs(x));
    double xMais = x + passo;
    double xMenos = x - passo;
    return (f.avaliar(xMais) - f.avaliar(xMenos)) / (xMais - xMenos);
}
