#pragma once
#include "Math.hpp"
#include "Tabletop.hpp"
#include "TableTilt.hpp"

namespace dk2vr {
// During the entry frame the globals can still describe the steep tabletop
// camera (native Z.z ~= 0). It is not a valid first-person reference.
// 0.8.99.1: a LEVEL first-person camera is valid too (report 20261006_101213, possessed chicken: native Z = (0, .46, 0) = camera looking straight ahead,
// its pitch swinging 0..45 degrees; every frame at pitch 0 was rejected -> black screen, then a few frames per second). The table camera of the entry
// frame has a much smaller y (.17 in the 0.8.36 report) and stays rejected.
inline bool possessionProjectionReady(Vec3 nativeZ,float screenX,float screenY,const Projection& lens) {
    return std::isfinite(nativeZ.x)&&std::isfinite(nativeZ.y)&&std::isfinite(nativeZ.z)&&((nativeZ.y>.1f&&nativeZ.z>.1f)||(nativeZ.y>.3f&&nativeZ.z>-.05f))&&
        std::isfinite(screenX)&&std::isfinite(screenY)&&screenX>1&&screenY>1&&
        std::isfinite(lens.focalX)&&std::isfinite(lens.focalY)&&lens.focalX>1&&lens.focalY>1;
}
// Use the same floor normal as the tabletop, including the native screen-axis
// scaling. atan2(raw Y, raw Z) leaves a residual tilt when the lens is widened.
inline Quat possessionLevelRotation(Vec3 nativeZ,float screenX,float screenY,const Projection& lens) {
    if(!possessionProjectionReady(nativeZ,screenX,screenY,lens)) return pitch(-Pi*.25f);
    Vec3 up{-nativeZ.x*screenX/lens.focalX,nativeZ.y*screenY/lens.focalY,nativeZ.z};
    if(!normalize(up)||up.y<.1f||up.z<-.01f) up={0,.70710678f,.70710678f};
    Vec3 right=sub({1,0,0},scale(up,up.x));
    if(!normalize(right)) return pitch(-Pi*.25f);
    return quatFromRows(right,up,cross(right,up));
}
inline void cycleSnapTurn(bool& enabled,float& degrees) {
    if(!enabled) {enabled=true;degrees=15;}
    else if(degrees==15) degrees=30;
    else if(degrees==30) degrees=45;
    else if(degrees==45) degrees=90;
    else enabled=false;
}
inline float possessionCounterPitch(float nativeY,float nativeZ) {
    return std::isfinite(nativeY)&&std::isfinite(nativeZ)&&nativeZ>.1f?
        std::clamp(std::atan2(nativeY,nativeZ),-.96f,.96f):0.f;
}
inline bool possessionTiltReady(float nativeY,float nativeZ) {
    return std::isfinite(nativeY)&&std::isfinite(nativeZ)&&nativeY>.1f&&nativeZ>.1f;
}
inline float possessionEntryPitch(float nativeY,float nativeZ) {
    // The native camera vector is zero during the first possession frame.
    // The 1.70 executable then settles at Y=.324, Z=.707 (24.6 degrees).
    return possessionTiltReady(nativeY,nativeZ)?possessionCounterPitch(nativeY,nativeZ):
        possessionCounterPitch(.324f,.707f);
}
inline bool possessionWorld(uint32_t requestedMode,uint32_t nativeMode) {
    return requestedMode==2||(requestedMode==0&&(nativeMode==1||nativeMode==2));
}
inline bool showWorldProjection(uint32_t requestedMode,uint32_t nativeMode,uint32_t worldDraws) {
    return requestedMode!=3&&(worldDraws>0||possessionWorld(requestedMode,nativeMode));
}
// 0.8.86: options panel on the FRONT wall of the VR room (straight ahead of the player), 50 % smaller than in 0.8.85, behind an always visible OPTIONS button.
// Items (ids): 1 snap turn (cycles OFF/15/30/45/90), 2 invert the map rotation (ON/OFF), 3 primary hand LEFT, 4 primary hand RIGHT,
// 5 OPTIONS button (opens / closes the panel), 6 close cross (top right corner of the panel), 10..16 laser colour (red, blue, yellow, orange,
// green, violet, pink). Layout units ("unit space") are those of 0.8.85; ComfortPanelScale shrinks them to metres. The same layout is used by the
// OpenXR laser hit test (host) and by the D3D9 room renderer (game DLL).
constexpr uint32_t ComfortItemSnap=1,ComfortItemInvert=2,ComfortItemHandLeft=3,ComfortItemHandRight=4,ComfortItemOpen=5,ComfortItemClose=6,ComfortItemPassthrough=7,ComfortItemOrb=8,ComfortItemMirror=9,ComfortItemColour=10,ComfortColours=7;
// 0.8.94.5: three sliders (red, green, blue, 0..255) of the PASSTHROUGH VD section.
constexpr uint32_t ComfortItemSliderR=20,ComfortItemSliderG=21,ComfortItemSliderB=22;
// 0.8.94.15: how far the table may be zoomed out. The engine draws every tile of the visible area, so this is THE fluidity setting
// (measured on report 20261005_083809: 408 units^2 visible = 702 draws and 29 fps, 101 units^2 = 50 draws and 85 fps).
constexpr uint32_t ComfortItemSliderZoom=23;
// 0.9.35: TABLE SIZE slider, 50..250 % of the table width of DK2VR.ini (TableWidth); F9 / F10 move it too.
constexpr uint32_t ComfortItemSliderTable=24;
constexpr float TableSizeMinPct=50.f,TableSizeMaxPct=250.f;
constexpr float ComfortPanelHalfW=.96f,ComfortPanelHalfH=1.30f;   // 0.9.47.41: +0.16 for the LEFT HAND ORB row
   // unit space (0.8.94.5: wider, two columns + full-width PASSTHROUGH section)
constexpr float ComfortPanelScale=.44f;                          // 0.9.47.41: 44 % (fits the bay between two stone ribs of the left wall)
// Button (metres, wall frame): sits just under the panel (frame included), always visible.
// 0.9.47.41: the button now sits ABOVE the panel (just under the torch of the left wall), centred on the panel.
constexpr float ComfortButtonX0=-.26f,ComfortButtonX1=.26f,ComfortButtonY0=.62f,ComfortButtonY1=.75f;
inline bool comfortPanelOpen(const Request& r) {return (r.comfortFlags&ComfortPanelOpen)!=0;}
constexpr float ComfortWallX=-2.385f,ComfortWallY=.88f,ComfortWallZ=-1.15f;   // room frame (metres)
inline Pose comfortWallPose(const Request& r) {
    const float floorY=(r.handFlags&RequestFloorKnown)?r.floorY:levelTable(r).position.y-.78f;
    const Pose room{r.anchor.orientation,{r.anchor.position.x,floorY,r.anchor.position.z}};
    // 0.9.47.41: LEFT side wall (room x = -2.4), in the bay between the 2nd and 3rd stone ribs (z = -1.95 and -.35), under the torch at z = -1.15,
    // stuck to the wall (2.8 cm in front of it). Panel +Z = room +X (toward the room), panel +X = room -Z. The panel is centred 0.88 m above the floor.
    // 0.9.47.42: same side as the minimap pedestal: ComfortWallRight = the RIGHT wall (x = +2.385, facing -X), otherwise the left wall.
    const bool right=(r.comfortFlags&ComfortWallRight)!=0;
    return {normalized(multiply(room.orientation,yawQuat(right?-Pi*.5f:Pi*.5f))),toWorld(room,{right?-ComfortWallX:ComfortWallX,ComfortWallY,ComfortWallZ})};
}
// 0.9.47.49: CONTROLS panel (controller guide) on the wall OPPOSITE to the options block: same button, same height, same bay. Items 40..43.
constexpr uint32_t ControlsItemOpen=40,ControlsItemClose=41,ControlsItemTabEn=42,ControlsItemTabFr=43;
constexpr float ControlsHalfW=.72f,ControlsHalfH=.52f;                  // metres, wall frame (the panel is centred where the options panel is)
constexpr float ControlsTabX0[2]={-.36f,.02f},ControlsTabX1[2]={-.02f,.36f},ControlsTabY0=.395f,ControlsTabY1=.505f;   // English | Francais
constexpr float ControlsCloseX0=.60f,ControlsCloseX1=.70f,ControlsCloseY0=.41f,ControlsCloseY1=.51f;
inline bool controlsOpen(const Request& r) {return (r.comfortFlags&ComfortControlsOpen)!=0;}
inline bool controlsFrench(const Request& r) {return (r.comfortFlags&ComfortControlsFrench)!=0;}
inline Pose controlsWallPose(const Request& r) {
    Request o=r;o.comfortFlags^=ComfortWallRight;   // the other wall
    return comfortWallPose(o);
}
// 0.9.47.58: in the DUNGEON room the CONTROLS panel (1.40 m) is wider than the bay between two stone ribs (1.30 m): the ribs hid its left column and its close cross
// (report 004800). There the open panel is drawn at 86 %; in the main-menu room (no ribs in front of it) it stays at 100 %. The laser test uses the same factor.
constexpr float ControlsDungeonFit=.86f;
inline uint32_t controlsHit(const Request& r,Vec3 origin,Vec3 dir,float& distance,float fit=1.f) {
    const Pose panel=controlsWallPose(r);
    const Vec3 o=toLocal(panel,origin),d=rotate(conjugate(panel.orientation),dir);
    if(o.z<=0||d.z>=-1e-4f) return 0;
    const float t=-o.z/d.z;if(t<.10f||t>5.f) return 0;
    const float x=o.x+t*d.x,y=o.y+t*d.y;
    if(x>=ComfortButtonX0&&x<=ComfortButtonX1&&y>=ComfortButtonY0&&y<=ComfortButtonY1) {distance=t;return ControlsItemOpen;}
    if(!controlsOpen(r)) return 0;
    const float fx=x/fit,fy=y/fit;   // 0.9.47.58: panel coordinates before the fit
    if(std::fabs(fx)>ControlsHalfW||std::fabs(fy)>ControlsHalfH) return 0;
    distance=t;
    if(fx>=ControlsCloseX0&&fx<=ControlsCloseX1&&fy>=ControlsCloseY0&&fy<=ControlsCloseY1) return ControlsItemClose;
    for(int k=0;k<2;++k) if(fx>=ControlsTabX0[k]&&fx<=ControlsTabX1[k]&&fy>=ControlsTabY0&&fy<=ControlsTabY1) return k==0?ControlsItemTabEn:ControlsItemTabFr;
    return 0xffffffffu;
}
// 0.8.99.3: centre lowered 1.45 -> 1.30 m. 0.8.99.2: width reduced by 25 % (2.6 -> 1.95 m), centre unchanged. 0.8.88: game screen (main menu, cinematics, end of level) on the FRONT wall of the room, 4:3, centred, 2.6 m wide, just in front of the wall.
constexpr float MenuScreenWidth=1.95f,MenuScreenY=1.30f,MenuScreenZ=-2.60f,MenuScreenPlaneOffset=.008f; // 0.8.89.1: 3 cm BEHIND the wall plane: the wall relief (parallax 5 cm) makes the visible stone surface lie behind z=-2.6
// 0.8.90: menu relief. Menu 3D vertices are rebuilt in 'menu space' (units of 1/rhw); MenuRefDepth = depth that lands on the wall plane; interface (rhw >= MenuUiRhw) is flattened just in front of it.
constexpr float MenuRefDepth=1.1f,MenuUiRhw=1.0f;
inline Pose menuScreenPose(const Request& r) {
    const float floorY=(r.handFlags&RequestFloorKnown)?r.floorY:levelTable(r).position.y-.78f;
    const Pose room{r.anchor.orientation,{r.anchor.position.x,floorY,r.anchor.position.z}};
    return {normalized(room.orientation),toWorld(room,{0.f,MenuScreenY,MenuScreenZ})};
}
struct ComfortRect {uint32_t id;float x0,y0,x1,y1;};
constexpr int ComfortMaxItems=24;
// 0.8.94.5 layout (unit space). Top: two columns (left: SNAP TURN, INVERT MAP; right: LASER COLOR, PRIMARY HAND), a label above each control.
// Bottom, full width: PASSTHROUGH VD (ON/OFF + preview of the final colour) and three sliders R, G, B (0..255).
constexpr float ComfortLabelY[4]={.81f,.81f,.37f,.37f};          // SNAP, LASER, INVERT, HAND label centres
constexpr float ComfortColA=-.45f,ComfortColB=.45f;              // column centres
constexpr float ComfortRowAy0=.56f,ComfortRowAy1=.69f,ComfortRowBy0=.11f,ComfortRowBy1=.24f;
constexpr float ComfortDividerY=-.01f,ComfortPassLabelY=-.11f,ComfortPassLabelX=-.42f;
constexpr float ComfortPassBtnX0=.12f,ComfortPassBtnX1=.52f,ComfortPassBtnY0=-.18f,ComfortPassBtnY1=-.03f;
constexpr float ComfortPassPrevX0=.64f,ComfortPassPrevX1=.84f,ComfortPassPrevY0=-.20f,ComfortPassPrevY1=-.01f;
constexpr float ComfortSliderY[3]={-.32f,-.48f,-.64f};          // R, G, B row centres (0.9.35: closer, room for TABLE SIZE)
constexpr float ComfortSwatchX0=-.86f,ComfortSwatchX1=-.71f,ComfortSwatchHalf=.062f;
constexpr float ComfortTrackX0=-.62f,ComfortTrackX1=.48f,ComfortTrackHalf=.028f,ComfortSliderHit=.075f;
constexpr float ComfortValueX=.68f;                               // centre of the 0..255 readout
// 0.8.94.15: TABLE ZOOM section (one slider, 0..100 % of the extra zoom-out the mod adds on top of the game's own limit)
constexpr float ComfortZoomDividerY=-.73f,ComfortZoomY=-.84f,ComfortZoomLabelY=ComfortZoomY,ComfortZoomLabelX=-.56f;   // 0.9.35: label and bar on one row
constexpr float ComfortTableY=-.90f,ComfortTableLabelX=-.56f;   // 0.9.47.21: ZOOM - MAX removed from the panel, TABLE SIZE takes the middle of the freed space
constexpr float TableSizeSnapPct=100.f,TableSizeSnapRange=6.f;     // 0.9.47.21: magnetic detent of the TABLE SIZE bar (100 +/- 6 % reads exactly 100)
constexpr float ComfortOrbY=-1.13f,ComfortOrbLabelX=-.56f,ComfortOrbBtnX0=.12f,ComfortOrbBtnX1=.52f,ComfortOrbBtnY0=-1.22f,ComfortOrbBtnY1=-1.04f;   // 0.9.47.41: LEFT HAND ORB row
// 0.9.47.61: PC MIRROR row at the very TOP of the panel (above SNAP TURN / LASER COLOR), clear of the close cross (x .80..93)
constexpr float ComfortMirrorY=1.05f,ComfortMirrorLabelX=-.56f,ComfortMirrorBtnX0=.12f,ComfortMirrorBtnX1=.52f,ComfortMirrorBtnY0=.96f,ComfortMirrorBtnY1=1.14f;
constexpr float ComfortZoomTrackX0=-.10f,ComfortZoomTrackX1=.62f;
inline int comfortLayout(ComfortRect out[ComfortMaxItems]) {
    int n=0;
    out[n++]={ComfortItemSnap,ComfortColA-.35f,ComfortRowAy0,ComfortColA+.35f,ComfortRowAy1};
    for(uint32_t c=0;c<ComfortColours;++c) {const float x=ComfortColB+(float(c)-3.f)*.12f;out[n++]={ComfortItemColour+c,x-.058f,ComfortRowAy0,x+.058f,ComfortRowAy1};}
    out[n++]={ComfortItemInvert,ComfortColA-.35f,ComfortRowBy0,ComfortColA+.35f,ComfortRowBy1};
    out[n++]={ComfortItemHandLeft,ComfortColB-.37f,ComfortRowBy0,ComfortColB-.02f,ComfortRowBy1};
    out[n++]={ComfortItemHandRight,ComfortColB+.02f,ComfortRowBy0,ComfortColB+.37f,ComfortRowBy1};
    out[n++]={ComfortItemPassthrough,ComfortPassBtnX0,ComfortPassBtnY0,ComfortPassBtnX1,ComfortPassBtnY1};
    for(int k=0;k<3;++k) out[n++]={ComfortItemSliderR+uint32_t(k),ComfortTrackX0-.02f,ComfortSliderY[k]-ComfortSliderHit,ComfortTrackX1+.02f,ComfortSliderY[k]+ComfortSliderHit};
    // 0.9.47.21: no ZOOM - MAX control any more (the stored ZoomOutExtra value is kept and still applied)
    out[n++]={ComfortItemSliderTable,ComfortZoomTrackX0-.02f,ComfortTableY-.070f,ComfortZoomTrackX1+.02f,ComfortTableY+.070f};
    out[n++]={ComfortItemOrb,ComfortOrbBtnX0,ComfortOrbBtnY0,ComfortOrbBtnX1,ComfortOrbBtnY1};
    out[n++]={ComfortItemMirror,ComfortMirrorBtnX0,ComfortMirrorBtnY0,ComfortMirrorBtnX1,ComfortMirrorBtnY1};
    out[n++]={ComfortItemClose,.80f,.99f,.93f,1.12f};
    return n;
}
inline bool comfortIsSlider(uint32_t id) {return (id>=ComfortItemSliderR&&id<=ComfortItemSliderB)||id==ComfortItemSliderZoom||id==ComfortItemSliderTable;}
// 0.9.47.21: magnetic detent of TABLE SIZE: within +/-6 % of 100 the value is exactly 100
inline float tableSizeSnap(float pct) {return std::fabs(pct-TableSizeSnapPct)<=TableSizeSnapRange?TableSizeSnapPct:pct;}
// Ray (LOCAL space) against the wall: returns the item id (0 none) and the distance. Closed: only the OPTIONS button exists.
// Open: the panel, its controls and the cross; 0xffffffff = on the panel between controls (the laser stops, nothing is pressed).
inline uint32_t comfortHit(const Request& r,Vec3 origin,Vec3 dir,float& distance) {
    const Pose panel=comfortWallPose(r);
    const Vec3 o=toLocal(panel,origin),d=rotate(conjugate(panel.orientation),dir);
    if(o.z<=0||d.z>=-1e-4f) return 0;
    const float t=-o.z/d.z;if(t<.10f||t>5.f) return 0;
    const float x=o.x+t*d.x,y=o.y+t*d.y;
    if(x>=ComfortButtonX0&&x<=ComfortButtonX1&&y>=ComfortButtonY0&&y<=ComfortButtonY1) {distance=t;return ComfortItemOpen;}
    if(!comfortPanelOpen(r)) return 0;
    if(std::fabs(x)>ComfortPanelHalfW*ComfortPanelScale||std::fabs(y)>ComfortPanelHalfH*ComfortPanelScale) return 0;
    distance=t;
    const float ux=x/ComfortPanelScale,uy=y/ComfortPanelScale;
    ComfortRect items[ComfortMaxItems];const int n=comfortLayout(items);
    for(int i=0;i<n;++i) if(ux>=items[i].x0&&ux<=items[i].x1&&uy>=items[i].y0&&uy<=items[i].y1) return items[i].id;
    return 0xffffffffu;
}
// 0.8.94.5: value (0..255) of a slider for a ray, wherever the ray crosses the wall plane (the drag keeps working when the laser leaves the bar).
// 'distance' = distance to the wall plane. Returns false if the ray does not reach the plane.
inline bool comfortSliderValue(const Request& r,Vec3 origin,Vec3 dir,float& value,float& distance,bool zoomSlider=false,bool tableSlider=false) {
    const Pose panel=comfortWallPose(r);
    const Vec3 o=toLocal(panel,origin),d=rotate(conjugate(panel.orientation),dir);
    if(o.z<=0||d.z>=-1e-4f) return false;
    const float t=-o.z/d.z;if(t<.05f||t>8.f) return false;
    const float ux=(o.x+t*d.x)/ComfortPanelScale;
    const bool wide=zoomSlider||tableSlider;
    const float x0=wide?ComfortZoomTrackX0:ComfortTrackX0,x1=wide?ComfortZoomTrackX1:ComfortTrackX1;
    const float f=std::clamp((ux-x0)/(x1-x0),0.f,1.f);
    value=tableSlider?tableSizeSnap(TableSizeMinPct+f*(TableSizeMaxPct-TableSizeMinPct)):f*(zoomSlider?100.f:255.f);distance=t;return true;
}
// 0.8.94.15: extra zoom-out, 0..100 % (ZoomOutExtra x100), carried in the free Request::wheelPose.orientation.x.
inline float zoomExtraPercent(const Request& r) {const float v=r.wheelPose.orientation.x;return std::isfinite(v)?std::clamp(v,0.f,100.f):100.f;}
// 0.9.35: table size, 50..250 % of TableWidth, carried in Request::wheelPose.orientation.y (shown by the options panel)
inline float tableSizePercent(const Request& r) {const float v=r.wheelPose.orientation.y;return std::isfinite(v)&&v>=TableSizeMinPct-.5f?std::clamp(v,TableSizeMinPct,TableSizeMaxPct):100.f;}
// Passthrough colour carried in Request::wheelPose.position (the skill wheel is disabled): x = red, y = green, z = blue, 0..255.
inline void passthroughRgb(const Request& r,float rgb[3]) {
    const float v[3]={r.wheelPose.position.x,r.wheelPose.position.y,r.wheelPose.position.z};
    for(int i=0;i<3;++i) rgb[i]=std::isfinite(v[i])?std::clamp(v[i],0.f,255.f):(i==1?255.f:0.f);
}
// Laser colours, in the order of the panel: red, blue, yellow, orange, green, violet, pink (0..255 each).
inline void laserColourRgb(uint32_t index,float rgb[3]) {
    static const float table[ComfortColours][3]={{255,30,22},{50,110,255},{255,225,40},{255,140,20},{60,230,80},{165,70,255},{255,105,190}};
    const float* c=table[index<ComfortColours?index:0];rgb[0]=c[0];rgb[1]=c[1];rgb[2]=c[2];
}
}
