#ifndef TB1_ANALISEG_H
#define TB1_ANALISEG_H

#include <string>
#include <utility>
#include <vector>
#include "InterfaceMetodo.h"

/* Item g. roda os métodos numa grade de (a3, a2) e imprime uma tabela
 no terminal com raiz, iterações, se convergiu e se passou de 0,3 cm */

// isolamento por varredura (Bolzano): subintervalos de tamanho passo com troca de sinal.
// Duas raizes a menos de um passo uma da outra nao trocam o sinal e passam despercebidas.
std::vector<std::pair<double, double>>
isolarRaizes(const Funcao& f, double xMin, double xMax, double passo);

// texto para quando isolarRaizes nao acha intervalo
std::string mensagemSemIntervalo(double xMin, double xMax, double passo);

// a posição falsa é montada aqui com o intervalo da menor raiz positiva
void executarLote(const std::vector<double>& valoresA3,
                  const std::vector<double>& valoresA2,
                  const Parametros& par,
                  const std::vector<const InterfaceMetodo*>& metodosAbertos,
                  double limite = 0.3);

#endif
