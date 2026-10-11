// DK2VR 0.8.69 - possession world at the headset rate, offline check of the REAL code of the VR thread (PossessionMotion::observe /
// extrapolate, extracted from src/plugin/Decoupled.inl at build time) on a synthetic first-person world. NEW in 0.8.69: the game turns
// in STEPS like the real one (mouse events injected at 90 Hz, read by the game at an irregular ~100 Hz, gain 1.48, a few ms of latency),
// which is what made the 0.8.68 extrapolation tremble (a late frame sent the world back). The old 0.8.68 algorithm is replayed inline as
// the witness: the test must show its backward steps and the absence of them with the new code.
#include "../src/common/MotionFit.hpp"
#include "../src/common/Decouple.hpp"
#include "../src/common/HeadingFollow.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <random>
#include <unordered_map>
#include <vector>
#include <cstdint>
typedef unsigned int UINT;
enum D3DPRIMITIVETYPE {D3DPT_POINTLIST=1,D3DPT_LINELIST=2,D3DPT_LINESTRIP=3,D3DPT_TRIANGLELIST=4,D3DPT_TRIANGLESTRIP=5,D3DPT_TRIANGLEFAN=6};
enum D3DFORMAT {D3DFMT_INDEX16=101,D3DFMT_INDEX32=102};
using namespace dk2vr;
static double fakeNow=0;static double milliseconds() {return fakeNow;}
static void log(const char* f,...) {(void)f;}
struct DrawRecord {
    D3DPRIMITIVETYPE type;UINT primitives,count,stride;DWORD fvf;
    size_t vertices,indices;D3DFORMAT indexFormat;bool indexed;uint32_t snapshot;
    bool hand=false;bool held3d=false;uint8_t view=0;bool overlay=false;
};
struct Package {
    uint64_t id=0;uint32_t epoch=0;
    std::vector<unsigned char> vertices,indices;std::vector<DrawRecord> records;
    bool panning=false,turning=false;uint32_t mode=0;float tableWidth=1;
    bool possession=false;Vec3 possPivot{};double contentMs=0;
    float possYaw=0;bool possYawOk=false;
    bool cube=false;Vec3 faceO{};Vec3 faceN[6][4]{};
    float possUp[3]{};bool possUpOk=false;
    int32_t possCam[3]{};float possCamZ=0;bool possCamOk=false;
};
#include "PossessionMotionExtract.inl"
static void expect(bool ok,const char* message) {if(!ok){printf("FAIL: %s\n",message);exit(1);}}
struct Obj {Vec3 pos;int count;bool mover;Vec3 vel;float phase;};
static double gGain=2.5;static bool gBob=false;static double gTick=0.033;static float gMover=.15f;static float gSpan=18.f;static bool gYaw=true,gCam=true,gCamMirror=false,gWitness=false;static double gSparseFrom=1e9;static int gSparseKeep=6;static bool gReverse=false;static double gWander=0;static double gLen=2400;static bool gAlong=false;static bool gClean=false,gNotch=false;static double gWob=0,gWobYaw=0;
static bool gInterp=false;   // 0.8.98: interpolated view (default of the mod); false = the 0.8.69-0.8.80 model
static bool gInterp098=false;static bool gLowLat=false;   // 0.8.99.1: the 0.8.99 low-latency settings (INI options since 0.8.99.1, defaults = 0.8.98)   // 0.8.99 witness: the 0.8.98 settings (lead 1 interval + 3 ms, carried on half an interval, no fading correction)
static bool gLateArrival=false; // 0.8.99: a game frame reaches the VR thread one game frame interval after its content (GPU-bound game, report 094957: arrival = interval + 6 ms)
static double gLastDelay=0,gLastOvershoot=0,gLastLag=0,gLastRough=0;static int gLastBack=0;
static std::vector<double> dbgC,dbgS,dbgC2,dbgS2;static double gDevC=0,gDevS=0;
static int scenario(int objectCount,double turnRate,double frameMin,double frameSpan,bool strict,bool walkSteps,float smoothMs,float moveSmooth=45.f) {
    std::mt19937 rng(11);std::uniform_real_distribution<float> U(0.f,1.f);
    const Vec3 pivot{0.f,0.f,0.f};
    // world: 320 draws of 4 mesh sizes (many identical signatures), 15 % movers (creatures), spread on a 18 m square around the creature
    std::vector<Obj> objs;const int sizes[4]={24,36,60,150};
    for(int i=0;i<objectCount;++i) objs.push_back({{(U(rng)-.5f)*gSpan,U(rng)*1.2f,(U(rng)-.5f)*gSpan},sizes[rng()%4],U(rng)<gMover,{(U(rng)-.5f)*.8f,0,(U(rng)-.5f)*.8f},U(rng)*6.f});
    auto buildPackage=[&](uint64_t id,double t,float yaw,Vec3 walk)->std::shared_ptr<Package> {
        auto p=std::make_shared<Package>();p->id=id;p->epoch=1;p->possession=true;p->possPivot=pivot;p->contentMs=t;p->mode=2;
        // the game's camera position: ints in 1/4096 tiles (3 scene units per tile), axes turned by 90 degrees (and mirrored when asked), height as an int and as a float
        p->possCamOk=gCam;p->possCam[0]=int32_t(std::lround(4096.*(-walk.z)/3.));p->possCam[1]=int32_t(std::lround(4096.*(gCamMirror?-walk.x:walk.x)/3.));p->possCam[2]=int32_t(std::lround(4096.*walk.y*.25));p->possCamZ=-walk.y*.5f;
        p->possYawOk=gYaw;p->possYaw=-yaw;   // the game's camera heading: opposite sign to the world rotation (learned by the code)
        for(size_t i=0;i<objs.size();++i) {
            if(t>gSparseFrom&&int(i)>=gSparseKeep) continue;   // the camera looks at (almost) nothing
            Obj o=objs[i];if(o.mover) {o.pos.x+=o.vel.x*float(t*.001);o.pos.z+=o.vel.z*float(t*.001);}
            // creature at P (game world) with heading yaw: the world around the anchor is L = R(yaw)(X - P)
            Vec3 r=rotate(yawQuat(yaw),sub(o.pos,walk));r=add(r,pivot);r.y+=float(gWander*std::sin(6.2831853*.4*t*.001));   // vertical wander of the whole picture that the game camera height does not carry (report 013045)
            // frustum: the game only draws what lies in front-ish (about 110 degrees), as it does in possession
            const float ang=std::atan2(r.x,-r.z);if(std::fabs(ang)>1.0f||std::fabs(r.x)+std::fabs(r.z)<.4f) continue;
            DrawRecord rec{D3DPT_TRIANGLELIST,UINT(o.count/3),UINT(o.count),16,0x2,p->vertices.size(),0,D3DFMT_INDEX16,false,0};
            p->vertices.resize(p->vertices.size()+size_t(o.count)*16);
            unsigned char* out=p->vertices.data()+rec.vertices;
            for(int v=0;v<o.count;++v) {
                const float a=v*6.2831853f/o.count;
                Vec3 q{r.x+.25f*std::cos(a)*(1+.3f*std::sin(3*a)),r.y+.2f*std::sin(a*2),r.z+.25f*std::sin(a)};
                memcpy(out+v*16,&q,12);float uv=0;memcpy(out+v*16+12,&uv,4);
            }
            p->records.push_back(rec);
        }
        return p;
    };

    // ---- the game's response: mouse events (what the host injects at 90 Hz) are consumed at irregular game frames -------------------------------------------------
    const double gain=gGain;
    const double ts=100;
    // commanded body turn: ramp up 0.25 s, hold until 1.6 s, ramp down 0.2 s (stick), rate in rad/s (what the user asks)
    auto cmdRate=[&](double ms)->double{const double s=(ms-ts)*.001;if(s<0) return 0;if(s<.25) return turnRate*s/.25;if(gReverse) {if(s<1.0) return turnRate;if(s<1.1) return turnRate*(1.-(s-1.0)/.05);if(s<1.5) return -turnRate;if(s<1.7) return -turnRate*(1.7-s)/.2;return 0;}if(s<1.6) return turnRate;if(s<1.8) return turnRate*(1.8-s)/.2;return 0;};
    // events: one per 11.11 ms headset frame (the host sends possessionTurn deltas per request), arriving 3..6 ms later
    std::vector<std::pair<double,double>> events;{double cum=0;std::mt19937 er(5);for(double t=ts-100;t<ts+gLen;t+=1000./90.) {const double d=cmdRate(t)*(1./90.);cum+=d;events.push_back({t+3+er()%4,d});}}
    auto cmdCum=[&](double ms)->double{double c=0;for(const auto& e:events) {if(e.first-3>ms) break;c+=e.second;}return c;}; // commanded yaw already sent at ms
    auto stairYaw=[&](double ms)->float{double c=0;for(const auto& e:events) {if(e.first>ms) break;c+=e.second;}return float(c*gain+(ms>ts+300?gWobYaw*std::sin(6.2831853*.62*(ms-ts-300)*.001):0.));};   // 0.8.77: the game camera's own heading sway while walking
    // creature position: 2 m/s forward from 0.3 s on; either smooth (rendered with interpolation) or in game logic ticks of 33 ms
    // 0.8.77: gAlong = the creature walks along its own heading (the stick forward), integrated from the heading of the game at 4 ms steps
    std::vector<Vec3> alongTab;if(gAlong) {alongTab.push_back({0,0,0});Vec3 pcur{0,0,0};for(double m=ts+300;m<ts+gLen+50;m+=4.) {const float yw=stairYaw(m+2);const double spd=1.9*std::clamp((m-ts-300)*.001/.25,0.,1.);const Vec3 f=rotate(yawQuat(-yw),{0.f,0.f,-1.f});pcur.x+=float(f.x*spd*.004);pcur.z+=float(f.z*spd*.004);alongTab.push_back(pcur);}}
    auto alongAt=[&](double ms)->Vec3{if(alongTab.empty()||ms<=ts+300) return {0,0,0};const double u=(ms-ts-300)/4.;const size_t i=std::min(alongTab.size()-1,size_t(u));return alongTab[i];};
    auto walkOf=[&](double ms)->Vec3{if(gAlong) {double mm=ms;if(walkSteps) mm=ts+300+std::floor((ms-ts-300)/(gTick*1000.))*gTick*1000.;Vec3 a=alongAt(mm);const double s2=std::clamp((mm-ts-300)*.001,0.,6.);const double wv=(gWob>0&&s2>0)?std::sin(6.2831853*.62*s2):0.;a.x+=float(gWob*wv);a.z+=float(.8*gWob*std::sin(6.2831853*.62*s2+1.)*(s2>0?1.:0.));a.y=float(gBob&&s2>0?.03*std::sin(6.2831853*2.6666667*s2):0.);return a;}
        double s=std::clamp((ms-ts-300)*.001,0.,6.);if(walkSteps) s=std::floor(s/gTick)*gTick;
        // walk animation (when gBob): 3 cm of vertical bob at 2.67 Hz and 3 cm of sideways sway at 1.33 Hz, both updated with the game ticks
        const double a=(gBob&&s>0)?s:0;const double wv=(gWob>0&&s>0)?std::sin(6.2831853*.62*s):0.;return {float(.6*s+(gBob?.03*std::sin(6.2831853*1.3333333*a):0)+gWob*wv),float(gBob?.03*std::sin(6.2831853*2.6666667*a):0),float(-1.8*s+.8*gWob*std::sin(6.2831853*.62*s+1.)*(s>0?1.:0.))};};
    PossessionMotion pm;pm.interp=gInterp;if(getenv("DK2VR_LEAD")) pm.interpLead=float(atof(getenv("DK2VR_LEAD")));if(getenv("DK2VR_FADE")) pm.interpCorrectMs=float(atof(getenv("DK2VR_FADE")));if(getenv("DK2VR_VEL")) pm.interpVelocityMs=float(atof(getenv("DK2VR_VEL")));if(gLowLat) {pm.interpLead=.3f;pm.interpExtrapMax=1.f;pm.interpCorrectMs=60.f;pm.interpVelocityMs=40.f;pm.interpStick=true;}if(gInterp098) {pm.interpLead=1.f;pm.interpExtrapMax=.5f;pm.interpCorrectMs=0.f;pm.interpVelocityMs=0.f;pm.interpStick=false;}if(getenv("DK2VR_NOSTICK")) pm.interpStick=false;pm.smoothMs=smoothMs;pm.moveSmoothMs=moveSmooth;pm.swaySmoothMs=150.f;pm.heightSmoothMs=120.f;pm.clean=gClean;pm.cleanNotch=gNotch;pm.viewGain=float(gGain);pm.gain=gClean?pm.viewGain:pm.gain;Request req{};
    std::vector<std::shared_ptr<Package>> pk;std::vector<double> tt;
    double t=ts;uint64_t id=1;std::mt19937 gr(3);std::uniform_real_distribution<float> GU(0.f,1.f);
    while(t<ts+gLen) {pk.push_back(buildPackage(id++,t,stairYaw(t),walkOf(t)));tt.push_back(t);t+=frameMin+GU(gr)*frameSpan;}
    // ---- headset replay at 90 Hz; the VR thread adopts the newest package that has arrived (hand-off 4..14 ms after its content time) ----------------------------------
    struct Series {std::vector<float> yaw;std::vector<double> probeX,probeZ,probeY;};
    Series raw,oldA,newA;
    size_t adopted=0;float oldOmega=0;double oldEst=-1e9;float prevStair=0;double prevContent=0;bool oldValid=false;
    const Vec3 probe{1.5f,0.f,-3.f};
    double lagNewSum=0,lagRawSum=0;int lagN=0;double overshootNew=0,overshootRaw=0,overshootOld=0;
    const double stopMs=ts+1800;
    double nowFirst=ts+30;
    for(double now=nowFirst;now<ts+gLen-100;now+=1000./90.) {
        req.possessionTurn=float(cmdCum(now));req.possessionMoveX=0;req.possessionMoveY=(now>ts+250)?1.f:0.f;
        pm.noteCommand(req,now);
        bool switched=false;size_t newest=adopted;
        for(size_t i=adopted;i<pk.size();++i) if((gLateArrival?(i+1<tt.size()?tt[i+1]:tt[i]+30)+2+GU(gr)*6:tt[i]+4+GU(gr)*10)<=now) newest=i+1;
        if(newest>adopted&&newest<=pk.size()) {adopted=newest;switched=true;}
        if(adopted==0) continue;
        const auto& cur=pk[adopted-1];
        if(switched) pm.observe(cur,now);
        float py=0;Vec3 pan{};pm.extrapolate(*cur,req,now,py,pan);if(gClean) {dbgC.push_back(pm.chainPos.z);dbgS.push_back(pm.sPos.z);dbgC2.push_back(pm.chainPos.x);dbgS2.push_back(pm.sPos.x);}
        const float contentYaw=stairYaw(cur->contentMs);const Vec3 contentWalk=walkOf(cur->contentMs);
        // ---- witness: the 0.8.68 algorithm (rate of the last two game frames, EMA 0.5, carried on by now - contentMs while commands are active)
        if(switched) {
            if(oldValid&&cur->contentMs>prevContent+2) {const float om=(contentYaw-prevStair)/float((cur->contentMs-prevContent)*.001);if(std::fabs(om)<8.f) {oldOmega=oldValid&&now-oldEst<200?.5f*oldOmega+.5f*om:om;oldEst=now;}}
            oldValid=true;prevStair=contentYaw;prevContent=cur->contentMs;
        }
        const float oldPy=(now-oldEst<200)?float(oldOmega*std::clamp(now-cur->contentMs,0.,60.)*.001):0.f;
        // displayed yaw of the world in game coordinates
        raw.yaw.push_back(contentYaw);oldA.yaw.push_back(contentYaw+oldPy);newA.yaw.push_back(contentYaw+py);
        {   // where a static world point appears (record-local), for the walk
            const Pose idp{{0,0,0,1},{0,0,0}};const Similarity fix=dungeonCorrection(idp,idp,1.f,pan,py,{0,0,0});
            const Vec3 content=rotate(yawQuat(contentYaw),sub(probe,contentWalk));
            const Vec3 shown=apply(fix,content);raw.probeX.push_back(content.x);raw.probeZ.push_back(content.z);raw.probeY.push_back(content.y);newA.probeX.push_back(shown.x);newA.probeZ.push_back(shown.z);newA.probeY.push_back(shown.y);
        }
        const double ideal=cmdCum(now)*gain;
        if(now>ts+500&&now<stopMs-100) {lagNewSum+=(ideal-(contentYaw+py));lagRawSum+=(ideal-contentYaw);++lagN;}
        if(now>stopMs) {const double fin=cmdCum(ts+gLen)*gain;overshootNew=std::max(overshootNew,double(contentYaw+py)-fin);overshootRaw=std::max(overshootRaw,double(contentYaw)-fin);overshootOld=std::max(overshootOld,double(contentYaw+oldPy)-fin);}
    }
    auto stats=[&](const std::vector<float>& y,double fromMs,double toMs,double& rms,int& back) {
        rms=0;back=0;int n=0;const size_t i0=size_t((fromMs-nowFirst)/(1000./90.)),i1=std::min(y.size(),size_t((toMs-nowFirst)/(1000./90.)));
        for(size_t i=i0+2;i<i1;++i) {const double d2=y[i]-2.*y[i-1]+y[i-2];rms+=d2*d2;++n;if(y[i]-y[i-1]<-1e-4) ++back;}
        rms=n?std::sqrt(rms/n)*57.2958:0;
    };
    double rr,ro,rn;int br,bo,bn;
    stats(raw.yaw,ts+600,stopMs-100,rr,br);stats(oldA.yaw,ts+600,stopMs-100,ro,bo);stats(newA.yaw,ts+600,stopMs-100,rn,bn);
    // walk roughness: second difference of the on-screen position of a static point (mm)
    double wRaw=0,wNew=0;int wn=0;{const size_t i0=size_t((ts+700-nowFirst)/(1000./90.)),i1=size_t((ts+1700-nowFirst)/(1000./90.));
        for(size_t i=i0+2;i<i1&&i<raw.probeX.size();++i) {const double a=raw.probeX[i]-2*raw.probeX[i-1]+raw.probeX[i-2],b=raw.probeZ[i]-2*raw.probeZ[i-1]+raw.probeZ[i-2];wRaw+=a*a+b*b;
            const double c=newA.probeX[i]-2*newA.probeX[i-1]+newA.probeX[i-2],d=newA.probeZ[i]-2*newA.probeZ[i-1]+newA.probeZ[i-2];wNew+=c*c+d*d;++wn;}}
    wRaw=wn?1000.*std::sqrt(wRaw/wn):0;wNew=wn?1000.*std::sqrt(wNew/wn):0;
    if(gInterp) {   // 0.8.98: the view follows the game's own camera path, interpolated in time (no stick model)
        double maxNew=0,maxRaw=0;const size_t i0=size_t((ts+300-nowFirst)/(1000./90.));
        for(size_t i=i0+1;i<newA.yaw.size();++i) {maxNew=std::max(maxNew,double(std::fabs(newA.yaw[i]-newA.yaw[i-1])));maxRaw=std::max(maxRaw,double(std::fabs(raw.yaw[i]-raw.yaw[i-1])));}
        gLastDelay=pm.ipFrames?pm.ipDelaySum/pm.ipFrames:0.;gLastOvershoot=overshootNew*57.2958;gLastLag=lagN?lagNewSum/lagN*57.2958:0;gLastRough=rn;gLastBack=bn;
        printf("[INTERP%s%s%s %d draws, %.0f deg/s, game %.1f-%.1f ms, walk %s%s] turn roughness (2nd diff rms) game frames %.3f -> shown %.3f deg ; backward steps %d -> %d ; largest step %.2f deg (game %.2f, stick alone %.2f) ; walk roughness %.1f -> %.1f mm ; lag vs stick %.2f deg (game frames %.2f) ; delay %.1f ms ; overshoot at the stop %.2f deg\n",
            gInterp098?" 0.8.98":"",gLowLat?" low latency (option)":"",gLateArrival?" late arrival":"",objectCount,turnRate*57.2958,frameMin,frameMin+frameSpan,walkSteps?"33 ms ticks":"smooth",gAlong?" along the heading":"",rr,rn,br,bn,maxNew*57.2958,maxRaw*57.2958,turnRate*gain/90.*57.2958,wRaw,wNew,
            lagN?lagNewSum/lagN*57.2958:0,lagN?lagRawSum/lagN*57.2958:0,gLastDelay,gLastOvershoot);
        if(turnRate>0&&!gReverse&&gLowLat&&!getenv("DK2VR_NOEXPECT")) expect(overshootNew*57.2958<1.5,"interpolated view: no visible bounce when the stick is released (under 1.5 deg)");
        if(turnRate>0) {
            if(!gReverse) expect(bn==0,"interpolated view: the world never goes backwards while turning");
            expect(rn<rr*.35+.02,"interpolated view: turn much smoother than the game frames");
            expect(maxNew*57.2958<turnRate*gain/90.*57.2958*2.2+.5,"interpolated view: no jump of the shown yaw (a saccade)");
        }
        if(wRaw>1.) expect(wNew<wRaw*.5,"interpolated view: the walk is much smoother than the game frames");
        return 0;
    }
    // walk animation left in the picture: standard deviation of the sideways / vertical position of a static point while walking straight
    double sdRawX=0,sdNewX=0,sdRawY=0,sdNewY=0;{
        const size_t i0=size_t(((gClean?ts+2800:ts+800)-nowFirst)/(1000./90.)),i1=std::min(raw.probeX.size(),size_t((ts+gLen-100-nowFirst)/(1000./90.)));
        // deviation from the slow path: the signal minus its mean over 0.75 s (one sway period), so that the walk itself (and the lag of the followers
        // while the walk starts) does not count, only the animation
        auto sd=[&](const std::vector<double>& v)->double{
            const long W=34;double s=0;long n=0;
            for(long i=long(i0)+W;i<long(i1)-W;++i) {double m=0;for(long k=-W;k<=W;++k) m+=v[size_t(i+k)];m/=double(2*W+1);const double d=v[size_t(i)]-m;s+=d*d;++n;}
            return n?std::sqrt(s/double(n)):0.;};
        if(i1>i0+10) {sdRawX=sd(raw.probeX);sdNewX=sd(newA.probeX);sdRawY=sd(raw.probeY);sdNewY=sd(newA.probeY);}
    }
    if(gClean&&turnRate==0.0&&gCam) {   // steady lag of the shown walk behind the game's walk, measured on the probe's depth (the lead cancels the filter lag)
        const size_t a=size_t((ts+2800-nowFirst)/(1000./90.)),b=std::min(raw.probeZ.size(),size_t((ts+gLen-200-nowFirst)/(1000./90.)));double lag=0;int n=0;for(size_t i=a;i<b;++i) {lag+=newA.probeZ[i]-raw.probeZ[i];++n;}lag=n?lag/n:0;
        // smoothed raw (0.75 s mean) so that the gait does not count
        printf("   CLEAN camera: mean depth offset of a static point against the game frames at the steady walk %.1f cm (bin0 ok %d v %.3f %.3f, ffGate %.2f)\n",lag*100,(int)pm.bins[0].ok,pm.bins[0].vx,pm.bins[0].vz,pm.ffGate);expect(std::fabs(lag)<.4,"clean camera: the shown path stays close to the creature at steady walk (the speed model is learned: a few percent of error integrate to some decimetres)");}
    if(gBob) printf("   walk animation (sway 3 cm, bob 3 cm, ticks %.0f ms): sideways deviation %.1f -> %.1f mm, vertical deviation %.1f -> %.1f mm\n",gTick*1000,1000*sdRawX,1000*sdNewX,1000*sdRawY,1000*sdNewY);
    if(gWander>0&&gCam) {printf("   vertical wander %.2f m: chain height at the end %.3f m, largest vertical correction %.3f m\n",gWander,pm.chainY,pm.exYMax);expect(std::fabs(pm.chainY)<.02,"the height chain does not follow a vertical wander the game camera does not have");expect(pm.exYMax<.01,"no vertical correction from that wander");}
    if(gClean&&gBob&&!gWitness&&sdRawY>1e-6) {printf("   CLEAN camera: walk animation left: sideways %.0f%%, vertical %.0f%% of the game's\n",100*sdNewX/std::max(1e-9,sdRawX),100*sdNewY/std::max(1e-9,sdRawY));expect(sdNewY<sdRawY*.15,"clean camera: vertical bob almost gone");if(turnRate==0.0&&gWob==0&&!gNotch) expect(sdNewX<sdRawX*.45,"clean camera: sideways sway strongly reduced");}
    if(gClean&&gWob>0&&turnRate==0.0&&!gWitness) {   // 0.8.77: the 0.6 Hz sway of the game camera (report 102307) must not be shown, and not delayed either
        double yr=0,yn=0;{const size_t a=size_t((ts+2800-nowFirst)/(1000./90.)),b=std::min(raw.yaw.size(),size_t((ts+gLen-100-nowFirst)/(1000./90.)));double mr=0,mn=0;for(size_t i=a;i<b;++i) {mr+=raw.yaw[i];mn+=newA.yaw[i];}const double nn=double(b-a);mr/=nn;mn/=nn;for(size_t i=a;i<b;++i) {yr+=(raw.yaw[i]-mr)*(raw.yaw[i]-mr);yn+=(newA.yaw[i]-mn)*(newA.yaw[i]-mn);}yr=std::sqrt(yr/nn)*57.2958;yn=std::sqrt(yn/nn)*57.2958;}
        if(gClean) {auto dev=[&](const std::vector<double>& v){double a=0;int n=0;for(size_t i=200;i+100<v.size();++i) {double m=0;for(int k=-72;k<=72;++k) m+=v[i+k];m/=145;a+=(v[i]-m)*(v[i]-m);++n;}return std::sqrt(a/std::max(1,n));};gDevC=dev(dbgC2);gDevS=dev(dbgS2);printf("   position sway (deviation from its 1.6 s mean) of the chain %.1f cm (x) / %.1f cm (z) -> shown %.1f / %.1f cm\n",100*dev(dbgC2),100*dev(dbgC),100*dev(dbgS2),100*dev(dbgS));dbgC.clear();dbgS.clear();dbgC2.clear();dbgS2.clear();}
        printf("   CLEAN v2 vs a 0.6 Hz game sway (%.0f cm, %.1f deg): heading sway %.2f -> %.2f deg ; sideways deviation %.1f -> %.1f mm (resets %llu, max |t| %.2f m, offset %.2f target %.2f deg valid %d gain %.2f)\n",gWob*100,gWobYaw*57.2958,yr,yn,1000*sdRawX,1000*sdNewX,(unsigned long long)pm.resets,pm.exMoveMax,pm.offset*57.2958,pm.offTarget*57.2958,(int)pm.offsetValid,pm.gain);
        expect(yn<yr*.2,"clean camera v2: the heading sway of the game is almost gone");if(gNotch) expect(gDevS<gDevC*.4,"gait notch: the 0.6 Hz position sway of the chain is cut by 60% or more"); else expect(sdNewX<.015,"clean camera v2: the sideways sway of the game is almost gone (under 15 mm at 3 m)");}
    if(gBob&&!gWitness&&!gClean) {expect(sdNewY<sdRawY*.3,"vertical walk bob is strongly reduced");if(turnRate==0.0) expect(sdNewX<sdRawX*.55,"sideways walk sway is clearly reduced");}   // while turning the path itself curves on the screen: only the vertical bob is judged
    printf("[%d draws, %.0f deg/s, game %.1f-%.1f ms, walk %s, smooth %.0f ms] turn roughness rms (2nd diff): game frames %.3f deg | 0.8.68 %.3f deg | 0.8.69 %.3f deg ; backward steps of %d headset frames: %d | %d | %d ; lag vs the stick %.2f deg (game frames %.2f) ; overshoot at the stop %.2f deg (game %.2f, 0.8.68 %.2f) ; walk roughness %.2f mm -> %.2f mm ; fits %llu ok %llu guessed %llu resets %llu\n",
        objectCount,turnRate*57.2958,frameMin,frameMin+frameSpan,walkSteps?"33 ms ticks":"smooth",smoothMs,rr,ro,rn,(int)raw.yaw.size(),br,bo,bn,lagN?lagNewSum/lagN*57.2958:0,lagN?lagRawSum/lagN*57.2958:0,overshootNew*57.2958,overshootRaw*57.2958,overshootOld*57.2958,
        wRaw,wNew,(unsigned long long)pm.fits,(unsigned long long)pm.fitsOk,(unsigned long long)pm.guessed,(unsigned long long)pm.resets);
    {   // 0.8.71: the chain must still follow the game's real yaw (no wrong fit accepted by the extra hypotheses / the weak fit)
        const double chainErr=(pm.chainYaw-stairYaw(pk[adopted-1]->contentMs))*57.2958;
        printf("   chain yaw error at the end %.2f deg ; fits rescued by another hypothesis %llu, weak fits %llu, failures %llu (few draws %llu)\n",chainErr,(unsigned long long)pm.rescued,(unsigned long long)pm.weakOk,(unsigned long long)pm.guessed,(unsigned long long)pm.failFew);
        expect(std::fabs(chainErr)<(strict?.3:3.0),"the chained yaw stays on the game's yaw (no false fit)");
        // largest step of the shown yaw between two headset frames while the stick is used (a jump = a saccade); the stick itself moves the world
        // by at most turnRate x gain / 90 per frame (+ the game's own catching up)
        double maxNew=0,maxRaw=0;const size_t i0=size_t((ts+300-nowFirst)/(1000./90.));
        for(size_t i=i0+1;i<newA.yaw.size();++i) {maxNew=std::max(maxNew,double(std::fabs(newA.yaw[i]-newA.yaw[i-1])));maxRaw=std::max(maxRaw,double(std::fabs(raw.yaw[i]-raw.yaw[i-1])));}
        printf("   largest step of the shown yaw between two headset frames %.2f deg (game frames %.2f deg ; the stick alone gives %.2f deg) ; camera heading %s (scale %.3f) used on %llu game frames, translation-only fits %llu ; camera position %s (%s, scale %.3f) used on %llu frames, height %s (scale %.3f) on %llu\n",maxNew*57.2958,maxRaw*57.2958,turnRate*gain/90.*57.2958,pm.yawLearned?"learned":"not learned",pm.yawScale,(unsigned long long)pm.yawUsed,(unsigned long long)pm.knownRot,pm.camLearned?"learned":"not learned",pm.camModel?"mirrored":"direct",std::hypot(pm.camAr,pm.camAi),(unsigned long long)pm.camUsed,pm.zLearned?"learned":"not learned",pm.zK,(unsigned long long)pm.zUsed);
        if(strict||gReverse) expect(maxNew*57.2958<turnRate*gain/90.*57.2958*2.2+.5,"no jump of the shown yaw (a saccade) beyond what the stick itself asks");
    }
    if(strict) {
        expect(pm.fitsOk>pm.fits*7/10,"most game-frame pairs give a valid fit");
        expect(rr>1.0,"the game frames really are a stair at this rate (roughness > 1 degree)");
        expect(bo>=3,"the 0.8.68 algorithm reproduces the backward steps (the reported tremor)");
        expect(bn==0,"0.8.69 never sends the world backwards while turning");
        expect(rn<rr*.45,"turn roughness at least halved");
        expect(rn<ro*.45,"turn roughness far below the 0.8.68 algorithm");
        expect(overshootNew*57.2958<1.0,"no bounce when the stick is released");
        expect(std::fabs(lagNewSum/lagN)*57.2958<=std::fabs(lagRawSum/lagN)*57.2958+0.5,"the smoothed world does not lag the stick more than the game frames do");
    } else {
        if(!gReverse) expect(bn<=br/8+1,"even in hard conditions no backward steps");
        expect(rn<=rr+0.01,"even in hard conditions it is not rougher than the game frames");
    }
    // snap turn: never smoothed (the world must jump like the game does)
    {
        Request snapReq{};snapReq.comfortFlags=ComfortSnapTurn;float py=0;Vec3 pan{};pm.extrapolate(*pk.back(),snapReq,ts+gLen,py,pan);
        expect(py==0.f,"snap turn is never smoothed");
    }
    return 0;
}
// 0.8.78 closed loop 'the game follows the view' (model test: HeadingFollow + the error formula of PossessionMotion, game of non linear gain 1.96..3.19
// per stick segment, a 0.6 Hz sway of its camera heading, 3 frames of latency, error measured with 2 frames of delay, game ticks of 11.4 ms +- jitter)
static void headingLoopTest(const char* name,bool follow,double turnProfile,bool expectGood,float dead=-1,float lpMs=-1,float tau=-1) {
    HeadingFollow hf;hf.enabled=follow;if(dead>=0) hf.deadDeg=dead;if(lpMs>0) hf.lowpassMs=lpMs;if(tau>0) hf.tauMs=tau;const double vg=2.7,ppr=420.;std::mt19937 rng(7);std::uniform_real_distribution<double> jit(.009,.014);
    double t=0,c=0,injected=0,psi=0,off=0,offV=0;double extraSum=0;const int D=3;double psiHist[8]{},cHist[8]{};double extraQueue[8]{};
    double worst=0,sumAbs=0,swayPsi=0;int n=0;double lastViewErr=0,curGain=2.7;std::vector<double> errQ(4,0.);
    double truePsi=0;
    for(;t<30.;) {
        const double dt=jit(rng);t+=dt;
        // stick: turn rate profile (rad/s of commanded turn): sweeps, stops, reversals
        double rate=0;if(turnProfile>0) {const double ph=std::fmod(t,9.);rate=ph<2?turnProfile:(ph<4?0:(ph<6?-turnProfile*.6:(ph<7?0:turnProfile*.3)));}
        c+=rate*dt;
        const int seg=int(t*.8)%5;static const double gs[5]={2.0,3.2,2.5,3.0,2.3};curGain=gs[seg];
        // body turn goes through the carry (whole pixels) like PossessionInput; the game turns by gain x pixels
        const int dx=int(std::lround((c-injected)*ppr));injected+=double(dx)/ppr;
        // controller: err measured on the (delayed) game frames
        const double c_content=cHist[D-1],psi_obs=psiHist[D-1]+(t>1?swayPsi:0);
        const double offTarget=psi_obs-vg*c_content;const double e=off-offTarget;
        errQ.push_back(e);const double eMeas=errQ[errQ.size()-3];errQ.erase(errQ.begin());
        const double extraCmd=hf.step(follow&&t>.5,float(eMeas),float(vg),dt);const int ex=hf.pixels(extraCmd,float(ppr));
        extraQueue[0]=double(dx+ex)/ppr;
        // game: heading moves with the delayed pixels
        const double delayed=extraQueue[D-1];truePsi+=curGain*delayed;
        for(int i=7;i>0;--i) extraQueue[i]=extraQueue[i-1];
        swayPsi=2.8*3.14159265/180.*std::sin(6.2831853*.62*t);
        psi=truePsi;for(int i=7;i>0;--i) {psiHist[i]=psiHist[i-1];cHist[i]=cHist[i-1];}psiHist[0]=psi;cHist[0]=injected;
        // view: stick heading + slow offset follower (4 s) towards the game
        {const double w=1./4.;const double e0=off-offTarget,ex_=std::exp(-w*dt),cc=offV+w*e0;off=offTarget+(e0+cc*dt)*ex_;offV=(offV-w*cc*dt)*ex_;}
        if(t<.6) {off=psi_obs-vg*c_content;offV=0;}
        const double view=vg*c+off,gameNow=psi+(t>1?swayPsi:0)*0;   // the sway is the camera only: the creature's own heading is truePsi
        const double d=(view-gameNow)*180./3.14159265;lastViewErr=d;
        if(t>4) {worst=std::max(worst,std::fabs(d));sumAbs+=std::fabs(d);++n;}
    }
    printf("   heading loop %-34s follow %s: view-game error avg %.1f deg, worst %.1f deg, last %.1f deg, extra turn sent %.0f deg\n",name,follow?"ON ":"off",n?sumAbs/n:0.,worst,lastViewErr,hf.sentTotal*57.29578*2.7);
    if(expectGood) {const double k=turnProfile>1.8?1.45:1.;expect(sumAbs/std::max(1,n)<7.*k,"heading loop: mean view-game error small (7 deg, 10 at the fastest stick)");expect(worst<30.*k,"heading loop: worst view-game error bounded (30 deg, 44 at the fastest stick)");}
    if(!follow&&turnProfile>0) expect(sumAbs/std::max(1,n)>15.,"heading loop witness: without the loop the game drifts away from the view");
}
static void notchUnit() {   // 0.8.79: the gait notch alone, on a synthetic position (steady walk 1.8 m/s + sway 0.2 m at 0.62 Hz + 30 Hz steps)
    PossessionMotion pm;pm.clean=true;pm.cleanNotch=true;pm.predictMove=true;pm.valid=true;pm.gaitAuto=false;pm.lastMs=0;
    double now=0,maxRaw=0,maxOut=0,lagSum=0;int n=0;double rawS=0,outS=0;
    for(int i=0;i<1400;++i) {
        now+=11.1;const double t=now*.001,stepT=std::floor(t*30.)/30.;
        pm.chainPos={float(.2*std::sin(6.2831853*.62*stepT)),0,float(-1.8*stepT)};
        pm.advance(now);
        if(t>4) {const double rx=.2*std::sin(6.2831853*.62*t),rz=-1.8*t;rawS+=rx*rx;outS+=double(pm.sPos.x)*pm.sPos.x;lagSum+=(rz-pm.sPos.z)/1.8;++n;}
    }
    printf("   gait notch alone: sideways sway %.1f -> %.1f cm rms, delay of the walk %.0f ms\n",100*std::sqrt(rawS/n),100*std::sqrt(outS/n),1000*lagSum/n);
    expect(std::sqrt(outS/n)<.25*std::sqrt(rawS/n),"gait notch: the sway at the walking frequency is cut by 75% or more");
    expect(std::fabs(lagSum/n)<.2,"gait notch: the walk is delayed by less than 200 ms");
}

