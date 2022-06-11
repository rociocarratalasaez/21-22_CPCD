#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Jun 11 15:17:59 2022

@author: guillermomarquinez
"""

import mpi4py
mpi4py.rc.initialize = False # do not initialize
# MPI automatically
mpi4py.rc.finalize = False # do not finalize MPI
# automatically
from mpi4py import MPI # import the 'MPI'
import random
import multiprocessing
from multiprocessing import Process, Array
from multiprocessing import Pool
import time

#Creación de matriz aleatoria
def matriz_aleatoria(n):
    a=[]
    l=2*n
    for i in range(0,l*l):
        m = random.randint(-10,10)
        a.append(m)
    return(a)


#Producto de matrices cuadradas escritas como vector por columnas
def prod_columnas(a,b):
    num_filas=int(len(a)**(1/2))
    mul=[0]*len(a)
    for j in range(0, num_filas):
        for i in range(0,num_filas):
            for k in range(0,num_filas):
                mul[i+j*num_filas]+=a[i+k*num_filas]*b[j*num_filas+k]
    return(mul)


#Suma de matrices escritas como vector por filas o columnas 
def suma_matrices(a,b):
    suma=[0]*len(a)
    for i in range(0,len(a)):
        suma[i]=a[i]+b[i]    
    return(suma)


#División en cuatro bloques de la matriz
def div_columnas(A):
    l=int(len(A)**(1/2))
    l1=int(l/2)

    A11=[0]*(l1*l1)
    A12=[0]*(l1*l1)
    A21=[0]*(l1*l1)
    A22=[0]*(l1*l1)
    
    for i in range(0,l1):
        for j in range(0,l1):
        
                A11[i+j*l1]=A[i+l*j]
                A12[i+j*l1]=A[l*l1+i+j*l]
                A21[i+j*l1]=A[j*l+l1+i]
                A22[i+j*l1]=A[l1*l+j*l+i+l1]
                
    return([A11,A12,A21,A22])


MPI.Init()


com=MPI.COMM_WORLD
size = com.Get_size()
rank = com.Get_rank()
#El proceso 0 va a inicializar el producto y depués va a juntar las soluciones. Para no utilizar innecesariamente más CPUs el proceso 0 va a hacer el producto de un bloque. ASí que es el encagardo de inicializar y tambíen es un CPU más.
if rank==0:
    n=5
    A=matriz_aleatoria(n)
    B=matriz_aleatoria(n)
    Sol=prod_columnas(A,B)
    print(Sol)
    [A11,A12,A21,A22]=div_columnas(A)
    [B11,B12,B21,B22]=div_columnas(B)
#Creamos un vector para poder repartirlo entre las cpus y que a cada una le toque un trozo adecuado
    Valor=[[A11,A12,B11,B21],[A11,A12,B12,B22], [A21,A22,B11,B21], [A21,A22,B12,B22]]

else:
    Valor= None


#Repartimos el dato Valor    
[A11,A12,B11,B21] = com.scatter(Valor, root=0)

#En cada Cpu hacemos el producto de cada bloque 
for i in range(0,size):
        C=suma_matrices(prod_columnas(A11, B11),prod_columnas(A12, B21))

#El Cpu 0 recoge cada bloque
C_ans = com.gather(C,root=0)


if rank==0:
    C11=C_ans[0]
    C12=C_ans[1]
    C21=C_ans[2]
    C22=C_ans[3]

    m=2*n
    C=[0]*(m*m)

    for i in range(0,n):
        for j in range(0,n):
            C[i+j*m]=C11[i+j*n]
            C[m*n+i+j*m]=C12[i+j*n]
            C[n+i+j*m]=C21[i+j*n]
            C[m*n+n+i+j*m]=C22[i+j*n]
    print(C)

MPI.Finalize()