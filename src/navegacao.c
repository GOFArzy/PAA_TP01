#include "navegacao.h"
#include "mapa.h"

/* Mapeamento de direcoes cardeais: 0=Cima, 1=Baixo, 2=Esquerda, 3=Direita */
const int DIRECOES_LIN[NUM_DIRECOES] = {-1, 1, 0, 0};
const int DIRECOES_COL[NUM_DIRECOES] = {0, 0, -1, 1};

/* Modo Normal (Euclidiano) */

int movimento_valido(const Labirinto *lab, int **visitado, int lin, int col) {
  if (!lab || !visitado) return 0;
  if (lin < 0 || lin >= lab->linhas)
    return 0;
  if (col < 0 || col >= lab->colunas)
    return 0;
  if (lab->grade[lin][col] == PAREDE)
    return 0;
  if (visitado[lin][col])
    return 0;
  return 1;
}

int proxima_posicao(const Labirinto *lab, int **visitado, int lin, int col,
                    int d, int *prox_lin, int *prox_col) {
  if (!lab || !visitado || !prox_lin || !prox_col) return 0;
  *prox_lin = lin + DIRECOES_LIN[d];
  *prox_col = col + DIRECOES_COL[d];
  return movimento_valido(lab, visitado, *prox_lin, *prox_col);
}

/* Modo Globo (Toroidal) */

int movimento_valido_globo(const Labirinto *lab, int **visitado, int lin,
                           int col) {
  if (!lab || !visitado) return 0;
  if (lab->grade[lin][col] == PAREDE)
    return 0;
  if (visitado[lin][col])
    return 0;
  return 1;
}

int proxima_posicao_globo(const Labirinto *lab, int **visitado, int lin,
                          int col, int d, int *prox_lin, int *prox_col) {
  if (!lab || !visitado || !prox_lin || !prox_col) return 0;
  *prox_lin = (lin + DIRECOES_LIN[d] + lab->linhas) % lab->linhas;
  *prox_col = (col + DIRECOES_COL[d] + lab->colunas) % lab->colunas;
  return movimento_valido_globo(lab, visitado, *prox_lin, *prox_col);
}
