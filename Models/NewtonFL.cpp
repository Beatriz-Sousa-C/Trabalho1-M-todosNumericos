#include "NewtonFL.h"
#include "FuncaoPendulo.h"
#include <cmath>

Resultado NewtonFL::calcular(const Funcao& funcao, const Parametros& parametros) const{

    Resultado resultado{};

    double d = parametros.aproximacaoInicial;
    double epsilon = parametros.precisao;
    double lambda = parametros.lambda;
    int maximoIteracoes = parametros.maximoIteracoes;
    
    resultado.raiz = d;
    resultado.erro = 0.0;
    resultado.iteracoes = 0;
    resultado.convergiu = false;

    if (!std::isfinite(d) || !std::isfinite(lambda) || !std::isfinite(epsilon) || maximoIteracoes<=0 || lambda<=0 || epsilon<=0 ){
        resultado.mensagem = "Parametro invalido identificado!";
        return resultado;
    }

    double FL = 0.0;
    double ultDerivada = 0;
    bool derivadaValida = false;

    resultado.historico.push_back(d);
    for(int i=0; i<maximoIteracoes; i++){
        double valorF = funcao.avaliar(d);
        double derivadaAtual = funcao.avaliarDerivada(d);

        if(!std::isfinite(valorF) || !std::isfinite(derivadaAtual)){
            resultado.mensagem = "Valor nao finito durante o calculo. Nao foi possivel completar a analise.";
            return resultado;
        }

        if (std::abs(derivadaAtual) >= lambda) {
            ultDerivada = derivadaAtual;
            derivadaValida = true;
        }

        if (std::abs(derivadaAtual) > lambda) {
            FL = derivadaAtual;
        } else {
            if (!derivadaValida) {
                resultado.mensagem = "Nao existe derivada valida anterior para reutilizar";
                return resultado;
            }
            FL = ultDerivada;
        }

        if(FL==0 || !std::isfinite(FL)){
            resultado.mensagem = "Denominador invalido (igual a zero ou nao finito)";
            return resultado;
        }

        double xProx = d - (valorF/FL);
        
        if(!std::isfinite(xProx)){
            resultado.mensagem = "Proxima aproximacao eh nao finita";
            return resultado;
        }
        
        resultado.historico.push_back(xProx);
        resultado.raiz = xProx;
        resultado.erro = std::abs(xProx - d);
        resultado.iteracoes = i + 1;

        if(std::abs(xProx-d)<epsilon){
            resultado.convergiu = true;
            resultado.erro = std::abs(xProx-d);
            resultado.mensagem = "Raiz calculada com sucesso!\n";
            if (std::abs(resultado.raiz) > 0.3) {
                resultado.mensagem += "ATENCAO: Deslocamento maior que 0,3 cm!";
            }
            return resultado;
        }

        d = xProx;
    }

    resultado.mensagem = "Limite maximo de iteracoes atingido.";
    return resultado;

}

std::string NewtonFL::nome() const{
    return "Newton com FL";
}