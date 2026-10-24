#include "mapa.h"
#include "saida.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "backtracking.h"

#define NUM_REPETICOES_PADRAO 10

static double obter_tempo_ms(void) {
    return ((double)clock() / CLOCKS_PER_SEC) * 1000.0;
}

typedef struct {
    char arquivo[256];
    int linhas;
    int colunas;
    int chaves;
    int solucao;
    int passos;
    double media;
    double min_t;
    double max_t;
    double desvio;
} ResultadoBenchmark;

static int executar_benchmark(const char *arquivo, int repeticoes, ResultadoBenchmark *res) {
    printf("\nArquivo: %s (%d repeticoes)\n", arquivo, repeticoes);

    Labirinto *lab = carregar_labirinto(arquivo);
    if (!lab) {
        fprintf(stderr, "Erro ao carregar '%s'.\n", arquivo);
        return 0;
    }

    double *tempos = (double *)malloc(repeticoes * sizeof(double));
    if (!tempos) {
        liberar_labirinto(lab);
        return 0;
    }

    int solucao = 0;
    int passos = 0;

    for (int r = 0; r < repeticoes; r++) {
        Caminho c;
        caminho_init(&c);

        double t0 = obter_tempo_ms();

        solucao = resolver_labirinto(lab, &c);

        double t1 = obter_tempo_ms();
        tempos[r] = t1 - t0;

        passos = c.tamanho;
        caminho_liberar(&c);
    }

    double soma = 0.0;
    double min_t = tempos[0];
    double max_t = tempos[0];

    for (int i = 0; i < repeticoes; i++) {
        soma += tempos[i];
        if (tempos[i] < min_t) min_t = tempos[i];
        if (tempos[i] > max_t) max_t = tempos[i];
    }

    double media = soma / repeticoes;

    double var = 0.0;
    for (int i = 0; i < repeticoes; i++) {
        double d = tempos[i] - media;
        var += d * d;
    }
    double desvio = (repeticoes > 1) ? sqrt(var / (repeticoes - 1)) : 0.0;

    printf("Solucao:       %s (%d passos)\n", solucao ? "Sim" : "Nao", passos);
    printf("Tempo medio:   %.4f ms\n", media);
    printf("Tempo minimo:  %.4f ms\n", min_t);
    printf("Tempo maximo:  %.4f ms\n", max_t);
    printf("Desvio padrao: %.4f ms\n", desvio);

    if (res) {
        snprintf(res->arquivo, sizeof(res->arquivo), "%s", arquivo);
        res->linhas = lab->linhas;
        res->colunas = lab->colunas;
        res->chaves = lab->chaves_necessarias;
        res->solucao = solucao;
        res->passos = passos;
        res->media = media;
        res->min_t = min_t;
        res->max_t = max_t;
        res->desvio = desvio;
    }

    free(tempos);
    liberar_labirinto(lab);
    return 1;
}

static void imprimir_tabela_comparativa(const ResultadoBenchmark *resultados, int total, int repeticoes) {
    if (total <= 0) return;

    printf("\n=================================================================================================================\n");
    printf("                                   TABELA COMPARATIVA DE BENCHMARK (N = %d)\n", repeticoes);
    printf("=================================================================================================================\n");
    printf("%-24s %-11s %-8s %-9s %-8s %-16s %-12s %-12s %-12s\n",
           "Instancia", "Dimensoes", "Chaves", "Solucao", "Passos", "Tempo Medio(ms)", "Min(ms)", "Max(ms)", "Desvio(ms)");
    printf("-----------------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < total; i++) {
        char dim[32];
        snprintf(dim, sizeof(dim), "%dx%d", resultados[i].linhas, resultados[i].colunas);
        printf("%-24s %-11s %-8d %-9s %-8d %-16.4f %-12.4f %-12.4f %-12.4f\n",
               resultados[i].arquivo,
               dim,
               resultados[i].chaves,
               resultados[i].solucao ? "Sim" : "Nao",
               resultados[i].passos,
               resultados[i].media,
               resultados[i].min_t,
               resultados[i].max_t,
               resultados[i].desvio);
    }
    printf("=================================================================================================================\n\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <labirinto1.txt> [labirinto2.txt ...] [repeticoes]\n", argv[0]);
        return 1;
    }

    int repeticoes = NUM_REPETICOES_PADRAO;
    int qtd_arquivos = argc - 1;

    if (argc > 2) {
        int rep = atoi(argv[argc - 1]);
        if (rep > 0) {
            repeticoes = rep;
            qtd_arquivos = argc - 2;
        }
    }

    ResultadoBenchmark *resultados = (ResultadoBenchmark *)malloc(qtd_arquivos * sizeof(ResultadoBenchmark));
    int coletados = 0;

    for (int i = 1; i <= qtd_arquivos; i++) {
        ResultadoBenchmark res;
        if (executar_benchmark(argv[i], repeticoes, &res)) {
            if (resultados) {
                resultados[coletados++] = res;
            }
        }
    }

    if (resultados) {
        imprimir_tabela_comparativa(resultados, coletados, repeticoes);
        free(resultados);
    }

    return 0;
}
