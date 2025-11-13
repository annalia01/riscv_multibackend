// Copyright 2022 ETH Zurich and University of Bologna.
//
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Author: Chi Zhang, ETH Zurich <chizhang@iis.ee.ethz.ch>

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
