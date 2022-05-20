#include <iostream>
#include <fstream>
#include <omp.h>
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

void printMatrixRows(int* data, int n, int m)
{
    printf("\n");
	for(int j = 0; j < n; j++){
        for(int i = 0; i < m; i++)
            printf("%d ", data[rwise(j,i,m)]);
        printf("\n");
	}
}

void printMatrixCols(int* data, int n, int m)
{
    printf("\n");
	for(int j = 0; j < n; j++){
        for(int i = 0; i < m; i++)
            printf("%d ", data[cwise(j,i,n)]);
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

void sumMatrices(int* res, int* m1, int* m2, int n, int m)
{
    for(int i = 0; i < n*m; i++){
        res[i] = m1[i] + m2[i];
	}
}

void sumMatricesPar(int* res, int* m1, int* m2, int n, int m)
{
    #pragma omp parallel for
    for(int i = 0; i < n*m; i++){
        res[i] = m1[i] + m2[i];
	}
}

void multMatrices(int* res, int* m1, int* m2, int n, int nm, int m)
{
    for(int j = 0; j < n; j++){
        for(int i = 0; i < m; i++){
            res[j*n + i] = 0;
            for(int k = 0; k < nm; k++){
                res[j*n + i] = res[j*n + i] +  m1[j*n + k] * m2[k*n + i];
            }
        }
	}
}



void multMatricesPar(int* res, int* m1, int* m2, int n, int nm, int m)
{
    #pragma omp parallel for
    for(int j = 0; j < n; j++){
        for(int i = 0; i < m; i++){
            res[j*n + i] = 0;
            for(int k = 0; k < nm; k++){
                res[j*n + i] = res[j*n + i] +  m1[j*n + k] * m2[k*n + i];
            }
        }
	}
}


int main(int argc, char **argv)
{
    struct timeval start_time1;
    struct timeval end_time1;

    struct timeval start_time2;
    struct timeval end_time2;

    int numth=atoi(argv[1]);
  
    int n = 2000; int m = numth*2000; int nm = 2000;
    int *mat1 = (int*) malloc(sizeof(int)*n*m);
    fillMatrix(mat1,n,m);
    int *mat2 = (int*) malloc(sizeof(int)*n*m);
    fillMatrix(mat2,n,m);
    int *mat3 = (int*) malloc(sizeof(int)*n*nm);
    fillMatrix(mat3,n,nm);
    int *mat4 = (int*) malloc(sizeof(int)*n*nm);
    fillMatrix(mat4,n,nm);
    
    int *resSum = (int*) malloc(sizeof(int)*n*m);
    int *resSum2 = (int*) malloc(sizeof(int)*n*nm);
    //int *resMul = (int*) malloc(sizeof(int)*n*m);
    //int *resMul2 = (int*) malloc(sizeof(int)*n*m);

    // calculate time: SUM
    omp_set_num_threads(1);
    gettimeofday(&start_time1, NULL);
    sumMatricesPar(resSum2, mat3, mat4, n, nm);
    gettimeofday(&end_time1, NULL);
    // calculate time: SUM Parallel
    omp_set_num_threads(numth);
    gettimeofday(&start_time2, NULL);
    sumMatricesPar(resSum, mat1, mat2, n, m);
    gettimeofday(&end_time2, NULL);

    double total_time1 = (end_time1.tv_sec - start_time1.tv_sec) + (end_time1.tv_usec - start_time1.tv_usec)/1000000.0;
    double total_time2 = (end_time2.tv_sec - start_time2.tv_sec) + (end_time2.tv_usec - start_time2.tv_usec)/1000000.0;
    printf("%f;%f\n", total_time1,total_time2);


    return 0;
}
