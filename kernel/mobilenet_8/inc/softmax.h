#ifndef SOFTMAX_RVV_H
#define SOFTMAX_RVV_H

#include <stdint.h>


void softmax_rvv(const float *input, float *output, int N);
static void exp_rvv(float *dst, const float *src, int N);
void int8_to_float32_rvv(const int8_t *src, float *dst, int N);

#endif
