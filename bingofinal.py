#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Jun 20 01:50:15 2022

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

#Función para crear una planilla aleatoria
def planilla(n):
    A=[]
    for i in range(0,n):
        x=random.randint(0,99)
        while x in A:
            x=random.randint(0,99)
        A.append(x)
    return A

#Función para crear todas las planillas y cerciorarse de que no se repitan
def planillas(num_jug, n):
    A=[0]
    for i in range (1,num_jug):
        x=planilla(n)
        while x in A:
            x=planilla(x)    
        A.append(x)
    return(A)
            
#Función para crear un número aleatorio entre 0 y 100 que no se haya dicho hasta ahora (el bombo del bingo))
def bombo(A):
    x=random.randint(0,99)
    while x in A:
        x=random.randint(0,99)
    A.append(x)
    return(x)

def funcionbingo(contador):
    if contador==10:
        Bingo=1
    else:
       Bingo=0
    return(Bingo)


#Comienza el Bingo!
MPI.Init()

com=MPI.COMM_WORLD
size = com.Get_size()
rank = com.Get_rank()

#La mesa prepara las planillas y las reparte, además inicaliza el Bombo, donde irán cayendo las bolas

if rank==0:
    A=planillas(size, 10)
    Bombo=[]
    contador=0
    Bingo=0
    victoria=0
    com.Barrier()


#Inicializamos un contador para cada jugador, cuando algún jugador tenga el número que 
#sale del bombo en su planilla sumará uno a su contador.

for i in range(1,size):
    if rank==i:
        contador=0
        A=None
        x=None
        Bingo=0
        com.Barrier()
plantilla=com.scatter(A,root=0)
#for i in range(1,size):
 #   if rank==i:
        #print("Soy el jugador "+str(rank)+ " y tengo la planilla "+str(plantilla))    


Bingo=0
for i in range(0,100):
    if Bingo==1:
        print("El bingo ha acabado")
    else:
        if rank==0:
            x=bombo(Bombo)


        x=com.bcast(x,root=0)
        for i in range(1,size):
            if rank==i:
                if x in plantilla:
                    contador+=1
                Bingo=funcionbingo(contador)

            #print("Soy el jugador "+str(rank)+" y mi contador es "+str(contador))

        Bingo=com.reduce(Bingo, op=MPI.MAX, root=0)


for i in range(1,size):
    if rank==i:
        if contador==10:
            print("Soy el jugador"+ str(rank)+ "y he ganado.")
            victoria=1
        else:
            print("Soy el jugador"+str(rank)+ "y he perdido")
            victoria=0

result=com.gather(victoria, root=0)
if rank==0:
    for i in range(1,size):
        if result[i]==1:
            print("Soy la mesa y ha ganado el jugador "+str(i))





MPI.Finalize()