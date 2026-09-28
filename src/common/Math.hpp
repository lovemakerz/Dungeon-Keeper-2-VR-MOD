#pragma once
#include "Protocol.hpp"
#include <cmath>
#include <algorithm>

namespace dk2vr {
constexpr float Pi = 3.14159265358979323846f;
inline Vec3 add(Vec3 a, Vec3 b) { return {a.x+b.x,a.y+b.y,a.z+b.z}; }
inline Vec3 sub(Vec3 a, Vec3 b) { return {a.x-b.x,a.y-b.y,a.z-b.z}; }
inline Vec3 scale(Vec3 a, float s) { return {a.x*s,a.y*s,a.z*s}; }
inline Quat conjugate(Quat q) { return {-q.x,-q.y,-q.z,q.w}; }
inline Vec3 cross(Vec3 a, Vec3 b) { return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; }
inline Vec3 rotate(Quat q, Vec3 p) {
    const Vec3 v{q.x,q.y,q.z};
    const auto t = scale(cross(v,p),2);
    return add(p,add(scale(t,q.w),cross(v,t)));
}
inline Quat multiply(Quat a, Quat b) {
    return {a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,
            a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
            a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,
            a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z};
}
inline Quat pitch(float radians) { return {std::sin(radians/2),0,0,std::cos(radians/2)}; }
inline Quat yawQuat(float radians) { return {0,std::sin(radians/2),0,std::cos(radians/2)}; }
inline float yawAngle(Quat q) {
    auto f=rotate(q,{0,0,-1});
    return std::atan2(-f.x,-f.z);
}
inline Quat yawOnly(Quat q) { return yawQuat(yawAngle(q)); }
inline Quat normalized(Quat q) {
    float l=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);
    if(!std::isfinite(l)||l<1e-8f) return {0,0,0,1};
    return {q.x/l,q.y/l,q.z/l,q.w/l};
}
inline Vec3 toWorld(const Pose& p, Vec3 v) { return add(rotate(p.orientation,v),p.position); }
inline Vec3 toLocal(const Pose& p, Vec3 v) { return rotate(conjugate(p.orientation),sub(v,p.position)); }
// compose(a,b) maps a point of frame b into the parent of frame a.
inline Pose compose(const Pose& a, const Pose& b) { return {normalized(multiply(a.orientation,b.orientation)),toWorld(a,b.position)}; }
// Rotation whose matrix rows are r0,r1,r2 (result = M * v). Rows must be orthonormal, det=+1.
inline Quat quatFromRows(Vec3 r0, Vec3 r1, Vec3 r2) {
    const float m00=r0.x,m01=r0.y,m02=r0.z,m10=r1.x,m11=r1.y,m12=r1.z,m20=r2.x,m21=r2.y,m22=r2.z;
    const float trace=m00+m11+m22; Quat q{};
    if(trace>0) {float s=std::sqrt(trace+1)*2;q.w=.25f*s;q.x=(m21-m12)/s;q.y=(m02-m20)/s;q.z=(m10-m01)/s;}
    else if(m00>m11&&m00>m22) {float s=std::sqrt(1+m00-m11-m22)*2;q.w=(m21-m12)/s;q.x=.25f*s;q.y=(m01+m10)/s;q.z=(m02+m20)/s;}
    else if(m11>m22) {float s=std::sqrt(1+m11-m00-m22)*2;q.w=(m02-m20)/s;q.x=(m01+m10)/s;q.y=.25f*s;q.z=(m12+m21)/s;}
    else {float s=std::sqrt(1+m22-m00-m11)*2;q.w=(m10-m01)/s;q.x=(m02+m20)/s;q.y=(m12+m21)/s;q.z=.25f*s;}
    return normalized(q);
}
struct Projection { float focalX, focalY, centerX, centerY; };
inline bool unproject(float sx,float sy,float rhw,const Projection& lens,float unitsPerMeter,Vec3& result) {
    if (!std::isfinite(rhw) || rhw<=1e-8f || !std::isfinite(unitsPerMeter) || unitsPerMeter<=0 ||
        !std::isfinite(lens.focalX) || !std::isfinite(lens.focalY) || lens.focalX<1 || lens.focalY<1) return false;
    float depth=1/(rhw*unitsPerMeter);
    result={(sx-lens.centerX)*depth/lens.focalX, -(sy-lens.centerY)*depth/lens.focalY,-depth};
    return std::isfinite(result.x)&&std::isfinite(result.y)&&std::isfinite(result.z)&&depth<10000;
}
// Column-vector mathematics stored in D3D's row-vector matrix convention.
// Right handed camera, -Z forward, D3D depth [0,1], asymmetric OpenXR FOV.
inline void projectionMatrix(const Fov& f,float n,float z,float* m) {
    std::fill(m,m+16,0.f);
    const float l=std::tan(f.left),r=std::tan(f.right),u=std::tan(f.up),d=std::tan(f.down);
    m[0]=2/(r-l); m[5]=2/(u-d); m[8]=(r+l)/(r-l); m[9]=(u+d)/(u-d);
    m[10]=z/(n-z); m[11]=-1; m[14]=n*z/(n-z);
}
}
