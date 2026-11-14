#ifndef MAPA_H
#define MAPA_H

// mapa.h - Leitura e representacao da caverna 
#define MAPA_OK            0
#define MAPA_ERRO_ARQUIVO -1   //nao foi possivel abrir/ler o arquivo 
#define MAPA_ERRO_MEMORIA -2   // falha de alocacao                     
#define MAPA_ERRO_FORMATO -3   // conteudo fora do formato esperado     

typedef struct {
    int linhas, colunas;
    int hp_inicial;
    int **valor;                        
    int inicio_linha, inicio_coluna;    //posicao de I                     
    int fim_linha, fim_coluna;          // posicao de F                     
} Mapa;

int mapa_ler(const char *arquivo, Mapa *mapa);
void mapa_liberar(Mapa *mapa);

#endif /* MAPA_H */