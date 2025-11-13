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

#ifndef _SSPGEMM_H
#define _SSPGEMM_H
#include <stdint.h>

void sspgemm_32(uint8_t M, uint8_t N, uint8_t rows, uint8_t cols, uint8_t *VALUES, uint8_t *col_idx, uint8_t *B, uint8_t cols_b, uint8_t *C);
int sspgemm_verify_matrix(uint8_t M, uint8_t N, uint8_t rows, uint8_t cols, uint8_t *VALUES, uint8_t *col_idx, uint8_t *B, uint8_t cols_b, uint8_t *OUT);

#endif
