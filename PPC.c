#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <stdbool.h>
#include <math.h>

#define TABULEIRO 16
typedef struct{
	int fitness;
	int indiceMaiorSeq;
	int tour[TABULEIRO];
}INDIVIDUO;

#define TORNEIO 3
#define MUTACAO 15
#define ELITISMO 10
#define GERACOES 50
#define POPULACAO 100

/* Função: escreveRelatorio

   Escreve no arquivo resultados.txt o tempo para execução
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
	arq = fopen("resultados.txt", "a");

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
int gerarNumAleatorio(int N)
{
	int r = (int)(N * rand() / RAND_MAX);
	return r;
}

/* Função: coordenadas

    Dado um número de uma casa do tabuleiro de xadrez
	calcula as suas respectivas coordenadas X e Y.

	Parâmetros:
		numeroCasa - número da casa no tabuleiro de xadrez.
		X - coordenada no eixo das abscissas.
		Y - coordenada no eixo das ordenadas.

	Retorno: 
		Nulo.
*/
void coordenadas(int numeroCasa, int *X, int *Y){
	int N = sqrt(TABULEIRO);
	*X = (numeroCasa-1)/N+1;
	*Y = (numeroCasa-1)%N+1;
}

/* Função: posicaoValida

    Dado as coordenadas X e Y verifica se o cavalo
	está numa posição válida.

	Parâmetros:
		X - coordenada no eixo das abscissas.
		Y - coordenada no eixo das ordenadas.

	Retorno: 
		True se a posição é válida e False caso não.
*/
bool posicaoValida(int X, int Y){
	return (X >= 0 && X < TABULEIRO && Y >= 0 && Y < TABULEIRO);
}

