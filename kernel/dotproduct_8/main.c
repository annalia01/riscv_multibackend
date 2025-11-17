
// Copyright 2020 ETH Zurich and University of Bologna.
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

// Author: Matteo Perotti <mperotti@iis.ee.ethz.ch>

#include <stdint.h>
#include <string.h>

#include "runtime.h"

#include "dotproduct.h"

#include "util.h"

#ifdef SPIKEGEM
#include <stdio.h>
#define NR_LANES 8
#else
#include "printf.h"
#endif
inline int64_t read_minstret(void) {
    int64_t value;
    asm volatile ("csrr %0, instret"
                  : "=r"(value));
    return value;
}
// Run also the scalar benchmark
#define SCALAR 0

// Check the vector results against golden vectors
#define CHECK 0

// Vector size (Byte)
extern uint64_t vsize;
// Vectors for benchmarks

extern int8_t v8a[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int8_t v8b[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
// Output vectors

extern int8_t res8_v, res8_s;

int main() {
  printf("\n");
  printf("==========\n");
  printf("=  DOTP  =\n");
  printf("==========\n");
  printf("\n");
  printf("\n");

  int64_t runtime_s, runtime_v;


  for (uint64_t avl = 8; avl <= (vsize >> 0); avl *= 8) {
    // Dotp
    printf("Calulating 8b dotp with vectors with length = %lu\n", avl);
    #ifdef SPIKEGEM
    int64_t start_minstret = read_minstret();
    #endif
    start_timer();
    res8_v = dotp_v8b(v8a, v8b, avl);
    stop_timer();
    #ifdef SPIKEGEM
    int64_t end_minstret = read_minstret();
    uint64_t delta_minstret = end_minstret - start_minstret;
    #endif
    runtime_v = get_timer();
    printf("Vector runtime: %ld\n", runtime_v);
    #ifdef SPIKEGEM
    printf("Instructions retired (CSR minstret): %lu\n", delta_minstret);
    #endif
    if (SCALAR) {
      start_timer();
      res8_s = dotp_s8b(v8a, v8b, avl);
      stop_timer();
      runtime_s = get_timer();
      printf("Scalar runtime: %ld\n", runtime_s);

      if (CHECK) {
        if (res8_v != res8_s) {
          printf("Error: v = %ld, g = %ld\n", res8_v, res8_s);
          return -1;
        }
      }
    }
  }

  printf("SUCCESS.\n");

  return 0;
}
