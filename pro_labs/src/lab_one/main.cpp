#include "mpi.h"
#include <stdio.h>
int main(int argc, char *argv[]) {
    MPI_Status Status;
    int RecvRank;
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        printf("\n Hello from process %3d ", rank);
        for (int i = 1; i < size; i++) {
            MPI_Recv(&RecvRank, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                     MPI_COMM_WORLD, &Status);
            printf("\n Hello from process %3d ", RecvRank);
        }
    } else
        MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}