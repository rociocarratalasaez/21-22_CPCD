#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

// Devuelve el indice del vector de la matriz guardada por filas
int indexRows(int i, int j, int m){ return(i*m+j); }
// Devuelve el indice del vector de la matriz guardada por filas
int indexCols(int i, int j, int n){ return(j*n+i); }

void prodMat0(int* m1, int* m2, int n, int m, int p, int *matRes, int byRows){
    int suma = 0;
    if(byRows == 1){
        for (int a = 0; a < p; a++) {
            for (int i = 0; i < n; i++) {
                suma = 0;
                for (int j = 0; j < m; j++) {
                    suma += m1[indexRows(i,j,m)] * m2[indexRows(j,a,p)];
                }
                matRes[indexRows(i,a,p)] = suma;
            }
        }
    }else{
        for (int a = 0; a < p; a++) {
            for (int i = 0; i < n; i++) {
                suma = 0;
                for (int j = 0; j < m; j++) {
                    suma += m1[indexCols(i,j,n)] * m2[indexCols(j,a,m)];
                }
                matRes[indexCols(i,a,m)] = suma;
            }
        }
    }
}

void prodMatPar(int* m1, int* m2, int n, int m, int p, int *matRes, int byRows){
    int suma = 0;
    if(byRows == 1){
        #pragma omp parallel for private(suma)
        for (int a = 0; a < p; a++) {
            for (int i = 0; i < n; i++) {
                suma = 0;
                for (int j = 0; j < m; j++) {
                    suma += m1[indexRows(i,j,m)] * m2[indexRows(j,a,p)];
                }
                matRes[indexRows(i,a,p)] = suma;
            }
        }
    }else{
        #pragma omp parallel for private(suma)
        for (int a = 0; a < p; a++) {
            for (int i = 0; i < n; i++) {
                suma = 0;
                for (int j = 0; j < m; j++) {
                    suma += m1[indexCols(i,j,n)] * m2[indexCols(j,a,m)];
                }
                matRes[indexCols(i,a,m)] = suma;
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

    // fil = 1 -> Fil / fil = 0 -> Col
    int n = atoi(argv[1]), m = n, p = n, numThreads = atoi(argv[2]), fil = atoi(argv[3]);
    double total_time = 0;
    int *matEx = malloc(sizeof(int)*n*m);
    int *matEx2 = malloc(sizeof(int)*m*p);
    fillMat(n, m, matEx, 3);
    fillMat(m, p, matEx2, 4);

    int *matRes = malloc(sizeof(int)*n*p);
    //int *matResTrue = malloc(sizeof(int)*n*p);

    //prodMat0(matEx, matEx2, n, m, p, matResTrue, fil);
    gettimeofday(&start_time, NULL);
    prodMatPar(matEx, matEx2, n, m, p, matRes, fil);
    gettimeofday(&end_time, NULL);
    //if(!checkEquals(matRes, matResTrue, n, m))
    //    exit(-1);
    total_time = (end_time.tv_sec - start_time.tv_sec) + 1e-6*(end_time.tv_usec - start_time.tv_usec);
    printf("%f;%d;%d;%d\n", total_time, numThreads, n, fil);

    free(matEx);free(matEx2);
    free(matRes);
    //free(matResTrue);

    return 0;
}
