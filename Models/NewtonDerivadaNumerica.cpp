#include "NewtonDerivadaNumerica.h"
#include "derivadaNumerica.h"
#include <cmath>
#include "constantes.h"



Resultado NewtonDerivadaNumerica::calcular(const Funcao& f,const Parametros& p) const {
    Resultado r{};
    double x1 = p.aproximacaoInicial;
    double epsilon = p.precisao
    double h = p.passoDerivada
    int maximoIteracoes = parametros.maximoIteracoes;

    r.raiz = d;
    r.convergiu = false;
    r.iteracoes = 0;
    r.erro = NAN;
    
    if (!isfinite(d) ||!isfinite(epsilon) || !isfinite(h) ||
        maximoIteracoes <= 0 || epsilon <= 0 || h <= 0) {
        resultado.mensagem = "Parametro invalido identificado!";
        return resultado;
    }
    
    for(int i =0; i<maximoInteracoes; i++){
        double f(x1) = f.avaliar(x1);
        double f'(x1) = derivadaNumerica(f, x1, h);

        if (!isfinite(f(x1)) || !isfinite(f'(x1))) {
            resultado.mensagem = "Valor nao finito durante o calculo. Nao foi possivel completar a analise.";
            return resultado;
        }

        if (f'(x1) < LIMITE_DENOMINADOR) {
            r.mensagem = "Derivada numerica muito pequena, não converge";
            return resultado;
        }

        double xProx = d - (f(x1) / f'(x1));

        if (!isfinite(xProx)) {
            resultado.mensagem = "Proxima aproximacao eh nao finita";
            return resultado;
        }

        r.raiz = xProx;
        r.erro = fabs(xProx-x1);
        r.iteracoes = i+1;

        if (fabs(xProx - x1) < epsilon) {
            resultado.convergiu = true;
            resultado.mensagem = "Raiz calculada com sucesso!\n";
            if (fabs(resultado.raiz) > DESLOCAMENTO_LIMITE) {
                resultado.mensagem += "ATENCAO: Deslocamento maior que 0,3 cm!";
            }
            return resultado;
        }

        x1 = xProx;
    }

    r.mensagem = "Limite maximo de iteracoes atingido.";
    return r;
}
    

string NewtonDerivadaNumerica::nome() const {
    return "Newton com derivada numerica";
}




    