#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define TABULEIRO 400 // Número de casas do tabuleiro (NxN)
typedef struct{
	int fitness; // Tamanho do maior percurso válido
	int tour[TABULEIRO]; // Percurso do cavalo
}INDIVIDUO;

#define MUTACAO 15
#define ELITISMO 10  
#define GERACOES 10000
#define POPULACAO 1000
#define TORNEIO 3

/* Função: escreveRelatorio

   Escreve no arquivo resultados.txt o tempo para execução
   do programa e o fitness do melhor indivíduo obtido.

	Parâmetros:
		tempo - Tempo gasto para execução do programa.
		fitness - Tamanho do maior percurso válido.
	
	Retorno: 
		Nulo.
*/
void escreveRelatorio(double tempo, int fitness, int geracao){
	FILE *arq, *binario;
	arq = fopen("resultados.txt", "a");
	binario = fopen("metricasPPC.in", "ab");

	if(arq == NULL || binario == NULL){
		printf("Problemas na criação do arquivo\n");
		return;
	}

	//Gravando os dados no arquivo usando a função fwrite
	fwrite (&geracao, sizeof(int), 1, binario);
  	fwrite (&tempo, sizeof(double), 1, binario);
  	fwrite (&fitness, sizeof(int), 1, binario);
	
	int result = fprintf(arq, "%d&%.3lf&%d\n",geracao, tempo, fitness);
	if(result == EOF)
		printf("Erro na gravação\n");
	fclose(arq);
	fclose(binario);
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

   	Dado um número de casa, calcula as coordenadas X e Y correspondentes.

	Parâmetros:
		numeroCasa - número de uma casa do tabuleiro.
		*X - ponteiro para coordenada X.
		*Y - ponteiro para coordenada Y.
		
	Retorno: 
		Nulo.
*/
void coordenadas(int numeroCasa, int *X, int *Y){
	int N = sqrt(TABULEIRO);
	*X = (numeroCasa-1)/N+1;
	*Y = (numeroCasa-1)%N+1;
}

/* Função: posicaoValida

   	Verifica se uma posição é valida, ou seja, se as coordenadas estão
	delimitadas pelas dimensões do tabuleiro.

	Parâmetros:
		X - coordenada X.
		Y - coordenada Y.
		
	Retorno: 
		True se for uma posição válida e false caso não.
*/
bool posicaoValida(int X, int Y){
	int N = sqrt(TABULEIRO);
	return (X >= 1 && X <= N && Y >= 1 && Y <= N);
}

/* Função: vizinhoValido

   	Verifica se uma casa (i+1) é um vizinho válido da anterior (i).

	Parâmetros:
		atual - número de casa na posição i.
		proximo - número de casa na posição i+1.
		
	Retorno: 
		True se for um vizinho válido e false caso não.
*/
bool vizinhoValido(int atual, int proximo){
	int X = 0, Y = 0, proximoX = 0, proximoY = 0, validoX = 0, validoY = 0;
	coordenadas(atual, &X, &Y);
	coordenadas(proximo, &proximoX, &proximoY);

	validoX = abs(proximoX - X);
	validoY = abs(proximoY - Y);

	return (validoX == 1 && validoY == 2) || (validoX == 2 && validoY == 1);
}

/* Função: numeroCasa

   	Dado as coordenadas X e Y retorna o número de casa correspondente.

	Parâmetros:
		X - coordenada X.
		Y - coordenada Y.
		
	Retorno: 
		Número de casa.
*/
int numeroCasa(int X, int Y){
	int N = sqrt(TABULEIRO);
	int numeroCasa = (Y - 1) * N + X;
	return numeroCasa; 
}

/* Função: movimentosPossiveis

   	Calcula o número de movimentos possíveis a partir de uma casa.

	Parâmetros:
		X - coordenada X.
		Y - coordenada Y.
		
	Retorno: 
		Número máximo de movimentos possíveis.
*/
int movimentosPossiveis(int X, int Y) {
	// Define os movimentos possíveis do cavalo
	int eixoX[] = {2, 1, -1, -2, -2, -1, 1, 2};
	int eixoY[] = {1, 2, 2, 1, -1, -2, -2, -1};
	int contador = 0;

	for (int i = 0; i < 8; i++) {
		int proximoX = X + eixoX[i];
		int proximoY = Y + eixoY[i];
		if (posicaoValida(proximoX, proximoY)) 
			contador++;
	}

	return contador;
}

/* Função: regraWarnsdorff

   	Dado um número de casa o seu vizinho válido - com MENOR 
	número de movimentos possíveis - é calculado.

	Parâmetros:
		casa - número de casa.
		visitadas - vetor booleano com número das casas visitadas.
		
	Retorno: 
		Vizinho válido com menor número de movimentos possíveis.
*/
int regraWarnsdorff(int casa, bool visitadas[TABULEIRO+1]){
	int X = 0, Y = 0;
	coordenadas(casa, &X, &Y);
	// Define os movimentos possíveis do cavalo
	int eixoX[] = {2, 1, -1, -2, -2, -1, 1, 2};
	int eixoY[] = {1, 2, 2, 1, -1, -2, -2, -1};
	// Inicializado com um valor acima do possível (8).
	int minMovimentos = 9, valor = 0, valorF = 0; 

	for(int i = 0; i < 8; i++) {
		int auxX = X + eixoX[i];
		int auxY = Y + eixoY[i];
		int aux = (auxX - 1) * sqrt(TABULEIRO) + auxY;
		if (posicaoValida(auxX, auxY) && !visitadas[aux] && vizinhoValido(casa,aux)){
			valor = aux;
			int movimentos = movimentosPossiveis(auxX, auxY);
			if(movimentos < minMovimentos){
				minMovimentos = movimentos;
				valorF = valor;
			}
		}
	}

	if(valorF == 0) valorF = valor;

	return valorF;
}

/* Função: proximoMovimento

   	Dado um número de casa o seu vizinho válido é calculado.

	Parâmetros:
		casa - número de casa.
		visitadas - vetor booleano com número das casas visitadas.
		
	Retorno: 
		Vizinho válido.
*/
int proximoMovimento(int casa, bool visitadas[TABULEIRO+1]) {
	int X = 0, Y = 0;
	coordenadas(casa, &X, &Y);
	// Define os movimentos possíveis do cavalo
	int eixoX[] = {2, 1, -1, -2, -2, -1, 1, 2};
	int eixoY[] = {1, 2, 2, 1, -1, -2, -2, -1};
	// Inicializado com um valor acima do possível (8).
	int minMovimentos = 9, valor = 0; 

	for(int i = 0; i < 8; i++) {
		int auxX = X + eixoX[i];
		int auxY = Y + eixoY[i];
		int aux = (auxX - 1) * sqrt(TABULEIRO) + auxY;
		if (posicaoValida(auxX, auxY) && !visitadas[aux] && vizinhoValido(casa,aux)){
			valor = aux;
			break;
		}
	}
	return valor;
}

/* Função: fitness

    Calcula o fitness de um indivíduo a partir da primeira posição do 
	vetor, considerando o número de movimentos válidos.

	Parâmetros:
		copia - Indivíduo.
		
	Retorno: 
		Um número inteiro que representa o fitness do indivíduo.
*/
INDIVIDUO fitness(INDIVIDUO copia){
	INDIVIDUO adaptado = copia;
	bool visitadas[TABULEIRO + 1] = {false};

	int casa = adaptado.tour[0], contador = 0;
	visitadas[casa] = true;

	for(int i = 0; i < TABULEIRO-1; i++){
		casa = adaptado.tour[i];
		int proximo = adaptado.tour[i+1];

		if(vizinhoValido(casa, proximo) && !visitadas[proximo]){
			visitadas[proximo] = true;
			contador++;
		}
		else{
			//proximo = proximoMovimento(casa, visitadas);
			proximo = regraWarnsdorff(casa, visitadas);
			if(proximo == 0) 
				break;

			adaptado.tour[i+1] = proximo;
			visitadas[proximo] = true;
			contador++;
		}
	}
	
	adaptado.fitness = contador;

	return adaptado;
}

/* Função: proximoMovimento

   	Dado um número de casa os seus vizinhos válidos são calculados e
	armazenados em um vetor. A função sorteia um desses valores e o retorna.

	Parâmetros:
		casa - número de casa.
		
	Retorno: 
		Vizinho válido.
*/
int vetorMovimentos(int casa) {
	int X = 0, Y = 0;
	coordenadas(casa, &X, &Y);

	int eixoX[] = {2, 1, -1, -2, -2, -1, 1, 2};
	int eixoY[] = {1, 2, 2, 1, -1, -2, -2, -1};
	int minMovimentos = 9, movimentos[9] = {0}, j = 0; 

	for(int i = 0; i < 8; i++) {
		int auxX = X + eixoX[i];
		int auxY = Y + eixoY[i];
		int aux = (auxX - 1) * sqrt(TABULEIRO) + auxY;
		if (posicaoValida(auxX, auxY) && vizinhoValido(casa,aux)){
			movimentos[j] = aux;
			j++;
		}
	}
	
	int valor =	movimentos[gerarNumAleatorio(j)];

	return valor;
}

/* Função: inicializa

    Inicializa aleatóriamente a POPULAÇÃO de INDIVÍDUOS.

	Parâmetros:
		população - sequência do tipo INDIVIDUO de tamanho POPULAÇÃO.
		
	Retorno: 
		Nulo.
*/
void inicializa(INDIVIDUO * populacao){
	for(int i = 0; i < POPULACAO; i++){
		for(int j = 0; j < TABULEIRO; j++)
			populacao[i].tour[j] = j;
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

/* Função: inicializaCentro

    Inicializa aleatóriamente a POPULAÇÃO de INDIVÍDUOS, com a exceção 
	da primeira posição do cromossomo dos INDIVÍDUOS que armazena o
	número de uma casa central.

	Parâmetros:
		população - sequência do tipo INDIVIDUO de tamanho POPULAÇÃO.
		
	Retorno: 
		Nulo.
*/
void inicializaCentro(INDIVIDUO *populacao)
{
	int meio = 0, N = sqrt(TABULEIRO);
	meio = (pow(N,2) + 1)/2;

	for(int i = 0; i < POPULACAO; i++){
		populacao[i].tour[0] = meio;
		for(int j = 1; j < TABULEIRO; j++)
			populacao[i].tour[j] = j;
	}
	
	for(int i = 0; i < POPULACAO; i++){
		for(int j = 1; j < TABULEIRO; j++){
			int pos = (rand() % (TABULEIRO-1) + 1);
			int temp = populacao[i].tour[j];
			populacao[i].tour[j] = populacao[i].tour[pos];
			populacao[i].tour[pos] = temp;
		}
		populacao[i] = fitness(populacao[i]);	
	}
}

/* Função: mutacao

    Altera o gene (número de casa) de uma frase filho (cópia), 
	substituindo-o por um número de 1 a NxN.

	Parâmetros:
		filho - Um dos indivíduos da população.
		
	Retorno: 
		O indivíduo da população após ser mutado.
*/
INDIVIDUO mutacao(INDIVIDUO filho){ 
	INDIVIDUO individuo = filho;
	
	int r = gerarNumAleatorio(100);
	if(r <= MUTACAO);
		individuo.tour[gerarNumAleatorio(TABULEIRO)] = rand() % TABULEIRO + 1;

	return individuo;
}

/* Função: mutacaoVizinhos

    Altera o gene 'g+1' (número de casa) de uma frase filho (cópia), 
	substituindo-o por um vizinho válido de 'g'.

	Parâmetros:
		filho - Um dos indivíduos da população.
		
	Retorno: 
		O indivíduo da população após ser mutado.
*/
INDIVIDUO mutacaoVizinhos(INDIVIDUO filho){ 
	INDIVIDUO individuo = filho;
	
	int r = gerarNumAleatorio(100);
	if(r <= MUTACAO){
		int aux = 0;
		do{
			aux = gerarNumAleatorio(TABULEIRO);
		}while(aux == 0 || aux == TABULEIRO-1);
		individuo.tour[aux+1] = vetorMovimentos(individuo.tour[aux]);
	}
	return individuo;
}

/* Função: selecaoPorTorneio

    Enquanto o torneio entre N indivíduos não terminar
	seleciona dois a dois e determina qual deles
	possui o maior fitness.

	Parâmetros:
		populacao - população de indivíduos (frases cópias).
		parametros - PARAMETROS.
		
	Retorno: 
		O indivíduo com menor fitness ontido pelo torneio.
*/
INDIVIDUO selecaoPorTorneio(INDIVIDUO *populacao){
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

/* Função: torneioDosDissimilares

	Se a flag for True o torneio privilegiará
	o INDIVÍDUO de maior fitness, do contrário, o de menor 
	fitness.

	Parâmetros:
		populacao - população de indivíduos (frases cópias).
		parametros - PARAMETROS.
		
	Retorno: 
		O indivíduo com o fitness (maior ou menor) obtido pelo torneio.
*/
INDIVIDUO torneioDosDissimilares(INDIVIDUO *populacao, bool flag){
	INDIVIDUO melhor;

	if(flag){
		melhor.fitness = -1;

		for(int i = 1; i < TORNEIO; i++){
			INDIVIDUO aux = populacao[gerarNumAleatorio(POPULACAO-1)];
			if(melhor.fitness == -1 || aux.fitness > melhor.fitness){
				melhor = aux;
			}
		}
	}else{
		melhor.fitness = TABULEIRO;

		for(int i = 1; i < TORNEIO; i++){
			INDIVIDUO aux = populacao[gerarNumAleatorio(POPULACAO-1)];
			if(melhor.fitness == TABULEIRO || aux.fitness < melhor.fitness){
				melhor = aux;
			}
		}
	}

	return melhor;
}

/* Função: recombinacaoUniforme

    Combina aleatóriamente os genes do pai e mãe
	no filho.

	Parâmetros:
		pai - Um dos indivíduos da população.
		mae - Um dos indivíduos da população.
		
	Retorno: 
		O indivíduo filho resultante da combinação do pai e mãe.
*/
INDIVIDUO recombinacaoUniforme(INDIVIDUO pai, INDIVIDUO mae){
	INDIVIDUO filho;

	for(int i = 0; i < TABULEIRO; i++){
		if(gerarNumAleatorio(2))
			filho.tour[i] = pai.tour[i];
		else
			filho.tour[i] = mae.tour[i];
	}
	return filho;
}

/* Função: comparacaoIndividuos

    Usada para execução do qsort, esta função recebe dois INDIVIDUO
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
		populacao - População de indivíduos (frases cópias).
		
	Retorno: 
		Número de indivíduos selecionados.
*/
int elitismo(INDIVIDUO *populacao){
	int selecionados = POPULACAO*ELITISMO/100;
	qsort(populacao, POPULACAO, sizeof(populacao[0]), comparacao);
	return selecionados;
}

/* Função: reproducao

	Aplica as funções de elitismo, torneio, recombinação, mutação,
	retornando uma nova população.

	Parâmetros:
		populacao - População de indivíduos (frases cópias).
		geracoes - Número de gerações executadas.

	Retorno: 
		Melhor indivíduo da população.
*/
INDIVIDUO reproducao(INDIVIDUO *populacao, int geracao){
	INDIVIDUO *novaPopulacao, melhor, pai, mae, filho;
	novaPopulacao = (INDIVIDUO *)malloc(POPULACAO * sizeof(INDIVIDUO));

	int taxaDeElitismo = elitismo(populacao);
	melhor = populacao[0];

	for(int i = 0; i < POPULACAO-taxaDeElitismo; i++){
		//pai = selecaoPorTorneio(populacao);
		//mae = selecaoPorTorneio(populacao);

		pai = torneioDosDissimilares(populacao, true);
		mae = torneioDosDissimilares(populacao, false);

		filho = recombinacaoUniforme(pai, mae);
		//filho = mutacao(filho);
		filho = mutacaoVizinhos(filho);
		filho = fitness(filho);

		if(filho.fitness > melhor.fitness)
			melhor = filho;

		novaPopulacao[i] = filho;
	}

	for(int i = taxaDeElitismo, j = 0; i < POPULACAO; i++, j++)
		populacao[i] = novaPopulacao[j];

	free(novaPopulacao);
	return melhor;
}

/* Função: verifica

	Verifica se o tour do INDIVÍDUO é valido.

	Parâmetros:
		copia - INDIVÍDUO da população.

	Retorno: 
		Nulo.
*/
void verifica(INDIVIDUO copia){
	bool visitadas[TABULEIRO+1] = {false};

	for(int i = 0; i < TABULEIRO; i++){
		if(visitadas[copia.tour[i]] == true){
			printf("TTTTTTT");
			return;
		}
		visitadas[copia.tour[i]] = true;
	}
	for(int i = 0; i < TABULEIRO-1; i++){
		if(!vizinhoValido(copia.tour[i], copia.tour[i+1])){
			printf("TTTTTTT");
			return;
		}
		visitadas[copia.tour[i]] = true;
	}
}

/* Função: AE	

	Implementa o Algoritmo Evolutivo para o problema do Percurso do Cavalo.
*/
void AE()
{
	clock_t inicio, fim;
	double total = 0;
	inicio = clock();

	INDIVIDUO *populacao, melhor;
	populacao = (INDIVIDUO *)malloc(POPULACAO * sizeof(INDIVIDUO));
	int geracao = 0;

	srand(time(NULL));
	inicializaCentro(populacao);

	for(geracao = 1; geracao <= GERACOES; geracao++){
		melhor = reproducao(populacao, geracao);
		printf("\nIteracao %d, melhor fitness %d.\n", geracao, melhor.fitness);
		
		if(melhor.fitness == TABULEIRO-1) 
			break;
	}

	if(geracao > GERACOES) geracao-=1;

	fim = clock();
	total = (double)(fim-inicio)/CLOCKS_PER_SEC;
	printf("Tempo total gasto pela CPU: %lf\n", total);
	escreveRelatorio(total,melhor.fitness, geracao);
	free(populacao);
}

int main(void){

	//for(int i = 0; i < 10; i++)
		AE();
	
	return 0;
}