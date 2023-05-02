#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define LETRAS 55
#define TAMANHO	117

typedef struct{
	int fitness;
	char frase[TAMANHO];
}POPULACAO;

#define MUTACAO	15
#define GERACOES 160
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
	const char alfabeto[] = "ABCDEFGHIJKLMNOPQRSTUVXWZabcdefghijklmnopqrstuvxwz,\n.  ";

	int r = gerarNumAleatorio(100);
	int posicao = gerarNumAleatorio(TAMANHO-2);

	if(r <= MUTACAO)
		filho.frase[posicao] = alfabeto[gerarNumAleatorio(LETRAS)];
}

POPULACAO recombinacaoDoisPontos(POPULACAO pai, POPULACAO mae){
	int pontoDeCrossoverUm = gerarNumAleatorio(TAMANHO-2);
	int pontoDeCrossoverDois = gerarNumAleatorio(TAMANHO-2);
	POPULACAO filho;

	if(pontoDeCrossoverDois < pontoDeCrossoverUm){
		int aux = pontoDeCrossoverDois;
		pontoDeCrossoverDois = pontoDeCrossoverUm;
		pontoDeCrossoverUm = aux;
	}

	for(int i = 0; i < TAMANHO; i++){
		if(i <= pontoDeCrossoverUm)
			filho.frase[i] = pai.frase[i];
		else if(i >= pontoDeCrossoverUm && i <=pontoDeCrossoverDois)
			filho.frase[i] = mae.frase[i];
		else
			filho.frase[i] = pai.frase[i];
	}
	return filho;
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

void melhores(POPULACAO populacao[INDIVIDUOS]){
	POPULACAO pai, mae;
	pai.fitness = RAND_MAX;
	mae.fitness = RAND_MAX;

	for(int i = 0; i < INDIVIDUOS; i++){
		if(populacao[i].fitness < pai.fitness)
			pai = populacao[i];
		else if(populacao[i].fitness < mae.fitness)
			mae = populacao[i];
	}

	populacao[0] = pai;
	populacao[1] = mae;
}

POPULACAO selecaoPorElitismo(POPULACAO populacao[INDIVIDUOS],const char* alvo){
	POPULACAO melhor;

	for(int i = 2; i < INDIVIDUOS; i++){
		melhores(populacao);
		
		POPULACAO filho = recombinacaoDoisPontos(populacao[i-2], populacao[i-1]);
		mutacao(filho);
		filho.fitness = fitness(filho,alvo);
		
		populacao[i] = filho;
	}
	melhor = populacao[0];

	return melhor;
}

POPULACAO reproducao(POPULACAO populacao[INDIVIDUOS],const char* alvo){
	POPULACAO novaPopulacao[INDIVIDUOS];
	POPULACAO melhor;
	melhor.fitness=RAND_MAX;

	int N = 3;

	for(int i = 0; i < INDIVIDUOS; i++){
		POPULACAO pai = selecaoPorTorneio(populacao, N);
		POPULACAO mae = selecaoPorTorneio(populacao, N);
		
		POPULACAO filho = recombinacaoDoisPontos(pai, mae);
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
	const char alfabeto[] = "ABCDEFGHIJKLMNOPQRSTUVXWZabcdefghijklmnopqrstuvxwz,\n.  ";

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

	const char alvo[] = "O Tejo tem grandes navios.\nE navega nele ainda,\nPara aqueles que veem em tudo o que la nao esta,\nA memoria das naus.";
	
	int geracao = 0;
	POPULACAO populacao[INDIVIDUOS];

	srand(time(NULL));
	inicializa(populacao, alvo);

	do{
		POPULACAO melhor;
		melhor.fitness=RAND_MAX;

		if(geracao <= (GERACOES - (GERACOES/4)))
			melhor = reproducao(populacao, alvo);
		else 
			melhor = selecaoPorElitismo(populacao, alvo);

		printf("\nIteracao %d, melhor fitness %d.\n", geracao, melhor.fitness);
		printf("%s\n", melhor.frase);
		geracao++;

	}while (geracao <= GERACOES);

	fim = clock();

	total = (double)(fim-inicio)/CLOCKS_PER_SEC;
	printf("Tempo total gasto pela CPU: %lf\n", total);
	return 0;
}