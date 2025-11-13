#include <stdint.h>
#ifdef SPIKEGEM
#define NR_LANES 8
#endif

// ----- parametri -----
int32_t M = 4;
int32_t N = 2;
int32_t rows = 3;
int32_t cols = 4;
int32_t cols_b = 5;


// ----- A in formato sparsità 2:4 -----

// VALUES[i][j] nei blocchi N:M
float VALUES[6] __attribute__((aligned(32 * NR_LANES))) = {
    // riga 0
    1.0f, 4.0f,
    // riga 1
    2.0f, 5.0f,
    // riga 2
    3.0f, 6.0f
};


int32_t col_idx[6] __attribute__((aligned(32 * NR_LANES)))= {
    // riga 0: prende col 0 e col 2
    0, 2,
    // riga 1: prende col 1 e col 3
    1, 3,
    // riga 2: prende col 0 e col 3
    0, 3
};

// ----- Matrice B (normale), dimensione = cols x cols_b = 4 x 5 -----
float B[20] __attribute__((aligned(32 * NR_LANES)))= {
    // riga 0
    1,  2,  3,  4,  5,
    // riga 1
    6,  7,  8,  9, 10,
    // riga 2
    11, 12, 13, 14, 15,
    // riga 3
    16, 17, 18, 19, 20
};

// ----- Matrice risultato (vector) -----
float C[15] __attribute__((aligned(32 * NR_LANES)));

