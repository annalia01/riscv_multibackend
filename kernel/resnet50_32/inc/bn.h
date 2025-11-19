#ifndef BN_H
#define BN_H

#include <stdint.h>

void ibatchnorm_2d_int32(int32_t *o, int32_t *i, int32_t mean, int32_t invstd, int32_t gamma, int32_t beta, int R, int C);

#endif
