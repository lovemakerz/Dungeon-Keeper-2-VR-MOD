#pragma once
#include "Math.hpp"
#include <vector>
#include <cstring>
#include <cmath>
#include <algorithm>

namespace dk2vr {
// made by LoVeMaKeRz
// Fit the ORIGINAL UV plane, rather than its camera-rotated screen rectangle.
// Repeated triangle corners must not change the pivot or the apparent roll.
inline Vec3 spriteWorldVertical(const Pose& nativeCamera,Vec3 nativeUp) { return rotate(nativeCamera.orientation,nativeUp); }
struct SpriteSample { Vec3 p; float u,v; };
struct UprightSprite {
    Vec3 center{},bottom{},right{},up{},sourceRight{},sourceDown{};
    float midU=0,midV=0,maxV=0,scaleU=0,scaleV=0;
    bool fit(const std::vector<SpriteSample>& s,Vec3 vertical,Vec3 eye,Vec3 fallbackRight) {
        if(s.size()<3) return false;
        auto dot=[](Vec3 a,Vec3 b){return a.x*b.x+a.y*b.y+a.z*b.z;};
        auto unit=[&](Vec3& a){float q=dot(a,a);if(!std::isfinite(q)||q<1e-12f) return false;a=scale(a,1.f/std::sqrt(q));return true;};
        double mu=0,mv=0;Vec3 mp{};
        float u0=1e9f,u1=-1e9f,v0=1e9f,v1=-1e9f;
        for(const auto& x:s) {
            if(!std::isfinite(x.u)||!std::isfinite(x.v)) return false;
            mu+=x.u;mv+=x.v;mp=add(mp,x.p);
            u0=std::min(u0,x.u);u1=std::max(u1,x.u);v0=std::min(v0,x.v);v1=std::max(v1,x.v);
        }
        if(u1-u0<1e-6f||v1-v0<1e-6f) return false;
        mu/=s.size();mv/=s.size();mp=scale(mp,1.f/float(s.size()));
        double uu=0,uv=0,vv=0;Vec3 pu{},pv{};
        for(const auto& x:s) {float du=x.u-float(mu),dv=x.v-float(mv);Vec3 dp=sub(x.p,mp);
            uu+=du*du;uv+=du*dv;vv+=dv*dv;pu=add(pu,scale(dp,du));pv=add(pv,scale(dp,dv));}
        double det=uu*vv-uv*uv;if(det<=1e-14) return false;
        Vec3 a=add(scale(pu,float(vv/det)),scale(pv,float(-uv/det)));
        Vec3 b=add(scale(pv,float(uu/det)),scale(pu,float(-uv/det)));
        scaleU=std::sqrt(dot(a,a));scaleV=std::sqrt(dot(b,b));
        if(!std::isfinite(scaleU)||!std::isfinite(scaleV)||scaleU<=0||scaleV<=0) return false;
        sourceRight=scale(a,1.f/scaleU);sourceDown=scale(b,1.f/scaleV);
        midU=(u0+u1)*.5f;midV=(v0+v1)*.5f;maxV=v1;
        center=add(mp,add(scale(a,midU-float(mu)),scale(b,midV-float(mv))));
        bottom=add(mp,add(scale(a,midU-float(mu)),scale(b,maxV-float(mv))));
        up=vertical;if(!unit(up)) return false;
        Vec3 toward=sub(eye,center);toward=sub(toward,scale(up,dot(toward,up)));
        right=cross(up,toward);
        if(!unit(right)) {right=sub(fallbackRight,scale(up,dot(fallbackRight,up)));if(!unit(right)) return false;}
        return true;
    }
    Vec3 vertex(float u,float v,bool bottomAnchored) const {
        return add(bottomAnchored?bottom:center,add(scale(right,(u-midU)*scaleU),scale(up,((bottomAnchored?maxV:midV)-v)*scaleV)));
    }
};
struct SpriteCorner { unsigned index;float u,v;Vec3 original{};float sx=0,sy=0; };   // 0.9.36: sx,sy = game screen offset (pixels) from the piece centroid
struct SpriteReadingAxes { Vec3 right,forward; };
// 0.9.47.33: a NUMBER that is born as ONE sprite ("20": two atlas cells side by side, touching). A single UV -> world fit over the two cells is meaningless
// (inside a cell x grows with u, from one cell to the next the second digit sits to the LEFT with a larger u: the fitted u axis comes out near zero, its sign
// and direction flip with the spacing of the two digits). Used as the native axes of a shared plane it turned the number back to front or edge-on in the
// headset (report 113401). Such a piece is laid out from its game-screen picture instead (screenAxes), which reads right from every side.
inline bool glyphCellRow(const std::vector<SpriteCorner>& c) {
    if(c.size()<8||c.size()>16||c.size()%4!=0) return false;
    int cells[4][2];int n=0;
    for(const auto& k:c) {
        if(!std::isfinite(k.u)||!std::isfinite(k.v)||k.u<-0.001f||k.v<-0.001f||k.u>1.001f||k.v>1.001f) return false;
        const int cu=std::min(3,int(k.u*4.f)),cv=std::min(3,int(k.v*4.f));
        const float fu=k.u*4.f-float(cu),fv=k.v*4.f-float(cv);
        if(fu<0.f||fv<0.f) return false;
        bool have=false;for(int i=0;i<n;++i) if(cells[i][0]==cu&&cells[i][1]==cv) have=true;
        if(!have) {if(n>=4) return false;cells[n][0]=cu;cells[n][1]=cv;++n;}
    }
    if(n<2||size_t(n)*4!=c.size()) return false;
    for(int i=0;i<n;++i) {   // every cell has exactly its 4 corners, spanning most of the cell
        float u0=1e9f,u1=-1e9f,v0=1e9f,v1=-1e9f;int cnt=0;
        for(const auto& k:c) if(std::min(3,int(k.u*4.f))==cells[i][0]&&std::min(3,int(k.v*4.f))==cells[i][1]) {++cnt;u0=std::min(u0,k.u);u1=std::max(u1,k.u);v0=std::min(v0,k.v);v1=std::max(v1,k.v);}
        if(cnt!=4||u1-u0<.2f||v1-v0<.2f) return false;
    }
    return true;
}
struct ReplaySprite {
    Vec3 up{};std::vector<SpriteCorner> corners;
    bool sharedPlane=false,hasAxes=false;bool noJoin=false;   // 0.9.47.33: noJoin = several glyph cells in one piece (a number born as one sprite): never given a shared plane
   Vec3 originalCenter{},sourceRight{},sourceDown{},sharedPivot{},sharedRight{},sharedDown{};
    float cx=0,cy=0,width=0,height=0,rhw=0;
    bool screenAxes=false;float pixelWorld=0;   // 0.9.36: rebuild from the game SCREEN layout (glyph orientation as on the monitor), world units per pixel

