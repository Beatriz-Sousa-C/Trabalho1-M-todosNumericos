#ifndef TB1_INTERFACE_H
#define TB1_INTERFACE_H

#include <iostream>
#include <string>
#include <vector>
#include "Quadros.h"

class Interface {
public:
    Interface();
    void executar();   // laço do menu principal

private:
    void novaExecucao();      // pergunta n, λ, a3, a2, e... e roda
    void casoPadrao();        // a3=1, a2=1, d0=0,5, λ=0,05, e=0,001
    void analiseGrade();      // grade a3 x a2 (alimenta os gráficos do item g)
    void gerarGraficos();     // chama o script Python
    void mostrarEExportar(const std::vector<LinhaQuadro>& linhas);
};

#endif //TB1_INTERFACE_H
