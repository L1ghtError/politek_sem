#include "mpi.h"
#include "mpi_calculations.h"
#include "payload/mpi_payload.h"
#include "stdio.h"
#include "string.h" // strcmp
#include "vars/mpi_var_input_factory.h"
#include "vars/mpi_var_print.h"
#include "vars/mpi_vars.h"
volatile int log_lvl = -1;

// logs with this level will be hidden if `log_lvl` is greater than 1
#define LOG_LVL_INFO 1
// same as `LOG_LVL_INFO` but for `log_lvl` > 10
#define LOG_LVL_CRIT 10

int check_op(int argc, char *argv[], const char *flag) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], flag) == 0) {
            return 1;
        }
    }
    return 0;
}

void log_header(int lvl, FILE *fd, const char *title,
                const paylaod_header_t *header) {
    if (lvl >= log_lvl)
        fd_print_header_ex(fd, title, header);
}

int mpi_crash(FILE *fd, char const *const reason) {
    fprintf(fd, reason);
    fflush(fd);
    MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
    return -1;
}

// payload type
#define MPI_PLD_TYPE MPI_LONG_DOUBLE

// Tags
#define tag_y1 1
#define tag_Y3 2
#define tag_y2 3
// y2 * y1`
#define tag_y2muly1t 4
// (Y3 * y2) * y2`
#define tag_Y3muly2muly2t 5
// Y3^3
#define tag_Y3pow3subY3 6

// Program options
struct prog_ops {
    int payload_size; // size of matrices or vectors
    int is_file_dump; // print to console or to file ?
    int is_verbose;   // print intermidiate states ?
    mpi_input_type_t input_type; // Linear, Manual, Random
    FILE *dump_fd; // exact place where to store calculations
};
typedef struct prog_ops prog_ops_t;

