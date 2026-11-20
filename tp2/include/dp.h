#ifndef DP_H
#define DP_H

#include "mapa.h"
#include "caminho.h"

// dp.h - Programacao dinamica para encontrar o melhor caminho

struct Solucao {
    int existe_caminho;       // 1 se F e alcancavel com HP > 0, 0 caso contrario
    int possivel;             // compatibilidade com codigo anterior
    int hp_final;             // maior HP ao chegar em F
    int **melhor_hp;          // tabela L x C com o maior HP em cada celula
    Coordenada **pai;         // tabela L x C com a celula antecessora
    int **num_caminhos;       // Extra 2: quantidade de caminhos otimos por celula
    int caminhos_otimos;      // Extra 2: total de caminhos otimos ate a saida F
};

int dp_resolver(const Mapa *mapa, Solucao *solucao);
void dp_liberar(const Mapa *mapa, Solucao *solucao);

// Extra 2: Imprime a tabela da DP com o melhor HP de cada celula
void dp_imprimir_tabela(const Mapa *mapa, const Solucao *solucao);

#endif /* DP_H */
