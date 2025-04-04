#ifndef _MPI_CALCULATIONS_
#define _MPI_CALCULATIONS_
#include "payload/mpi_payload.h"
#include "vars/mpi_vars.h"
// Variant 3

// Sets values of `b` to my variant. `b` or `bi` can be
// used interchangeably.
// Because b1 is strictly `colum-vecor`, example is:
// `[5.11]`
// `[6.74]`
// `[9.23]`
// we cares only about it height, and can ignore width
void mpi_calc_b(paylaod_header_t *b) {
    for (int i = 0; i < b->h; i++) {
        // Uses `+ 1` to starting counting from 1 to
        // prevent from dividing by zero
        b->ptr[i] = i % 2 == 0 ? ((payload)3 / (((i + 1) * (i + 1)) + 3))
                               : ((payload)3 / (i + 1));
    }
}

// Sets values of `C2` to my variant. `C2` or `Cij` can be
// used interchangeably.
void mpi_calc_Cij(paylaod_header_t *Cij) {
    for (int i = 0; i < Cij->h; i++) {
        for (int j = 0; j < Cij->w; j++) {
            // Uses `+ 2` to starting counting from 1 to
            // prevent from dividing by zero
            Cij->ptr[i * Cij->w + j] = (payload)1 / ((i + j + 2) * (i + j + 2));
        }
    }
}

// Calculates values of `y1` for `MpiVars_t` to my variant.
// Requires `A` and `b` to be set!
int mpi_calc_y1(MpiVars_t *vars) {

    const paylaod_header_t left = {vars->A, vars->N, vars->N};
    const paylaod_header_t right = {vars->b, 1, vars->N};
    paylaod_header_t out = {vars->y1, 1, vars->N};
    return payload_matrix_mult(&left, &right, &out);
}

// Calculates values of `y2` for `MpiVars_t` to my variant.
// Requires `A1` and `b1` and `c1` to be set!
int mpi_calc_y2(MpiVars_t *vars) {
    const int N = vars->N;
    // tmp is a vector
    payload *tmp = (payload_matrix)malloc((sizeof(*tmp) * vars->N));

    // w = 1 and h = N means that varible colum-base vector
    paylaod_header_t left = {vars->b1, 1, N};
    // w = 1 and h = 1 means that varible is scalar
    payload p = 3;
    paylaod_header_t alpha = {&p, 1, 1};
    // using `y2` as one more additional tmp varible
    paylaod_header_t beta = {vars->y2, 1, N};

    int res = payload_scalar_mult(&left, &alpha, &beta);
    if (res < 0) {
        free(tmp);
        return res;
    }
    left.ptr = vars->c1;
    alpha.ptr = tmp;
    alpha.h = N;

    res = payload_vec_add(&left, &beta, &alpha);
    if (res < 0) {
        free(tmp);
        return res;
    }

    left.ptr = vars->A1;
    left.w = N;
    left.h = N;
    // beta still points to `y2`
    res = payload_matrix_mult(&left, &alpha, &beta);
    free(tmp);
    if (res < 0)
        return res;

    return 0;
}

// Calculates values of `Y3` for `MpiVars_t` to my variant.
// Requires `B2` and `C2` and `A2` to be set!
int mpi_calc_Y3(MpiVars_t *vars) {
    const int N = vars->N;
    // tmp is a matrix
    payload *tmp = (payload_matrix)malloc((sizeof(*tmp) * vars->N) * vars->N);

    paylaod_header_t left = {vars->B2, N, N};
    paylaod_header_t acc = {vars->C2, N, N};
    paylaod_header_t tmp_header = {tmp, N, N};

    int res = payload_matrix_sub(&left, &acc, &tmp_header);

    if (res < 0) {
        free(tmp);
        return res;
    }
    left.ptr = vars->A2;
    acc.ptr = vars->Y3;
    res = payload_matrix_mult(&left, &tmp_header, &acc);
    free(tmp);
    if (res < 0)
        return res;

    return 0;
}

#endif // _MPI_CALCULATIONS_