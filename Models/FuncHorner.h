#ifndef TB1_FUNCHORNER_H
#define TB1_FUNCHORNER_H

#include <vector>
#include "InterfaceMetodo.h"
using namespace std;


class FuncHorner : public Funcao {
public:
    FuncHorner(double a3, double a2);
    explicit FuncHorner(vector<double> coeficientes);

    double avaliar(double x) const override; // p(x)
    double avaliarDerivada(double x) const override; // p'(x)

    void avaliarComDerivada(double x, double& p, double& dp) const;

 
    double briotRuffini(double r, vector<double>& quociente) const;

    const vector<double>& coeficientes() const { return coef_; }

    int passadasHorner() const { return passadasHorner_; }

private:
    vector<double> coef_;
    mutable int passadasHorner_ = 0;
};

#endif