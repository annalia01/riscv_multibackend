#include <stdint.h>

#ifdef SPIKEGEM
#define NR_LANES 8
#endif

#define IN_H 16
#define IN_W 16

#define CONV1_OUT_H (IN_H - 2)   // 14
#define CONV1_OUT_W (IN_W - 2)   // 14

#define PW_CH 8

#define PW1_OUT_H (CONV1_OUT_H)  // 14
#define PW1_OUT_W (CONV1_OUT_W)  // 14

#define DW_OUT_H  (PW1_OUT_H - 2) // 12
#define DW_OUT_W  (PW1_OUT_W - 2) // 12

#define PW2_OUT_H (DW_OUT_H)   // 12
#define PW2_OUT_W (DW_OUT_W)   // 12

#define POOL_OUT_H (PW2_OUT_H / 2) // 6
#define POOL_OUT_W (PW2_OUT_W / 2) // 6

#define FC_IN_ROWS (PW_CH * POOL_OUT_H)   // 8 * 6 = 48
#define FC_IN_COLS (POOL_OUT_W)          // 6
#define FLAT_SIZE  (FC_IN_ROWS * FC_IN_COLS)  // 288

#define FC_OUT 10


// =========================
// 1) INPUT IMAGE 16×16
// =========================
int32_t input_image[IN_H * IN_W] __attribute__((aligned(32*NR_LANES))) =
{
    1,2,3,4,5,6,7,8, 9,10,11,12,13,14,15,16,
    2,3,4,5,6,7,8,9, 10,11,12,13,14,15,16,1,
    3,4,5,6,7,8,9,10, 11,12,13,14,15,16,1,2,
    4,5,6,7,8,9,10,11, 12,13,14,15,16,1,2,3,

    5,6,7,8,9,10,11,12, 13,14,15,16,1,2,3,4,
    6,7,8,9,10,11,12,13, 14,15,16,1,2,3,4,5,
    7,8,9,10,11,12,13,14, 15,16,1,2,3,4,5,6,
    8,9,10,11,12,13,14,15, 16,1,2,3,4,5,6,7,

    9,10,11,12,13,14,15,16, 1,2,3,4,5,6,7,8,
    10,11,12,13,14,15,16,1, 2,3,4,5,6,7,8,9,
    11,12,13,14,15,16,1,2, 3,4,5,6,7,8,9,10,
    12,13,14,15,16,1,2,3, 4,5,6,7,8,9,10,11,

    13,14,15,16,1,2,3,4, 5,6,7,8,9,10,11,12,
    14,15,16,1,2,3,4,5, 6,7,8,9,10,11,12,13,
    15,16,1,2,3,4,5,6, 7,8,9,10,11,12,13,14,
    16,1,2,3,4,5,6,7, 8,9,10,11,12,13,14,15
};


int32_t conv1_filter_3x3[9] __attribute__((aligned(32*NR_LANES))) =
{
    1, 0, -1,
    1, 0, -1,
    1, 0, -1
};


int32_t pw_before_weights[PW_CH] __attribute__((aligned(32*NR_LANES)));

int32_t pw_before_bias[PW_CH] __attribute__((aligned(32*NR_LANES)));


int32_t dw_filters[PW_CH * 9] __attribute__((aligned(32*NR_LANES)));


int32_t pw_after_weights[PW_CH * PW_CH] __attribute__((aligned(32*NR_LANES)));

int32_t pw_after_bias[PW_CH] __attribute__((aligned(32*NR_LANES)));


int32_t fc_weights[FLAT_SIZE * FC_OUT] __attribute__((aligned(32*NR_LANES)));

int32_t fc_bias[FC_OUT] __attribute__((aligned(32*NR_LANES)));


static void init_params()
{
 
    for (int i = 0; i < PW_CH; i++) {
        pw_before_weights[i] = (i % 3) + 1;  
        pw_before_bias[i] = 0;
    }

    for (int c = 0; c < PW_CH; c++) {
        int base = c * 9;
        dw_filters[base + 0] = 1;
        dw_filters[base + 1] = 0;
        dw_filters[base + 2] = -1;

        dw_filters[base + 3] = 1;
        dw_filters[base + 4] = 0;
        dw_filters[base + 5] = -1;

        dw_filters[base + 6] = 1;
        dw_filters[base + 7] = 0;
        dw_filters[base + 8] = -1;
    }

 
    for (int oc = 0; oc < PW_CH; oc++) {
        for (int ic = 0; ic < PW_CH; ic++) {
            pw_after_weights[oc * PW_CH + ic] = (oc + ic) % 4 - 1; 
        }
        pw_after_bias[oc] = 0;
    }

 
    for (int i = 0; i < FLAT_SIZE * FC_OUT; i++)
        fc_weights[i] = (i % 5) - 2;

    for (int i = 0; i < FC_OUT; i++)
        fc_bias[i] = 0;
}
