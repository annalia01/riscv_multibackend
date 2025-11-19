#ifndef AVGPOOL_H
#define AVGPOOL_H

#include <stdint.h>

void avgpool2x2(const int8_t *in, int H, int W, int8_t *out);

#endif
