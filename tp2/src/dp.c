#include "dp.h"
#include "mapa.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// Funcoes auxiliares
static int **alocar_matriz(int linhas, int colunas) {
  int i;
  int **m = calloc(linhas, sizeof(int *));
  if (m == NULL)
    return NULL;
  for (i = 0; i < linhas; i++) {
    m[i] = calloc(colunas, sizeof(int));
    if (m[i] == NULL) {
      for (int k = 0; k < i; k++)
        free(m[k]);
      free(m);
      return NULL;
    }
  }
  return m;
}

static void liberar_matriz(int **m, int linhas) {
  int i;
  if (m == NULL)
    return;
  for (i = 0; i < linhas; i++)
    free(m[i]);
  free(m);
}

// Funcao principal
int dp_resolver(const Mapa *mapa, Solucao *solucao) {
  int i, j;
  int **dp;   // tabela de HP máximo em cada célula
  int **pred; // 2 = veio da direita, 1 = veio de baixo, 0 = nenhum

  // Inicializa solucao
  solucao->possivel = 0;
  solucao->hp_final = 0;
  solucao->tamanho_caminho = 0;
  solucao->caminho = NULL;

  // Aloca as matrizes dp e pred
  dp = alocar_matriz(mapa->linhas, mapa->colunas);
  pred = alocar_matriz(mapa->linhas, mapa->colunas);
  if (dp == NULL || pred == NULL) {
    liberar_matriz(dp, mapa->linhas);
    liberar_matriz(pred, mapa->linhas);
    return 0;
  }

  // Inicializa tudo com INT_MIN
  for (i = 0; i < mapa->linhas; i++)
    for (j = 0; j < mapa->colunas; j++)
      dp[i][j] = INT_MIN;

  // Caso base — posição inicial
  dp[mapa->inicio_linha][mapa->inicio_coluna] = mapa->hp_inicial;

  // Preenche a tabela DP
  for (i = mapa->inicio_linha; i >= 0; i--) {
    for (j = mapa->inicio_coluna; j >= 0; j--) {
      if (i == mapa->inicio_linha && j == mapa->inicio_coluna)
        continue; // caso base já tratado

      int melhor = INT_MIN;
      int direcao = 0;

      // Veio de baixo
      if (i + 1 < mapa->linhas && dp[i + 1][j] > 0 && dp[i + 1][j] > melhor) {
        melhor = dp[i + 1][j];
        direcao = 1;
      }

      // Veio da direita
      if (j + 1 < mapa->colunas && dp[i][j + 1] > 0 && dp[i][j + 1] > melhor) {
        melhor = dp[i][j + 1];
        direcao = 2;
      }

      if (melhor != INT_MIN) {
        dp[i][j] = melhor + mapa->valor[i][j];
        pred[i][j] = direcao;
        if (dp[i][j] <= 0)
          dp[i][j] = INT_MIN; // morreu nesta célula
      }
    }
  }

  // Verifica se F é alcançável
  int fl = mapa->fim_linha, fc = mapa->fim_coluna;
  if (dp[fl][fc] == INT_MIN) {
    liberar_matriz(dp, mapa->linhas);
    liberar_matriz(pred, mapa->linhas);
    return 0;
  }

  solucao->possivel = 1;
  solucao->hp_final = dp[fl][fc];

  // Conta o tamanho do caminho
  int ci = fl, cj = fc, tam = 1;
  while (ci != mapa->inicio_linha || cj != mapa->inicio_coluna) {
    if (pred[ci][cj] == 1)
      ci++; // veio de baixo → voltar para baixo
    else
      cj++; // veio da direita → voltar para direita
    tam++;
  }

  // Aloca e preenche (de I até F)
  solucao->tamanho_caminho = tam;
  solucao->caminho = malloc(tam * sizeof(int[2]));
  if (solucao->caminho == NULL) {
    liberar_matriz(dp, mapa->linhas);
    liberar_matriz(pred, mapa->linhas);
    solucao->possivel = 0;
    return 0;
  }

  // Percorre de novo, guardando de trás para frente
  ci = fl;
  cj = fc;
  int k = tam - 1;
  solucao->caminho[k][0] = ci;
  solucao->caminho[k][1] = cj;
  while (ci != mapa->inicio_linha || cj != mapa->inicio_coluna) {
    k--;
    if (pred[ci][cj] == 1)
      ci++;
    else
      cj++;
    solucao->caminho[k][0] = ci;
    solucao->caminho[k][1] = cj;
  }

  liberar_matriz(dp, mapa->linhas);
  liberar_matriz(pred, mapa->linhas);
  return 1;
}

// Libera
void dp_liberar(const Mapa *mapa, Solucao *solucao) {
  (void)mapa;
  if (solucao->caminho != NULL) {
    free(solucao->caminho);
    solucao->caminho = NULL;
  }
  solucao->tamanho_caminho = 0;
}