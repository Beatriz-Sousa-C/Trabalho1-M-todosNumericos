#ifndef TB1_GRAFICOS_H
#define TB1_GRAFICOS_H

#include <string>
#include <vector>
#include "../Models/EstudoPassoH.h"

// Retorna true se gerou pelo menos uma figura.
bool gerarFiguras(const std::string& pastaCsv, const std::string& pastaFiguras);

// Gráfico log-log |erro| x h do estudo do passo (figuras/07_estudo_passo_h.svg).
bool gerarFiguraEstudoPassoH(const std::vector<PontoEstudoH>& pontos, double a3, double a2, double x,
                             double hPadrao, const std::string& pastaFiguras);

#endif //TB1_GRAFICOS_H
