#ifndef TB1_POSICAOFALSA_H
#define TB1_POSICAOFALSA_H

#include "InterfaceMetodo.h"


class PosicaoFalsa : public InterfaceMetodo {
public:
    Resultado calcular(const Funcao& funcao,
                       const Parametros& parametros) const override;
    std::string nome() const override { return "Posicao falsa"; }
};

#endif
