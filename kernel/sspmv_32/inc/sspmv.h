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

#ifndef _SSPMV_H
#define _SSPMV_H
#include <stdint.h>
void sspmv_32(int32_t M, int32_t N, int32_t rows, int32_t cols, const float * VALUES, const int32_t * col_idx, const float * IN_VEC, float * OUT_VEC);
int sspmv_verify(int32_t M, int32_t N, int32_t rows, int32_t cols,
                 const float *VALUES, const int32_t *col_idx,
                 const float *IN_VEC, const float *OUT_VEC);
#endif
