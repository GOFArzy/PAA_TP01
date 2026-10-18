/*
 * navegacao.c
 *
 * Modulo de Navegacao e Validacao de Movimentos.
 *
 * Responsabilidades:
 *  - Validar movimentos nas 4 direcoes cardeais (cima, baixo, esq, dir).
 *  - Suporte ao Modo Globo (movimento toroidal, sem paredes nas bordas).
 *  - Fornecer a funcao de calculo da proxima posicao para o backtracking.
 *
 * Restricoes do problema:
 *  - Nao e permitido andar em diagonais.
 *  - Nao e permitido atravessar paredes (TipoCelula == PAREDE).
 *  - Nao e permitido revisitar uma sala ja visitada.
 *
 * Modo Globo (Extra 3):
 *  - O labirinto e tratado como um toroide.
 *  - Sair pela borda direita entra pela esquerda, e vice-versa.
 *  - Sair pela borda inferior entra pela superior, e vice-versa.
 *  - Formula: nova_pos = (pos + delta + tamanho) % tamanho
 */

#include "navegacao.h"
#include "mapa.h"

/* Vetores de direcao
 * Ordem: { cima, baixo, esquerda, direita }
 */
const int DIRECOES_LIN[NUM_DIRECOES] = {-1, 1, 0, 0};
const int DIRECOES_COL[NUM_DIRECOES] = {0, 0, -1, 1};

// Modo Normal

/*
 * movimento_valido()
 *
 * Verifica se a celula (lin, col) pode ser visitada em modo normal:
 *   1. Estar dentro dos limites da grade (0 <= lin < linhas, 0 <= col <
 * colunas).
 *   2. Nao ser uma parede (TipoCelula != PAREDE).
 *   3. Nao ter sido visitada ainda (visitado[lin][col] == 0).
 */
int movimento_valido(const Labirinto *lab, int **visitado, int lin, int col) {
  /* Verifica limites da grade */
  if (lin < 0 || lin >= lab->linhas)
    return 0;
  if (col < 0 || col >= lab->colunas)
    return 0;

  /* Verifica se e parede */
  if (lab->grade[lin][col] == PAREDE)
    return 0;

  /* Verifica se ja foi visitada */
  if (visitado[lin][col])
    return 0;

  return 1;
}

/*
 * proxima_posicao()
 *
 * Calcula a posicao resultante de mover da celula (lin, col) na direcao d
 * em modo normal. Escreve o resultado nos ponteiros prox_lin e prox_col.
 *
 * Retorna 1 se o movimento e valido, 0 caso contrario.
 */
int proxima_posicao(const Labirinto *lab, int **visitado, int lin, int col,
                    int d, int *prox_lin, int *prox_col) {
  *prox_lin = lin + DIRECOES_LIN[d];
  *prox_col = col + DIRECOES_COL[d];

  return movimento_valido(lab, visitado, *prox_lin, *prox_col);
}

// Modo Globo (Extra 3) - Movimento Toroidal

/*
 * movimento_valido_globo()
 *
 * Igual ao modo normal, mas as coordenadas ja foram normalizadas via
 * aritmetica modular antes da chamada (em proxima_posicao_globo).
 * Portanto, apenas verifica parede e visitado — os limites sao sempre validos
 * apos o wrap-around.
 */
int movimento_valido_globo(const Labirinto *lab, int **visitado, int lin,
                           int col) {
  /* Apos wrap-around toroidal, lin e col ja estao dentro dos limites */
  if (lab->grade[lin][col] == PAREDE)
    return 0;
  if (visitado[lin][col])
    return 0;
  return 1;
}

/*
 * proxima_posicao_globo()
 *
 * Calcula a proxima posicao a partir de (lin, col) na direcao d aplicando
 * wrap-around toroidal: o labirinto "se conecta" nas bordas, como o jogo
 * da cobrinha.
 *
 * Formula:
 *   nova_lin = (lin + DIRECOES_LIN[d] + lab->linhas)  % lab->linhas
 *   nova_col = (col + DIRECOES_COL[d] + lab->colunas) % lab->colunas
 *
 * O somatorio com lab->linhas/colunas antes do modulo garante resultado
 * positivo mesmo quando (lin + delta) seria negativo (ex: lin=0, delta=-1).
 *
 * Retorna 1 se o movimento e valido, 0 caso contrario.
 */
int proxima_posicao_globo(const Labirinto *lab, int **visitado, int lin,
                          int col, int d, int *prox_lin, int *prox_col) {
  /* Aplica wrap-around toroidal */
  *prox_lin = (lin + DIRECOES_LIN[d] + lab->linhas) % lab->linhas;
  *prox_col = (col + DIRECOES_COL[d] + lab->colunas) % lab->colunas;

  return movimento_valido_globo(lab, visitado, *prox_lin, *prox_col);
}
