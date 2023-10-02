#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <stdbool.h>
#include <math.h>

#define TABULEIRO 25 // Tabuleiro 5x5
typedef struct{
	int fitness;
	int indiceMaiorSeq;
	int tour[TABULEIRO];
}INDIVIDUO;

#define TORNEIO 3
#define MUTACAO 30 
#define ELITISMO 5
#define GERACOES 300
#define POPULACAO 100

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

int numeroCasa(int X, int Y){
	int N = sqrt(TABULEIRO);
	int numeroCasa = (Y - 1) * N + X;
	return numeroCasa; 
}

bool posicaoValida(int X, int Y){
	int N = sqrt(TABULEIRO);
	return (X >= 1 && X <= N && Y >= 1 && Y <= N);
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

int proximoMovimento(int X, int Y) {
	// Define os movimentos possíveis do cavalo
	int eixoX[] = {2, 1, -1, -2, -2, -1, 1, 2};
	int eixoY[] = {1, 2, 2, 1, -1, -2, -2, -1};

	// Inicializado com um valor acima do possível (8).
	int minMovimentos = 9, valor = 0; 

    for (int i = 0; i < 8; i++) {
        int auxX = X + eixoX[i];
        int auxY = Y + eixoY[i];
        if (posicaoValida(auxX, auxY)) {
            int movimentos = movimentosPossiveis(auxX, auxY);
            if (movimentos < minMovimentos){
                minMovimentos = movimentos;
				valor  = numeroCasa(auxX, auxY);
            }
        }
    }
	return valor;
}

bool vizinhoValido(int atual, int proximo){
	int X = 0, Y = 0, proximoX = 0, proximoY = 0, validoX = 0, validoY = 0;
	coordenadas(atual, &X, &Y);
	coordenadas(proximo, &proximoX, &proximoY);

	validoX = abs(proximoX - X);
	validoY = abs(proximoY - Y);

	return (validoX == 1 && validoY == 2) || (validoX == 2 && validoY == 1);
}

INDIVIDUO regraWandorsnoff(INDIVIDUO copia){
	INDIVIDUO adaptado = copia;
	int posicao[TABULEIRO + 1] = {false};
	
	for(int i = 0; i < TABULEIRO; i++)
		posicao[copia.tour[i]] = i;

	
	for(int i = 0; i < TABULEIRO-1; i++){
		int casa = copia.tour[i];
		int proximo = copia.tour[i+1];
		
		if(!vizinhoValido(casa, proximo)){
			int X = 0, Y = 0;
			coordenadas(casa, &X, &Y);
			proximo = proximoMovimento(X, Y);
			int aux = copia.tour[i+1], aux2 = posicao[proximo];
			copia.tour[i+1] = proximo;
			copia.tour[posicao[proximo]] = aux;
			posicao[proximo] = i+1;
			posicao[aux] = aux2;
			
		}
	}
	adaptado = copia;
	return adaptado;
}

INDIVIDUO fitness(INDIVIDUO copia)
{	
	INDIVIDUO adaptado = copia;
	int contador = 0, maiorSequencia = 0, indiceMaiorSeq = 0, indice = 0;
	for(int i = 0; i < TABULEIRO; i++){
		if(vizinhoValido(adaptado.tour[i], adaptado.tour[i+1])){
			contador++;
			if(contador == 1) 
				indice = i;
		}
		else{
			if(contador > maiorSequencia){
				maiorSequencia = contador;
				indiceMaiorSeq = indice;
			}
			contador = 0;
			indice = i; 
		}
	}
	adaptado.fitness = maiorSequencia;
	adaptado.indiceMaiorSeq = indiceMaiorSeq;

	return adaptado;
}

void inicializa(INDIVIDUO populacao[POPULACAO])
{
	for(int i = 0; i < POPULACAO; i++){
		for(int j = 0; j < TABULEIRO; j++)
			populacao[i].tour[j] = j+1;
	}
	/*
	for(int i = 0; i < POPULACAO; i++){
		populacao[i] = adaptacao(populacao[i]);
		populacao[i] = fitness(populacao[i]);
	}*/
	
	
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

INDIVIDUO mutacao(INDIVIDUO filho){ 
	INDIVIDUO individuo = filho;
	
	int r = gerarNumAleatorio(100);	
	if(r <= MUTACAO){
		int A = rand() % TABULEIRO + 1;
		int B = rand() % TABULEIRO + 1;
		int C = individuo.tour[A];
		individuo.tour[A] = individuo.tour[B];
		individuo.tour[B] = C;
	}
	return individuo;
}

INDIVIDUO recombinacaoUniforme(INDIVIDUO pai, INDIVIDUO mae){
	INDIVIDUO filho;

	bool visitadas[TABULEIRO+1] = {false};
	for(int i = 0; i < TABULEIRO; i++){
		if(gerarNumAleatorio(2) == 1)
			filho.tour[i] = pai.tour[i];
		else
			filho.tour[i] = mae.tour[i];
		
		int casa = filho.tour[i];
		if(visitadas[casa]){
			for(int j = 1; j<= TABULEIRO; j++){
				if(visitadas[j] == false){
					filho.tour[i] = j;
					visitadas[j] = true;
					break;
				}
			}
		}
		visitadas[casa] = true;
	}

	return filho;
}

int comparacao(const void* A, const void* B){
	INDIVIDUO C = *(INDIVIDUO*)A;
	INDIVIDUO D = *(INDIVIDUO*)B;
	if(C.fitness < D.fitness) return 1;
	else return -1;
}

int elitismo(INDIVIDUO populacao[POPULACAO]){
	//Elitismo aplicado a 5% da população
	int selecionados = POPULACAO*ELITISMO/100;
	qsort(populacao, POPULACAO, sizeof(populacao[0]), comparacao);
	return selecionados;
}

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

INDIVIDUO reproducao(INDIVIDUO populacao[POPULACAO], int geracoes){
	INDIVIDUO novaPopulacao[POPULACAO], melhor, pai, mae, filho;
	melhor.fitness = -1;
	int taxaDeElitismo = 0;

	//Elitismo aplicado a 1/3 do número de gerações
	if(geracoes >= GERACOES/2){
		taxaDeElitismo = elitismo(populacao);
		melhor = populacao[0];
	}

	for(int i = taxaDeElitismo; i < POPULACAO; i++){
		pai = selecaoPorTorneio(populacao);
		mae = selecaoPorTorneio(populacao);

		filho = recombinacaoUniforme(pai, mae);
		filho = mutacao(filho);
		//filho = adaptacao(filho);
		filho = fitness(filho);

		if(filho.fitness > melhor.fitness) 
			melhor = filho;
	
		novaPopulacao[i] = filho;
	}

	for(int i = taxaDeElitismo; i < POPULACAO; i++)
		populacao[i] = novaPopulacao[i];

	return melhor;
}


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