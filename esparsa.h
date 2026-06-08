#ifndef ESPARSA_H
#define ESPARSA_H

/* Representa uma rota em uma matriz esparsa de adjacencia. */
typedef struct NoRota {
    int destino;                 /* indice do aeroporto de destino */
    int distanciaKm;             /* distancia da rota em quilometros */
    struct NoRota *proximo;      /* proxima rota da mesma origem */
} NoRota;

NoRota **criarMatrizEsparsa(int capacidade);
void liberarMatrizEsparsa(NoRota **matrizEsparsa, int quantidade);

int inserirNaMatrizEsparsa(NoRota **matrizEsparsa, int origem, int destino, int distanciaKm);
int removerDaMatrizEsparsa(NoRota **matrizEsparsa, int origem, int destino);
int buscarNaMatrizEsparsa(NoRota **matrizEsparsa, int origem, int destino);

#endif
