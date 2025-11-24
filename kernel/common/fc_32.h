#ifndef FC_H
#define FC_H

#include <stdint.h>

void fc_32(int32_t *mat, int32_t *vec, int rows, int cols, int32_t *out_vec);
void add_bias_rvv_32(int32_t *out, const int32_t *bias, int N);
void vector_copy_int32(int32_t *dst, const int32_t *src, int N);
void vector_add_int32(int32_t *dst, const int32_t *a, const int32_t *b,int N);

#endif
