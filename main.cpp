#include <iostream>
#include "Views/Interface.h"
#include "Controllers/CalculoController.h"
using namespace std;

int main() {
    Interface i = Interface();
    cout << "Olá main" << endl;
    CalculoController c;
    Resultado r = c.calcularNewtonRaphson();
    cout << r.raiz <<endl;
}