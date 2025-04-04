#ifndef _MPI_VAR_INPUT_FACTORY_
#define _MPI_VAR_INPUT_FACTORY_
#include "mpi_vars.h"

#include "mpi_var_linear.h"
#include "mpi_var_manual.h"
#include "mpi_var_rand.h"

enum MPI_INPUT_TYPE {
    // iterates over ALL elements and
    // assigns them to i starting from 0
    MPI_INPUT_LINEAR = 0,
    // manual console input
    MPI_INPUT_MANUAL = 1,
    // iterates over ALL elements and
    // assigns elements to random values
    MPI_INPUT_RANDOM = 2,
};

typedef enum MPI_INPUT_TYPE mpi_input_type_t;

int MpiVars_Fill(MpiVars_t *val, mpi_input_type_t type) {
    switch (type) {
    case MPI_INPUT_LINEAR:
        MpiVars_SetLinear(val);
        break;
    case MPI_INPUT_MANUAL:
        MpiVars_SetManual(val);
        break;
    case MPI_INPUT_RANDOM:
        MpiVars_SetRand(val, 0xdeadbeef);
        break;

    default:
        return -1;
    }
    return 0;
}

#endif // _MPI_VAR_INPUT_FACTORY_