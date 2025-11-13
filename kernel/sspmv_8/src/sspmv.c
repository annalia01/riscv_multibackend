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

void sspmv_32(uint8_t M, uint8_t N, uint8_t rows, uint8_t cols,
              uint8_t *VALUES, uint8_t *col_idx,
              uint8_t *IN_VEC, uint8_t *OUT_VEC) {
    uint8_t nnzpr = (cols / M) * N;
    uint8_t ldi = nnzpr;
    uint8_t *i_ = VALUES;

    for (int i = 0; i < rows; i++) {

        // --- azzera accumulatore scalare ---
        uint32_t acc = 0;   // usa un accumulatore più largo

        int remaining = nnzpr;
        uint8_t offset = 0;

        while (remaining > 0) {
            int vl;
            asm volatile("vsetvli %0, %1, e8, m1, ta, ma"
                         : "=r"(vl) : "r"(remaining) : "memory");

            // === carica blocco di VALUES ===
            asm volatile("vle8.v v0, (%0)" :: "r"(i_ + offset) : "memory");


            // === prepara indici byte ===
            static uint8_t s1_vec[256] __attribute__((aligned(64)));
            for (int j = 0; j < vl; j++) {
                uint8_t block_id = (offset + j) / N;
                uint8_t s1 = col_idx[i * nnzpr + offset + j];
                s1 += block_id * M;
                s1_vec[j] = s1; // offset in byte (ok per e8)
            }
            // === gather e moltiplicazione ===
            asm volatile("vle8.v v12, (%0)" :: "r"(s1_vec) : "memory");
            asm volatile("vluxei8.v v8, (%0), v12" :: "r"(IN_VEC) : "memory");

            asm volatile("vmul.vv v16, v8, v0" ::: "memory");

            // === riduzione del CHUNK (sempre intera) ===
            asm volatile("vmv.v.i v4, 0" ::: "memory");
            asm volatile("vredsum.vs v4, v16, v4" ::: "memory");

            // estrai risultato parziale come intero
            uint32_t part;
            asm volatile("vmv.x.s %0, v4" : "=r"(part));
            acc += part;

            offset += vl;
            remaining -= vl;
        }

        // --- fine riga ---
        OUT_VEC[i] = (uint8_t)acc;  // se ti serve modulo 256

        // avanza il puntatore VALUES alla prossima riga
        asm volatile("add %0, %0, %1" : "+&r"(i_) : "r"(ldi));
    }
}

int sspmv_verify(uint8_t M, uint8_t N, uint8_t rows, uint8_t cols,
                 uint8_t *VALUES, uint8_t *col_idx,
                 uint8_t *IN_VEC, uint8_t *OUT_VEC) {

  uint8_t nnzpr = (cols / M) * N; // numero di non-zero per riga

  for (int32_t i = 0; i < rows; ++i) {
    uint8_t res = OUT_VEC[i];  // risultato del kernel

    uint32_t golden = 0;
    for (int32_t j = 0; j < nnzpr; ++j) {
      uint8_t block_id = j / N;                      // blocco N:M
      uint8_t s1 = col_idx[i * nnzpr + j];           // indice relativo
      s1 += block_id * M;                            // indice reale nella riga
      golden += (uint32_t)VALUES[i * nnzpr + j] * (uint32_t)IN_VEC[s1];
    }

    uint8_t golden_byte = (uint8_t)golden; // stesso “range” del kernel

    // Confronto con risultato kernel
    if (golden_byte != res) {
      printf("Sorry, wrong value! at index %d, result = %d, golden = %d\n",
             i, res, golden_byte);
      return i; // ritorna la riga dove fallisce
    }
  }

  return 0; // tutto corretto
}
