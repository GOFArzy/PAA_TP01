#ifndef ANALISE_H
#define ANALISE_H

#include "mapa.h"

typedef struct {
    long chamadas_recursivas;
    int  profundidade_atual;
    int  profundidade_maxima;
    long podas;
    long retrocessos;
} AnaliseMetricas;

void analise_iniciar(AnaliseMetricas *m);
void analise_registrar_chamada(AnaliseMetricas *m, int profundidade);
void analise_registrar_poda(AnaliseMetricas *m);
void analise_registrar_retrocesso(AnaliseMetricas *m);
void analise_imprimir(const AnaliseMetricas *m);

#endif