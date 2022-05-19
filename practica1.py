#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu May 19 11:45:57 2022

@author: guillermomarquinez
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
    
#Cambio del formato de la matriz, escrita por filas o columnas.
def cambiar_forma_matriz(a):
    b=[0]*len(a)
    l=int(len(a)**(1/2))
    for i in range(0,l):
        for j in range(0,l):
            b[l*i+j]=a[l*j+i]
    return(b)
            

            
#Como hemos visto gracias a la práctica 2, para que multiprocessing pueda 
#sacar rendimiento de la computación paralela necesitamos que las funciones sean 
#completamente independientes. Así que vamos a utilizar la mima técnica que en 
#la práctica 2, vamos a dividir la matriz en cuatro bloques.       


def div_filas(A):
    l=int(len(A)**(1/2))
    l1=int(l/2)

    A11=[0]*(l1*l1)
    A12=[0]*(l1*l1)
    A21=[0]*(l1*l1)
    A22=[0]*(l1*l1)
    
    for i in range(0,l1):
        for j in range(0,l1):
        
                A11[i*l1+j]=A[i*l+j]
                A12[i*l1+j]=A[l1+i*l+j]
                A21[i*l1+j]=A[j+l1*l+i*l]
                A22[i*l1+j]=A[l1*l+j+i*l+l1]
                
    return([A11,A12,A21,A22])


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

#Obligamos a que las matrices cuadradas aleatorias generadas
# sean de dimension l*l, con l par.

def matriz_aleatoria(n):
    a=[]
    l=2*n
    for i in range(0,l*l):
        m = random.randint(-10,10)
        a.append(m)
    return(a)

#Hacemos el producto por bloques para cada bloque, y lo guardamos en el 
#bloque correspondiente de la matriz de salida que será un array de memoria compartida

def prod_filas_11(a,b,s):
    [a11,a12,a21,a22]=div_filas(a)
    [b11,b12,b21,b22]=div_filas(b)
    
    mul=prod_filas(a11, b11)
    mul1=prod_filas(a12,b21)
    mul2=suma_matrices(mul, mul1)
    
    l1=int(len(a11)**(1/2))
    l=int(len(s)**(1/2))

    for i in range(0,l1):
        for j in range(0,l1):
            s[i*l+j]=mul2[i*l1+j]
                    
def prod_filas_12(a,b,s):
    [a11,a12,a21,a22]=div_filas(a)
    [b11,b12,b21,b22]=div_filas(b)
    
    mul=prod_filas(a11, b12)
    mul1=prod_filas(a12,b22)
    mul2=suma_matrices(mul, mul1)
    #Añadimos a s la solución en su bloque correspondiente
    
    l1=int(len(a11)**(1/2))
    l=int(len(s)**(1/2))

    for i in range(0,l1):
        for j in range(0,l1):
            s[i*l+j+l1]=mul2[i*l1+j]
        
def prod_filas_21(a,b,s):
    [a11,a12,a21,a22]=div_filas(a)
    [b11,b12,b21,b22]=div_filas(b)
    
    mul=prod_filas(a21, b11)
    mul1=prod_filas(a22,b21)
    mul2=suma_matrices(mul, mul1)
    #Añadimos a s la solución en su bloque correspondiente
    
    l1=int(len(a11)**(1/2))
    l=int(len(s)**(1/2))

    for i in range(0,l1):
        for j in range(0,l1):
            s[l1*l+j+l*i]=mul2[i*l1+j]
            
def prod_filas_22(a,b,s):
     [a11,a12,a21,a22]=div_filas(a)
     [b11,b12,b21,b22]=div_filas(b)
     
     mul=prod_filas(a21, b12)
     mul1=prod_filas(a22,b22)
     mul2=suma_matrices(mul, mul1)
     #Añadimos a s la solución en su bloque correspondiente
     
     l1=int(len(a11)**(1/2))
     l=int(len(s)**(1/2))

     for i in range(0,l1):
         for j in range(0,l1):
             s[l1*l+l1+i*l+j]=mul2[i*l1+j]
             
def prod_columnas_11(a,b,s):
    [a11,a12,a21,a22]=div_columnas(a)
    [b11,b12,b21,b22]=div_columnas(b)
    
    mul=prod_columnas(a11, b11)
    mul1=prod_columnas(a12,b21)
    mul2=suma_matrices(mul, mul1)
    #Añadimos a s la solución en su bloque correspondiente
    
    l1=int(len(a11)**(1/2))
    l=int(len(s)**(1/2))

    for i in range(0,l1):
        for j in range(0,l1):
            s[i+j*l]=mul2[i+j*l1]
                    
