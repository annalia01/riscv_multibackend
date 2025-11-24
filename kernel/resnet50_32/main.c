#ifdef SPIKEGEM
#include <stdio.h>
#define NR_LANES 8
#else
#include "printf.h"
#endif

#include <stdint.h>
#include "runtime.h"
#include "util.h"
#include "iconv2d_7x7_32.h"
#include "iconv2d_32.h"
#include "relu_32.h"
#include "max_pool_32.h"
#include "fc_32.h"
#include "bn_32.h"
#include "avg_pool_32.h"

extern int32_t input_image[];   
extern int32_t filter_7x7[];    
extern int32_t filter_3x3[];    
extern int32_t fc_weights[];    
extern int32_t fc_bias[];       


#define IN_H 224
#define IN_W 224


#define CONV7_F        7
#define CONV1_OUT_H    (IN_H - CONV7_F + 1)  
#define CONV1_OUT_W    (IN_W - CONV7_F + 1)  


#define POOL1_OUT_H    (CONV1_OUT_H / 2)     
#define POOL1_OUT_W    (CONV1_OUT_W / 2)     


#define RB_H           (POOL1_OUT_H)
#define RB_W           (POOL1_OUT_W)

#define NUM_RES_BLOCKS 24



#define FINAL_H        (AP3_OUT_H)
#define FINAL_W        (AP3_OUT_W)
#define FLAT_SIZE      (FINAL_H * FINAL_W)   

#define FC_OUT         10


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

  
    static int32_t conv1_out[CONV1_OUT_H * CONV1_OUT_W] __attribute__((aligned(32 * NR_LANES)));

    static int32_t pool1_out[POOL1_OUT_H * POOL1_OUT_W] __attribute__((aligned(32 * NR_LANES)));

    static int32_t resblock_buf_a[RB_H * RB_W] __attribute__((aligned(32 * NR_LANES)));
    static int32_t resblock_buf_b[RB_H * RB_W] __attribute__((aligned(32 * NR_LANES)));
    static int32_t resblock_tmp[RB_H * RB_W] __attribute__((aligned(32 * NR_LANES)));
    static int32_t fc_out[FC_OUT] __attribute__((aligned(32 * NR_LANES)));

    const int32_t bn_mean   = 4;
    const int32_t bn_invstd = 1;
    const int32_t bn_gamma  = 1;
    const int32_t bn_beta   = 3;

    printf("Running: CONV7x7 → BN → ReLU → MAXPOOL → "
           "ResidualBlocks(3x3, BN, ReLU) × %d → "
           "AVGPOOL ×3 → FC\n", NUM_RES_BLOCKS);

#ifdef SPIKEGEM
    uint64_t start_minstret = read_minstret();
#endif
    start_timer();

 
    printf("\n[Layer] CONV1 7x7...\n");
    iconv2d_7x7(conv1_out, input_image, filter_7x7,
                CONV1_OUT_H, CONV1_OUT_W, CONV7_F);

    printf("[Layer] BN after CONV1...\n");
    ibatchnorm_2d_int32(conv1_out, conv1_out,
                        bn_mean, bn_invstd, bn_gamma, bn_beta,
                        CONV1_OUT_H, CONV1_OUT_W);

    printf("[Layer] ReLU after CONV1...\n");
    relu_32(conv1_out, CONV1_OUT_H * CONV1_OUT_W);


    printf("[Layer] MAXPOOL 2x2...\n");
    maxpool2x2_32(conv1_out, CONV1_OUT_H, CONV1_OUT_W, pool1_out);

 
    vector_copy_int32(resblock_buf_a, pool1_out, RB_H * RB_W);

    int32_t *res_in  = resblock_buf_a;
    int32_t *res_out = resblock_buf_b;
    int dim_H=RB_H;
    int dim_W=RB_W;

    for (int b = 0; b < NUM_RES_BLOCKS; ++b) {

        printf("[Layer] Residual Block %d / %d\n", b + 1, NUM_RES_BLOCKS);

 
        iconv2d_3x3(resblock_tmp, res_in, filter_3x3,
                    dim_H, dim_W, 3);
        dim_H= RB_H -2*(b+1) -2*b;
        dim_W= RB_W -2*(b+1) -2*b;
        ibatchnorm_2d_int32(resblock_tmp, resblock_tmp,
                            bn_mean, bn_invstd, bn_gamma, bn_beta, dim_H, dim_W);

        relu_32(resblock_tmp, dim_H * dim_W);


        iconv2d_3x3(res_out, resblock_tmp, filter_3x3,
                    dim_H, dim_W, 3);
        dim_H-=2;
        dim_W-=2;
        ibatchnorm_2d_int32(res_out, res_out,
                            bn_mean, bn_invstd, bn_gamma, bn_beta,
                            dim_H, dim_W);

 
        vector_add_int32(res_out, res_out, res_in, dim_H * dim_W);

        
        relu_32(res_out, dim_H * dim_W);

       
        int32_t *tmp = res_in;
        res_in  = res_out;
        res_out = tmp;
    }

   
    printf("[Layer] AVGPOOL 2x2 #1...\n");
     
    
    const int AP1_OUT_H= dim_H/2;
    const int AP1_OUT_W= dim_W/2;
    int32_t avg1_out[AP1_OUT_H * AP1_OUT_W] __attribute__((aligned(32 * NR_LANES)));
    avgpool2x2_32(res_in, dim_H, dim_W, avg1_out);
    
    
    printf("[Layer] AVGPOOL 2x2 #2...\n");
    const int AP2_OUT_H= AP1_OUT_H/2;
    const int AP2_OUT_W= AP1_OUT_W/2;
    int32_t avg2_out[AP2_OUT_H * AP2_OUT_W]__attribute__((aligned(32 * NR_LANES)));
    
    
    avgpool2x2_32(avg1_out, AP1_OUT_H, AP1_OUT_W, avg2_out);
    
    printf("[Layer] AVGPOOL 2x2 #3...\n");
    const int AP3_OUT_H= AP2_OUT_H/2;
    const int AP3_OUT_W= AP2_OUT_W/2;
    int32_t avg3_out[AP3_OUT_H * AP3_OUT_W] __attribute__((aligned(32 * NR_LANES)));
    avgpool2x2_32(avg2_out, AP2_OUT_H, AP2_OUT_W, avg3_out);
    
    
    printf("[Layer] FC layer...\n");


    int32_t *final_feat = avg3_out; 


    fc_32(fc_weights, final_feat, FC_OUT, FLAT_SIZE, fc_out);


    add_bias_rvv_32(fc_out, fc_bias, FC_OUT);

    stop_timer();

#ifdef SPIKEGEM
    uint64_t end_minstret = read_minstret();
    printf("Instructions retired: %lu\n", end_minstret - start_minstret);
#endif

    printf("Execution: %d cycles.\n", get_timer());

    //print_vector_int(fc_out, FC_OUT, "FC output");

    return 0;
}
