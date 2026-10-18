#ifndef SAIDA_H
#define SAIDA_H

#include "mapa.h"

/* Impressao da sequencia de coordenadas no formato [l,c],[l,c],... */
void imprimir_caminho(const Caminho *caminho);

/* Mensagem padrao quando nao ha solucao viavel */
void imprimir_sem_solucao(void);

/* Gravacao do caminho formatado em arquivo texto */
int salvar_caminho_arquivo(const char *nome_arquivo, const Caminho *caminho);

/* Exibicao de resumo com metricas da execucao */
void imprimir_resumo(int encontrou_solucao, const Caminho *caminho, double tempo_ms, long long chamadas_rec, int nivel_max_rec);

#endif /* SAIDA_H */
