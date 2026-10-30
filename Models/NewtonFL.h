#ifndef NEWTON_FL_H
#define NEWTON_FL_H
#include "InterfaceMetodo.h"

class NewtonFL:
    public InterfaceMetodo{
        public:
        Resultado calcular(const Funcao& funcao, const Parametros& parametros) const override;
        std:: string nome() const override;
    };

#endif