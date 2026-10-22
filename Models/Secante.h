#ifndef TB1_SECANTE_H
#define TB1_SECANTE_H

#include "InterfaceMetodo.h"
using namespace std;

class Secante : public InterfaceMetodo {
public:
    Resultado calcular(const Funcao& funcao, const Parametros& parametros) const override;

    string nome() const override;
};

#endif // TB1_SECANTE_H