#include <stdint.h>
#include <stddef.h>
#include <stdio.h>


static const float exp2_lut[256] = {
    1.000000f,1.002711f,1.005430f,1.008156f,1.010889f,1.013630f,1.016378f,1.019134f,
    1.021897f,1.024668f,1.027446f,1.030231f,1.033025f,1.035825f,1.038634f,1.041450f,
    1.044274f,1.047105f,1.049945f,1.052792f,1.055647f,1.058510f,1.061381f,1.064260f,
    1.067146f,1.070041f,1.072944f,1.075854f,1.078773f,1.081700f,1.084634f,1.087577f,
    1.090528f,1.093487f,1.096454f,1.099429f,1.102412f,1.105404f,1.108403f,1.111411f,
    1.114427f,1.117451f,1.120484f,1.123525f,1.126574f,1.129631f,1.132697f,1.135771f,
    1.138854f,1.141945f,1.145044f,1.148152f,1.151268f,1.154393f,1.157526f,1.160668f,
    1.163818f,1.166977f,1.170144f,1.173320f,1.176504f,1.179697f,1.182899f,1.186109f,
    1.189328f,1.192555f,1.195791f,1.199036f,1.202289f,1.205551f,1.208822f,1.212102f,
    1.215390f,1.218687f,1.221993f,1.225307f,1.228631f,1.231963f,1.235304f,1.238653f,
    1.242012f,1.245379f,1.248755f,1.252140f,1.255534f,1.258937f,1.262348f,1.265769f,
    1.269198f,1.272637f,1.276084f,1.279540f,1.283005f,1.286480f,1.289963f,1.293455f,
    1.296956f,1.300466f,1.303985f,1.307513f,1.311051f,1.314597f,1.318152f,1.321716f,
    1.325289f,1.328872f,1.332463f,1.336063f,1.339673f,1.343291f,1.346919f,1.350556f,
    1.354202f,1.357857f,1.361521f,1.365195f,1.368877f,1.372569f,1.376270f,1.379980f,
    1.383699f,1.387428f,1.391165f,1.394912f,1.398668f,1.402433f,1.406208f,1.409992f,
    1.413785f,1.417587f,1.421399f,1.425220f,1.429050f,1.432890f,1.436739f,1.440597f,
    1.444464f,1.448341f,1.452227f,1.456122f,1.460026f,1.463940f,1.467863f,1.471796f,
    1.475738f,1.479689f,1.483650f,1.487620f,1.491599f,1.495588f,1.499586f,1.503593f,
    1.507610f,1.511636f,1.515672f,1.519717f,1.523771f,1.527835f,1.531908f,1.535991f,
    1.540083f,1.544184f,1.548295f,1.552416f,1.556546f,1.560685f,1.564834f,1.568992f,
    1.573159f,1.577336f,1.581523f,1.585718f,1.589924f,1.594138f,1.598362f,1.602596f,
    1.606839f,1.611091f,1.615353f,1.619625f,1.623906f,1.628196f,1.632496f,1.636806f,
    1.641125f,1.645453f,1.649791f,1.654138f,1.658495f,1.662862f,1.667237f,1.671623f,
    1.676017f,1.680422f,1.684835f,1.689258f,1.693691f,1.698134f,1.702586f,1.707047f,
    1.711518f,1.715999f,1.720489f,1.724989f,1.729498f,1.734017f,1.738545f,1.743083f,
    1.747631f,1.752188f,1.756755f,1.761332f,1.765918f,1.770513f,1.775118f,1.779733f,
    1.784357f,1.788991f,1.793635f,1.798288f,1.802951f,1.807624f,1.812306f,1.816998f,
    1.821700f,1.826411f,1.831132f,1.835863f,1.840604f,1.845354f,1.850115f,1.854884f,
    1.859664f,1.864454f,1.869253f,1.874062f,1.878881f,1.883710f,1.888548f,1.893396f
};

#define LOG2_E      1.4426950408889634f
#define LN2         0.6931471805599453f
#define CLAMP_LOW  (-10.0f)
#define CLAMP_HIGH (10.0f)



