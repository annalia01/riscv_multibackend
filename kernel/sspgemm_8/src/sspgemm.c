/*
 * Vector kernel implemented for the thesis work of <Annalia Ruggiero>.
 *
 * The algorithm is based on the S2 scheme described in:
 * <Titopoulos; Alexandridis; Peltekis; Nicopoulos; Dimitrakopoulos>,
 * <Optimizing Structured-Sparse Matrix Multiplication in RISC-V Vector Processors>, <2025>.
 *
 * In this implementation, non-zero values are stored following
 * the layout used in algorithm S3, while the computational flow
 * follows algorithm S2.
 */

#include "sspgemm.h"
#include "runtime.h"
#include "util.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#ifdef SPIKEGEM
#include <stdio.h>
#else 
#include "printf.h"
#endif


void sspgemm_32(uint8_t M, uint8_t N,
                uint8_t rows, uint8_t cols,
                uint8_t *VALUES, uint8_t *col_idx,
                uint8_t *B, uint8_t cols_b,
                uint8_t *C) {

    uint8_t nnzpr = (cols / M) * N;  

    for (int i = 0; i < rows; i++) {

        uint8_t *c_row = &C[i * cols_b];

      
        for (int k = 0; k < cols_b; ) {

            int32_t remaining = cols_b - k;
            int32_t vl;

            asm volatile("vsetvli %0, %1, e8, m1, ta, ma"
                         : "=r"(vl)
                         : "r"(remaining));

            asm volatile("vmv.v.i v0, 0");

            for (int j = 0; j < nnzpr; j++) {

                uint8_t block_id = j / N;
                uint8_t s1 = col_idx[i * nnzpr + j];
                s1 += block_id * M; 

                uint8_t *b_ptr = &B[s1 * cols_b + k];  

                asm volatile("vle8.v v4, (%0)" :: "r"(b_ptr));

                uint8_t s0 = VALUES[i * nnzpr + j];

                asm volatile("vmacc.vx v0, %0, v4" :: "r"(s0));
            }

            uint8_t *c_ptr = c_row + k;
            asm volatile("vse8.v v0, (%0)" :: "r"(c_ptr));
            k += vl;
        }
    }
}

int sspgemm_verify_matrix(uint8_t M, uint8_t N, uint8_t rows, uint8_t cols, uint8_t *VALUES, uint8_t *col_idx, uint8_t *B, uint8_t cols_b, uint8_t *OUT) 
{
    uint8_t nnzpr = (cols / M) * N; 
    for (int i = 0; i < rows; i++) {

        for (int k = 0; k < cols_b; k++) {

            uint32_t golden = 0.0;
            for (int32_t j = 0; j < nnzpr; j++) {

                uint8_t block_id = j / N;

                uint8_t s1 = col_idx[i * nnzpr + j];
                s1 += block_id * M;

                uint8_t a_ij = VALUES[i * nnzpr + j];

                golden += (uint32_t)a_ij * (uint32_t)B[s1 * cols_b + k];
            }

            uint8_t res = OUT[i * cols_b + k];

            if (fabsf((uint8_t)golden - res) > 1e-3f) {
                printf("Wrong value at C[%d,%d]: result=%d, golden=%d\n",
                       i, k, res, golden);
                return i * cols_b + k; 
            }
        }
    }

    return 0; 
}
