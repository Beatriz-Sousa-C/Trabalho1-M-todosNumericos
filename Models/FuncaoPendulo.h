#ifndef FUNCAO_PENDULO_H
#define FUNCAO_PENDULO_H
#include "InterfaceMetodo.h"

class FuncaoPendulo:
    public Funcao{
        private:
            double a3;
            double a2;

        public:
            FuncaoPendulo(double a3, double a2);
            double avaliar(double d) const override;
            double avaliarDerivada(double d) const override;
    };

#endif