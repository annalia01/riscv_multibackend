
#ifndef AVGPOOL_H
#define AVGPOOL_H

#include <stdint.h>

void avgpool2x2_32(const int32_t *in, int H, int W, int32_t *out);

#endif
