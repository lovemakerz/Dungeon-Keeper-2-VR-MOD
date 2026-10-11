#pragma once
#include <array>
#include <algorithm>
#include <cstdint>
#include <cmath>
namespace dk2vr {
struct Cadence {
    std::array<float,512> intervals{};uint32_t stored=0,seen=0,over22=0,over50=0;double total=0,last=0;float worst=0;
    void sample(double now){
        if(last>0&&now>=last) {
            const float ms=float(now-last);
            if(std::isfinite(ms)&&ms>0) {intervals[seen%intervals.size()]=ms;stored=std::min<uint32_t>(stored+1,intervals.size());++seen;total+=ms;worst=std::max(worst,ms);over22+=ms>22.22f;over50+=ms>50;}
        }
        last=now;
    }
    float percentile(float p) const {if(!stored)return 0;auto copy=intervals;std::sort(copy.begin(),copy.begin()+stored);return copy[size_t((stored-1)*p)];}
    void clearWindow(){stored=seen=over22=over50=0;total=0;worst=0;}
    void pause(){last=0;}
};
}
