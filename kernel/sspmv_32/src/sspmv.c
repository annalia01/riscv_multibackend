/*
 * Vector kernel implemented for the thesis work of <Annalia Ruggiero>.
 *
 * The algorithm is based on the S3 scheme described in:
 * <Titopoulos; Alexandridis; Peltekis; Nicopoulos; Dimitrakopoulos>,
 * <Optimizing Structured-Sparse Matrix Multiplication in RISC-V Vector Processors>, <2025>.
 *
 *
 */

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

void sspmv_32(int32_t M, int32_t N, int32_t rows, int32_t cols,
              float *VALUES, int32_t *col_idx,
              float *IN_VEC, float *OUT_VEC) {

    int32_t nnzpr = (cols / M) * N;
    int32_t ldi = nnzpr << 2;
    float *i_ = VALUES;

    for (int i = 0; i < rows; i++) {

        float acc = 0.0f;

        int remaining = nnzpr;
        int offset = 0;

        while (remaining > 0) {
            int vl;
            asm volatile("vsetvli %0, %1, e32, m4, ta, ma"
                         : "=r"(vl) : "r"(remaining) : "memory");
            asm volatile("vle32.v v0, (%0)" :: "r"(i_ + offset) : "memory");

            static int32_t s1_vec[256] __attribute__((aligned(64)));
            for (int j = 0; j < vl; j++) {
                int32_t block_id = (offset + j) / N;
                int32_t s1 = col_idx[i * nnzpr + offset + j];
                s1 += block_id * M;
                s1_vec[j] = s1 << 2; // offset in byte
            }

            asm volatile("vle32.v v12, (%0)" :: "r"(s1_vec) : "memory");
            asm volatile("vluxei32.v v8, (%0), v12" :: "r"(IN_VEC) : "memory");
            asm volatile("vfmul.vv v16, v8, v0" ::: "memory");

            asm volatile("vmv.v.i v4, 0" ::: "memory");
            asm volatile("vfredsum.vs v4, v16, v4" ::: "memory");

            float part;
            asm volatile("vfmv.f.s %0, v4" : "=f"(part));
            acc += part;

            offset += vl;
            remaining -= vl;
        }

        OUT_VEC[i] = acc;
        asm volatile("add %0, %0, %1" : "+&r"(i_) : "r"(ldi));
    }
}

int sspmv_verify(int32_t M, int32_t N, int32_t rows, int32_t cols,
                 float * VALUES, int32_t * col_idx,
                 float * IN_VEC, float * OUT_VEC) {

  int32_t nnzpr = (cols / M) * N; 

  for (int32_t i = 0; i < rows; ++i) {
    float res = OUT_VEC[i]; 

    float golden = 0.0;
    for (int32_t j = 0; j < nnzpr; ++j) {
      int32_t block_id = j / N;                      
      int32_t s1 = col_idx[i * nnzpr + j];           
      s1 += block_id * M;                            
      golden += VALUES[i * nnzpr + j] * IN_VEC[s1]; 
    }


    if (fabsf(golden - res) > 1e-3f) {
      printf("Sorry, wrong value! at index %d, result = %f, golden = %f\n",
             i, res, golden);
      return i; 
    }
  }

  return 0; 
}
