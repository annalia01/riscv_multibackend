// benchmarks/openblas_mains/amax_rvv_main.c
#ifdef SPIKEGEM
#include <stdio.h>
#else 
#include "printf.h"
#endif
#include <stdint.h>
#include <stdlib.h>
#include "runtime.h"   // start_timer(), stop_timer(), get_timer()
#include "util.h"

typedef long BLASLONG;

// Prototipo corretto di amax_rvv
extern float CNAME(BLASLONG n, float *x, BLASLONG inc_x);

// CSR per Spike
static inline int64_t read_minstret(void) {
    int64_t value;
    asm volatile ("csrr %0, instret" : "=r"(value));
    return value;
}

int main(void)
{
    BLASLONG n = 32;
    BLASLONG inc_x = 1;

    float *x = malloc(sizeof(float) * n);
    if (!x) {
        printf("malloc failed\n");
        return 1;
    }

    // Inizializza il vettore con valori positivi/negativi
    for (BLASLONG i = 0; i < n; i++) {
        if (i % 2 == 0)
            x[i] = -(float)(i + 1);
        else
            x[i] =  (float)(i + 1);
    }

    // ===== MISURE =====
#ifdef SPIKEGEM
    int64_t start_minstret = read_minstret();
#endif

    start_timer();

    float result = CNAME(n, x, inc_x);

    stop_timer();

#ifdef SPIKEGEM
    int64_t end_minstret = read_minstret();
    uint64_t delta_minstret = end_minstret - start_minstret;
#endif

    int64_t runtime = get_timer();


    // ===== OUTPUT =====
    printf("\namax_rvv\n");
    printf("result = %f\n", result);
    printf("The execution took %ld cycles (timer)\n", runtime);

#ifdef SPIKEGEM
    printf("Instructions retired (CSR minstret): %lu\n", delta_minstret);
#endif

    free(x);
    return 0;
}
