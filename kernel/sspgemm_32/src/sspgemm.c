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


void sspgemm_32(int32_t M, int32_t N,
                int32_t rows, int32_t cols,
                float *VALUES, int32_t *col_idx,
                float *B, int32_t cols_b,
                float *C) {

    int32_t nnzpr = (cols / M) * N;  

    for (int32_t i = 0; i < rows; i++) {

        float *c_row = &C[i * cols_b];

      
        for (int32_t k = 0; k < cols_b; ) {

            int32_t remaining = cols_b - k;
            int32_t vl;

            asm volatile("vsetvli %0, %1, e32, m8, ta, ma"
                         : "=r"(vl)
                         : "r"(remaining));

            asm volatile("vmv.v.i v0, 0");

            for (int32_t j = 0; j < nnzpr; j++) {

                int32_t block_id = j / N;
                int32_t s1 = col_idx[i * nnzpr + j];
                s1 += block_id * M; 

                float *b_ptr = &B[s1 * cols_b + k];  

                asm volatile("vle32.v v8, (%0)" :: "r"(b_ptr));

                float s0 = VALUES[i * nnzpr + j];

                asm volatile("vfmacc.vf v0, %0, v8" :: "f"(s0));
            }

            float *c_ptr = c_row + k;
            asm volatile("vse32.v v0, (%0)" :: "r"(c_ptr));
            k += vl;
        }
    }
}

int sspgemm_verify_matrix(int32_t M, int32_t N, int32_t rows, int32_t cols, float *VALUES, int32_t *col_idx, float *B, int32_t cols_b, float *OUT) 
{
    int32_t nnzpr = (cols / M) * N; 
    for (int32_t i = 0; i < rows; i++) {

        for (int32_t k = 0; k < cols_b; k++) {

            float golden = 0.0f;
            for (int32_t j = 0; j < nnzpr; j++) {

                int32_t block_id = j / N;

                int32_t s1 = col_idx[i * nnzpr + j];
                s1 += block_id * M;

                float a_ij = VALUES[i * nnzpr + j];

                golden += a_ij * B[s1 * cols_b + k];
            }

            float res = OUT[i * cols_b + k];

            if (fabsf(golden - res) > 1e-3f) {
                printf("Wrong value at C[%d,%d]: result=%f, golden=%f\n",
                       i, k, res, golden);
                return i * cols_b + k; 
            }
        }
    }

    return 0; 
}
