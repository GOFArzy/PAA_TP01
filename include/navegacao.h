#ifndef NAVEGACAO_H
#define NAVEGACAO_H

#include "mapa.h"

#define NUM_DIRECOES 4

/* Vetores de deslocamento ortogonal: Cima, Baixo, Esquerda, Direita */
extern const int DIRECOES_LIN[NUM_DIRECOES];
extern const int DIRECOES_COL[NUM_DIRECOES];

/* Valida limites, paredes e celulas visitadas no modo normal (retorna 1 se
 * valido, 0 se invalido) */
int movimento_valido(const Labirinto *lab, int **visitado, int lin, int col);

/* Valida paredes e celulas visitadas no modo toroidal (limites sao wrap-around)
 */
int movimento_valido_globo(const Labirinto *lab, int **visitado, int lin,
                           int col);

/* Calcula coordenada resultante e valida movimento no modo normal */
int proxima_posicao(const Labirinto *lab, int **visitado, int lin, int col,
                    int d, int *prox_lin, int *prox_col);

/* Calcula coordenada resultante com wrap-around toroidal e valida o movimento
 */
int proxima_posicao_globo(const Labirinto *lab, int **visitado, int lin,
                          int col, int d, int *prox_lin, int *prox_col);

#endif /* NAVEGACAO_H */
