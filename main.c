#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <sys/time.h>

#define LETRAS 55
#define TAMANHO	117

typedef struct{
	int fitness;
	char frase[TAMANHO];
}POPULACAO;

#define MUTACAO	15
#define GERACOES 280
#define INDIVIDUOS 400
#define TORNEIO 3
#define ELITISMO 10

void escreveRelatorio(double tempo, int fitness){
	FILE *arq;
	arq = fopen("relatorio.txt", "a");

	if(arq == NULL){
		printf("Problemas na criação do arquivo\n");
		return;
	}
	int result = fprintf(arq, "%.3lf&%d\n",tempo,fitness);
	if(result == EOF)
		printf("Erro na gravação\n");
	fclose(arq);
}

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

void inicializa(POPULACAO populacao[INDIVIDUOS], const char *alvo)
{
	const char alfabeto[] = "ABCDEFGHIJKLMNOPQRSTUVXWZabcdefghijklmnopqrstuvxwz,\n.  ";

	int i = 0, j = 0;
	for (i = 0; i < INDIVIDUOS; i++){
		for(j = 0; j < TAMANHO-1; j++)
			populacao[i].frase[j] = alfabeto[gerarNumAleatorio(LETRAS)];
		populacao[i].frase[j] = '\0';
		populacao[i].fitness = fitness(populacao[i], alvo);
	}
}

POPULACAO mutacao(POPULACAO filho){ 
	POPULACAO individuo = filho;
	const char alfabeto[] = "ABCDEFGHIJKLMNOPQRSTUVXWZabcdefghijklmnopqrstuvxwz,\n.  ";

	int r = gerarNumAleatorio(100);
	int posicao = gerarNumAleatorio(TAMANHO-2);

	if(r <= MUTACAO)
		individuo.frase[posicao] = alfabeto[gerarNumAleatorio(LETRAS)];

	return individuo;
}

POPULACAO recombinacaoUniforme(POPULACAO pai, POPULACAO mae){
	POPULACAO filho;

	for(int i = 0; i < TAMANHO; i++){
		if(gerarNumAleatorio(2) == 1)
			filho.frase[i] = pai.frase[i];
		else
			filho.frase[i] = mae.frase[i];
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

void shellSort(POPULACAO populacao[INDIVIDUOS], int n) {
	for (int intervalo = n/2; intervalo > 0; intervalo/= 2) {
		for (int i = intervalo; i < n; i+= 1) {
			POPULACAO aux = populacao[i];
			int j = 0;
			for (j = i; j >= intervalo && populacao[j - intervalo].fitness > aux.fitness; j -= intervalo) {
				populacao[j] = populacao[j - intervalo];
			}
			populacao[j] = aux;
		}
	}
}

int elitismo(POPULACAO populacao[INDIVIDUOS],const char* alvo){
	int selecionados = INDIVIDUOS*ELITISMO/100;
	
	int size = sizeof(populacao)/sizeof(populacao[0]);
	shellSort(populacao, size);

	return selecionados;
}

POPULACAO reproducao(POPULACAO populacao[INDIVIDUOS],const char* alvo, int geracoes)
{
	POPULACAO novaPopulacao[INDIVIDUOS], melhor, pai, mae, filho;
	melhor.fitness=RAND_MAX;

	int taxaDeElitismo = 0;
	if(geracoes >= GERACOES/2){
		taxaDeElitismo = elitismo(populacao, alvo);
		melhor = populacao[0];
	}

	for(int i = taxaDeElitismo; i < INDIVIDUOS; i++){
		pai = selecaoPorTorneio(populacao);
		mae = selecaoPorTorneio(populacao);

		filho = recombinacaoUniforme(pai, mae);
		filho = mutacao(filho);
		filho.fitness = fitness(filho,alvo);

		if(filho.fitness < melhor.fitness) 
			melhor = filho;
	
		novaPopulacao[i] = filho;
	}

	for(int i = taxaDeElitismo; i < INDIVIDUOS; i++)
		populacao[i] = novaPopulacao[i];

	return melhor;
}

int main(void)
{
	clock_t inicio, fim;
	double total = 0;
	inicio = clock();

	const char alvo[] = "O Tejo tem grandes navios.\nE navega nele ainda,\nPara aqueles que veem em tudo o que la nao esta,\nA memoria das naus.";
	
	int geracao = 0;
	POPULACAO populacao[INDIVIDUOS], melhor;

	srand(time(NULL));
	inicializa(populacao, alvo);

	do{
		melhor = reproducao(populacao, alvo, geracao);
			
		printf("\nIteracao %d, melhor fitness %d.\n", geracao, melhor.fitness);
		printf("%s\n", melhor.frase);
		geracao++;

	}while (geracao <= GERACOES);

	fim = clock();
	total = (double)(fim-inicio)/CLOCKS_PER_SEC;
	printf("Tempo total gasto pela CPU: %lf\n", total);
	escreveRelatorio(total,melhor.fitness);

	return 0;
}