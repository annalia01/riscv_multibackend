#include <stdint.h>
#include <stddef.h>

void avgpool2x2(const int32_t *in, int H, int W, int32_t *out)
{
    int H_out = H / 2;
    int W_out = W / 2;

    for (int r = 0; r < H_out; r++) {

        const int32_t *row0 = in + (2 * r) * W;
        const int32_t *row1 = in + (2 * r + 1) * W;

        int32_t *out_row = out + r * W_out;

        for (int c = 0; c < W_out; c++) {

            const int32_t *p0 = row0 + 2 * c;   
            const int32_t *p1 = row1 + 2 * c;   

            size_t vl;

           
            asm volatile("vsetvli %0, %1, e32, m4, ta, ma"
                        : "=r"(vl)
                        : "r"(2));

            
            asm volatile("vle32.v v0, (%0)" :: "r"(p0));

            
            asm volatile("vle32.v v4, (%0)" :: "r"(p1));

            
            asm volatile("vadd.vv v8, v0, v4");

            
            asm volatile("vmv.v.i v12, 0");
            asm volatile("vredsum.vs v12, v8, v12");

            int32_t sum;
            asm volatile("vmv.x.s %0, v12" : "=r"(sum));


            out_row[c] = sum/4;
        }
    }
}
