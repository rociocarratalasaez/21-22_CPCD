#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu May 19 19:01:18 2022

@author: guillermomarquinez
"""

import mpi4py
mpi4py.rc.initialize = False # do not initialize
# MPI automatically
mpi4py.rc.finalize = False # do not finalize MPI
# automatically
from mpi4py import MPI # import the 'MPI'
# module

MPI.Init()

com=MPI.COMM_WORLD

rank=com.Get_rank()

num_procs=com.Get_size()
if rank==0:
        data=10
        com.send(data,dest=1,tag=42)
        print("Soy el proceso 0 y he enviado al proceso 1 el número: "+str(data))
        data_final=com.recv(source=(num_procs-1),tag=42)
        print("Soy el proceso 0, y ya se ha completado la vuelta entera, he recibido el valor: " +str(data_final))
for i in range (1,num_procs):
        if rank==i:
                k=i-1
                data=com.recv(source=k,tag=42)
                print("Soy el proceso" +str(rank) + " y he recibido de "+str(rank-1)+"  el número: "+str(data))
                j=(rank+1)%num_procs
                data+=rank
                com.send(data,dest=j,tag=42)
MPI.Finalize()