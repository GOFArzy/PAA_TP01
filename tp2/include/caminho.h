#ifndef CAMINHO_H
#define CAMINHO_H

#include <stdio.h>
#include "mapa.h"

// caminho.h - Reconstrucao e validacao do caminho

#define CAMINHO_OK                  0
#define CAMINHO_ERRO_PARAMETROS    -1   // ponteiro nulo ou parametros invalidos
#define CAMINHO_ERRO_MEMORIA       -2   // falha de alocacao
#define CAMINHO_SEM_SOLUCAO        -3   // nao existe caminho ate F
#define CAMINHO_INVALIDO_INICIO    -4   // caminho nao comeca em I
#define CAMINHO_INVALIDO_FIM       -5   // caminho nao termina em F
#define CAMINHO_INVALIDO_MOVIMENTO -6   // passo ilegal (diferente de cima ou esquerda)
#define CAMINHO_INVALIDO_MORTE     -7   // HP <= 0 durante o trajeto
#define CAMINHO_INVALIDO_LIMITES   -8   // coordenada fora dos limites da matriz

typedef struct {
    int linha;
    int coluna;
} Coordenada;

// Declaracao antecipada de Solucao (definida em dp.h)
struct Solucao;
typedef struct Solucao Solucao;

int caminho_reconstruir(const Mapa *mapa, const Solucao *solucao,
                        Coordenada **caminho, int *quantidade);
int caminho_validar(const Mapa *mapa, const Coordenada *caminho, int quantidade);
void caminho_liberar(Coordenada *caminho);

// Extra 3: relatorio e visualizacao da rota
typedef struct {
    int passos;
    int hp_inicial;
    int hp_final;
    int hp_minimo;
    int hp_maximo;
    int dano_total;
    int cura_total;
} EstatisticasCaminho;

int caminho_calcular_estatisticas(const Mapa *mapa, const Coordenada *caminho,
                                 int quantidade, EstatisticasCaminho *estat);
void caminho_imprimir_relatorio(FILE *saida, const Mapa *mapa,
                               const Coordenada *caminho, int quantidade);
void caminho_desenhar_ascii(FILE *saida, const Mapa *mapa,
                            const Coordenada *caminho, int quantidade);

#endif /* CAMINHO_H */
