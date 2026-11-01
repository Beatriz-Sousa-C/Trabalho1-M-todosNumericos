#include "FuncHorner.h"
#include <stdexcept>
#include <utility>

using namespace std;

FuncHorner::FuncHorner(double a3, double a2)
    : coef_{a3, 0.0, -9.0 * a2, 3.0} {}

FuncHorner::FuncHorner(vector<double> coeficientes)
    : coef_(move(coeficientes)) {
    if (coef_.empty())
        throw invalid_argument("FuncHorner: polinômio sem coeficientes");
}


void FuncHorner::avaliarComDerivada(double x, double& p, double& dp) const {
    passadasHorner_++;
    p = coef_[0];
    dp = 0.0;
    for (size_t i = 1; i < coef_.size(); i++) {
        dp = dp * x + p;
        p  = p  * x + coef_[i];
    }
}

double FuncHorner::avaliar(double x) const {
    double p = coef_[0];
    for (size_t i = 1; i < coef_.size(); i++) p = p * x + coef_[i];
    return p;
}

double FuncHorner::avaliarDerivada(double x) const {
    double p, dp;
    avaliarComDerivada(x, p, dp);
    return dp;
}


double FuncHorner::briotRuffini(double r, vector<double>& quociente) const {
    quociente.clear();
    double b = coef_[0];
    for (size_t i = 1; i < coef_.size(); i++) {
        quociente.push_back(b);
        b = b * r + coef_[i];
    }
    return b;  // resto
}