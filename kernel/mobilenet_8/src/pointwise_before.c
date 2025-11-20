#include <stdint.h>
#include <stddef.h>
#include "pointwise_before.h"

#define PW_OUT_C 8

void pointwise_before(const int8_t *in, const int8_t *w, const int8_t *b, int H, int W, int8_t *out)
{
    int HW = H * W;

    for (int oc = 0; oc < PW_OUT_C; ++oc) {

        int8_t weight = w[oc];
        int8_t bias   = b[oc];

        const int8_t *in_ptr  = in;
        int8_t *out_ptr  = out + oc * HW;

        int remaining = HW;

        while (remaining > 0) {

            size_t vl;
            asm volatile(
                "vsetvli %0, %1, e8, m8, ta, ma"
                : "=r"(vl)
                : "r"(remaining)
            );

            asm volatile("vle8.v v0, (%0)" :: "r"(in_ptr));

            asm volatile("vmul.vx v8, v0, %0" :: "r"(weight));

            asm volatile("vadd.vx v16, v8, %0" :: "r"(bias));

            asm volatile("vse8.v v16, (%0)" :: "r"(out_ptr));

            in_ptr  += vl;
            out_ptr += vl;
            remaining -= vl;
        }
    }
}
