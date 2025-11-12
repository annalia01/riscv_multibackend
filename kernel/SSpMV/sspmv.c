void sspmv_32(int32_t M, int32_t N, int32_t C, int32_t Q, const int32_t * VALUES) {
int32_t nnzpr = M*N;
asm volatile("vsetvli zero, %0, e32, m2, ta, ma" : "r"(nnzpr));
int32_t ldi = Q << 2;
int32_t *i=values;
  
for(int i=0; i<C; i++) {
asm volatile("vle32.v v0,  (%0); add %0, %0, %1" : "+&r"(values) : "r"(ldi));
asm volatile("vmv.v.i v3,0"); 
}
for(int j=0; j<(Q/M)*N; j++) {

}

}
