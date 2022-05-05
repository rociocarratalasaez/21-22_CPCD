#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

void sumaMat0(int* m1, int* m2, int n, int m, int *matRes){
    for(int i = 0; i < n*m; i++){
        matRes[i] = m1[i] + m2[i];
    }
}

void sumaMatPar(int* m1, int* m2, int n, int m, int *matRes){
    #pragma omp parallel for
    for(int i = 0; i < n*m; i++){
        matRes[i] = m1[i] + m2[i];
    }
}

int checkEquals(int* m1, int* m2, int n, int m){
    for(int i = 0; i < n*m; i++)
        if(m1[i] != m2[i])
            return 0;
    return 1;
}

void fillMat(int n, int m, int *matRes, int s0){
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

    // Para sum
    int *matEx = malloc(sizeof(int)*n*m);
    fillMat(n, m, matEx, 1);
    int *matEx2 = malloc(sizeof(int)*n*m);
    fillMat(n, m, matEx2, 2);

    int *matRes = malloc(sizeof(int)*n*m);
    int *matResTrue = malloc(sizeof(int)*n*m);

    // Para comprobacion
    //sumaMat0(matEx, matEx2, n, m, matResTrue);

    // Medir tiempos
    gettimeofday(&start_time, NULL);
    sumaMatPar(matEx, matEx2, n, m, matRes);
    gettimeofday(&end_time, NULL);
    total_time = (end_time.tv_sec - start_time.tv_sec) + 1e-6*(end_time.tv_usec - start_time.tv_usec);

    //if(!checkEquals(matRes, matResTrue, n, m))
    //    exit(-1);
    printf("%f;%d;%d;%d\n", total_time, n, numThreads, fil);

    free(matEx);free(matEx2);
    free(matRes);free(matResTrue);

    return 0;
}
