#ifndef DEPTHWISE_H
#define DEPTHWISE_H

#include <stdint.h>

void depthwise(int8_t *out, const int8_t *in, const int8_t *filt,
                               int H_in, int W_in);

#endif 
