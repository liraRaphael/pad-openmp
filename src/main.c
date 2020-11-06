#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#define posicao(I, J, COLUNAS) ((I)*(COLUNAS) + (J))


//gerador de numeros aleat�rios
float random_number(){
	return ((float) (rand() % 2000)/100)  - 10;
}


// aloca dinamicamente como proposto pelo professor
float * alocar(int dimensaoA,int dimensaoB){
	float * ponteiro;
	ponteiro = malloc(sizeof(float) * dimensaoA * dimensaoB);
	return ponteiro;
}


// gera a matriz de forma aleat�ria
float * gerarMatriz(char * path,int dimensaoA,int dimensaoB){
	
  // manipula o arquivo para escrita
	FILE * arquivo;
	arquivo = fopen(path,"w");
	
  //aloca e define o tamanho total
	float * matriz = alocar(dimensaoA,dimensaoB);	
	int MAX = dimensaoA * dimensaoB;
	
	int i = 0;
		
  //faz o loop, atribuindo valor aleat�rio e salva o arquivo
	for(i = 0;i<MAX;i++){
		matriz[i] = random_number();
		fprintf(arquivo,"%.2f\n",matriz[i]);
	}
	
	fclose(arquivo);
	
	return matriz;
	
}

// le o arquivo 
float * lerArquivo(char * path,int dimensaoA,int dimensaoB){
	
  //abre o arquivo para leitura
	FILE * arquivo;
	arquivo = fopen(path,"r");
	
  //caso o arquivo n�o exista, gera um novo
	if(!arquivo){		
		return gerarMatriz(path,dimensaoA,dimensaoB);
	}
		
		
  // aloca a matriz dinamicamente
	float * matriz = alocar(dimensaoA,dimensaoB);	
	int MAX = dimensaoA * dimensaoB;
	
	int i = 0;
	
 
   // faz o loop de leitura
	for(i = 0;i<MAX;i++){
		fscanf(arquivo,"%f\n", &matriz[i]);	
	}
	
	fclose(arquivo);
	
	return matriz;
	
}


int main(int argc,char ** argv){
	
  // verifica se todos os argumentos est�o
	if(argc != 8){
		printf("argumentos invalidos!\n");
		return 1;
	}
	
	int 
		i,j,k;
		
  // transforma o argumento em inteiro
	int y = atoi(argv[1]);
	int w = atoi(argv[2]);
	int v = atoi(argv[3]);
	

  //aloca e le os arquivos do vetor
	float 
		* matrizA = lerArquivo(argv[4],y,w), 
		* matrizB = lerArquivo(argv[5],w,v), 
		* matrizC = lerArquivo(argv[6],v,1), 
		* matrizD = alocar(y,1),
    * matrizAux = alocar(y,v),
		result;	
		
  // caso falhe algum arquivo
	if(matrizA == NULL || matrizB == NULL || matrizC == NULL){		
		return 1;
	}
	
 
	
	///calcula a matriz D
	#pragma omp parallel for shared(y,w,v,matrizA,matrizB,matrizC,matrizD,matrizAux) private(i,j,k)
	for(i=0;i<y;i++){	       							
		for(j=0;j<v;j++){	
      matrizAux[posicao(i,j,v)] = 0.0;           						
			for(k=0;k<w;k++){	
				matrizAux[posicao(i,j,v)] += (matrizA[posicao(i,k,w)] * matrizB[posicao(k,j,v)]) ;										
			}
		}					
	}
 

  ///calcula a matriz D
	#pragma omp parallel for shared(y,w,v,matrizA,matrizB,matrizC,matrizD) private(i,j,result)
  for(i=0;i<y;i++){	 
    matrizD[i] = 0.0;
    for(j=0;j<v;j++){	
      
		  matrizD[i] += matrizAux[posicao(i,j,v)] * matrizC[j];							
    }	
  }

 
	
	
	result = 0.0;
	//faz a soma da redu��o
	#pragma omp parallel shared(matrizD) private(i)
	{
		#pragma omp parallel for reduction(+:result) shared(matrizD) private(i)
		for(i=0;i<y;i++){
       
			result+= matrizD[i];
		}
	}
	
  // printa a redu��o
	printf("%f",result);
	
  // abre o arquivo para grava��o da matriz D
	FILE * arquivo;
	arquivo = fopen(argv[7],"w");
	
  //faz o loop de grava��o
	for(i=0;i<y;i++){
		fprintf(arquivo,"%.2f\n",matrizD[i]);
	}
	
	fclose(arquivo);	
	
  return 0;
  
  
}
