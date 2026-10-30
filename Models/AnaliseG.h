#ifndef TB1_ANALISEG_H
#define TB1_ANALISEG_H

#include <bits/stdc++.h>
#include "InterfaceMetodo.h"
using namespace std;

/* Item g. roda os métodos numa grade de (a3, a2) e imprime uma tabela
 no terminal com raiz, iterações, se convergiu e se passou de 0,3 cm */

// isolamento por varredura (Bolzano). PROVISÓRIO: trocar pela versão do P1.
vector<std::pair<double, double>>
isolarRaizes(const Funcao& f, double xMin, double xMax, double passo);

// a posição falsa é montada aqui com o intervalo da menor raiz positiva
void executarLote(const vector<double>& valoresA3,
                  const vector<double>& valoresA2,
                  const Parametros& par,
                  const vector<const InterfaceMetodo*>& metodosAbertos,
                  double limite = 0.3);

#endif 