#include <stdint.h>

#ifdef SPIKEGEM
#define NR_LANES 8
#endif

#define IN_H 224
#define IN_W 224

// Conv 7x7
#define CONV7_F 7
#define CONV1_OUT_H (IN_H - CONV7_F + 1)   // 218
#define CONV1_OUT_W (IN_W - CONV7_F + 1)   // 218

// MaxPool 2x2
#define POOL1_OUT_H (CONV1_OUT_H / 2)      // 109
#define POOL1_OUT_W (CONV1_OUT_W / 2)      // 109

#define FINAL_H 1
#define FINAL_W 1
#define FLAT_SIZE (FINAL_H * FINAL_W)      // = 1

#define FC_OUT 10


int32_t input_image[IN_H * IN_W] __attribute__((aligned(32 * NR_LANES)));



int32_t filter_7x7[49] __attribute__((aligned(32 * NR_LANES))) = {
     1, 0, -1, 1, 0, -1, 1,
     1, 0, -1, 1, 0, -1, 1,
     1, 0, -1, 1, 0, -1, 1,
     1, 0, -1, 1, 0, -1, 1,
     1, 0, -1, 1, 0, -1, 1,
     1, 0, -1, 1, 0, -1, 1,
     1, 0, -1, 1, 0, -1, 1
};



int32_t filter_3x3[9]
    __attribute__((aligned(32 * NR_LANES))) = {
     1,  0, -1,
     1,  0, -1,
     1,  0, -1
};


int32_t fc_weights[FLAT_SIZE * FC_OUT]
    __attribute__((aligned(32 * NR_LANES)));

int32_t fc_bias[FC_OUT]
    __attribute__((aligned(32 * NR_LANES)));




static void init_dataset()
{
    
    for (int r = 0; r < IN_H; r++) {
        for (int c = 0; c < IN_W; c++) {
            
            input_image[r * IN_W + c] = (r * 131 + c * 17) % 256;
        }
    }

    for (int i = 0; i < FC_OUT; i++)
        fc_weights[i] = (i % 5) - 2;   

    for (int j = 0; j < FC_OUT; j++)
        fc_bias[j] = j; 
}
