#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define LETRAS 26
#define TAMANHO	29
typedef struct{
	int fitness;
	char frase[TAMANHO];
}POPULACAO;

#define MUTACAO	15
#define GERACOES 40
#define INDIVIDUOS 400

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
	const char alfabeto[TAMANHO] = "abcdefghijklmnopqrstuvxwz  ";

	int r = gerarNumAleatorio(100);
	int posicao = gerarNumAleatorio(TAMANHO-2);

	if(r <= MUTACAO)
		filho.frase[posicao] = alfabeto[gerarNumAleatorio(LETRAS)];
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

POPULACAO selecaoPorTorneio(POPULACAO populacao[INDIVIDUOS], int N){
	POPULACAO melhor = populacao[gerarNumAleatorio(INDIVIDUOS-1)];
	int i = 2;

	do{
		POPULACAO aux = populacao[gerarNumAleatorio(INDIVIDUOS-1)];
		if(aux.fitness < melhor.fitness){
			melhor = aux;
		}
		i++;
	}while(i < N);

	return melhor;
}

POPULACAO reproducao(POPULACAO populacao[INDIVIDUOS],const char* alvo){
	POPULACAO novaPopulacao[INDIVIDUOS];
	POPULACAO melhor;
	melhor.fitness=RAND_MAX;

	for(int i = 0; i < INDIVIDUOS; i++){
		POPULACAO pai = selecaoPorTorneio(populacao, 3);
		POPULACAO mae = selecaoPorTorneio(populacao, 3);
		
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
	const char alfabeto[TAMANHO] = "abcdefghijklmnopqrstuvxwz  ";

	int i = 0, j = 0;
	for (i = 0; i < INDIVIDUOS; i++){
		for(j = 0; j < TAMANHO-1; j++){
			populacao[i].frase[j] = alfabeto[gerarNumAleatorio(LETRAS)];
		}
		populacao[i].frase[j] = '\0';
		populacao[i].fitness = fitness(populacao[i], alvo);
	}
}

int main(void)
{
	clock_t inicio, fim;
	double total = 0;
	inicio = clock();

	const char alvo[] = "o tejo guarda grandes navios";
	
	int geracao = 0;
	POPULACAO populacao[INDIVIDUOS];

	srand(time(NULL));
	inicializa(populacao, alvo);

	do{
		POPULACAO melhor;
		melhor.fitness=RAND_MAX;

		melhor = reproducao(populacao, alvo);

		printf("Iteracao %d, pai pontos %d: %s\n", geracao, melhor.fitness, melhor.frase);
		geracao++;

	}while (geracao <= GERACOES);

	fim = clock();

	total = (double)(fim-inicio)/CLOCKS_PER_SEC;
	printf("Tempo total gasto pela CPU: %lf\n", total);
	return 0;
}