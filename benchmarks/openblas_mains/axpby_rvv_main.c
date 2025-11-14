#ifdef SPIKEGEM
#include <stdio.h>
#else 
#include "printf.h"
#endif
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "runtime.h"

typedef long BLASLONG;

// Prototipo corretto del kernel AXBPY OpenBLAS
extern int CNAME(BLASLONG n,
                 float alpha, float *x, BLASLONG inc_x,
                 float beta,  float *y, BLASLONG inc_y);

// CSR minstret
static inline int64_t read_minstret(void) {
    int64_t value;
    asm volatile ("csrr %0, instret" : "=r"(value));
    return value;
}

int main(void)
{
    BLASLONG n = 16;
    BLASLONG incx = 1;
    BLASLONG incy = 1;

    float alpha = 2.0f;
    float beta  = 3.0f;

    float *x = malloc(n * sizeof(float));
    float *y = malloc(n * sizeof(float));

    if (!x || !y) {
        printf("malloc failed\n");
        return 1;
    }

    // Inizializzazione x[i] = i+1, y[i] = (10-i)
    for (BLASLONG i = 0; i < n; i++) {
        x[i] = (float)(i + 1);
        y[i] = (float)(10 - i);
    }

#ifdef SPIKEGEM
    int64_t start_minstret = read_minstret();
#endif

    start_timer();
    CNAME(n, alpha, x, incx, beta, y, incy);
    stop_timer();

#ifdef SPIKEGEM
    int64_t end_minstret = read_minstret();
    uint64_t delta_minstret = end_minstret - start_minstret;
#endif

    int64_t cycles = get_timer();

    printf("\naxpby_rvv\n");
    printf("alpha = %f, beta = %f\n", alpha, beta);

    // Mostra i primi 8 valori di y come conferma
    printf("y[0..7] = { ");
    for (int i = 0; i < 8 && i < n; i++)
        printf("%f ", y[i]);
    printf("}\n");

    printf("The execution took %ld cycles (timer)\n", cycles);

#ifdef SPIKEGEM
    printf("Instructions retired (CSR minstret): %lu\n", delta_minstret);
#endif

    free(x);
    free(y);
    return 0;
}
