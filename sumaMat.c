#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

void sumaMat0(double* m1, double* m2, int n, int m, double *matRes){
    long long int index;
    for(int i = 0; i < n; i++){
	for (int j = 0; j < m; j++ ){
	index = (long long int) i*m+j;
        matRes[index] = m1[index] + m2[index]; }
    }
}

void sumaMatPar(double* m1, double* m2, int n, int m, double *matRes){
    #pragma omp parallel for
    for(int i = 0; i < n; i++){
	for(int j = 0; j < m; j++){
            matRes[i*m+j] = m1[i*m+j] + m2[i*m+j];
	}
    }
}

int checkEquals(int* m1, int* m2, int n, int m){
    for(int i = 0; i < n*m; i++)
        if(m1[i] != m2[i])
            return 0;
    return 1;
}

void fillMat(int n, int m, double *matRes, int s0){
    #pragma omp parallel for
    for(int i = 0; i < n*m; i++)
        matRes[i] = (s0*341*i/m)%15;
}

int main(int argc, char *argv[])
{

    struct timeval start_time;
    struct timeval end_time;

    // fil = 1 -> Fil / fil = 0 -> Col
    int n = atoi(argv[1]), m = n, p = n, numThreads = atoi(argv[2]), fil = atoi(argv[3]);
    double total_time = 0;
    omp_set_num_threads(12);
    // Para sum
    double *matEx = malloc(sizeof(double)*n*m);
    fillMat(n, m, matEx, 1);
    double *matEx2 = malloc(sizeof(double)*n*m);
    fillMat(n, m, matEx2, 2);

    double *matRes = malloc(sizeof(double)*n*m);
    //int *matResTrue = malloc(sizeof(int)*n*m);

    // Para comprobacion
    //sumaMat0(matEx, matEx2, n, m, matResTrue);
    omp_set_num_threads(numThreads);
    // Medir tiempos
    gettimeofday(&start_time, NULL);
    sumaMatPar(matEx, matEx2, n, m, matRes);
    gettimeofday(&end_time, NULL);
    total_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec)/1000000.0;

    //if(!checkEquals(matRes, matResTrue, n, m))
    //    exit(-1);
    printf("%f;%d;%d;%d\n", total_time, n, numThreads, fil);

    free(matEx);free(matEx2);
    free(matRes);//free(matResTrue);

    return 0;
}
