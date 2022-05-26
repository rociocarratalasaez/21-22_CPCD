#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

// Devuelve el indice del vector de la matriz guardada por filas
int indexCols(int i, int j, int n){ return(j*n+i); }

void prodMat0(int* m1, int* m2, int *matRes, int n){
    int suma = 0;
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            suma = 0;
            for (int j = 0; j < n; j++) {
                suma += m1[indexCols(i,j,n)] * m2[indexCols(j,k,n)];
            }
            matRes[indexCols(i,k,n)] = suma;
        }
    }
}

void prodMatBloques(int* m1, int* m2, int *matRes, int n, int NB){
    int i1, i2, j1, j2, k1, k2, p = n/NB;
    #pragma omp parallel for private(i1, i2, j1, j2, k1, k2)
    for (int alpha = 1; alpha <= NB; alpha++){
        i1 = (alpha - 1) * p;
        i2 = alpha * p;
        for (int beta = 1; beta <= NB; beta++){
            j1 = (beta - 1) * p;
            j2 = beta * p;
            for (int gamma = 1; gamma <= NB; gamma++){
                k1 = (gamma - 1) * p;
                k2 = gamma * p;
                for (int j = j1; j < j2; j++) {
                    for (int k = k1; k < k2; k++) {
                        for (int i = i1; i < i2; i++) {
                            matRes[indexCols(i,j,n)] = matRes[indexCols(i,j,n)] + m1[indexCols(i,k,n)]*m2[indexCols(k,j,n)];
                        }
                    }
                }
            }
        }
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

    int n = atoi(argv[1]), numThreads = atoi(argv[2]), NB = atoi(argv[3]);
    double total_time = 0;
    int *matEx = calloc(n*n, sizeof(int));
    int *matEx2 = calloc(n*n, sizeof(int));
    fillMat(n, n, matEx, 3);
    fillMat(n, n, matEx2, 4);

    int *matRes = calloc(n*n, sizeof(int));
    //int *matResTrue = calloc(n*n, sizeof(int));

    // PRODUCTO EN SERIE
    //prodMat0(matEx, matEx2, matResTrue, n);
    // PRODUCTO EN PARALELO
    gettimeofday(&start_time, NULL);
    prodMatBloques(matEx, matEx2, matRes, n, NB);
    gettimeofday(&end_time, NULL);
    // COMPROBACION
    //if(!checkEquals(matRes, matResTrue, n, n))
    //    exit(-1);
    // RESULTADOS
    total_time = (end_time.tv_sec - start_time.tv_sec) + 1e-6*(end_time.tv_usec - start_time.tv_usec);
    printf("%f;%d;%d;%d\n", total_time, numThreads, n, NB);

    free(matEx);free(matEx2);
    free(matRes);//free(matResTrue);

    return 0;
}
