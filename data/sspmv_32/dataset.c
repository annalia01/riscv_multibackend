#include <stdio.h>
#include <stdint.h>

#define M 4
#define N 2
#define ROWS 3
#define COLS 8

float VALUES[ROWS * (COLS/M) * N] = {
    // riga 0
    1.0, 2.0, 3.0, 4.0,
    // riga 1
    5.0, 6.0, 7.0, 8.0,
    // riga 2
    9.0, 10.0, 11.0, 12.0
};

int32_t col_idx[ROWS * (COLS/M) * N] = {
    // riga 0
    0, 2, 1, 3,
    // riga 1
    1, 2, 0, 2,
    // riga 2
    0, 3, 1, 2
};

float IN_VEC[COLS] = {1, 2, 3, 4, 5, 6, 7, 8};
float OUT_VEC[ROWS];
