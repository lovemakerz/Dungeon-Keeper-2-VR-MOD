#pragma once
#include "Math.hpp"
#include <cmath>
#include <vector>
namespace dk2vr {
// Use one shared Z origin for the complete held creature. Subtracting each
// sub-model's own origin makes its limbs drift away from the torso in VR.
inline bool heldDepthPixels(float projectedInputZ,float commonOriginZ,float xyScale,float zScale,float& result) {
    if(!std::isfinite(xyScale)||!std::isfinite(zScale)||std::fabs(zScale)<1e-8f||xyScale==0)return false;
    result=(projectedInputZ-commonOriginZ)*(xyScale/zScale);
    return std::isfinite(result)&&std::fabs(result)<100000.f;
}
inline Vec3 heldModelPoint(float sx,float sy,float depth,float cursorX,float cursorY) {
    return {sx-cursorX,-(sy-cursorY),-depth};
}
inline float heldIconSplit(float configured,float modelRight) {return std::max(configured,modelRight);}
inline bool heldNeedsUpright(float width,float height) {return std::isfinite(width)&&std::isfinite(height)&&width>80.f&&width>height*1.5f;}
inline Vec3 heldUprightPoint(Vec3 p,float cx,float cy) {return {cx-(p.y-cy),cy+(p.x-cx),p.z};}

// 0.9.47.22: the axes of the flat held window (billboard facing the head, vertical kept). The 3D model uses the SAME frame, so x / y (the game's own
// screen coordinates: orientation and swing) land exactly where the flat image drew them; only the depth axis (toward the head) is new.
inline float heldDot(Vec3 a,Vec3 b) {return a.x*b.x+a.y*b.y+a.z*b.z;}
inline bool heldWindowAxes(Vec3 hot,Vec3 head,Vec3& right,Vec3& up,Vec3& to) {
    to=sub(head,hot);float l=std::sqrt(heldDot(to,to));if(!std::isfinite(l)||l<1e-4f) return false;to=scale(to,1/l);
    right=cross({0,1,0},to);float rl=std::sqrt(heldDot(right,right));if(!std::isfinite(rl)||rl<1e-4f) return false;right=scale(right,1/rl);
    up=cross(to,right);return true;
}
// World position of a model point (pixels relative to the hotspot, y up, z toward the head) in that frame.
inline Vec3 heldWorldPoint(Vec3 hot,Vec3 right,Vec3 up,Vec3 to,float metresPerPixel,Vec3 p) {
    return add(hot,scale(add(add(scale(right,p.x),scale(up,p.y)),scale(to,p.z)),metresPerPixel));
}
// Direction of the depth axis, measured from the game's own data: the held creature is depth-tested by the game with the z of its screen vertices.
// If model depth and that z grow together and ZFUNC is LESS/LESSEQUAL, a larger depth is farther away (+1: toward-head axis = -depth).
// Weak or missing correlation: the previous value is kept.
struct HeldDepthSign {
    double n=0,sa=0,sb=0,saa=0,sbb=0,sab=0;
    void add(float depthPixels,float gameZ) {if(!std::isfinite(depthPixels)||!std::isfinite(gameZ)) return;const double a=depthPixels,b=gameZ;++n;sa+=a;sb+=b;saa+=a*a;sbb+=b*b;sab+=a*b;}
    // returns the new sign (+1/-1) when the correlation is clear, otherwise 'previous'; 'corr' receives the correlation (0 when unknown)
    int result(bool nearIsSmaller,int previous,double& corr) const {
        corr=0;if(n<6) return previous;
        const double va=saa/n-(sa/n)*(sa/n),vb=sbb/n-(sb/n)*(sb/n),cv=sab/n-(sa/n)*(sb/n);
        if(!(va>1.0)||!(vb>1e-12)) return previous;
        corr=cv/std::sqrt(va*vb);if(!(std::fabs(corr)>.5)) return previous;
        return (corr>0?1:-1)*(nearIsSmaller?1:-1);
    }
};

// ---------------------------------------------------------------------------------------------------------------------------------------------
// 0.9.47.23 - DEPTH DIRECTION FROM THE FACES OF THE MODEL (replaces the 0.9.47.22 correlation with the game's z, which never became strong).
// The game draws a creature with its own culling rule (0x58B9D0): a triangle is visible when (x1-x0)*(y2-y0) - (y1-y0)*(x2-x0) >= 0 (screen, y down).
// Those 'front' triangles are the ones turned toward the viewer, so on a closed model they are NEARER than the 'back' ones (that the game also
// submits since DrawBackFaces=1). The correlation between 'front/back' and the model depth gives the direction of the depth axis, whatever the game
// does with its z buffer. sign +1 = a larger model depth is FARTHER from the viewer (toward-the-viewer axis = -depth), -1 = nearer.
inline float heldTriArea(const float* a,const float* b,const float* c) {return (b[0]-a[0])*(c[1]-a[1])-(b[1]-a[1])*(c[0]-a[0]);}
struct HeldFaceVote {
    double n=0,sa=0,sd=0,saa=0,sdd=0,sad=0;
    size_t front=0,back=0;
    void add(float area,float depth) {
        if(!std::isfinite(area)||!std::isfinite(depth)||std::fabs(area)<.5f) return;
        const double a=area>=0.f?1.0:-1.0,d=depth;++n;sa+=a;sd+=d;saa+=a*a;sdd+=d*d;sad+=a*d;if(a>0) ++front;else ++back;
    }
    // correlation front(+1)/back(-1) against depth; 0 when there are too few triangles, only one kind of face, or a flat model
    double correlation(double minTriangles=12.0) const {
        if(n<minTriangles||front<3||back<3) return 0;
        const double va=saa/n-(sa/n)*(sa/n),vd=sdd/n-(sd/n)*(sd/n),cv=sad/n-(sa/n)*(sd/n);
        if(!(va>1e-9)||!(vd>1.0)) return 0;
        const double c=cv/std::sqrt(va*vd);return std::isfinite(c)?c:0;
    }
};
// Decision kept across calls and frames (the convention of the engine is one and the same for every creature). Evidence fades slowly (0.98 per model call).
struct HeldDepthDecider {
    double score=0,weight=0;int sign=0;   // 0 = undecided
    void add(const HeldFaceVote& v) {
        const double c=v.correlation();if(c==0) return;
        score=score*.98+c*v.n;weight=weight*.98+v.n;
        if(weight<150) return;
        const double mean=score/weight;const int want=mean<0?1:-1;
        if(sign==0) {if(std::fabs(mean)>=.10) sign=want;}
        else if(want!=sign&&std::fabs(mean)>=.18) sign=want;
    }
};

// ---------------------------------------------------------------------------------------------------------------------------------------------
// 0.9.47.24 - from report 081721 (0.9.47.23): the held creatures were still a MIX of flat window and 3D pieces (jagged 2D edges next to polygons). Two
// changes, both here:
//  (1) depth of EVERY vertex comes from the z the game writes in the vertex itself (a linear function of the model depth); the native hooks are only
//      used to measure the slope (pixels of depth per unit of z) - heldSlopeFit - so no triangle depends on a hook match any more;
//  (2) depth direction by OVERLAP: where a front triangle (the game's own rule: area >= 0) and a back triangle cover the same screen point, the front
//      one must be nearer. The 0.9.47.23 comparison of MEAN depths was wrong on open or thin models (the spear of the elf gave the opposite answer).
struct HeldTri {float x[3],y[3],d[3];};
struct HeldOverlapVote {double nearSmaller=0,nearLarger=0;};   // front triangle at a SMALLER depth than the back one behind it / at a larger one
inline bool heldInTriangle(float px,float py,const HeldTri& t,float& depth) {
    const float d=(t.y[1]-t.y[2])*(t.x[0]-t.x[2])+(t.x[2]-t.x[1])*(t.y[0]-t.y[2]);
    if(std::fabs(d)<1e-6f) return false;
    const float a=((t.y[1]-t.y[2])*(px-t.x[2])+(t.x[2]-t.x[1])*(py-t.y[2]))/d,b=((t.y[2]-t.y[0])*(px-t.x[2])+(t.x[0]-t.x[2])*(py-t.y[2]))/d,c=1.f-a-b;
    if(a<0.f||b<0.f||c<0.f) return false;
    depth=a*t.d[0]+b*t.d[1]+c*t.d[2];return true;
}
inline void heldOverlapVote(const std::vector<HeldTri>& tris,HeldOverlapVote& vote,size_t cap=300) {
    std::vector<const HeldTri*> front,back;
    for(const auto& t:tris) {
        float a[2]={t.x[0],t.y[0]},b[2]={t.x[1],t.y[1]},c[2]={t.x[2],t.y[2]};const float area=heldTriArea(a,b,c);
        if(!std::isfinite(area)||std::fabs(area)<.5f) continue;
        if(area>=0.f) {if(front.size()<cap) front.push_back(&t);} else if(back.size()<cap) back.push_back(&t);
    }
    auto test=[&](const HeldTri& probe,const HeldTri& other,bool probeIsFront) {
        const float cx=(probe.x[0]+probe.x[1]+probe.x[2])/3.f,cy=(probe.y[0]+probe.y[1]+probe.y[2])/3.f,cd=(probe.d[0]+probe.d[1]+probe.d[2])/3.f;
        float od=0;if(!heldInTriangle(cx,cy,other,od)) return;
        const float df=probeIsFront?cd:od,db=probeIsFront?od:cd;   // front depth, back depth
        if(df<db-1.f) vote.nearSmaller+=1.0;else if(df>db+1.f) vote.nearLarger+=1.0;
    };
    for(const auto* f:front) for(const auto* b:back) {test(*f,*b,true);test(*b,*f,false);}
}
// Decision kept across draws and frames; sign +1 = a larger depth is FARTHER. Evidence fades slowly; flips only with strong repeated opposite evidence.
struct HeldSignDecider {
    double score=0,weight=0;int sign=0;
    void add(const HeldOverlapVote& v) {
        const double w=v.nearSmaller+v.nearLarger;if(w<=0) return;
        score=score*.98+(v.nearSmaller-v.nearLarger);weight=weight*.98+w;
        if(weight<30) return;
        const double mean=score/weight;const int want=mean>0?1:-1;
        if(sign==0) {if(std::fabs(mean)>=.3) sign=want;}
        else if(want!=sign&&std::fabs(mean)>=.5) sign=want;
    }
};
// Slope of the model depth (pixels) against the z the game writes in the vertex, through the vertices whose depth is known from the native hooks.
inline bool heldSlopeFit(const float* z,const float* depth,size_t n,double& slope,double& corr) {
    if(n<6) return false;
    double mz=0,md=0;for(size_t i=0;i<n;++i) {mz+=z[i];md+=depth[i];}mz/=n;md/=n;
    double zz=0,dd=0,zd=0;for(size_t i=0;i<n;++i) {const double a=z[i]-mz,b=depth[i]-md;zz+=a*a;dd+=b*b;zd+=a*b;}
    if(!(zz>1e-14)||!(dd>1.0)) return false;
    slope=zd/zz;corr=zd/std::sqrt(zz*dd);return std::isfinite(slope)&&std::isfinite(corr);
}
}
