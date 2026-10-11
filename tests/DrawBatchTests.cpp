#include "../src/common/DrawBatch.hpp"
#include <cassert>
#include <cstdio>
#include <vector>
using namespace dk2vr;
struct R {int type=4;unsigned primitives=1,count=3,stride=16,fvf=42;size_t vertices=0,indices=0;int indexFormat=16;bool indexed=true;unsigned snapshot=1;bool hand=false,held3d=false;};
int main(){
    for(int format:{16,32}) {
        R a,b;a.indexFormat=b.indexFormat=format;b.vertices=48;b.indices=format==16?6:12;
        std::vector<unsigned char> bytes(format==16?12:24);
        unsigned size=format==16?2:4;
        for(unsigned i=0;i<6;++i) {uint32_t n=i%3;memcpy(bytes.data()+i*size,&n,size);}
        assert(mergeDraw(a,b,bytes.data(),4,16));assert(a.count==6&&a.primitives==2);
        for(unsigned i=0;i<6;++i) {uint32_t n=0;memcpy(&n,bytes.data()+i*size,size);assert(n==i);}
    }
    R a,b;a.indexed=b.indexed=false;b.vertices=48;
    assert(mergeDraw(a,b,nullptr,4,16));assert(a.count==6);
    a=R{};b=R{};b.vertices=48;b.indices=6;
    unsigned char bad[12]{};uint16_t n=9;memcpy(bad+6,&n,2);
    assert(!mergeDraw(a,b,bad,4,16));assert(a.count==3);uint16_t v;memcpy(&v,bad+6,2);assert(v==9);
    b.snapshot=2;assert(!mergeDraw(a,b,bad,4,16));b.snapshot=1;
    b.hand=true;assert(!mergeDraw(a,b,bad,4,16));b.hand=false;
    b.held3d=true;assert(!mergeDraw(a,b,bad,4,16));b.held3d=false;
    b.type=5;assert(!mergeDraw(a,b,bad,4,16));
    puts("PASS: draw merge preserves indexed triangle order, 16/32-bit indices, boundaries and hand separation.");
}
