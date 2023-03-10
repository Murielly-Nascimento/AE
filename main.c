/*
	@file AE.c
	@brief Implementação de um algoritmo evolutivo
	@author Rosetta Code
	@date 04/01/2023

	O código a seguir é uma adaptação do disponibilizado no site Rosetta Code. 
	A versão original é encontrada neste link: https://rosettacode.org/wiki/Evolutionary_algorithm

	@details Pseudocódigo
	
    Início
        Sequência de caracteres alvo: "O TEJO GUARDA GRANDES NAVIOS".
        Uma sequência de caracteres aleatória (pai) com todas as letras maiúsculas do alfabeto e um espaço, do mesmo tamanho da frase alvo.
        Uma função de fitness que calcula o quão próximo a frase pai está da frase alvo.
        Uma função de mutação que, dado uma frase e um grau de mutação, retorna uma cópia da frase com alguns caracteres modificados.
        Enquanto a frase pai não for igual a frase alvo:
            copia a frase pai C vezes, sendo que em cada vez há uma probabilidade de que outro caractere sofra mutação.
            Seleciona a frase pai com o maior grau de fitness dentre as cópias e descarta as demais.
            Repete até que a frase pai, seja igual (ou semelhante) a frase alvo.
	Fim

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char alvo[] = "o tejo guarda grandes navios";
const char pai[] = "abcdefghijklmnopqrstuvxwz ";

#define ESCOLHA (sizeof(pai) - 1)
#define MUTACAO 30
#define COPIAS 30


// Retorna um número aleatório de 0 a n-1 
int gerarNumAleatorio(int n)
{
	int r, rand_max = RAND_MAX - (RAND_MAX % n);
	while((r = rand()) >= rand_max);
	return r / (rand_max / n);
}

// Número de caracteres diferentes entre a sequência A e B
int grauDeDiferenca(const char *copia, const char *alvo)
{
	int i = 0, qtdDistintos = 0;
	for (i = 0; copia[i]; i++)
		qtdDistintos += (copia[i] != alvo[i]);
	return qtdDistintos;
}

void mutacao(const char *a, char *b)
{
	int i = 0;
	for (i = 0; i < a[i]; i++){
		if(gerarNumAleatorio(MUTACAO)) b[i] = a[i];
		else b[i] = pai[gerarNumAleatorio(ESCOLHA)];
	}
	b[i] = '\0';
}

int inicializa(int n)
{
	int r, rand_max = RAND_MAX - (RAND_MAX % n);
	srand(time(NULL));
	while((r = rand()) >= rand_max);
	return r / (rand_max / n);
}

int main(void)
{
	int i = 0, j = 0, unfit, melhor, iteracao = 0;
	char descendentes[COPIAS][sizeof(alvo) / sizeof(char)];

	descendentes[0][0] = pai[inicializa(ESCOLHA)];

	for (i = 1; i < alvo[i]; i++)
		descendentes[0][i] = pai[gerarNumAleatorio(ESCOLHA)];
	descendentes[0][i] = 0;

	do{
		for (i = 1; i < COPIAS; i++)
			//recombinacao(descendentes[0], descendentes[i]);
			mutacao(descendentes[0], descendentes[i]);

		//Encontra a cópia com o melhor fitness
		for (i = 0; i < COPIAS; i++) {
			unfit = grauDeDiferenca(descendentes[i], alvo);
			if(unfit < melhor || !i) {
				melhor = unfit;
				j = i;
			}
		}

		if(j != 0){
			strcpy(descendentes[0], descendentes[j]);
		}	
		printf("Iteracao %d, pontos %d: %s\n", iteracao++, melhor, descendentes[0]);
	}while (melhor != 0);

	return 0;
}