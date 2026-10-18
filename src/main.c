#include "mapa.h"
#include "saida.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* TODO: Pessoas 2, 3 e 4 - incluir headers conforme forem finalizados */
// #include "backtracking.h"
// #include "navegacao.h"
// #include "cli.h"
// #include "analise.h"
// #include "visualizador.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_labirinto.txt> [flags]\n", argv[0]);
        return 1;
    }

    const char *arquivo_entrada = argv[1];

    /* TODO: Pessoa 3 (cli.c) - processar argumentos extras (--globo, --visual) */
    int modo_globo = 0;
    int modo_visual = 0;
    (void)modo_globo;
    (void)modo_visual;

    /* Leitura do labirinto (Pessoa 1) */
    Labirinto *lab = carregar_labirinto(arquivo_entrada);
    if (!lab) {
        return 1;
    }

    Caminho caminho_solucao;
    caminho_init(&caminho_solucao);

    /* TODO: Pessoa 4 (analise.c) - inicializar contadores de metricas */
    long long chamadas_rec = 0;
    int nivel_max_rec = 0;

    clock_t t_inicio = clock();

    /* TODO: Pessoa 2 (backtracking.c) - chamar a busca recursiva do labirinto */
    // int sucesso = resolver_labirinto(lab, &caminho_solucao);
    int sucesso = 0;

    clock_t t_fim = clock();
    double tempo_ms = ((double)(t_fim - t_inicio) / CLOCKS_PER_SEC) * 1000.0;

    /* Exibicao dos resultados (Pessoa 5) */
    if (sucesso) {
        imprimir_caminho(&caminho_solucao);
    } else {
        imprimir_sem_solucao();
    }

    imprimir_resumo(sucesso, &caminho_solucao, tempo_ms, chamadas_rec, nivel_max_rec);

    /* Liberacao de recursos */
    caminho_liberar(&caminho_solucao);
    liberar_labirinto(lab);

    return 0;
}
