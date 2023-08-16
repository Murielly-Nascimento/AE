#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <stdbool.h>
#include <math.h>

#define TABULEIRO 25

typedef struct{
	int fitness;
	int indiceMaiorSeq;
	int tour[TABULEIRO];
}POPULACAO;

#define TORNEIO 3
#define MUTACAO 15
#define GERACOES 20
#define INDIVIDUOS 100

int gerarNumAleatorio(int N)
{
	int r = (int)(N * rand() / RAND_MAX);
	return r;
}

void coordenadas(int numeroCasa, int *X, int *Y){
	int N = sqrt(TABULEIRO);
	*X = (numeroCasa-1)/N+1;
	*Y = (numeroCasa-1)%N+1;
}

bool posicaoValida(int X, int Y){
	return (X >= 0 && X < TABULEIRO && Y >= 0 && Y < TABULEIRO);
}

bool sequenciaValida(POPULACAO copia){
	int vertices = 0, X = 0, Y = 0;
	bool visitadas[TABULEIRO+1] = {false}; //O vetor visitadas preecisa ir de 1 a N^2.

	for (int i = 0; i < TABULEIRO; i++){
		coordenadas(copia.tour[i], &X, &Y);
		if(!posicaoValida(X,Y))
			return false;	
		else{
			if(visitadas[copia.tour[i]])
				return false;
			else
				visitadas[copia.tour[i]] = true;
		}
	}

	return true;
}

bool vizinhoValido(int anterior, int atual){
	int X = 0, Y = 0, proximoX = 0, proximoY = 0, validoX = 0, validoY = 0;
	coordenadas(anterior, &X, &Y);
	coordenadas(atual, &proximoX, &proximoY);

	validoX = abs(proximoX - X);
	validoY = abs(proximoY - Y);

	return (validoX == 1 && validoY == 2) || (validoX == 2 && validoY == 1);
}

POPULACAO fitness(POPULACAO copia)
{	
	POPULACAO individuo = copia;
	if(!sequenciaValida(copia)){
		individuo.fitness = 0;
		return individuo;
	}

	int contador = 0, maiorSequencia = 0, indiceMaiorSeq = 0, indice = 0;
	for(int i = 0; i < TABULEIRO; i++){
		if(vizinhoValido(copia.tour[i], copia.tour[i+1])){
			contador++;
			if(contador > maiorSequencia){
				maiorSequencia = contador;
				indiceMaiorSeq = indice;
			}
		}else{
			contador = 0;
			indice = i+1; 
		}
	}
	individuo.fitness = maiorSequencia;
	individuo.indiceMaiorSeq = indiceMaiorSeq;

	return individuo;
}

void inicializa(POPULACAO populacao[INDIVIDUOS])
{
	for(int i = 0; i < INDIVIDUOS; i++){
		for(int j = 0; j < TABULEIRO; j++)
			populacao[i].tour[j] = j+1;
	}
	
	for(int i = 0; i < INDIVIDUOS; i++){
		for(int j = 0; j < TABULEIRO; j++){
			int pos = gerarNumAleatorio(TABULEIRO);
			int temp = populacao[i].tour[j];
			populacao[i].tour[j] = populacao[i].tour[pos];
			populacao[i].tour[pos] = temp;
		}
		populacao[i] = fitness(populacao[i]);	
	}
}

POPULACAO correcao(POPULACAO filho){
	POPULACAO corrigido = filho;
	int sequencia[TABULEIRO+1] = {0};

	// Removemos os número repetidos
	for(int i = 0; i < TABULEIRO; i++){
		int aux = corrigido.tour[i];
		if(sequencia[aux] > 1)
			corrigido.tour[i] = 0;
		else
			sequencia[aux] +=1;
	}

	for(int i = 0; i < TABULEIRO; i++){
		if(corrigido.tour[i] == 0){
			for(int j = 1; j <= TABULEIRO; j++){
				if(sequencia[j] == 0){
					corrigido.tour[i] = j;
					sequencia[j] +=1;
					break;
				}
			}
		}
	}

	return corrigido;
}

POPULACAO mutacao(POPULACAO filho){ 
	POPULACAO individuo = filho;

	int r = gerarNumAleatorio(100);
	int posicao = gerarNumAleatorio(TABULEIRO);
	
	// Este é um operador de mutação baseada no vizinho válido.
	// Em outras palavras, selecionamos uma posição aleatoriamente.
	// Encontramos um vizinho válido da sequência e realizamos a troca.
	if(r <= MUTACAO){
		int casa = individuo.tour[posicao-1];
		for(int i = 0; i < TABULEIRO; i++){
			if(vizinhoValido(casa, individuo.tour[i])){
				int temp = individuo.tour[posicao];
				individuo.tour[posicao] = individuo.tour[i];
				individuo.tour[i] = temp;
			}
		}
	}
	return individuo;
}

//Reprodução por ponto duplo heurístico
POPULACAO recombinacaoDoisPontos(POPULACAO pai, POPULACAO mae){
	POPULACAO filho, melhor, pior;

	if(pai.fitness > mae.fitness){ 
		melhor = pai;
		pior = mae;
	}
	else{ 
		melhor = mae;
		pior = pai;
	}

	int corteInicial = melhor.indiceMaiorSeq;
	int corteFinal = melhor.indiceMaiorSeq + melhor.fitness;

	for(int i = 0; i < TABULEIRO; i++){
		if(i >= corteInicial && i <= corteFinal)
			filho.tour[i] = melhor.tour[i];
		else
			filho.tour[i] = pior.tour[i];
	}
	return filho;
}

POPULACAO selecaoPorTorneio(POPULACAO populacao[INDIVIDUOS]){
	POPULACAO melhor;
	melhor.fitness = -1;

	for(int i = 1; i < TORNEIO; i++){
		POPULACAO aux = populacao[gerarNumAleatorio(INDIVIDUOS-1)];
		if(melhor.fitness == -1 || aux.fitness < melhor.fitness){
			melhor = aux;
		}
	}
	return melhor;
}

POPULACAO reproducao(POPULACAO populacao[INDIVIDUOS]){
	POPULACAO novaPopulacao[INDIVIDUOS], melhor, pai, mae, filho;
	melhor.fitness = -1;

	for(int i = 0; i < INDIVIDUOS; i++){
		pai = selecaoPorTorneio(populacao);
		mae = selecaoPorTorneio(populacao);

		filho = recombinacaoDoisPontos(pai, mae);
		filho = mutacao(filho);
		filho = correcao(filho);
		filho = fitness(filho);

		if(filho.fitness > melhor.fitness) 
			melhor = filho;
	
		novaPopulacao[i] = filho;
	}

	for(int i = 0; i < INDIVIDUOS; i++)
		populacao[i] = novaPopulacao[i];

	return melhor;
}

int main(void)
{
	POPULACAO populacao[INDIVIDUOS], melhor;
	int geracao = 0;

	srand(time(NULL));
	inicializa(populacao);
	
	do{
		melhor = reproducao(populacao);
		printf("\nIteracao %d, melhor fitness %d.\n", geracao, melhor.fitness);
		geracao++;
	}while(geracao <= GERACOES);
	
	return 0;
}