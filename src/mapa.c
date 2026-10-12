#include "mapa.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAMINHO_CAP_INICIAL 64

Labirinto* carregar_labirinto(const char *arquivo) {
    if (!arquivo) return NULL;

    FILE *fp = fopen(arquivo, "r");
    if (!fp) {
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo '%s'.\n", arquivo);
        return NULL;
    }

    Labirinto *lab = (Labirinto *)malloc(sizeof(Labirinto));
    if (!lab) {
        fprintf(stderr, "Erro: falha ao alocar memoria para o labirinto.\n");
        fclose(fp);
        return NULL;
    }

    lab->grade = NULL;
    lab->inicio_lin = -1;
    lab->inicio_col = -1;
    lab->bau_lin = -1;
    lab->bau_col = -1;

    if (fscanf(fp, "%d %d", &lab->linhas, &lab->colunas) != 2) {
        fprintf(stderr, "Erro: dimensoes do labirinto invalidas.\n");
        free(lab);
        fclose(fp);
        return NULL;
    }

    if (lab->linhas <= 0 || lab->colunas <= 0) {
        fprintf(stderr, "Erro: dimensoes devem ser positivas (%d x %d).\n", lab->linhas, lab->colunas);
        free(lab);
        fclose(fp);
        return NULL;
    }

    if (fscanf(fp, "%d", &lab->chaves_necessarias) != 1 || lab->chaves_necessarias < 0) {
        fprintf(stderr, "Erro: quantidade de chaves invalida.\n");
        free(lab);
        fclose(fp);
        return NULL;
    }

    lab->grade = (TipoCelula **)malloc(lab->linhas * sizeof(TipoCelula *));
    if (!lab->grade) {
        fprintf(stderr, "Erro: falha ao alocar linhas da grade.\n");
        free(lab);
        fclose(fp);
        return NULL;
    }

    for (int i = 0; i < lab->linhas; i++) {
        lab->grade[i] = (TipoCelula *)malloc(lab->colunas * sizeof(TipoCelula));
        if (!lab->grade[i]) {
            fprintf(stderr, "Erro: falha ao alocar colunas na linha %d.\n", i);
            for (int k = 0; k < i; k++) {
                free(lab->grade[k]);
            }
            free(lab->grade);
            free(lab);
            fclose(fp);
            return NULL;
        }
    }

    for (int i = 0; i < lab->linhas; i++) {
        for (int j = 0; j < lab->colunas; j++) {
            char token[16];
            if (fscanf(fp, "%15s", token) != 1) {
                fprintf(stderr, "Erro: dados insuficientes na posicao [%d,%d].\n", i, j);
                liberar_labirinto(lab);
                fclose(fp);
                return NULL;
            }

            switch (token[0]) {
                case '0':
                    lab->grade[i][j] = SALA_VAZIA;
                    break;
                case '1':
                    lab->grade[i][j] = PAREDE;
                    break;
                case 'A':
                    if (lab->inicio_lin != -1) {
                        fprintf(stderr, "Erro: multiplas posicoes de inicio (A) encontradas.\n");
                        liberar_labirinto(lab);
                        fclose(fp);
                        return NULL;
                    }
                    lab->grade[i][j] = AVENTUREIRO;
                    lab->inicio_lin = i;
                    lab->inicio_col = j;
                    break;
                case 'C':
                    lab->grade[i][j] = CHAVE;
                    break;
                case 'X':
                    if (lab->bau_lin != -1) {
                        fprintf(stderr, "Erro: multiplas posicoes de bau (X) encontradas.\n");
                        liberar_labirinto(lab);
                        fclose(fp);
                        return NULL;
                    }
                    lab->grade[i][j] = BAU;
                    lab->bau_lin = i;
                    lab->bau_col = j;
                    break;
                default:
                    fprintf(stderr, "Erro: simbolo desconhecido '%s' em [%d,%d].\n", token, i, j);
                    liberar_labirinto(lab);
                    fclose(fp);
                    return NULL;
            }
        }
    }

    fclose(fp);

    if (lab->inicio_lin < 0 || lab->inicio_col < 0) {
        fprintf(stderr, "Erro: posicao inicial (A) nao especificada no arquivo.\n");
        liberar_labirinto(lab);
        return NULL;
    }
    if (lab->bau_lin < 0 || lab->bau_col < 0) {
        fprintf(stderr, "Erro: posicao do bau (X) nao especificada no arquivo.\n");
        liberar_labirinto(lab);
        return NULL;
    }

    return lab;
}

void liberar_labirinto(Labirinto *lab) {
    if (!lab) return;

    if (lab->grade) {
        for (int i = 0; i < lab->linhas; i++) {
            free(lab->grade[i]);
        }
        free(lab->grade);
    }

    free(lab);
}

int** alocar_matriz_int(int linhas, int colunas) {
    int **mat = (int **)malloc(linhas * sizeof(int *));
    if (!mat) return NULL;

    for (int i = 0; i < linhas; i++) {
        mat[i] = (int *)calloc(colunas, sizeof(int));
        if (!mat[i]) {
            for (int k = 0; k < i; k++) {
                free(mat[k]);
            }
            free(mat);
            return NULL;
        }
    }

    return mat;
}

void liberar_matriz_int(int **mat, int linhas) {
    if (!mat) return;

    for (int i = 0; i < linhas; i++) {
        free(mat[i]);
    }
    free(mat);
}

void caminho_init(Caminho *c) {
    c->passos = NULL;
    c->tamanho = 0;
    c->capacidade = 0;
}

void caminho_adicionar(Caminho *c, int lin, int col) {
    if (c->tamanho >= c->capacidade) {
        int nova_cap = (c->capacidade == 0) ? CAMINHO_CAP_INICIAL : c->capacidade * 2;
        Posicao *novo = (Posicao *)realloc(c->passos, nova_cap * sizeof(Posicao));
        if (!novo) {
            fprintf(stderr, "Erro: falha ao realocar vetor de caminho.\n");
            return;
        }
        c->passos = novo;
        c->capacidade = nova_cap;
    }

    c->passos[c->tamanho].linha = lin;
    c->passos[c->tamanho].coluna = col;
    c->tamanho++;
}

void caminho_remover_ultimo(Caminho *c) {
    if (c->tamanho > 0) {
        c->tamanho--;
    }
}

Caminho caminho_copiar(const Caminho *c) {
    Caminho copia;
    copia.tamanho = c->tamanho;
    copia.capacidade = c->tamanho;

    if (c->tamanho > 0) {
        copia.passos = (Posicao *)malloc(copia.capacidade * sizeof(Posicao));
        if (copia.passos) {
            memcpy(copia.passos, c->passos, c->tamanho * sizeof(Posicao));
        } else {
            fprintf(stderr, "Erro: falha ao duplicar caminho.\n");
            copia.tamanho = 0;
            copia.capacidade = 0;
        }
    } else {
        copia.passos = NULL;
    }

    return copia;
}

void caminho_liberar(Caminho *c) {
    free(c->passos);
    c->passos = NULL;
    c->tamanho = 0;
    c->capacidade = 0;
}
