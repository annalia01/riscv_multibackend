#ifndef BN_H
#define BN_H

#include <stdint.h>

void ibatchnorm_2d_8(int8_t *o, int8_t *i, int8_t mean, int8_t invstd, int8_t gamma, int8_t beta, int R, int C);

#endif
