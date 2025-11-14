#ifdef SPIKEGEM
#include <stdio.h>
#else 
#include "printf.h"
#endif
#include <stdint.h>
#include <stdlib.h>
#include "runtime.h"

typedef long BLASLONG;

// Prototipo del kernel copy_rvv
extern int CNAME(BLASLONG n, float *x, BLASLONG inc_x,
                               float *y, BLASLONG inc_y);

// CSR minstret (solo per Spike)
static inline int64_t read_minstret(void) {
    int64_t value;
    asm volatile ("csrr %0, instret" : "=r"(value));
    return value;
}

int main(void)
{
    BLASLONG n    = 32;
    BLASLONG incx = 1;
    BLASLONG incy = 1;

    float *x = malloc(n * sizeof(float));
    float *y = malloc(n * sizeof(float));

    if (!x || !y) {
        printf("malloc failed\n");
        return 1;
    }

    // x[i] = i+1
    for (BLASLONG i = 0; i < n; i++) {
        x[i] = (float)(i + 1);
    }

    // y[i] = -100 - i (per vedere la copia)
    for (BLASLONG i = 0; i < n; i++) {
        y[i] = -100.0f - (float)i;
    }

#ifdef SPIKEGEM
    int64_t start_minstret = read_minstret();
#endif

    start_timer();
    CNAME(n, x, incx, y, incy);
    stop_timer();

#ifdef SPIKEGEM
    int64_t end_minstret = read_minstret();
    uint64_t delta_minstret = end_minstret - start_minstret;
#endif

    int64_t runtime = get_timer();

    printf("\n==== copy_rvv ====\n");

    printf("x[0..7] = { ");
    for (int i = 0; i < 8 && i < n; i++)
        printf("%f ", x[i]);
    printf("}\n");

    printf("y[0..7] after copy = { ");
    for (int i = 0; i < 8 && i < n; i++)
        printf("%f ", y[i]);
    printf("}\n");

    printf("\nExecution time: %ld cycles\n", runtime);

#ifdef SPIKEGEM
    printf("Instructions retired (CSR minstret): %lu\n", delta_minstret);
#endif

    free(x);
    free(y);
    return 0;
}
