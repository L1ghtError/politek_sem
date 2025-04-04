#ifndef _MPI_VARS_RAND_
#define _MPI_VARS_RAND_
#include "mpi_vars.h"

void MpiVars_SetRand(MpiVars_t *var, int seed) {
    srand(seed);
    for (int i = 0; i < var->total_cont_size; i++) {
        // because we know that all elementrs placed in oredr
        // we set all values without buff overflow
        // COUTION this function sets ALL elements to rand,
        // including y1,y2,Y3...
        var->A[i] = rand() % 6;
    }
}

#endif // _MPI_VARS_RAND_
