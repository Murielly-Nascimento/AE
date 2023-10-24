#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define TABULEIRO 400
typedef struct{
	int fitness;
	int tour[TABULEIRO];
}INDIVIDUO;

#define TORNEIO 4
#define MUTACAO 15
#define ELITISMO 10  
#define GERACOES 10000
#define POPULACAO 1000

void escreveRelatorio(double tempo, int fitness){
	FILE *arq, *binario;
	arq = fopen("resultados.txt", "a");
	binario = fopen("metricasPPC.in", "ab");

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
	int N = sqrt(TABULEIRO);
	return (X >= 1 && X <= N && Y >= 1 && Y <= N);
}

bool vizinhoValido(int atual, int proximo){
	int X = 0, Y = 0, proximoX = 0, proximoY = 0, validoX = 0, validoY = 0;
	coordenadas(atual, &X, &Y);
	coordenadas(proximo, &proximoX, &proximoY);

	validoX = abs(proximoX - X);
	validoY = abs(proximoY - Y);

	return (validoX == 1 && validoY == 2) || (validoX == 2 && validoY == 1);
}

int numeroCasa(int X, int Y){
	int N = sqrt(TABULEIRO);
	int numeroCasa = (Y - 1) * N + X;
	return numeroCasa; 
}

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

int proximoMovimento(int casa, bool visitadas[TABULEIRO+1]) {
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
			proximo = proximoMovimento(casa, visitadas);
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

void inicializa(INDIVIDUO *populacao)
{
	for(int i = 0; i < POPULACAO; i++){
		populacao[i].tour[0] = 1;
		for(int j = 1; j < TABULEIRO; j++)
			populacao[i].tour[j] = j+1;
	}
	
	for(int i = 0; i < POPULACAO; i++){
		for(int j = 1; j < TABULEIRO; j++){
			int pos = (rand() % ((TABULEIRO-1) - 1 + 1)) + 1;
			int temp = populacao[i].tour[j];
			populacao[i].tour[j] = populacao[i].tour[pos];
			populacao[i].tour[pos] = temp;
		}
		populacao[i] = fitness(populacao[i]);	
	}
}

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


INDIVIDUO mutacao(INDIVIDUO filho){ 
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

int comparacao(const void* A, const void* B){
	INDIVIDUO C = *(INDIVIDUO*)A;
	INDIVIDUO D = *(INDIVIDUO*)B;
	if(C.fitness < D.fitness) return 1;
	else return -1;
}

int elitismo(INDIVIDUO *populacao){
	int selecionados = POPULACAO*ELITISMO/100;
	qsort(populacao, POPULACAO, sizeof(populacao[0]), comparacao);
	return selecionados;
}

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

INDIVIDUO reproducao(INDIVIDUO *populacao){
	INDIVIDUO *novaPopulacao, melhor, pai, mae, filho;
	novaPopulacao = (INDIVIDUO *)malloc(POPULACAO * sizeof(INDIVIDUO));

	int taxaDeElitismo = elitismo(populacao);
	melhor = populacao[0];

	for(int i = taxaDeElitismo; i < POPULACAO; i++){
		//pai = selecaoPorTorneio(populacao);
		//mae = selecaoPorTorneio(populacao);
		pai = populacao[i];
		mae = populacao[POPULACAO-i];

		filho = recombinacaoUniforme(pai, mae);
		filho = mutacao(filho);	
		filho = fitness(filho);

		if(filho.fitness > melhor.fitness) 
			melhor = filho;
		novaPopulacao[i] = filho;
	}

	for(int i = taxaDeElitismo; i < POPULACAO; i++)
		populacao[i] = novaPopulacao[i];

	free(novaPopulacao);
	return melhor;
}

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

int main(void)
{
	clock_t inicio, fim;
	double total = 0;
	inicio = clock();

	INDIVIDUO *populacao, melhor;
	populacao = (INDIVIDUO *)malloc(POPULACAO * sizeof(INDIVIDUO));
	int geracao = 0;

	srand(time(NULL));
	inicializa(populacao);

	do{
		melhor = reproducao(populacao);
		printf("\nIteracao %d, melhor fitness %d.\n", geracao, melhor.fitness);
		geracao++;
		if(melhor.fitness == TABULEIRO-1) break;
	}while(geracao <= GERACOES);

	fim = clock();
	total = (double)(fim-inicio)/CLOCKS_PER_SEC;
	printf("Tempo total gasto pela CPU: %lf\n", total);
	escreveRelatorio(total,melhor.fitness);
	verifica(melhor);
	free(populacao);
	
	return 0;
}