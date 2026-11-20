#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mapa.h"
#include "dp.h"
#include "caminho.h"
#include "saida.h"

#define ARQUIVO_SAIDA_PADRAO "resultado.txt"

// Exibe informacoes de uso e ajuda do programa
static void exibir_ajuda(const char *programa)
{
    printf("=================================================================\n");
    printf("  TP02 - PAA: Escape da Caverna com Programacao Dinamica\n");
    printf("=================================================================\n\n");
    printf("Uso: %s <arquivo_mapa> [arquivo_saida] [opcoes]\n\n", programa);
    printf("Parametros:\n");
    printf("  <arquivo_mapa>    Caminho do arquivo da caverna a ser processado (obrigatorio).\n");
    printf("  [arquivo_saida]   Caminho para gravacao do resultado (padrao: '%s').\n\n", ARQUIVO_SAIDA_PADRAO);
    printf("Opcoes:\n");
    printf("  -v, --verbose     Exibe relatorio detalhado do percurso e mapa ASCII no terminal.\n");
    printf("  -h, --help        Exibe esta mensagem de ajuda e encerra.\n\n");
    printf("Formato da saida em '%s':\n", ARQUIVO_SAIDA_PADRAO);
    printf("  - Se houver rota: uma coordenada por linha (linha coluna), de I ate F.\n");
    printf("  - Se nao houver rota viavel: exclusivamente a palavra '%s'.\n", SAIDA_TEXTO_SEM_SOLUCAO);
}

int main(int argc, char *argv[])
{
    const char *arquivo_mapa = NULL;
    const char *arquivo_saida = ARQUIVO_SAIDA_PADRAO;
    int verbose = 0;

    // Analise de argumentos de linha de comando
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            exibir_ajuda(argv[0]);
            return EXIT_SUCCESS;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
        } else if (arquivo_mapa == NULL) {
            arquivo_mapa = argv[i];
        } else if (strcmp(arquivo_saida, ARQUIVO_SAIDA_PADRAO) == 0) {
            arquivo_saida = argv[i];
        } else {
            fprintf(stderr, "Aviso: argumento adicional ignorado: '%s'\n", argv[i]);
        }
    }

    // Validacao do argumento obrigatorio
    if (arquivo_mapa == NULL) {
        fprintf(stderr, "Erro: Caminho do arquivo de mapa nao fornecido.\n");
        fprintf(stderr, "Uso: %s <arquivo_mapa> [arquivo_saida] [opcoes]\n", argv[0]);
        fprintf(stderr, "Execute '%s --help' para mais informacoes.\n", argv[0]);
        return EXIT_FAILURE;
    }

    // 1. Modulo de Leitura do Mapa (Rafa)
    Mapa mapa;
    int status_mapa = mapa_ler(arquivo_mapa, &mapa);
    if (status_mapa != MAPA_OK) {
        switch (status_mapa) {
            case MAPA_ERRO_ARQUIVO:
                fprintf(stderr, "Erro: Nao foi possivel abrir o arquivo '%s' (arquivo inexistente ou sem permissao).\n", arquivo_mapa);
                break;
            case MAPA_ERRO_MEMORIA:
                fprintf(stderr, "Erro: Falha na alocacao de memoria ao carregar o mapa.\n");
                break;
            case MAPA_ERRO_FORMATO:
                fprintf(stderr, "Erro: Formato invalido no arquivo '%s'. Verifique dimensoes, tokens e posicoes de I e F.\n", arquivo_mapa);
                break;
            default:
                fprintf(stderr, "Erro desconhecido ao ler o mapa (codigo %d).\n", status_mapa);
                break;
        }
        return EXIT_FAILURE;
    }

    // 2. Modulo de Programacao Dinamica (Tomas)
    Solucao solucao;
    int tem_solucao = dp_resolver(&mapa, &solucao);
    if (solucao.melhor_hp == NULL || solucao.pai == NULL) {
        fprintf(stderr, "Erro: Falha de alocacao de memoria no modulo de Programacao Dinamica.\n");
        mapa_liberar(&mapa);
        return EXIT_FAILURE;
    }

    // 3. Modulo de Reconstrucao e Validacao do Caminho (Otavio)
    Coordenada *caminho = NULL;
    int quantidade_passos = 0;
    int status_caminho = CAMINHO_SEM_SOLUCAO;

    if (tem_solucao && solucao.existe_caminho) {
        status_caminho = caminho_reconstruir(&mapa, &solucao, &caminho, &quantidade_passos);
        if (status_caminho == CAMINHO_OK) {
            int status_val = caminho_validar(&mapa, caminho, quantidade_passos);
            if (status_val != CAMINHO_OK) {
                fprintf(stderr, "Aviso: Rota reconstruida falhou na validacao (codigo %d).\n", status_val);
                status_caminho = CAMINHO_SEM_SOLUCAO;
            }
        }
    }

    // 4. Modulo de Gravacao e Exibicao de Saida (Gabs)
    if (status_caminho == CAMINHO_OK && caminho != NULL && quantidade_passos > 0) {
        int status_saida = resultado_gravar(arquivo_saida, 1, caminho, quantidade_passos);
        if (status_saida != SAIDA_OK) {
            fprintf(stderr, "Erro: Falha ao gravar resultado em '%s'.\n", arquivo_saida);
        } else {
            printf("[SUCESSO] Rota encontrada! Passos: %d | HP final maximo: %d | Gravado em: %s\n",
                   quantidade_passos, solucao.hp_final, arquivo_saida);
        }

        // Exibicao detalhada e visualizacao ASCII caso requisitado
        if (verbose) {
            caminho_imprimir_relatorio(stdout, &mapa, caminho, quantidade_passos);
            caminho_desenhar_ascii(stdout, &mapa, caminho, quantidade_passos);
        }
    } else {
        int status_saida = resultado_gravar(arquivo_saida, 0, NULL, 0);
        if (status_saida != SAIDA_OK) {
            fprintf(stderr, "Erro: Falha ao gravar resultado em '%s'.\n", arquivo_saida);
        } else {
            printf("[SEM SOLUCAO] Nao ha rota viavel ate a saida F. Gravado \"%s\" em '%s'.\n",
                   SAIDA_TEXTO_SEM_SOLUCAO, arquivo_saida);
        }
    }

    // 5. Liberacao de memoria de todos os modulos
    caminho_liberar(caminho);
    dp_liberar(&mapa, &solucao);
    mapa_liberar(&mapa);

    return EXIT_SUCCESS;
}
