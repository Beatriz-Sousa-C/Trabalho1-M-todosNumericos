#ifndef TB1_NEWTONHORNER_H
#define TB1_NEWTONHORNER_H

#include "InterfaceMetodo.h"

/* Newton com Horner mesma fórmula do Newton original,
x_{k+1} = x_k − p(x_k)/p'(x_k), mas p e p' saem de uma única iteração
do Horner, deve dar a mesma raiz do Newton original mas com menos contas. */
class NewtonHorner : public InterfaceMetodo {
public:
    Resultado calcular(const Funcao& funcao,
                       const Parametros& parametros) const override;
    std::string nome() const override { return "Newton com Horner"; }
};

#endif