// 0.8.84: records of a cube face are never used by the motion interpolation / fit (normal view only)
static void viewRecordsTest() {
    Package q;q.vertices.resize(1000);DrawRecord r{};r.stride=12;r.count=10;r.vertices=0;
    expect(MotionInterp::usable(q,r),"a normal-view record is usable for the motion fit");
    for(int v=1;v<=6;++v) {r.view=uint8_t(v);expect(!MotionInterp::usable(q,r),"a cube-face record is not used by the motion fit");}
    r.view=0;r.overlay=true;expect(!MotionInterp::usable(q,r),"0.8.99: an attack effect drawn over the view (it follows the game camera) is not used by the motion fit");r.overlay=false;
    printf("   cube-face records kept out of the motion fit: OK\n");
}

int main() {
    notchUnit();
#ifdef SWEEP
    for(float sm:{40.f,70.f,100.f,150.f,250.f}) scenario(320,1.57,7.5,9.5,false,false,sm);
    for(float sm:{40.f,70.f,100.f,150.f,250.f}) scenario(320,3.14,7.5,9.5,false,false,sm);
    return 0;
#endif
    // 0.8.98 interpolated view (the default of the mod): the conditions of the real game in possession (report 131654: 20-45 ms per game frame)
    gInterp=true;
    scenario(320,1.57,20.,25.,false,false,100.f);  // 90 deg/s, 20-45 ms game frames
    scenario(320,2.09,20.,25.,false,true,100.f);   // 120 deg/s, walk in 33 ms ticks
    gAlong=true;scenario(320,1.57,20.,25.,false,true,100.f);gAlong=false;   // walking along the heading while turning
    scenario(320,0.0,20.,25.,false,true,100.f);    // walking straight
    scenario(320,1.57,7.5,9.5,false,false,100.f);  // fast game frames
    gReverse=true;scenario(320,2.09,20.,25.,false,false,100.f);gReverse=false;   // fast stick reversals
    gSparseFrom=100+700;gSparseKeep=3;scenario(120,1.57,20.,25.,false,true,100.f);gSparseFrom=1e9;   // the camera looks at almost nothing
    // 0.8.99: lower latency. Real arrival (one game frame interval after the content), 0.8.98 settings as the witness
    gLateArrival=true;
    for(int k=0;k<3;++k) {
        const double rate=k==2?2.09:1.57,fmin=k==1?30.:20.,fspan=k==1?25.:25.;const bool steps=k!=0;
        gInterp098=true;scenario(320,rate,fmin,fspan,false,steps,100.f);const double oldDelay=gLastDelay,oldLag=gLastLag;gInterp098=false;
        scenario(320,rate,fmin,fspan,false,steps,100.f);expect(std::fabs(gLastDelay-oldDelay)<.01,"0.8.99.1: the default view is the 0.8.98 one");
        gLowLat=true;scenario(320,rate,fmin,fspan,false,steps,100.f);gLowLat=false;
        printf("   0.8.99 vs 0.8.98: delay %.1f -> %.1f ms, lag behind the stick %.2f -> %.2f deg\n",oldDelay,gLastDelay,oldLag,gLastLag);
        expect(gLastDelay<oldDelay-.5*(fmin+.5*fspan),"0.8.99: the shown view is at least half a game frame interval more recent than in 0.8.98");
        expect(std::fabs(gLastLag)<std::fabs(oldLag),"0.8.99: the view lags the stick less than in 0.8.98");
    }
    gReverse=true;scenario(320,2.09,20.,25.,false,false,100.f);gReverse=false;   // stick reversals with the real arrival
    gLateArrival=false;
    gInterp=false;
    scenario(320,1.57,7.5,9.5,true,false,100.f);   // 90 deg/s, 58-115 game fps (the 000051 report conditions), smooth walk
    scenario(320,2.09,7.5,9.5,true,false,100.f);   // 120 deg/s commanded = the fastest stick (PossessionTurnSpeed)
    scenario(320,1.57,7.5,9.5,false,true,100.f);   // walk in 33 ms logic ticks
    scenario(120,1.57,12.,18.,false,false,100.f);  // sparse world, 33-60 game fps
    scenario(60,2.09,7.5,9.5,false,false,100.f);   // very sparse world, fastest stick
    scenario(320,0.0,7.5,9.5,false,true,100.f);   // walking straight only, logic ticks of 33 ms
    scenario(320,0.0,7.5,9.5,false,false,100.f);  // walking straight only, smooth
    gBob=true;
    scenario(320,0.0,7.5,9.5,false,true,100.f);   // walking with the creature animation, 33 ms logic ticks
    gTick=0.04;scenario(320,0.0,7.5,9.5,false,true,100.f);
    gTick=0.05;scenario(320,0.0,7.5,9.5,false,true,100.f);
    gBob=false;gTick=0.033;
    // 0.8.76 CLEAN camera: the creature's animation (bob / sway) must be (almost) gone, without lag behind the creature
    gSpan=60.f;   // long straight walks: the world must not run out of objects
    gClean=true;gLen=5200;gBob=true;gTick=0.033;scenario(900,0.0,7.5,9.5,false,true,100.f);gTick=0.05;scenario(900,0.0,7.5,9.5,false,true,100.f);gTick=0.033;
    gSparseFrom=100+700;gSparseKeep=3;scenario(120,0.0,7.5,9.5,false,true,100.f);gLen=2400;gAlong=true;scenario(120,1.0,7.5,9.5,false,true,100.f);scenario(320,1.57,7.5,9.5,false,true,100.f);gAlong=false;gSparseFrom=1e9;
    gLen=2400;gBob=false;scenario(320,1.57,7.5,9.5,false,true,100.f);scenario(320,2.09,7.5,9.5,true,false,100.f);gLen=5200;
    // 0.8.77: the game camera sways at 0.6 Hz while walking (20 cm, 2.8 deg): the shown view must not carry it
    gBob=true;gWob=.2;gWobYaw=2.8/57.2958;scenario(900,0.0,7.5,9.5,false,true,100.f);gTick=0.05;scenario(900,0.0,7.5,9.5,false,true,100.f);gTick=0.033;
    // 0.8.79: the position through the GAIT NOTCH (default of the mod): same world, sway of 0.62 Hz, steady walk, turns, steps in 33/50 ms ticks
    gNotch=true;gBob=true;gWob=.2;gWobYaw=2.8/57.2958;gLen=5200;scenario(900,0.0,7.5,9.5,false,true,100.f);gTick=0.05;scenario(900,0.0,7.5,9.5,false,true,100.f);gTick=0.033;
    gWob=0;gWobYaw=0;gLen=2400;gAlong=true;scenario(320,1.0,7.5,9.5,false,true,100.f);scenario(320,1.57,7.5,9.5,false,true,100.f);gAlong=false;gBob=false;scenario(320,1.57,7.5,9.5,false,true,100.f);scenario(320,2.09,7.5,9.5,true,false,100.f);gNotch=false;
    gWob=0;gWobYaw=0;gBob=false;gClean=false;gLen=2400;gSpan=18.f;
    // 0.8.73: report 011103: the walk animation (bob / sway) in a view of almost nothing, where the picture cannot be matched: the game's camera position is used
    gBob=true;gTick=0.033;gSparseFrom=100+700;gSparseKeep=3;
    scenario(120,0.0,7.5,9.5,false,true,100.f);                          // direct axes
    gCamMirror=true;scenario(120,0.0,7.5,9.5,false,true,100.f);gCamMirror=false;   // mirrored axes
    scenario(120,1.0,7.5,9.5,false,true,100.f);                          // walking while turning
    gWitness=true;gCam=false;scenario(120,0.0,7.5,9.5,false,true,100.f);gCam=true;gWitness=false;   // the same without the camera position (witness 0.8.72)
    gSparseFrom=1e9;gBob=false;
    // 0.8.74: report 013045: the picture fits drifted vertically by 1.2 m while the game camera height was constant: the height chain must stay flat
    gWander=.25;scenario(120,0.0,7.5,9.5,false,true,100.f);gSparseFrom=100+700;gSparseKeep=3;scenario(120,1.0,7.5,9.5,false,true,100.f);gSparseFrom=1e9;gWander=0;
    // 0.8.72: report 005539: the camera looks at almost nothing for seconds (8-76 draws): the picture cannot be matched, the game's camera heading is used
    gSparseFrom=100+700;gSparseKeep=4;scenario(120,1.57,7.5,9.5,false,true,100.f);gSparseKeep=2;scenario(120,2.09,12.,18.,false,true,100.f);gSparseFrom=1e9;
    gYaw=false;gSparseFrom=100+700;gSparseKeep=4;scenario(120,1.57,7.5,9.5,false,true,100.f);gSparseFrom=1e9;gYaw=true;   // the same without the camera heading (witness 0.8.71)
    // 0.8.72: fast reversals of the stick (report 005539: jumps of 13 to 43 degrees at the reversals)
    gReverse=true;scenario(320,2.09,7.5,9.5,false,false,100.f);scenario(120,2.09,12.,18.,false,true,100.f);scenario(45,2.09,12.,18.,false,true,100.f);gReverse=false;
    // 0.8.71: the conditions of report 003917 (a good third of the game frames could not be fitted): few draws, many creatures, wide world
    gMover=.35f;gSpan=30.f;
    scenario(45,1.57,12.,18.,false,true,100.f);
    scenario(30,2.09,12.,18.,false,true,100.f);
    scenario(70,1.57,7.5,9.5,false,true,100.f);
    gMover=.15f;gSpan=18.f;
    gGain=1.2;scenario(320,1.57,7.5,9.5,false,false,100.f);   // the game answers with a gain far from the starting value: it is learned
    gGain=2.5;
#ifdef LOOPSWEEP
    for(float lp:{100.f,200.f,400.f}) for(float tau:{120.f,200.f,350.f}) for(float dd:{1.5f,2.5f}) {printf("lp %.0f tau %.0f dead %.1f ",lp,tau,dd);headingLoopTest("sweep80",true,1.4,false,dd,lp,tau);}
    return 0;
#endif
    headingLoopTest("turn 80 deg/s sweeps",false,1.4,false);
    headingLoopTest("turn 80 deg/s sweeps",true,1.4,true);
    headingLoopTest("turn 120 deg/s sweeps",true,2.09,true);
    headingLoopTest("no turning (walk only)",true,0.0,true);
    {   // 0.8.78: walk of a stick direction not learned yet, from the learned ones
        PossessionMotion q;q.bins[0].ok=true;q.bins[0].vx=0;q.bins[0].vz=-2.f;q.bins[2].ok=true;q.bins[2].vx=1.5;q.bins[2].vz=0;double x=0,z=0;
        expect(q.fallbackVelocity(4,x,z)&&std::fabs(x)<1e-9&&std::fabs(z-2.)<1e-9,"fallback: the opposite direction is the negated one");
        expect(q.fallbackVelocity(1,x,z)&&std::fabs(x-1.5*.70710678)<1e-6&&std::fabs(z+2.*.70710678)<1e-6,"fallback: a diagonal is the sum of its neighbours / sqrt 2");
        {PossessionMotion q1;q1.bins[0].ok=true;q1.bins[0].vz=-2.f;expect(!q1.fallbackVelocity(2,x,z)&&!q1.fallbackVelocity(1,x,z),"fallback: nothing to derive from -> not used");}
        printf("   fallback of not yet learned directions: OK\n");
    }
    viewRecordsTest();
    printf("PossessionMotionTests OK\n");
    return 0;
}
