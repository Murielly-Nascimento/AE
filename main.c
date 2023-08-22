#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#define LETRAS 28
#define TAMANHO	117
typedef struct{
	int fitness;
	char frase[TAMANHO];
}POPULACAO;

#define TORNEIO 3
#define MUTACAO	15
#define ELITISMO 10
#define GERACOES 280
#define INDIVIDUOS 300

/* Função: escreveRelatorio

   Escreve no arquivo relatorio.txt o tempo para execução
   do programa e o fitness do melhor indivíduo obtido.

	Parâmetros:
		tempo - Tempo gasto para execução do programa.
		fitness - Número de caracteres distintos da frase alvo 
				  do indivíduo mais adaptado da População.
	
	Retorno: 
		Nulo.
*/
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

/* Função: gerarNumAleatorio

   Gera um número aleatório no intervalo de 0 a N-1.

	Parâmetros:
		n - Intervalo.
		
	Retorno: 
		Um número inteiro aleatório.
	
	Confira o seguinte link para mais informações sobre a função rand()
		https://c-faq.com/lib/randrange.html
*/
int gerarNumAleatorio(int n)
{
	int r = (int)(n * rand() / RAND_MAX);
	return r;
}

/* Função: fitness

    Calcula o fitness de um indivíduo baseado no número de caracteres
	distintos entre a frase alvo e a cópia. Quanto menor o fitness
	mais adaptado o indivíduo é.

	Parâmetros:
		copia - Indivíduo.
		alvo - Sequência de caracteres da frase alvo
		
	Retorno: 
		Um número inteiro que representa o fitness do indivíduo.
*/
int fitness(POPULACAO copia, const char *alvo)
{
	int qtdDistintos = 0;

	for (int i = 0; i < TAMANHO; i++){
		if(copia.frase[i] != alvo[i])
			qtdDistintos++;
	}
	return qtdDistintos;
}

/* Função: inicializa

    Inicializa as frases da sequência população e calcula o fitness
	de cada uma delas. Sendo que os caracteres usados para a inicialização 
	são aqueles contidos na frase alvo.

	Parâmetros:
		população - sequência do tipo POPULACAO de tamanho INDIVIDUOS.
		alvo - Sequência de caracteres da frase alvo.
		
	Retorno: 
		Nulo.
*/
void inicializa(POPULACAO populacao[INDIVIDUOS], const char *alvo)
{
	const char alfabeto[] = "OTjEgPvtqltAemoridnaus,\n.  ";

	int i = 0, j = 0;
	for (i = 0; i < INDIVIDUOS; i++){
		for(j = 0; j < TAMANHO-1; j++)
			populacao[i].frase[j] = alfabeto[gerarNumAleatorio(LETRAS)];
		populacao[i].frase[j] = '\0';
		populacao[i].fitness = fitness(populacao[i], alvo);
	}
}

/* Função: mutacao

    Altera o gene (caracter) de uma frase filho (cópia), sendo que
	os caracteres usados para a mutação são aqueles contidos na frase alvo.

	Parâmetros:
		filho - Um dos indivíduos da população.
		
	Retorno: 
		O indivíduo da população após ser mutado.
*/
POPULACAO mutacao(POPULACAO filho){ 
	POPULACAO individuo = filho;
	const char alfabeto[] = "OTjEgPvtqltAemoridnaus,\n.  ";

	int r = gerarNumAleatorio(100);
	int posicao = gerarNumAleatorio(TAMANHO-2);

	if(r <= MUTACAO)
		individuo.frase[posicao] = alfabeto[gerarNumAleatorio(LETRAS)];
		//individuo.frase[posicao] = alvo[posicao];

	return individuo;
}

/* Função: recombinacaoUniforme

    Combina aleatóriamente os genes (caracteres) da frase pai e mãe
	no filho.

	Parâmetros:
		pai - Um dos indivíduos da população.
		mae - Um dos indivíduos da população.
		
	Retorno: 
		O indivíduo filho resultante da combinação do pai e mãe.
*/
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

/* Função: selecaoPorTorneio

    Enquanto o torneio entre N indivíduos não terminar
	seleciona dois a dois e determina qual deles
	possui o menor fitness.

	Parâmetros:
		populacao - população de indivíduos (frases cópias).
		
	Retorno: 
		O indivíduo com menor fitness ontido pelo torneio.
*/
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

/* Função: comparação

    Usada para execução do qsort, esta função recebe dois INDIVIDUOS
	e os compara determinando qual deles possui menor fitness.

	Parâmetros:
		A - indivíduo.
		B - indivíduo.
		
	Retorno: 
		Se A é maior que B, ou o contrário.
*/
int comparacao(const void* A, const void* B){
	POPULACAO C = *(POPULACAO*)A;
	POPULACAO D = *(POPULACAO*)B;
	if(C.fitness > D.fitness) return 1;
	else return -1;
}

/* Função: elitismo

    Calcula o número de indivíduos da população que
	não sofreram a ação dos operadores recombinação e mutação
	na função reproducao. Em seguida chama a função de ordenação
	qsort, os indivíduos mais adaptados ocupam as posições iniciais.

	Parâmetros:
		populacao - População de indivíduos (frases cópias).
		
	Retorno: 
		Número de indivíduos selecionados.
*/
int elitismo(POPULACAO populacao[INDIVIDUOS]){
	int selecionados = INDIVIDUOS*ELITISMO/100;
	qsort(populacao, INDIVIDUOS, sizeof(populacao[0]), comparacao);
	return selecionados;
}

/* Função: reproducao

	Aplica as funções de elitismo, torneio, recombinação, mutação,
	retornando uma nova população.

	Parâmetros:
		populacao - População de indivíduos (frases cópias).
		alvo - Sequência de caracteres da frase alvo.
		geracoes - Número de gerações executadas.

	Retorno: 
		Melhor indivíduo da população.
*/
POPULACAO reproducao(POPULACAO populacao[INDIVIDUOS],const char* alvo, int geracoes)
{
	POPULACAO novaPopulacao[INDIVIDUOS], melhor, pai, mae, filho;
	melhor.fitness=RAND_MAX;
	int taxaDeElitismo = 0;

	if(geracoes >= GERACOES/2){
		taxaDeElitismo = elitismo(populacao);
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

/* Função: main

	Implementa o Algoritmo Evolutivo para o problema: dado uma frase alvo 
	reproduza-a a partir de uma população de frases cópias geradas aleatóriamente.
*/
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