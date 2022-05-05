#include <iostream>
#include <fstream>
#include <omp.h>
#include <math.h>
#include "matrix.h"

using namespace std;


Matrix::Matrix(int w)
{
    wise = w;
    nrows = 0;
    ncols = 0;
    m = new float[nrows*ncols];
}

Matrix::Matrix(int nr, int nc, int w)
{
    wise = w;
    nrows = nr;
    ncols = nc;
    m = new float[nrows*ncols];
}

Matrix::Matrix(const Matrix& mat)
{
    wise = mat.wise;
    nrows = mat.nrows;
    ncols = mat.ncols;
    m = new float[nrows*ncols];

    for(int i = 0; i < nrows*ncols; i++){
        m[i] = mat.m[i];
    }
}

Matrix::~Matrix()
{
    delete m;
}

float Matrix::getElement(int i, int j){
    if(wise == 0){
        return m[i*ncols + j];
    }
    else{
        return m[i + j*nrows];
    }
}

void Matrix::setElement(int i, int j, float element){
    if(wise == 0){
        m[i*ncols + j] = element;
    }
    else{
        m[i + j*nrows] = element;
    }
}


Matrix Matrix::sum(const Matrix &mat){
    int nr1 = nrows; int nr2 = mat.nrows;
    int nc1 = ncols; int nc2 = mat.ncols;
    try{
        if(nc1 != nc2 | nr1 != nr2){
            throw rowsColsExc();
        }
    }
    catch(rowsColsExc& e){
        cerr << e.what() << endl;
        return 1;
    }

    Matrix res(nr1,nc1,mat.wise);
    for(int i = 0; i < res.nrows*res.ncols; i++){
        res.m[i] = m[i] + mat.m[i];
    }
    return res;
}

Matrix Matrix::sumPar(const Matrix &mat){
    int nr1 = nrows; int nr2 = mat.nrows;
    int nc1 = ncols; int nc2 = mat.ncols;
    try{
        if(nc1 != nc2 | nr1 != nr2){
            throw rowsColsExc();
        }
    }
    catch(rowsColsExc& e){
        cerr << e.what() << endl;
        return 1;
    }

    Matrix res(nr1,nc1,mat.wise);

    #pragma omp parallel for
    for(int i = 0; i < res.nrows*res.ncols; i++){
        res.m[i] = m[i] + mat.m[i];
    }
    return res;
}

Matrix Matrix::operator - (){
    Matrix res(nrows,ncols,wise);
    for(int i = 0; i < res.nrows*res.ncols; i++){
        res.m[i] = -m[i];
    }
    return res;
}


Matrix Matrix::prod(Matrix &mat){
    int nr1 = nrows; int nr2 = mat.nrows;
    int nc1 = ncols; int nc2 = mat.ncols;
    try{
        if(nc1 != nr2){
            throw rowsColsExc();
        }
    }
    catch(rowsColsExc& e){
        cerr << e.what() << endl;
        return 1;
    }
    Matrix res(nr1,nc2,mat.wise);

    for(int i = 0; i < res.nrows; i++){
        for(int j = 0; j < res.ncols; j++){
            res.setElement(i,j,0);
            for(int k = 0; k < nc1; k++){
                float el = res.getElement(i,j) + (*this).getElement(i,k)*mat.getElement(k,j);
                res.setElement(i,j,el);
             }
        }
    }
    return res;
}

Matrix Matrix::prodPar(Matrix &mat){
    int nr1 = nrows; int nr2 = mat.nrows;
    int nc1 = ncols; int nc2 = mat.ncols;
    try{
        if(nc1 != nr2){
            throw rowsColsExc();
        }
    }
    catch(rowsColsExc& e){
        cerr << e.what() << endl;
        return 1;
    }
    Matrix res(nr1,nc2,mat.wise);

    #pragma omp parallel for
    for(int i = 0; i < res.nrows; i++){
        for(int j = 0; j < res.ncols; j++){
            res.setElement(i,j,0);
            for(int k = 0; k < nc1; k++){
                float el = res.getElement(i,j) + (*this).getElement(i,k)*mat.getElement(k,j);
                res.setElement(i,j,el);
             }
        }
    }
    return res;
}


Matrix& Matrix::operator = (const Matrix& mat){

    delete m;
    this->nrows = mat.nrows;
    this->ncols = mat.ncols;

    m = new float[nrows*ncols];
    for(int i = 0; i < nrows; i++){
        m[i] = mat.m[i];
    }
    return *this;
}

bool Matrix::operator == (const Matrix& mat) const{
    if (nrows != mat.nrows & ncols != mat.ncols){
        return false;
    }
    for (int i = 0; i < nrows; i++){
		for (int j = 0; j < ncols; j++){
			if (m[i] != mat.m[i]){
                return false;
			}
		}
	}
	return true;
}

void Matrix::setNcols(int nc){
    ncols = nc;
}

void Matrix::setNrows(int nr){
    nrows = nr;
}

void Matrix::printMatrix(){
    cout << endl;
    for (int i = 0; i < nrows; i++){
		for (int j = 0; j < ncols; j++){
			cout << ' ' << (*this).getElement(i,j) + 0 << ' ';
		}
		cout << endl;
	}
	cout << endl;
}

void Matrix::fillMatrix(){
    for (int i = 0; i < nrows*ncols; i++){
        m[i] = rand() % 100;
    }
}

int Matrix::getNcols(){
    return ncols;
}
int Matrix::getNrows(){
    return nrows;
}
