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
 
def prod_filas(a,b):
    num_filas=int(len(a)**(1/2))
    mul=[0]*len(a)
    for i in range(0, num_filas):
        for j in range(0,num_filas):
              for k in range(0,num_filas):
                    mul[i*num_filas+j]+=a[i*num_filas+k]*b[j+num_filas*k]
    return(mul)
    

def prod_columnas(a,b):
    num_filas=int(len(a)**(1/2))
    mul=[0]*len(a)
    for j in range(0, num_filas):
        for i in range(0,num_filas):
            for k in range(0,num_filas):
                mul[i+j*num_filas]+=a[i+k*num_filas]*b[j*num_filas+k]
    return(mul)
    
def suma_matrices(a,b):
    suma=[0]*len(a)
    for i in range(0,len(a)):
        suma[i]=a[i]+b[i]    
    return(suma)
    
def matriz_aleatoria(n):
    a=[]
    for i in range(0,n*n):
        n = random.randint(-10,10)
        a.append(n)
    return(a)

if __name__ == '__main__':
    
    n=400
    a=matriz_aleatoria(n)
    b=matriz_aleatoria(n)
    sol=[0]*len(a)
    start1 = time.time()
    sol=prod_filas(a,b)
    end1=time.time()
    T1=end1-start1
    print("El tiempo sin mutiprocessing es;"+ str(T1))

    start2=time.time()
    for i in range(0, 2):
        p1=multiprocessing.Process(target=prod_filas, args=(a,b,))
        p1.start()
    p1.join()
    
    

    end2=time.time()
    T2=end2-start2
    print("El tiempo con mutiprocessing es;"+ str(T2))
    
    
    