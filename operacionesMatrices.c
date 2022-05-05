#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

// Devuelve el indice del vector de la matriz guardada por filas
int indexRows(int i, int j, int m){ return(i*m+j); }
// Devuelve el indice del vector de la matriz guardada por filas
int indexCols(int i, int j, int n){ return(j*n+i); }

void sumaMat0(int* m1, int* m2, int n, int m, int *matRes){
    int i;
    for(i = 0; i < n*m; i++){
        matRes[i] = m1[i] + m2[i];
    }
}

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

void sumaMatPar(int* m1, int* m2, int n, int m, int *matRes){
    #pragma omp parallel for
    for(int i = 0; i < n*m; i++){
        matRes[i] = m1[i] + m2[i];
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

int main()
{
    // GUARDADOS POR FILAS
    // RELLENAR MATRICES GRANDES
    struct timeval start_time;
    struct timeval end_time;

    int n = 1000, m = n, p = n, reps = 3, numThreads = 2;
    int nThreads[] = {2,3,4,5,6,7,8};

    // Para sum
    int *matEx = malloc(sizeof(int)*n*m);
    fillMat(n, m, matEx, 1);
    int *matEx2 = malloc(sizeof(int)*n*m);
    fillMat(n, m, matEx2, 2);

    int *matRes = malloc(sizeof(int)*n*m);
    int *matResTrue = malloc(sizeof(int)*n*m);

    sumaMat0(matEx, matEx2, n, m, matResTrue);
    sumaMatPar(matEx, matEx2, n, m, matRes);

    // Para mult
    int *matEx3 = malloc(sizeof(int)*n*m);
    fillMat(n, m, matEx3, 3);
    int *matEx4 = malloc(sizeof(int)*m*p);
    fillMat(m, p, matEx4, 4);

    int *matRes2 = malloc(sizeof(int)*n*p);
    int *matResTrue2Fil = malloc(sizeof(int)*n*p);
    int *matResTrue2Col = malloc(sizeof(int)*n*p);
    // 1. EN SERIE
    double *timesProd0 = malloc(sizeof(double)*reps), total_time = 0;
    for(int i = 0; i < reps; i++){
        gettimeofday(&start_time, NULL);
        prodMat0(matEx3, matEx4, n, m, p, matResTrue2Fil, 1);
        gettimeofday(&end_time, NULL);
        total_time = (end_time.tv_sec - start_time.tv_sec) + 1e-6*(end_time.tv_usec - start_time.tv_usec);
        timesProd0[i] = total_time;
    }

    printf("Filas\tColumnas\tnThreads\tDim\n");
    for(int i = 0; i < reps; i++){
        printf("%f\t%f\t1\t%d\n", timesProd0[i], timesProd0[i], n);
    }
    prodMat0(matEx3, matEx4, n, m, p, matResTrue2Col, 0);
    // 2. EN PARALELO CON DISTINTOS Threads
    double *timesProdParRows = malloc(sizeof(double)*reps);
    double *timesProdParCol = malloc(sizeof(double)*reps);
    for(int j = 0; j < numThreads; j++){

        omp_set_num_threads(nThreads[j]);
        // MULTIPLICACION DE MATRICES GUARDADAS POR FILAS
        for(int i = 0; i < reps; i++){
            gettimeofday(&start_time, NULL);
            prodMatPar(matEx3, matEx4, n, m, p, matRes2, 1);
            gettimeofday(&end_time, NULL);
            if(!checkEquals(matRes2, matResTrue2Fil, n, m))
                exit(-1);
            total_time = (end_time.tv_sec - start_time.tv_sec) + 1e-6*(end_time.tv_usec - start_time.tv_usec);
            timesProdParRows[i] = total_time;
        }

        // MULTIPLICACION DE MATRICES GUARDADAS POR COLUMNAS
        for(int i = 0; i < reps; i++){
            gettimeofday(&start_time, NULL);
            prodMatPar(matEx3, matEx4, n, m, p, matRes2, 0);
            gettimeofday(&end_time, NULL);
            if(!checkEquals(matRes2, matResTrue2Col, n, m))
                exit(-1);
            total_time = (end_time.tv_sec - start_time.tv_sec) + 1e-6*(end_time.tv_usec - start_time.tv_usec);
            timesProdParCol[i] = total_time;
        }

        for(int i = 0; i < reps; i++){
            printf("%f\t%f\t%d\t%d\n", timesProdParRows[i], timesProdParCol[i], nThreads[j], n);
        }
    }

    free(matEx);free(matEx2);free(matEx3);free(matEx4);
    free(matRes);free(matRes2);free(matResTrue);free(matResTrue2Fil);free(matResTrue2Col);
    free(timesProdParRows);free(timesProdParCol);

    return 0;
}
