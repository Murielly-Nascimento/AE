#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#define TAMANHO	800
typedef struct{
	int fitness; // Número de caracteres distintos entre a frase alvo e a frase cópia (quanto menor melhor).
	char frase[TAMANHO]; // Frase cópia.
}INDIVIDUO;

#define LETRAS 100
typedef struct{
	char *fraseAlvo; // Frase alvo determinada pelo usuário
	char alfabeto[LETRAS]; // Caracteres presentes na frase alvo
	int qtdLetras; // Quantidade de caracteres distintos presentes na frase alvo
	int populacao; // Tamanho da população
	int geracoes; // Número de gerações
	int elitismo; // Taxa de elitismo (1 a 100)
	int mutacao; // Taxa de mutação (1 a 100)
	int torneio; // Número de participantes da seleção por torneio
}PARAMETROS;

/* Função: escreveArquivo

   Escreve no arquivo entrada.in a frase alvo desejada pelo usuário
   e os parâmetros especificados por ele.
*/
void escreveArquivo(){
	FILE *arq = fopen("entrada.in", "wb");
	
	if(arq == NULL){
		printf("Erro ao abrir o arquivo.\n");
		return;
	}
	
	int populacao = 900, geracoes = 1800, mutacao = 15, elitismo = 5, torneio = 3;
	char fraseAlvo[] = {"O Tejo e mais belo que o rio que corre pela minha aldeia,Mas o Tejo nao e mais belo que o rio que corre pela minha aldeia.Porque o Tejo nao e o rio que corre pela minha aldeia,O Tejo tem grandes navios,E nele navega ainda,Para aqueles que veem em tudo o que la nao esta,A memoria das naus.O Tejo desce de Espanha,E o Tejo entra no mar em Portugal.Toda a gente sabe isso.Mas poucos sabem qual e o rio da minha aldeia,E para onde ele vai,E donde ele vem.E por isso, porque pertence a menos gente,e mais livre e maior o rio da minha aldeia.Pelo Tejo vai se para o Mundo.Para alem do Tejo ha a America.E a fortuna daqueles que a encontram.Ninguem nunca pensou no que ha para alem.Do rio da minha aldeia.O rio da minha aldeia nao faz pensar em nada.Quem esta ao pe dele esta so ao pe dele."};
	//char fraseAlvo[] = {"o tejo guarda grandes navios"};
	//char fraseAlvo[] = {"O Tejo tem grandes navios,E nele navega ainda,Para aqueles que veem em tudo o que la nao esta,A memoria das naus."};
	//char fraseAlvo[] = {"O Tejo tem grandes navios,E nele navega ainda,Para aqueles que veem em tudo o que la nao esta,A memoria das naus.O Tejo desce de Espanha,E o Tejo entra no mar em Portugal.Toda a gente sabe isso.Mas poucos sabem qual e o rio da minha aldeia,E para onde ele vai,E donde ele vem.E por isso, porque pertence a menos gente,E mais livre e maior o rio da minha aldeia."};

	fwrite(&populacao, sizeof(int), 1, arq);
	fwrite(&geracoes, sizeof(int), 1, arq);
	fwrite(&mutacao, sizeof(int), 1, arq);
	fwrite(&elitismo, sizeof(int), 1, arq);
	fwrite(&torneio, sizeof(int), 1, arq);
	fwrite(fraseAlvo, sizeof(char), strlen(fraseAlvo), arq);
	
	fclose(arq);
	
	return;
}

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
	FILE *arq, *binario;
	arq = fopen("relatorio.txt", "a");
	binario = fopen("metricas.in", "ab");

	if(arq == NULL || binario == NULL){
		printf("Problemas na criação do arquivo\n");
		return;
	}
	
	//Gravando os dados no arquivo usando a função fwrite
  	fwrite (&tempo, sizeof(double), 1, binario);
  	fwrite (&fitness, sizeof(int), 1, binario); 

	int result = fprintf(arq, "%.3lf&%d\n",tempo,fitness);
	if(result == EOF)
		printf("Erro na gravação\n");
	
	fclose(arq);
	fclose(binario);
}

