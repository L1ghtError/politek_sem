#ifndef _MPI_VAR_PRINT_
#define _MPI_VAR_PRINT_

#include "mpi_vars.h"
#include "payload/mpi_payload_print.h"

void MpiVars_Print(FILE *fd, MpiVars_t *var) {
    fd_print_element_ex(fd, "Matrix A:", var->A, var->N, var->N);
    // May contain memory garbage
    fd_print_element_ex(fd, "Vector b:", var->b, var->N, 1);

    fd_print_element_ex(fd, "Matrix A1:", var->A1, var->N, var->N);
    fd_print_element_ex(fd, "Vector b1:", var->b1, var->N, 1);
    fd_print_element_ex(fd, "Vector c1:", var->c1, var->N, 1);

    fd_print_element_ex(fd, "Matrix A2:", var->A2, var->N, var->N);
    fd_print_element_ex(fd, "Matrix B2:", var->B2, var->N, var->N);
    // May contain memory garbage
    fd_print_element_ex(fd, "Matrix C2:", var->C2, var->N, var->N);

    // May contain memory garbage
    fd_print_element_ex(fd, "Matrix y1:", var->y1, var->N, 1);
    fd_print_element_ex(fd, "Vector y2:", var->y2, var->N, 1);
    fd_print_element_ex(fd, "Vector Y3:", var->Y3, var->N, var->N);

    fprintf(fd, "\n");
    fflush(fd);
}

#endif //_MPI_VAR_PRINT_