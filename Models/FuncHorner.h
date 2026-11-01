#ifndef TB1_FUNCHORNER_H
#define TB1_FUNCHORNER_H

#include <vector>
#include "InterfaceMetodo.h"


class FuncHorner : public Funcao {
public:
    FuncHorner(double a3, double a2);
    explicit FuncHorner(std::vector<double> coeficientes);

    double avaliar(double x) const override; // p(x)
    double avaliarDerivada(double x) const override; // p'(x)

    void avaliarComDerivada(double x, double& p, double& dp) const;

 
    double briotRuffini(double r, std::vector<double>& quociente) const;

    const std::vector<double>& coeficientes() const { return coef_; }

    int passadasHorner() const { return passadasHorner_; }

private:
    std::vector<double> coef_;
    mutable int passadasHorner_ = 0;
};

#endif