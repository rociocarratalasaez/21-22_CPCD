#include <iostream>
#include <fstream>
#include <omp.h>
#include "mpi.h"
#include <sys/time.h>


using namespace std;

// Functions to print matrix saved row/column-wise,
// fill matrix with random numbers in (0,99)

int rwise(int i, int j, int m)
{
    return i*m + j;
}

int cwise(int i, int j, int n)
{
    return i + j*n;
}

void printMatrixCols(int* data, int n, int m)
{
    printf("\n");
	for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++)
            printf("%d ", data[cwise(i,j,n)]);
        printf("\n");
	}
}

void fillMatrix(int* data, int n, int m)
{
    for(int j = 0; j < n; j++){
        for(int i = 0; i < m; i++)
            data[j*m + i] = rand() % 100;
	}
}

// Transform matrix == transforms part of a matrix into smaller one
// columns
void transMatrixCols(int* data, int* dataNew, int n, int m, int k2)
{
    for(int j = 0; j < m/2; j++){
        for(int i = 0; i < n; i++)
            dataNew[cwise(i,j,n)] = data[cwise(i,j+k2*(m/2),n)]; //i + j*n;
	}
}
// rows
void transMatrixRows(int* data, int* dataNew, int n, int m, int k1)
{
    for(int j = 0; j < m; j++){
        for(int i = 0; i < n/2; i++)
            dataNew[cwise(i,j,n/2)] = data[cwise(i+k1*(n/2),j,n)]; //i + j*n;
	}
}
// collect matrix = colocates smaller matrix into bigger one
void collectMatrix(int* data, int* dataNew, int n, int m, int k1, int k2)
{
    for(int j = 0; j < m/2; j++){
        for(int i = 0; i < n/2; i++)
            data[cwise(i+k1*(n/2),j+k2*(m/2),n)] = dataNew[cwise(i,j,n/2)]; //i + j*n;
	}
}
// Save results in a text file

void saveResults(string res, string file)
{
    std::ofstream fs;
    fs.open(file, fstream::app);
    fs << res << "\n";
    fs.close();
}

// Sequential codes without parallel optimization,
// sum and product of matrices

void multMat(int* res, int* m1, int* m2, int n, int nm, int m)
{
    #pragma omp parallel for
    for(int k = 0; k < nm; k++){
        for(int j = 0; j < m; j++){
            for(int i = 0; i < n; i++){
                res[cwise(i,j,n)] = res[cwise(i,j,n)] +  m1[cwise(i,k,n)] * m2[cwise(k,j,nm)];
            }
        }
	}
}

