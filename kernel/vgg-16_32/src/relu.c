#include <stdint.h>
#include <stddef.h>

void relu(int32_t *data, int N) {

    int idx = 0;

    while (idx < N) {

        size_t vl;

        asm volatile("vsetvli %0, %1, e32, m8"
                     : "=r"(vl)
                     : "r"(N - idx));

        asm volatile("vle32.v v0, (%0)"
                     :
                     : "r"(data + idx));

        asm volatile("vmv.v.i v8, 0");

        asm volatile("vmax.vv v16, v0, v8");

        asm volatile("vse32.v v16, (%0)"
                     :
                     : "r"(data + idx));

        idx += vl;
    }
}
