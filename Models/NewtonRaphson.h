//
// Created by arthur on 20/10/2026.
//

#ifndef TRABALHO1_M_TODOSNUMERICOS_NEWTONRAPHSON_H
#define TRABALHO1_M_TODOSNUMERICOS_NEWTONRAPHSON_H
#include "InterfaceMetodo.h"
class NewtonRaphson : public InterfaceMetodo {
    public:
        Resultado calcular(const Funcao &funcao, const Parametros &parametros) const override;
        std::string nome() const override;
};
#endif //TRABALHO1_M_TODOSNUMERICOS_NEWTONRAPHSON_H
