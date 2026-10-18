#ifndef NAVEGACAO_H
#define NAVEGACAO_H

#include "mapa.h"

// Numero de direcoes possiveis de movimento (cima, baixo, esquerda, direita).
#define NUM_DIRECOES 4

// Vetor de deslocamento para as 4 direcoes cardeais.
// Ordem: cima, baixo, esquerda, direita.
extern const int DIRECOES_LIN[NUM_DIRECOES];
extern const int DIRECOES_COL[NUM_DIRECOES];

/*
 * Verifica se a posicao (lin, col) e valida no labirinto em modo NORMAL.
 *  - Deve estar dentro dos limites da grade.
 *  - Nao pode ser parede.
 *  - Nao pode ter sido visitada.
 *
 * Retorna 1 se valida, 0 caso contrario.
 */
int movimento_valido(const Labirinto *lab, int **visitado, int lin, int col);

/*
 * Verifica se a posicao e valida no labirinto em modo GLOBO.
 *  - As bordas se "conectam": sair por um lado entra pelo lado oposto.
 *  - Aplica arith. modular: nova = (pos + delta + tamanho) % tamanho
 *  - Nao pode ser parede.
 *  - Nao pode ter sido visitada.
 *
 * Retorna 1 se valida, 0 caso contrario.
 */
int movimento_valido_globo(const Labirinto *lab, int **visitado, int lin,
                           int col);

/*
 * Calcula a proxima posicao a partir de (lin, col) na direcao d em modo NORMAL.
 * Escreve o resultado em *prox_lin e *prox_col.
 * Retorna 1 se o movimento e valido, 0 caso contrario.
 */
int proxima_posicao(const Labirinto *lab, int **visitado, int lin, int col,
                    int d, int *prox_lin, int *prox_col);

// Calcula a proxima posicao a partir de (lin, col) na direcao d em modo GLOBO.
//  Aplica wrap-around toroidal nas bordas.
//  Escreve o resultado em *prox_lin e *prox_col.
//  Retorna 1 se o movimento e valido, 0 caso contrario.
int proxima_posicao_globo(const Labirinto *lab, int **visitado, int lin,
                          int col, int d, int *prox_lin, int *prox_col);

#endif /* NAVEGACAO_H */
