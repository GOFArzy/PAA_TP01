#include "saida.h"

#include <stdio.h>
#include <stdlib.h>

void imprimir_caminho(const Caminho *caminho) {
    if (!caminho || caminho->tamanho == 0) {
        printf("Sem passos registrados.\n");
        return;
    }

    /* Formato padrao: [l,c],[l,c],... */
    for (int i = 0; i < caminho->tamanho; i++) {
        printf("[%d,%d]", caminho->passos[i].linha, caminho->passos[i].coluna);
        if (i < caminho->tamanho - 1) {
            printf(",");
        }
    }
    printf("\n");
}

void imprimir_sem_solucao(void) {
    printf("Nao foi possivel encontrar um caminho valido para coletar todas as chaves e abrir o bau.\n");
}

int salvar_caminho_arquivo(const char *nome_arquivo, const Caminho *caminho) {
    if (!nome_arquivo || !caminho) return 0;

    FILE *fp = fopen(nome_arquivo, "w");
    if (!fp) {
        fprintf(stderr, "Erro ao abrir o arquivo '%s' para salvar o caminho.\n", nome_arquivo);
        return 0;
    }

    for (int i = 0; i < caminho->tamanho; i++) {
        fprintf(fp, "[%d,%d]", caminho->passos[i].linha, caminho->passos[i].coluna);
        if (i < caminho->tamanho - 1) {
            fprintf(fp, ",");
        }
    }
    fprintf(fp, "\n");

    fclose(fp);
    return 1;
}

void imprimir_resumo(int encontrou_solucao, const Caminho *caminho, double tempo_ms, long long chamadas_rec, int nivel_max_rec) {
    printf("\n--- Resumo da Execucao ---\n");
    if (encontrou_solucao && caminho) {
        printf("Status:              Solucao encontrada\n");
        printf("Total de passos:     %d\n", caminho->tamanho);
    } else {
        printf("Status:              Sem solucao\n");
        printf("Total de passos:     0\n");
    }
    printf("Tempo decorrido:     %.4f ms\n", tempo_ms);

    if (chamadas_rec >= 0) {
        printf("Chamadas recursivas: %lld\n", chamadas_rec);
    }
    if (nivel_max_rec >= 0) {
        printf("Profundidade maxima: %d\n", nivel_max_rec);
    }
    printf("--------------------------\n\n");
}
