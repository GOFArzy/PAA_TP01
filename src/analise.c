#include "analise.h"
#include <stdio.h>

void analise_iniciar(AnaliseMetricas *m) {
    if (!m) return;
    m->chamadas_recursivas = 0;
    m->profundidade_atual   = 0;
    m->profundidade_maxima  = 0;
    m->podas                = 0;
    m->retrocessos          = 0;
}

void analise_registrar_chamada(AnaliseMetricas *m, int profundidade) {
    if (!m) return;
    m->chamadas_recursivas++;
    m->profundidade_atual = profundidade;
    if (profundidade > m->profundidade_maxima)
        m->profundidade_maxima = profundidade;
}

void analise_registrar_poda(AnaliseMetricas *m) {
    if (!m) return;
    m->podas++;
}

void analise_registrar_retrocesso(AnaliseMetricas *m) {
    if (!m) return;
    m->retrocessos++;
}

void analise_imprimir(const AnaliseMetricas *m) {
    if (!m) return;
    printf("\n===== MODO ANALISE =====\n");
    printf("Chamadas recursivas totais : %ld\n", m->chamadas_recursivas);
    printf("Profundidade maxima        : %d\n",  m->profundidade_maxima);
    printf("Podas (ramificacoes ruins) : %ld\n", m->podas);
    printf("Retrocessos (backtracks)   : %ld\n", m->retrocessos);
    printf("========================\n\n");
}