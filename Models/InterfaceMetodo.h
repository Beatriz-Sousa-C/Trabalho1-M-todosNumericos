#include <string>
#include <cmath>
#ifndef TB1_INTERFACEMETODO_H

#define TB1_INTERFACEMETODO_H

using namespace std;

class Funcao {
public:
    virtual double avaliar(double x) const = 0;
    virtual double avaliarDerivada(double x) const = 0;
    virtual ~Funcao() = default;
};

class Parametros {
public:
    double aproximacaoInicial; // ou a que é o inicio do intervalo
    double precisao;
    double lambda;
    double passoDerivada;
    int maximoIteracoes;
    double b; // Fim do intervalo

    Parametros(double a, double eps, double lam, double h, int maxIter,
               double b = NAN)
        : aproximacaoInicial(a), precisao(eps), lambda(lam),
          passoDerivada(h), maximoIteracoes(maxIter),
          b(std::isnan(b) ? a : b)
    {}

};

struct Resultado {
    double raiz{};
    double erro{};
    int iteracoes{};
    bool convergiu{};
    string mensagem;
};

//Contrato comum para os métodos numéricos de cálculo de raízes.
class InterfaceMetodo {
    public:
        virtual Resultado calcular(
            const Funcao& funcao,
            const Parametros& parametros
        ) const = 0;

        virtual string nome() const = 0;
        virtual ~InterfaceMetodo() = default;
};


#endif //TB1_INTERFACEMETODO_H
