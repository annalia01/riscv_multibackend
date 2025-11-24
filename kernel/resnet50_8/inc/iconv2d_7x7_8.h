#ifndef ICONV2D_7_8_H
#define ICONV2D_7_8_H

#include <stdint.h>


void iconv2d_7x7_uint8(int8_t *o, uint8_t *i, int8_t *f, int64_t M, int64_t N,
                 int64_t F);
void iconv2d_7x7_block_uint8(int8_t *o, uint8_t *i, int8_t *f, int64_t R, int64_t C,
                       int32_t n_, int64_t F);
                       
#define MIN(a, b) ((a) < (b) ? (a) : (b))                       
#endif
