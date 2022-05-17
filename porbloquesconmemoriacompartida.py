#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue May 17 00:43:06 2022

@author: guillermomarquinez
"""

import random
import multiprocessing
from multiprocessing import Process, Array, Queue
from multiprocessing import Pool
import time

#PARTE 3 y PARTE 4

#Recuperamos el programa más eficiente de la PARTE 1

def prod_1(A,B,C):
    l=int(len(A)**(1/2))
    
    for i in range(0, l):
        for j in range (0,l):
            for k in range (0,l):
                C[l*j+i]+= A[i+k*l]*B[j*l+k]
    return(C)


#Dividimos las matrices en 4 bloques, para ello necesitamos que la matriz A 
#sea cuadrada de dimensión l*l donde 2 divide a l.

def div(A):
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

#Hacemos el producto por bloques, cada bloque queda determinado por un
#proceso totalmente independiente de los demás.
#La última variable de las funciones es la varaible compartida por los procesos independientes
#Es decir, es la solución final a la que cada proceso aporta su su bloque


def proceso_primero(a,b,c,s):
    [a11,a12,a21,a22]=div(a)
    [b11,b12,b21,b22]=div(b)
    [c11,c12,c21,c22]=div(c)
    
    C11_1=prod_1(a11,b11,c11)
    C11=prod_1(a12, b21, C11_1)
#Añadimos a s la solución en su bloque correspondiente    
    
    l1=int(len(C11)**(1/2))
    l=int(len(s)**(1/2))

    for i in range(0,l1):
        for j in range(0,l1):
            s[i+j*l]=C11[i+j*l1]
  

def proceso_segundo(a,b,c,s):
    [a11,a12,a21,a22]=div(a)
    [b11,b12,b21,b22]=div(b)
    [c11,c12,c21,c22]=div(c)

    C12_1=prod_1(a11, b12, c12)
    C12=prod_1(a12,b22, C12_1)
#Añadimos a s la solución en su bloque correspondiente
    
    l1=int(len(C12)**(1/2))
    l=int(len(s)**(1/2))

    for i in range(0,l1):
        for j in range(0,l1):
            s[i+(j+l1)*l]=C12[i+j*l1]
    
   


def proceso_tercero(a,b,c,s):
    [a11,a12,a21,a22]=div(a)
    [b11,b12,b21,b22]=div(b)
    [c11,c12,c21,c22]=div(c)
    
    C21_1=prod_1(a21,b11,c21)
    C21=prod_1(a22,b21, C21_1)
#Añadimos a s la solución en su bloque correspondiente
    
    l1=int(len(C21)**(1/2))
    l=int(len(s)**(1/2))

    for i in range(0,l1):
        for j in range(0,l1):
            s[i+l1+j*l]=C21[i+j*l1]
    
    
    
def proceso_cuarto(a,b,c,s):
    [a11,a12,a21,a22]=div(a)
    [b11,b12,b21,b22]=div(b)
    [c11,c12,c21,c22]=div(c)
    
    C22_1=prod_1(a21,b12,c22)
    C22=prod_1(a22,b22,C22_1)
#Añadimos a s la solución en su bloque correspondiente
    
    l1=int(len(C22)**(1/2))
    l=int(len(s)**(1/2))
    
    for i in range(0,l1):
        for j in range(0,l1):
            s[i+l1+(j+l1)*l]=C22[i+j*l1]
    
    
#Programa principal
    
if __name__ == '__main__':
    A=matriz_aleatoria(30)
    B=matriz_aleatoria(30)
    C=matriz_aleatoria(30)
#El resultado final, al que cada proceso añadira su resultado en su bloque correspondiente 
#es shared_result y está en la memoria compartida

    shared_result=multiprocessing.Array('i',len(A))
    start=time.time()

#Cada proceso añade su resultado
    p1=multiprocessing.Process(target=proceso_primero, args=(A,B,C,shared_result,))
    p1.start()
    p2=multiprocessing.Process(target=proceso_segundo,args=(A,B,C,shared_result,))
    p2.start()
    p3=multiprocessing.Process(target=proceso_tercero, args=(A,B,C,shared_result,))
    p3.start()
    p4=multiprocessing.Process(target=proceso_cuarto,args=(A,B,C,shared_result,))
    p4.start()
    
    p1.join()
    p2.join()
    p3.join()
    p4.join()
    end=time.time()
#Vemos cuánto tiempo nos cuesta el programa utilizando múltiples procesos
    T1=end-start 
    print("El tiempo usando multiples procesos es"+ str(T1))
    
#Aquí, hacemos el mismo producto en secuencial, utilizando el programa que habiamos hecho
#para la pregunta 1
    start=time.time()
    Result=prod_1(A,B,C)
    end=time.time()
    T2=end-start

#Comparamos con el tiempo en secuencial
    print("El tiempo sin múltiples procesos es "+ str(T2))
#El resultado final del producto utilizando el paralelismo no es una array como tal
#pero lo podemos recuperar como array de la siguiente manera 
    Resultado_paralelo=shared_result[:]
   
#Para probar que el programa paralelizado está bien hecho tenemos que comprobar que 
#Resultado_paralelo==Result.
#Como curiosidad: la función prod_1 sobreescribe la matriz C. Sin embargo, en el 
#programa en paralelo la sobreescribe en cada proceso 'child', dejando C en el proceso 
#principal (parent process) tal y como se generó. Si, en cambio, antes de hacer el programa
#en paralelo, hacemos prod_1 en secuencial vamos a modificar la matriz C en el proceso principal
#y al hacer el producto en paralelo estaremos tomando otra matriz C, por lo que
#el resultado en paralelo y secuencial no será el mismo.
