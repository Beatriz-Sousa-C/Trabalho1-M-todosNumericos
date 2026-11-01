#include <iostream>
#include <cmath>
#include <iomanip>
#include <functional>
#include <stdexcept>
#include "InterfaceMetodo.h"
#include "Secante.h"
#include "constantes.h"

using namespace std;


Resultado Secante::calcular(const Funcao& funcao,const Parametros& p) const{
    Resultado r;
    
    double x0 = p.aproximacaoInicial;
    double x1 = p.b;
    double d = p.aproximacaoInicial;
    double epsilon = p.precisao;
    int maximoIteracoes = p.maximoIteracoes;
    
    r.raiz = d;
    r.iteracoes = 0;
    r.convergiu = false;
    r.erro = NAN;

    if(x0==x1){
        r.raiz = x0;
        r.convergiu = false;
        r.mensagem = "Secante precisa de dois pontos distintos.";
        r.iteracoes =0;
        return r;
    }

    if (!std::isfinite(x0) || !std::isfinite(x1) || !std::isfinite(epsilon) || maximoIteracoes<=0 || epsilon<=0 ){
        r.mensagem = "Parametro invalido identificado!";
        return r;
    }

    double f0= funcao.avaliar(x0);
    double f1 = funcao.avaliar(x1);
    r.historico.push_back(x0);
    r.historico.push_back(x1);

    for(int i=0; i<p.maximoIteracoes; i++){
        if (!std::isfinite(f0) || !std::isfinite(f1)) {
            r.raiz = x1;
            r.iteracoes = i;
            r.mensagem = "Valor nao finito durante o calculo.";
            return r;
        }

        double denom = f1-f0;
        if(fabs(denom)<LIMITE_DENOMINADOR){
            r.raiz = x1;
            r.convergiu = false;
            r.iteracoes = i;
            r.mensagem = "O denominador ficou muito pequeno, secante na horizontal.";
            return r;
    }

    // double x2 = (x0*f1 - x1*f0)/denom poderia ser feito assim, mas por ter mais operavoes seria mais perigoso de se perder infrmações devido ao erro acumulado.
    double x2 = x1 - f1 * (x1 - x0) / denom;
    r.historico.push_back(x2);
    if (!std::isfinite(x2)) {
        r.raiz = x1;
        r.iteracoes = i + 1;
        r.mensagem = "Proxima aproximacao eh nao finita.";
        return r;
    }
    r.erro = fabs(x2 - x1);

    if( r.erro<p.precisao){
        r.raiz = x2;
        r.iteracoes = i + 1;
        r.convergiu = true;
        r.mensagem = "Convergiu";
        if (fabs(r.raiz) > DESLOCAMENTO_LIMITE) {
            r.mensagem += "\nATENCAO: Deslocamento maior que 0,3 cm!";
        }
        return r;
    }

    x0 = x1;
    f0 = f1;
    x1 = x2;
    f1 = funcao.avaliar(x1);


    }   
    r.raiz = x1;
    r.iteracoes = p.maximoIteracoes;
    r.convergiu = false;
    r.mensagem = "Maximo de iteracoes atingido.";
    return r;
}
string Secante::nome() const {
    return "Secante";
}


