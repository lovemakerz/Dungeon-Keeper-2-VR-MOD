#include "../src/common/MirrorLayout.hpp"
#include <cassert>
#include <cstdio>
using namespace dk2vr;
int main() {
    {   // Quest 3 eye (2064x2208) in 1280x720, fill: covers the whole window, aspect kept, centred crop
        const MirrorRect r=mirrorLayout(1280,720,2064,2208,true);
        assert(std::fabs(r.w/r.h-2064.f/2208.f)<1e-4f);
        assert(std::fabs(r.w-1280)<1e-2f&&std::fabs(r.x)<1e-2f);
        assert(r.y<=0.f&&r.y+r.h>=720.f-1e-2f);
        assert(std::fabs(r.y-(720-r.h)*.5f)<1e-2f);
    }
    {   // crop centre moved up / down: never outside the image
        const MirrorRect up=mirrorLayout(1280,720,2064,2208,true,0.f),dn=mirrorLayout(1280,720,2064,2208,true,1.f);
        assert(std::fabs(up.y)<1e-2f&&std::fabs(dn.y+dn.h-720)<1e-2f);
    }
    {   // fit (whole eye visible): bars left and right, aspect kept
        const MirrorRect r=mirrorLayout(1280,720,2064,2208,false);
        assert(std::fabs(r.h-720)<1e-2f&&std::fabs(r.w/r.h-2064.f/2208.f)<1e-4f&&std::fabs(r.x-(1280-r.w)*.5f)<1e-2f&&r.w<1280);
    }
    {   // 16:9 eye in a 16:9 window: exact both ways
        const MirrorRect a=mirrorLayout(1280,720,1600,900,true),b=mirrorLayout(1280,720,1600,900,false);
        assert(std::fabs(a.w-1280)<1e-2f&&std::fabs(a.h-720)<1e-2f&&std::fabs(b.w-1280)<1e-2f);
    }
    {const MirrorRect r=mirrorLayout(0,0,0,0,true);assert(r.w==0&&r.h==0);}
    {int w=1280,h=500;mirrorKeep169(w,h,false);assert(w==1280&&h==720);w=500;h=720;mirrorKeep169(w,h,true);assert(h==720&&w==1280);w=10;h=10;mirrorKeep169(w,h,false);assert(w==320&&h==180);}
    puts("MirrorLayoutTests OK");
}
