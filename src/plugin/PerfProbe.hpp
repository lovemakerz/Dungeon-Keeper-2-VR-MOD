#pragma once
// DK2VR 0.9.25 - made by LoVeMaKeRz
// PERF probe + automatic A/B test of the GPU scheduling priorities (measurement build).
// Pure logic (no Direct3D): schedule of the configurations, frame-time histogram, per scene / per configuration totals.
// The game thread owns the schedule and the game-frame totals; the VR thread only adds its own counters (atomics).
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <algorithm>

namespace perfab {

constexpr int Modes=4;     // 0 main menu / cinematics room, 1 table (dungeon), 2 possession, 3 other (loading, flat screen)
constexpr int Configs=3;
struct Config {int vr,game;const char* name;};
// A = expected best (VR thread ahead of the game on the GPU), B = game as high as the VR thread, C = the 0.9.24 priorities
// (the other 0.9.25 changes - host copy without wait, host priorities, VR wait that sleeps - stay on in all three).
inline const Config configs[Configs]={{7,0,"A (VR +7, jeu 0)"},{7,7,"B (VR +7, jeu +7)"},{0,0,"C (VR 0, jeu 0 : priorites de 0.9.24)"}};
inline const char* const modeNames[Modes]={"menu","plateau","possession","autre"};

// Frame-time histogram: 0.5 ms bins up to 250 ms (+ overflow), exact max.
struct Hist {
    static constexpr int Bins=500;
    uint32_t bin[Bins+1]{};uint64_t n=0;double sum=0,worst=0;uint64_t over34=0,over50=0;
    void add(double ms) {
        if(!(ms>=0)||!std::isfinite(ms)) return;
        int b=int(ms*2.);if(b>Bins) b=Bins;++bin[b];++n;sum+=ms;worst=std::max(worst,ms);
        if(ms>34.) {++over34;}
        if(ms>50.) {++over50;}
    }
    // smallest bin upper edge with at least p of the samples at or below it
    double pct(double p) const {
        if(!n) return 0.;
        const double want=p*double(n);double acc=0;
        for(int b=0;b<=Bins;++b) {acc+=bin[b];if(acc>=want) return b==Bins?worst:(b+1)*.5;}
        return worst;
    }
    double mean() const {return n?sum/double(n):0.;}
    void merge(const Hist& o) {for(int b=0;b<=Bins;++b) bin[b]+=o.bin[b];n+=o.n;sum+=o.sum;worst=std::max(worst,o.worst);over34+=o.over34;over50+=o.over50;}
    void clear() {*this=Hist{};}
};

// Which configuration is active at a given time (ms since the start of the cycle).
inline int configAt(double sinceStartMs,double periodMs) {
    if(!(periodMs>0)||!(sinceStartMs>=0)) return 0;
    return int(std::fmod(std::floor(sinceStartMs/periodMs),double(Configs)));
}

// VR-thread side (atomics: written by the VR thread, read and reset by the game thread).
struct VrTotals {
    std::atomic<uint64_t> images{0},wallUs{0},waitUs{0},gpuUs{0},gpuN{0};
    void add(uint64_t img,uint64_t wall,uint64_t wait) {images+=img;wallUs+=wall;waitUs+=wait;}
    void gpu(uint64_t us) {gpuUs+=us;++gpuN;}
};
struct VrSnapshot {
    uint64_t images=0,wallUs=0,waitUs=0,gpuUs=0,gpuN=0;
    double fps() const {return wallUs?1e6*double(images)/double(wallUs):0.;}
    double waitMs() const {return images?double(waitUs)/1000./double(images):0.;}
    double gpuMs() const {return gpuN?double(gpuUs)/1000./double(gpuN):0.;}
    void merge(const VrSnapshot& o) {images+=o.images;wallUs+=o.wallUs;waitUs+=o.waitUs;gpuUs+=o.gpuUs;gpuN+=o.gpuN;}
};
inline VrSnapshot take(VrTotals& t) {
    VrSnapshot s;s.images=t.images.exchange(0);s.wallUs=t.wallUs.exchange(0);s.waitUs=t.waitUs.exchange(0);s.gpuUs=t.gpuUs.exchange(0);s.gpuN=t.gpuN.exchange(0);return s;
}

struct Shared {
    std::atomic<int> mode{3},config{0},fixedVr{7},fixedGame{0};std::atomic<bool> settling{true},enabled{false};
    VrTotals vr[Modes][Configs];   // filled by the VR thread while not settling
    VrTotals vrWindow;             // every VR image, for the 5 s line
};
inline Shared& shared() {static Shared s;return s;}

// Game-thread totals.
struct GameTotals {
    Hist hist[Modes][Configs];
    VrSnapshot vr[Modes][Configs];
    Hist window;int windowMode=3;
    uint64_t windowModeFrames[Modes]{};
    void collectVr() {for(int m=0;m<Modes;++m) for(int c=0;c<Configs;++c) vr[m][c].merge(take(shared().vr[m][c]));}
    // Table of the measured configurations, one line per scene with enough data. Returns the number of lines written.
    template<class Log> int bilan(Log log,bool ab,double minSeconds=8.) const {
        int lines=0;
        for(int m=0;m<Modes-1;++m) {
            char line[900];int k=snprintf(line,sizeof(line),"BILAN PERF %s (0.9.25%s):",modeNames[m],ab?", test A/B automatique":"");bool any=false;
            for(int c=0;c<Configs;++c) {
                const Hist& h=hist[m][c];if(h.sum<minSeconds*1000.) continue;any=true;
                const VrSnapshot& v=vr[m][c];
                k+=snprintf(line+k,sizeof(line)-k," | %s : jeu %.1f i/s (median %.1f ms, 95%% %.1f ms, >50 ms %.1f %%), casque %.1f i/s (attente GPU VR %.2f ms, GPU VR %.2f ms), %.0f s",
                    ab?configs[c].name:"reglage fixe",h.n*1000./h.sum,h.pct(.5),h.pct(.95),100.*double(h.over50)/double(std::max<uint64_t>(1,h.n)),v.fps(),v.waitMs(),v.gpuMs(),h.sum/1000.);
                if(k>=int(sizeof(line))-1) break;
            }
            if(any) {log("%s",line);++lines;}
        }
        return lines;
    }
};

}  // namespace perfab
