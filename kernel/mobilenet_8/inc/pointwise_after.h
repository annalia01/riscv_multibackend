#ifndef POINTAFTER_H
#define POINTAFTER_H

#include <stdint.h>

void pointwise_after(const int8_t *in, const int8_t *w, const int8_t *b, int H, int W, int8_t *out);

#endif
