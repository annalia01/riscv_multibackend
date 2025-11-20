#ifndef FC_H
#define FC_H

#include <stdint.h>

void fc(int8_t *mat, int8_t *vec, int rows, int cols, int8_t *out_vec);
void add_bias_rvv(int8_t *out, const int8_t *bias, int N);
#endif
