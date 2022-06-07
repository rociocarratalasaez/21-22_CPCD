#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu May 19 18:21:24 2022

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
        for i in range(1,num_procs):
                com.send(data,dest=i,tag=42)
                print("Soy el proceso central y envío a "+str(i)+" el valor "+str(data))
                data=com.recv(source=i, tag=43)
                print("Soy el proceso central y he recibido de "+str(i)+" el valor "+str(data))
for i in range(1,num_procs):
        if rank==i:
                data=com.recv(source=0, tag=42)
                data+=rank
                com.send(data,dest=0,tag=43)

MPI.Finalize()

