#ifndef ICONV2D_7_32
#define ICONV2D_7_32
#include <stdint.h>

void iconv2d_7x7(int32_t *o, int32_t *i, int32_t *f, int64_t M, int64_t N,
                 int64_t F);
void iconv2d_7x7_block(int32_t *o, int32_t *i, int32_t *f, int64_t R, int64_t C,
                       int32_t n_, int64_t F);
                       
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
