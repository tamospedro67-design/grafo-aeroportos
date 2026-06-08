#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "grafo.h"

static void copiarTexto(char *destino, const char *origem, int tamanho) {
    /* Copia o texto sem deixar passar do tamanho do vetor. */
    strncpy(destino, origem, tamanho - 1);
    destino[tamanho - 1] = '\0';
}

static int normalizarCodigoAeroporto(const char *codigo, char codigoNormalizado[TAM_CODIGO]) {
    int i;

    if (codigo == NULL || strlen(codigo) != 3) {
        return 0;
    }

    for (i = 0; i < 3; i++) {
        unsigned char caractere = (unsigned char) codigo[i];

        if (!isalpha(caractere)) {
            return 0;
        }

        codigoNormalizado[i] = (char) toupper(caractere);
    }

    codigoNormalizado[3] = '\0';
    return 1;
}

Grafo *criarGrafo(int capacidade) {
    Grafo *grafo;

    if (capacidade <= 0) {
        return NULL;
    }

    grafo = (Grafo *) malloc(sizeof(Grafo));
    if (grafo == NULL) {
        return NULL;
    }

    grafo->aeroportos = (Aeroporto *) malloc(sizeof(Aeroporto) * capacidade);
    grafo->matrizEsparsa = criarMatrizEsparsa(capacidade);

    if (grafo->aeroportos == NULL || grafo->matrizEsparsa == NULL) {
        free(grafo->aeroportos);
        liberarMatrizEsparsa(grafo->matrizEsparsa, 0);
        free(grafo);
        return NULL;
    }

    grafo->quantidade = 0;
    grafo->capacidade = capacidade;

    return grafo;
}

void liberarGrafo(Grafo *grafo) {
    if (grafo == NULL) {
        return;
    }

    liberarMatrizEsparsa(grafo->matrizEsparsa, grafo->quantidade);
    free(grafo->aeroportos);
    free(grafo);
}

int adicionarAeroporto(Grafo *grafo, const char *codigo, const char *nome, const char *cidade) {
    Aeroporto *aeroporto;
    char codigoNormalizado[TAM_CODIGO];

    if (grafo == NULL || codigo == NULL || nome == NULL || cidade == NULL) {
        return 0;
    }

    if (grafo->quantidade >= grafo->capacidade) {
        printf("Nao foi possivel adicionar. Capacidade maxima atingida.\n");
        return 0;
    }

    if (!normalizarCodigoAeroporto(codigo, codigoNormalizado)) {
        printf("Codigo invalido. Use exatamente 3 letras, como GRU.\n");
        return 0;
    }

    if (buscarAeroportoPorCodigo(grafo, codigoNormalizado) != -1) {
        printf("Ja existe aeroporto com o codigo %s.\n", codigoNormalizado);
        return 0;
    }

    /* O novo aeroporto entra na primeira posicao livre do vetor. */
    aeroporto = &grafo->aeroportos[grafo->quantidade];
    copiarTexto(aeroporto->codigo, codigoNormalizado, TAM_CODIGO);
    copiarTexto(aeroporto->nome, nome, TAM_NOME);
    copiarTexto(aeroporto->cidade, cidade, TAM_CIDADE);

    grafo->quantidade++;
    return 1;
}

int buscarAeroportoPorCodigo(Grafo *grafo, const char *codigo) {
    int i;
    char codigoNormalizado[TAM_CODIGO];

    if (grafo == NULL || codigo == NULL) {
        return -1;
    }

    if (!normalizarCodigoAeroporto(codigo, codigoNormalizado)) {
        return -1;
    }

    /* Procura o codigo comparando com cada aeroporto ja cadastrado. */
    for (i = 0; i < grafo->quantidade; i++) {
        if (strcmp(grafo->aeroportos[i].codigo, codigoNormalizado) == 0) {
            return i;
        }
    }

    return -1;
}

int adicionarRota(Grafo *grafo, const char *codigoOrigem, const char *codigoDestino, int distanciaKm) {
    int origem;
    int destino;
    int distanciaAnterior;

    if (grafo == NULL || codigoOrigem == NULL || codigoDestino == NULL) {
        return 0;
    }

    if (distanciaKm <= 0) {
        printf("Distancia invalida. Informe um valor maior que zero.\n");
        return 0;
    }

    /* Converte os codigos informados para os indices usados internamente. */
    origem = buscarAeroportoPorCodigo(grafo, codigoOrigem);
    destino = buscarAeroportoPorCodigo(grafo, codigoDestino);

    if (origem == -1 || destino == -1) {
        printf("Aeroporto de origem ou destino nao encontrado.\n");
        return 0;
    }

    if (origem == destino) {
        printf("Origem e destino nao podem ser o mesmo aeroporto.\n");
        return 0;
    }

    distanciaAnterior = buscarNaMatrizEsparsa(grafo->matrizEsparsa, origem, destino);
    if (!inserirNaMatrizEsparsa(grafo->matrizEsparsa, origem, destino, distanciaKm)) {
        printf("Erro de memoria ao adicionar rota.\n");
        return 0;
    }

    if (distanciaAnterior > 0) {
        printf("Rota ja existia. Distancia atualizada.\n");
    }

    return 1;
}