/* Função: leArquivo

   Lê no arquivo entrada.in com os parâmetros definidos pelo usuário
   para os testes.
	
	Retorno: 
		PARAMETROS - estrutura com as especificações (mutacao, frase alvo, elitismo) 
		para a execução do programa.
*/
PARAMETROS leArquivo(){
	FILE * arq = fopen("entrada.in", "rb");
	if(arq == NULL){
		printf("Erro ao abrir o arquivo.\n");
		return;
	}

	int populacao = 0, geracoes = 0, mutacao = 0, elitismo = 0, torneio = 0;
	char fraseAlvo[TAMANHO] = {'\0'};

	fread(&populacao, sizeof(int), 1, arq);
	fread(&geracoes, sizeof(int), 1, arq);
	fread(&mutacao, sizeof(int), 1, arq);
	fread(&elitismo, sizeof(int), 1, arq);
	fread(&torneio, sizeof(int), 1, arq);
	fread(fraseAlvo, sizeof(char), TAMANHO, arq);
	fclose(arq);
	
	PARAMETROS parametros;
	parametros.populacao = populacao;
	parametros.geracoes = geracoes;
	parametros.mutacao = mutacao;
	parametros.elitismo = elitismo;
	parametros.torneio = torneio;

	int contador = 0;
	for(int i = 0; i < TAMANHO; i++){
		if(fraseAlvo[i] == '\0'){
			contador++;
			break;
		}
		else contador++;
	}
	parametros.fraseAlvo = (char*)malloc(sizeof(char)*contador);
	strcpy(parametros.fraseAlvo, fraseAlvo);
	
	return parametros;
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
		parametros - PARAMETROS.
		
	Retorno: 
		Um número inteiro que representa o fitness do indivíduo.
*/
int fitness(INDIVIDUO copia, PARAMETROS parametros)
{
	int qtdDistintos = 0;

	for (int i = 0; i < strlen(parametros.fraseAlvo); i++){
		if(copia.frase[i] != parametros.fraseAlvo[i])
			qtdDistintos++;
	}
	return qtdDistintos;
}

/* Função: mutacao

    Altera o gene (caracter) de uma frase filho (cópia), sendo que
	os caracteres usados para a mutação são aqueles contidos na frase alvo.

	Parâmetros:
		filho - Um dos indivíduos da população.
		parametros - PARAMETROS.
		
	Retorno: 
		O indivíduo da população após ser mutado.
*/
INDIVIDUO mutacao(INDIVIDUO filho, PARAMETROS parametros){ 
	INDIVIDUO individuo = filho;

	int r = gerarNumAleatorio(100);
	int posicao = gerarNumAleatorio(strlen(parametros.fraseAlvo));
	int letras = parametros.qtdLetras;

	if(r <= parametros.mutacao)
		individuo.frase[posicao] = parametros.alfabeto[gerarNumAleatorio(letras)];
		
	return individuo;
}

/* Função: recombinacaoUniforme

    Combina aleatóriamente os genes (caracteres) da frase pai e mãe
	no filho.

	Parâmetros:
		pai - Um dos indivíduos da população.
		mae - Um dos indivíduos da população.
		N -  Tamanho da frase alvo.
		
	Retorno: 
		O indivíduo filho resultante da combinação do pai e mãe.
*/
INDIVIDUO recombinacaoUniforme(INDIVIDUO pai, INDIVIDUO mae, int N){
	INDIVIDUO filho;

	for(int i = 0; i < N; i++){
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
		parametros - PARAMETROS.
		
	Retorno: 
		O indivíduo com menor fitness ontido pelo torneio.
*/
INDIVIDUO selecaoPorTorneio(INDIVIDUO *populacao, PARAMETROS parametros){
	INDIVIDUO melhor;
	melhor.fitness = -1;

	for(int i = 1; i < parametros.torneio; i++){
		INDIVIDUO aux = populacao[gerarNumAleatorio(parametros.populacao-1)];
		if(melhor.fitness == -1 || aux.fitness < melhor.fitness){
			melhor = aux;
		}
	}
	return melhor;
}

/* Função: comparacaoIndividuos

    Usada para execução do qsort, esta função recebe dois INDIVIDUO
	e os compara determinando qual deles possui menor fitness.

	Parâmetros:
		A - indivíduo.
		B - indivíduo.
		
	Retorno: 
		Se A é maior que B, ou o contrário.
*/
int comparacaoIndividuos(const void* A, const void* B){
	INDIVIDUO C = *(INDIVIDUO*)A;
	INDIVIDUO D = *(INDIVIDUO*)B;
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
		parametros - PARAMETROS.
		
	Retorno: 
		Número de indivíduos selecionados.
*/
int elitismo(INDIVIDUO *populacao, PARAMETROS parametros){
	int selecionados = parametros.populacao*parametros.elitismo/100;
	qsort(populacao, parametros.populacao, sizeof(populacao[0]), comparacaoIndividuos);
	return selecionados;
}

/* Função: reproducao

	Aplica as funções de elitismo, torneio, recombinação, mutação,
	retornando uma nova população.

	Parâmetros:
		populacao - População de indivíduos (frases cópias).
		parametros - PARAMETROS.
		geracoes - Número de gerações executadas.

	Retorno: 
		Melhor indivíduo da população.
*/
INDIVIDUO reproducao(INDIVIDUO *populacao, PARAMETROS parametros, int geracoes)
{
	INDIVIDUO melhor, pai, mae, filho, *novaPopulacao;
	novaPopulacao = (INDIVIDUO *)malloc(parametros.populacao * sizeof(INDIVIDUO));

	int taxaDeElitismo = elitismo(populacao, parametros);
	melhor = populacao[0];

	for(int i = taxaDeElitismo; i < parametros.populacao; i++){
		pai = selecaoPorTorneio(populacao, parametros);
		mae = selecaoPorTorneio(populacao, parametros);

		filho = recombinacaoUniforme(pai, mae, strlen(parametros.fraseAlvo));
		filho = mutacao(filho, parametros);
		filho.fitness = fitness(filho,parametros);

		if(filho.fitness < melhor.fitness) 
			melhor = filho;
	
		novaPopulacao[i] = filho;
	}

	for(int i = taxaDeElitismo; i < parametros.populacao; i++){
		populacao[i] = novaPopulacao[i];
	}

	free(novaPopulacao);

	return melhor;
}

/* Função: inicializa

    Inicializa as frases da sequência população e calcula o fitness
	de cada uma delas. Sendo que os caracteres usados para a inicialização 
	são aqueles contidos na frase alvo.

	Parâmetros:
		população - sequência do tipo INDIVIDUO de tamanho INDIVIDUO.
		parametros - PARAMETROS.
		
	Retorno: 
		Nulo.
*/
void inicializa(INDIVIDUO *populacao, PARAMETROS parametros)
{
	int i = 0, j = 0, letras = parametros.qtdLetras;
	for (i = 0; i < parametros.populacao; i++){
		for(j = 0; j < strlen(parametros.fraseAlvo); j++)
			populacao[i].frase[j] = parametros.alfabeto[gerarNumAleatorio(letras)];
		populacao[i].frase[j] = '\0';
		populacao[i].fitness = fitness(populacao[i], parametros);
	}
}

/* Função: comparacaoCaracteres

    Usada para execução do alfabeto, esta função recebe dois caracteres
	e os compara determinando qual deles é menor.

	Parâmetros:
		A - indivíduo.
		B - indivíduo.
		
	Retorno: 
		Se A é maior que B, ou o contrário.
*/
int comparacaoCaracteres(const void* A, const void* B){
	if(*(char*)A > *(char*)B) return 1;
	else return -1;
}

/* Função: alfabeto

    Determina quais caracteres compõem a frase alvo.

	Parâmetros:
		parametros - PARAMETROS.
		
	Retorno: 
		Nulo.
*/
PARAMETROS alfabeto(PARAMETROS parametros)
{
	char frase[TAMANHO] = {'\0'}, alfabeto[LETRAS] = {'\0'};
	int j = 0;
	strcpy(frase, parametros.fraseAlvo);
	qsort(frase, strlen(frase), sizeof(char), comparacaoCaracteres);
	
	// Eliminamos os caracteres duplicados
	for(int i = 0; i < strlen(frase); i++){
		if(frase[i] == '\0') 
			break;
		else if(frase[i] != frase[i+1] && frase[i] > 0){
			alfabeto[j] = frase[i];
			j++;
		}
	}
	
	// Atríbuimos o vetor alfabeto (sem caracteres repetidos) ao parâmetro alfabeto.
	for(int i = 0; i <= j; i++){
		parametros.alfabeto[i] = alfabeto[i];
	}
	parametros.qtdLetras = j;

	return parametros;
}

/* Função: liberaMemoria

    Libera a memória dos arrays e estruturas alocados dinâmicamente.

	Parâmetros:
		populacao - População de INDIVÍDUOS.
		parametros - PARAMETROS.
		
	Retorno: 
		Nulo.
*/
void liberaMemoria(INDIVIDUO* populacao, PARAMETROS parametros){
	free(populacao);
	free(parametros.fraseAlvo);
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

	escreveArquivo();
	PARAMETROS parametros = leArquivo();
	parametros = alfabeto(parametros);

	INDIVIDUO melhor, *populacao;
	int geracao = 0;
	populacao = (INDIVIDUO *)malloc(parametros.populacao * sizeof(INDIVIDUO));
	
	srand(time(NULL));
	inicializa(populacao, parametros);

	do{
		melhor = reproducao(populacao, parametros, geracao);
			
		printf("\nIteracao %d, melhor fitness %d.\n", geracao, melhor.fitness);
		printf("%s\n", melhor.frase);
		geracao++;

	}while (geracao <= parametros.geracoes);

	fim = clock();
	total = (double)(fim-inicio)/CLOCKS_PER_SEC;
	printf("Tempo total gasto pela CPU: %lf\n", total);
	escreveRelatorio(total,melhor.fitness);

	liberaMemoria(populacao, parametros);
	
	return 0;
}