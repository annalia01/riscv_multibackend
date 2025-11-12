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

#include "sspmv.h"
#include "runtime.h"
#include "util.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#ifdef SPIKEGEM
#include <stdio.h>
#else 
#include "printf.h"
#endif

void sspmv_32(int32_t M, int32_t N, int32_t rows, int32_t cols, const float * VALUES, const int32_t * col_idx, const float * IN_VEC, float * OUT_VEC) {
int32_t nnzpr = (cols/M)*N;
asm volatile("vsetvli zero, %0, e32, m2, ta, ma" :: "r"(nnzpr));
int32_t ldi = nnzpr << 2;
float *i_=VALUES;
  
for(int i=0; i<rows; i++) {
asm volatile("vle32.v v0,  (%0); add %0, %0, %1" : "+&r"(i_) : "r"(ldi));
asm volatile("vmv.v.i v4,0"); 
for(int j=0; j<(cols/M)*N; j++) {
int32_t block_id=j/N;
int32_t s1=col_idx[i*nnzpr+j];
s1+=block_id*M;
int32_t offset = s1 << 2;
asm volatile("vmv.v.x v12, %0" :: "r"(offset));
asm volatile("vloxei32.v v8, (%0), v12" :: "r"(IN_VEC));
asm volatile("vfmul.vv v16, v8, v0"); // moltiplicazione 
asm volatile("vfredsum.vs v4, v16, v4"); // riduzione
}
float result;
asm volatile("vfmv.f.s %0, v16" : "=f"(result));
OUT_VEC[i] = result;

}
}

int sspmv_verify(int32_t M, int32_t N, int32_t rows, int32_t cols,
                 const float *VALUES, const int32_t *col_idx,
                 const float *IN_VEC, const float *OUT_VEC) {

  int32_t nnzpr = (cols / M) * N; // numero di non-zero per riga

  for (int32_t i = 0; i < rows; ++i) {
    float res = OUT_VEC[i];  // risultato del kernel

    float golden = 0.0;
    for (int32_t j = 0; j < nnzpr; ++j) {
      int32_t block_id = j / N;                      // blocco N:M
      int32_t s1 = col_idx[i * nnzpr + j];           // indice relativo
      s1 += block_id * M;                            // indice reale nella riga
      golden += VALUES[i * nnzpr + j] * IN_VEC[s1];  // accumula prodotto
    }

    // Confronto con risultato kernel
    if (fabsf(golden - res) > 1e-3f) {
      printf("Sorry, wrong value! at index %d, result = %f, golden = %f\n",
             i, res, golden);
      return i; // ritorna la riga dove fallisce
    }
  }

  return 0; // tutto corretto
}


