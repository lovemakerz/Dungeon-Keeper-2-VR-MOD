#include "../src/common/FrameGuard.hpp"
#include <cassert>
#include <cstdio>
using namespace dk2vr;
int main(){
    assert(projectedTriangleArea(0,0,2,0,0,2)==2);
    assert(projectedTriangleArea(0,0,0,2,2,0)==2);
    assert(projectedTriangleArea(0,0,0,0,0,0)==0);
    assert(projectedTriangleArea(NAN,0,0,2,2,0)==0);
    PossessionFrameGuard g;
    assert(g.accept(4,100));assert(!g.accept(.2f,110));assert(!g.accept(.2f,159));
    assert(g.accept(.2f,160)); // real scene change is never frozen forever
    assert(g.accept(4,170));assert(!g.accept(.2f,180));assert(g.accept(4,190));
    g.reset();assert(g.accept(.01f,191));
    assert(g.accept(3,200));assert(g.accept(2,210)); // moderate variation is untouched
    puts("PASS: projected coverage and bounded partial-frame hold.");
}
