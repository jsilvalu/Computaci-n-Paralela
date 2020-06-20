#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
//Definición de color para impresión
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"
typedef struct {
	double  x,  // Coordenada x del punto
		   y;  // Coordenada y del punto
} punto_t;

void initialize(double *m,int N, punto_t *mesh, int P, double lr, double ur, double lm, double um)
{
	int i,j;
	double space;

	//Los valores del polinomio se general aleatoriamente
	for(i=0;i<N*N;i++)  
		m[i] =((1.*rand())/RAND_MAX)*(ur-lr)+lr;
	// Los puntos de la malla se generan equidistantes
	space=(um-lm)/(P-1);
	for(i=0;i<P;i++)
		for(j=0;j<P;j++) {
			mesh[i*P+j].x=lm+i*space;
			mesh[i*P+j].y=lm+j*space;
		}
}

void escribir(double *m, int t){
	int i, j;

	for (i = 0; i < t; i++) {
		for (j = 0; j < t; j++)
			printf("%.2lf ",m[i*t+j]);
		printf("\n");
	}
	printf("\n");
}

void escribirm(punto_t *m, int t, int exp){
	int i, j;
	printf("Coordenadas x de la malla del experimento %d:\n", exp);
	for (i = 0; i < t; i++) {
		for (j = 0; j < t; j++)
			printf("%.2f ",m[i*t+j].x);
		printf("\n");
	}
	printf("\nCoordenadas y de la malla del experimento %d:\n", exp);
	for (i = 0; i < t; i++) {
		for (j = 0; j < t; j++)
			printf("%.2f ",m[i*t+j].y);
		printf("\n");
	}
	printf("\n");
}

/*
c
c     mseconds - returns elapsed milliseconds since Jan 1st, 1970.
c
*/
long long mseconds(){
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec*1000 + t.tv_usec/1000;
}

int ObtenerParametros(int argc, char *argv[], int *debug) {
	int i;
	*debug=0;
	if (argc<2)
		return 0;
	for (i=2; i<argc;) {
		if (strcmp(argv[i], "-d")==0) {
			*debug=1;
			i++;
		}
		else
			return 0;
	}
	return 1;
}

// **************************************************************************
// ***************************** IMPLEMENTACI�N *****************************
// **************************************************************************
/*
Implementaci�n secuencial
N -> Tama�o de la matriz de coeficientes (NxN)
A -> Matriz de coeficientes
P -> Tama�o de la malla de puntos
malla -> Malla de coordenadas (x,y) de tama�o P
r -> Vector resultado de tama�o P
*/
void sec(int N,double *A, int P, punto_t *malla, double *r) {
	
	double potenciaN=0.0, potenciaM=0.0, denominador=0.0,numerador =0.0;
	//Memoria dinámica para el resultado en un vector memory allocation
	double *vectoResul =(double *) malloc(sizeof(double)*N);

	//i - Iterador, iter_n - Iterador del sumatorio de la n, iter_m iterador del sumatorio de m
	int i=0, iter_n=0, iter_m=0;

	for(i=0; i<P; i++){   //Bucle iterador
		potenciaN = potenciaM = 1;

		for(iter_n=0; iter_n <= N-1; iter_n++){	//Bucle para el sumatorio de n

			for(iter_m=0; iter_m <= N-1; iter_m++){  //Bucle para el sumatorio  de m

				if(iter_n == 0){//Calculo funcion									
					vectoResul[iter_m] = potenciaM;  
					denominador = potenciaN * potenciaM;  //Obtengo el denominador
					numerador = A[iter_n*N+iter_m];		  //Obtengo el numerador
					r[i] +=  (numerador / denominador);   //Obtengo el cociente resultante
					potenciaM = potenciaM * malla[i].y;
				}else{		
					numerador = A[iter_n*N+iter_m];       //Obtengo el numerador
					denominador = potenciaN * vectoResul[iter_m];// el denominador						
					r[i] += numerador / denominador ;     //Obtengo el cociente resultante
				}
			}
			potenciaN = potenciaN * malla[i].x;
		}
	}
}

