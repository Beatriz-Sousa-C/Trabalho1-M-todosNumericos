#include "Views/Interface.h"

// Toda a lógica de entrada, execução dos métodos e saída agora está em
// Views/Interface.cpp (menu). A antiga rodarCombinacao() foi movida para lá.
int main() {
    Interface ui;
    ui.executar();
    return 0;
}
