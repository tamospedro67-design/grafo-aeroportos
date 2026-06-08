#ifndef GRAFO_H
#define GRAFO_H

#define TAM_CODIGO 4
#define TAM_NOME 100
#define TAM_CIDADE 60

/* Representa um aeroporto, que sera um vertice do grafo. */
typedef struct {
    char codigo[TAM_CODIGO];
    char nome[TAM_NOME];
    char cidade[TAM_CIDADE];
} Aeroporto;

#include "esparsa.h"

/* Guarda todos os aeroportos e as rotas em formato de matriz esparsa. */
typedef struct {
    Aeroporto *aeroportos;       /* vetor com os aeroportos cadastrados */
    NoRota **matrizEsparsa;      /* cada posicao aponta para a lista de rotas */
    int quantidade;              /* total de aeroportos cadastrados */
    int capacidade;              /* limite maximo de aeroportos */
} Grafo;

Grafo *criarGrafo(int capacidade);
void liberarGrafo(Grafo *grafo);

int adicionarAeroporto(Grafo *grafo, const char *codigo, const char *nome, const char *cidade);
int buscarAeroportoPorCodigo(Grafo *grafo, const char *codigo);

int adicionarRota(Grafo *grafo, const char *codigoOrigem, const char *codigoDestino, int distanciaKm);
int removerRota(Grafo *grafo, const char *codigoOrigem, const char *codigoDestino);
int verificarRota(Grafo *grafo, const char *codigoOrigem, const char *codigoDestino);

void listarAeroportos(Grafo *grafo);
void listarRotas(Grafo *grafo);
void imprimirMatrizEsparsa(Grafo *grafo);
void carregarDadosExemplo(Grafo *grafo);

#endif
