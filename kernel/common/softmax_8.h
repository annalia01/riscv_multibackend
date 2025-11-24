#ifndef SOFTMAX_RVV_H
#define SOFTMAX_RVV_H

#include <stdint.h>


void softmax_rvv_8(const float *input, float *output, int N);
static void exp_rvv_8(float *dst, const float *src, int N);


#endif
