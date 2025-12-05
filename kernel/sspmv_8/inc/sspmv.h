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
void sspmv_32(uint8_t M, uint8_t N, uint8_t rows, uint8_t cols, uint8_t * VALUES, uint8_t * col_idx, uint8_t * IN_VEC, uint8_t * OUT_VEC);
int sspmv_verify(uint8_t M, uint8_t N, uint8_t rows, uint8_t cols,
                 uint8_t * VALUES, uint8_t * col_idx,
                 uint8_t * IN_VEC, uint8_t *OUT_VEC);
#endif
