#include <stdio.h>
#include <stdint.h>

#define M 4
#define N 2
#define ROWS 3
#define COLS 8

float VALUES[ROWS * (COLS/M) * N] = {
    // riga 0
    1.0f, 2.0f, 3.0f, 4.0f,
    // riga 1
    5.0f, 6.0f, 7.0f, 8.0f,
    // riga 2
    9.0f, 10.0f, 11.0f, 12.0f
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
