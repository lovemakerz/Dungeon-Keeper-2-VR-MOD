#pragma once
// DK2VR 0.4.1 - wooden table model shared by the host (handle grab points) and the game DLL (drawing).
// Table frame: origin = centre of the dungeon floor, +X right, +Y up, +Z towards the player, metres.
#include "Math.hpp"

namespace dk2vr {
struct TableModel {
    float halfWidth=.4f,halfDepth=.3f;     // dungeon opening
    static constexpr float Border=.085f;   // wooden frame around the opening
    static constexpr float Top=-.004f;     // frame surface (just below the dungeon floor)
    static constexpr float Thickness=.045f;
    static constexpr float LipWidth=.022f,LipHeight=.014f; // raised rounded rim on the outer edge
    // 0.4.1: two drop handles per side, pivots on the frame's outer face just above its lower edge.
    static constexpr float HandlePivot=Top-Thickness+.006f;
    static constexpr float HandleHalfSpan=.03f,HandleDropLength=.04f,HandleOut=.012f;
    static constexpr int HandleCount=8;
    static constexpr float GrabRadius=.10f;
    float outerX() const {return halfWidth+Border;}
    float outerZ() const {return halfDepth+Border;}
    float bottom() const {return Top-Thickness;}
    // Handle i: side i/2 (0 -X, 1 +X, 2 -Z, 3 +Z), two per side at half of the side's half length.
    static Vec3 sideOut(int side) {return side==0?Vec3{-1,0,0}:side==1?Vec3{1,0,0}:side==2?Vec3{0,0,-1}:Vec3{0,0,1};}
    static Vec3 sideLateral(int side) {Vec3 o=sideOut(side);return {-o.z,0,o.x};} // cross(up, out)
    float face(int side) const {return (side<2?outerX():outerZ())-.006f;}
    float handleLateral(int i) const {int side=i/2;float half=side<2?outerZ():outerX();return (i%2?1.f:-1.f)*half*.5f;}
    // Grab point: in front of the hanging bail.
    Vec3 handle(int i) const {
        int side=i/2;Vec3 o=sideOut(side),l=sideLateral(side);
        return add(scale(o,face(side)+HandleOut),add(Vec3{0,HandlePivot-HandleDropLength*.6f,0},scale(l,handleLateral(i))));
    }
};
// aspect <= 0: the game's own shape, unknown to the host -> square assumption.
inline TableModel tableModel(float widthMeters,float aspect) {
    TableModel m;m.halfWidth=std::max(.05f,widthMeters*.5f);
    m.halfDepth=m.halfWidth*(aspect>0?aspect:1.f);return m;
}
}
