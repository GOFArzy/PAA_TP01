#include "dp.h"
#include "caminho.h"
#include "mapa.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// Funcoes auxiliares de alocacao
static int **alocar_matriz_int(int linhas, int colunas) {
  int i;
  int **m = (int **)calloc((size_t)linhas, sizeof(int *));
  if (m == NULL)
    return NULL;
  for (i = 0; i < linhas; i++) {
    m[i] = (int *)calloc((size_t)colunas, sizeof(int));
    if (m[i] == NULL) {
      for (int k = 0; k < i; k++)
        free(m[k]);
      free(m);
      return NULL;
    }
  }
  return m;
}

static void liberar_matriz_int(int **m, int linhas) {
  if (m == NULL)
    return;
  for (int i = 0; i < linhas; i++)
    free(m[i]);
  free(m);
}

static Coordenada **alocar_matriz_coord(int linhas, int colunas) {
  int i;
  Coordenada **m = (Coordenada **)calloc((size_t)linhas, sizeof(Coordenada *));
  if (m == NULL)
    return NULL;
  for (i = 0; i < linhas; i++) {
    m[i] = (Coordenada *)calloc((size_t)colunas, sizeof(Coordenada));
    if (m[i] == NULL) {
      for (int k = 0; k < i; k++)
        free(m[k]);
      free(m);
      return NULL;
    }
  }
  return m;
}

static void liberar_matriz_coord(Coordenada **m, int linhas) {
  if (m == NULL)
    return;
  for (int i = 0; i < linhas; i++)
    free(m[i]);
  free(m);
}

// Funcao principal
int dp_resolver(const Mapa *mapa, Solucao *solucao) {
  int i, j;

  if (mapa == NULL || solucao == NULL)
    return 0;

  // Inicializa solucao
  solucao->existe_caminho = 0;
  solucao->possivel = 0;
  solucao->hp_final = 0;
  solucao->melhor_hp = NULL;
  solucao->pai = NULL;
  solucao->num_caminhos = NULL;
  solucao->caminhos_otimos = 0;

  // Aloca matrizes melhor_hp, pai e num_caminhos (Extra 2)
  solucao->melhor_hp = alocar_matriz_int(mapa->linhas, mapa->colunas);
  solucao->pai = alocar_matriz_coord(mapa->linhas, mapa->colunas);
  solucao->num_caminhos = alocar_matriz_int(mapa->linhas, mapa->colunas);
  if (solucao->melhor_hp == NULL || solucao->pai == NULL ||
      solucao->num_caminhos == NULL) {
    dp_liberar(mapa, solucao);
    return 0;
  }

  // Inicializa tabela com INT_MIN e predecessores com (-1, -1)
  for (i = 0; i < mapa->linhas; i++) {
    for (j = 0; j < mapa->colunas; j++) {
      solucao->melhor_hp[i][j] = INT_MIN;
      solucao->pai[i][j].linha = -1;
      solucao->pai[i][j].coluna = -1;
      solucao->num_caminhos[i][j] = 0;
    }
  }

  // Caso base — posicao inicial I
  int il = mapa->inicio_linha;
  int ic = mapa->inicio_coluna;
  if (il < 0 || il >= mapa->linhas || ic < 0 || ic >= mapa->colunas) {
    dp_liberar(mapa, solucao);
    return 0;
  }
  solucao->melhor_hp[il][ic] = mapa->hp_inicial;
  solucao->num_caminhos[il][ic] = 1; // Extra 2: exatamente 1 caminho no inicio

  // Preenche a tabela DP
  for (i = il; i >= 0; i--) {
    for (j = ic; j >= 0; j--) {
      if (i == il && j == ic)
        continue; // caso base ja tratado

      int melhor = INT_MIN;
      int caminhos = 0;
      Coordenada origem = {-1, -1};

      // Veio de baixo (movimento para cima)
      if (i + 1 < mapa->linhas && solucao->melhor_hp[i + 1][j] > 0) {
        melhor = solucao->melhor_hp[i + 1][j];
        caminhos = solucao->num_caminhos[i + 1][j];
        origem.linha = i + 1;
        origem.coluna = j;
      }

      // Veio da direita (movimento para esquerda)
      if (j + 1 < mapa->colunas && solucao->melhor_hp[i][j + 1] > 0) {
        int hp_dir = solucao->melhor_hp[i][j + 1];
        if (hp_dir > melhor) {
          melhor = hp_dir;
          caminhos = solucao->num_caminhos[i][j + 1];
          origem.linha = i;
          origem.coluna = j + 1;
        } else if (hp_dir == melhor && melhor != INT_MIN) {
          // Extra 2: Empate no melhor HP, soma os caminhos alternativos
          caminhos += solucao->num_caminhos[i][j + 1];
        }
      }

      if (melhor != INT_MIN) {
        int hp_novo = melhor + mapa->valor[i][j];
        if (hp_novo > 0) {
          solucao->melhor_hp[i][j] = hp_novo;
          solucao->num_caminhos[i][j] = caminhos;
          solucao->pai[i][j] = origem;
        }
      }
    }
  }

  // Verifica se F e alcancavel
  int fl = mapa->fim_linha;
  int fc = mapa->fim_coluna;
  if (fl >= 0 && fl < mapa->linhas && fc >= 0 && fc < mapa->colunas) {
    if (solucao->melhor_hp[fl][fc] > 0) {
      solucao->existe_caminho = 1;
      solucao->possivel = 1;
      solucao->hp_final = solucao->melhor_hp[fl][fc];
      solucao->caminhos_otimos = solucao->num_caminhos[fl][fc];
      return 1;
    }
  }

  solucao->existe_caminho = 0;
  solucao->possivel = 0;
  solucao->hp_final = 0;
  solucao->caminhos_otimos = 0;
  return 0;
}

// Libera memoria
void dp_liberar(const Mapa *mapa, Solucao *solucao) {
  if (mapa == NULL || solucao == NULL)
    return;

  if (solucao->melhor_hp != NULL) {
    liberar_matriz_int(solucao->melhor_hp, mapa->linhas);
    solucao->melhor_hp = NULL;
  }
  if (solucao->pai != NULL) {
    liberar_matriz_coord(solucao->pai, mapa->linhas);
    solucao->pai = NULL;
  }
  if (solucao->num_caminhos != NULL) {
    liberar_matriz_int(solucao->num_caminhos, mapa->linhas);
    solucao->num_caminhos = NULL;
  }

  solucao->existe_caminho = 0;
  solucao->possivel = 0;
  solucao->hp_final = 0;
  solucao->caminhos_otimos = 0;
}

// Extra 2: Imprime a tabela da programacao dinamica no terminal
void dp_imprimir_tabela(const Mapa *mapa, const Solucao *solucao) {
  int i, j;

  if (mapa == NULL || solucao == NULL || solucao->melhor_hp == NULL)
    return;

  printf("=== Tabela de Programacao Dinamica (Melhor HP) ===\n");
  for (i = 0; i < mapa->linhas; i++) {
    for (j = 0; j < mapa->colunas; j++) {
      if (solucao->melhor_hp[i][j] == INT_MIN)
        printf("   -");
      else
        printf("%4d", solucao->melhor_hp[i][j]);
    }
    printf("\n");
  }
  if (solucao->existe_caminho) {
    printf("HP maximo ao chegar em F: %d | Caminhos otimos possiveis: %d\n",
           solucao->hp_final, solucao->caminhos_otimos);
  }
}
