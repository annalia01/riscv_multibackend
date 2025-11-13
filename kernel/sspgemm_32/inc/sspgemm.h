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

#ifndef _SSPGEMM_H
#define _SSPGEMM_H
#include <stdint.h>

void sspgemm_32(int32_t M, int32_t N, int32_t rows, int32_t cols, float *VALUES, int32_t *col_idx, float *B, int32_t cols_b, float *C);
int sspgemm_verify_matrix(int32_t M, int32_t N, int32_t rows, int32_t cols, float *VALUES, int32_t *col_idx, float *B, int32_t cols_b, float *OUT);

#endif
