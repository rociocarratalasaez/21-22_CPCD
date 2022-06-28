#include <iostream>
#include <algorithm>
#include "mpi.h"

using namespace std;

void printCard(int* data, int n, int player)
{
    printf("Player %d: \n",player);
	for(int i = 0; i < n; i++){
        printf("%d ", data[i]);
	}
	printf("\n");
}

void printNums(int* data, int ind)
{
    printf("\n");
	for(int i = 0; i < ind; i++){
        printf("%d, ", data[i]);
	}
	printf("\n");
}

void fillList(int* data, int n, int range)
{
    for(int j = 0; j < n; j++){
        data[j] = rand() % range;
    }
}

int checkBingo(int* data, int size_data, int bingo_lim){
    int num_check = 0;
    int bingo = 0;
    for(int j = 0; j < size_data; j++){
        if(data[j] == bingo_lim){
            num_check++;
        }
    }
    if(num_check == size_data){
        bingo = 1;
    }

    return bingo;
}

int main(int argc, char** argv)
{

    // settings
	int mpi_id, num_procs;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);

	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	int range = 100;
    int list_size = 20;
    //process 0
    if(mpi_id == 0){
        //create cards with numbers and send them to players
        for(int i = 1; i < num_procs; i++){
            int *card_i = (int*) malloc(sizeof(int)*list_size);
            fillList(card_i,list_size,range);
            printCard(card_i, list_size,i);
            fflush(stdout);
            MPI_Send(card_i,list_size,MPI_INT,i,0,MPI_COMM_WORLD);
            //printf("Soy el proceso 0 y mando la carta al jugador %d. \n",i);
            //fflush(stdout);
        }

        int in_progress = 1;
        // numbers that can be chosen
        int *numbers = (int*) malloc(sizeof(int)*range);
            for(int j = 0; j < range; j++){
                numbers[j] = j;
            }
        random_shuffle(numbers, numbers + range);
        int ind = 0;

        int winner;
        printf("\nSoy el proceso 0 y he elegido siguientes numeros: \n");
        fflush(stdout);
        while(in_progress == 1){
            //game progress
            int rn_num = numbers[ind];
            ind++;

            for(int i = 1; i < num_procs; i++){
                MPI_Send(&rn_num,1,MPI_INT,i,0,MPI_COMM_WORLD);
                //printf("Soy el proceso 0 y mando el numero %d al jugador %d. \n",rn_num,i);
                //fflush(stdout);
            }
            int stop_game = 0;
            for(int i = 1; i < num_procs; i++){
                int rec_progress;
                MPI_Recv(&rec_progress,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                fflush(stdout);
                if (rec_progress == 1){
                    //printf("El juego continua.");
                    //fflush(stdout);
                }
                else{
                    winner = i;
                    stop_game++;
                }
            }
            if (stop_game > 0){
                in_progress = 0;
                //printf("Soy el proceso 0 y mando el numero %d al jugador %d. \n",rn_num,i);
                //fflush(stdout);
                printNums(numbers,ind);
                fflush(stdout);
            }

            for(int i = 1; i < num_procs; i++){
                MPI_Send(&in_progress,1,MPI_INT,i,0,MPI_COMM_WORLD);
            }
            for(int i = 1; i < num_procs; i++){
                int delay;
                MPI_Recv(&delay,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                fflush(stdout);
            }
        }
        printf("\nHa ganado el proceso %d, fin de la partida. Gracias por jugar.\n",winner);
    //players
    //recive card + exchanging information with process 0 about chosen numbers
	} else if (mpi_id > 0 && mpi_id < num_procs){
        int *card_player = (int*) malloc(sizeof(int)*list_size);
        MPI_Recv(card_player,list_size,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        //printf("Soy el proceso %d y recibo la carta del 0. \n",mpi_id);

        int prog = 1;
        while(prog == 1){
            //get number from 0 and check the card
            int num_choice;
            MPI_Recv(&num_choice,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            //printf("Soy el jugador %d y recibo el numero %d del 0. \n",mpi_id,num_choice);
            //fflush(stdout);
            for(int i = 0; i < list_size; i++){
                if(card_player[i] == num_choice){
                    card_player[i] = range;
                }
            }
            int bingo = checkBingo(card_player, list_size, range);
            int in_prog = 1 - bingo;
            MPI_Send(&in_prog,1,MPI_INT,0,0,MPI_COMM_WORLD);

            if(bingo == 1){
                printf("\n Soy el %d... Bingo!\n",mpi_id);
            }

            fflush(stdout);

            int prog_resp;
            MPI_Recv(&prog_resp,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            prog = prog_resp;


            if(bingo == 0 && prog == 0){
                printf("\n Soy el %d... he perdido esta vez.\n",mpi_id);
            }
            // time delay

            int delay = 0;
            MPI_Send(&delay,1,MPI_INT,0,0,MPI_COMM_WORLD);
        }
	}

	MPI_Finalize();

    return 0;
}