// **************************************************************************
// *************************** FIN IMPLEMENTACI�N ***************************
// **************************************************************************

/*
Recibir� los siguientes par�metros (los par�metros entre corchetes son opcionales): fichEntrada [-d]
fichEntrada -> Obligatorio. Fichero de entrada con los par�metros de lanzamiento de los experimentos
-d -> Opcional. Si se indica, se mostrar�n por pantalla los valores iniciales, finales y tiempo de cada experimento
*/
int main(int argc,char *argv[]) {
	int i,
		debug=0,	 // Indica si se desean mostrar los tiempos y resultados parciales de los experimentos
		N,		 // Tama�o de la matriz de coeficientes A (NxN)
		P,		 // N�mero de puntos de la malla (PxP)
		cuantos,	 // N�mero de experimentos a realizar
		semilla;	 // Semilla para la generaci�n de n�meros aleatorios
	double *A, 	 // Matriz de coeficientes. Se representa en forma de vector. Para acceder a la fila f y la columna c: A[f*N+c]
		  *r,      // Resultado (PxP)
		  lr,	 // Valor m�nimo de los coeficientes
		  ur,	 // Valor m�ximo de los coeficientes
		  lm,	 // Valor m�nimo de la malla
		  um;	 // Valor m�ximo de la malla
	punto_t *malla; // Malla de puntos (PxP)
	long long ti,	 // Tiempo inicial
			tf,	 // Tiempo final
			tt=0; // Tiempo acumulado de los tiempos parciales de todos los experimentos realizados
	FILE *f;		 // Fichero con los datos de entrada
	
	if (!ObtenerParametros(argc, argv, &debug)) {
		printf(ANSI_COLOR_RED " [ERROR!] Ejecucion incorrecta\nEl formato correcto es %s fichEntrada [-d]\n" ANSI_COLOR_RESET "\n", argv[0]);
		return 0;
	}
	
	
	// Se leen el n�mero de experimentos a realizar
	f=fopen(argv[1],"r");
	fscanf(f, "%d",&cuantos);
	for(i=0;i<cuantos;i++)
	{
		fscanf(f, "%d",&N);       // Tama�o de la matriz de coeficientes A (NxN)
		fscanf(f, "%d",&P);       // N�mero de puntos de la malla (PxP)
		fscanf(f, "%d",&semilla); // Semilla para la generacion aleatoria de los datos
		fscanf(f, "%lf",&lr);	 // Valor m�nimo de los coeficientes
		fscanf(f, "%lf",&ur);	 // Valor m�ximo de los coeficientes
		fscanf(f, "%lf",&lm);	 // Valor m�nimo de la malla
		fscanf(f, "%lf",&um);	 // Valor m�ximo de la malla

		// Reserva de memoria para los coeficientes, los puntos en la malla y el resultado
		A = (double *) calloc(sizeof(double),N*N);
		malla = (punto_t *) malloc(sizeof(punto_t)*P*P);
		r=(double *) malloc(sizeof(double)*P*P);

		srand(semilla);
		initialize(A,N,malla,P,lr,ur,lm,um);

		if (debug) {
			printf("Matriz de coeficientes del experimento %d:\n", i); escribir(A,N);
			escribirm(malla,P,i);
		}
		
		ti=mseconds(); 
		sec(N,A,P*P,malla,r);
		tf=mseconds(); 
		tt+=tf-ti;
		
		if (debug) {
			printf("\n" ANSI_COLOR_CYAN);
			printf("======================================================================\n");
			printf("=========  Tiempo del experimento %d: %Ld ms\n", i, tf-ti);
			printf("======================================================================\n\n" ANSI_COLOR_RESET);
			printf(" · Resultado del experimento %d:\n\n", i); escribir(r, P);
		}
		free(A);
		free(malla);
		free(r);
    }
    	printf("\n" ANSI_COLOR_CYAN);
	printf("======================================================================\n");
	printf("=========  Tiempo total de %d experimentos: %Ld ms\n", cuantos, tt);
	printf("======================================================================\n\n" ANSI_COLOR_RESET);
    fclose(f);
	return 0;
}
