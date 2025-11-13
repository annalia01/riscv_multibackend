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

void sspgemm_32(uint8_t M, uint8_t N, uint8_t rows, uint8_t cols, uint8_t *VALUES, uint8_t *col_idx, uint8_t *B, uint8_t cols_b, uint8_t *C);
int sspgemm_verify_matrix(uint8_t M, uint8_t N, uint8_t rows, uint8_t cols, uint8_t *VALUES, uint8_t *col_idx, uint8_t *B, uint8_t cols_b, uint8_t *OUT);

#endif
