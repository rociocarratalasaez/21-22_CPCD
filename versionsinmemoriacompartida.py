#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu May  5 17:58:33 2022

@author: guillermo_marquinez
"""


import random
import multiprocessing
from multiprocessing import Process, Array
from multiprocessing import Pool
import time

#Producto de matrices cuadradas escritas como vector por filas 
def prod_filas(a,b):
    num_filas=int(len(a)**(1/2))
    mul=[0]*len(a)
    for i in range(0, num_filas):
        for j in range(0,num_filas):
              for k in range(0,num_filas):
                    mul[i*num_filas+j]+=a[i*num_filas+k]*b[j+num_filas*k]
    return(mul)
    
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
    
#Generación de una matriz cuadrada aleatoria de n * n elementos escrita en un vector
def matriz_aleatoria(n):
    a=[]
    for i in range(0,n*n):
        n = random.randint(-10,10)
        a.append(n)
    return(a)

#Cambio del formato de la matriz, escrita por filas o columnas.
def cambiar_forma_matriz(a):
    b=[0]*len(a)
    l=int(len(a)**(1/2))
    for i in range(0,l):
        for j in range(0,l):
            b[l*i+j]=a[l*j+i]
    return(b)
            
        
#Programa principal para compara tiempos
if __name__ == '__main__':
    
    n=1000
    a=matriz_aleatoria(n)
    b=matriz_aleatoria(n)
    aa=cambiar_forma_matriz(a)
    bb=cambiar_forma_matriz(b)
    start1 = time.time()
    sol=prod_filas(a,b)
    end1=time.time()
    T1=end1-start1
    print("El tiempo por filas sin mutiprocessing es;"+ str(T1))
    
    start1 = time.time()
    sol=prod_columnas(aa,bb)
    end1=time.time()
    T2=end1-start1
    print("El tiempo por columnas sin mutiprocessing es;"+ str(T2))
    
    start1=time.time()
    for i in range(0, 2):
        p1=multiprocessing.Process(target=prod_filas, args=(a,b,))
        p1.start()
    p1.join()
    
    

    end1=time.time()
    T3=end1-start1
    print("El tiempo por filas con mutiprocessing es;"+ str(T3))
    
    start1=time.time()
    for i in range(0, 2):
        p1=multiprocessing.Process(target=prod_columnas, args=(aa,bb,))
        p1.start()
    p1.join()
    
    end1=time.time()
    T4=end1-start1
    print("El tiempo por columnas con mutiprocessing es;"+ str(T4))
    
    
