#ifndef _MPI_PAYLOAD_
#define _MPI_PAYLOAD_
typedef double payload;
#include "math.h"
#include "stdlib.h" // malloc
inline payload paylaod_abs(payload x) { return x >= 0 ? x : -x; }

typedef payload *payload_matrix;
typedef payload *payload_vec_col;
typedef payload *payload_vec_row;

struct PayloadHeader {
    payload *ptr;
    int w;
    int h;
};
typedef struct PayloadHeader paylaod_header_t;

inline paylaod_header_t *new_payload(int w, int h) {
    const int struct_size = sizeof(paylaod_header_t);
    paylaod_header_t *ptr =
        (paylaod_header_t *)malloc(struct_size + (sizeof(payload) * w * h));
    ptr->w = w;
    ptr->h = h;
    ptr->ptr = (payload *)(ptr + 1);
    return ptr;
}

inline void destroy_payload(paylaod_header_t *payloadh) { free(payloadh); }
// Provides Transposition for matrix,vectors  and for scalars as well :)
inline int payload_transposition(paylaod_header_t *header) {
    const int w = header->w;
    const int h = header->h;
    if (w < 1 || h < 1)
        return -1;

    // scalar
    if (w == 1 && h == 1)
        return 0;
    // vector
    int tmp = h;
    if ((w == 1 && h > 1) || (w > 1 && h == 1)) {
        header->h = w;
        header->w = tmp;
        return 0;
    }
    payload *vals = header->ptr;
    // square matrix
    if (w == h) {
        for (int i = 0; i < h; i++) {
            const int jt = w - (w - i);
            for (int j = jt; j < w; j++) {
                const int major = (i * w) + j;
                const int minor = (j * w) + i;

                double tmp = vals[minor];
                vals[minor] = vals[major];
                vals[major] = tmp;
            }
        }
    } else {
        // This project is not assumes to work with NxM matrices
        return -1;
    }
    return 0;
}
// Provides mat x scalar, vec x scalar multiplicatoin
inline int payload_scalar_mult(const paylaod_header_t *mat,
                               const paylaod_header_t *scalar,
                               paylaod_header_t *out) {
    if (mat->w != out->w || mat->h != out->h)
        return -1;

    const payload *_mat = mat->ptr;
    const payload *_scalar = scalar->ptr;
    payload *_out = out->ptr;

    for (int i = 0; i < mat->h; i++) {
        for (int j = 0; j < mat->w; j++) {
            _out[i * out->w + j] = _mat[i * out->w + j] * (*_scalar);
        }
    }
    return 0;
}
// Generic functoin to validate Providesd vectors
// return length on sucess
// return -1 on failure
inline int _payload_vec_validate_len(const paylaod_header_t *left,
                                     const paylaod_header_t *right,
                                     const paylaod_header_t *out) {
    int left_len = left->h > 1 ? left->h : left->w;
    int right_len = right->h > 1 ? right->h : right->w;
    int out_len = out->h > 1 ? out->h : out->w;

    if (left_len != right_len || left_len != out_len || left_len < 1)
        return -1;
    return left_len;
}

// Provides vec x vec multiplicatoin
inline int payload_vec_mult(const paylaod_header_t *left,
                            const paylaod_header_t *right,
                            paylaod_header_t *out) {
    const payload *_left = left->ptr;
    const payload *_right = right->ptr;
    payload *_out = out->ptr;

    if (left->w == right->w && left->h == right->h && left->w == out->w &&
        left->h == out->h) {
        const int len = left->w == 1 ? left->h : left->w;
        if (len < 2) {
            return -1;
        }
        for (int i = 0; i < len; i++) {
            _out[i] = _left[i] * _right[i];
        }
        return 0;
        // column x row multiplication
    } else if (left->w == 1 && right->h == 1 && left->h == right->w &&
               left->h > 0 && out->h == left->h && out->w == right->w) {
        for (int i = 0; i < out->h; i++) {
            for (int j = 0; j < out->w; j++) {
                _out[(i * out->w) + j] =
                    _left[i % left->h] * _right[j % right->w];
            }
        }
        return 0;
        // row x column multiplication
    } else if (left->h == 1 && right->w == 1 && left->w == right->h &&
               left->w > 0 && out->w == 1 && out->h == 1) {
        *out->ptr = 0;
        for (int i = 0; i < left->w; i++) {
            *out->ptr += _left[i % left->w] * _right[i % right->h];
        }
        return 0;
    }
    return -1;
}

// Provides vec x vec addition
inline int payload_vec_add(const paylaod_header_t *left,
                           const paylaod_header_t *right,
                           paylaod_header_t *out) {
    const int len = _payload_vec_validate_len(left, right, out);
    if (len < 0)
        return len;

    for (int i = 0; i < len; i++) {
        out->ptr[i] = left->ptr[i] + right->ptr[i];
    }
    return 0;
}

// Provides vec x vec addition
inline int payload_vec_sub(const paylaod_header_t *left,
                           const paylaod_header_t *right,
                           paylaod_header_t *out) {
    const int len = _payload_vec_validate_len(left, right, out);
    if (len < 0)
        return len;

    for (int i = 0; i < len; i++) {
        out->ptr[i] = left->ptr[i] - right->ptr[i];
    }
    return 0;
}

// Provides mat x mat, mat x vec multiplicatoin
inline int payload_matrix_mult(const paylaod_header_t *left,
                               const paylaod_header_t *right,
                               paylaod_header_t *out) {
    if (left->w != right->h || left->h != out->h || right->w != out->w)
        return -1;

    const payload *_left = left->ptr;
    const payload *_right = right->ptr;
    payload *_out = out->ptr;
    for (int i = 0; i < left->h; i++) {
        for (int j = 0; j < right->w; j++) {
            _out[i * out->w + j] = 0;

            for (int k = 0; k < right->h; k++) {
                _out[i * out->w + j] +=
                    _left[i * left->w + k] * _right[j + (k * right->w)];
            }
        }
    }
    return 0;
}

// Provides  ONLY mat x mat addition
inline int payload_matrix_add(const paylaod_header_t *left,
                              const paylaod_header_t *right,
                              paylaod_header_t *out) {
    if (left->w != right->w || left->h != right->h || left->w != out->w ||
        left->h != out->h)
        return -1;

    const payload *_left = left->ptr;
    const payload *_right = right->ptr;
    payload *_out = out->ptr;

    for (int i = 0; i < left->h; i++) {
        for (int j = 0; j < right->w; j++) {
            _out[(i * out->w) + j] =
                _left[(i * left->w) + j] + _right[(i * right->w) + j];
        }
    }
    return 0;
}

// Provides  ONLY mat x mat subtraction
// The only difference between `payload_matrix_add` and `payload_matrix_sub`
// is sign `+` or `-`
inline int payload_matrix_sub(const paylaod_header_t *left,
                              const paylaod_header_t *right,
                              paylaod_header_t *out) {
    if (left->w != right->w || left->h != right->h || left->w != out->w ||
        left->h != out->h)
        return -1;

    const payload *_left = left->ptr;
    const payload *_right = right->ptr;
    payload *_out = out->ptr;

    for (int i = 0; i < left->h; i++) {
        for (int j = 0; j < right->w; j++) {
            _out[(i * out->w) + j] =
                _left[(i * left->w) + j] - _right[(i * right->w) + j];
        }
    }
    return 0;
}

#endif // _MPI_PAYLOAD_