#include "Controllers/CalculoController.h"

int main() {
    CalculoController controller;

    controller.rodarCombinacao(1.0, 1.0, 0.05, 0.5, 0.001, 100);
    controller.rodarCombinacao(2.0, 1.0, 0.05, 0.5, 0.001, 100);
    controller.rodarCombinacao(1.0, 0.5, 0.05, 0.5, 0.001, 100);

    controller.apresentarResultados();
    controller.estudarPassoH(1.0, 1.0, 0.5);
    return 0;
}
