#ifndef TB1_BISSECAO_H
#define TB1_BISSECAO_H

#include "InterfaceMetodo.h"

class Bissecao : public InterfaceMetodo {
public:
    Resultado calcular(const Funcao& funcao,
                       const Parametros& parametros) const override;
    std::string nome() const override;
};

#endif // TB1_BISSECAO_H