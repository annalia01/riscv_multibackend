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

#include "sspgemm.h"
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


void sspgemm_32(int32_t M, int32_t N,
                int32_t rows, int32_t cols,
                float *VALUES, int32_t *col_idx,
                float *B, int32_t cols_b,
                float *C) {

    int32_t nnzpr = (cols / M) * N;  

    for (int32_t i = 0; i < rows; i++) {

        float *c_row = &C[i * cols_b];

      
        for (int32_t k = 0; k < cols_b; ) {

            int32_t remaining = cols_b - k;
            int32_t vl;

            asm volatile("vsetvli %0, %1, e32, m1, ta, ma"
                         : "=r"(vl)
                         : "r"(remaining));

            asm volatile("vmv.v.i v0, 0");

            for (int32_t j = 0; j < nnzpr; j++) {

                int32_t block_id = j / N;
                int32_t s1 = col_idx[i * nnzpr + j];
                s1 += block_id * M; 

                float *b_ptr = &B[s1 * cols_b + k];  

                asm volatile("vle32.v v4, (%0)" :: "r"(b_ptr));

                float s0 = VALUES[i * nnzpr + j];

                asm volatile("vfmacc.vf v0, %0, v4" :: "f"(s0));
            }

            float *c_ptr = c_row + k;
            asm volatile("vse32.v v0, (%0)" :: "r"(c_ptr));
            k += vl;
        }
    }
}

int sspgemm_verify_matrix(int32_t M, int32_t N, int32_t rows, int32_t cols, float *VALUES, int32_t *col_idx, float *B, int32_t cols_b, float *OUT) 
{
    int32_t nnzpr = (cols / M) * N; 
    for (int32_t i = 0; i < rows; i++) {

        for (int32_t k = 0; k < cols_b; k++) {

            float golden = 0.0f;
            for (int32_t j = 0; j < nnzpr; j++) {

                int32_t block_id = j / N;

                int32_t s1 = col_idx[i * nnzpr + j];
                s1 += block_id * M;

                float a_ij = VALUES[i * nnzpr + j];

                golden += a_ij * B[s1 * cols_b + k];
            }

            float res = OUT[i * cols_b + k];

            if (fabsf(golden - res) > 1e-3f) {
                printf("Wrong value at C[%d,%d]: result=%f, golden=%f\n",
                       i, k, res, golden);
                return i * cols_b + k; 
            }
        }
    }

    return 0; 
}
