
#ifndef TB1_CALCULOCONTROLLER_H
#define TB1_CALCULOCONTROLLER_H
#include <vector>
#include "../Models/InterfaceMetodo.h"
#include "../Models/FuncaoPendulo.h"
#include "../Models/NewtonFL.h"
#include "../Models/NewtonRaphson.h"
#include "../Views/Quadros.h"

class CalculoController {
    public:
        Resultado calcularNewtonRaphson(double aproximacaoInicial,double precisao,double a3,double a2);

        Resultado calcularNewtonFL(double aproximacaoInicial,double precisao,double lambda,double a3,double a2);

        Resultado calcularNewtonNumerico(double aproximacaoInicial,double precisao,double passoDerivada,double a3,double a2);

        void rodarCombinacao(double a3, double a2, double lambda, double d0, double precisao, int maxIter);

        void estudarPassoH(double a3, double a2, double x);

        void apresentarResultados();

    private:
        std::vector<LinhaQuadro> linhas;
};
#endif //TB1_CALCULOCONTROLLER_H
