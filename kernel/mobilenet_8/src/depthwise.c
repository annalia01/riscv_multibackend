#include <stdint.h>
#include "iconv2d.h"
#include "depthwise.h"

#define DW_C 8


void depthwise(int8_t *out, const int8_t *in, const int8_t *filt,
                               int H_in, int W_in)
{
    int H_out  = H_in - 2;
    int W_out  = W_in - 2;
    int HW_in  = H_in  * W_in;
    int HW_out = H_out * W_out;

    for (int c = 0; c < DW_C; ++c) {

        const int8_t *in_ch   = in   + c * HW_in;
        const int8_t *f_ch    = filt + c * 9;
        int8_t       *out_ch  = out  + c * HW_out;

        iconv2d_3x3_uint8(out_ch, (int8_t *)in_ch, (int8_t *)f_ch, H_out, W_out, 3);
    }
}
