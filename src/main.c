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
		i,j,k,l;
		
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
	#pragma omp parallel for shared(y,w,v,matrizA,matrizB,matrizC,matrizD) private(i,j,k,l,result)
	for(i=0;i<y;i++){	
    result = 0;								
		for(j=0;j<w;j++){							
			for(k=0;k<v;k++){	
				result += (matrizA[posicao(i,j,w)] * matrizB[posicao(j,k,v)]);										
			}
		}			
		for(l=0;l<v;l++){								
			result += matrizC[l];
		}		
		matrizD[i] = result;
	}
	
 
 
	
	
	result = 0;
	//faz a soma da redução
	#pragma omp parallel shared(matrizD) private(i)
	{
		#pragma omp parallel for reduction(+:result) shared(matrizD) private(i)
		for(i=0;i<y;i++){
			result+= matrizD[i];
		}
	}
	
	printf("%.2f",result);
	
	FILE * arquivo;
	arquivo = fopen(argv[7],"w");
	
	for(i=0;i<y;i++){
		fprintf(arquivo,"%.2f\n",matrizD[i]);
	}
	
	fclose(arquivo);	
	
    return 0;
}
