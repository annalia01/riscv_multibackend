#include <stdint.h>
#include <stddef.h>
#include "pointwise_after.h"

#define PW_IN_C  8
#define PW_OUT_C 8

void pointwise_after(const int32_t *in, const int32_t *w, const int32_t *b, int H, int W, int32_t *out)
{
    int HW = H * W;

    for (int oc = 0; oc < PW_OUT_C; ++oc) {

        int32_t bias = b[oc];
        int remaining = HW;
        int offset = 0;  

        while (remaining > 0) {

            size_t vl;
            asm volatile("vsetvli %0, %1, e32, m8, ta, ma" : "=r"(vl) : "r"(remaining));

            asm volatile("vmv.v.i v0, 0");

            
            for (int ic = 0; ic < PW_IN_C; ++ic) {

                const int32_t *in_ch = in + ic * HW + offset;
                int32_t w_oc_ic      = w[oc * PW_IN_C + ic];

                
                asm volatile("vle32.v v8, (%0)" :: "r"(in_ch));

                asm volatile("vmacc.vx v0, %0, v8" :: "r"(w_oc_ic));
            }

       
            asm volatile("vadd.vx v16, v0, %0" :: "r"(bias));

            int32_t *out_ptr = out + oc * HW + offset;
            asm volatile("vse32.v v16, (%0)" :: "r"(out_ptr));

            offset    += vl;
            remaining -= vl;
        }
    }
}
