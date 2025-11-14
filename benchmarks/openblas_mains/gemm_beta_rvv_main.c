#ifdef SPIKEGEM
#include <stdio.h>
#else 
#include "printf.h"
#endif
#include <stdint.h>
#include <stdlib.h>
#include "runtime.h"

typedef long BLASLONG;

// Prototipo corretto del kernel gemm_beta
extern int CNAME(BLASLONG m, BLASLONG n,
                 BLASLONG dummy1, float beta,
                 float *dummy2, BLASLONG dummy3,
                 float *dummy4, BLASLONG dummy5,
                 float *c, BLASLONG ldc);

// CSR minstret (solo su Spike)
static inline int64_t read_minstret(void) {
    int64_t value;
    asm volatile ("csrr %0, instret" : "=r"(value));
    return value;
}

int main(void)
{
    // dimensioni esempio
    BLASLONG m = 8;
    BLASLONG n = 8;
    BLASLONG ldc = n;

    float beta = 2.0f;

    float *C = malloc(m * n * sizeof(float));
    if (!C) {
        printf("malloc failed\n");
        return 1;
    }

    // Inizializzazione: C[i,j] = i+j
    for (BLASLONG i = 0; i < m; i++) {
        for (BLASLONG j = 0; j < n; j++) {
            C[i * ldc + j] = (float)(i + j);
        }
    }

#ifdef SPIKEGEM
    int64_t start_minstret = read_minstret();
#endif

    start_timer();
    CNAME(m, n, 0, beta, NULL, 0, NULL, 0, C, ldc);
    stop_timer();

#ifdef SPIKEGEM
    int64_t end_minstret = read_minstret();
    uint64_t delta_minstret = end_minstret - start_minstret;
#endif

    int64_t cycles = get_timer();

    printf("\n==== gemm_beta_rvv ====\n");
    printf("beta = %f\n", beta);
    printf("C after scaling:\n");

    for (BLASLONG i = 0; i < m; i++) {
        for (BLASLONG j = 0; j < n; j++) {
            printf("%6.1f ", C[i * ldc + j]);
        }
        printf("\n");
    }

    printf("\nThe execution took %ld cycles (timer)\n", cycles);

#ifdef SPIKEGEM
    printf("Instructions retired (CSR minstret): %lu\n", delta_minstret);
#endif

    free(C);
    return 0;
}
