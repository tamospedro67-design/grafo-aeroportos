#include <stdlib.h>

#include "esparsa.h"

NoRota **criarMatrizEsparsa(int capacidade) {
    NoRota **matrizEsparsa;
    int i;

    if (capacidade <= 0) {
        return NULL;
    }

    matrizEsparsa = (NoRota **) malloc(sizeof(NoRota *) * capacidade);
    if (matrizEsparsa == NULL) {
        return NULL;
    }

    for (i = 0; i < capacidade; i++) {
        matrizEsparsa[i] = NULL;
    }

    return matrizEsparsa;
}

void liberarMatrizEsparsa(NoRota **matrizEsparsa, int quantidade) {
    int i;

    if (matrizEsparsa == NULL) {
        return;
    }

    for (i = 0; i < quantidade; i++) {
        NoRota *atual = matrizEsparsa[i];

        while (atual != NULL) {
            NoRota *remover = atual;
            atual = atual->proximo;
            free(remover);
        }
    }

    free(matrizEsparsa);
}

int inserirNaMatrizEsparsa(NoRota **matrizEsparsa, int origem, int destino, int distanciaKm) {
    NoRota *atual;
    NoRota *novaRota;

    if (matrizEsparsa == NULL || distanciaKm <= 0) {
        return 0;
    }

    atual = matrizEsparsa[origem];
    while (atual != NULL) {
        if (atual->destino == destino) {
            atual->distanciaKm = distanciaKm;
            return 1;
        }

        atual = atual->proximo;
    }

    novaRota = (NoRota *) malloc(sizeof(NoRota));
    if (novaRota == NULL) {
        return 0;
    }

    novaRota->destino = destino;
    novaRota->distanciaKm = distanciaKm;
    novaRota->proximo = matrizEsparsa[origem];
    matrizEsparsa[origem] = novaRota;

    return 1;
}

int removerDaMatrizEsparsa(NoRota **matrizEsparsa, int origem, int destino) {
    NoRota *atual;
    NoRota *anterior = NULL;

    if (matrizEsparsa == NULL) {
        return 0;
    }

    atual = matrizEsparsa[origem];
    while (atual != NULL) {
        if (atual->destino == destino) {
            if (anterior == NULL) {
                matrizEsparsa[origem] = atual->proximo;
            } else {
                anterior->proximo = atual->proximo;
            }

            free(atual);
            return 1;
        }

        anterior = atual;
        atual = atual->proximo;
    }

    return 0;
}

int buscarNaMatrizEsparsa(NoRota **matrizEsparsa, int origem, int destino) {
    NoRota *atual;

    if (matrizEsparsa == NULL) {
        return 0;
    }

    atual = matrizEsparsa[origem];
    while (atual != NULL) {
        if (atual->destino == destino) {
            return atual->distanciaKm;
        }

        atual = atual->proximo;
    }

    return 0;
}
