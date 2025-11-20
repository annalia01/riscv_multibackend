#ifndef FC_H
#define FC_H

#include <stdint.h>

void fc(int32_t *mat, int32_t *vec, int rows, int cols, int32_t *out_vec);
void add_bias_rvv(int32_t *out, const int32_t *bias, int N);

#endif
