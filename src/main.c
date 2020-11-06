#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#define posicao(I, J, COLUNAS) ((I)*(COLUNAS) + (J))


//gerador de numeros aleatórios
float random_number(){
	return ((float) (rand() % 2000)/100)  - 10;
}


// aloca dinamicamente como proposto pelo professor
float * alocar(int dimensaoA,int dimensaoB){
	float * ponteiro;
	ponteiro = malloc(sizeof(float) * dimensaoA * dimensaoB);
	return ponteiro;
}


// gera a matriz de forma aleatória
float * gerarMatriz(char * path,int dimensaoA,int dimensaoB){
	
  // manipula o arquivo para escrita
	FILE * arquivo;
	arquivo = fopen(path,"w");
	
  //aloca e define o tamanho total
	float * matriz = alocar(dimensaoA,dimensaoB);	
	int MAX = dimensaoA * dimensaoB;
	
	int i = 0;
		
  //faz o loop, atribuindo valor aleatório e salva o arquivo
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
	
  //caso o arquivo não exista, gera um novo
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

/**
*
*
* - Calcula a matriz A*B
*
*/
float * calculaMatrizAB(int y, int w, int v,float * matrizA,float * matrizB){

  int
    i,j,k;
    
  float
    * ab = alocar(y,v); // gera uma matriz AByxv = Ayxw * Bwxv

  
	#pragma omp parallel for shared(y,w,v,matrizA,matrizB,ab) private(i,j,k)
	for(i=0;i<y;i++){	       							
		for(j=0;j<v;j++){	
       //zera a matriz temporaria para fazer a somatória
      ab[posicao(i,j,v)] = 0.0;           						
			for(k=0;k<w;k++){	
				ab[posicao(i,j,v)] += (matrizA[posicao(i,k,w)] * matrizB[posicao(k,j,v)]) ;										
			}
		}					
	}
 
   return ab;
 
} 

/**
*
*
* - Calcula a matriz C
*
*/
float * calculaMatrizD(int y, int v,float * matrizC,float * matrizAB){

  int
    i,j,k;
    
  float
    * matrizD = alocar(y,1); // gera o "vetor" D

  
	///calcula a matriz D = aux * C
	#pragma omp parallel for shared(y,v,matrizC,matrizD) private(i,j)
  for(i=0;i<y;i++){	 
    matrizD[i] = 0.0;
    for(j=0;j<v;j++){	      
		  matrizD[i] += matrizAB[posicao(i,j,v)] * matrizC[j];							
    }	
  }
 
   return matrizD;
 
} 

double reducaoMatrizD(int y, float * matrizD){
  
  int
    i;
  
  double
    reducao = 0;
    
	#pragma omp parallel for reduction(+:reducao) shared(matrizD,y) private(i)
	for(i=0;i<y;i++){             
		reducao += matrizD[i];
	}
 
   return reducao;
 
}


int main(int argc,char ** argv){
	
  // verifica se todos os argumentos estão
	if(argc != 8){
		printf("argumentos invalidos!\n");
		return 1;
	}
	
	int 
		i,j,k,
    y = atoi(argv[1]),
    w = atoi(argv[2]),
    v = atoi(argv[3]);
		

  //aloca e le os arquivos do vetor
	float 
		* matrizA = lerArquivo(argv[4],y,w), 
		* matrizB = lerArquivo(argv[5],w,v), 
		* matrizC = lerArquivo(argv[6],v,1), 
		* matrizD,
    * matrizAB;
  
  double
		reducao;	//salvara o resultado da redução
		
  // caso falhe algum arquivo
	if(matrizA == NULL || matrizB == NULL || matrizC == NULL){		
		return 1;
	}
 
 
	
   /*
   *
   *
   * - EXECUÇÃO DO CALCULO DAS MATRIZES 
   *
   *
   */
   
   matrizAB = calculaMatrizAB(y,w,v,matrizA,matrizB);
   matrizD = calculaMatrizD(y,v,matrizC,matrizAB);
   reducao = reducaoMatrizD(y,matrizD);
	  

  /*
   *
   *
   * - FIM DA EXECUÇÃO DE REDUÇÃO  
   *
   *
   */
   
   
   
	
  // printa a redução
	printf("%f",reducao);
	
  // abre o arquivo para gravação da matriz D
	FILE * arquivo;
	arquivo = fopen(argv[7],"w");
	
  //faz o loop de gravação
	for(i=0;i<y;i++){
		fprintf(arquivo,"%.2f\n",matrizD[i]);
	}
	
	fclose(arquivo);	
	
  return 0;
  
  
}
