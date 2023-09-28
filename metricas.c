#include<stdio.h>
#include<math.h>

int main(void){

	FILE *binario = fopen("metricasPPC.in", "rb");

	if(binario == NULL){
		printf("Erro ao abrir arquivo.\n");
		return 0;
	}

	int fitness[10] = {0}, i = 0, finalArquivo = 1;
	double tempo = 0, mediaTempo = 0, mediaFitness = 0, desvioPadrao = 0;
	
	while(!feof(binario)){
		fread(&tempo, sizeof(double), 1, binario);
		fread(&fitness[i], sizeof(int), 1, binario);
		
		mediaTempo += tempo;
		mediaFitness += fitness[i];
		i++;
	}

	mediaFitness = mediaFitness/10;
	mediaTempo = mediaTempo/10;

	for(int i = 0; i < 10; i++){
		desvioPadrao += pow(fitness[i] - mediaFitness, 2);
	}
	desvioPadrao = sqrt(desvioPadrao / 10);
	
	fclose(binario);

	return 0;
}