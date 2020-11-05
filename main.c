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
	
	#pragma omp parallel for
	for(int i = 0;i<MAX;i++){
		matriz[i] = random_number();
		fprintf(arquivo,"%f\n",matriz[i]);
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
		* matrizA, 
		* matrizB, 
		* matrizC, 
		* matrizD = alocar(y,1),
		result;	
	
	#pragma omp parallel shared(y,w,v,matrizA,matrizB,matrizC)
	{
		matrizA = gerarMatriz(argv[4],y,w);
		matrizB = gerarMatriz(argv[5],w,v);
		matrizC = gerarMatriz(argv[6],v,1);
	}
	
	FILE * arquivo;
	arquivo = fopen(argv[7],"w");
	
	#pragma omp parallel shared(y,w,v,matrizA,matrizB,matrizC,matrizD) private(i,j,k)
	{
		
		#pragma omp parallel for reduction(+:result) collapse(3) shared(y,w,v,matrizA,matrizB,matrizC,matrizD) private(i,j,k)
		for(i=0;i<y;i++){
			result = 0;
			for(j=0;j<w;j++){
				for(k=0;k<v;k++){
					result+= (matrizA[posicao(i,j,w)] * matrizB[posicao(j,k,v)]) * matrizC[posicao(k,1,1)];
				}
			}	
			fprintf(arquivo,"%f\n",result);
			matrizD[posicao(y,1,1)] = result;
			
		}
		
	}
	
	fclose(arquivo);
	
    return 0;
}
