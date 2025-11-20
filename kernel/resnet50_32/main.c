#ifdef SPIKEGEM
#include <stdio.h>
#define NR_LANES 8
#else
#include "printf.h"
#endif

#include <stdint.h>
#include "runtime.h"
#include "util.h"
#include "iconv2d.h"
#include "relu.h"
#include "max_pool.h"
#include "fc.h"
#include "bn.h"
#include "avg_pool.h"

extern int32_t input_image[];
extern int32_t filter_3x3[];
extern int32_t fc_weights[];
extern int32_t fc_bias[];

#define IN_H 16
#define IN_W 16

#define CONV_F 3
#define CONV1_OUT_H (IN_H - 2)
#define CONV1_OUT_W (IN_W - 2)

#define POOL1_OUT_H (CONV1_OUT_H / 2)
#define POOL1_OUT_W (CONV1_OUT_W / 2)

#define CONV2_OUT_H (POOL1_OUT_H - 2)
#define CONV2_OUT_W (POOL1_OUT_W - 2)

#define POOL2_OUT_H (CONV2_OUT_H / 2)
#define POOL2_OUT_W (CONV2_OUT_W / 2)

#define FLAT_SIZE   (POOL2_OUT_H * POOL2_OUT_W)
#define FC_OUT      10

static inline uint64_t read_minstret(void) {
    uint64_t value;
    asm volatile ("csrr %0, instret" : "=r"(value));
    return value;
}

void print_matrix_int(const int32_t *m, int H, int W, const char *name) {
    printf("\n--- %s (%dx%d) ---\n", name, H, W);
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++)
            printf("%4d ", m[r*W + c]);
        printf("\n");
    }
}

void print_vector_int(const int32_t *v, int N, const char *name) {
    printf("\n--- %s (%d) ---\n", name, N);
    for (int i = 0; i < N; i++)
        printf("%d ", v[i]);
    printf("\n");
}

int main() {

    static int32_t conv1_out[CONV1_OUT_H * CONV1_OUT_W] __attribute__((aligned(32*NR_LANES)));

    static int32_t pool1_out[POOL1_OUT_H * POOL1_OUT_W] __attribute__((aligned(32*NR_LANES)));

    static int32_t conv2_out[CONV2_OUT_H * CONV2_OUT_W] __attribute__((aligned(32*NR_LANES)));

    static int32_t pool2_out[POOL2_OUT_H * POOL2_OUT_W] __attribute__((aligned(32*NR_LANES)));
    static int32_t fc_out[FC_OUT] __attribute__((aligned(32*NR_LANES)));


    const int32_t bn_mean  = 0;
    const int32_t bn_invstd = 1;
    const int32_t bn_gamma = 1;
    const int32_t bn_beta  = 0;

    printf("Running CONV → BN → ReLU → MAXPOOL → CONV → BN → ReLU → AVGPOOL → FC\n");

#ifdef SPIKEGEM
    uint64_t start_minstret = read_minstret();
#endif
    start_timer();

    printf("\nRunning CONV1 3x3...\n");
    iconv2d_3x3(conv1_out, input_image, filter_3x3,
                CONV1_OUT_H, CONV1_OUT_W, CONV_F);
    
    printf("\nRunning Batch...\n");
    ibatchnorm_2d_int32(conv1_out, conv1_out, bn_mean, bn_invstd,
                        bn_gamma, bn_beta, CONV1_OUT_H, CONV1_OUT_W);
    
    printf("\nRunning ReLU...\n");
    relu(conv1_out, CONV1_OUT_H * CONV1_OUT_W);

    printf("\nRunning MAXPOOL...\n");
    maxpool2x2(conv1_out, CONV1_OUT_H, CONV1_OUT_W, pool1_out);


    printf("\nRunning CONV2 3x3...\n");
    iconv2d_3x3(conv2_out, pool1_out, filter_3x3,
                CONV2_OUT_H, CONV2_OUT_W, CONV_F);

    printf("\nRunning Batch...\n");
    ibatchnorm_2d_int32(conv2_out, conv2_out, bn_mean, bn_invstd,
                        bn_gamma, bn_beta, CONV2_OUT_H, CONV2_OUT_W);

    printf("\nRunning ReLU...\n");
    relu(conv2_out, CONV2_OUT_H * CONV2_OUT_W);

    printf("\nRunning AVGPOOL 2x2...\n");
    avgpool2x2(conv2_out, CONV2_OUT_H, CONV2_OUT_W, pool2_out);

    
    printf("\nRunning FC layer...\n");
    fc(pool2_out, fc_weights, POOL2_OUT_H, POOL2_OUT_W, fc_out);

    add_bias_rvv(fc_out, fc_bias, FC_OUT);
    stop_timer();

#ifdef SPIKEGEM
    uint64_t end_minstret = read_minstret();
    printf("Instructions retired: %lu\n", end_minstret - start_minstret);
#endif

    printf("Execution: %d cycles.\n", get_timer());

    return 0;
}
