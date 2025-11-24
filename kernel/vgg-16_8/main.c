#ifdef SPIKEGEM
#include <stdio.h>
#define NR_LANES 8
#else
#include "printf.h"
#endif

#include <stdint.h>
#include "runtime.h"
#include "util.h"
#include "iconv2d_8.h"
#include "relu_8.h"
#include "max_pool_8.h"
#include "fc_8.h"
#include "softmax_8.h"

extern int8_t input_image[];

extern int8_t conv1_1_w[];
extern int8_t conv1_2_w[];

extern int8_t conv2_1_w[];
extern int8_t conv2_2_w[];

extern int8_t conv3_1_w[];
extern int8_t conv3_2_w[];
extern int8_t conv3_3_w[];

extern int8_t conv4_1_w[];
extern int8_t conv4_2_w[];
extern int8_t conv4_3_w[];

extern int8_t conv5_1_w[];
extern int8_t conv5_2_w[];
extern int8_t conv5_3_w[];


extern int8_t fc1_w[];
extern int8_t fc1_b[];
extern int8_t fc2_w[];
extern int8_t fc2_b[];
extern int8_t fc3_w[];
extern int8_t fc3_b[];


#define IN_H 224
#define IN_W 224


#define C11_H (IN_H - 2)
#define C11_W (IN_W - 2)
#define C12_H (C11_H - 2)
#define C12_W (C11_W - 2)
#define P1_H  (C12_H / 2)   
#define P1_W  (C12_W / 2)

#define C21_H (P1_H - 2)
#define C21_W (P1_W - 2)
#define C22_H (C21_H - 2)
#define C22_W (C21_W - 2)
#define P2_H  (C22_H / 2)   // 53
#define P2_W  (C22_W / 2)


#define C31_H (P2_H - 2)
#define C31_W (P2_W - 2)
#define C32_H (C31_H - 2)
#define C32_W (C31_W - 2)
#define C33_H (C32_H - 2)
#define C33_W (C32_W - 2)
#define P3_H  (C33_H / 2)   // 23
#define P3_W  (C33_W / 2)

#define C41_H (P3_H - 2)
#define C41_W (P3_W - 2)
#define C42_H (C41_H - 2)
#define C42_W (C41_W - 2)
#define C43_H (C42_H - 2)
#define C43_W (C42_W - 2)
#define P4_H  (C43_H / 2)   // 8
#define P4_W  (C43_W / 2)

#define C51_H (P4_H - 2)
#define C51_W (P4_W - 2)
#define C52_H (C51_H - 2)
#define C52_W (C51_W - 2)
#define C53_H (C52_H - 2)
#define C53_W (C52_W - 2)
#define P5_H  (C53_H / 2)   // 1
#define P5_W  (C53_W / 2)

#define FLAT_SIZE (P5_H * P5_W)   // = 1
#define FC1_OUT  64
#define FC2_OUT  64
#define FC3_OUT  10

static inline uint64_t read_minstret(void) {
    uint64_t value;
    asm volatile("csrr %0, instret" : "=r"(value));
    return value;
}


