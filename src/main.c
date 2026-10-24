#include "mapa.h"
#include "saida.h"
#include "backtracking.h"
#include "navegacao.h"
#include "cli.h"
#include "analise.h"
#include "visualizador.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    Opcoes opts;
    if (!cli_parse(argc, argv, &opts)) {
        return 1;
    }

    /* Leitura do labirinto a partir do arquivo de entrada */
    Labirinto *lab = carregar_labirinto(opts.arquivo_entrada);
    if (!lab) {
        return 1;
    }

    Caminho caminho_solucao;
    caminho_init(&caminho_solucao);

    ListaCaminhos todos_caminhos;
    lista_caminhos_init(&todos_caminhos);

    AnaliseMetricas metricas;
    AnaliseMetricas *ptr_metricas = NULL;
    if (opts.modo_analise) {
        analise_iniciar(&metricas);
        ptr_metricas = &metricas;
    }

    clock_t t_inicio = clock();

    /* Execução da busca via Backtracking */
    int sucesso = resolver_labirinto_completo(lab,&caminho_solucao, opts.modo_todos ? &todos_caminhos : NULL, &opts, ptr_metricas);

    clock_t t_fim = clock();
    double tempo_ms = ((double)(t_fim - t_inicio) / CLOCKS_PER_SEC) * 1000.0;

    /* Exibição dos resultados */
    if (sucesso) {
        if (opts.modo_todos) {
            printf("\n===== TODOS OS CAMINHOS ENCONTRADOS (%d) =====\n", todos_caminhos.total);
            for (int i = 0; i < todos_caminhos.total; i++) {
                printf("Caminho %d: ", i + 1);
                imprimir_caminho(&todos_caminhos.itens[i]);
            }
        } else {
            imprimir_caminho(&caminho_solucao);
        }
    } else {
        imprimir_sem_solucao();
    }

    /* Exibição das métricas analíticas se solicitado */
    if (opts.modo_analise && ptr_metricas) {
        analise_imprimir(ptr_metricas);
    }

    /* Resumo padronizado de execução */
    imprimir_resumo(sucesso,
                    &caminho_solucao,
                    tempo_ms,
                    ptr_metricas ? ptr_metricas->chamadas_recursivas : -1,
                    ptr_metricas ? ptr_metricas->profundidade_maxima : -1);

    /* Visualização ANSI no terminal (se requisitado) */
    if (opts.modo_visual && sucesso) {
        printf("Pressione Enter para iniciar a animacao do caminho...");
        getchar();
        visualizador_animar_caminho(lab, &caminho_solucao, 80000); /* 80ms por passo */
        printf("\nResultado final no mapa:\n");
        visualizador_desenhar_caminho_final(lab, &caminho_solucao);
    }

    /* Liberação de todos os recursos alocados */
    if (opts.modo_todos) {
        lista_caminhos_liberar(&todos_caminhos);
    }
    caminho_liberar(&caminho_solucao);
    liberar_labirinto(lab);

    return 0;
}
