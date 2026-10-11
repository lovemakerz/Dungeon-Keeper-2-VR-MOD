// DK2VR 0.8.78 - closed loop "the game follows the view".
// The VR view of a possessed creature is driven ONLY by the sticks (heading = viewGain x commanded turn + a very slow offset). The game turns
// by its own, non linear law (gain 1.96..3.19 per stick segment, report 104505): its heading drifts away from the view by 7..30 degrees.
// This controller turns the game (extra mouse pixels, never part of the commanded turn) until its heading agrees with the view again.
//   err  = view heading - game heading (radians, game-heading units, measured on the game frames by PossessionMotion)
//   the error is low-passed (the 0.6 Hz sway of the game camera, +-2.8 deg, must not be chased), a dead band is removed, the rest is turned
//   away with a time constant, rate limited. Output = commanded-turn radians (divide by the game gain), sub-pixel carry kept by the caller.
#pragma once
#include <atomic>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace dk2vr {
struct HeadingFollowShared {   // written by the VR thread (PossessionMotion), read by the game thread (PossessionInput)
    inline static std::atomic<float> err{0.f};
    inline static std::atomic<float> gain{2.7f};
    inline static std::atomic<uint64_t> stampMs{0};
    inline static std::atomic<bool> valid{false};
    // configuration (INI), written once by the VR thread when it starts, copied by the game thread at every tick
    inline static std::atomic<bool> cfgEnabled{true};inline static std::atomic<float> cfgDead{2.f},cfgLowpass{150.f},cfgTau{300.f},cfgMax{120.f};
    static uint64_t nowMs() {return uint64_t(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());}
    static void publish(bool ok,double errRad,double gameGain) {const uint64_t nowMs_=nowMs();
        err.store(float(errRad),std::memory_order_relaxed);gain.store(float(gameGain),std::memory_order_relaxed);
        stampMs.store(nowMs_,std::memory_order_relaxed);valid.store(ok,std::memory_order_release);
    }
    static bool read(float& e,float& g) {
        if(!valid.load(std::memory_order_acquire)) return false;
        if(nowMs()-stampMs.load(std::memory_order_relaxed)>150) return false;   // the VR thread stopped publishing
        e=err.load(std::memory_order_relaxed);g=gain.load(std::memory_order_relaxed);return std::isfinite(e)&&std::isfinite(g)&&g>.3f;
    }
};
struct HeadingFollow {
    bool enabled=true;
    float deadDeg=2.f,lowpassMs=150.f,tauMs=300.f,maxDegPerS=120.f;
    float lp=0.f;       // low-passed error (rad)
    double carry=0.;    // commanded-turn radians not yet sent as whole pixels
    double sentTotal=0.,rateNow=0.;
    void loadShared() {enabled=HeadingFollowShared::cfgEnabled.load();deadDeg=HeadingFollowShared::cfgDead.load();lowpassMs=HeadingFollowShared::cfgLowpass.load();tauMs=HeadingFollowShared::cfgTau.load();maxDegPerS=HeadingFollowShared::cfgMax.load();}
    void clear() {lp=0.f;carry=0.;rateNow=0.;}
    // one game tick. Returns the commanded-turn radians to ADD to the mouse this tick (0 when idle).
    double step(bool ok,float err,float gameGain,double dtSec) {
        dtSec=std::clamp(dtSec,0.,.05);
        if(!enabled||!ok||dtSec<=0.) {lp*=.9f;rateNow=0.;return 0.;}
        lp+=(err-lp)*float(1.-std::exp(-dtSec*1000./std::max(50.f,lowpassMs)));
        const float dead=deadDeg*3.14159265f/180.f,a=std::fabs(lp);
        const float s=a>dead?(lp>0?a-dead:-(a-dead)):0.f;
        const double mx=maxDegPerS*3.14159265/180.;
        rateNow=std::clamp(double(s)*1000./std::max(50.f,tauMs),-mx,mx);   // game-heading rad/s
        return rateNow*dtSec/std::max(.3,double(gameGain));
    }
    // whole pixels of a commanded turn amount, the remainder is kept
    int pixels(double cmdRad,float pixelsPerRadian) {
        carry+=cmdRad;const int dx=int(std::lround(carry*pixelsPerRadian));
        carry-=double(dx)/pixelsPerRadian;sentTotal+=double(dx)/pixelsPerRadian;return dx;
    }
};
}
