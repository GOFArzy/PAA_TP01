#ifndef DP_H
#define DP_H

#include "mapa.h"

typedef struct {
  int possivel;
  int hp_final;
  int tamanho_caminho; // quantos passos tem o caminho
  int (*caminho)[2];   // caminho de I até F
} Solucao;

int dp_resolver(const Mapa *mapa, Solucao *solucao);
void dp_liberar(const Mapa *mapa, Solucao *solucao);

#endif /* DP_H */