#pragma once
// DK2VR 0.3.0 - decoupled VR rendering: pure mathematics shared by the game-side VR thread and the tests.
// A game frame is recorded once, in the VR space of the moment it was recorded ("record-LOCAL").
// The VR thread redraws it at the headset rate: the table may have moved since (grab, turn), and the
// player's hand may have dragged the dungeon further (pan prediction). All of this is one similarity
// transform applied through the view matrix; the head pose comes from the newest request.
#include "Tabletop.hpp"

namespace dk2vr {
// x' = scale * rotate(rotation, x) + translation
struct Similarity { Quat rotation{0,0,0,1}; float factor=1; Vec3 translation{}; };
inline Vec3 apply(const Similarity& m,Vec3 p) {return add(scale(rotate(m.rotation,p),m.factor),m.translation);}
inline Similarity similarity(const Pose& p) {return {p.orientation,1,p.position};}
inline Similarity inverse(const Similarity& m) {
    Quat c=conjugate(m.rotation);float s=1/m.factor;
    return {c,s,scale(rotate(c,m.translation),-s)};
}
// then(a,b): first a, then b  (x -> b(a(x))).
inline Similarity then(const Similarity& a,const Similarity& b) {
    return {normalized(multiply(b.rotation,a.rotation)),a.factor*b.factor,apply(b,a.translation)};
}
// D3D row-vector matrix (x_row * M): row i = factor * rotate(rotation, e_i), row 3 = translation.
inline void matrixOf(const Similarity& m,float* out) {
    const Vec3 e[3]={{1,0,0},{0,1,0},{0,0,1}};
    for(int i=0;i<3;++i) {Vec3 r=scale(rotate(m.rotation,e[i]),m.factor);out[i*4]=r.x;out[i*4+1]=r.y;out[i*4+2]=r.z;out[i*4+3]=0;}
    out[12]=m.translation.x;out[13]=m.translation.y;out[14]=m.translation.z;out[15]=1;
}
inline Vec3 transformRow(const float* m,Vec3 p) {
    return {p.x*m[0]+p.y*m[4]+p.z*m[8]+m[12],p.x*m[1]+p.y*m[5]+p.z*m[9]+m[13],p.x*m[2]+p.y*m[6]+p.z*m[10]+m[14]};
}
// Plane (n,d) of the source space expressed in the destination space of m (sign of distances kept).
inline Plane transformPlane(const Plane& p,const Similarity& m) {
    Vec3 n=rotate(m.rotation,p.normal);
    return {n,p.offset*m.factor-dot(n,m.translation)};
}
// Record-LOCAL -> current LOCAL.
// recorded: table pose used when the game frame was recorded; now: current table pose;
// widthRatio: current table width / recorded width; pan: dungeon displacement in the table frame
// (metres of the recorded table, horizontal).
inline Similarity tableCorrection(const Pose& recorded,const Pose& now,float widthRatio,Vec3 pan) {
    Similarity m=inverse(similarity(recorded));
    m=then(m,Similarity{{0,0,0,1},1,pan});
    m=then(m,Similarity{{0,0,0,1},std::isfinite(widthRatio)&&widthRatio>0?widthRatio:1.f,{}});
    return then(m,similarity(now));
}
// 0.3.1: the physical table (base, rim, clipping rectangle) follows only the table pose; the dungeon also
// receives the motion the game has not drawn yet: turn by 'yaw' around 'pivot' then slide by 'pan'
// (table frame, metres of the recorded table).
inline Similarity dungeonCorrection(const Pose& recorded,const Pose& now,float widthRatio,Vec3 pan,float yaw,Vec3 pivot) {
    Similarity m=inverse(similarity(recorded));
    m=then(m,Similarity{{0,0,0,1},1,scale(pivot,-1)});
    m=then(m,Similarity{yawQuat(yaw),1,{0,0,0}});
    m=then(m,Similarity{{0,0,0,1},1,add(pivot,pan)});
    m=then(m,Similarity{{0,0,0,1},std::isfinite(widthRatio)&&widthRatio>0?widthRatio:1.f,{}});
    return then(m,similarity(now));
}
inline float wrapRadians(float a) {while(a>Pi) a-=2*Pi;while(a<-Pi) a+=2*Pi;return a;}
inline float handsAngle(Vec3 left,Vec3 right) {return std::atan2(right.x-left.x,right.z-left.z);}
// Hand displacement (LOCAL) -> horizontal displacement in the table frame, before the width scale.
inline Vec3 panInTable(Vec3 handDelta,const Pose& tableNow,float widthRatio,float limit) {
    Vec3 d=rotate(conjugate(tableNow.orientation),handDelta);d.y=0;
    if(std::isfinite(widthRatio)&&widthRatio>0) d=scale(d,1/widthRatio);
    float l=std::sqrt(d.x*d.x+d.z*d.z);
    if(!std::isfinite(l)) return {0,0,0};
    if(l>limit) d=scale(d,limit/l);
    return d;
}
}
