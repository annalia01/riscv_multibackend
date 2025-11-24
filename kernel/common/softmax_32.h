#ifndef SOFTMAX_RVV_H
#define SOFTMAX_RVV_H

#include <stdint.h>


void softmax_rvv_32(const float *input, float *output, int N);
static void exp_rvv_32(float *dst, const float *src, int N);
void int32_to_float32_rvv(const int32_t *src, float *dst, int N);
#endif