void PROC_0(prog_ops_t ops) {
    MPI_Request Y3_proc1_req;
    MPI_Request Y3_proc2_req;
    MPI_Status status;
    MpiVars_t *var = NewMpiVars(ops.payload_size);
    paylaod_header_t *tmp_mat = new_payload(var->N, var->N);
    paylaod_header_t *tmp_mat_1 = new_payload(var->N, var->N);
    paylaod_header_t *tmp_mat_2 = new_payload(var->N, var->N);
    paylaod_header_t *tmp_mat_3 = new_payload(var->N, var->N);
    paylaod_header_t *tmp_mat_4 = new_payload(var->N, var->N);
    paylaod_header_t *tmp_vec = new_payload(1, var->N);

    MpiVars_Fill(var, ops.input_type);
    MPI_Bcast(var, sizeofMpiVars(var), MPI_CHAR, 0, MPI_COMM_WORLD);

    // inits `Cij` also known as `C2`
    paylaod_header_t C2_header = {var->C2, var->N, var->N};
    mpi_calc_Cij(&C2_header);
    // inits `Y3`
    mpi_calc_Y3(var);
    MPI_Isend(var->Y3, var->N * var->N, MPI_PLD_TYPE, 1, tag_Y3, MPI_COMM_WORLD,
              &Y3_proc1_req);
    MPI_Isend(var->Y3, var->N * var->N, MPI_PLD_TYPE, 2, tag_Y3, MPI_COMM_WORLD,
              &Y3_proc2_req);
    // TODO: investigate or remove
    // MPI_Send(tmp_mat->ptr, tmp_mat->h * tmp_mat->w, MPI_PLD_TYPE, 0,
    //          tag_Y3muly2muly2t, MPI_COMM_WORLD);
    paylaod_header_t Y3_header = {var->Y3, var->N, var->N};

    // Y3^2
    int res = payload_matrix_mult(&Y3_header, &Y3_header, tmp_mat);
    if (res != 0) {
        mpi_crash(ops.dump_fd, "Y3^2 failed");
    }
    log_header(LOG_LVL_INFO, ops.dump_fd, "Y3^2", tmp_mat);

    MPI_Recv(var->y1, var->N, MPI_PLD_TYPE, 2, tag_y1, MPI_COMM_WORLD, &status);
    paylaod_header_t y1_header = {var->y1, 1, var->N};
    // (Y3^2) * y1
    res = payload_matrix_mult(tmp_mat, &y1_header, tmp_vec);
    if (res != 0) {
        mpi_crash(ops.dump_fd, "(Y3^2) * y1 failed");
    }
    log_header(LOG_LVL_INFO, ops.dump_fd, "(Y3^2) * y1", tmp_vec);
    // y1`
    res = payload_transposition(&y1_header);
    if (res != 0) {
        mpi_crash(ops.dump_fd, "y1` failed");
    }
    // (Y3^2 * y1) * y1`
    res = payload_vec_mult(tmp_vec, &y1_header, tmp_mat_2);
    if (res != 0) {
        mpi_crash(ops.dump_fd, "(Y3^2 * y1) * y1` failed");
    }
    log_header(LOG_LVL_INFO, ops.dump_fd, "(Y3^2 * y1) * y1`", tmp_mat_2);

    MPI_Recv(tmp_mat_3->ptr, var->N * var->N, MPI_PLD_TYPE, 1,
             tag_Y3muly2muly2t, MPI_COMM_WORLD, &status);

    MPI_Recv(tmp_mat_4->ptr, var->N * var->N, MPI_PLD_TYPE, 2, tag_Y3pow3subY3,
             MPI_COMM_WORLD, &status);

    //   (Y3 * y2 * y2`) + (Y3^3 - Y3)
    res = payload_matrix_add(tmp_mat_3, tmp_mat_4, tmp_mat_1);
    if (res != 0) {
        mpi_crash(ops.dump_fd, "(Y3 * y2 * y2`) + (Y3^3 - Y3) failed");
    }
    log_header(LOG_LVL_INFO, ops.dump_fd, "(Y3 * y2 * y2`) + (Y3^3 - Y3)",
               tmp_mat);

    MPI_Recv(tmp_mat_4->ptr, var->N * var->N, MPI_PLD_TYPE, 2, tag_y2muly1t,
             MPI_COMM_WORLD, &status);

    //   (Y3 * y2 * y2` + Y3^3 - Y3) + (y2 * y1`)
    res = payload_matrix_add(tmp_mat_1, tmp_mat_4, tmp_mat);
    if (res != 0) {
        mpi_crash(ops.dump_fd,
                  "(Y3 * y2 * y2` + Y3^3 - Y3) + (y2 * y1`) failed");
    }
    log_header(LOG_LVL_INFO, ops.dump_fd,
               "(Y3 * y2 * y2` + Y3^3 - Y3) + (y2 * y1`)", tmp_mat);
    //   (Y3 * y2 * y2` + Y3^3 - Y3 + y2 * y1`) +  (Y3^2 * y1 * y1`)
    res = payload_matrix_add(tmp_mat, tmp_mat_2, tmp_mat_3);
    if (res != 0) {
        mpi_crash(ops.dump_fd, "(Y3 * y2 * y2` + Y3^3 - Y3 + y2 * y1`) +  "
                               "(Y3^2 * y1 * y1`) failed");
    }
    log_header(
        LOG_LVL_CRIT, ops.dump_fd,
        "x:\n(Y3 * y2 * y2` + Y3^3 - Y3 + y2 * y1`) +  (Y3^2 * y1 * y1`):",
        tmp_mat_3);
    MPI_Wait(&Y3_proc1_req, MPI_STATUS_IGNORE);
    MPI_Wait(&Y3_proc2_req, MPI_STATUS_IGNORE);
    return;
}
void PROC_1(prog_ops_t ops) {
    MPI_Status status;
    MPI_Request y2_req;

    MpiVars_t *var = NewMpiVars(ops.payload_size);
    paylaod_header_t *tmp_vec = new_payload(1, var->N);
    paylaod_header_t *tmp_mat = new_payload(var->N, var->N);
    MPI_Bcast(var, sizeofMpiVars(var), MPI_CHAR, 0, MPI_COMM_WORLD);
    UpdateAddresses(var);
    mpi_calc_y2(var);
    MPI_Isend(var->y2, var->N, MPI_PLD_TYPE, 2, tag_y2, MPI_COMM_WORLD,
              &y2_req);

    MPI_Recv(var->Y3, var->N * var->N, MPI_PLD_TYPE, 0, tag_Y3, MPI_COMM_WORLD,
             &status);
    paylaod_header_t Y3_header = {var->Y3, var->N, var->N};
    paylaod_header_t y2_header = {var->y2, 1, var->N};
    // Y3 * y2
    int res = payload_matrix_mult(&Y3_header, &y2_header, tmp_vec);
    if (res != 0) {
        mpi_crash(ops.dump_fd, "Y3 * y2 failed");
    }
    log_header(LOG_LVL_INFO, ops.dump_fd, "(Y3 * y2)", tmp_vec);
    res = payload_transposition(&y2_header);
    if (res != 0) {
        mpi_crash(ops.dump_fd, "y2` failed");
    }
    // (Y3 * y2) * y2`
    res = payload_vec_mult(tmp_vec, &y2_header, tmp_mat);
    if (res != 0) {
        mpi_crash(ops.dump_fd, "(Y3 * y2) * y2` failed");
    }
    log_header(LOG_LVL_INFO, ops.dump_fd, "(Y3 * y2) * y2`", tmp_mat);
    MPI_Send(tmp_mat->ptr, tmp_mat->h * tmp_mat->w, MPI_PLD_TYPE, 0,
             tag_Y3muly2muly2t, MPI_COMM_WORLD);

    MPI_Wait(&y2_req, MPI_STATUS_IGNORE);
    return;
}
void PROC_2(prog_ops_t ops) {
    MPI_Status status;
    MPI_Request Y3pow3subY3_req;

    MpiVars_t *var = NewMpiVars(ops.payload_size);
    paylaod_header_t *tmp_mat = new_payload(var->N, var->N);
    paylaod_header_t *tmp_mat_2 = new_payload(var->N, var->N);

    MPI_Bcast(var, sizeofMpiVars(var), MPI_CHAR, 0, MPI_COMM_WORLD);
    UpdateAddresses(var);

    // inits `bi` also known as `b`
    paylaod_header_t b_header = {var->b, 1, var->N};
    mpi_calc_b(&b_header);
    mpi_calc_y1(var);
    MPI_Send(var->y1, var->N, MPI_PLD_TYPE, 0, tag_y1, MPI_COMM_WORLD);
    // even though 0 pid will calculete Y3^2 we will also do that
    MPI_Recv(var->Y3, var->N * var->N, MPI_PLD_TYPE, 0, tag_Y3, MPI_COMM_WORLD,
             &status);
    paylaod_header_t Y3_header = {var->Y3, var->N, var->N};

    // Y3^2
    int res = payload_matrix_mult(&Y3_header, &Y3_header, tmp_mat);
    if (res != 0) {
        mpi_crash(ops.dump_fd, "Y3^2 failed");
    }
    log_header(LOG_LVL_INFO, ops.dump_fd, "Y3^2", tmp_mat);
    // Y3^3
    res = payload_matrix_mult(tmp_mat, &Y3_header, tmp_mat_2);
    if (res != 0) {
        mpi_crash(ops.dump_fd, "Y3^3 failed");
    }
    log_header(LOG_LVL_INFO, ops.dump_fd, "Y3^3", tmp_mat_2);
    // (Y3^3) - Y3
    res = payload_matrix_sub(tmp_mat_2, &Y3_header, tmp_mat);
    if (res != 0) {
        mpi_crash(ops.dump_fd, "(Y3^3) - Y3 failed");
    }
    log_header(LOG_LVL_INFO, ops.dump_fd, "(Y3^3) - Y3", tmp_mat);
    MPI_Isend(tmp_mat->ptr, var->N * var->N, MPI_PLD_TYPE, 0, tag_Y3pow3subY3,
              MPI_COMM_WORLD, &Y3pow3subY3_req);

    MPI_Recv(var->y2, var->N, MPI_PLD_TYPE, 1, tag_y2, MPI_COMM_WORLD, &status);
    paylaod_header_t y2_header = {var->y2, 1, var->N};
    paylaod_header_t y1_header = {var->y1, 1, var->N};
    // y`
    res = payload_transposition(&y1_header);
    if (res != 0) {
        mpi_crash(ops.dump_fd, "y` failed");
    }
    // y2 * y1`
    res = payload_vec_mult(&y2_header, &y1_header, tmp_mat);
    if (res != 0) {
        mpi_crash(ops.dump_fd, "y2 * y1` failed");
    }
    log_header(LOG_LVL_INFO, ops.dump_fd, "y2 * y1`", tmp_mat);
    MPI_Send(tmp_mat->ptr, tmp_mat->h * tmp_mat->w, MPI_PLD_TYPE, 0,
             tag_y2muly1t, MPI_COMM_WORLD);

    MPI_Wait(&Y3pow3subY3_req, MPI_STATUS_IGNORE);
    return;
}

