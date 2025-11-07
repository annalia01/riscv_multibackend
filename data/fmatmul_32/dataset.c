
#include <stdint.h>
#include "runtime.h"
#ifdef SPIKEGEM
#define NR_LANES 8
#endif

uint64_t M = 32;
uint64_t N = 32;
uint64_t P = 32;

float a[32 * 32] __attribute__((aligned(32 * NR_LANES), section(".l2")));
float b[32 * 32] __attribute__((aligned(32 * NR_LANES), section(".l2")));
float c[32 * 32] __attribute__((aligned(32 * NR_LANES), section(".l2")));
float g[32 * 32] __attribute__((aligned(32 * NR_LANES), section(".l2")));

void init_dataset() {
  for (uint64_t i = 0; i < M; ++i) {
    for (uint64_t j = 0; j < N; ++j) {
      a[i * N + j] = (float)(i + j);
    }
  }

  for (uint64_t i = 0; i < N; ++i) {
    for (uint64_t j = 0; j < P; ++j) {
      b[i * P + j] = (float)(i == j);  // matrice identità
    }
  }

  for (uint64_t i = 0; i < M * P; ++i) {
    c[i] = 0.0;
    g[i] = 0.0;
  }

  for (uint64_t i = 0; i < M; ++i) {
    for (uint64_t j = 0; j < P; ++j) {
      for (uint64_t k = 0; k < N; ++k) {
        g[i * P + j] += a[i * N + k] * b[k * P + j];
      }
    }
  }
}
