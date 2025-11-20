#include <stdint.h>
#include <stddef.h>
#ifdef SPIKEGEM
#include <stdio.h>
#else
#include "printf.h"
#endif
void maxpool2x2(const int8_t *in, int H, int W, int8_t *out)
{
    int H_out = H / 2;
    int W_out = W / 2;

    for (int r = 0; r < H_out; r++) {

        const int8_t *row0 = in + (2 * r) * W;
        const int8_t *row1 = in + (2 * r + 1) * W;

        int8_t *out_row = out + r * W_out;

        for (int c = 0; c < W_out; c++) {


            const int8_t *p0 = row0 + 2 * c;
            const int8_t *p1 = row1 + 2 * c;

            size_t vl;

            asm volatile("vsetvli %0, %1, e8, m4, ta, ma" : "=r"(vl) : "r"(2));

            asm volatile("vle8.v v0, (%0)" :: "r"(p0));
            asm volatile("vle8.v v8, (%0)" :: "r"(p1));
            asm volatile("vmax.vv v16, v0, v8");
            printf("...\n");
            asm volatile("vmv.v.i v24, 0");
            asm volatile("vredmax.vs v24, v16, v24");

            int8_t result;
            asm volatile("vmv.x.s %0, v24" : "=r"(result));

            out_row[c] = result;
        }
    }
}
