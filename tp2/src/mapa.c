#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include "mapa.h"

#define TAM_TOKEN 64

static void mapa_zerar(Mapa *mapa)
{
    mapa->linhas = mapa->colunas = 0;
    mapa->hp_inicial = 0;
    mapa->valor = NULL;
    mapa->inicio_linha = mapa->inicio_coluna = -1;
    mapa->fim_linha = mapa->fim_coluna = -1;
}
// Converte token em inteiro (aceita sinal explicito: +20, -10, 0). 
static int token_para_int(const char *tok, int *saida)
{
    char *fim;
    long v;

    errno = 0;
    v = strtol(tok, &fim, 10);
    if (fim == tok || *fim != '\0' || errno == ERANGE || v < INT_MIN || v > INT_MAX)
        return 0;
    *saida = (int)v;
    return 1;
}
void mapa_liberar(Mapa *mapa)
{
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
int mapa_ler(const char *arquivo, Mapa *mapa)
{
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
    if (fscanf(f, "%d %d %d", &mapa->linhas, &mapa->colunas, &mapa->hp_inicial) != 3
        || mapa->linhas <= 0 || mapa->colunas <= 0) {
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
                if (achou_i) { fclose(f); mapa_liberar(mapa); return MAPA_ERRO_FORMATO; }
                achou_i = 1;
                mapa->inicio_linha = i;
                mapa->inicio_coluna = j;
                mapa->valor[i][j] = 0;
            } else if (strcmp(tok, "F") == 0) {
                if (achou_f) { fclose(f); mapa_liberar(mapa); return MAPA_ERRO_FORMATO; }
                achou_f = 1;
                mapa->fim_linha = i;
                mapa->fim_coluna = j;
                mapa->valor[i][j] = 0;
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