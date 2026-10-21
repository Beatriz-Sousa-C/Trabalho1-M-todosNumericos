#include <iostream>
#include "Views/Interface.h"
#include "Models/NewtonRaphson.h"
#include "Controllers/item_a.h"
using namespace std;

int main() {
    Interface i = Interface();
    NewtonRaphson n = NewtonRaphson();
    cout << "Olá main" << endl;
    cout << n.nome() << endl;
    cout << raiz_quadrada(16) << endl;
}