#include "payload/mpi_payload.h"
#include <assert.h>

// ONLY for comp-time arrays
#define ARRLEN(x) (sizeof(x) / sizeof(*x))

int main() {
// should be constexpr size_t, but this is C
#define w 4
#define h 4
    const payload mat[w * h] = {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4};
    const int vl = h;

    const payload vec[] = {4, 3, 2, 1};

    const payload expected[] = {20, 20, 20, 20};
    const payload out[h];
    assert(h == ARRLEN(vec));

    assert(ARRLEN(vec) == ARRLEN(out));
    assert(ARRLEN(vec) == ARRLEN(expected));

    // specifies `(payload *)` only to remove compiler warnings
    paylaod_header_t math = {(payload *)mat, w, h};
    // decides is it row or column based vector
    paylaod_header_t vech = {(payload *)vec, 1, h};
    paylaod_header_t outh = {(payload *)out, 1, h};
    int res = payload_matrix_mult(&math, &vech, &outh);
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