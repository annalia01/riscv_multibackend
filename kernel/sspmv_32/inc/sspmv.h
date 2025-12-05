/*
 * Vector kernel implemented for the thesis work of <Annalia Ruggiero>.
 *
 * The algorithm is based on the S3 scheme described in:
 * <Titopoulos; Alexandridis; Peltekis; Nicopoulos; Dimitrakopoulos>,
 * <Optimizing Structured-Sparse Matrix Multiplication in RISC-V Vector Processors>, <2025>.
 *
 *
 */

#ifndef _SSPMV_H
#define _SSPMV_H
#include <stdint.h>
void sspmv_32(int32_t M, int32_t N, int32_t rows, int32_t cols, float * VALUES, int32_t * col_idx, float * IN_VEC, float * OUT_VEC);
int sspmv_verify(int32_t M, int32_t N, int32_t rows, int32_t cols,
                 float * VALUES, int32_t * col_idx,
                 float * IN_VEC, float *OUT_VEC);
#endif
