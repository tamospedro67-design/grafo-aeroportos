#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grafo.h"

#define CAPACIDADE_MAXIMA 50
#define TAM_ENTRADA_CODIGO 20

static void limparBufferEntrada(void) {
    int c;

    /* Remove caracteres que ficaram no teclado apos uma leitura. */
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

static void lerTexto(const char *mensagem, char *destino, int tamanho) {
    int encontrouQuebra;

    printf("%s", mensagem);

    if (fgets(destino, tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }

    encontrouQuebra = (strchr(destino, '\n') != NULL);
    destino[strcspn(destino, "\n")] = '\0';

    /* Se o usuario digitou mais do que cabia no vetor, descarta o restante. */
    if (!encontrouQuebra) {
        limparBufferEntrada();
    }
}

static int lerInteiro(const char *mensagem) {
    int valor;
    int resultado;

    printf("%s", mensagem);
    resultado = scanf("%d", &valor);
    limparBufferEntrada();

    if (resultado != 1) {
        return -1;
    }

    return valor;
}

static void mostrarMenu(void) {
    printf("\n===== Aeroportos em Grafo =====\n");
    printf("1 - Listar aeroportos\n");
    printf("2 - Listar rotas\n");
    printf("3 - Adicionar aeroporto\n");
    printf("4 - Adicionar rota\n");
    printf("5 - Remover rota\n");
    printf("6 - Verificar rota\n");
    printf("7 - Imprimir matriz esparsa\n");
    printf("0 - Sair\n");
}

int main(void) {
    Grafo *grafo;
    int opcao;

    /* Cria o grafo e carrega alguns aeroportos para demonstracao. */
    grafo = criarGrafo(CAPACIDADE_MAXIMA);
    if (grafo == NULL) {
        printf("Erro ao criar o grafo.\n");
        return 1;
    }

    carregarDadosExemplo(grafo);

    do {
        /* Repete o menu ate o usuario escolher a opcao de sair. */
        mostrarMenu();
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                listarAeroportos(grafo);
                break;

            case 2:
                listarRotas(grafo);
                break;

            case 3: {
                char codigo[TAM_ENTRADA_CODIGO];
                char nome[TAM_NOME];
                char cidade[TAM_CIDADE];

                lerTexto("Codigo do aeroporto (3 letras): ", codigo, TAM_ENTRADA_CODIGO);
                lerTexto("Nome do aeroporto: ", nome, TAM_NOME);
                lerTexto("Cidade: ", cidade, TAM_CIDADE);

                if (adicionarAeroporto(grafo, codigo, nome, cidade)) {
                    printf("Aeroporto adicionado com sucesso.\n");
                }
                break;
            }

            case 4: {
                char origem[TAM_ENTRADA_CODIGO];
                char destino[TAM_ENTRADA_CODIGO];
                int distancia;

                lerTexto("Codigo do aeroporto de origem: ", origem, TAM_ENTRADA_CODIGO);
                lerTexto("Codigo do aeroporto de destino: ", destino, TAM_ENTRADA_CODIGO);
                distancia = lerInteiro("Distancia em km: ");

                if (adicionarRota(grafo, origem, destino, distancia)) {
                    printf("Rota adicionada com sucesso.\n");
                }
                break;
            }

            case 5: {
                char origem[TAM_ENTRADA_CODIGO];
                char destino[TAM_ENTRADA_CODIGO];

                lerTexto("Codigo do aeroporto de origem: ", origem, TAM_ENTRADA_CODIGO);
                lerTexto("Codigo do aeroporto de destino: ", destino, TAM_ENTRADA_CODIGO);

                if (removerRota(grafo, origem, destino)) {
                    printf("Rota removida com sucesso.\n");
                }
                break;
            }

            case 6: {
                char origem[TAM_ENTRADA_CODIGO];
                char destino[TAM_ENTRADA_CODIGO];
                int distancia;

                lerTexto("Codigo do aeroporto de origem: ", origem, TAM_ENTRADA_CODIGO);
                lerTexto("Codigo do aeroporto de destino: ", destino, TAM_ENTRADA_CODIGO);

                distancia = verificarRota(grafo, origem, destino);
                if (distancia > 0) {
                    printf("Existe rota %s -> %s com %d km.\n", origem, destino, distancia);
                } else {
                    printf("Nao existe rota direta entre os aeroportos informados.\n");
                }
                break;
            }

            case 7:
                imprimirMatrizEsparsa(grafo);
                break;

            case 0:
                printf("Encerrando o programa.\n");
                break;

            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    } while (opcao != 0);

    liberarGrafo(grafo);
    return 0;
}
