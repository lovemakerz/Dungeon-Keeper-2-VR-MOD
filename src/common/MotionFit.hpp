#pragma once
// DK2VR 0.8.68 - possession rendered at the headset rate: measure how the game's world moved between two game frames.
// Pure mathematics (shared by the VR thread and the tests). Everything is in the horizontal plane of the VR space.
// Model, same convention as yawQuat()/dungeonCorrection(): b = R(phi) * a + t, positions relative to the turn pivot,
// R(phi): x' = x*cos(phi) + z*sin(phi), z' = -x*sin(phi) + z*cos(phi)   (= rotate(yawQuat(phi), v) with y = 0).
#include "Math.hpp"
#include <vector>

namespace dk2vr {
struct PlanarPair {float ax,az,bx,bz,w;};   // position before (a) and after (b), relative to the pivot, weight
struct PlanarFit {
    bool ok=false;float phi=0,tx=0,tz=0;     // rotation (rad) then translation (same unit as the positions)
    int pairs=0,inliers=0;float share=0,rms=0,spread=0;
};
inline Vec3 planarRotate(float phi,float x,float z) {const float c=std::cos(phi),s=std::sin(phi);return {x*c+z*s,0,-x*s+z*c};}
// Robust rigid fit (iterated, residual-trimmed). 'tolerance': smallest inlier radius; 'minInliers' / 'minShare': acceptance;
// 'minSpread': the points must surround the pivot widely enough for the rotation to be determined;
// 'maxRms': residual of the accepted fit (a real rigid motion fits to millimetres, random pairs never do).
inline PlanarFit fitPlanar(const std::vector<PlanarPair>& pairs,float tolerance,int minInliers,float minShare,float minSpread,float maxRms=1e9f) {
    PlanarFit r;r.pairs=int(pairs.size());
    if(pairs.size()<size_t(std::max(3,minInliers))) return r;
    std::vector<char> in(pairs.size(),1);std::vector<float> res(pairs.size(),0.f);
    float phi=0,tx=0,tz=0;int count=int(pairs.size());
    for(int iter=0;iter<6;++iter) {
        double W=0,cax=0,caz=0,cbx=0,cbz=0;
        for(size_t i=0;i<pairs.size();++i) if(in[i]) {const double w=pairs[i].w;W+=w;cax+=w*pairs[i].ax;caz+=w*pairs[i].az;cbx+=w*pairs[i].bx;cbz+=w*pairs[i].bz;}
        if(W<=0) return r;
        cax/=W;caz/=W;cbx/=W;cbz/=W;
        double dot=0,crs=0,var=0;
        for(size_t i=0;i<pairs.size();++i) if(in[i]) {
            const double w=pairs[i].w,ax=pairs[i].ax-cax,az=pairs[i].az-caz,bx=pairs[i].bx-cbx,bz=pairs[i].bz-cbz;
            dot+=w*(ax*bx+az*bz);crs+=w*(az*bx-ax*bz);var+=w*(ax*ax+az*az);
        }
        r.spread=float(std::sqrt(var/W));
        phi=(std::fabs(dot)+std::fabs(crs)<1e-12)?0.f:float(std::atan2(crs,dot));
        const Vec3 rc=planarRotate(phi,float(cax),float(caz));
        tx=float(cbx)-rc.x;tz=float(cbz)-rc.z;
        for(size_t i=0;i<pairs.size();++i) {
            const Vec3 q=planarRotate(phi,pairs[i].ax,pairs[i].az);
            const float ex=q.x+tx-pairs[i].bx,ez=q.z+tz-pairs[i].bz;res[i]=std::sqrt(ex*ex+ez*ez);
        }
        std::vector<float> tmp;tmp.reserve(pairs.size());
        for(size_t i=0;i<pairs.size();++i) if(in[i]) tmp.push_back(res[i]);
        if(tmp.empty()) return r;
        std::nth_element(tmp.begin(),tmp.begin()+tmp.size()/2,tmp.end());
        const float limit=std::min(std::max(tolerance,2.5f*tmp[tmp.size()/2]),std::max(tolerance,maxRms*2.f));
        int now=0;bool changed=false;
        for(size_t i=0;i<pairs.size();++i) {const char v=res[i]<=limit?1:0;if(v!=in[i]) changed=true;in[i]=v;now+=v;}
        count=now;
        if(!changed||count<std::max(3,minInliers)) break;
    }
    double sum=0;int n=0;for(size_t i=0;i<pairs.size();++i) if(in[i]) {sum+=double(res[i])*res[i];++n;}
    r.inliers=n;r.share=pairs.empty()?0.f:float(n)/float(pairs.size());r.rms=n?float(std::sqrt(sum/n)):0.f;
    r.phi=phi;r.tx=tx;r.tz=tz;
    r.ok=n>=minInliers&&r.share>=minShare&&r.spread>=minSpread&&r.rms<=maxRms&&std::isfinite(phi)&&std::isfinite(tx)&&std::isfinite(tz);
    (void)count;
    return r;
}
}
