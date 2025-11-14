#ifdef SPIKEGEM
#include <stdio.h>
#else 
#include "printf.h"
#endif
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "runtime.h"

typedef long BLASLONG;

// Prototipo corretto del kernel nrm2_rvv
extern float CNAME(BLASLONG n, float *x, BLASLONG inc_x);

// CSR minstret (solo Spike)
static inline int64_t read_minstret(void) {
    int64_t value;
    asm volatile ("csrr %0, instret" : "=r"(value));
    return value;
}

int main(void)
{
    BLASLONG n    = 32;
    BLASLONG incx = 1;

    float *x = malloc(n * sizeof(float));
    if (!x) {
        printf("malloc failed\n");
        return 1;
    }

    // Inizializzazione: x[i] = i+1 (1,2,...,32)
    for (BLASLONG i = 0; i < n; i++) {
        x[i] = (float)(i + 1);
    }

#ifdef SPIKEGEM
    int64_t start_minstret = read_minstret();
#endif

    start_timer();
    float result = CNAME(n, x, incx);
    stop_timer();

#ifdef SPIKEGEM
    int64_t end_minstret = read_minstret();
    uint64_t delta_minstret = end_minstret - start_minstret;
#endif

    int64_t runtime = get_timer();

    printf("\nnrm2_rvv\n");
    printf("result = %f\n", result);
    printf("The execution took %ld cycles (timer)\n", runtime);
#ifdef SPIKEGEM
    printf("Instructions retired (CSR minstret): %lu\n", delta_minstret);
#endif

    free(x);
    return 0;
}
