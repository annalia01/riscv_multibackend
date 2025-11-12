void sspmv_32(int32_t M, int32_t N, int32_t rows, int32_t cols, const float * VALUES, const int32_t * col_idx, const float * IN_VEC) {
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