int main(int argc, char *argv[]) {
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Barrier(MPI_COMM_WORLD);
    if (size != 3) {
        if (rank == 0) {
            printf("Error: 3 processes required instead of %d, abort\n", size);
            fflush(stdout);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        printf("\n");
        MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
        return -1;
    }

    // skip io, saves only result
    int trunc_flag = check_op(argc, argv, "-trunc");
    prog_ops_t ops = {DEFAULT_DIMS, 1, !trunc_flag, MPI_INPUT_LINEAR};
    // skip user input
    int fast_flag = check_op(argc, argv, "-fast");

    if (rank == 0 && fast_flag == 0) {

        printf("Enter payload size (int): ");
        fflush(stdout);
        scanf("%d", &ops.payload_size);

        printf("Dump to file? (0 = No, 1 = Yes): ");
        fflush(stdout);
        scanf("%d", &ops.is_file_dump);

        printf("Verbose mode? (0 = No, 1 = Yes): ");
        fflush(stdout);
        scanf("%d", &ops.is_verbose);

        printf("Input type (0 = LINEAR, 1 = MANUAL, 2 = RANDOM): ");
        fflush(stdout);
        scanf("%d", &ops.input_type);
    }
    MPI_Bcast(&ops, sizeof(ops), MPI_CHAR, 0, MPI_COMM_WORLD);
    log_lvl = ops.is_verbose == 1 ? 1 : 5;
    ops.dump_fd = stdout;
    if (ops.is_file_dump) {
        char filename[256];
        sprintf(filename, "dump_%d.txt", rank);

        ops.dump_fd = fopen(filename, "w");
        if (ops.dump_fd == NULL) {
            printf("Error: cannot open %s, abort\n", filename);
            fflush(stdout);
            MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
            return -1;
        }
    }
    if (rank == 0)
        PROC_0(ops);
    if (rank == 1)
        PROC_1(ops);
    if (rank == 2)
        PROC_2(ops);

    if (ops.is_file_dump) {
        fclose(ops.dump_fd);
    }
    MPI_Finalize();
    return 0;
}