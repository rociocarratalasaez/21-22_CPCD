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

void multMatColsPar_ijk(int* res, int* m1, int* m2, int n, int nm, int m)
{
    #pragma omp parallel for
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            for(int k = 0; k < nm; k++){
                res[cwise(i,j,n)] = res[cwise(i,j,n)] +  m1[cwise(i,k,n)] * m2[cwise(k,j,nm)];
            }
        }
	}
}

void multMatColsPar_ikj(int* res, int* m1, int* m2, int n, int nm, int m)
{
    #pragma omp parallel for
    for(int i = 0; i < n; i++){
        for(int k = 0; k < nm; k++){
            for(int j = 0; j < m; j++){
                res[cwise(i,j,n)] = res[cwise(i,j,n)] +  m1[cwise(i,k,n)] * m2[cwise(k,j,nm)];
            }
        }
	}
}

void multMatColsPar_jik(int* res, int* m1, int* m2, int n, int nm, int m)
{
    #pragma omp parallel for
    for(int j = 0; j < m; j++){
        for(int i = 0; i < n; i++){
            for(int k = 0; k < nm; k++){
                res[cwise(i,j,n)] = res[cwise(i,j,n)] +  m1[cwise(i,k,n)] * m2[cwise(k,j,nm)];
            }
        }
	}
}

void multMatColsPar_jki(int* res, int* m1, int* m2, int n, int nm, int m)
{
    #pragma omp parallel for
    for(int j = 0; j < m; j++){
        for(int k = 0; k < nm; k++){
            for(int i = 0; i < n; i++){
                res[cwise(i,j,n)] = res[cwise(i,j,n)] +  m1[cwise(i,k,n)] * m2[cwise(k,j,nm)];
            }
        }
	}
}

void multMatColsPar_kij(int* res, int* m1, int* m2, int n, int nm, int m)
{
    #pragma omp parallel for
    for(int k = 0; k < nm; k++){
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                res[cwise(i,j,n)] = res[cwise(i,j,n)] +  m1[cwise(i,k,n)] * m2[cwise(k,j,nm)];
            }
        }
	}
}

void multMatColsPar_kji(int* res, int* m1, int* m2, int n, int nm, int m)
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

void multMatColsPar_kji_block(int* res, int* m1, int* m2, int n, int nm, int m, int nBlock, int p)
{
    #pragma omp parallel for
    for(int alpha = 0; alpha < nBlock; alpha++){
        int u = alpha*p;
        for(int beta = 0; beta < nBlock; beta++){
            int v = beta*p;
            for(int gamma = 0; gamma < nBlock; gamma++){
                int w = gamma*p;
                for(int k = w; k < w+p; k++){
                    for(int j = v; j < v+p; j++){
                        for(int i = u; i < u+p; i++){
                            res[cwise(i,j,n)] = res[cwise(i,j,n)] +  m1[cwise(i,k,n)] * m2[cwise(k,j,nm)];
                        }
                    }
                }
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

    struct timeval start_time3;
    struct timeval end_time3;

    struct timeval start_time4;
    struct timeval end_time4;

    struct timeval start_time5;
    struct timeval end_time5;

    struct timeval start_time6;
    struct timeval end_time6;

    int numth=atoi(argv[1]);

    int n = 4; int m = 4; int nm = 4;
    int *mat1 = (int*) malloc(sizeof(int)*n*nm);
    fillMatrix(mat1,n,nm);
    int *mat2 = (int*) malloc(sizeof(int)*nm*m);
    fillMatrix(mat2,nm,m);

    printMatrixCols(mat1,n,nm);
    printMatrixCols(mat2,nm,m);

    int *resMul = (int*) malloc(sizeof(int)*n*m);
    fill(resMul, resMul+n*m, 0);
    int *resMul2 = (int*) malloc(sizeof(int)*n*m);
    fill(resMul2, resMul2+n*m, 0);
    int *resMul3 = (int*) malloc(sizeof(int)*n*m);
    fill(resMul3, resMul3+n*m, 0);
    int *resMul4 = (int*) malloc(sizeof(int)*n*m);
    fill(resMul4, resMul4+n*m, 0);
    int *resMul5 = (int*) malloc(sizeof(int)*n*m);
    fill(resMul5, resMul5+n*m, 0);
    int *resMul6 = (int*) malloc(sizeof(int)*n*m);
    fill(resMul6, resMul6+n*m, 0);

    multMatColsPar_kji_block(resMul, mat1, mat2, n, nm, m, 2, 2);
    printMatrixCols(resMul,n,m);


    /*
    omp_set_num_threads(1);
    // calculate time: PRODUCT ijk
    gettimeofday(&start_time1, NULL);
    multMatColsPar_ijk(resMul, mat1, mat2, n, nm, m);
    gettimeofday(&end_time1, NULL);
    // calculate time: PRODUCT ikj
    gettimeofday(&start_time2, NULL);
    multMatColsPar_ikj(resMul2, mat1, mat2, n, nm, m);
    gettimeofday(&end_time2, NULL);
    // calculate time: PRODUCT jik
    gettimeofday(&start_time3, NULL);
    multMatColsPar_jik(resMul3, mat1, mat2, n, nm, m);
    gettimeofday(&end_time3, NULL);
    // calculate time: PRODUCT jki
    gettimeofday(&start_time4, NULL);
    multMatColsPar_jki(resMul4, mat1, mat2, n, nm, m);
    gettimeofday(&end_time4, NULL);
    // calculate time: PRODUCT kij
    gettimeofday(&start_time5, NULL);
    multMatColsPar_kij(resMul5, mat1, mat2, n, nm, m);
    gettimeofday(&end_time5, NULL);
    // calculate time: PRODUCT kji
    gettimeofday(&start_time6, NULL);
    multMatColsPar_kji(resMul6, mat1, mat2, n, nm, m);
    gettimeofday(&end_time6, NULL);


    double total_time1 = (end_time1.tv_sec - start_time1.tv_sec) + (end_time1.tv_usec - start_time1.tv_usec)/1000000.0;
    double total_time2 = (end_time2.tv_sec - start_time2.tv_sec) + (end_time2.tv_usec - start_time2.tv_usec)/1000000.0;
    double total_time3 = (end_time3.tv_sec - start_time3.tv_sec) + (end_time3.tv_usec - start_time3.tv_usec)/1000000.0;
    double total_time4 = (end_time4.tv_sec - start_time4.tv_sec) + (end_time4.tv_usec - start_time4.tv_usec)/1000000.0;
    double total_time5 = (end_time5.tv_sec - start_time5.tv_sec) + (end_time5.tv_usec - start_time5.tv_usec)/1000000.0;
    double total_time6 = (end_time6.tv_sec - start_time6.tv_sec) + (end_time6.tv_usec - start_time6.tv_usec)/1000000.0;
    printf("%f;%f;%f;%f;%f;%f\n", total_time1,total_time2,total_time3,total_time4,total_time5,total_time6);

    */

    return 0;
}
