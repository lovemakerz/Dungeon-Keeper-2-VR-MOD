// DK2VR 0.9.47.8 - made by LoVeMaKeRz - fast expf must agree with the library exp to float accuracy.
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include "../src/common/FastMath.hpp"
static int failures=0;
#define CHECK(c) do{if(!(c)){std::printf("FAIL line %d: %s\n",__LINE__,#c);++failures;}}while(0)
int main() {
    double worst=0;float worstAt=0;long n=0;
    for(float x=-86.9f;x<88.f;x+=0.00137f) {
        const double ref=std::exp((double)x),got=fastmath::expf_fast(x);
        const double rel=std::fabs(got-ref)/ref;if(rel>worst) {worst=rel;worstAt=x;}++n;
    }
    std::printf("PASS-INFO: %ld points in [-87, 88], worst relative error %.3g at x=%g\n",n,worst,worstAt);
    CHECK(worst<3e-7);
    CHECK(fastmath::expf_fast(0.f)==1.f);
    CHECK(fastmath::expf_fast(-100.f)==0.f);
    CHECK(fastmath::expf_fast(-2250.f)==0.f);
    CHECK(std::isinf(fastmath::expf_fast(100.f)));
    CHECK(std::isnan(fastmath::expf_fast(NAN)));
    // the exact use in the menu relief: depth = Ref + farK*(1-exp(-k*(d-Ref)/farK)), Ref=?, k=.45, farK=2, near .15
    double worstDepth=0;
    for(float rhw=1e-4f;rhw<5.f;rhw*=1.01f) {
        const float d=1.f/std::max(rhw,1e-4f),ref=1.f;
        if(d>=ref) {const float a=2.f*(1.f-std::exp(-.45f*(d-ref)/2.f)),b=2.f*(1.f-fastmath::expf_fast(-.45f*(d-ref)/2.f));worstDepth=std::max<double>(worstDepth,std::fabs(a-b));}
        else {const float c=.15f*(1.f-std::exp(-.45f*(ref-d)/.15f)),e=.15f*(1.f-fastmath::expf_fast(-.45f*(ref-d)/.15f));worstDepth=std::max<double>(worstDepth,std::fabs(c-e));}
    }
    std::printf("PASS-INFO: worst depth difference in the menu relief formula %.3g\n",worstDepth);
    CHECK(worstDepth<2e-6);
    if(failures) {std::printf("FastMathTests: %d failure(s)\n",failures);return 1;}
    std::printf("PASS: FastMathTests (fast expf, menu relief formula)\n");
    return 0;
}
