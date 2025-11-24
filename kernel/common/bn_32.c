#include <stdint.h>
#include <stddef.h>

void ibatchnorm_2d_int32(int32_t *o, int32_t *i, int32_t mean, int32_t invstd, int32_t gamma, int32_t beta, int R, int C)
{
    for (int64_t r = 0; r < R; r++) {

        int32_t *i_row = i + r * C;
        int32_t *o_row = o + r * C;

        int64_t n = C;     
        int32_t *pi = i_row;
        int32_t *po = o_row;

        while (n > 0) {

            size_t vl;

           
            asm volatile("vsetvli %0, %1, e32, m4, ta, ma" : "=r"(vl) : "r"(n));

            asm volatile("vle32.v v0, (%0)" :: "r"(pi));

            asm volatile("vsub.vx v4, v0, %0" :: "r"(mean));

            asm volatile("vmul.vx v8, v4, %0" :: "r"(invstd));

            asm volatile("vmul.vx v12, v8, %0" :: "r"(gamma));

            asm volatile("vadd.vx v16, v12, %0" :: "r"(beta));

            asm volatile("vse32.v v16, (%0)" :: "r"(po));

      
            pi += vl;
            po += vl;
            n  -= vl;
        }
    }
}
