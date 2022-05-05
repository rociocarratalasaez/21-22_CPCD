#include <iostream>
#include <fstream>
#include <omp.h>
#include <sys/time.h>

#include "matrix.h"

using namespace std;

// Save results in a text file

void saveResults(string res, string file)
{
    std::ofstream fs;
    fs.open(file, fstream::app);
    fs << res << "\n";
    fs.close();
}


int main(int argc, char** argv)
{

    struct timeval start_time1;
    struct timeval end_time1;

    struct timeval start_time2;
    struct timeval end_time2;
/*
    struct timeval start_time3;
    struct timeval end_time3;

    struct timeval start_time4;
    struct timeval end_time4;
*/

    int n1 = 1000; int m1 = 1000;
    int n2 = 1000; int m2 = 1000;

    Matrix A(n1,m1,0);
    A.fillMatrix();

    Matrix B(n2,m2,0);
    B.fillMatrix();

    // calculate time: SUM
    gettimeofday(&start_time1, NULL);
    A.sum(B);
    gettimeofday(&end_time1, NULL);
    // calculate time: SUM Parallel
    gettimeofday(&start_time2, NULL);
    A.sumPar(B);
    gettimeofday(&end_time2, NULL);

    double total_time1 = (end_time1.tv_sec - start_time1.tv_sec) + (end_time1.tv_usec - start_time1.tv_usec)/1000000.0;
    double total_time2 = (end_time2.tv_sec - start_time2.tv_sec) + (end_time2.tv_usec - start_time2.tv_usec)/1000000.0;
    printf("%f;%f\n", total_time1,total_time2);

/*

    // calculate time: MULT
    gettimeofday(&start_time3, NULL);
    A.prod(B);
    gettimeofday(&end_time3, NULL);
    // calculate time: MULT Parallel
    gettimeofday(&start_time4, NULL);
    A.prodPar(B);
    gettimeofday(&end_time4, NULL);

    double total_time3 = (end_time3.tv_sec - start_time3.tv_sec) + (end_time3.tv_usec - start_time3.tv_usec)/1000000.0;
    double total_time4 = (end_time4.tv_sec - start_time4.tv_sec) + (end_time4.tv_usec - start_time4.tv_usec)/1000000.0;
    printf("%f;%f\n", total_time3,total_time4);

*/

    return 0;
}
