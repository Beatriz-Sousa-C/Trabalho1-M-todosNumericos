#include "FuncaoPendulo.h"

FuncaoPendulo::FuncaoPendulo(double a3, double a2): a3(a3), a2(a2){}

double FuncaoPendulo::avaliar(double d) const {
    return a3*d*d*d-9*a2*d+3;
}

double FuncaoPendulo::avaliarDerivada(double d) const{
    return 3*a3*d*d-9*a2;
}