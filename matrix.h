#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include <iostream>
#include <fstream>
#include <omp.h>
#include <exception>


using namespace std;

class Matrix {
    protected:
        int nrows; //number of rows
        int ncols; //number of columns
        int wise; //0: row-wise, 1: column-wise
        float* m; //elements stored linearly
        void setNcols(int nc); //set number of columns
        void setNrows(int nr); //set number of rows
    public:
        Matrix(int w); //base constructor
        Matrix(int nr, int nc,int w); //create matrix with given size
        Matrix(const Matrix&); //constructor initialized by copy
        ~Matrix(); //destructor

        void fillMatrix(); //fill matrix with random numbers in (0,99)
        void printMatrix(); //print matrix
        int getNcols(); //return number columns
        int getNrows(); //return number rows
        float getElement(int i, int j);  //return [i,j]-element
        void setElement(int i, int j, float element); //set [i,j]-element to value "element"
        Matrix sum(const Matrix&); //sum of matrices
        Matrix sumPar(const Matrix&); //sum of matrices PARALLEL
        Matrix operator - (); //change sign of matrix
        Matrix prod(Matrix&); //matrix product
        Matrix prodPar(Matrix&); //matrix product PARALLEL
        Matrix& operator = (const Matrix&); //assigning operator
        bool operator == (const Matrix&) const; //compare matrices

};

// exceptions

struct rowsColsExc : public exception {
   const char * what () const throw () {
      return "Number of rows/cols does not match - Unable to perform given matrix operation.";
   }
};

struct indxOutOfRange : public exception {
   const char * what () const throw () {
      return "Index out of range.";
   }
};

struct openFileErr : public exception {
   const char * what () const throw () {
      return "Constructor error - Unable to open file.";
   }
};

struct emptyFileErr : public exception {
   const char * what () const throw () {
      return "Constructor error - File is empty.";
   }
};

struct inputFileErr : public exception {
   const char * what () const throw () {
      return "Constructor error - Number of rows/columns not specified.";
   }
};

struct dimFileErr : public exception {
   const char * what () const throw () {
      return "Constructor error - Matrix dimensions does not match given data.";
   }
};

#endif // MATRIX_H_INCLUDED
