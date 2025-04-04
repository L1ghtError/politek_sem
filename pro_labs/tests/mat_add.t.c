#include "payload/mpi_payload.h"
#include <assert.h>

// ONLY for comp-time arrays
#define ARRLEN(x) (sizeof(x) / sizeof(*x))

int main() {
// should be constexpr size_t, but this is C
#define w 4
#define h 4
    const payload mat1[w * h] = {1, 2, 3, 4, 1, 2, 3, 4,
                                 1, 2, 3, 4, 1, 2, 3, 4};
    const int vl = (int)ARRLEN(mat1);

    // 4 x 4
    const payload mat2[] = {4, 3, 2, 1, 4, 3, 2, 1, 4, 3, 2, 1, 4, 3, 2, 1};

    const payload expected[] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
    const payload out[ARRLEN(mat1)];
    assert(ARRLEN(mat1) == ARRLEN(mat2));
    assert(ARRLEN(mat1) == ARRLEN(out));
    assert(ARRLEN(mat1) == ARRLEN(expected));

    // specifies `(payload *)` only to remove compiler warnings
    paylaod_header_t mat1h = {(payload *)mat1, w, h};
    paylaod_header_t mat2h = {(payload *)mat2, w, h};
    paylaod_header_t outh = {(payload *)out, w, h};
    int res = payload_matrix_add(&mat1h, &mat2h, &outh);

    if (res != 0) {
        return res;
    }
    int is_failed = 0;
    // floating point operations doesnt guaranteed
    // exact precision, so we need to have some
    // acceptable level of inaccuracy
    double tolerance = 0.05;
    for (int i = 0; i < vl; i++) {
        // out[i] != expected[i] is not acceptable in case
        // of float-point varibles
        if (paylaod_abs(out[i] - expected[i]) > tolerance) {
            is_failed = 1;
            break;
        }
    }
    return is_failed;
}