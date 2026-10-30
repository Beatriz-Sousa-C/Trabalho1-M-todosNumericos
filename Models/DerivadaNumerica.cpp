#include "DerivadaNumerica.h"

double derivadaNumerica(const Funcao& f, double x, double h) {
    return (f.avaliar(x + h) - f.avaliar(x - h)) / (2.0 * h);
}