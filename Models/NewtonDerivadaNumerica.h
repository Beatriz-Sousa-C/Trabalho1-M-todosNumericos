#ifndef TB1_NEWTONDERIVADANUMERICA_H
#define TB1_NEWTONDERIVADANUMERICA_H

#include "InterfaceMetodo.h"

class NewtonDerivadaNumerica : public InterfaceMetodo {
public:
    Resultado calcular(const Funcao& funcao,
                       const Parametros& parametros) const override;
    std::string nome() const override;
};

#endif // TB1_NEWTONDERIVADANUMERICA_Hs