static void exp_rvv(float *dst, const float *src, int N)
{
    int remaining = N;
    const float *s = src;
    float *d = dst;

    while (remaining > 0)
    {
        size_t vl;
        asm volatile("vsetvli %0, %1, e32, m2"
                     : "=r"(vl) : "r"(remaining));

        asm volatile("vle32.v v0, (%0)" :: "r"(s));

        asm volatile("vfmul.vf v4, v0, %0" :: "f"(LOG2_E));

        asm volatile("vfcvt.x.f.v v8, v4");   
        asm volatile("vfcvt.f.x.v v12, v8");   

       
        asm volatile("vfsub.vv v16, v4, v12");

  
        asm volatile("vfmul.vf v20, v16, %0" :: "f"(255.0f));
        asm volatile("vfcvt.xu.f.v v24, v20");


        asm volatile("vluxei32.v v28, (%0), v24" :: "r"(exp2_lut));

        asm volatile("vmv.v.i v4, 0");
        asm volatile("vmv.v.i v8, 0");
        asm volatile("vmv.v.i v16, 0");
        asm volatile("vmv.v.i v20, 0");
        asm volatile("vmv.v.i v24, 0");
        asm volatile("vfmul.vf v4, v12, %0" :: "f"(LN2)); 
        asm volatile("vfmul.vv v8, v4, v4");             
        asm volatile("vfmul.vf v16, v8, %0" :: "f"(0.5f)); 

        asm volatile("vfadd.vf v20, v4, %0" :: "f"(1.0f));

        asm volatile("vfadd.vv v24, v20, v16");             

        asm volatile("vfmul.vv v0, v28, v24");

        asm volatile("vse32.v v0, (%0)" :: "r"(d));

        s += vl;
        d += vl;
        remaining -= vl;
    }
}


void softmax_rvv(const float *input, float *output, int N)
{
    float max_val, sum_val;

    // ------------------------------
    // 1) MAX
    // ------------------------------
    int remaining = N;
    const float *p = input;
    size_t vl;

    asm volatile("vsetvli %0, %1, e32, m4"
                 : "=r"(vl) : "r"(remaining));
    asm volatile("vle32.v v0, (%0)" :: "r"(p));

    p += vl;
    remaining -= vl;

    while (remaining > 0) {
        asm volatile("vsetvli %0, %1, e32, m4"
                     : "=r"(vl) : "r"(remaining));
        asm volatile("vle32.v v4, (%0)" :: "r"(p));
        asm volatile("vmax.vv v0, v0, v4");
        p += vl;
        remaining -= vl;
    }

    asm volatile("vsetvli zero, %0, e32, m4" :: "r"(1));
    asm volatile("vredmax.vs v8, v0, v0");
    asm volatile("vmv.x.s %0, v8" : "=r"(max_val));


    remaining = N;
    p = input;
    float *tmp = output;

    while (remaining > 0) {
        asm volatile("vsetvli %0, %1, e32, m4"
                     : "=r"(vl) : "r"(remaining));

        asm volatile("vle32.v v0, (%0)" :: "r"(p));

        asm volatile("vfsub.vf v0, v0, %0" :: "f"(max_val));
        asm volatile("vfmax.vf v0, v0, %0" :: "f"(CLAMP_LOW));
        asm volatile("vfmin.vf v0, v0, %0" :: "f"(CLAMP_HIGH));

        asm volatile("vse32.v v0, (%0)" :: "r"(tmp));

        p += vl;
        tmp += vl;
        remaining -= vl;
    }

    exp_rvv(output, output, N);

    remaining = N;
    const float *q = output;

    asm volatile("vsetvli %0, %1, e32, m4"
                 : "=r"(vl) : "r"(remaining));
    asm volatile("vle32.v v0, (%0)" :: "r"(q));

    q += vl;
    remaining -= vl;

    while (remaining > 0) {
        asm volatile("vsetvli %0, %1, e32, m4"
                     : "=r"(vl) : "r"(remaining));
        asm volatile("vle32.v v4, (%0)" :: "r"(q));
        asm volatile("vfadd.vv v0, v0, v4");
        q += vl;
        remaining -= vl;
    }

    asm volatile("vsetvli zero, %0, e32, m4" :: "r"(1));
    asm volatile("vredsum.vs v8, v0, v0");
    asm volatile("vmv.x.s %0, v8" : "=r"(sum_val));

    remaining = N;
    float *outp = output;

    while (remaining > 0) {
        asm volatile("vsetvli %0, %1, e32, m4"
                     : "=r"(vl) : "r"(remaining));

        asm volatile("vle32.v v0, (%0)" :: "r"(outp));
        asm volatile("vfdiv.vf v0, v0, %0" :: "f"(sum_val));
        asm volatile("vse32.v v0, (%0)" :: "r"(outp));

        outp += vl;
        remaining -= vl;
    }
}
