#include "backtracking.h"
#include "navegacao.h"
#include <stdio.h>
#include <stdlib.h>

#define CAPACIDADE_INICIAL_LISTA 16

/* --- Funções para gerenciar a Lista de Caminhos (Extra 2) --- */

void lista_caminhos_init(ListaCaminhos *lista) {
    if (!lista) return;
    lista->itens = NULL;
    lista->total = 0;
    lista->capacidade = 0;
}

void lista_caminhos_adicionar(ListaCaminhos *lista, const Caminho *c) {
    if (!lista || !c) return;

    if (lista->total >= lista->capacidade) {
        int nova_cap = (lista->capacidade == 0) ? CAPACIDADE_INICIAL_LISTA : lista->capacidade * 2;
        Caminho *novo = (Caminho *)realloc(lista->itens, nova_cap * sizeof(Caminho));
        if (!novo) {
            fprintf(stderr, "Erro: falha ao realocar memoria para lista de caminhos.\n");
            return;
        }
        lista->itens = novo;
        lista->capacidade = nova_cap;
    }

    lista->itens[lista->total] = caminho_copiar(c);
    lista->total++;
}

void lista_caminhos_liberar(ListaCaminhos *lista) {
    if (!lista) return;
    for (int i = 0; i < lista->total; i++) {
        caminho_liberar(&lista->itens[i]);
    }
    free(lista->itens);
    lista->itens = NULL;
    lista->total = 0;
    lista->capacidade = 0;
}

/* --- Função Recursiva do Backtracking --- */

static int backtrack_recursivo(const Labirinto *lab,
                               int lin,
                               int col,
                               int **visitado,
                               Caminho *caminho_atual,
                               int chaves_coletadas,
                               int profundidade,
                               Caminho *caminho_solucao,
                               ListaCaminhos *todos_caminhos,
                               int modo_globo,
                               int modo_todos,
                               AnaliseMetricas *metricas) {
    /* 1. Registro de métricas da chamada */
    if (metricas) {
        analise_registrar_chamada(metricas, profundidade);
    }

    /* 2. Fazer a escolha: visitar a célula atual */
    visitado[lin][col] = 1;
    caminho_adicionar(caminho_atual, lin, col);

    TipoCelula celula_atual = lab->grade[lin][col];
    int pegou_chave = 0;

    if (celula_atual == CHAVE) {
        chaves_coletadas++;
        pegou_chave = 1;
    }

    /* 3. Caso Base: Chegou ao Baú */
    if (celula_atual == BAU) {
        if (chaves_coletadas == lab->chaves_necessarias) {
            if (modo_todos && todos_caminhos) {
                lista_caminhos_adicionar(todos_caminhos, caminho_atual);
                /* Se também precisamos do primeiro caminho no caminho_solucao */
                if (caminho_solucao && caminho_solucao->tamanho == 0) {
                    Caminho copia = caminho_copiar(caminho_atual);
                    *caminho_solucao = copia;
                }
                /* No modo todos, desfaz a escolha e continua explorando */
                if (pegou_chave) chaves_coletadas--;
                caminho_remover_ultimo(caminho_atual);
                visitado[lin][col] = 0;
                if (metricas) analise_registrar_retrocesso(metricas);
                return 1;
            } else {
                /* Modo normal: copia para a solução final e retorna sucesso imediato */
                if (caminho_solucao) {
                    Caminho copia = caminho_copiar(caminho_atual);
                    *caminho_solucao = copia;
                }
                return 1;
            }
        } else {
            /* Poda por Inviabilidade Futura:
             * Como o regulamento veda revisitar salas (sem ciclos), caso o aventureiro
             * pise na câmara do baú sem ter coletado todas as chaves, ele ficaria impossibilitado
             * de retornar a esta mesma câmara no futuro. Logo, este ramo é podado imediatamente. */
            if (metricas) {
                analise_registrar_poda(metricas);
                analise_registrar_retrocesso(metricas);
            }
            caminho_remover_ultimo(caminho_atual);
            visitado[lin][col] = 0;
            return 0;
        }
    }

    /* 4. Tentar explorar as 4 direções ortogonais */
    int encontrou_solucao = 0;

    for (int d = 0; d < NUM_DIRECOES; d++) {
        int prox_lin = -1, prox_col = -1;
        int valido = 0;

        if (modo_globo) {
            valido = proxima_posicao_globo(lab, visitado, lin, col, d, &prox_lin, &prox_col);
        } else {
            valido = proxima_posicao(lab, visitado, lin, col, d, &prox_lin, &prox_col);
        }

        if (valido) {
            int res = backtrack_recursivo(lab, prox_lin, prox_col, visitado,
                                          caminho_atual, chaves_coletadas,
                                          profundidade + 1, caminho_solucao,
                                          todos_caminhos, modo_globo, modo_todos,
                                          metricas);
            if (res) {
                encontrou_solucao = 1;
                /* Se não estiver buscando todos os caminhos, propaga o sucesso imediatamente */
                if (!modo_todos) {
                    return 1;
                }
            }
        } else if (metricas) {
            /* Movimento inválido / parede / já visitado conta como poda de ramo */
            analise_registrar_poda(metricas);
        }
    }

    /* 5. Desfazer a escolha (Backtrack): nenhum ramo a partir daqui levou a fim desejado */
    if (pegou_chave) {
        chaves_coletadas--;
    }
    caminho_remover_ultimo(caminho_atual);
    visitado[lin][col] = 0;

    if (metricas) {
        analise_registrar_retrocesso(metricas);
    }

    return encontrou_solucao;
}

/* --- Funções de Entrada da Interface --- */

int resolver_labirinto_completo(const Labirinto *lab,
                               Caminho *caminho_solucao,
                               ListaCaminhos *todos_caminhos,
                               const Opcoes *opts,
                               AnaliseMetricas *metricas) {
    if (!lab || !lab->grade) return 0;

    int modo_globo = (opts && opts->modo_globo);
    int modo_todos = (opts && opts->modo_todos);

    /* Aloca a matriz de controle de visitados */
    int **visitado = alocar_matriz_int(lab->linhas, lab->colunas);
    if (!visitado) {
        fprintf(stderr, "Erro ao alocar matriz de visitados.\n");
        return 0;
    }

    Caminho caminho_temp;
    caminho_init(&caminho_temp);

    if (metricas) {
        analise_iniciar(metricas);
    }

    int sucesso = backtrack_recursivo(lab,
                                      lab->inicio_lin,
                                      lab->inicio_col,
                                      visitado,
                                      &caminho_temp,
                                      0,              /* chaves coletadas */
                                      1,              /* profundidade inicial */
                                      caminho_solucao,
                                      todos_caminhos,
                                      modo_globo,
                                      modo_todos,
                                      metricas);

    /* Limpeza de recursos temporários */
    caminho_liberar(&caminho_temp);
    liberar_matriz_int(visitado, lab->linhas);

    return sucesso;
}

int resolver_labirinto(const Labirinto *lab, Caminho *caminho_solucao) {
    return resolver_labirinto_completo(lab, caminho_solucao, NULL, NULL, NULL);
}