int main() {

    printf("Running VGG-16...\n");

 
    static int8_t c11[C11_H*C11_W] __attribute__((aligned(32*NR_LANES)));
    static int8_t c12[C12_H*C12_W] __attribute__((aligned(32*NR_LANES)));
    static int8_t p1[P1_H*P1_W]     __attribute__((aligned(32*NR_LANES)));

    static int8_t c21[C21_H*C21_W] __attribute__((aligned(32*NR_LANES)));
    static int8_t c22[C22_H*C22_W] __attribute__((aligned(32*NR_LANES)));
    static int8_t p2[P2_H*P2_W]     __attribute__((aligned(32*NR_LANES)));

    static int8_t c31[C31_H*C31_W] __attribute__((aligned(32*NR_LANES)));
    static int8_t c32[C32_H*C32_W] __attribute__((aligned(32*NR_LANES)));
    static int8_t c33[C33_H*C33_W] __attribute__((aligned(32*NR_LANES)));
    static int8_t p3[P3_H*P3_W]     __attribute__((aligned(32*NR_LANES)));

    static int8_t c41[C41_H*C41_W] __attribute__((aligned(32*NR_LANES)));
    static int8_t c42[C42_H*C42_W] __attribute__((aligned(32*NR_LANES)));
    static int8_t c43[C43_H*C43_W] __attribute__((aligned(32*NR_LANES)));
    static int8_t p4[P4_H*P4_W]     __attribute__((aligned(32*NR_LANES)));

    static int8_t c51[C51_H*C51_W] __attribute__((aligned(32*NR_LANES)));
    static int8_t c52[C52_H*C52_W] __attribute__((aligned(32*NR_LANES)));
    static int8_t c53[C53_H*C53_W] __attribute__((aligned(32*NR_LANES)));
    static int8_t p5[P5_H*P5_W]     __attribute__((aligned(32*NR_LANES)));

    static int8_t fc1_out[FC1_OUT] __attribute__((aligned(32*NR_LANES)));
    static int8_t fc2_out[FC2_OUT] __attribute__((aligned(32*NR_LANES)));
    static int8_t fc3_out[FC3_OUT] __attribute__((aligned(32*NR_LANES)));
    static float softmax_out[FC3_OUT];

#ifdef SPIKEGEM
    uint64_t start_minstret = read_minstret();
#endif
    start_timer();

    iconv2d_3x3_uint8(c11, input_image, conv1_1_w, IN_H, IN_W, 3);
    relu_8(c11, C11_H*C11_W);
    iconv2d_3x3_uint8(c12, c11, conv1_2_w, C11_H, C11_W, 3);  
    relu_8(c12, C12_H*C12_W);
    maxpool2x2_8(c12, C12_H, C12_W, p1);

    iconv2d_3x3_uint8(c21, p1, conv2_1_w, P1_H, P1_W, 3);
    relu_8(c21, C21_H*C21_W);
    iconv2d_3x3_uint8(c22, c21, conv2_2_w, C21_H, C21_W, 3);
    relu_8(c22, C22_H*C22_W);
    maxpool2x2_8(c22, C22_H, C22_W, p2);

    iconv2d_3x3_uint8(c31, p2, conv3_1_w, P2_H, P2_W, 3);   
    relu_8(c31, C31_H*C31_W);
    iconv2d_3x3_uint8(c32, c31, conv3_2_w, C31_H, C31_W, 3); 
    relu_8(c32, C32_H*C32_W);
    iconv2d_3x3_uint8(c33, c32, conv3_3_w, C32_H, C32_W, 3); 
    relu_8(c33, C33_H*C33_W);
    maxpool2x2_8(c33, C33_H, C33_W, p3);

    iconv2d_3x3_uint8(c41, p3, conv4_1_w, P3_H, P3_W, 3);   
    relu_8(c41, C41_H*C41_W);
    iconv2d_3x3_uint8(c42, c41, conv4_2_w, C41_H, C41_W, 3); 
    relu_8(c42, C42_H*C42_W);
    iconv2d_3x3_uint8(c43, c42, conv4_3_w, C42_H, C42_W, 3); 
    relu_8(c43, C43_H*C43_W);
    maxpool2x2_8(c43, C43_H, C43_W, p4);

    iconv2d_3x3_uint8(c51, p4, conv5_1_w, P4_H, P4_W, 3);   
    relu_8(c51, C51_H*C51_W);
    iconv2d_3x3_uint8(c52, c51, conv5_2_w, C51_H, C51_W, 3); 
    relu_8(c52, C52_H*C52_W);
    iconv2d_3x3_uint8(c53, c52, conv5_3_w, C52_H, C52_W, 3); 
    relu_8(c53, C53_H*C53_W);
    maxpool2x2_8(c53, C53_H, C53_W, p5);  

    int8_t *flat = p5; 

    fc_8(fc1_w, flat, FC1_OUT, FLAT_SIZE, fc1_out);
    add_bias_rvv_8(fc1_out, fc1_b, FC1_OUT);
    relu_8(fc1_out, FC1_OUT);

    fc_8(fc2_w, fc1_out, FC2_OUT, FC1_OUT, fc2_out);
    add_bias_rvv_8(fc2_out, fc2_b, FC2_OUT);
    relu_8(fc2_out, FC2_OUT);

    fc_8(fc3_w, fc2_out, FC3_OUT, FC2_OUT, fc3_out);
    add_bias_rvv_8(fc3_out, fc3_b, FC3_OUT);
    relu_8(fc3_out, FC3_OUT);

    float fc3_f[FC3_OUT];
    for(int i=0; i<FC3_OUT; i++) fc3_f[i]=(float)fc3_out[i];
  
    softmax_rvv_8(fc3_f, softmax_out, FC3_OUT);

    stop_timer();

#ifdef SPIKEGEM
    uint64_t end_minstret = read_minstret();
    printf("Instructions retired: %lu\n", end_minstret - start_minstret);
#endif

    printf("Execution time: %d cycles.\n", get_timer());
    return 0;
}
