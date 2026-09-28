#pragma once
#include "Math.hpp"
#include <array>

namespace dk2vr {
inline float dot(Vec3 a, Vec3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
inline bool normalize(Vec3& v) {
    float length=std::sqrt(dot(v,v));
    if(!std::isfinite(length)||length<1e-6f) return false;
    v=scale(v,1/length);return true;
}
struct Plane {
    Vec3 normal{};float offset=0;
    float distance(Vec3 p) const {return dot(normal,p)+offset;}
};
// A ground-aligned rectangle in the reconstructed native camera coordinates.
// Its dimensions depend on the native camera, never on the headset pose.
struct Tabletop {
    Vec3 center{},right{},forward{},up{};
    float halfWidth=0,halfDepth=0;
    bool valid=false;
    Vec3 point(float x,float z,float height=0) const {
        return add(center,add(scale(right,x),add(scale(forward,z),scale(up,height))));
    }
    std::array<Plane,4> planes() const {
        return {{{right,halfWidth-dot(right,center)},
                 {scale(right,-1),halfWidth+dot(right,center)},
                 {forward,halfDepth-dot(forward,center)},
                 {scale(forward,-1),halfDepth+dot(forward,center)}}};
    }
};
inline Plane transformPlane(const Plane& plane,const Pose& from,const Pose& to) {
    Vec3 worldNormal=rotate(from.orientation,plane.normal);
    return {rotate(conjugate(to.orientation),worldNormal),
            plane.offset+dot(worldNormal,sub(to.position,from.position))};
}
// Same rectangle expressed with another unit scale (directions unchanged).
inline Tabletop scaled(Tabletop t,float factor) {
    t.center=scale(t.center,factor);t.halfWidth*=factor;t.halfDepth*=factor;
    t.valid=t.valid&&std::isfinite(factor)&&factor>0;return t;
}
// Pose of the native camera in the table frame: origin = rectangle centre on the
// dungeon floor, +X = native screen right, +Y = floor normal (up), +Z = towards
// the player (opposite of the native viewing direction projected on the floor).
// world = compose(tablePose, nativeCameraInTable(t)) then toWorld(world, cameraPoint).
inline Pose nativeCameraInTable(const Tabletop& t) {
    const Vec3 back=scale(t.forward,-1);
    Pose p;
    p.orientation=quatFromRows(t.right,t.up,back);
    p.position={-dot(t.right,t.center),-dot(t.up,t.center),-dot(back,t.center)};
    return p;
}
inline Tabletop makeTabletop(Vec3 up,Vec3 ground,const Projection& lens,
                             float left,float top,float right,float bottom,float crop,bool circumscribe=false) {
    Tabletop table;
    // The floor must face the camera (+Z towards the viewer). 0.2.0 wrongly required up.y>0,
    // which rejected the top-down camera used by DKII when zoomed out.
    if(!normalize(up)||up.z<.05f||lens.focalX<1||lens.focalY<1||
       !std::isfinite(crop)||crop<=0||crop>1||right<=left||bottom<=top) return table;
    table.up=up;
    table.right=sub({1,0,0},scale(up,up.x));
    if(!normalize(table.right)) return {};
    table.forward=cross(up,table.right);
    if(!normalize(table.forward)) return {};
    float groundDistance=dot(up,ground);
    Vec3 corners[4]{};
    float screenX[4]={left,right,left,right},screenY[4]={top,top,bottom,bottom};
    for(int i=0;i<4;++i) {
        Vec3 ray{(screenX[i]-lens.centerX)/lens.focalX,-(screenY[i]-lens.centerY)/lens.focalY,-1};
        float divisor=dot(up,ray);
        if(std::fabs(divisor)<1e-5f) return {};
        float depth=groundDistance/divisor;
        if(!std::isfinite(depth)||depth<=.02f||depth>100) return {};
        corners[i]=scale(ray,depth);
    }
    // Inscribe an axis-aligned rectangle in the perspective footprint. Taking
    // the intersection of the near/far widths removes the trapezoid corners.
    float xMin=std::max(dot(table.right,corners[0]),dot(table.right,corners[2]));
    float xMax=std::min(dot(table.right,corners[1]),dot(table.right,corners[3]));
    float zMin=std::max(dot(table.forward,corners[2]),dot(table.forward,corners[3]));
    float zMax=std::min(dot(table.forward,corners[0]),dot(table.forward,corners[1]));
    // 0.4.7 (TableView=1): rectangle AROUND the footprint, the whole screen view fits in it.
    if(circumscribe) {
        xMin=std::min(dot(table.right,corners[0]),dot(table.right,corners[2]));
        xMax=std::max(dot(table.right,corners[1]),dot(table.right,corners[3]));
        zMin=std::min(dot(table.forward,corners[2]),dot(table.forward,corners[3]));
        zMax=std::max(dot(table.forward,corners[0]),dot(table.forward,corners[1]));
    }
    table.halfWidth=(xMax-xMin)*.5f*crop;
    table.halfDepth=(zMax-zMin)*.5f*crop;
    table.center=add(scale(up,groundDistance),add(scale(table.right,(xMin+xMax)*.5f),scale(table.forward,(zMin+zMax)*.5f)));
    table.valid=std::isfinite(table.halfWidth)&&std::isfinite(table.halfDepth)&&table.halfWidth>.01f&&table.halfDepth>.01f;
    return table;
}
// 0.2.6: rectangle valid at every height between two levels (distances along 'up', negative below
// the camera): intersection of the inscribed footprints of both planes; centre placed on the LOW plane.
inline Tabletop makeLeveledTabletop(Vec3 up,float low,float high,const Projection& lens,
                                   float left,float top,float right,float bottom,float crop) {
    auto a=makeTabletop(up,scale(up,low),lens,left,top,right,bottom,1.f);
    auto b=makeTabletop(up,scale(up,high),lens,left,top,right,bottom,1.f);
    if(!a.valid||!b.valid||!std::isfinite(crop)||crop<=0||crop>1) return {};
    auto bounds=[](const Tabletop& t,float& x0,float& x1,float& z0,float& z1) {
        float cx=dot(t.right,t.center),cz=dot(t.forward,t.center);
        x0=cx-t.halfWidth;x1=cx+t.halfWidth;z0=cz-t.halfDepth;z1=cz+t.halfDepth;
    };
    float ax0,ax1,az0,az1,bx0,bx1,bz0,bz1;bounds(a,ax0,ax1,az0,az1);bounds(b,bx0,bx1,bz0,bz1);
    float x0=std::max(ax0,bx0),x1=std::min(ax1,bx1),z0=std::max(az0,bz0),z1=std::min(az1,bz1);
    Tabletop t=a;
    t.halfWidth=(x1-x0)*.5f*crop;t.halfDepth=(z1-z0)*.5f*crop;
    t.center=add(scale(t.up,low),add(scale(t.right,(x0+x1)*.5f),scale(t.forward,(z0+z1)*.5f)));
    t.valid=std::isfinite(t.halfWidth)&&std::isfinite(t.halfDepth)&&t.halfWidth>.01f&&t.halfDepth>.01f;
    return t;
}
// 0.3.2: the visible dungeon is wider than deep for the player: depth limited to aspect x width,
// centred (the native view footprint is deeper than wide at the game's camera pitch).
// 0.4.7: grows the short side so that the rectangle has the table's shape (depth = aspect x width).
inline Tabletop fitAspect(Tabletop t,float aspect) {
    if(!t.valid||!std::isfinite(aspect)||aspect<=0) return t;
    if(t.halfDepth<t.halfWidth*aspect) t.halfDepth=t.halfWidth*aspect; else t.halfWidth=t.halfDepth/aspect;
    return t;
}
// True when rectangle 'outer' (same axes) contains 'inner' along right (x) and forward (z).
inline void covers(const Tabletop& outer,const Tabletop& inner,bool& x,bool& z) {
    float ox=dot(outer.right,outer.center),oz=dot(outer.forward,outer.center),ix=dot(inner.right,inner.center),iz=dot(inner.forward,inner.center);
    x=outer.valid&&ox-outer.halfWidth<=ix-inner.halfWidth+1e-4f&&ox+outer.halfWidth>=ix+inner.halfWidth-1e-4f;
    z=outer.valid&&oz-outer.halfDepth<=iz-inner.halfDepth+1e-4f&&oz+outer.halfDepth>=iz+inner.halfDepth-1e-4f;
}
inline Tabletop limitDepth(Tabletop t,float aspect) {
    if(t.valid&&std::isfinite(aspect)&&aspect>0&&t.halfDepth>t.halfWidth*aspect) t.halfDepth=t.halfWidth*aspect;
    return t;
}
// Screen area where the engine really draws the 3D world (pixels, same convention as the lens centre).
struct ScreenRect {float left,top,right,bottom;};
inline ScreenRect fullScreen(const Projection& lens) {return {0,0,2*(lens.centerX+.5f),2*(lens.centerY+.5f)};}
inline Tabletop makeNativeTabletop(Vec3 nativeZ,Vec3 nativeTranslation,const Projection& lens,
                                  float screenFocalX,float screenFocalY,float units,float crop,ScreenRect area,bool circumscribe=false) {
    if(!std::isfinite(units)||units<=0||lens.focalX<1||lens.focalY<1) return {};
    float sx=screenFocalX/lens.focalX,sy=screenFocalY/lens.focalY;
    Vec3 up{-nativeZ.x*sx,nativeZ.y*sy,nativeZ.z};
    Vec3 ground{nativeTranslation.x*sx/units,-nativeTranslation.y*sy/units,-nativeTranslation.z/units};
    return makeTabletop(up,ground,lens,area.left,area.top,area.right,area.bottom,crop,circumscribe);
}
inline Tabletop makeNativeTabletop(Vec3 nativeZ,Vec3 nativeTranslation,const Projection& lens,
                                  float screenFocalX,float screenFocalY,float units,float crop) {
    if(!std::isfinite(units)||units<=0||lens.focalX<1||lens.focalY<1) return {};
    float sx=screenFocalX/lens.focalX,sy=screenFocalY/lens.focalY;
    // DKII uses negative world Z above the floor (live camera Z=-5.879...).
    // First reconstruct the camera axes, then take NEGATIVE native Z as up.
    Vec3 up{-nativeZ.x*sx,nativeZ.y*sy,nativeZ.z};
    Vec3 ground{nativeTranslation.x*sx/units,-nativeTranslation.y*sy/units,-nativeTranslation.z/units};
    return makeTabletop(up,ground,lens,0,0,2*(lens.centerX+.5f),2*(lens.centerY+.5f),crop);
}
}
