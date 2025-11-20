#include <stdint.h>
#include <stddef.h>

void relu(int8_t *data, int N) {

    int idx = 0;

    while (idx < N) {

        size_t vl;

        asm volatile("vsetvli %0, %1, e8, m8, ta, ma"
                     : "=r"(vl)
                     : "r"(N - idx));

        asm volatile("vle8.v v0, (%0)"
                     :
                     : "r"(data + idx));

        asm volatile("vmv.v.i v8, 0");

        asm volatile("vmax.vv v16, v0, v8");

        asm volatile("vse8.v v16, (%0)"
                     :
                     : "r"(data + idx));

        idx += vl;
    }
}
