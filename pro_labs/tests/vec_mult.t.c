#include "payload/mpi_payload.h"
#include <assert.h>

// ONLY for comp-time arrays
#define ARRLEN(x) (sizeof(x) / sizeof(*x))

int main() {
    const payload vec1[] = {1.21, 7.12, 4.161};
    const int vl = (const int)ARRLEN(vec1);
    const payload vec2[] = {2, -1, 112};
    const payload expected[] = {2.42, -7.12, 466.032};
    const payload out[ARRLEN(vec1)];
    assert(ARRLEN(vec1) == ARRLEN(vec2));
    assert(ARRLEN(vec1) == ARRLEN(out));
    assert(ARRLEN(vec1) == ARRLEN(expected));
    // specifies `(payload *)` only to remove compiler warnings
    paylaod_header_t vec1h = {(payload *)vec1, vl, 1};
    paylaod_header_t vec2h = {(payload *)vec2, vl, 1};
    paylaod_header_t outh = {(payload *)out, vl, 1};
    int res = payload_vec_mult(&vec1h, &vec2h, &outh);
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