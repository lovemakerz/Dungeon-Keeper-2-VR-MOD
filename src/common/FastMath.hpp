#pragma once
// DK2VR 0.9.47.8 - made by LoVeMaKeRz
// Fast single-precision exp for the menu relief (menuPos): the MinGW libm expf goes through the double-precision x87 exp
// (measured 20 % of the game thread's CPU time in the main menu, report 133308). Same result to ~2 ulp (relative error < 2e-7).
#include <cstdint>
#include <cstring>
#include <cmath>

namespace fastmath {

inline float expf_fast(float x) {
    if(!(x>-87.f)) return x!=x?x:0.f;      // underflow (and NaN passes through)
    if(x>88.f) return INFINITY;
    // x = n*ln2 + r, |r| <= ln2/2 (Cody-Waite two-part ln2)
    const float fn=std::floor(x*1.44269504088896341f+.5f);
    float r=x-fn*0.693359375f;
    r-=fn*(-2.12194440e-4f);
    // exp(r) on [-0.347, 0.347], degree-6 polynomial (Cephes expf coefficients)
    const float r2=r*r;
    float p=1.9875691500E-4f;
    p=p*r+1.3981999507E-3f;
    p=p*r+8.3334519073E-3f;
    p=p*r+4.1665795894E-2f;
    p=p*r+1.6666665459E-1f;
    p=p*r+5.0000001201E-1f;
    p=p*r2+r+1.f;
    const int32_t n=int32_t(fn);
    uint32_t bits=uint32_t(n+127)<<23;      // 2^n (n in [-126, 127] here)
    float scale;std::memcpy(&scale,&bits,4);
    return p*scale;
}

}  // namespace fastmath
