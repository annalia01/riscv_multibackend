#ifndef POINTAFTER_H
#define POINTAFTER_H

#include <stdint.h>

void pointwise_after(const int32_t *in, const int32_t *w, const int32_t *b, int H, int W, int32_t *out);

#endif
