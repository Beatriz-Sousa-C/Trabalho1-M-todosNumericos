#ifndef TB1_SECANTE_H
#define TB1_SECANTE_H

#include "InterfaceMetodo.h"

// Parametros: aproximacaoInicial = x0, b = x1 (duas aproximacoes iniciais distintas)
class Secante : public InterfaceMetodo {
public:
    Resultado calcular(const Funcao& funcao, const Parametros& parametros) const override;
    std::string nome() const override;
};

#endif // TB1_SECANTE_H
