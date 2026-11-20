#include "mapa.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TAM_TOKEN 64

static void mapa_zerar(Mapa *mapa) {
  mapa->linhas = mapa->colunas = 0;
  mapa->hp_inicial = 0;
  mapa->valor = NULL;
  mapa->inicio_linha = mapa->inicio_coluna = -1;
  mapa->fim_linha = mapa->fim_coluna = -1;
}
// Converte token em inteiro (aceita sinal explicito: +20, -10, 0).
static int token_para_int(const char *tok, int *saida) {
  char *fim;
  long v;

  errno = 0;
  v = strtol(tok, &fim, 10);
  if (fim == tok || *fim != '\0' || errno == ERANGE || v < INT_MIN ||
      v > INT_MAX)
    return 0;
  *saida = (int)v;
  return 1;
}
void mapa_liberar(Mapa *mapa) {
  int i;

  if (mapa == NULL)
    return;
  if (mapa->valor != NULL) {
    for (i = 0; i < mapa->linhas; i++)
      free(mapa->valor[i]);
    free(mapa->valor);
  }
  mapa_zerar(mapa);
}
int mapa_ler(const char *arquivo, Mapa *mapa) {
  FILE *f;
  char tok[TAM_TOKEN];
  int i, j, achou_i = 0, achou_f = 0;

  if (arquivo == NULL || mapa == NULL)
    return MAPA_ERRO_ARQUIVO;

  mapa_zerar(mapa);

  f = fopen(arquivo, "r");
  if (f == NULL)
    return MAPA_ERRO_ARQUIVO;
  // Primeira linha: linhas colunas hp_inicial
  if (fscanf(f, "%d %d %d", &mapa->linhas, &mapa->colunas, &mapa->hp_inicial) !=
          3 ||
      mapa->linhas <= 0 || mapa->colunas <= 0) {
    fclose(f);
    mapa_zerar(mapa);
    return MAPA_ERRO_FORMATO;
  }
  // Aloca matriz linhas x colunas
  mapa->valor = (int **)calloc((size_t)mapa->linhas, sizeof(int *));
  if (mapa->valor == NULL) {
    fclose(f);
    mapa_zerar(mapa);
    return MAPA_ERRO_MEMORIA;
  }
  for (i = 0; i < mapa->linhas; i++) {
    mapa->valor[i] = (int *)calloc((size_t)mapa->colunas, sizeof(int));
    if (mapa->valor[i] == NULL) {
      fclose(f);
      mapa_liberar(mapa);
      return MAPA_ERRO_MEMORIA;
    }
  }
  // Celulas: inteiro, 'I' ou 'F' separados por espaco
  for (i = 0; i < mapa->linhas; i++) {
    for (j = 0; j < mapa->colunas; j++) {
      if (fscanf(f, "%63s", tok) != 1) {
        fclose(f);
        mapa_liberar(mapa);
        return MAPA_ERRO_FORMATO;
      }
      if (strcmp(tok, "I") == 0) {
        if (achou_i) {
          fclose(f);
          mapa_liberar(mapa);
          return MAPA_ERRO_FORMATO;
        }
        achou_i = 1;
        mapa->inicio_linha = i;
        mapa->inicio_coluna = j;
        mapa->valor[i][j] = 0;
      } else if (strcmp(tok, "F") == 0) {
        if (achou_f) {
          fclose(f);
          mapa_liberar(mapa);
          return MAPA_ERRO_FORMATO;
        }
        achou_f = 1;
        mapa->fim_linha = i;
        mapa->fim_coluna = j;
        mapa->valor[i][j] = 0;
      } else if (strcmp(tok, "#") == 0 || strcmp(tok, "X") == 0 ||
                 strcmp(tok, "P") == 0) {
        // parede ou obstaculo intransponivel (extra 1)
        mapa->valor[i][j] = MAPA_PAREDE;
      } else if (!token_para_int(tok, &mapa->valor[i][j])) {
        fclose(f);
        mapa_liberar(mapa);
        return MAPA_ERRO_FORMATO;
      }
    }
  }
  fclose(f);
  // Precisa existir exatamente um I e um F
  if (!achou_i || !achou_f) {
    mapa_liberar(mapa);
    return MAPA_ERRO_FORMATO;
  }
  return MAPA_OK;
}

// Extra 1: Verifica se a celula e uma parede/obstaculo
int mapa_eh_parede(const Mapa *mapa, int linha, int coluna) {
  if (mapa == NULL || mapa->valor == NULL)
    return 0;
  if (linha < 0 || linha >= mapa->linhas || coluna < 0 ||
      coluna >= mapa->colunas)
    return 0;
  return (mapa->valor[linha][coluna] == MAPA_PAREDE);
}

// Imprime a caverna com destaque para as celulas I e F e paredes '#'
void mapa_imprimir(const Mapa *mapa) {
  int i, j;

  if (mapa == NULL || mapa->valor == NULL)
    return;

  printf("=== Mapa da Caverna (%d x %d) ===\n", mapa->linhas, mapa->colunas);
  for (i = 0; i < mapa->linhas; i++) {
    for (j = 0; j < mapa->colunas; j++) {
      if (i == mapa->inicio_linha && j == mapa->inicio_coluna)
        printf("   I");
      else if (i == mapa->fim_linha && j == mapa->fim_coluna)
        printf("   F");
      else if (mapa->valor[i][j] == MAPA_PAREDE)
        printf("   #");
      else
        printf("%4d", mapa->valor[i][j]);
    }
    printf("\n");
  }
}

// Exibe resumo com monstros, pocoes, salas vazias e paredes
void mapa_resumo(const Mapa *mapa) {
  int i, j;
  int monstros = 0, pocoes = 0, vazias = 0, paredes = 0;
  int dano_total = 0, cura_total = 0;

  if (mapa == NULL || mapa->valor == NULL)
    return;

  for (i = 0; i < mapa->linhas; i++) {
    for (j = 0; j < mapa->colunas; j++) {
      // Ignora celulas especiais de inicio e saida
      if ((i == mapa->inicio_linha && j == mapa->inicio_coluna) ||
          (i == mapa->fim_linha && j == mapa->fim_coluna))
        continue;

      int v = mapa->valor[i][j];
        if (v == MAPA_PAREDE) {
          paredes++;
        } else if (v < 0) {
          monstros++;
          dano_total += (-v);
        } else if (v > 0) {
          pocoes++;
          cura_total += v;
        } else {
          vazias++;
        }
      }
    }

    printf("=== Resumo da Caverna ===\n");
    printf("Dimensoes: %d x %d | HP inicial: %d\n", mapa->linhas, mapa->colunas,
           mapa->hp_inicial);
    printf("Inicio I: (%d, %d) | Saida F: (%d, %d)\n", mapa->inicio_linha,
           mapa->inicio_coluna, mapa->fim_linha, mapa->fim_coluna);
    if (paredes > 0)
      printf("Paredes (obstaculos): %d\n", paredes);
    printf("Monstros: %d (dano total: %d)\n", monstros, dano_total);
    printf("Pocoes: %d (cura total: %d)\n", pocoes, cura_total);
    printf("Salas vazias: %d\n", vazias);
  }
