#include <stdint.h>

#ifdef SPIKEGEM
#define NR_LANES 8
#endif

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
#define P2_H  (C22_H / 2)
#define P2_W  (C22_W / 2)

#define C31_H (P2_H - 2)
#define C31_W (P2_W - 2)
#define C32_H (C31_H - 2)
#define C32_W (C31_W - 2)
#define C33_H (C32_H - 2)
#define C33_W (C32_W - 2)
#define P3_H  (C33_H / 2)
#define P3_W  (C33_W / 2)

#define C41_H (P3_H - 2)
#define C41_W (P3_W - 2)
#define C42_H (C41_H - 2)
#define C42_W (C41_W - 2)
#define C43_H (C42_H - 2)
#define C43_W (C42_W - 2)
#define P4_H  (C43_H / 2)
#define P4_W  (C43_W / 2)

#define C51_H (P4_H - 2)
#define C51_W (P4_W - 2)
#define C52_H (C51_H - 2)
#define C52_W (C51_W - 2)
#define C53_H (C52_H - 2)
#define C53_W (C52_W - 2)
#define P5_H  (C53_H / 2)
#define P5_W  (C53_W / 2)

#define FLAT_SIZE (P5_H * P5_W)   

#define FC1_OUT 64
#define FC2_OUT 64
#define FC3_OUT 10


int8_t input_image[IN_H * IN_W] __attribute__((aligned(32 * NR_LANES)));


#define FILL_FILTER(name) \
int32_t name[9] __attribute__((aligned(32*NR_LANES))) = { \
    1, 0, -1, \
    1, 0, -1, \
    1, 0, -1  \
};

FILL_FILTER(conv1_1_w)
FILL_FILTER(conv1_2_w)
FILL_FILTER(conv2_1_w)
FILL_FILTER(conv2_2_w)
FILL_FILTER(conv3_1_w)
FILL_FILTER(conv3_2_w)
FILL_FILTER(conv3_3_w)
FILL_FILTER(conv4_1_w)
FILL_FILTER(conv4_2_w)
FILL_FILTER(conv4_3_w)
FILL_FILTER(conv5_1_w)
FILL_FILTER(conv5_2_w)
FILL_FILTER(conv5_3_w)


int8_t fc1_w[FC1_OUT * FLAT_SIZE] __attribute__((aligned(32*NR_LANES)));
int8_t fc1_b[FC1_OUT]              __attribute__((aligned(32*NR_LANES)));

int8_t fc2_w[FC2_OUT * FC1_OUT] __attribute__((aligned(32*NR_LANES)));
int8_t fc2_b[FC2_OUT]            __attribute__((aligned(32*NR_LANES)));

int8_t fc3_w[FC3_OUT * FC2_OUT] __attribute__((aligned(32*NR_LANES)));
int8_t fc3_b[FC3_OUT]            __attribute__((aligned(32*NR_LANES)));



static void init_fc()
{

    for (int r = 0; r < IN_H; r++) {
        for (int c = 0; c < IN_W; c++) {
            
            input_image[r * IN_W + c] = (r * 131 + c * 17) % 256;
        }
    }
    // FC1
    for (int i = 0; i < FC1_OUT * FLAT_SIZE; i++)
        fc1_w[i] = (i % 5) - 2;    
    for (int i = 0; i < FC1_OUT; i++)
        fc1_b[i] = 0;

    // FC2
    for (int i = 0; i < FC2_OUT * FC1_OUT; i++)
        fc2_w[i] = (i % 7) - 3;
    for (int i = 0; i < FC2_OUT; i++)
        fc2_b[i] = 0;

    // FC3
    for (int i = 0; i < FC3_OUT * FC2_OUT; i++)
        fc3_w[i] = (i % 11) - 5;
    for (int i = 0; i < FC3_OUT; i++)
        fc3_b[i] = 0;
}
