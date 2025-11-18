#ifdef SPIKEGEM
#include <stdio.h>
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
#include "softmax.h"


extern int32_t input_image[];
extern int32_t filter_3x3[];
extern int32_t fc_weights[];
extern int32_t fc_bias[];

#define IN_H 16
#define IN_W 16

#define CONV_F 3
#define CONV_OUT_H (IN_H - 2)
#define CONV_OUT_W (IN_W - 2)

#define POOL_OUT_H (CONV_OUT_H / 2)
#define POOL_OUT_W (CONV_OUT_W / 2)

#define FLAT_SIZE (POOL_OUT_H * POOL_OUT_W)
#define FC_OUT 10

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

void print_vector_float(const float *v, int N, const char *name) {
    printf("\n--- %s (%d) ---\n", name, N);
    for (int i = 0; i < N; i++)
        printf("%.4f ", v[i]);
    printf("\n");
}


int main() {

    // ====== BUFFER LAYER ======
    static int32_t conv_out[CONV_OUT_H * CONV_OUT_W];
    static int32_t pool_out[POOL_OUT_H * POOL_OUT_W];
    static int32_t fc_out[FC_OUT];
    static float softmax_out[FC_OUT];


    // ========================================================
    // 1) CONV 3×3
    // ========================================================
    printf("Running CONV 3x3...\n");
    #ifdef SPIKEGEM
    uint64_t start_minstret = read_minstret();
    #endif
    start_timer();
    iconv2d_3x3(conv_out,
                input_image,
                filter_3x3,
                CONV_OUT_H, CONV_OUT_W, CONV_F);

    //print_matrix_int(conv_out, CONV_OUT_H, CONV_OUT_W, "Conv Output");


    // ========================================================
    // 2) ReLU
    // ========================================================
    printf("\nRunning ReLU...\n");

    relu(conv_out, CONV_OUT_H * CONV_OUT_W);

    //print_matrix_int(conv_out, CONV_OUT_H, CONV_OUT_W, "ReLU Output");


    // ========================================================
    // 3) MaxPool 2×2
    // ========================================================
    printf("\nRunning MaxPool...\n");

    maxpool2x2(conv_out, CONV_OUT_H, CONV_OUT_W, pool_out);

    //print_matrix_int(pool_out, POOL_OUT_H, POOL_OUT_W, "MaxPool Output");


    // ========================================================
    // 4) Fully Connected
    // ========================================================
    printf("\nRunning FC layer...\n");

    fc(pool_out, fc_weights, POOL_OUT_H, POOL_OUT_W, fc_out);

    //print_vector_int(fc_out, FC_OUT, "FC Output (logits int32)");


    // ========================================================
    // 5) Convert to float for softmax
    // ========================================================
    float fc_out_f[FC_OUT];
    for (int i = 0; i < FC_OUT; i++)
        fc_out_f[i] = (float) fc_out[i];


    // ========================================================
    // 6) Softmax
    // ========================================================
    printf("\nRunning Softmax...\n");

    softmax_rvv(fc_out_f, softmax_out, FC_OUT);
    stop_timer();
    #ifdef SPIKEGEM
    uint64_t end_minstret = read_minstret();
    uint64_t delta_minstret = end_minstret - start_minstret;
    #endif
    //print_vector_float(softmax_out, FC_OUT, "Softmax Output");
    int64_t runtime = get_timer();
    #ifdef SPIKEGEM
    printf("Instructions retired (CSR minstret): %lu\n", delta_minstret);
    #endif
    printf("The execution took %d cycles.\n", runtime);

    return 0;
}

