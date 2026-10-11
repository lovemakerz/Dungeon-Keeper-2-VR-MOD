// DK2VR 0.9.47.10 - made by LoVeMaKeRz - the native draw gate drops a call only when every argument matches, never leaks, and switches itself off on anomalies.
#include <cstdio>
#include "../src/common/NativeSkip.hpp"
static int failures=0;
#define CHECK(c) do{if(!(c)){std::printf("FAIL line %d: %s\n",__LINE__,#c);++failures;}}while(0)
using namespace nativeskip;
static Call plain(const void* v,uint32_t n=10,uint32_t st=32,uint32_t type=4){Call c;c.kind=0;c.type=type;c.primitives=n;c.stride=st;c.verts=v;return c;}
static Call indexed(const void* v,const void* ix,uint32_t n=10){Call c;c.kind=1;c.type=4;c.primitives=n;c.stride=32;c.verts=v;c.indices=ix;c.minIndex=0;c.numVertices=30;c.indexFormat=101;return c;}
int main() {
    int a=0,b=0,i1=0,i2=0;
    {   // armed + same call = dropped, once
        Gate g;g.arm(plain(&a));
        CHECK(g.take(plain(&a)));CHECK(!g.take(plain(&a)));CHECK(g.dropped==1&&g.stale==0&&g.mismatched==0);
    }
    {   // nothing armed: nothing dropped
        Gate g;CHECK(!g.take(plain(&a)));CHECK(g.dropped==0);
    }
    {   // every field of a plain call matters
        Gate g;
        g.arm(plain(&a));CHECK(!g.take(plain(&b)));
        g.arm(plain(&a));CHECK(!g.take(plain(&a,11)));
        g.arm(plain(&a));CHECK(!g.take(plain(&a,10,36)));
        g.arm(plain(&a));CHECK(!g.take(plain(&a,10,32,5)));
        CHECK(g.dropped==0&&g.mismatched==4);
        Call k=indexed(&a,&i1);g.arm(plain(&a));CHECK(!g.take(k));   // different kind
    }
    {   // every field of an indexed call matters
        Gate g;Call base=indexed(&a,&i1);
        g.arm(base);CHECK(g.take(base));
        Call x=base;x.indices=&i2;g.arm(base);CHECK(!g.take(x));
        x=base;x.minIndex=1;g.arm(base);CHECK(!g.take(x));
        x=base;x.numVertices=31;g.arm(base);CHECK(!g.take(x));
        x=base;x.indexFormat=102;g.arm(base);CHECK(!g.take(x));
        x=base;x.verts=&b;g.arm(base);CHECK(!g.take(x));
        CHECK(g.dropped==1&&g.mismatched==5);
    }
    {   // a gate that was armed and never consumed is cleared and counted
        Gate g;g.arm(plain(&a));g.clear();CHECK(!g.isArmed&&g.stale==1);
        g.clear();CHECK(g.stale==1);   // clearing an empty gate is free
        g.arm(plain(&a));g.arm(plain(&b));CHECK(g.stale==2&&g.isArmed);CHECK(g.take(plain(&b)));
    }
    {   // too many anomalies: the gate switches itself off
        Gate g;for(int n=0;n<200&&g.active();++n){g.arm(plain(&a));g.clear();}
        CHECK(!g.active()&&g.stale>=Gate::AnomalyLimit);
    }
    {   // long clean run: nothing is ever counted as an anomaly
        Gate g;for(int n=0;n<100000;++n){Call c=(n&1)?indexed(&a,&i1,uint32_t(n)):plain(&b,uint32_t(n));g.clear();g.arm(c);if(!g.take(c)) {CHECK(false);break;}}
        CHECK(g.active()&&g.dropped==100000&&g.stale==0&&g.mismatched==0);
    }
    {   // lean mode: only after VerifyDrops confirmed drops without any anomaly, and never again after an anomaly
        Gate g;CHECK(!g.lean());
        for(uint64_t n=0;n<Gate::VerifyDrops-1;++n){g.arm(plain(&a));g.take(plain(&a));}
        CHECK(!g.lean());g.arm(plain(&a));g.take(plain(&a));CHECK(g.lean());
        g.arm(plain(&a));g.take(plain(&b));CHECK(!g.lean()&&g.active());   // one anomaly: back to the careful mode, still skipping
        for(int n=0;n<1000;++n){g.arm(plain(&a));g.take(plain(&a));}
        CHECK(!g.lean());
        Gate h;for(uint64_t n=0;n<Gate::VerifyDrops;++n){h.arm(plain(&a));h.take(plain(&a));}
        CHECK(h.lean());h.arm(plain(&a));h.clear();CHECK(!h.lean());
    }
    if(failures) {std::printf("NativeSkipTests: %d failure(s)\n",failures);return 1;}
    std::printf("PASS: NativeSkipTests (gate drops only an exact match, no leak, self-disables)\n");
    return 0;
}
