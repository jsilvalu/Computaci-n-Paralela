typedef struct {
	double  x, y; 
} punto_t;

/**
* Función del kernel potencias
* Se calculan las potencias operando paralelamente con accesos a memoria
*/
__kernel void potencias(__global punto_t *baseEXP, __global double *vectorPotencias, int MAXExp, char ejecutarX1_ejecutarY0){
    
    double potencia = 1, base=0;
    int i;
    
    if(ejecutarX1_ejecutarY0)	
	base = baseEXP[get_global_id(0)].x;
    else	
	base = baseEXP[get_global_id(0)].y;

    for(i=0; i<MAXExp; i++){
        vectorPotencias[get_global_id(0) * MAXExp + i] = potencia;
        potencia = potencia * base;
    }
}

/**
* Funcion del kernel funcion
* Se realiza el cálculo de la fórmula general de la función masiva.
*/
__kernel void funcion(int tamMallaPts, int MAXExp, __global double *A, __global double *MatrizPotX, __global double *MatrizPotY, __global double *vectorResultado){

    int i, iter_n, iter_m;
    double numerador=0, denominador=0;

    for(i = ((tamMallaPts/get_global_size(0)) * get_global_id(0)); i<((get_global_id(0)+1)*(tamMallaPts/get_global_size(0))); i++){
        vectorResultado[i] = 0;
        //Obtención de iteración aplicada a la fórmula general de la función masiva
        for(iter_n=0; iter_n<MAXExp; iter_n++)
            for(iter_m=0; iter_m<MAXExp; iter_m++){
                //Asigno A de n,m en el numerador
		numerador= A[iter_n*MAXExp+iter_m]; 
                //Asigno a denominador el valor de X elevado a n e Y elevado a m de la iteración i
		denominador= MatrizPotX[i*MAXExp+iter_n] * MatrizPotY[i*MAXExp+iter_m];
                //Sumatorio del resultado final de la operación acumulando
                vectorResultado[i] +=  numerador/denominador ;
            }
    }
}
