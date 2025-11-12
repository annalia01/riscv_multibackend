#include <stdint.h>
#ifdef SPIKEGEM 
#define NR_LANES 8
#endif

int32_t M = 4;
int32_t N = 2;
int32_t rows = 3;
int32_t cols = 8;

float VALUES[rows * (COLS/M) * N] __attribute__((aligned(32 * NR_LANES), section(".l2"))) = {
    // riga 0
    1.0, 2.0, 3.0, 4.0,
    // riga 1
    5.0, 6.0, 7.0, 8.0,
    // riga 2
    9.0, 10.0, 11.0, 12.0
};

int32_t col_idx[rows * (COLS/M) * N] __attribute__((aligned(32 * NR_LANES), section(".l2"))) = {
    // riga 0
    0, 2, 1, 3,
    // riga 1
    1, 2, 0, 2,
    // riga 2
    0, 3, 1, 2
};

float IN_VEC[cols] __attribute__((aligned(32 * NR_LANES), section(".l2"))) = {1, 2, 3, 4, 5, 6, 7, 8};
float OUT_VEC[rows] __attribute__((aligned(32 * NR_LANES), section(".l2")));
