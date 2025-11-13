void sspgemm_32(
    int32_t M, int32_t N,
    int32_t rows, int32_t cols,
    float *VALUES, int32_t *col_idx,
    float *IN_VEC,
    float *B, int32_t cols_b,   
    float *C                
) {

    int32_t nnzpr = (cols / M) * N;   
    asm volatile("vsetvli zero, %0, e32, m2, ta, ma" :: "r"(cols_b));

    for (int i = 0; i < rows; i++) {

        // v2 = 0
        asm volatile("vmv.v.i v2, 0");

        for (int j = 0; j < nnzpr; j++) {

            int32_t block_id = j / N;
            int32_t s1 = col_idx[i * nnzpr + j];

            s1 += block_id * M;

            float *b_ptr = &B[s1 * cols_b];
            asm volatile("vle32.v v1, (%0)" :: "r"(b_ptr));

            float s0 = VALUES[i * nnzpr + j];

            asm volatile("vfmacc.vf v2, %0, v1" :: "f"(s0));
        }

        float *c_ptr = &C[i * cols_b];
        asm volatile("vse32.v v2, (%0)" :: "r"(c_ptr));
    }
}
