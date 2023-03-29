#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAMANHO	29
typedef struct{
	int fitness;
	char frase[TAMANHO];
}POPULACAO;

#define MUTACAO	15
#define GERACOES 30
#define INDIVIDUOS 300

int gerarNumAleatorio(int n)
{
	int r = rand() % n;
	return r;
}

int fitness(POPULACAO copia, const char *alvo)
{
	int qtdDistintos = 0;

	for (int i = 0; i < TAMANHO; i++){
		if(copia.frase[i] != alvo[i])
			qtdDistintos++;
	}
	return qtdDistintos;
}

void mutacao(POPULACAO filho)
{
	const char alfabeto[] = "abcdefghijklmnopqrstuvxwz ";

	int r = gerarNumAleatorio(100);
	int posicao = gerarNumAleatorio(TAMANHO-2);

	if(r <= MUTACAO)
		filho.frase[posicao] = alfabeto[gerarNumAleatorio(28)];
}

POPULACAO recombinacaoUmPonto(POPULACAO pai, POPULACAO mae){
	int pontoDeCrossover = gerarNumAleatorio(TAMANHO-2);
	POPULACAO filho;

	for(int i = 0; i < TAMANHO; i++){
		if(i <= pontoDeCrossover)
			filho.frase[i] = pai.frase[i];
		else
			filho.frase[i] = mae.frase[i];
	}
	return filho;
}

POPULACAO torneio(POPULACAO A, POPULACAO B){
	if(A.fitness < B.fitness) return A;
	else return B;
}

POPULACAO selecaoPorTorneio(POPULACAO populacao[INDIVIDUOS],const char* alvo){
	POPULACAO novaPopulacao[INDIVIDUOS];
	POPULACAO melhor;
	melhor.fitness=RAND_MAX;

	for(int i = 0; i < INDIVIDUOS; i++){
		POPULACAO pai = torneio(populacao[gerarNumAleatorio(INDIVIDUOS-1)], populacao[gerarNumAleatorio(INDIVIDUOS-1)]);
		POPULACAO mae = torneio(populacao[gerarNumAleatorio(INDIVIDUOS-1)], populacao[gerarNumAleatorio(INDIVIDUOS-1)]);
		
		POPULACAO filho = recombinacaoUmPonto(pai, mae);
		mutacao(filho);
		filho.fitness = fitness(filho,alvo);

		if(filho.fitness < melhor.fitness) 
			melhor = filho;
		
		novaPopulacao[i] = filho;
	}

	for(int i = 0; i < INDIVIDUOS; i++)
		populacao[i] = novaPopulacao[i];

	return melhor;
}

void inicializa(POPULACAO populacao[INDIVIDUOS], const char *alvo)
{
	const char alfabeto[] = "abcdefghijklmnopqrstuvxwz ";

	int i = 0, j = 0;
	for (i = 0; i < INDIVIDUOS; i++){
		for(j = 0; j < TAMANHO-1; j++){
			if(j == 0 && i == 0){
				srand(time(NULL));
				int letra  = rand() % 25;
				populacao[i].frase[j] = alfabeto[letra];
			}
			else 
				populacao[i].frase[j] = alfabeto[gerarNumAleatorio(25)];
		}
		populacao[i].frase[j] = '\0';
		populacao[i].fitness = fitness(populacao[i], alvo);
	}
}

int main(void)
{
	const char alvo[] = "o tejo guarda grandes navios";
	
	int geracao = 0;
	POPULACAO populacao[INDIVIDUOS];

	inicializa(populacao, alvo);

	do{
		POPULACAO melhor;
		melhor.fitness=RAND_MAX;

		melhor = selecaoPorTorneio(populacao, alvo);

		printf("Iteracao %d, pai pontos %d: %s\n", geracao, melhor.fitness, melhor.frase);
		geracao++;

	}while (geracao <= GERACOES);

	return 0;
}