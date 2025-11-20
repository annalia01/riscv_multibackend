#ifndef DEPTHWISE_H
#define DEPTHWISE_H

#include <stdint.h>

void depthwise(int32_t *out, const int32_t *in, const int32_t *filt,
                               int H_in, int W_in);

#endif 
