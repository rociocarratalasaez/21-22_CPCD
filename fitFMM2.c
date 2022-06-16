#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

double PI = acos(-1.0);

// Funciones auxiliares
double mean(double *vec, int n);
void covMatrix(double *v1, double *v2, double *v3, int n, double *cv12,
               double *cv13, double *cv23, double *cv22, double *cv33);
void seq(double* vec, double start, double end, int size);
double mod(double a, double b);

// Funciones para la estimacion de parametros
void step1FMM(double* vData, double* timePoints, int n, double alpha,
              double omega, double* params);
void fitFMM(double* vData, double* timePoints, int n, double* seqAlphas,
            int nAlpha, double* seqOmegas, int nOmega, double* optParams);


int main(int argc, char *argv[]){


    struct timeval start_time;
    struct timeval end_time;
    // Definimos tamaño del grid de busqueda {omega(i)}x{alpha(j)}
    // i = 1,...,nOmega; j = 1,...,nAlpha
    //int nOmega = 300, nAlpha = 300, n, numThreads = 1;
    int nOmega = atoi(argv[1]), nAlpha = atoi(argv[2]), 
	numThreads = atoi(argv[4]), n;
    double aux, total_time = 0;
    char* datafile = argv[3];
    //char* datafile = "exampleSignal2.txt";
    double *seqAlphas = malloc(sizeof(double)*nAlpha);
    double *seqOmegas = malloc(sizeof(double)*nOmega);

    // Lectura de la senal indicada como argumento:
    FILE *file;
    if ((file = fopen(datafile, "r")) == NULL){
        printf("File error.");
        return(-1);
    }
    fscanf(file, "%d", &n);
    double *vData = malloc(sizeof(double)*n);
    for(int i = 0; i < n; i++){
        fscanf(file, "%lf", &vData[i]);
    }
    fclose(file);

    // Construimos el grid para alpha y omega (en alpha
    // lineal, en omega logaritmico, se necesita mejor
    // precision en valores bajos)
    seq(seqAlphas, 1.0/nAlpha, 2*PI, nAlpha);
    seq(seqOmegas, log(1.0/nOmega), log(1), nOmega);
    for(int i = 0; i < nOmega; i++){
        seqOmegas[i] = exp(seqOmegas[i]);
    }

    // Vector de tiempos
    double *timeSeq = malloc(sizeof(double)*n);
    seq(timeSeq, 0, 2*PI, n);
    double *params = malloc(sizeof(double)*6);


    /* ESTIMACION DE LOS PARAMETROS OPTIMOS */
    gettimeofday(&start_time, NULL);
    fitFMM(vData, timeSeq, n, seqAlphas, nAlpha, seqOmegas, nOmega, params);
    gettimeofday(&end_time, NULL);

    total_time = (end_time.tv_sec - start_time.tv_sec) + 1e-6*(end_time.tv_usec - start_time.tv_usec);
    printf("%f;%d;%d;%d\n", total_time, numThreads, nOmega, nAlpha);

    /*printf("\nM = %f, A = %f, alpha = %f, beta = %f, omega = %f, RSS = %f",
           params[0], params[1], params[2], params[3], params[4], params[5]);*/
    free(seqAlphas); free(seqOmegas);
    free(vData); free(timeSeq);
    free(params);
    return(0);
}

void seq(double* vec, double start, double end, int size){
    double diff = (end-start)/(size);
    for(int i = 0; i < size; i++){
        vec[i] = start + i*diff;
    }
}

double mean(double *vec, int n){
   double sum = 0;
   for(int i = 0; i < n; i++) {
      sum += vec[i];
   }
   return(sum/n);
}

double mod(double a, double b){
    return(a-b*floor(a/b));
}

void covMatrix(double *v1, double *v2, double *v3, int n, double *cv12,
               double *cv13, double *cv23, double *cv22, double *cv33){
    double mean1 = mean(v1, n), mean2 = mean(v2, n), mean3 = mean(v3, n);
    double sum12 = 0, sum13 = 0, sum23 = 0, sum22 = 0, sum33 = 0;
    for(int i = 0; i < n; i++){
        sum12 += v1[i]*v2[i];
        sum13 += v1[i]*v3[i];
        sum23 += v2[i]*v3[i];
        sum22 += (v2[i]-mean2)*(v2[i]-mean2);
        sum33 += (v3[i]-mean3)*(v3[i]-mean3);
    }
    *cv12 = sum12/(n-1) - mean1*mean2;
    *cv13 = sum13/(n-1) - mean1*mean3;
    *cv23 = sum23/(n-1) - mean2*mean3;
    *cv22 = sum22/(n-1);
    *cv33 = sum33/(n-1);
}

void fitFMM(double* vData, double* timePoints, int n, double* seqAlphas,
            int nAlpha, double* seqOmegas, int nOmega, double* optParams){
    double *params = malloc(sizeof(double)*6); //A, M, alpha, beta, omega, RSS
    double *RSSs = malloc(sizeof(double)*nAlpha*nOmega);
    double minRSS = 9e9;
    int minIndex = 0;
    double tStar[n];
    double costStar[n];
    double sintStar[n];
    // No es necesario variables privadas
    #pragma omp parallel for private(tStar, costStar, sintStar)
    for(int i = 0; i < nOmega; i++){
        for(int j = 0; j < nAlpha; j++){
            step1FMM(vData, timePoints, n, seqAlphas[j], seqOmegas[i], params, 
		     tStar, costStar, sintStar);
            RSSs[i*nAlpha + j] = params[5];
        }
    }

    // Busqueda de los mejores parametros
    for(int i = 0; i < nOmega*nAlpha; i++){
        if(minRSS > RSSs[i]){
            minIndex = i;
            minRSS = RSSs[i];
        }
    }
    step1FMM(vData, timePoints, n, seqAlphas[minIndex%nAlpha],
             seqOmegas[minIndex/nAlpha], optParams, tStar, costStar, sintStar);
}

void step1FMM(double* vData, double* timePoints, int n, double alpha, double omega,
              double* params, double* tStar, double* costStar, double* sintStar){
    // NOTA: Quizas para optimizar, no declarar los tres punteros en cada llamada
    // y reutilizar los vectores
    //double *tStar = malloc(sizeof(double)*n);
    //double *costStar = malloc(sizeof(double)*n);
    //double *sintStar = malloc(sizeof(double)*n);
    double cv12, cv13, cv23, cv22, cv33;
    for(int i = 0; i < n; i++){
        tStar[i] = alpha + 2*atan(omega*tan((timePoints[i] - alpha)/2));
        costStar[i] = cos(tStar[i]);
        sintStar[i] = sin(tStar[i]);
    }

    covMatrix(vData, costStar, sintStar, n, &cv12, &cv13, &cv23, &cv22, &cv33);
    double denominator = cv22*cv33 - cv23*cv23;
    double cosCoeff = (cv12*cv33 - cv13*cv23)/denominator;
    double sinCoeff = (cv13*cv22 - cv12*cv23)/denominator;
    params[0] = mean(vData, n) - cosCoeff*mean(costStar, n) - sinCoeff*mean(sintStar, n);

    double phiEst = atan2(-sinCoeff, cosCoeff);
    params[1] = sqrt(cosCoeff*cosCoeff + sinCoeff*sinCoeff);
    params[2] = alpha;
    params[3] = mod(phiEst + alpha, 2*PI);
    params[4] = omega;
    params[5] = 0;
    for(int i = 0; i < n; i++){
        params[5] += pow(vData[i] - (params[0] + params[1]*cos(params[3] + tStar[i] - params[2])), 2);
    }
    params[5] = params[5]/n;
}


