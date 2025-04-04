#ifndef _MPI_VARS_
#define _MPI_VARS_
#include "payload/mpi_payload.h"
#include "stdlib.h" // malloc

struct MpiVars {
    // Initial
    payload_matrix A;  // Random or input
    payload_vec_col b; // Formula b = (i % 2) == 0 ? (3 / ((i * i) + 3)) : 3 / i

    payload_matrix A1;  // Random or input
    payload_vec_col b1; // Random or input
    payload_vec_col c1; // Random or input

    payload_matrix A2; // Random or input
    payload_matrix B2; // Random or input
    payload_matrix C2; // Formula C2 = 1 / ((i + j) * (i + j))
    // Calculated
    payload_vec_col y1; // Formula y1 = (A * b)

    payload_vec_col y2; // Formula y2 = A1 * (b1 + (2 * c1))

    payload_matrix Y3; // Formula Y3 = A2 * (B2 - C2)

    // At the end because of the C mem align
    int N; // size of matrices and vectors

    // Got free 4 bytes ;)
    int total_cont_size;
};
typedef struct MpiVars MpiVars_t;

inline int sizeofMpiVars(const MpiVars_t *mpvars) {
    const size_t payload_size = sizeof(payload);
    return mpvars->total_cont_size * payload_size;
}

void UpdateAddresses(MpiVars_t *ptr) {
    if (ptr == NULL) {
        return;
    }
    const size_t struct_size = sizeof(*ptr);
    const size_t payload_size = sizeof(payload);
    const int N = ptr->N;
    const size_t vec_size = payload_size * N;
    const size_t mat_size = vec_size * N;

    // COUTION 1 == sizeof(*ptr)!
    payload *iter = (payload *)(ptr + 1);

    const size_t mat_off = mat_size / payload_size;
    const size_t vec_off = vec_size / payload_size;
    ptr->A = (iter += mat_off) - mat_off;

    ptr->b = (iter += vec_off) - vec_off;

    ptr->A1 = (iter += mat_off) - mat_off;
    ptr->b1 = (iter += vec_off) - vec_off;
    ptr->c1 = (iter += vec_off) - vec_off;

    ptr->A2 = (iter += mat_off) - mat_off;
    ptr->B2 = (iter += mat_off) - mat_off;
    ptr->C2 = (iter += mat_off) - mat_off;

    ptr->y1 = (iter += vec_off) - vec_off;
    ptr->y2 = (iter += vec_off) - vec_off;
    ptr->Y3 = (iter += mat_off) - mat_off;
}

MpiVars_t *NewMpiVars(const int N) {
    MpiVars_t *ptr;
    // Explicit for readability
    const size_t struct_size = sizeof(*ptr);
    const size_t payload_size = sizeof(payload);

    const size_t vec_size = payload_size * N;
    const size_t mat_size = vec_size * N;

    const size_t A_size = mat_size;
    const size_t b_size = vec_size;

    const size_t A1_size = mat_size;
    const size_t b1_size = vec_size;
    const size_t c1_size = vec_size;

    const size_t A2_size = mat_size;
    const size_t B2_size = mat_size;
    const size_t C2_size = mat_size;

    const size_t y1_size = vec_size;
    const size_t y2_size = vec_size;
    const size_t Y3_size = mat_size;

    const size_t total_container_size = A_size + b_size + A1_size + b1_size +
                                        c1_size + A2_size + B2_size + C2_size +
                                        y1_size + y2_size + Y3_size;
    ptr = (MpiVars_t *)malloc(struct_size + total_container_size);
    ptr->N = N;
    ptr->total_cont_size = total_container_size / payload_size;
    UpdateAddresses(ptr);
    return ptr;
}

void DestroyMpiVars(MpiVars_t *t) { return free(t); }

#endif // _MPI_VARS_