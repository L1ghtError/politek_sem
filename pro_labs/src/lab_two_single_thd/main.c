#include "stdio.h"

#include "mpi_calculations.h"
#include "payload/mpi_payload.h"
#include "vars/mpi_var_input_factory.h"
#include "vars/mpi_var_print.h"
#include "vars/mpi_vars.h"

#define ENABLE_INTERMIDIATE_LOGS 0
#define DUMP_TO_FILE 1
int main() {
    const int enable_logs = ENABLE_INTERMIDIATE_LOGS;
    MpiVars_t *var = NewMpiVars(DEFAULT_DIMS);
    MpiVars_Fill(var, MPI_INPUT_LINEAR);
    int is_file = DUMP_TO_FILE;
    FILE *fd = stdout;
    if (is_file) {
        fd = fopen("single_thread_dump.txt", "w");
        if (fd == NULL) {
            return -1;
        }
    }

    // inits `bi` also known as `b`
    paylaod_header_t b_header = {var->b, 1, var->N};
    mpi_calc_b(&b_header);

    // inits y1 and y2
    mpi_calc_y1(var);
    mpi_calc_y2(var);

    // inits `Cij` also known as `C2`
    paylaod_header_t C2_header = {var->C2, var->N, var->N};
    mpi_calc_Cij(&C2_header);
    mpi_calc_Y3(var);
    if (enable_logs) {
        fprintf(fd, "--- Vars initited as:\n");
        MpiVars_Print(fd, var);
        fprintf(fd, "------ Strating computation:\n");
    }
    // just set a of temporary varibles with fancy naming
    paylaod_header_t *alpha_v = new_payload(1, var->N);
    paylaod_header_t *theta_v = new_payload(1, var->N);
    paylaod_header_t *beta_m = new_payload(var->N, var->N);
    paylaod_header_t *gamma_m = new_payload(var->N, var->N);
    paylaod_header_t *delta_m = new_payload(var->N, var->N);
    paylaod_header_t *epsilon_m = new_payload(var->N, var->N);
    // wrappers for `y2` and `Y3`
    paylaod_header_t y2_header = {var->y2, 1, var->N};
    paylaod_header_t Y3_header = {var->Y3, var->N, var->N};
    // `Y3 * y2`
    int res = payload_matrix_mult(&Y3_header, &y2_header, alpha_v);
    if (res != 0) {
        return res;
    }
    if (enable_logs)
        fd_print_header_ex(fd, "(Y3 * y2)", alpha_v);
    // this is y2`
    paylaod_header_t y2t_header = {var->y2, var->N, 1};
    // (Y3 * y2) * y2`
    res = payload_vec_mult(alpha_v, &y2t_header, beta_m);
    if (res != 0) {
        return res;
    }
    if (enable_logs)
        fd_print_header_ex(fd, "(Y3 * y2) * y2`", beta_m);
    // Y3^2
    res = payload_matrix_mult(&Y3_header, &Y3_header, gamma_m);
    if (res != 0) {
        return res;
    }
    if (enable_logs)
        fd_print_header_ex(fd, "Y3^2", gamma_m);
    // (Y3^2) * y1
    alpha_v->ptr = var->y1;
    res = payload_matrix_mult(gamma_m, alpha_v, theta_v);
    if (res != 0) {
        return res;
    }
    if (enable_logs)
        fd_print_header_ex(fd, "(Y3^2) * y1", theta_v);
    // (Y3^2 * y1) * y1`
    // vector transposition doesnt changes internal data of `y1`
    payload_transposition(alpha_v);
    res = payload_vec_mult(theta_v, alpha_v, delta_m);
    if (res != 0) {
        return res;
    }
    if (enable_logs)
        fd_print_header_ex(fd, "(Y3^2 * y1) * y1`", delta_m);
    // Y3^3
    res = payload_matrix_mult(gamma_m, &Y3_header, epsilon_m);
    if (res != 0) {
        return res;
    }
    if (enable_logs)
        fd_print_header_ex(fd, "Y3^3", epsilon_m);
    //   (Y3 * y2 * y2`) + (Y3^3)
    res = payload_matrix_add(beta_m, epsilon_m, gamma_m);
    if (res != 0) {
        return res;
    }
    if (enable_logs)
        fd_print_header_ex(fd, "(Y3 * y2 * y2`) + (Y3^3)", gamma_m);
    //   (Y3 * y2 * y2` + Y3^3) - Y3
    res = payload_matrix_sub(gamma_m, &Y3_header, beta_m);
    if (res != 0) {
        return res;
    }
    if (enable_logs)
        fd_print_header_ex(fd, "(Y3 * y2 * y2` + Y3^3) - Y3", beta_m);
    // y2 * y1`
    res = payload_vec_mult(&y2_header, alpha_v, gamma_m);
    if (res != 0) {
        return res;
    }
    if (enable_logs)
        fd_print_header_ex(fd, "y2 * y1`", gamma_m);
    //   (Y3 * y2 * y2` + Y3^3 - Y3) + (y2 * y1`)
    res = payload_matrix_add(gamma_m, beta_m, epsilon_m);
    if (res != 0) {
        return res;
    }
    if (enable_logs)
        fd_print_header_ex(fd, "(Y3 * y2 * y2` + Y3^3 - Y3) + (y2 * y1`)",
                           beta_m);
    //   (Y3 * y2 * y2` + Y3^3 - Y3 + y2 * y1`) +  (Y3^2 * y1 * y1`)
    res = payload_matrix_add(epsilon_m, delta_m, gamma_m);
    if (res != 0) {
        return res;
    }

    fd_print_header_ex(
        fd, "(Y3 * y2 * y2` + Y3^3 - Y3 + y2 * y1`) +  (Y3^2 * y1 * y1`):",
        gamma_m);

    if (is_file) {
        fclose(fd);
    }
    return 0;
}