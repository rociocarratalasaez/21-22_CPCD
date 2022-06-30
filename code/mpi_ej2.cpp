#include <stdio.h>
#include "mpi.h"

int main(int argc, char** argv)
{
	int mpi_id, num_procs;
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);
	
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	int ini = 10;
	if(mpi_id == 0){  
	   MPI_Send(&ini,1,MPI_INT,1,0,MPI_COMM_WORLD);
	   printf("Soy el %d y mando %d. \n",mpi_id,ini);
	   fflush(stdout);
	   int res;
           MPI_Recv(&res,1,MPI_INT,num_procs-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
           printf("Result: %d. \n",res);
	   fflush(stdout);
	} else if (mpi_id > 0 && mpi_id < num_procs-1){ 
	   int inter;
	   MPI_Recv(&inter,1,MPI_INT,mpi_id-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);	
	   inter = inter + mpi_id;
	   MPI_Send(&inter,1,MPI_INT,mpi_id+1,0,MPI_COMM_WORLD);
	   printf("Soy el %d y mando %d. \n",mpi_id,inter);
	   fflush(stdout);         
	} else if (mpi_id == num_procs-1){
	   int last;
           MPI_Recv(&last,1,MPI_INT,mpi_id-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
           last = last + mpi_id;
	   printf("Soy el %d y mando %d. \n",mpi_id,last);
           fflush(stdout);
	   MPI_Send(&last,1,MPI_INT,0,0,MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return 0;
}

