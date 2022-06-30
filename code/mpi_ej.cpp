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
	   for(int j = 1; j < num_procs; j++){
	     MPI_Send(&ini,1,MPI_INT,j,0,MPI_COMM_WORLD);
             printf("Soy el 0 y mando %d a %d. \n",ini,j);
             fflush(stdout);
	   }
	   for(int j =1; j < num_procs; j++){
	     int res;
             MPI_Recv(&res,1,MPI_INT,j,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
             printf("Result: %d de %d. \n",res,j);
             fflush(stdout);
	  }
	} else {
	   int inter;
	   MPI_Recv(&inter,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	   inter = inter + mpi_id;
	   MPI_Send(&inter,1,MPI_INT,0,0,MPI_COMM_WORLD);
	   printf("Soy el %d y mando %d. \n",mpi_id,inter);
	   fflush(stdout);
	}

	MPI_Finalize();

	return 0;
}

