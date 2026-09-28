#pragma once
// DK2VR 0.6.1 - placement of the VR HUD modules, shared by the game thread (laser hit -> game cursor)
// and the VR thread (drawing). All poses in LOCAL space (metres). A module shows a rectangle of the
// transparent interface texture (the game's 2D interface redrawn without the dungeon behind it).
#include "Math.hpp"
#include "TableModel.hpp"

namespace dk2vr {
inline float dot3(Vec3 a,Vec3 b) {return a.x*b.x+a.y*b.y+a.z*b.z;}
// Fractions of the 4:3 game area inside the back buffer (measured on 2560x1440 captures, 0.6.0).
// The rectangles do not overlap; they are generous so that tooltips drawn near a bar stay visible.
struct HudRegion {float u0,v0,u1,v1;};
constexpr int HudCount=5;
enum HudIndex {HudTop=0,HudStats=1,HudMinimap=2,HudBottom=3,HudScreen=4};
// 0.6.7: real bar outlines; the rest of the game area (menus, options screen, tooltips) is the wall screen.
constexpr HudRegion HudRegions[HudCount]={
    {0.f,0.f,1.f,.090f},      // health, gold, message line
    {0.f,.693f,.73f,.760f},   // timer / statistics bar
    {0.f,.760f,.178f,1.f},    // minimap and its four corner buttons
    {.182f,.760f,1.f,1.f},    // creatures / rooms / spells panel
    {0.f,.090f,1.f,.715f}};   // everything in between (down to 'Continue game' of the options screen): options screen, menus, tooltips (front wall)
// 0.6.9.3: while the options screen is open, its panel shows the whole game area from below the top bar down to
// the bottom of the statistics bar, taken from the full interface texture (options + statistics bar composed in
// the game's own order): 'Continue game' overlaps the statistics bar, a separate statistics panel kept a piece of it.
constexpr HudRegion HudScreenWhole={0.f,.090f,1.f,.760f};
// pointerFlags bits 8.. carry the HUD settings chosen on the host (F2 etc.).
constexpr uint32_t PointerAimValid=1u<<3,PointerTriggerRaw=1u<<4,PointerSecondRaw=1u<<5;
constexpr uint32_t HudLeftHanded=1u<<8,HudBottomNear=1u<<9;
constexpr uint32_t HudMinimapShift=10; // 2 bits: 0 pedestal, 1 watch, 2 watch when looked at
inline int hudMinimapMode(uint32_t flags) {return int((flags>>HudMinimapShift)&3u)%3;}
struct HudSizes {float wall=1.8f,bottom=.80f,minimap=.30f,cursor=.10f;};
struct HudPanel {bool shown=false;Pose pose{};float w=0,h=0;float x0=0,y0=0,x1=0,y1=0;}; // pixel rectangle
struct HudLayout {
    HudPanel panel[HudCount];
    bool pedestal=false;Pose pedestalTop{};Vec3 pedestalFoot{};float plateW=0,plateH=0;
    // 0.6.8: button on the pedestal column (moves the pedestal to the other side of the table).
    bool button=false;Pose buttonPose{};float buttonW=0,buttonH=0;bool arrowRight=false;
};
inline void hudPixels(int i,float W,float H,float& x0,float& y0,float& x1,float& y1,bool screenWhole=false) {
    const float iw=std::min(W,H*4.f/3.f),ih=std::min(H,W*3.f/4.f),ox=(W-iw)*.5f,oy=(H-ih)*.5f;
    const HudRegion& r=(screenWhole&&i==HudScreen)?HudScreenWhole:HudRegions[i];
    x0=ox+r.u0*iw;y0=oy+r.v0*ih;x1=ox+r.u1*iw;y1=oy+r.v1*ih;
}
inline Quat hudFacing(Vec3 from,Vec3 to,float tiltDegrees) {
    Vec3 d=sub(to,from);float yaw=std::atan2(d.x,d.z); // +Z of the panel towards 'to'
    return normalized(multiply(yawQuat(yaw),pitch(tiltDegrees*Pi/180.f)));
}
// dungeonTop: height of the dungeon walls above the table (metres); depthRatio: table depth/width.
// pedestalSide: 0 = beside the pointing hand's opposite side (handedness), 1 = left, 2 = right of the table.
inline HudLayout layoutHud(const Request& r,const HudSizes& s,float depthRatio,float dungeonTop,float W,float H,bool screen=false,int pedestalSide=0) {
    HudLayout L;
    if(W<64||H<64) return L;
    auto set=[&](int i,const Pose& p,float width){
        HudPanel& q=L.panel[i];hudPixels(i,W,H,q.x0,q.y0,q.x1,q.y1,screen);
        q.pose=p;q.w=width;q.h=width*(q.y1-q.y0)/std::max(1.f,q.x1-q.x0);q.shown=true;};
    auto height=[&](int i,float width){float x0,y0,x1,y1;hudPixels(i,W,H,x0,y0,x1,y1,screen);return width*(y1-y0)/std::max(1.f,x1-x0);};
    const bool floorKnown=r.handFlags&RequestFloorKnown;
    const float floorY=floorKnown?r.floorY:r.table.position.y-.78f;
    const Vec3 head=scale(add(r.eyes[0].pose.position,r.eyes[1].pose.position),.5f);
    // Player side of the table: the side facing the head, whatever the table rotation.
    const TableModel m=tableModel(r.tableWidthMeters,depthRatio);
    const Vec3 hl=toLocal(r.table,head);
    float yaw=0,across=m.outerX(),along=m.outerZ();
    if(std::fabs(hl.x)/m.outerX()>std::fabs(hl.z)/m.outerZ()) {yaw=hl.x>0?Pi*.5f:-Pi*.5f;across=m.outerZ();along=m.outerX();}
    else if(hl.z<0) yaw=Pi;
    const Pose side=compose(r.table,Pose{yawQuat(yaw),{0,0,0}}); // +Z towards the player, -Z far edge
    const float lift=std::clamp(dungeonTop,0.f,.8f)+.03f;
    // Creatures/rooms/spells: behind the dungeon on the far edge, facing the player, above the walls.
    // 0.6.7: statistics bar and top bar stacked above the creatures/rooms/spells panel, same width, +15 %.
    // 0.6.9.1: the options/menu screen no longer projects on the far wall; it is stacked with the other panels.
    // 0.6.9.2: order (top to bottom) health bar / options screen / timer (statistics) bar / creatures panel.
    // The options screen sits whole ON the timer bar (no gap) and the health bar moves up above it.
    auto stack=[&](const Pose& bottom,float w){
        const float hb=height(HudBottom,w),hs=height(HudStats,w),ht=height(HudTop,w),gap=.012f;
        set(HudBottom,bottom,w);
        float y=hb*.5f+gap;
        // 0.6.9.3: options open -> one whole panel (options + statistics bar), no separate statistics panel.
        if(screen) {const float hsc=height(HudScreen,w);
            set(HudScreen,compose(bottom,Pose{{0,0,0,1},{0,y+hsc*.5f,0}}),w);y+=hsc;}
        else {set(HudStats,compose(bottom,Pose{{0,0,0,1},{0,y+hs*.5f,0}}),w);y+=hs;}
        set(HudTop,compose(bottom,Pose{{0,0,0,1},{0,y+gap+ht*.5f,0}}),w);};
    if(r.pointerFlags&HudBottomNear) {
        const float w=std::min(s.bottom,.45f)*1.15f,h=height(HudBottom,w),tilt=-60.f;
        stack(compose(side,Pose{pitch(tilt*Pi/180.f),{0,.02f+h*.5f*std::cos(tilt*Pi/180.f),along+.10f}}),w);
    } else {
        const float w=s.bottom*1.15f,h=height(HudBottom,w),tilt=-10.f;
        stack(compose(side,Pose{pitch(tilt*Pi/180.f),{0,lift+h*.5f*std::cos(tilt*Pi/180.f),-(along+.06f)}}),w);
    }
    // Minimap: pedestal beside the far corner (left, or right for left-handed players), facing the player.
    const bool left=r.pointerFlags&HudLeftHanded;
    const int mode=hudMinimapMode(r.pointerFlags);
    const int watchHand=left?1:0;
    const bool watchValid=r.handFlags&(watchHand?HandRightValid:HandLeftValid);
    if(mode==0||!watchValid) {
        const float sx=pedestalSide==2?1.f:pedestalSide==1?-1.f:(left?1.f:-1.f);
        const float w=s.minimap,h=height(HudMinimap,w),tilt=-35.f;
        Vec3 base=toWorld(side,{sx*(across+.30f),0,-along*.45f});
        const float topY=r.table.position.y+.10f;
        Vec3 center{base.x,topY+h*.5f*std::cos(tilt*Pi/180.f),base.z};
        Vec3 eye{head.x,center.y,head.z};
        Pose p{hudFacing(center,eye,tilt),center};
        set(HudMinimap,p,w);
        L.pedestal=true;L.pedestalTop=p;L.pedestalFoot={base.x,floorY,base.z};L.plateW=w+.04f;L.plateH=h+.04f;
        {const Vec3 under=toWorld(p,{0,-L.plateH*.25f,-.03f});
         if(under.y-floorY>.25f) {
             const Quat yaw=yawOnly(p.orientation);const float mid=under.y-.085f; // 0.6.9: top of the column, smaller
             L.button=true;L.buttonW=.06f;L.buttonH=.06f;L.arrowRight=sx<0;
             L.buttonPose=Pose{yaw,add(Vec3{under.x,mid,under.z},rotate(yaw,Vec3{0,0,.064f}))};}}
    } else {
        Pose p=compose(r.hands[watchHand],Pose{pitch(-60.f*Pi/180.f),{0,.045f,.075f}});
        bool visible=true;
        if(mode==2) {
            Vec3 n=rotate(p.orientation,{0,0,1}),to=sub(head,p.position);float l=std::sqrt(dot3(to,to));
            visible=l>1e-4f&&dot3(n,to)/l>.35f;
        }
        if(visible) set(HudMinimap,p,std::min(s.minimap,.16f));
    }
    return L;
}
// 0.6.8: ray against the pedestal button (front face, +Z towards the player).
inline bool hitButton(const HudLayout& L,Vec3 origin,Vec3 dir,float& t,float maxDistance=4.f) {
    if(!L.button) return false;
    Vec3 o=toLocal(L.buttonPose,origin),d=rotate(conjugate(L.buttonPose.orientation),dir);
    if(d.z>-1e-4f||o.z<=0) return false;
    const float k=-o.z/d.z;if(k<=0||k>=maxDistance) return false;
    if(std::fabs(o.x+d.x*k)>L.buttonW*.5f||std::fabs(o.y+d.y*k)>L.buttonH*.5f) return false;
    t=k;return true;
}
// Ray against the shown modules. Returns the module index (or -1), distance and back-buffer fraction.
inline int hitHud(const HudLayout& L,Vec3 origin,Vec3 dir,float W,float H,float& t,float& fx,float& fy,float maxDistance=4.f) {
    int best=-1;t=maxDistance;
    for(int i=0;i<HudCount;++i) {
        const HudPanel& q=L.panel[i];if(!q.shown) continue;
        Vec3 o=toLocal(q.pose,origin),d=rotate(conjugate(q.pose.orientation),dir);
        if(d.z>-1e-4f||o.z<=0) continue; // from the front only
        float k=-o.z/d.z;if(k<=0||k>=t) continue;
        float x=o.x+d.x*k,y=o.y+d.y*k;
        if(std::fabs(x)>q.w*.5f||std::fabs(y)>q.h*.5f) continue;
        float u=x/q.w+.5f,v=.5f-y/q.h;
        fx=(q.x0+u*(q.x1-q.x0))/W;fy=(q.y0+v*(q.y1-q.y0))/H;t=k;best=i;
    }
    return best;
}
}
