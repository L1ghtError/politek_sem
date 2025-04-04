#ifndef _MPI_VARS_LINEAR_
#define _MPI_VARS_LINEAR_
#include "mpi_vars.h"

void MpiVars_SetLinear(MpiVars_t *var) {
    for (int i = 0; i < var->total_cont_size; i++) {
        // because we know that all elementrs placed in oredr
        // we set all values without buff overflow
        // COUTION this function sets ALL elements to i,
        // including y1,y2,Y3...
        var->A[i] = i % 6;
    }
}

#endif // _MPI_VARS_LINEAR_
