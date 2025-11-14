#ifdef SPIKEGEM
#include <stdio.h>
#else 
#include "printf.h"
#endif
#include <stdint.h>
#include <stdlib.h>
#include "runtime.h"

typedef long BLASLONG;

// Prototipo corretto del kernel (FLOAT = float)
extern int CNAME(BLASLONG n, BLASLONG dummy0, BLASLONG dummy1,
                 float da, float *x, BLASLONG inc_x,
                 float *y, BLASLONG inc_y,
                 float *dummy, BLASLONG dummy2);

// CSR minstret (solo Spike)
static inline int64_t read_minstret(void) {
    int64_t value;
    asm volatile ("csrr %0, instret" : "=r"(value));
    return value;
}

int main(void)
{
    BLASLONG n    = 16;
    BLASLONG incx = 1;

    float da = 2.0f;  // fattore di scala

    float *x = malloc(n * sizeof(float));
    if (!x) {
        printf("malloc failed\n");
        return 1;
    }

    // Inizializza x[i] = i+1
    for (BLASLONG i = 0; i < n; i++) {
        x[i] = (float)(i + 1);
    }

    // Parametri dummy (non usati da questo kernel)
    BLASLONG dummy0 = 0;
    BLASLONG dummy1 = 0;
    float   *y      = NULL;
    BLASLONG incy   = 0;
    float   *dummy  = NULL;
    BLASLONG dummy2 = 1;   // != 0 → esegue la scalatura, non azzera

#ifdef SPIKEGEM
    int64_t start_minstret = read_minstret();
#endif

    start_timer();
    CNAME(n, dummy0, dummy1, da, x, incx, y, incy, dummy, dummy2);
    stop_timer();

#ifdef SPIKEGEM
    int64_t end_minstret = read_minstret();
    uint64_t delta_minstret = end_minstret - start_minstret;
#endif

    int64_t runtime = get_timer();

    printf("\nscal_rvv (o kernel equivalente)\n");
    printf("da = %f\n", da);

    printf("x[0..7] dopo la scalatura = { ");
    for (int i = 0; i < 8 && i < n; i++) {
        printf("%f ", x[i]);
    }
    printf("}\n");

    printf("The execution took %ld cycles (timer)\n", runtime);
#ifdef SPIKEGEM
    printf("Instructions retired (CSR minstret): %lu\n", delta_minstret);
#endif

    free(x);
    return 0;
}
