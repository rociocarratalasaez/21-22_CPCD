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
	for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++)
            printf("%d ", data[rwise(i,j,m)]);
        printf("\n");
	}
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

void multMatricesRows(int* res, int* m1, int* m2, int n, int nm, int m)
{
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            res[rwise(i,j,m)] = 0;
            for(int k = 0; k < nm; k++){
                res[rwise(i,j,m)] = res[rwise(i,j,m)] +  m1[rwise(i,k,nm)] * m2[rwise(k,j,m)];
            }
        }
	}
}

void multMatricesCols(int* res, int* m1, int* m2, int n, int nm, int m)
{
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            res[cwise(i,j,n)] = 0;
            for(int k = 0; k < nm; k++){
                printf("i: %d, j: %d \n",i,j);
                res[cwise(i,j,n)] = res[cwise(i,j,n)] +  m1[cwise(i,k,n)] * m2[cwise(k,j,nm)];
                printf("i,k: %d; k,j: %d \n",m1[cwise(i,k,n)],m2[cwise(k,j,nm)]);
            }
        }
	}
}


void multMatricesRowsPar(int* res, int* m1, int* m2, int n, int nm, int m)
{
    #pragma omp parallel for
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            res[rwise(i,j,m)] = 0;
            for(int k = 0; k < nm; k++){
                res[rwise(i,j,m)] = res[rwise(i,j,m)] +  m1[rwise(i,k,nm)] * m2[rwise(k,j,m)];
            }
        }
	}
}

void multMatricesColsPar(int* res, int* m1, int* m2, int n, int nm, int m)
{
    #pragma omp parallel for
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            res[cwise(i,j,n)] = 0;
            for(int k = 0; k < nm; k++){
                res[cwise(i,j,n)] = res[cwise(i,j,n)] +  m1[cwise(i,k,n)] * m2[cwise(k,j,nm)];
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

    int n1 = 500; int m1 = 500; int nm1 = 500;
    int *mat1 = (int*) malloc(sizeof(int)*n1*nm1);
    fillMatrix(mat1,n1,nm1);
    int *mat2 = (int*) malloc(sizeof(int)*nm1*m1);
    fillMatrix(mat2,nm1,m1);
    int n2 = numth* 500; int m2 = 500; int nm2 = 500;
    int *mat3 = (int*) malloc(sizeof(int)*n2*nm2);
    fillMatrix(mat3,n2,nm2);
    int *mat4 = (int*) malloc(sizeof(int)*nm2*m2);
    fillMatrix(mat4,nm2,m2);
    
    int *resMul = (int*) malloc(sizeof(int)*n1*m1);
    int *resMul2 = (int*) malloc(sizeof(int)*n2*m2);


    // calculate time: SUM
    omp_set_num_threads(1);
    gettimeofday(&start_time1, NULL);
    multMatricesRowsPar(resMul, mat1, mat2, n1, nm1, m1);
    gettimeofday(&end_time1, NULL);
    // calculate time: SUM Parallel
    omp_set_num_threads(numth);
    gettimeofday(&start_time2, NULL);
    multMatricesRowsPar(resMul2, mat3, mat4, n2, nm2, m2);
    gettimeofday(&end_time2, NULL);

    double total_time1 = (end_time1.tv_sec - start_time1.tv_sec) + (end_time1.tv_usec - start_time1.tv_usec)/1000000.0;
    double total_time2 = (end_time2.tv_sec - start_time2.tv_sec) + (end_time2.tv_usec - start_time2.tv_usec)/1000000.0;
    printf("%f;%f\n", total_time1,total_time2);



    return 0;
}
