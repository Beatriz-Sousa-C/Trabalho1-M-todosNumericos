#ifndef TB1_POSICAOFALSA_H
#define TB1_POSICAOFALSA_H

#include "InterfaceMetodo.h"

class PosicaoFalsa : public InterfaceMetodo {
public:
    PosicaoFalsa(double a, double b) : a_(a), b_(b) {}

    Resultado calcular(const Funcao& funcao,
                       const Parametros& parametros) const override;
    std::string nome() const override { return "Posicao falsa"; }

private:
    double a_, b_;
};

#endif