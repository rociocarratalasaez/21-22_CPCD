#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Jun 11 18:54:04 2022

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

#Producto de cada bloque para usar multiprocessing
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
             

MPI.Init()

if __name__ == '__main__':
    com=MPI.COMM_WORLD
    size = com.Get_size()
    rank = com.Get_rank()
    #El proceso 0 va a inicializar el producto y depués va a juntar las soluciones. Para no utilizar innecesariamente más CPUs el proceso 0 va a hacer el producto de un bloque. ASí que es el encagardo de inicializar y tambíen es un CPU más.
    if rank==0:
        #Creamos las matrices aleatorias a multiplicar. Para asegurarnos que el número de filas es par, vamos a crear las matrices de dimensión (2n)*(2n)
        n=5
        A=matriz_aleatoria(n)
        B=matriz_aleatoria(n)
    
        #Medimos el tiempo en secuencial
        start=time.time()
        Sol=prod_columnas(A,B)
        end=time.time()
        tiempo_secuencial=end-start
        print("El tiempo para la multiplicación de matrices cuadradas de dimensión" +str(2*n)+" en secuencial  es " + str(tiempo_secuencial))
    
        #Vamos a ver cuanto tarda el programa con multithreading con cuatro procesos
        shared_result_columnas=multiprocessing.Array('i',len(A))
        start=time.time()
        
        p1=multiprocessing.Process(target=prod_columnas_11, args=(A,B,shared_result_columnas,))
        p1.start()
        p2=multiprocessing.Process(target=prod_columnas_12, args=(A,B,shared_result_columnas,))
        p2.start()
        p3=multiprocessing.Process(target=prod_columnas_21, args=(A,B,shared_result_columnas,))
        p3.start()
        p4=multiprocessing.Process(target=prod_columnas_22, args=(A,B,shared_result_columnas,))
        p4.start()
        
        p1.join()
        p2.join()
        p3.join()
        p4.join()
        
        end=time.time()
        tiempo_multiprocessing_4=end-start
        print("El tiempo para la multiplicación de matrices cuadradas de dimensión" +str(2*n)+" con multithreading y 4 threads es " + str(tiempo_multiprocessing_4))
        
        start=time.time()
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
        end=time.time()
        tiempo_MPI=end-start
        print("El tiempo para la multiplicación de matrices cuadradas de dimensión" +str(2*n)+" con MPI y 4 módulos es " + str(tiempo_MPI))




    #Vamos a utilizar ahora multithreading junto con MPI.
    if rank==0:
        
        [A11,A12,A21,A22]=div_columnas(A)
        [B11,B12,B21,B22]=div_columnas(B)
        #Creamos un vector para poder repartirlo entre las cpus y que a cada una le toque un trozo adecuado
        Valor=[[A11,A12,B11,B21],[A11,A12,B12,B22], [A21,A22,B11,B21], [A21,A22,B12,B22]]

    else:
        Valor= None
    #Repartimos el dato Valor    
    [A11,A12,B11,B21] = com.scatter(Valor, root=0)
    
    #En cada Cpu hacemos el producto de cada bloque utilizando multithreading. Es decir, dividimos cada bloque en cuarto subbloques
    for i in range(0,size):
        shared_result_columnas_1=multiprocessing.Array('i',len(A11))
        shared_result_columnas_2=multiprocessing.Array('i',len(A11))
       
        
        p1=multiprocessing.Process(target=prod_columnas_11, args=(A11,B11,shared_result_columnas_1,))
        p1.start()
        p2=multiprocessing.Process(target=prod_columnas_12, args=(A11,B11,shared_result_columnas_1,))
        p2.start()
        p3=multiprocessing.Process(target=prod_columnas_21, args=(A11,B11,shared_result_columnas_1,))
        p3.start()
        p4=multiprocessing.Process(target=prod_columnas_22, args=(A11,B11,shared_result_columnas_1,))
        p4.start()
        
        q1=multiprocessing.Process(target=prod_columnas_11, args=(A12,B21,shared_result_columnas_2,))
        q1.start()
        q2=multiprocessing.Process(target=prod_columnas_12, args=(A12,B21,shared_result_columnas_2,))
        q2.start()
        q3=multiprocessing.Process(target=prod_columnas_21, args=(A12,B21,shared_result_columnas_2,))
        q3.start()
        q4=multiprocessing.Process(target=prod_columnas_22, args=(A12,B21,shared_result_columnas_2,))
        q4.start()
        
        p1.join()
        p2.join()
        p3.join()
        p4.join()
        
        q1.join()
        q2.join()
        q3.join()
        q4.join()
        
        MPI_multi_bloque=suma_matrices(shared_result_columnas_1, shared_result_columnas_2)
        
    MPI_multi_ans = com.gather(MPI_multi_bloque,root=0)    
    if rank==0:
        C11=MPI_multi_ans[0]
        C12=MPI_multi_ans[1]
        C21=MPI_multi_ans[2]
        C22=MPI_multi_ans[3]

        m=2*n
        C=[0]*(m*m)

        for i in range(0,n):
            for j in range(0,n):
                C[i+j*m]=C11[i+j*n]
                C[m*n+i+j*m]=C12[i+j*n]
                C[n+i+j*m]=C21[i+j*n]
                C[m*n+n+i+j*m]=C22[i+j*n]
        end=time.time()
        tiempo_MPI_multi=end-start
        print("El tiempo para la multiplicación de matrices cuadradas de dimensión" +str(2*n)+" con MPI y multithreading con 4 módulos y cuatro threads cada módulo es " + str(tiempo_MPI))




        
    
MPI.Finalize()