/* Função: sequenciaValida

    Verifica se o percurso do cavalo é válido e
	se não vistou a mesma casa mais de uma vez.

	Parâmetros:
		copia - Indivíduo pertecente a população.

	Retorno: 
		True se a sequência é válida e False caso não.
*/
bool sequenciaValida(INDIVIDUO copia){
	int vertices = 0, X = 0, Y = 0;
	bool visitadas[TABULEIRO+1] = {false};

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

/* Função: vizinhoValido

    Verifica se a casa N+1 a ser visitada pelo cavalo é válida.

	Parâmetros:
		atual - Número da casa N.
		proximo - Número da casa N+1.

	Retorno: 
		True se o vizinho é válido e False caso não.
*/
bool vizinhoValido(int atual, int proximo){
	int X = 0, Y = 0, proximoX = 0, proximoY = 0, validoX = 0, validoY = 0;
	coordenadas(atual, &X, &Y);
	coordenadas(proximo, &proximoX, &proximoY);

	validoX = abs(proximoX - X);
	validoY = abs(proximoY - Y);

	return (validoX == 1 && validoY == 2) || (validoX == 2 && validoY == 1);
}

/* Função: fitness

    O fitness é calculado como sendo a maior sequência de
	movimentos válidos feitos pelo cavalo. A posição i
	em que a maior sequência se inicia é guardada.

	Parâmetros:
		copia - Indivíduo da população.

	Retorno: 
		Indivíduo com valor de fitness correspondente.
*/
INDIVIDUO fitness(INDIVIDUO copia)
{	
	INDIVIDUO individuo = copia;
	if(!sequenciaValida(copia)){
		individuo.fitness = 0;
		return individuo;
	}

	int contador = 0, maiorSequencia = 0, indiceMaiorSeq = 0, indice = 0;
	for(int i = 0; i < TABULEIRO; i++){
		if(vizinhoValido(copia.tour[i], copia.tour[i+1])){
			contador++;
			if(contador > maiorSequencia){
				if(maiorSequencia == 0){
					indiceMaiorSeq = i; 
				}
				else 
					indiceMaiorSeq = indice;
				maiorSequencia = contador;
			}
		}else{
			contador = 0;
			indice = i; 
		}
	}
	individuo.fitness = maiorSequencia;
	individuo.indiceMaiorSeq = indiceMaiorSeq;

	return individuo;
}

/* Função: inicializa

    Inicializa a população de indivíduos com número aleatórios de 1 a N
	e calcula o seu fitness.

	Parâmetros:
		populacao - População de indivíduos.

	Retorno: 
		Nulo.
*/
void inicializa(INDIVIDUO populacao[POPULACAO])
{
	for(int i = 0; i < POPULACAO; i++){
		for(int j = 0; j < TABULEIRO; j++)
			populacao[i].tour[j] = j+1;
	}
	
	for(int i = 0; i < POPULACAO; i++){
		for(int j = 0; j < TABULEIRO; j++){
			int pos = gerarNumAleatorio(TABULEIRO);
			int temp = populacao[i].tour[j];
			populacao[i].tour[j] = populacao[i].tour[pos];
			populacao[i].tour[pos] = temp;
		}
		populacao[i] = fitness(populacao[i]);	
	}
}

/* Função: correcao

    Remove os números repetidos do percurso do cavalo e
	os substitui pelas casas não visitadas.

	Parâmetros:
		filho - Indivíduo da população.

	Retorno: 
		Indivíduo com o tour corrigido.
*/
INDIVIDUO correcao(INDIVIDUO filho){
	INDIVIDUO corrigido = filho;
	int sequencia[TABULEIRO+1] = {0};

	// Marcamos no vetor sequencia os números repetidos.
	for(int i = 0; i < TABULEIRO; i++){
		int aux = corrigido.tour[i];
		if(sequencia[aux] >= 1)
			corrigido.tour[i] = 0;
		else
			sequencia[aux] +=1;
	}

	// Removemos os números repetidos e os substituímos por
	// aqueles que não apareceram no percurso do cavalo.
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

/* Função: mutacao

    Altera o gene - número de casa do percurso do cavalo - de filho (cópia), 
	sendo que a preferência é para vizinhos válidos.

	Parâmetros:
		filho - Indivíduo da população.

	Retorno: 
		Indivíduo com o percurso alterado.
*/
/*INDIVIDUO mutacao(INDIVIDUO filho){ 
	INDIVIDUO individuo = filho;
	
	int r = gerarNumAleatorio(100);
	int posicao = gerarNumAleatorio(TABULEIRO);
	int casa = individuo.tour[posicao-1];
		
	if(r <= MUTACAO){
		// Este é um operador de mutação baseada no vizinho válido.
		// Em outras palavras, selecionamos uma posição aleatoriamente.
		// Encontramos um vizinho válido da sequência e realizamos a troca.
		for(int i = 0; i < TABULEIRO; i++){
			if(vizinhoValido(casa, individuo.tour[i])){
				int temp = individuo.tour[posicao];
				individuo.tour[posicao] = individuo.tour[i];
				individuo.tour[i] = temp;
				break;
			}
		}
	}
	return individuo;
}*/

INDIVIDUO mutacao(INDIVIDUO filho){ 
	INDIVIDUO individuo = filho;
	
	int r = gerarNumAleatorio(100);
	int posicao = gerarNumAleatorio(TABULEIRO);
	int casa = individuo.tour[posicao-1];
		
	if(r <= MUTACAO){
		int aux = individuo.indiceMaiorSeq + individuo.fitness;
		if (aux < TABULEIRO-1){
			for(int i = 0; i < TABULEIRO; i++){
				if(vizinhoValido(individuo.tour[aux], individuo.tour[i])){
					int temp = individuo.tour[aux+1];
					individuo.tour[aux+1] = individuo.tour[i];
					individuo.tour[i] = temp;
					break;
				}
			}
		}else{
			aux = individuo.indiceMaiorSeq;
			for(int i = 0; i < TABULEIRO; i++){
				if(vizinhoValido(individuo.tour[aux], individuo.tour[i])){
					int temp = individuo.tour[aux-1];
					individuo.tour[aux-1] = individuo.tour[i];
					individuo.tour[i] = temp;
					break;
				}
			}
		}
	}
	return individuo;
}

/* Função: recombinacaoDoisPontos

    Dado os genes do pai e da mãe realiza a recombinação a
	partir de dois pontos de corte favoráveis. Em outras palavras,
	a maior sequência válida do percurso é mantida.

	Parâmetros:
		pai - Indivíduo da população.
		mae - Indivíduo da população.

	Retorno: 
		Filho resultante da recombinação.
*/
INDIVIDUO recombinacaoDoisPontos(INDIVIDUO pai, INDIVIDUO mae){
	INDIVIDUO filho, melhor, pior;

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
/*INDIVIDUO recombinacaoDoisPontos(INDIVIDUO pai, INDIVIDUO mae){
	INDIVIDUO filho;

	for(int i = 0; i < TABULEIRO; i++){
		if(gerarNumAleatorio(2) == 1)
			filho.tour[i] = pai.tour[i];
		else
			filho.tour[i] = mae.tour[i];
	}

	return filho;
}*/

/* Função: comparação

    Usada para execução do qsort, esta função recebe dois POPULACAO
	e os compara determinando qual deles possui maior fitness.

	Parâmetros:
		A - indivíduo.
		B - indivíduo.
		
	Retorno: 
		Se A é menor que B, ou o contrário.
*/
int comparacao(const void* A, const void* B){
	INDIVIDUO C = *(INDIVIDUO*)A;
	INDIVIDUO D = *(INDIVIDUO*)B;
	if(C.fitness < D.fitness) return 1;
	else return -1;
}

/* Função: elitismo

    Calcula o número de indivíduos da população que
	não sofreram a ação dos operadores recombinação e mutação
	na função reproducao. Em seguida chama a função de ordenação
	qsort, os indivíduos mais adaptados ocupam as posições iniciais.

	Parâmetros:
		populacao - População de indivíduos.
		
	Retorno: 
		Número de indivíduos selecionados.
*/
int elitismo(INDIVIDUO populacao[POPULACAO]){
	int selecionados = POPULACAO*ELITISMO/100;
	qsort(populacao, POPULACAO, sizeof(populacao[0]), comparacao);
	return selecionados;
}

/* Função: selecaoPorTorneio

    Enquanto o torneio entre N indivíduos não terminar
	seleciona dois a dois e determina qual deles
	possui o maior fitness.

	Parâmetros:
		populacao - população de indivíduos (frases cópias).
		
	Retorno: 
		O indivíduo com maior fitness obtido pelo torneio.
*/
INDIVIDUO selecaoPorTorneio(INDIVIDUO populacao[POPULACAO]){
	INDIVIDUO melhor;
	melhor.fitness = -1;

	for(int i = 1; i < TORNEIO; i++){
		INDIVIDUO aux = populacao[gerarNumAleatorio(POPULACAO-1)];
		if(melhor.fitness == -1 || aux.fitness > melhor.fitness){
			melhor = aux;
		}
	}
	return melhor;
}

/* Função: reproducao

	Aplica as funções de elitismo, torneio, recombinação, mutação,
	retornando uma nova população.

	Parâmetros:
		populacao - População de indivíduos.
		geracoes - Número de gerações executadas.

	Retorno: 
		Melhor indivíduo da população.
*/
INDIVIDUO reproducao(INDIVIDUO populacao[POPULACAO], int geracoes){
	INDIVIDUO novaPopulacao[POPULACAO], melhor, pai, mae, filho;
	melhor.fitness = -1;
	int taxaDeElitismo = 0;

	if(geracoes >= GERACOES/2){
		taxaDeElitismo = elitismo(populacao);
		melhor = populacao[0];
	}

	for(int i = taxaDeElitismo; i < POPULACAO; i++){
		pai = selecaoPorTorneio(populacao);
		mae = selecaoPorTorneio(populacao);

		filho = recombinacaoDoisPontos(pai, mae);
		filho = correcao(filho);
		filho = fitness(filho);
		filho = mutacao(filho);
		filho = fitness(filho);

		if(filho.fitness > melhor.fitness) 
			melhor = filho;
	
		novaPopulacao[i] = filho;
	}

	for(int i = taxaDeElitismo; i < POPULACAO; i++)
		populacao[i] = novaPopulacao[i];

	return melhor;
}

/* Função: main

	Implementa o Algoritmo Evolutivo para o Problema do Percurso do Cavalo.
*/
int main(void)
{
	clock_t inicio, fim;
	double total = 0;
	inicio = clock();

	INDIVIDUO populacao[POPULACAO], melhor;
	int geracao = 0;

	srand(time(NULL));
	inicializa(populacao);
	
	do{
		melhor = reproducao(populacao, geracao);
		printf("\nIteracao %d, melhor fitness %d.\n", geracao, melhor.fitness);
		geracao++;
	}while(geracao <= GERACOES);

	fim = clock();
	total = (double)(fim-inicio)/CLOCKS_PER_SEC;
	printf("Tempo total gasto pela CPU: %lf\n", total);
	escreveRelatorio(total,melhor.fitness);
	
	return 0;
}