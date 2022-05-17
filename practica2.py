#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon May 16 10:02:56 2022

@author: guillermomarquinez
"""

import random
import multiprocessing
from multiprocessing import Process, Array
from multiprocessing import Pool
import time
 

#PARTE 1
#Producto actualizado de matrices cuadradas, considerando las 6 posibilidades diferentes:
#Las matrices cuadradas están almacenadas como vectores por columnas
def prod_1(A,B,C):
    l=int(len(A)**(1/2))
    for i in range(0, l):
        for j in range (0,l):
            for k in range (0,l):
                C[l*j+i]+= A[i+k*l]*B[j*l+k]
    return(C)

def prod_2(A,B,C):
    l=int(len(A)**(1/2))
    for i in range(0, l):
        for k in range (0,l):
            for j in range (0,l):
                C[l*j+i]+= A[i+k*l]*B[j*l+k]
    return(C)

def prod_3(A,B,C):
    l=int(len(A)**(1/2))
    for j in range(0, l):
        for i in range (0,l):
            for k in range (0,l):
                C[l*j+i]+= A[i+k*l]*B[j*l+k]
    return(C)

def prod_4(A,B,C):
    l=int(len(A)**(1/2))
    for j in range(0, l):
        for k in range (0,l):
            for i in range (0,l):
                C[l*j+i]+= A[i+k*l]*B[j*l+k]
    return(C)

def prod_5(A,B,C):
    l=int(len(A)**(1/2))
    for k in range(0, l):
        for i in range (0,l):
            for j in range (0,l):
                C[l*j+i]+= A[i+k*l]*B[j*l+k]
    return(C)

def prod_6(A,B,C):
    l=int(len(A)**(1/2))
    for k in range(0, l):
        for j in range (0,l):
            for i in range (0,l):
                C[l*j+i]+= A[i+k*l]*B[j*l+k]
    return(C)


def matriz_aleatoria(n):
    a=[]
    for i in range(0,n*n):
        n = random.randint(-10,10)
        a.append(n)
    return(a)

#PARTE 2
#Comparamos los tiempos de las 6 opciones diferentes.
#Para ello, generamos las matrices aleatorias A,B,C de dimensión 2.000*2.000


def matriz_aleatoria(n):
    a=[]
    for i in range(0,n*n):
        n = random.randint(-10,10)
        a.append(n)
    return(a)

#Generamos las matrices
A=matriz_aleatoria(500)
B=matriz_aleatoria(500)
C=matriz_aleatoria(500)

#Comenzamos a comparar tiempos
start=time.time()
D1=prod_1(A,B,C)
end=time.time()
T1=end-start

start=time.time()
D2=prod_2(A,B,C)
end=time.time()
T2=end-start

start=time.time()
D3=prod_3(A,B,C)
end=time.time()
T3=end-start

start=time.time()
D4=prod_4(A,B,C)
end=time.time()
T4=end-start

start=time.time()
D5=prod_5(A,B,C)
end=time.time()
T5=end-start

start=time.time()
D6=prod_6(A,B,C)
end=time.time()
T6=end-start
print("FIN")