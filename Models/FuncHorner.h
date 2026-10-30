#ifndef TB1_FUNCHORNER_H
#define TB1_FUNCHORNER_H

#include <vector>
#include "InterfaceMetodo.h"
using namespace std;

/* essa é a função do pêndulo p(d) = a3·d³+0·d²−9·a2·d+3 avaliada pelo
método de Horner. Coeficientes do maior para o menor grau: [a3, 0, −9·a2, 3] */
class FuncHorner : public Funcao {
public:
    FuncHorner(double a3, double a2);
    explicit FuncHorner(vector<double> coeficientes);

    double avaliar(double x) const override; // p(x)
    double avaliarDerivada(double x) const override; // p'(x)

    // Uma única passada do Horner retorna p(x) e p'(x) juntos
    void avaliarComDerivada(double x, double& p, double& dp) const;

    /*divide p(x) por (x−r), preenche o quociente e
     devolve o resto (p(r)). Se r é raiz então o resto fica perto de 0*/
    double briotRuffini(double r, vector<double>& quociente) const;

    const vector<double>& coeficientes() const { return coef_; }

private:
    vector<double> coef_;
};

#endif