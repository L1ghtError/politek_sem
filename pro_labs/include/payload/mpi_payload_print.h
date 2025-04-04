#ifndef _MPI_PAYLOAD_PRINT_
#define _MPI_PAYLOAD_PRINT_
#include "mpi_payload.h"
#include "stdio.h" // printf

#define GREEN_HIGHLIGHT "\033[32m"
#define END_HIGHLIGHT "\033[0m"

// Prints values and coordinates of payload container
void print_element(FILE *fd, const payload *container, int w, int h) {
    const char *format = fd == stdout ? GREEN_HIGHLIGHT
                             "Val[%d][%d]" END_HIGHLIGHT ":%.3f "
                                      : "Val[%d][%d] :%.3f ";
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            fprintf(fd, format, i, j, container[i * w + j]);
        }
        fprintf(fd, "\n");
    }
}
// Prints values and coordinates of payload container
// also takes title that will be printed before values and newline after
void _print_element_ex(FILE *fd, const char *title, const payload *container,
                       int w, int h) {
    fprintf(fd, "%s\n", title);
    print_element(fd, container, w, h);
    fprintf(fd, "\n");
    fflush(fd);
}

// Prints values and coordinates of payload container
// also takes title that will be printed before values and newline after
void print_element_ex(const char *title, const payload *container, int w,
                      int h) {
    _print_element_ex(stdout, title, container, w, h);
}

// Prints values and coordinates of paylaod_header_t
void print_header(const paylaod_header_t *header) {
    print_element(stdout, header->ptr, header->w, header->h);
    fflush(stdout);
}

void print_header_ex(const char *title, const paylaod_header_t *header) {
    _print_element_ex(stdout, title, header->ptr, header->w, header->h);
}

void fd_print_header(FILE *fd, const paylaod_header_t *header) {
    print_element(fd, header->ptr, header->w, header->h);
}

void fd_print_header_ex(FILE *fd, const char *title,
                        const paylaod_header_t *header) {
    _print_element_ex(fd, title, header->ptr, header->w, header->h);
}

void fd_print_element_ex(FILE *fd, const char *title, const payload *container,
                         int w, int h) {
    _print_element_ex(fd, title, container, w, h);
}

#endif // _MPI_PAYLOAD_PRINT_