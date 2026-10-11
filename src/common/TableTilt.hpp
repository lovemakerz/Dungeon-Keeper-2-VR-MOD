#pragma once
// DK2VR 0.9.0 - tilting table (shared by the host, which moves it, and the game DLL, which draws it).
//
// Mechanics (classic tilt-top table): a turned central column on four curved feet carries an iron yoke and a horizontal
// AXLE under the centre of the top. The axle is parallel to the player's left-right axis (the yaw of the recenter anchor),
// so the top always tilts TOWARDS the player, whatever its own rotation: the top still turns on itself, in its own plane,
// on a round turntable plate (the cradle between the axle and the top tilts but does not turn).
// A dungeon floor lever beside the near corner (right, left for left-handed players: opposite the minimap pedestal)
// sets the tilt: lever upright = level, pulled towards the player = tilted. Notches every TiltNotchDeg.
//
// Frames: LEVEL = table pose without the tilt (what the grab code moves; +Z towards the player at placement),
//         STAND = level position, recenter yaw (column, feet, axle, lever), CRADLE = stand at the axle, tilted (no spin),
//         TABLE = Request::table (tilted, spun): the top, the dungeon, the handles.
#include "Protocol.hpp"
#include "Math.hpp"
#include "TableModel.hpp"
#include <cmath>
#include <algorithm>

namespace dk2vr {
constexpr float TiltMaxDeg=45.f;
constexpr float TiltNotchDeg=5.f;
constexpr float TiltAxleY=-.20f;        // axle height under the dungeon floor (table frame), metres
constexpr float LeverMinDeg=0.f,LeverMaxDeg=30.f;   // lever upright = level ... leaning 30 deg towards the player = TiltMaxDeg
constexpr float LeverGrabRadius=.11f;
// Request::leverFlags
constexpr uint32_t LeverHoverLeft=1u<<0,LeverHoverRight=1u<<1,LeverHeldLeft=1u<<2,LeverHeldRight=1u<<3;

inline float degToRad(float d) {return d*float(Pi)/180.f;}
inline Quat tiltWorld(Quat yaw,float tilt) {return normalized(multiply(multiply(yaw,pitch(tilt)),conjugate(yaw)));}
// Host: tilted table pose from the level pose. Pivot = the axle (TiltAxleY under the level centre).
inline Pose tiltTable(const Pose& level,Quat yaw,float tilt) {
    const Quat q=tiltWorld(yaw,tilt);
    const Vec3 pivot=add(level.position,Vec3{0,TiltAxleY,0});
    return {normalized(multiply(q,level.orientation)),add(pivot,rotate(q,Vec3{0,-TiltAxleY,0}))};
}
// Game DLL: back to the level pose (exact inverse of tiltTable).
inline Pose untiltTable(const Pose& table,Quat yaw,float tilt) {
    const Quat q=tiltWorld(yaw,tilt);
    const Vec3 pivot=add(table.position,rotate(q,Vec3{0,TiltAxleY,0}));
    return {normalized(multiply(conjugate(q),table.orientation)),add(pivot,Vec3{0,-TiltAxleY,0})};
}
inline float requestTilt(const Request& r) {const float t=r.tableTilt;return std::isfinite(t)?std::clamp(t,0.f,degToRad(TiltMaxDeg)):0.f;}
inline Quat standYaw(const Request& r) {return yawOnly(r.anchor.orientation);}
inline Pose levelTable(const Request& r) {return untiltTable(r.table,standYaw(r),requestTilt(r));}
inline Pose standPose(const Request& r) {return {standYaw(r),levelTable(r).position};}
inline Pose cradlePose(const Request& r) {   // origin on the axle
    const Pose s=standPose(r);
    return compose(compose(s,Pose{{0,0,0,1},{0,TiltAxleY,0}}),Pose{pitch(requestTilt(r)),{0,0,0}});
}
// Lever rig in the STAND frame. floorT = floor height relative to the table origin (negative).
struct LeverRig {Vec3 pivot;float arm=.7f;float side=1;};
inline LeverRig leverRig(const TableModel& m,float floorT,bool leftHanded) {
    LeverRig g;g.side=leftHanded?-1.f:1.f;
    const float ox=m.outerX(),oz=m.outerZ(),spin=std::sqrt(ox*ox+oz*oz);   // the top may turn on itself: stay out of its circle
    const float z=oz+.08f;   // just beyond the near edge: within reach of a seated player
    const float x=std::max(ox+.12f,std::sqrt(std::max(0.f,(spin+.10f)*(spin+.10f)-z*z)));
    g.pivot={g.side*x,floorT+.14f,z};
    g.arm=std::clamp(-floorT+.10f-.14f,.35f,1.2f);   // grip a little above the table top
    return g;
}
inline float leverAngle(float tilt) {return degToRad(LeverMinDeg)+degToRad(LeverMaxDeg-LeverMinDeg)*(tilt/degToRad(TiltMaxDeg));}
inline float tiltFromLever(float angle) {return std::clamp((angle-degToRad(LeverMinDeg))/degToRad(LeverMaxDeg-LeverMinDeg)*degToRad(TiltMaxDeg),0.f,degToRad(TiltMaxDeg));}
// Lever pose in the stand frame: origin at the pivot, +Y along the arm.
inline Pose leverLocal(const LeverRig& g,float tilt) {return {pitch(leverAngle(tilt)),g.pivot};}
inline Vec3 leverKnobLocal(const LeverRig& g,float tilt) {return toWorld(leverLocal(g,tilt),Vec3{0,g.arm-.04f,0});}
// Floor height relative to the LEVEL table origin, as the room and the table legs use it.
inline float tableFloorT(const Request& r,const Pose& level) {
    return (r.handFlags&RequestFloorKnown)?r.floorY-level.position.y:-.78f;
}
}
