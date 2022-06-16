#include <stdio.h>
#include "mpi.h"
int main(int argc, char** argv)
{
	int mpi_id, num_procs, valor = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	
	if(mpi_id == 0){
		valor = 10;
		for(int i = 1; i < num_procs; i++){
			MPI_Send(&valor, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			printf("Soy el proceso 0 y mando un %d!\n",valor);
	                fflush(stdout);
			MPI_Recv(&valor, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("Soy el proceso 0 y me ha llegado: %d\n", valor);
			fflush(stdout);
		}
	}else{
		MPI_Recv(&valor, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Soy el proceso %d y me ha llegado: %d\n", mpi_id, valor);
		fflush(stdout);
		valor = valor + mpi_id;
		MPI_Send(&valor, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 0;
}
