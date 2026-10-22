#ifndef TB1_ESTUDOPASSOH_H
#define TB1_ESTUDOPASSOH_H

#include "InterfaceMetodo.h"
#include <vector>

struct PontoEstudoH {
    double h;
    double derivadaAproximada;
    double erro;
};

std::vector<PontoEstudoH> estudoPassoH(const Funcao& funcao, double x, int expoenteMin = 1, int expoenteMax = 12);

#endif // TB1_ESTUDOPASSOH_H