#include <stdint.h>
#include <stddef.h>

void ibatchnorm_2d(int8_t *o, int8_t *i, int8_t mean, int8_t invstd, int8_t gamma, int8_t beta, int R, int C)
{
    for (int64_t r = 0; r < R; r++) {

        int8_t *i_row = i + r * C;
        int8_t *o_row = o + r * C;

        int n = C;     
        int8_t *pi = i_row;
        int8_t *po = o_row;

        while (n > 0) {

            size_t vl;

           
            asm volatile("vsetvli %0, %1, e8, m4, ta, ma" : "=r"(vl) : "r"(n));

            asm volatile("vle8.v v0, (%0)" :: "r"(pi));

            asm volatile("vsub.vx v4, v0, %0" :: "r"(mean));

            asm volatile("vmul.vx v8, v4, %0" :: "r"(invstd));

            asm volatile("vmul.vx v12, v8, %0" :: "r"(gamma));

            asm volatile("vadd.vx v16, v12, %0" :: "r"(beta));

            asm volatile("vse8.v v16, (%0)" :: "r"(po));

      
            pi += vl;
            po += vl;
            n  -= vl;
        }
    }
}