int main(int argc, char** argv)
{
    struct timeval start_time1; struct timeval end_time1;
    int n = 1500; int m = 1500; int nm = 1500;

    // ---------------------------------------------------------
    // sequential code

    int *mat1 = (int*) malloc(sizeof(int)*n*nm);
    fillMatrix(mat1,n,nm);
    int *mat2 = (int*) malloc(sizeof(int)*nm*m);
    fillMatrix(mat2,nm,m);

    int *resMul = (int*) malloc(sizeof(int)*n*m);
    fill(resMul, resMul+n*m, 0);

    omp_set_num_threads(1);

    gettimeofday(&start_time1, NULL);
    multMat(resMul, mat1, mat2, n/2, nm, m/2);
    gettimeofday(&end_time1, NULL);

    double total_time1 = (end_time1.tv_sec - start_time1.tv_sec) + (end_time1.tv_usec - start_time1.tv_usec)/1000000.0;
    printf("%f\n", total_time1);

    // ---------------------------------------------------------
    // OpenMP

    int *mat1 = (int*) malloc(sizeof(int)*n*nm);
    fillMatrix(mat1,n,nm);
    int *mat2 = (int*) malloc(sizeof(int)*nm*m);
    fillMatrix(mat2,nm,m);

    int *resMul = (int*) malloc(sizeof(int)*n*m);
    fill(resMul, resMul+n*m, 0);

    int numth=atoi(argv[1]);
    omp_set_num_threads(numth);

    gettimeofday(&start_time1, NULL);
    multMat(resMul, mat1, mat2, n/2, nm, m/2);
    gettimeofday(&end_time1, NULL);

    double total_time1 = (end_time1.tv_sec - start_time1.tv_sec) + (end_time1.tv_usec - start_time1.tv_usec)/1000000.0;
    printf("%f\n", total_time1);

    // ---------------------------------------------------------
    // MPI

    // basic settings
	int mpi_id, num_procs;
	MPI_Init(NULL, NULL);

	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);

	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    //int numth=atoi(argv[1]);
    int numth = 1;
    omp_set_num_threads(numth);

    int half = num_procs/2;

    //process 0
    if(mpi_id == 0){
        //create matrices, fill with rnd numbers
        int *mat1 = (int*) malloc(sizeof(int)*n*nm);
        fillMatrix(mat1,n,nm);
        int *mat2 = (int*) malloc(sizeof(int)*nm*m);
        fillMatrix(mat2,nm,m);

        int *resMul = (int*) malloc(sizeof(int)*n*m);
        fill(resMul, resMul+n*m, 0);

        gettimeofday(&start_time1, NULL);
        //send corresponding parts of matrices to every process 1-3
        for(int i = 1; i < num_procs; i++){
            int a = i/half; int b = i % half;
            int *auxMat1 = (int*) malloc(sizeof(int)*(n/2)*nm);
            int *auxMat2 = (int*) malloc(sizeof(int)*nm*(m/2));
            transMatrixRows(mat1, auxMat1, n, nm, a);
            transMatrixCols(mat2, auxMat2, nm, m, b);
            MPI_Send(auxMat1,(n/2)*nm,MPI_INT,i,0,MPI_COMM_WORLD);
            MPI_Send(auxMat2,nm*(m/2),MPI_INT,i,1,MPI_COMM_WORLD);
            //printf("Soy el proceso %d y mando las matrices a %d. \n",mpi_id,i);
            //fflush(stdout);
        }
        //process 0 calculates its own part of matrix multiplication
        int *auxMat1 = (int*) malloc(sizeof(int)*(n/2)*nm);
        int *auxMat2 = (int*) malloc(sizeof(int)*nm*(m/2));
        transMatrixRows(mat1, auxMat1, n, nm, 0);
        transMatrixCols(mat2, auxMat2, nm, m, 0);
        int *resMul_11 = (int*) malloc(sizeof(int)*(n/2)*(m/2));
        fill(resMul_11, resMul_11+(n/2)*(m/2), 0);
        multMat(resMul_11, auxMat1, auxMat2, n/2, nm, m/2);
        collectMatrix(resMul, resMul_11, n, m, 0, 0);

        //recive parts of product matrix from 1-3
        for(int i = 1; i < num_procs; i++){
            int a = i/half; int b = i % half;
            int *resMul_i = (int*) malloc(sizeof(int)*(n/2)*(m/2));
            MPI_Recv(resMul_i,(n/2)*(m/2),MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            collectMatrix(resMul, resMul_i, n, m, a, b);
            //printf("Soy el proceso %d y recibo la matriz de %d. \n",mpi_id,i);
            //fflush(stdout);
        }

    //process 1-3
    //recive parts, multiplicate, send
	} else if (mpi_id > 0 && mpi_id < num_procs){
        int *auxMat1i = (int*) malloc(sizeof(int)*(n/2)*nm);
        int *auxMat2i = (int*) malloc(sizeof(int)*nm*(m/2));
        MPI_Recv(auxMat1i,(n/2)*nm,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(auxMat2i,nm*(m/2),MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        //printf("Soy el proceso %d y recibo las matrices de 0. \n",mpi_id);
        int *resMul_i = (int*) malloc(sizeof(int)*(n/2)*(m/2));
        fill(resMul_i, resMul_i+(n/2)*(m/2), 0);
        multMat(resMul_i, auxMat1i, auxMat2i, n/2, nm, m/2);
        MPI_Send(resMul_i,(n/2)*(m/2),MPI_INT,0,0,MPI_COMM_WORLD);
        //printf("Soy el proceso %d y mando la matriz a 0. \n",mpi_id);
        //fflush(stdout);
	}

	gettimeofday(&end_time1, NULL);

    double total_time1 = (end_time1.tv_sec - start_time1.tv_sec) + (end_time1.tv_usec - start_time1.tv_usec)/1000000.0;
    printf("%f\n", total_time1);

	MPI_Finalize();


    return 0;
}

