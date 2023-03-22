#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char alvo[] = "o tejo guarda grandes navios";
const char alfabeto[] = "abcdefghijklmnopqrstuvxwz ";

#define TAMANHO (sizeof(alvo) / sizeof(char))
#define ESCOLHA (sizeof(alfabeto) - 1)

#define MUTACAO 15
#define COPIAS 300
#define GERACOES 300

int gerarNumAleatorio(int n)
{
	int r = rand() % n;
	return r;
}

int grauDeDiferenca(const char *copia)
{
	int i = 0, qtdDistintos = 0;
	for (i = 0; i < TAMANHO; i++){
		if(copia[i] != alvo[i])
			qtdDistintos++;
	}
	return qtdDistintos;
}

int selecaoPorTorneio(char populacao[COPIAS][TAMANHO]){

	int melhor=RAND_MAX, segundoMelhor=RAND_MAX;
	int unfit = 0, pai = 0, mae = 0;

	for(int i = 2; i < COPIAS; i++){
		unfit = grauDeDiferenca(populacao[i]);
		if(unfit < melhor){
			melhor = unfit;
			pai = i;
		}
		else if(unfit < segundoMelhor){
			segundoMelhor = unfit;
			mae = i;
		}
	}

	strcpy(populacao[0], populacao[pai]);
	strcpy(populacao[1], populacao[mae]);

	return melhor;
}

void mutacao(char *descendente)
{
	int r = gerarNumAleatorio(100);
	int posicao = gerarNumAleatorio(TAMANHO-2);
	if(r <= MUTACAO)
		descendente[posicao] = alfabeto[gerarNumAleatorio(ESCOLHA)];
}

void recombinacaoUmPonto(char *pai, char *mae, char *filho){
	int pontoDeCrossover = TAMANHO/2;

	for(int i = 0; i < TAMANHO; i++){
		if(i <= pontoDeCrossover)
			filho[i] = pai[i];
		else
			filho[i] = mae[i];
	}
}

int inicializa(char *primeiroDescendente)
{
	int letraAleatoria = 0, i = 0;
	srand(time(NULL));
	letraAleatoria = rand() % ESCOLHA;

	primeiroDescendente[0] = alfabeto[letraAleatoria];

	for(i = 1; i < TAMANHO-1; i++)
		primeiroDescendente[i] = alfabeto[gerarNumAleatorio(ESCOLHA)];

	primeiroDescendente[i] = '\0';
	
	return letraAleatoria;
}

int main(void)
{
	int iteracao = 0;
	char populacao[COPIAS][TAMANHO] = {'\0'};

	// Inicializamos o pai e a mae
	inicializa(populacao[0]);
	strcpy(populacao[1],alfabeto);
	
	do{
		for (int i = 2; i < COPIAS; i++){
			recombinacaoUmPonto(populacao[i-2], populacao[i-1], populacao[i]);
			mutacao(populacao[i]);
		}

		int melhor = selecaoPorTorneio(populacao);

		printf("Iteracao %d, pai pontos %d: %s\n", iteracao, melhor, populacao[0]);
		iteracao++;

	}while (iteracao <= GERACOES);

	return 0;
}