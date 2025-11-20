#include <stdint.h>
#include <stddef.h>


static inline void exp_poly(float *dst, float *src, int N)
{
    int remaining = N;
    float *s = src;
    float *d = dst;

    while (remaining > 0) {

        size_t vl;
        asm volatile("vsetvli %0, %1, e32, m4, ta, ma"
                     : "=r"(vl) : "r"(remaining));

        // v0 = x
        asm volatile("vle32.v v0, (%0)" :: "r"(s));

        // v1 = x*x
        asm volatile("vmul.vv v4, v0, v0");

        // v2 = x*x*x
        asm volatile("vmul.vv v8, v4, v0");

        // v3 = x²/2
        asm volatile("vfmul.vf v12, v4, %0" :: "f"(0.5f));

        // v4 = x³/6
        asm volatile("vfmul.vf v16, v8, %0" :: "f"(0.1666667f));

        // v5 = 1 + x
        asm volatile("vfadd.vf v20, v0, %0" :: "f"(1.0f));

        // v5 += x²/2
        asm volatile("vfadd.vv v20, v20, v12");

        // v5 += x³/6
        asm volatile("vfadd.vv v20, v20, v16");

        // store v5
        asm volatile("vse32.v v20, (%0)" :: "r"(d));

        s += vl;
        d += vl;
        remaining -= vl;
    }
}



void softmax_rvv(const float *input, float *output, int N)
{
    float max_val;
    float sum_val;

    int remaining = N;
    const float *p = input;

    size_t vl;
    asm volatile("vsetvli %0, %1, e32, m8, ta, ma" : "=r"(vl) : "r"(remaining));

    asm volatile("vle32.v v0, (%0)" :: "r"(p));
    p += vl;
    remaining -= vl;

    while (remaining > 0) {

        asm volatile("vsetvli %0, %1, e32, m8, ta, ma" : "=r"(vl) : "r"(remaining));

        asm volatile("vle32.v v8, (%0)" :: "r"(p));
        asm volatile("vmax.vv v0, v0, v8");

        p += vl;
        remaining -= vl;
    }

    asm volatile("vsetvli zero, %0, e32, m8, ta, ma" :: "r"(1));
    asm volatile("vredmax.vs v16, v0, v0");
    asm volatile("vmv.x.s %0, v16" : "=r"(max_val));

    remaining = N;
    p = input;
    float *tmp = output;   

    while (remaining > 0) {

        asm volatile("vsetvli %0, %1, e32, m8, ta, ma" : "=r"(vl) : "r"(remaining));

        asm volatile("vle32.v v0, (%0)" :: "r"(p));

        asm volatile("vfsub.vf v0, v0, %0" :: "f"(max_val));

        asm volatile("vse32.v v0, (%0)" :: "r"(tmp));

        p += vl;
        tmp += vl;
        remaining -= vl;
    }

    exp_poly(output, output, N);

    remaining = N;
    const float *q = output;

    asm volatile("vsetvli %0, %1, e32, m8, ta, ma" : "=r"(vl) : "r"(remaining));

    asm volatile("vle32.v v0, (%0)" :: "r"(q));
    q += vl;
    remaining -= vl;

    while (remaining > 0) {

        asm volatile("vsetvli %0, %1, e32, m8, ta, ma" : "=r"(vl) : "r"(remaining));

        asm volatile("vle32.v v8, (%0)" :: "r"(q));
        asm volatile("vfadd.vv v0, v0, v8");

        q += vl;
        remaining -= vl;
    }

    asm volatile("vsetvli zero, %0, e32, m8, ta, ma" :: "r"(1));
    asm volatile("vredsum.vs v16, v0, v0");
    asm volatile("vmv.x.s %0, v16" : "=r"(sum_val));

    remaining = N;
    float *outp = output;

    while (remaining > 0) {

        asm volatile("vsetvli %0, %1, e32, m8, ta, ma" : "=r"(vl) : "r"(remaining));

        asm volatile("vle32.v v0, (%0)" :: "r"(outp));
        asm volatile("vfdiv.vf v0, v0, %0" :: "f"(sum_val));
        asm volatile("vse32.v v0, (%0)" :: "r"(outp));

        outp += vl;
        remaining -= vl;
    }
}

