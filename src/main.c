#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#define posicao(I, J, COLUNAS) ((I)*(COLUNAS) + (J))

float random_number(){
	return ((float) (rand() % 2000)/100)  - 10;
}


float * alocar(int dimensaoA,int dimensaoB){
	float * ponteiro;
	ponteiro = malloc(sizeof(float) * dimensaoA * dimensaoB);
	return ponteiro;
}

float * gerarMatriz(char * path,int dimensaoA,int dimensaoB){
	
	FILE * arquivo;
	arquivo = fopen(path,"w");
	
	float * matriz = alocar(dimensaoA,dimensaoB);	
	int MAX = dimensaoA * dimensaoB;
	
	int i = 0;
		
	for(i = 0;i<MAX;i++){
		matriz[i] = random_number();
		fprintf(arquivo,"%.2f\n",matriz[i]);
	}
	
	fclose(arquivo);
	
	return matriz;
	
}


float * lerArquivo(char * path,int dimensaoA,int dimensaoB){
	
	FILE * arquivo;
	arquivo = fopen(path,"r");
	
	if(!arquivo){		
		return gerarMatriz(path,dimensaoA,dimensaoB);
	}
		
		
	float * matriz = alocar(dimensaoA,dimensaoB);	
	int MAX = dimensaoA * dimensaoB;
	
	int i = 0;
	
	for(i = 0;i<MAX;i++){
		fscanf(arquivo,"%f\n", &matriz[i]);
	}
	
	fclose(arquivo);
	
	return matriz;
	
}


int main(int argc,char ** argv){
	
	if(argc != 8){
		printf("argumentos invalidos!\n");
		return 1;
	}
	
	int 
		i,j,k;
		
	int y = atoi(argv[1]);
	int w = atoi(argv[2]);
	int v = atoi(argv[3]);
	

	float 
		* matrizA = lerArquivo(argv[4],y,w), 
		* matrizB = lerArquivo(argv[5],w,v), 
		* matrizC = lerArquivo(argv[6],v,1), 
		* matrizD = alocar(y,1),
		result;	
		
	if(matrizA == NULL || matrizB == NULL || matrizC == NULL){		
		return 1;
	}
	
	///calcula a matriz D
	#pragma omp parallel shared(y,w,v,matrizA,matrizB,matrizC,matrizD,arquivo) private(i,j,k)
	{
		
		#pragma omp parallel for collapse(3) shared(y,w,v,matrizA,matrizB,matrizC,matrizD,arquivo) private(i,j,k)
		for(i=0;i<y;i++){
			matrizD[posicao(y,1,1)] = 0;
			for(j=0;j<w;j++){
				for(k=0;k<v;k++){
					matrizD[posicao(y,1,1)] += (matrizA[posicao(i,j,w)] * matrizB[posicao(j,k,v)]) * matrizC[posicao(k,1,1)];
				}
			}	
		}
		
	}
		
	
	result = 0;
	//faz a soma da redução
	#pragma omp parallel shared(matrizD) private(i)
	{
		#pragma omp parallel for reduction(+:result) shared(matrizD) private(i)
		for(i=0;i<y;i++){
			result = matrizD[posicao(y,1,1)];
		}
	}
	
	FILE * arquivo;
	arquivo = fopen(argv[7],"w");
	
	for(i=0;i<y;i++){
		fprintf(arquivo,"%.2f\n",matrizD[posicao(y,1,1)]);
	}
	
	fclose(arquivo);
	
	
	printf("%.2f",result);
	
	
	
    return 0;
}