def prod_columnas_12(a,b,s):
    [a11,a12,a21,a22]=div_columnas(a)
    [b11,b12,b21,b22]=div_columnas(b)
    
    mul=prod_columnas(a11, b12)
    mul1=prod_columnas(a12,b22)
    mul2=suma_matrices(mul, mul1)
    #Añadimos a s la solución en su bloque correspondiente
    
    l1=int(len(a11)**(1/2))
    l=int(len(s)**(1/2))

    for i in range(0,l1):
        for j in range(0,l1):
            s[i+(j+l1)*l]=mul2[i+j*l1]
        
def prod_columnas_21(a,b,s):
    [a11,a12,a21,a22]=div_columnas(a)
    [b11,b12,b21,b22]=div_columnas(b)
    
    mul=prod_columnas(a21, b11)
    mul1=prod_columnas(a22,b21)
    mul2=suma_matrices(mul, mul1)
    #Añadimos a s la solución en su bloque correspondiente
    
    l1=int(len(a11)**(1/2))
    l=int(len(s)**(1/2))

    for i in range(0,l1):
        for j in range(0,l1):
            s[i+l1+j*l]=mul2[i+j*l1]
            
def prod_columnas_22(a,b,s):
     [a11,a12,a21,a22]=div_columnas(a)
     [b11,b12,b21,b22]=div_columnas(b)
     
     mul=prod_columnas(a21, b12)
     mul1=prod_columnas(a22,b22)
     mul2=suma_matrices(mul, mul1)
     #Añadimos a s la solución en su bloque correspondiente
     
     l1=int(len(a11)**(1/2))
     l=int(len(s)**(1/2))
     
     for i in range(0,l1):
         for j in range(0,l1):
             s[i+l1+(j+l1)*l]=mul2[i+j*l1]
             


#Programa principal para comparar tiempos
if __name__ == '__main__':
    
    n=2
    a=matriz_aleatoria(n)
    b=matriz_aleatoria(n)
    
    aa=cambiar_forma_matriz(a)
    bb=cambiar_forma_matriz(b)
    
    
    start = time.time()
    sol_filas=prod_filas(a,b)
    end=time.time()
    T1=end-start
    print("El tiempo por filas sin mutiprocessing es "+ str(T1))
    
    start = time.time()
    sol_columnas=prod_columnas(aa,bb)
    end=time.time()
    T2=end-start
    print("El tiempo por columnas sin mutiprocessing es "+ str(T2))
    

    #variable de memoria compartida
    shared_result_filas=multiprocessing.Array('i',len(a))
    start=time.time()
    
    p1=multiprocessing.Process(target=prod_filas_11, args=(a,b,shared_result_filas,))
    p1.start()
    p2=multiprocessing.Process(target=prod_filas_12, args=(a,b,shared_result_filas,))
    p2.start()
    p3=multiprocessing.Process(target=prod_filas_21, args=(a,b,shared_result_filas,))
    p3.start()
    p4=multiprocessing.Process(target=prod_filas_22, args=(a,b,shared_result_filas,))
    p4.start()
    
    p1.join()
    p2.join()
    p3.join()
    p4.join()
    
    end=time.time()
    T3=end-start
    print("El tiempo por filas con mutiprocessing es "+ str(T3))
    
    #variable de memoria compartida
    shared_result_columnas=multiprocessing.Array('i',len(a))
    start=time.time()
    
    p1=multiprocessing.Process(target=prod_columnas_11, args=(aa,bb,shared_result_columnas,))
    p1.start()
    p2=multiprocessing.Process(target=prod_columnas_12, args=(aa,bb,shared_result_columnas,))
    p2.start()
    p3=multiprocessing.Process(target=prod_columnas_21, args=(aa,bb,shared_result_columnas,))
    p3.start()
    p4=multiprocessing.Process(target=prod_columnas_22, args=(aa,bb,shared_result_columnas,))
    p4.start()
    
    p1.join()
    p2.join()
    p3.join()
    p4.join()
    
    end=time.time()
    T4=end-start
    print("El tiempo por columnas con mutiprocessing es "+ str(T4))
    
    #Para recuperar los resultados de la memoria compartida
    Resultado_filas_multiprocessing=shared_result_filas[:]
    Resultado_columnas_multiprocessing=shared_result_columnas[:]