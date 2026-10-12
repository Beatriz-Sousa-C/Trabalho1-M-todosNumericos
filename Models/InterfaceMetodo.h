#include <string>
#ifndef TB1_INTERFACEMETODO_H
#define TB1_INTERFACEMETODO_H

class Funcao {
public:
    virtual double avaliar(double x) const = 0;
    virtual double avaliarDerivada(double x) const = 0;
    virtual ~Funcao() = default;
};

struct Parametros {
    double aproximacaoInicial;
    double precisao;
    double lambda;
    double passoDerivada;
    int maximoIteracoes;
};

struct Resultado {
    double raiz{};
    double erro{};
    int iteracoes{};
    bool convergiu{};
    std::string mensagem;
};

//Contrato comum para os métodos numéricos de cálculo de raízes.
class InterfaceMetodo {
    public:
        virtual Resultado calcular(
            const Funcao& funcao,
            const Parametros& parametros
        ) const = 0;

        virtual std::string nome() const = 0;
        virtual ~InterfaceMetodo() = default;
};


#endif //TB1_INTERFACEMETODO_H
