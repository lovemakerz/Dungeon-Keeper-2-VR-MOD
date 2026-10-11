#pragma once
#include <cmath>
#include <cstdint>
namespace dk2vr {
inline float projectedTriangleArea(float ax,float ay,float bx,float by,float cx,float cy) {
    const float area=std::fabs((bx-ax)*(cy-ay)-(by-ay)*(cx-ax))*.5f;
    return std::isfinite(area)?area:0.f;
}
// A short hold, never an unlimited freeze. Only compare consecutive possession frames.
struct PossessionFrameGuard {
    float previous=0;uint64_t lastGood=0;uint32_t rejected=0;
    void reset(){previous=0;lastGood=0;}
    bool accept(float coverage,uint64_t now) {
        if(previous>.5f&&coverage<previous*.25f&&now>=lastGood&&now-lastGood<60) {
            ++rejected;return false;
        }
        previous=std::isfinite(coverage)?coverage:0.f;lastGood=now;return true;
    }
};
}
