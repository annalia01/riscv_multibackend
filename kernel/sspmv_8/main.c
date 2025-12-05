/*
 * Vector kernel implemented for the thesis work of <Annalia Ruggiero>.
 *
 * The algorithm is based on the S3 scheme described in:
 * <Titopoulos; Alexandridis; Peltekis; Nicopoulos; Dimitrakopoulos>,
 * <Optimizing Structured-Sparse Matrix Multiplication in RISC-V Vector Processors>, <2025>.
 *
 *
 */

#include <stdint.h>
#include <string.h>

#include "sspmv.h"
#include "runtime.h"
#include "util.h"

#ifdef SPIKEGEM
#include <stdio.h>
#define NR_LANES 8
#else 
#include "printf.h"
#endif 

extern uint8_t cols;
extern uint8_t rows;
extern uint8_t M;
extern uint8_t N;

extern uint8_t VALUES[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern uint8_t col_idx[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern uint8_t IN_VEC[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern uint8_t OUT_VEC[] __attribute__((aligned(32 * NR_LANES), section(".l2")));

static inline uint64_t read_minstret(void) {
    uint64_t value;
    asm volatile ("csrr %0, instret" : "=r"(value));
    return value;
}

int main() {
  printf("\n");
  printf("==========\n");
  printf("=  SpMV  =\n");
  printf("==========\n");
  printf("\n");
  printf("\n");
  

  printf("\n");
  printf(
      "-------------------------------------------------------------------\n");
  printf(
      "Calculating a (%d x %d) x %d sparse matrix vector multiplication...\n",
      rows, cols);
 
  printf(
      "-------------------------------------------------------------------\n");
  printf("\n");

  printf("calculating ... \n");
  #ifdef SPIKEGEM
  uint64_t start_minstret = read_minstret();
  #endif 
  start_timer();
  sspmv_32(M, N, rows, cols, VALUES, col_idx, IN_VEC, OUT_VEC);
  stop_timer();
  #ifdef SPIKEGEM
  uint64_t end_minstret = read_minstret();
  uint64_t delta_minstret = end_minstret - start_minstret;
  #endif
  // Metrics
  int64_t runtime = get_timer();
    
  #ifdef SPIKEGEM
  printf("Instructions retired (CSR minstret): %lu\n", delta_minstret);
  #endif
  printf("The execution took %d cycles.\n", runtime);


  printf("Verifying ...\n");
  if (sspmv_verify(M, N, rows, cols, VALUES, col_idx, IN_VEC, OUT_VEC)) {
    return 1;
  } else {
    printf("Passed.\n");
  }
  return 0;
}
