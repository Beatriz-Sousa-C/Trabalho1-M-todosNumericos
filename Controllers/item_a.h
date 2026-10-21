#ifndef TRABALHO1_M_TODOSNUMERICOS_ITEM_A_H
#define TRABALHO1_M_TODOSNUMERICOS_ITEM_A_H
#include "../Models/NewtonRaphson.h"
class FuncaoRaiz: public Funcao {
public:
    double avaliar(double x) const override;
    double avaliarDerivada(double x_raiz) const override;
};

double raiz_quadrada(double x);
#endif //TRABALHO1_M_TODOSNUMERICOS_ITEM_A_H