    bool rebuild(std::vector<unsigned char>& bytes,size_t base,unsigned stride,unsigned count,Vec3 eye,const SpriteReadingAxes* reading=nullptr) const {
        if(stride<12||base+size_t(count)*stride>bytes.size()) return false;
        if(sharedPlane) {
            auto dot=[](Vec3 a,Vec3 b){return a.x*b.x+a.y*b.y+a.z*b.z;};
            Vec3 normal=sub(eye,sharedPivot);float len=std::sqrt(dot(normal,normal));if(!std::isfinite(len)||len<1e-6f)return false;normal=scale(normal,1.f/len);
            Vec3 right=cross(up,normal);len=std::sqrt(dot(right,right));if(len<1e-6f){right=sub(Vec3{1,0,0},scale(normal,normal.x));len=std::sqrt(dot(right,right));}if(len<1e-6f)return false;right=scale(right,1.f/len);Vec3 vertical=cross(normal,right);
            if(reading) {
                // Rotate IN the already validated badge plane only. The projected
                // glyph vertical must keep the same screen X even off-centre.
                const float depth=dot(normal,reading->forward);
                if(std::isfinite(depth)&&std::abs(depth)>1e-5f) {
                    Vec3 xGradient=sub(reading->right,scale(reading->forward,dot(normal,reading->right)/depth));
                    Vec3 v=cross(normal,xGradient);float length=std::sqrt(dot(v,v));
                    if(std::isfinite(length)&&length>1e-6f) {
                        v=scale(v,1.f/length);Vec3 viewerUp=cross(reading->right,reading->forward);
                        if(dot(v,viewerUp)<0)v=scale(v,-1.f);
                        vertical=v;right=cross(vertical,normal);
                    }
                }
            }
            for(auto c:corners)if(c.index>=count)return false;
            // Preserve the native FRONT/BACK separation instead of flattening
            // glyph, disc and ring into one depth-fighting surface.
            Vec3 sourceNormal=cross(sharedDown,sharedRight);float sourceLength=std::sqrt(dot(sourceNormal,sourceNormal));
            if(!std::isfinite(sourceLength)||sourceLength<1e-6f)return false;sourceNormal=scale(sourceNormal,1.f/sourceLength);
            for(auto c:corners){Vec3 delta=sub(c.original,sharedPivot);Vec3 pos=add(sharedPivot,add(add(scale(right,dot(delta,sharedRight)),scale(vertical,-dot(delta,sharedDown))),scale(normal,dot(delta,sourceNormal))));memcpy(bytes.data()+base+size_t(c.index)*stride,&pos,12);}
            return true;
        }
        std::vector<SpriteSample> samples;samples.reserve(corners.size());
        for(auto c:corners) {if(c.index>=count) return false;Vec3 p;memcpy(&p,bytes.data()+base+size_t(c.index)*stride,12);samples.push_back({p,c.u,c.v});}
        UprightSprite frame;if(!frame.fit(samples,up,eye,{1,0,0})) return false;
        auto dot=[](Vec3 a,Vec3 b){return a.x*b.x+a.y*b.y+a.z*b.z;};
        // Icons face the full HMD position, with pitch and yaw but no headset roll.
        Vec3 normal=sub(eye,frame.center);float nl=std::sqrt(dot(normal,normal));
        if(!std::isfinite(nl)||nl<1e-6f) return false;
        normal=scale(normal,1.f/nl);Vec3 right=cross(frame.up,normal);
        float rl=std::sqrt(dot(right,right));
        if(rl<1e-5f) {right=sub(frame.right,scale(normal,dot(frame.right,normal)));rl=std::sqrt(dot(right,right));}
        if(rl<1e-6f) return false;
        frame.right=scale(right,1.f/rl);frame.up=cross(normal,frame.right);
        if(screenAxes&&pixelWorld>0&&std::isfinite(pixelWorld)) {
            // 0.9.36: the numbers above the creatures were lying sideways (report 004324): the texture of a glyph is not always stored with u = screen right,
            // the game turns it with its texture coordinates. The corners are laid out as they are on the game screen (an affine image of the quad, the
            // piece faces the game camera), around their original centroid, in the plane facing the headset.
            Vec3 c0{};for(const auto& c:corners)c0=add(c0,c.original);c0=scale(c0,1.f/float(corners.size()));
            for(auto c:corners) {Vec3 p=add(c0,add(scale(frame.right,c.sx*pixelWorld),scale(frame.up,-c.sy*pixelWorld)));memcpy(bytes.data()+base+size_t(c.index)*stride,&p,12);}
            return true;
        }
        for(auto c:corners) {Vec3 p=frame.vertex(c.u,c.v,false);memcpy(bytes.data()+base+size_t(c.index)*stride,&p,12);}
        return true;
    }
};

}
