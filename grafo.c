#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grafo.h"

static void copiarTexto(char *destino, const char *origem, int tamanho) {
    strncpy(destino, origem, tamanho - 1);
    destino[tamanho - 1] = '\0';
}

/*
 * Cria o grafo de aeroportos.
 *
 * A matriz e esparsa porque, em um sistema real, nem todo aeroporto possui
 * rota direta para todos os outros aeroportos. Guardar uma matriz completa
 * gastaria memoria com muitas posicoes vazias.
 *
 * Por isso, cada linha da matriz e uma lista encadeada:
 * - a posicao i do vetor representa o aeroporto de origem i;
 * - cada no da lista representa uma coluna preenchida, isto e, uma rota
 *   existente saindo desse aeroporto.
 */
Grafo *criarGrafo(int capacidade) {
    Grafo *grafo;
    int i;

    if (capacidade <= 0) {
        return NULL;
    }

    grafo = (Grafo *) malloc(sizeof(Grafo));
    if (grafo == NULL) {
        return NULL;
    }

    grafo->aeroportos = (Aeroporto *) malloc(sizeof(Aeroporto) * capacidade);
    grafo->matrizEsparsa = (NoRota **) malloc(sizeof(NoRota *) * capacidade);

    if (grafo->aeroportos == NULL || grafo->matrizEsparsa == NULL) {
        free(grafo->aeroportos);
        free(grafo->matrizEsparsa);
        free(grafo);
        return NULL;
    }

    grafo->quantidade = 0;
    grafo->capacidade = capacidade;

    for (i = 0; i < capacidade; i++) {
        grafo->matrizEsparsa[i] = NULL;
    }

    return grafo;
}

void liberarGrafo(Grafo *grafo) {
    int i;

    if (grafo == NULL) {
        return;
    }

    for (i = 0; i < grafo->quantidade; i++) {
        NoRota *atual = grafo->matrizEsparsa[i];

        while (atual != NULL) {
            NoRota *remover = atual;
            atual = atual->proximo;
            free(remover);
        }
    }

    free(grafo->matrizEsparsa);
    free(grafo->aeroportos);
    free(grafo);
}

int adicionarAeroporto(Grafo *grafo, const char *codigo, const char *nome, const char *cidade) {
    Aeroporto *aeroporto;

    if (grafo == NULL || codigo == NULL || nome == NULL || cidade == NULL) {
        return 0;
    }

    if (grafo->quantidade >= grafo->capacidade) {
        printf("Nao foi possivel adicionar. Capacidade maxima atingida.\n");
        return 0;
    }

    if (strlen(codigo) != 3) {
        printf("Codigo invalido. Use exatamente 3 letras, como GRU.\n");
        return 0;
    }

    if (buscarAeroportoPorCodigo(grafo, codigo) != -1) {
        printf("Ja existe aeroporto com o codigo %s.\n", codigo);
        return 0;
    }

    aeroporto = &grafo->aeroportos[grafo->quantidade];
    copiarTexto(aeroporto->codigo, codigo, TAM_CODIGO);
    copiarTexto(aeroporto->nome, nome, TAM_NOME);
    copiarTexto(aeroporto->cidade, cidade, TAM_CIDADE);

    grafo->quantidade++;
    return 1;
}

int buscarAeroportoPorCodigo(Grafo *grafo, const char *codigo) {
    int i;

    if (grafo == NULL || codigo == NULL) {
        return -1;
    }

    for (i = 0; i < grafo->quantidade; i++) {
        if (strcmp(grafo->aeroportos[i].codigo, codigo) == 0) {
            return i;
        }
    }

    return -1;
}

int adicionarRota(Grafo *grafo, const char *codigoOrigem, const char *codigoDestino, int distanciaKm) {
    int origem;
    int destino;
    NoRota *atual;
    NoRota *novaRota;

    if (grafo == NULL || codigoOrigem == NULL || codigoDestino == NULL) {
        return 0;
    }

    if (distanciaKm <= 0) {
        printf("Distancia invalida. Informe um valor maior que zero.\n");
        return 0;
    }

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

    atual = grafo->matrizEsparsa[origem];
    while (atual != NULL) {
        if (atual->destino == destino) {
            atual->distanciaKm = distanciaKm;
            printf("Rota ja existia. Distancia atualizada.\n");
            return 1;
        }
        atual = atual->proximo;
    }

    novaRota = (NoRota *) malloc(sizeof(NoRota));
    if (novaRota == NULL) {
        printf("Erro de memoria ao adicionar rota.\n");
        return 0;
    }

    novaRota->destino = destino;
    novaRota->distanciaKm = distanciaKm;
    novaRota->proximo = grafo->matrizEsparsa[origem];
    grafo->matrizEsparsa[origem] = novaRota;

    return 1;
}

int removerRota(Grafo *grafo, const char *codigoOrigem, const char *codigoDestino) {
    int origem;
    int destino;
    NoRota *atual;
    NoRota *anterior = NULL;

    if (grafo == NULL || codigoOrigem == NULL || codigoDestino == NULL) {
        return 0;
    }

    origem = buscarAeroportoPorCodigo(grafo, codigoOrigem);
    destino = buscarAeroportoPorCodigo(grafo, codigoDestino);

    if (origem == -1 || destino == -1) {
        printf("Aeroporto de origem ou destino nao encontrado.\n");
        return 0;
    }

    atual = grafo->matrizEsparsa[origem];
    while (atual != NULL) {
        if (atual->destino == destino) {
            if (anterior == NULL) {
                grafo->matrizEsparsa[origem] = atual->proximo;
            } else {
                anterior->proximo = atual->proximo;
            }

            free(atual);
            return 1;
        }

        anterior = atual;
        atual = atual->proximo;
    }

    printf("Rota nao encontrada.\n");
    return 0;
}

int verificarRota(Grafo *grafo, const char *codigoOrigem, const char *codigoDestino) {
    int origem;
    int destino;
    NoRota *atual;

    if (grafo == NULL || codigoOrigem == NULL || codigoDestino == NULL) {
        return 0;
    }

    origem = buscarAeroportoPorCodigo(grafo, codigoOrigem);
    destino = buscarAeroportoPorCodigo(grafo, codigoDestino);

    if (origem == -1 || destino == -1) {
        return 0;
    }

    atual = grafo->matrizEsparsa[origem];
    while (atual != NULL) {
        if (atual->destino == destino) {
            return atual->distanciaKm;
        }
        atual = atual->proximo;
    }

    return 0;
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

    adicionarAeroporto(grafo, "GRU", "Aeroporto Internacional de Guarulhos", "S\303\243o Paulo");
    adicionarAeroporto(grafo, "VCP", "Aeroporto de Viracopos", "Campinas");
    adicionarAeroporto(grafo, "CGH", "Aeroporto de Congonhas", "S\303\243o Paulo");
    adicionarAeroporto(grafo, "GIG", "Aeroporto do Gale\303\243o", "Rio de Janeiro");
    adicionarAeroporto(grafo, "BSB", "Aeroporto Internacional de Bras\303\255lia", "Bras\303\255lia");

    adicionarRota(grafo, "GRU", "VCP", 100);
    adicionarRota(grafo, "GRU", "GIG", 360);
    adicionarRota(grafo, "VCP", "BSB", 800);
    adicionarRota(grafo, "CGH", "GRU", 35);
    adicionarRota(grafo, "BSB", "GIG", 930);
}