int removerRota(Grafo *grafo, const char *codigoOrigem, const char *codigoDestino) {
    int origem;
    int destino;

    if (grafo == NULL || codigoOrigem == NULL || codigoDestino == NULL) {
        return 0;
    }

    origem = buscarAeroportoPorCodigo(grafo, codigoOrigem);
    destino = buscarAeroportoPorCodigo(grafo, codigoDestino);

    if (origem == -1 || destino == -1) {
        printf("Aeroporto de origem ou destino nao encontrado.\n");
        return 0;
    }

    if (removerDaMatrizEsparsa(grafo->matrizEsparsa, origem, destino)) {
        return 1;
    }

    printf("Rota nao encontrada.\n");
    return 0;
}

int verificarRota(Grafo *grafo, const char *codigoOrigem, const char *codigoDestino) {
    int origem;
    int destino;

    if (grafo == NULL || codigoOrigem == NULL || codigoDestino == NULL) {
        return 0;
    }

    origem = buscarAeroportoPorCodigo(grafo, codigoOrigem);
    destino = buscarAeroportoPorCodigo(grafo, codigoDestino);

    if (origem == -1 || destino == -1) {
        return 0;
    }

    return buscarNaMatrizEsparsa(grafo->matrizEsparsa, origem, destino);
}

void listarAeroportos(Grafo *grafo) {
    int i;

    if (grafo == NULL) {
        return;
    }

    if (grafo->quantidade == 0) {
        printf("Nenhum aeroporto cadastrado.\n");
        return;
    }

    printf("\nAeroportos cadastrados:\n");
    for (i = 0; i < grafo->quantidade; i++) {
        /* O indice mostrado e o mesmo usado internamente pelo grafo. */
        printf("%d - %s | %s | %s\n",
               i,
               grafo->aeroportos[i].codigo,
               grafo->aeroportos[i].nome,
               grafo->aeroportos[i].cidade);
    }
}

void listarRotas(Grafo *grafo) {
    int i;
    int encontrou = 0;

    if (grafo == NULL) {
        return;
    }

    printf("\nRotas cadastradas:\n");
    for (i = 0; i < grafo->quantidade; i++) {
        /* Cada linha da matriz esparsa e uma lista de rotas da mesma origem. */
        NoRota *atual = grafo->matrizEsparsa[i];

        while (atual != NULL) {
            printf("%s -> %s | %d km\n",
                   grafo->aeroportos[i].codigo,
                   grafo->aeroportos[atual->destino].codigo,
                   atual->distanciaKm);
            encontrou = 1;
            atual = atual->proximo;
        }
    }

    if (!encontrou) {
        printf("Nenhuma rota cadastrada.\n");
    }
}

void imprimirMatrizEsparsa(Grafo *grafo) {
    int i;

    if (grafo == NULL) {
        return;
    }

    printf("\nMatriz esparsa de adjacencia:\n");
    printf("Cada linha mostra apenas as colunas preenchidas, ou seja, rotas existentes.\n\n");

    for (i = 0; i < grafo->quantidade; i++) {
        NoRota *atual = grafo->matrizEsparsa[i];

        /* Mostra somente as posicoes preenchidas da matriz. */
        printf("[%s] -> ", grafo->aeroportos[i].codigo);

        if (atual == NULL) {
            printf("NULL\n");
        } else {
            while (atual != NULL) {
                printf("(%s, %d km) -> ",
                       grafo->aeroportos[atual->destino].codigo,
                       atual->distanciaKm);
                atual = atual->proximo;
            }

            printf("NULL\n");
        }
    }
}

void carregarDadosExemplo(Grafo *grafo) {
    if (grafo == NULL) {
        return;
    }

    /* Dados iniciais para testar o programa sem cadastrar tudo manualmente. */
    adicionarAeroporto(grafo, "GRU", "Aeroporto Internacional de Guarulhos", "Sao Paulo");
    adicionarAeroporto(grafo, "VCP", "Aeroporto de Viracopos", "Campinas");
    adicionarAeroporto(grafo, "CGH", "Aeroporto de Congonhas", "Sao Paulo");
    adicionarAeroporto(grafo, "GIG", "Aeroporto do Galeao", "Rio de Janeiro");
    adicionarAeroporto(grafo, "BSB", "Aeroporto Internacional de Brasilia", "Brasilia");

    adicionarRota(grafo, "GRU", "VCP", 100);
    adicionarRota(grafo, "GRU", "GIG", 360);
    adicionarRota(grafo, "VCP", "BSB", 800);
    adicionarRota(grafo, "CGH", "GRU", 35);
    adicionarRota(grafo, "BSB", "GIG", 930);
}
