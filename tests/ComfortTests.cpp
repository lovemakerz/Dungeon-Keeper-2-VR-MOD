#include "../src/common/Comfort.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
using namespace dk2vr;
int main() {
    Request r{};r.anchor={{0,0,0,1},{2,1.6f,3}};r.table.position.y=.8f;
    r.handFlags=RequestFloorKnown;r.floorY=0;
    // 0.8.86: options panel on the far wall (front wall, centred: room x = 0, y 2.0, z = -2.25, facing +Z = toward the player), half size, OPTIONS button + cross
    Pose panel=comfortWallPose(r);
    // 0.9.47.41: the panel moved to the LEFT wall (room x = -2.385, y .88, z = -1.15), facing +X (into the room)
    assert(std::fabs(panel.position.x-(2.f+ComfortWallX))<1e-4f&&std::fabs(panel.position.y-ComfortWallY)<1e-4f&&std::fabs(panel.position.z-(3+ComfortWallZ))<1e-4f);
    assert(rotate(panel.orientation,{0,0,1}).x>.99f&&rotate(panel.orientation,{1,0,0}).z<-.99f);
    assert((PointerComfort&PointerLeft)==0&&(PointerComfort&PointerRight)==0);
    r.anchor.orientation=yawQuat(Pi*.5f);
    panel=comfortWallPose(r);
    const Vec3 loc=toLocal(r.anchor,panel.position);
    assert(std::fabs(loc.x-ComfortWallX)<1e-4f&&std::fabs(loc.z-ComfortWallZ)<1e-4f);
    {const Vec3 n=rotate(panel.orientation,{0,0,1}),e=rotate(r.anchor.orientation,{1,0,0});assert(std::fabs(dot(n,e)-1)<1e-4f);}   // faces room +X whatever the room yaw
    assert(ComfortPanelScale==.44f&&(ComfortPanelOpen&(ComfortLaserMask|ComfortSnapTurn|ComfortInvertTurn))==0);
    {Request q=r;q.comfortFlags|=ComfortWallRight;const Pose rp=comfortWallPose(q);const Vec3 l2=toLocal(q.anchor,rp.position);   // 0.9.47.42: right wall
     assert(std::fabs(l2.x+ComfortWallX)<1e-4f&&std::fabs(l2.z-ComfortWallZ)<1e-4f);
     assert(std::fabs(dot(rotate(rp.orientation,{0,0,1}),rotate(q.anchor.orientation,{-1,0,0}))-1)<1e-4f);
     assert((ComfortWallRight&(ComfortLaserMask|ComfortSnapTurn|ComfortInvertTurn|ComfortPanelOpen|ComfortPassthrough|ComfortHideOrb))==0);}
    {   // 0.9.47.49: CONTROLS wall = the wall opposite to the options; button always answers, panel only when open, tabs and cross
        Request q=r;const Pose cp=controlsWallPose(q);const Vec3 lc=toLocal(q.anchor,cp.position);
        assert(std::fabs(lc.x+ComfortWallX)<1e-4f&&std::fabs(lc.z-ComfortWallZ)<1e-4f);                 // options on the left -> controls on the right
        Request qr=q;qr.comfortFlags|=ComfortWallRight;assert(std::fabs(toLocal(qr.anchor,controlsWallPose(qr).position).x-ComfortWallX)<1e-4f);   // and the reverse
        assert((ComfortControlsOpen|ComfortControlsFrench)&&((ComfortControlsOpen|ComfortControlsFrench)&(ComfortLaserMask|ComfortSnapTurn|ComfortInvertTurn|ComfortPanelOpen|ComfortPassthrough|ComfortHideOrb|ComfortWallRight))==0);
        const Vec3 cf=rotate(cp.orientation,{0,0,-1});float t=0;
        auto go=[&](const Request& rq,float lx,float ly){const Pose pp=controlsWallPose(rq);const Vec3 from=toWorld(pp,{lx,ly,1.2f});return controlsHit(rq,from,rotate(pp.orientation,{0,0,-1}),t);};
        (void)cf;
        assert(go(q,0,(ComfortButtonY0+ComfortButtonY1)*.5f)==ControlsItemOpen);
        assert(go(q,ControlsTabX0[0]+.05f,.45f)==0);                                                        // closed: tabs do not exist
        q.comfortFlags|=ComfortControlsOpen;
        assert(go(q,-.2f,.45f)==ControlsItemTabEn&&go(q,.2f,.45f)==ControlsItemTabFr&&go(q,.65f,.46f)==ControlsItemClose);
        assert(go(q,0,-.1f)==0xffffffffu&&go(q,1.0f,0)==0);
    }
    const Vec3 fwd=rotate(panel.orientation,{0,0,-1});
    auto fire=[&](float lx,float ly,float& t){const Vec3 from=toWorld(panel,{lx,ly,1.2f});return comfortHit(r,from,fwd,t);};
    {
        float t=0;
        // closed: only the OPTIONS button answers
        assert(!comfortPanelOpen(r));
        assert(fire(0,(ComfortButtonY0+ComfortButtonY1)*.5f,t)==ComfortItemOpen&&std::fabs(t-1.2f)<1e-3f);
        assert(fire(0,0,t)==0&&fire(.3f,.1f,t)==0);
        r.comfortFlags|=ComfortPanelOpen;assert(comfortPanelOpen(r));
        ComfortRect items[ComfortMaxItems];const int n=comfortLayout(items);assert(n==19);   // 0.9.47.21: ZOOM - MAX removed (18->17); 0.8.94.15: + the DEZOOM TABLE slider; 0.9.35: + TABLE SIZE
        for(int a=0;a<n;++a) for(int b=a+1;b<n;++b) assert(items[a].x1<=items[b].x0||items[b].x1<=items[a].x0||items[a].y1<=items[b].y0||items[b].y1<=items[a].y0);
        for(int a=0;a<n;++a) {   // each control is hit at its centre (unit space * 0.5 = metres)
            const float cx=(items[a].x0+items[a].x1)*.5f*ComfortPanelScale,cy=(items[a].y0+items[a].y1)*.5f*ComfortPanelScale;
            assert(fire(cx,cy,t)==items[a].id&&std::fabs(t-1.2f)<1e-3f);
        }
        assert(fire(0,(ComfortButtonY0+ComfortButtonY1)*.5f,t)==ComfortItemOpen);          // the button still answers when open (toggle)
        assert(fire(0,-ComfortPanelHalfH*ComfortPanelScale-.05f,t)==0);                      // below the panel
        assert(fire(0,ComfortPanelHalfH*ComfortPanelScale+.018f,t)==0);                      // between the panel and the button above it
        {bool mir=false;for(int a=0;a<n;++a) mir|=items[a].id==ComfortItemMirror;assert(mir&&ComfortMirrorOn!=0&&(ComfortMirrorOn&(ComfortLaserMask|ComfortSnapTurn|ComfortInvertTurn|ComfortPanelOpen|ComfortPassthrough|ComfortHideOrb|ComfortWallRight|ComfortControlsOpen|ComfortControlsFrench))==0);
         for(int a=0;a<n;++a) if(items[a].id!=ComfortItemMirror&&items[a].id!=ComfortItemClose) assert(items[a].y1<ComfortMirrorBtnY0-.02f);}   // PC MIRROR is the TOP row
        {bool orb=false;for(int a=0;a<n;++a) orb|=items[a].id==ComfortItemOrb;assert(orb&&(ComfortHideOrb&(ComfortLaserMask|ComfortSnapTurn|ComfortInvertTurn|ComfortPanelOpen|ComfortPassthrough))==0);}
        assert(fire(ComfortPanelHalfW*ComfortPanelScale+.05f,0,t)==0);                       // right of the panel
        // 0.8.94.5: everything stays inside the panel, no overlap between labels/controls rows, sliders map the ray to 0..255 even off the bar
        for(int a=0;a<n;++a) assert(items[a].x0>=-ComfortPanelHalfW&&items[a].x1<=ComfortPanelHalfW&&items[a].y0>=-ComfortPanelHalfH&&items[a].y1<=ComfortPanelHalfH);
        assert(ComfortLabelY[0]-.04f>ComfortRowAy1&&ComfortLabelY[2]-.04f>ComfortRowBy1&&ComfortRowAy0>ComfortLabelY[2]+.04f&&ComfortRowBy0>ComfortDividerY+.02f);
        assert(ComfortPassLabelY+.04f<ComfortDividerY&&ComfortSliderY[0]+ComfortSliderHit<ComfortPassBtnY0);
        {float v=0,td=0;
         const Vec3 from=toWorld(panel,{ComfortTrackX0*ComfortPanelScale,ComfortSliderY[1]*ComfortPanelScale,1.2f});
         assert(comfortSliderValue(r,from,fwd,v,td)&&std::fabs(v)<.01f&&std::fabs(td-1.2f)<1e-3f);
         const Vec3 mid=toWorld(panel,{(ComfortTrackX0+ComfortTrackX1)*.5f*ComfortPanelScale,5.f,1.2f});
         assert(comfortSliderValue(r,mid,fwd,v,td)&&std::fabs(v-127.5f)<.01f);          // far above the bar: the drag still reads the x position
         const Vec3 far=toWorld(panel,{3.f,0,1.2f});assert(comfortSliderValue(r,far,fwd,v,td)&&v==255.f);
         const Vec3 zf=toWorld(panel,{ComfortZoomTrackX0*ComfortPanelScale,ComfortZoomY*ComfortPanelScale,1.2f});
         assert(comfortSliderValue(r,zf,fwd,v,td,true)&&std::fabs(v)<.01f);
         const Vec3 zt=toWorld(panel,{ComfortZoomTrackX1*ComfortPanelScale,ComfortZoomY*ComfortPanelScale,1.2f});
         assert(comfortSliderValue(r,zt,fwd,v,td,true)&&std::fabs(v-100.f)<.01f);
         {const Vec3 a0=toWorld(panel,{ComfortZoomTrackX0*ComfortPanelScale,ComfortTableY*ComfortPanelScale,1.2f}),a1=toWorld(panel,{ComfortZoomTrackX1*ComfortPanelScale,ComfortTableY*ComfortPanelScale,1.2f});
          assert(comfortSliderValue(r,a0,fwd,v,td,false,true)&&std::fabs(v-TableSizeMinPct)<.01f&&comfortSliderValue(r,a1,fwd,v,td,false,true)&&std::fabs(v-TableSizeMaxPct)<.01f);
          Request q2{};assert(tableSizePercent(q2)==100.f);q2.wheelPose.orientation.y=180.f;assert(tableSizePercent(q2)==180.f);q2.wheelPose.orientation.y=999.f;assert(tableSizePercent(q2)==TableSizeMaxPct);
          assert(comfortIsSlider(ComfortItemSliderTable));   // 0.9.35 TABLE SIZE
          // 0.9.47.21: magnetic detent at 100 %, and no ZOOM - MAX control left in the layout
          assert(tableSizeSnap(100.f)==100.f&&tableSizeSnap(94.5f)==100.f&&tableSizeSnap(105.9f)==100.f&&tableSizeSnap(93.f)==93.f&&tableSizeSnap(107.f)==107.f&&tableSizeSnap(50.f)==50.f&&tableSizeSnap(250.f)==250.f);
          for(float pct:{97.f,100.f,103.f}) {const float f=(pct-TableSizeMinPct)/(TableSizeMaxPct-TableSizeMinPct);
              const Vec3 sp=toWorld(panel,{(ComfortZoomTrackX0+f*(ComfortZoomTrackX1-ComfortZoomTrackX0))*ComfortPanelScale,ComfortTableY*ComfortPanelScale,1.2f});
              assert(comfortSliderValue(r,sp,fwd,v,td,false,true)&&v==100.f);}
          {ComfortRect it2[ComfortMaxItems];const int n2=comfortLayout(it2);for(int i=0;i<n2;++i) assert(it2[i].id!=ComfortItemSliderZoom);}}
         {Request z{};z.wheelPose.orientation.x=37.f;assert(std::fabs(zoomExtraPercent(z)-37.f)<1e-4f);
          z.wheelPose.orientation.x=NAN;assert(zoomExtraPercent(z)==100.f);}
         float rgb[3];Request q{};q.wheelPose.position={0,255,0};passthroughRgb(q,rgb);assert(rgb[1]==255&&rgb[0]==0);
         q.wheelPose.position={NAN,300,-4};passthroughRgb(q,rgb);assert(rgb[0]==0&&rgb[1]==255&&rgb[2]==0);}
        assert(fire(0,(ComfortRowAy1+.04f)*ComfortPanelScale,t)==0xffffffffu);            // on the panel, between controls
        const Vec3 back=toWorld(panel,{0,0,-1.2f});assert(comfortHit(r,back,rotate(panel.orientation,{0,0,1}),t)==0);   // behind the wall
        float rgb[3];for(uint32_t c=0;c<ComfortColours;++c) {laserColourRgb(c,rgb);assert(rgb[0]>=0&&rgb[0]<=255&&(rgb[0]+rgb[1]+rgb[2])>200);}
        float a0[3],a1[3];laserColourRgb(0,a0);laserColourRgb(1,a1);assert(a0[0]!=a1[0]);
        assert(((ComfortLaserMask>>ComfortLaserShift)==7)&&(ComfortInvertTurn&(ComfortLaserMask|ComfortSnapTurn))==0);
        assert((ComfortPassthrough&(ComfortLaserMask|ComfortSnapTurn|ComfortInvertTurn|ComfortPanelOpen))==0);
        // the panel (with its frame) and the button never overlap in metres
        assert(ComfortButtonY0>ComfortPanelHalfH*ComfortPanelScale+.03f*ComfortPanelScale);   // the button sits above the panel frame
    }
    bool snap=false;float degrees=30;
    for(float expected:{15.f,30.f,45.f,90.f}) {cycleSnapTurn(snap,degrees);assert(snap&&degrees==expected);}
    cycleSnapTurn(snap,degrees);assert(!snap);
    const float correction=possessionCounterPitch(.412f,.707f);
    assert(correction>.5f&&correction<.6f);
    assert(rotate(pitch(-correction),{0,0,-1}).y<-.5f); // native centre moves below level gaze
    assert(possessionCounterPitch(NAN,1.f)==0);
    assert(std::fabs(possessionEntryPitch(0,0)-possessionCounterPitch(.324f,.707f))<1e-5f);
    assert(std::fabs(possessionEntryPitch(.412f,.707f)-correction)<1e-5f);
    const Projection lens{176,176,512,384};
    for(Vec3 nativeUp: {Vec3{0,.324f,.707f},Vec3{.03f,.324f,.707f},Vec3{-.08f,.412f,.68f}}) {
        Vec3 up{-nativeUp.x*512/176,nativeUp.y*384/176,nativeUp.z};assert(normalize(up));
        const Vec3 level=rotate(possessionLevelRotation(nativeUp,512,384,lens),up);
        assert(std::fabs(level.x)<1e-5f&&std::fabs(level.z)<1e-5f&&std::fabs(level.y-1)<1e-5f);
    }
    {   // 0.8.75: the live level. Native up x values seen in the 0.8.74 report (-0.011 .. +0.008, 45 degree camera): each one must give a level floor,
        // and the roll they stand for is the amount the frozen level let through (about 1.8 degrees for x = 0.011).
        const Quat frozen=possessionLevelRotation({0,.324f,.70710678f},512,384,lens);
        for(float x:{-.011f,-.004f,0.f,.008f,.011f}) {
            const Vec3 nativeUp{x,.324f,.70710678f};
            Vec3 up{-nativeUp.x*512/176,nativeUp.y*384/176,nativeUp.z};assert(normalize(up));
            const Vec3 live=rotate(possessionLevelRotation(nativeUp,512,384,lens),up);
            assert(std::fabs(live.x)<1e-5f&&std::fabs(live.z)<1e-5f&&std::fabs(live.y-1)<1e-5f);   // live level: the floor is level
            const Vec3 frozenSees=rotate(frozen,up);                                               // frozen level: the floor is tilted by the camera roll
            const double rollDeg=std::atan2(double(frozenSees.x),double(frozenSees.y))*180./Pi;
            printf("live level: native up x %+.3f -> roll the frozen level lets through %+.2f deg\n",x,rollDeg);
            if(x==.011f) assert(std::fabs(rollDeg)>1.5&&std::fabs(rollDeg)<2.2);
        }
    }
    Vec3 fallback=rotate(possessionLevelRotation({0,0,0},512,384,lens),{0,.70710678f,.70710678f});
    assert(std::fabs(fallback.y-1)<1e-5f&&std::fabs(fallback.z)<1e-5f);
    // Real 0.8.36 report: entry is the previous table projection, then the
    // manual recenter reads the actual first-person projection at 45 degrees.
    const Vec3 entryTable{0,.1737f,-0.f};
    assert(!possessionProjectionReady(entryTable,512,384,lens));
    const Quat entry=possessionLevelRotation(entryTable,512,384,lens);
    const Quat manual=possessionLevelRotation({0,.324f,.70710678f},512,384,lens);
    assert(std::fabs(2*std::atan2(-entry.x,entry.w)-Pi*.25f)<1e-5f);
    assert(std::fabs(2*std::atan2(-manual.x,manual.w)-Pi*.25f)<.001f);
    assert(possessionProjectionReady({0,.324f,.70710678f},512,384,lens));
    {   // 0.8.99.1: possessed chicken (report 20261006_101213): level camera native Z (0, .461, -0) is a valid first-person view, levelled as it is
        const Vec3 chicken{0,.461f,-0.f};
        assert(possessionProjectionReady(chicken,512,384,lens));
        const Quat lv=possessionLevelRotation(chicken,512,384,lens);
        assert(std::fabs(2*std::atan2(-lv.x,lv.w))<1e-4f);   // no pitch correction: the camera already looks straight ahead
        assert(possessionProjectionReady({0,.40f,.20f},512,384,lens));
        assert(!possessionProjectionReady({0,.25f,0.f},512,384,lens));   // small y with no depth: still the table camera of the entry frame
        printf("possessed chicken (level camera): accepted, levelled without pitch\n");
    }
    assert(!possessionProjectionReady({0,NAN,1},512,384,lens));
    assert(!possessionProjectionReady({0,.324f,.707f},NAN,384,lens));
    for(float depth:{-8.f,-2.f,.01f,2.f,8.f}) {
        Vec3 point{1.25f,-.75f,-depth},out;
        assert(unprojectOmni(lens.centerX+lens.focalX*point.x/depth,lens.centerY-lens.focalY*point.y/depth,1/depth,lens,1,out));
        assert(std::fabs(out.x-point.x)<1e-4f&&std::fabs(out.y-point.y)<1e-4f&&std::fabs(out.z-point.z)<1e-4f);
    }
    Vec3 invalid;assert(!unprojectOmni(0,0,0,lens,1,invalid));assert(!unprojectOmni(0,0,NAN,lens,1,invalid));
    assert(showWorldProjection(0,2,0)); // no PC flash if the possessed frustum has no native draw
    assert(showWorldProjection(2,0,0));
    assert(!showWorldProjection(0,0,0));
    assert(!showWorldProjection(3,2,12));
    puts("PASS: options wall panel (hit test, colours), possession comfort and non-game pointer flag.");
}
