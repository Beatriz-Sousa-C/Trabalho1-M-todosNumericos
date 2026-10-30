#include <iostream>
#include "Views/Interface.h"
#include "Models/FuncaoPendulo.h"
#include "Models/NewtonFL.h"

using namespace std;

int main() {
    Interface i = Interface();
    NewtonRaphson n = NewtonRaphson();
    cout << "Olá main" << endl;

    FuncaoPendulo funcao(1.0, 1.0);

    Parametros parametros;
    parametros.aproximacaoInicial = 0.5;
    parametros.precisao = 0.001;
    parametros.lambda = 0.05;
    parametros.passoDerivada = 0.0001;
    parametros.maximoIteracoes = 100;

    NewtonFL metodo;

    Resultado resultado = metodo.calcular(funcao, parametros);
   
    cout << "Metodo: " << metodo.nome() << endl;
    cout << "Convergiu: " << (resultado.convergiu ? "Sim" : "Nao") << endl;
    cout << "Raiz aproximada: " << resultado.raiz << endl;
    cout << "Erro: " << resultado.erro << endl;
    cout << "Iteracoes: " << resultado.iteracoes << endl;
    cout << "Mensagem: " << resultado.mensagem << endl;
    std::cout << "f(raiz): " << funcao.avaliar(resultado.raiz) << '\n';

    std::cout << "f(0.337609): "
          << funcao.avaliar(0.337609) << '\n';

    std::cout << "f(0.5): "
          << funcao.avaliar(0.5) << '\n';

    return 0;
}

}


