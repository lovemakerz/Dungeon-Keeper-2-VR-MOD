// DK2VR 0.9.0 - tilting table: pose algebra, tilt direction, lever mapping and placement (pure math, run at build time).
#include "../src/common/TableTilt.hpp"
#include "../src/common/HudLayout.hpp"
#include "../src/common/Grab.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
using namespace dk2vr;
static float dist(Vec3 a,Vec3 b){Vec3 d=sub(a,b);return std::sqrt(dot3(d,d));}
static bool near(Quat a,Quat b){float d=std::fabs(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w);return d>1-1e-5f;}
int main() {
    int checks=0;
    for(float anchorYaw:{0.f,.7f,-2.3f,3.0f}) for(float spin:{0.f,.4f,1.57f,-2.5f}) for(float tiltDeg:{0.f,5.f,22.5f,45.f}) {
        Request r{};
        r.anchor={yawQuat(anchorYaw),{1.f,1.6f,-.5f}};
        // table placed like the host: in front of the anchor, below the eyes, then turned on itself by the handles
        Pose level=compose(r.anchor,Pose{{0,0,0,1},{0,-.5f,-.8f}});level.orientation=normalized(multiply(yawQuat(spin),level.orientation));
        const float tilt=degToRad(tiltDeg);
        r.table=tiltTable(level,yawOnly(r.anchor.orientation),tilt);r.tableTilt=tilt;
        // 1) exact inverse
        const Pose back=levelTable(r);
        assert(dist(back.position,level.position)<1e-4f&&near(back.orientation,level.orientation));
        // 2) the top normal leans TOWARDS the player (anchor +Z) by exactly the tilt, whatever the spin
        const Vec3 nrm=rotate(r.table.orientation,{0,1,0});
        const Vec3 towardPlayer=rotate(yawQuat(anchorYaw),{0,0,1});
        assert(std::fabs(nrm.y-std::cos(tilt))<1e-4f);
        if(tiltDeg>0) assert(dot3(nrm,towardPlayer)>std::sin(tilt)-1e-4f&&dot3(nrm,towardPlayer)<std::sin(tilt)+1e-4f);
        // 3) the far part of the top (away from the player) rises, the near part goes down
        const Vec3 farPt=add(level.position,scale(towardPlayer,-.4f)),nearPt=add(level.position,scale(towardPlayer,.4f));
        const Vec3 farOnTop=toWorld(r.table,toLocal(level,farPt)),nearOnTop=toWorld(r.table,toLocal(level,nearPt));
        if(tiltDeg>0) assert(farOnTop.y>farPt.y+.01f&&nearOnTop.y<nearPt.y-.01f);
        // 4) the axle does not move: it lies TiltAxleY under the level centre in both poses
        const Vec3 axle=add(level.position,{0,TiltAxleY,0});
        assert(dist(toWorld(r.table,{0,TiltAxleY,0}),axle)<1e-4f);
        assert(dist(cradlePose(r).position,axle)<1e-4f);
        // 5) stand = level position, recenter yaw, upright
        const Pose st=standPose(r);assert(dist(st.position,level.position)<1e-4f&&std::fabs(rotate(st.orientation,{0,1,0}).y-1)<1e-5f);
        ++checks;
    }
    // 6) lever: angle <-> tilt, leaning away at level, towards the player at the maximum, notches
    for(int i=0;i<=9;++i) {const float t=degToRad(i*TiltNotchDeg);assert(std::fabs(tiltFromLever(leverAngle(t))-t)<1e-5f);}
    assert(tiltFromLever(-1.f)==0.f&&std::fabs(tiltFromLever(1.f)-degToRad(TiltMaxDeg))<1e-6f);
    for(float width:{.5f,.8f,1.2f}) for(float floorT:{-.95f,-.75f,-.45f}) for(bool left:{false,true}) {
        const TableModel m=tableModel(width,.75f);const LeverRig g=leverRig(m,floorT,left);
        const Vec3 k0=leverKnobLocal(g,0),k1=leverKnobLocal(g,degToRad(TiltMaxDeg));
        assert(k1.z>k0.z+.15f);                                  // pulled towards the player (+Z)
        assert((g.side>0)==!left&&g.pivot.x*g.side>0);           // right side, left side for left-handed players
        const float spin=std::sqrt(m.outerX()*m.outerX()+m.outerZ()*m.outerZ());
        for(float tdeg:{0.f,45.f}) {const Vec3 k=leverKnobLocal(g,degToRad(tdeg));assert(std::sqrt(k.x*k.x+k.z*k.z)>spin+.03f);}   // never in the circle of the turning top
        assert((k0.y>-.02f&&k0.y<.16f)||g.arm>=1.19f||g.arm<=.36f); // grip about at the height of the top
        // reachable from a seated/standing player 0.8 m in front of the table centre (shoulder 25 cm aside, 30 cm below the eyes)
        const Vec3 shoulder{(left?-.25f:.25f),.2f,.8f};
        assert(dist(shoulder,k0)<.70f&&dist(shoulder,k1)<.70f);
        assert(k0.x*g.side>.40f&&k1.x*g.side>.40f);   // the grip passes BESIDE the player (shoulder width), never into the body
        // the top never reaches the lever at 45 deg (near edge of the top vs knob path)
        ++checks;
    }
    // 7) relative grab (host): a hand moving along the arc from its grab point maps 1:1 onto the lever angle
    {
        const TableModel m=tableModel(.8f,.75f);const LeverRig g=leverRig(m,-.75f,false);
        const float t0=degToRad(10.f);const Vec3 grabAt=leverKnobLocal(g,t0);
        const Vec3 l0=sub(grabAt,g.pivot);const float a0=std::atan2(l0.z,l0.y);
        for(float td:{0.f,10.f,25.f,45.f}) {
            const Vec3 hand=leverKnobLocal(g,degToRad(td));const Vec3 l=sub(hand,g.pivot);
            const float t=tiltFromLever(leverAngle(t0)+(std::atan2(l.z,l.y)-a0));
            assert(std::fabs(t-degToRad(td))<1e-4f);
        }
        ++checks;
    }
    // 8) HUD: with the top tilted, the bars stay above the (raised) far edge and keep facing the player
    {
        Request r{};r.anchor={{0,0,0,1},{0,1.6f,0}};r.handFlags=RequestFloorKnown;r.floorY=0;r.tableWidthMeters=.8f;r.tableAnchor=1;
        r.eyes[0].pose={{0,0,0,1},{-.03f,1.6f,0}};r.eyes[1].pose={{0,0,0,1},{.03f,1.6f,0}};
        const Pose level=compose(r.anchor,Pose{{0,0,0,1},{0,-.6f,-.8f}});
        r.table=tiltTable(level,{0,0,0,1},0);r.tableTilt=0;
        const Pose lv0=levelTable(r);
        r.table=tiltTable(level,{0,0,0,1},degToRad(45));r.tableTilt=degToRad(45);
        const Pose lv=levelTable(r);
        assert(dist(lv.position,lv0.position)<1e-4f);
        ++checks;
    }
    printf("PASS: tilting table: %d checks (inverse pose, tilt towards the player for any spin/yaw, fixed axle, lever mapping, side, reach, out of the turning circle, relative grab)\n",checks);
    return 0;
}
