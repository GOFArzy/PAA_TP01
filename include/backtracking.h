#ifndef BACKTRACKING_H
#define BACKTRACKING_H

#include "mapa.h"
#include "cli.h"
#include "analise.h"

/* Estrutura para armazenar múltiplos caminhos (Extra 2: --todos) */
typedef struct {
    Caminho *itens;
    int total;
    int capacidade;
} ListaCaminhos;

/* Gerenciamento da lista dinâmica de caminhos */
void lista_caminhos_init(ListaCaminhos *lista);
void lista_caminhos_adicionar(ListaCaminhos *lista, const Caminho *c);
void lista_caminhos_liberar(ListaCaminhos *lista);

/*
 * Função completa de resolução:
 * Suporta modo normal, toroidal (--globo), exaustivo (--todos) e métricas (--analise).
 * Retorna 1 se encontrou ao menos uma solução, ou 0 se não há caminho viável.
 */
int resolver_labirinto_completo(const Labirinto *lab,
                               Caminho *caminho_solucao,
                               ListaCaminhos *todos_caminhos,
                               const Opcoes *opts,
                               AnaliseMetricas *metricas);

/*
 * Interface simplificada (modo padrão):
 * Mantém compatibilidade direta com chamadas simples do benchmark e testes básicos.
 */
int resolver_labirinto(const Labirinto *lab, Caminho *caminho_solucao);

#endif /* BACKTRACKING_H */
