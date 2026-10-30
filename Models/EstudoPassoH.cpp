#include "EstudoPassoH.h"
#include "derivadaNumerica.h"
#include <cmath>


using namespace std;

vector<PontoEstudoH> estudoPassoH(const Funcao& f, double x,  int expoenteMin, int expoenteMax) {
    vector<PontoEstudoH> pontos;
    double exata = f.avaliarDerivada(x);
    for (int k = expoenteMin; k <= expoenteMax; k++) {
        double h = pow(10.0, -k);                 // h = 10^-k
        double aprox = derivadaNumerica(funcao, x, h);
        pontos.push_back({h, aprox, fabs(aprox - exata)});
    }
    return pontos;
}


