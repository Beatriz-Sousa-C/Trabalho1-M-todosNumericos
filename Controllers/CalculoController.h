
#ifndef TB1_CALCULOCONTROLLER_H
#define TB1_CALCULOCONTROLLER_H
#include "../Models/InterfaceMetodo.h"
#include "../Models/FuncaoPendulo.h"
#include "../Models/NewtonFL.h"
#include "../Models/NewtonRaphson.h"

class CalculoController {
    public:
        Resultado calcularNewtonRaphson();

        Resultado calcularNewtonFL();

        Resultado calcularNewtonNumerico();
};
#endif //TB1_CALCULOCONTROLLER_H
