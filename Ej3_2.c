#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include "mpi.h"

int indexCols(int i, int j, int n){ return(j*n+i); }

void printMatrix(int* M, int n, char origen){
    printf("%c - ", origen);
    for(int i = 0; i < n; i++)
	printf("%d ", M[i]);
    printf("\n");
}

void prodMat0(int* m1, int* m2, int *matRes, int n){
    int suma = 0;
    for (int k = 0; k < n/2; k++) {
        for (int i = 0; i < n/2; i++) {
            suma = 0;
            for (int j = 0; j < n; j++) {
                suma += m1[indexCols(i,j,n/2)] * m2[indexCols(j,k,n)];
            }
            matRes[indexCols(i,k,n/2)] = suma;
        }
    }
}

void fillMat(int n, int m, int *matRes, int s0){
    for(int i = 0; i < n*m; i++)
        matRes[i] = (s0*341*i/m)%15;
}

int main(int argc, char *argv[])
{
    //struct timeval start_time;
    //struct timeval end_time;
    // Iniciamos MPI
    int mpi_id, num_procs, valor = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int n = atoi(argv[1]);
    //double total_time = 0;

    //int *matResTrue = calloc(n*n, sizeof(int));

    // PRODUCTO EN PARALELO
    /*gettimeofday(&start_time, NULL);
    prodMatBloques(matEx, matEx2, matRes, n, NB);
    gettimeofday(&end_time, NULL);*/
    // COMPROBACION
    //if(!checkEquals(matRes, matResTrue, n, n))
    //    exit(-1);
    // RESULTADOS
    //total_time = (end_time.tv_sec - start_time.tv_sec) + 1e-6*(end_time.tv_usec - start_time.tv_usec);
    //printf("%f;%d;%d;%d\n", total_time, numThreads, n, NB);

    if(mpi_id == 0){
        // Creamos las matrices A y B
        int *A = calloc(n*n, sizeof(int));
        int *B = calloc(n*n, sizeof(int));
        int *C = calloc(n*n, sizeof(int));
        int *A2 = calloc(n*n/2, sizeof(int));
        fillMat(n, n, A, 3);
        fillMat(n, n, B, 4);
        // A es la estructura intermedia para A
        for(int i = 0; i <= 1; i++){ // Fila 1 o 2 (BLOQUE)
            for(int j = 0; j <= 1; j++){ // Columna 1 o 2 (BLOQUE)
                if(2*i+j != 0){ // Si no es el nodo 0
                    MPI_Send(&B[j*n*n/2], n*n/2, MPI_INT, 2*i+j, 0, MPI_COMM_WORLD);
                    for(int j2 = 0; j2 < n; j2++){ // n columnas
                        for(int i2 = 0; i2 < n/2; i2++){ // n/2 filas
                            A2[j2*n/2 + i2] = A[j2*n + i2 + n*i/2];
                        }
                    }
                    MPI_Send(A2, n*n/2, MPI_INT, 2*i+j, 0, MPI_COMM_WORLD);
                }
            }
        }
        // Multiplicacion de N0
        int suma = 0;
        for (int k = 0; k < n/2; k++) {
            for (int i = 0; i < n/2; i++) {
                suma = 0;
                for (int j = 0; j < n/2; j++) {
                    suma += A[i + n*j] * B[indexCols(j,k,n)];
                }
                C[i + k*n] = suma;
            }
        }
	printMatrix(C, n*n, 'o');
    }else{
        int *A = calloc(n*n/2, sizeof(int));
        int *B = calloc(n*n/2, sizeof(int));
        int *C = calloc(n*n/4, sizeof(int));
        MPI_Recv(B, n*n/2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(A, n*n/2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        prodMat0(A, B, C, n);
	printMatrix(C, n*n/4, 'C');
        //MPI_Send(&C, n*n/4, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}

