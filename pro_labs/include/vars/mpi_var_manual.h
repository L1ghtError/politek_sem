#ifndef _MPI_VARS_MANUAL_
#define _MPI_VARS_MANUAL_
#include "mpi_vars.h"
#include "stdio.h"
#define FGETS_INPUT_SIZE 100

void set_element(payload *container, int h, int w) {
    char input[FGETS_INPUT_SIZE];
    for (int i = 0; i < h; i++) {
        printf("\n");
        for (int j = 0; j < w; j++) {
            printf("Element[%d][%d]:", i, j);
            if (fgets(input, sizeof(input), stdin) != NULL) {
                char *endptr;
                double value = strtod(input, &endptr);

                if (endptr == input || *endptr != '\n' && *endptr != '\0') {
                    printf("Invalid input, please enter a valid number.\n");
                    j--;
                } else {
                    container[i * w + j] = value;
                }
            }
        }
    }
}

void MpiVars_SetManual(MpiVars_t *var) {
    printf("Please, provide values for Matrix A:\n");
    set_element(var->A, var->N, var->N);

    printf("Please, provide values for Matrix A1:\n");
    set_element(var->A1, var->N, var->N);

    printf("Please, provide values for Vector b1:\n");
    set_element(var->b1, 1, var->N);

    printf("Please, provide values for Vector c1:\n");
    set_element(var->c1, 1, var->N);

    printf("Please, provide values for Matrix A2:\n");
    set_element(var->A2, var->N, var->N);

    printf("Please, provide values for Matrix B2:\n");
    set_element(var->B2, var->N, var->N);
}

#endif // _MPI_VARS_MANUAL_
