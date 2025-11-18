#include <stdint.h>
#ifdef SPIKEGEM
#define NR_LANES 8
#endif

#define IN_H 16
#define IN_W 16

#define CONV_F 3
#define CONV_OUT_H (IN_H - 2)
#define CONV_OUT_W (IN_W - 2)

#define POOL_OUT_H (CONV_OUT_H / 2)
#define POOL_OUT_W (CONV_OUT_W / 2)

#define FLAT_SIZE (POOL_OUT_H * POOL_OUT_W)
#define FC_OUT 10


int32_t input_image[IN_H * IN_W]  __attribute__((aligned(32*NR_LANES)))= {
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



int32_t filter_3x3[9]  __attribute__((aligned(32*NR_LANES)))= {
    1, 0, -1,
    1, 0, -1,
    1, 0, -1
};


int32_t fc_weights[FLAT_SIZE * FC_OUT]  __attribute__((aligned(32*NR_LANES)));
int32_t fc_bias[FC_OUT]  __attribute__((aligned(32*NR_LANES)));


__attribute__((constructor))
static void init_fc()
{
    for (int i = 0; i < FLAT_SIZE * FC_OUT; i++)
        fc_weights[i] = (i % 4) - 2;

    for (int j = 0; j < FC_OUT; j++)
        fc_bias[j] = 0;
}
