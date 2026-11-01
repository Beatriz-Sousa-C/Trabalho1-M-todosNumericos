#ifndef TB1_CONSTANTES_H
#define TB1_CONSTANTES_H

#include "InterfaceMetodo.h"

const double LIMITE_DENOMINADOR = 1e-12;
const double DESLOCAMENTO_LIMITE = 0.3;
const double PASSO_ISOLAMENTO = 0.5;

// d e um deslocamento: raiz <= 0 nao tem sentido fisico e nao conta como "passou de 0,3 cm"
inline bool semSentidoFisico(const Resultado& r) {
    return r.convergiu && r.raiz <= 0;
}

inline bool passouLimite(const Resultado& r) {
    return r.convergiu && r.raiz > DESLOCAMENTO_LIMITE;
}

inline void avisarDeslocamento(Resultado& r) {
    if (r.raiz <= 0)
        r.mensagem += "\nATENCAO: raiz d <= 0, sem sentido fisico para o pendulo.";
    else if (r.raiz > DESLOCAMENTO_LIMITE)
        r.mensagem += "\nATENCAO: Deslocamento maior que 0,3 cm!";
}

#endif
