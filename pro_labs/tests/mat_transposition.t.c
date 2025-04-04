#include "payload/mpi_payload.h"
#include <assert.h>

// ONLY for comp-time arrays
#define ARRLEN(x) (sizeof(x) / sizeof(*x))

int main() {
// should be constexpr, but this is C
#define w 4
#define h 4
    const payload mat[w * h] = {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4};
    const payload expected[] = {1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4};
    const int vl = (int)ARRLEN(mat);
    // specifies `(payload *)` only to remove compiler warnings
    paylaod_header_t math = {(payload *)mat, w, h};
    int res = payload_transposition(&math);

    if (res != 0) {
        return res;
    }
    int is_failed = 0;
    // floating point operations doesnt guaranteed
    // exact precision, so we need to have some
    // acceptable level of inaccuracy
    double tolerance = 0.05;
    for (int i = 0; i < vl; i++) {
        // mat[i] != expected[i] is not acceptable in case
        // of float-point varibles
        if (paylaod_abs(mat[i] - expected[i]) > tolerance) {
            is_failed = 1;
            break;
        }
    }
    return is_failed;
}