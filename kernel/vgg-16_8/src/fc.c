#include <stdint.h>
#include <stddef.h>


void fc(int8_t *mat, int8_t *vec, int rows, int cols, int8_t *out_vec)
{
    for(int r = 0; r < rows; r++) {

        int8_t *row_ptr = mat + r * cols;
        int8_t acc = 0;

        int c = 0;
        while(c < cols) {

            size_t vl;
            asm volatile("vsetvli %0, %1, e8, m8, ta, ma"
                        : "=r"(vl)
                        : "r"(cols - c));
            asm volatile("vle8.v v0, (%0)" :: "r"(row_ptr + c));

            asm volatile("vle8.v v8, (%0)" :: "r"(vec + c));

            asm volatile("vmul.vv v16, v0, v8");

            asm volatile("vmv.v.i v24, 0");
            asm volatile("vredsum.vs v24, v16, v24");

            int8_t tmp;
            asm volatile("vmv.x.s %0, v24" : "=r"(tmp));

            acc += tmp;
            c += vl;
        }

        out_vec[r] = acc;
    }
}


void add_bias_rvv(int8_t *out, const int8_t *bias, int N)
{
    int remaining = N;
    int offset = 0;

    while (remaining > 0) {
        size_t vl;

        asm volatile("vsetvli %0, %1, e8, m8, ta, ma" : "=r"(vl) : "r"(remaining));

        const int8_t *out_ptr  = out  + offset;
        const int8_t *bias_ptr = bias + offset;

        asm volatile("vle8.v v0, (%0)" :: "r"(out_ptr));
        asm volatile("vle8.v v8, (%0)" :: "r"(bias_ptr));

        // v0 = v0 + v4
        asm volatile("vadd.vv v16, v0, v8");

        // Salva risultato
        asm volatile("vse8.v v16, (%0)" :: "r"(out_ptr));

        offset += vl;
        
        remaining -= vl;
    }
}
