#ifdef SPIKEGEM
#include <stdio.h>
#define NR_LANES 8
#else
#include "printf.h"
#endif

#include <stdint.h>
#include "runtime.h"
#include "util.h"
#include "avg_pool_32.h"
#include "iconv2d_32.h"
#include "relu_32.h"
#include "fc_32.h"
#include "depthwise.h"
#include "pointwise_before.h"
#include "pointwise_after.h"
#include "softmax_32.h"



extern int32_t input_image[] __attribute__((aligned(32*NR_LANES)));
extern int32_t conv1_filter_3x3[] __attribute__((aligned(32*NR_LANES)));
extern int32_t pw_before_weights[] __attribute__((aligned(32*NR_LANES)));  
extern int32_t pw_before_bias[] __attribute__((aligned(32*NR_LANES)));     
extern int32_t dw_filters[] __attribute__((aligned(32*NR_LANES)));         
extern int32_t pw_after_weights[] __attribute__((aligned(32*NR_LANES)));   
extern int32_t pw_after_bias[] __attribute__((aligned(32*NR_LANES)));     
extern int32_t fc_weights[] __attribute__((aligned(32*NR_LANES)));         
extern int32_t fc_bias[] __attribute__((aligned(32*NR_LANES)));            

#define IN_H 16
#define IN_W 16

#define CONV1_F      3
#define CONV1_OUT_H  (IN_H - 2)   
#define CONV1_OUT_W  (IN_W - 2)   

#define PW_CH        8            

#define PW1_OUT_H    (CONV1_OUT_H)  
#define PW1_OUT_W    (CONV1_OUT_W)  

#define DW_OUT_H     (PW1_OUT_H - 2) 
#define DW_OUT_W     (PW1_OUT_W - 2) 

#define PW2_OUT_H    (DW_OUT_H)      
#define PW2_OUT_W    (DW_OUT_W)      

#define POOL_OUT_H   (PW2_OUT_H / 2)
#define POOL_OUT_W   (PW2_OUT_W / 2) 

#define FC_IN_ROWS   (PW_CH * POOL_OUT_H)        
#define FC_IN_COLS   (POOL_OUT_W)                 
#define FLAT_SIZE    (FC_IN_ROWS * FC_IN_COLS)    

#define FC_OUT       10


static inline uint64_t read_minstret(void) {
    uint64_t value;
    asm volatile ("csrr %0, instret" : "=r"(value));
    return value;
}


static void print_matrix_int(const int32_t *m, int H, int W, const char *name) {
    printf("\n--- %s (%dx%d) ---\n", name, H, W);
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++)
            printf("%4d ", m[r*W + c]);
        printf("\n");
    }
}

static void print_vector_int(const int32_t *v, int N, const char *name) {
    printf("\n--- %s (%d) ---\n", name, N);
    for (int i = 0; i < N; i++)
        printf("%d ", v[i]);
    printf("\n");
}

static void print_vector_float(const float *v, int N, const char *name) {
    printf("\n--- %s (%d) ---\n", name, N);
    for (int i = 0; i < N; i++)
        printf("%.4f ", v[i]);
    printf("\n");
}


int main() {

   
    static int32_t conv1_out[CONV1_OUT_H * CONV1_OUT_W] __attribute__((aligned(32*NR_LANES)));
    static int32_t pw1_out[PW_CH * PW1_OUT_H * PW1_OUT_W] __attribute__((aligned(32*NR_LANES)));
    static int32_t dw_out[PW_CH * DW_OUT_H * DW_OUT_W] __attribute__((aligned(32*NR_LANES)));
    static int32_t pw2_out[PW_CH * PW2_OUT_H * PW2_OUT_W] __attribute__((aligned(32*NR_LANES)));
    static int32_t pool_out[PW_CH * POOL_OUT_H * POOL_OUT_W] __attribute__((aligned(32*NR_LANES)));
    static int32_t fc_out[FC_OUT] __attribute__((aligned(32*NR_LANES)));
    static float softmax_out[FC_OUT] __attribute__((aligned(32*NR_LANES)));

    printf("Running Mini-MobileNet: Conv3x3 → PW_before → DW → PW_after → AvgPool → FC → Softmax\n");

#ifdef SPIKEGEM
    uint64_t start_minstret = read_minstret();
#endif

    start_timer();

    printf("\nRunning Conv1 3x3...\n");
    iconv2d_3x3(conv1_out,
                input_image,
                conv1_filter_3x3,
                CONV1_OUT_H, CONV1_OUT_W,
                CONV1_F);

    relu_32(conv1_out, CONV1_OUT_H * CONV1_OUT_W);

    printf("\nRunning Pointwise BEFORE (1→8)...\n");
    pointwise_before(conv1_out,
                     pw_before_weights,
                     pw_before_bias,
                     CONV1_OUT_H,
                     CONV1_OUT_W,
                     pw1_out);

  
    relu_32(pw1_out, PW_CH * PW1_OUT_H * PW1_OUT_W);

    printf("\nRunning Depthwise 3x3 (8→8)...\n");
    depthwise(dw_out,
              pw1_out,
              dw_filters,
              PW1_OUT_H,
              PW1_OUT_W);


    relu_32(dw_out, PW_CH * DW_OUT_H * DW_OUT_W);

    printf("\nRunning Pointwise AFTER (8→8)...\n");
    pointwise_after(dw_out,
                    pw_after_weights,
                    pw_after_bias,
                    DW_OUT_H,
                    DW_OUT_W,
                    pw2_out);

    relu_32(pw2_out, PW_CH * PW2_OUT_H * PW2_OUT_W);

    printf("\nRunning AvgPool2x2 on 8 channels...\n");
    for (int c = 0; c < PW_CH; ++c) {
        const int32_t *in_ch  = pw2_out  + c * (PW2_OUT_H * PW2_OUT_W);
        int32_t       *out_ch = pool_out + c * (POOL_OUT_H * POOL_OUT_W);

        avgpool2x2_32(in_ch, PW2_OUT_H, PW2_OUT_W, out_ch);
    }

 

    printf("\nRunning FC layer...\n");


    fc_32(pool_out, fc_weights, FC_IN_ROWS, FC_IN_COLS, fc_out);
    add_bias_rvv_32(fc_out, fc_bias, FC_OUT);

    printf("\nRunning Softmax...\n");
    float fc_out_f[FC_OUT];
    int32_to_float32_rvv(fc_out, fc_out_f, FC_OUT);

    softmax_rvv_32(fc_out_f, softmax_out, FC_OUT);

    stop_timer();

#ifdef SPIKEGEM
    uint64_t end_minstret = read_minstret();
    printf("Instructions retired (CSR minstret): %lu\n",
           (unsigned long)(end_minstret - start_minstret));
#endif

    int64_t cycles = get_timer();
    printf("The execution took %d cycles.\n", (int)cycles);


    return 0;
}
