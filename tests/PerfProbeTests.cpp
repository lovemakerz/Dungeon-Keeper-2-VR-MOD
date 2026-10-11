// DK2VR 0.9.25 - made by LoVeMaKeRz. Offline checks of the PERF probe logic (src/plugin/PerfProbe.hpp).
#include "../src/plugin/PerfProbe.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
using namespace perfab;
static std::vector<std::string> lines;
int main() {
    // histogram: percentiles at 0.5 ms resolution, max exact, stutter counters
    Hist h;for(int i=0;i<90;++i) h.add(11.1);for(int i=0;i<9;++i) h.add(40.);h.add(300.);
    assert(h.n==100&&h.worst==300.&&h.over34==10&&h.over50==1);
    assert(h.pct(.5)==11.5&&h.pct(.95)==40.5&&h.pct(1.)==300.);
    h.add(-1.);h.add(0./0.);assert(h.n==100);   // invalid samples ignored
    Hist g;g.add(20.);g.merge(h);assert(g.n==101&&g.worst==300.);g.clear();assert(g.n==0&&g.pct(.5)==0.);
    // schedule: A, B, C, A ... every period; negative or invalid -> A
    assert(configAt(0,10000)==0&&configAt(9999,10000)==0&&configAt(10000,10000)==1&&configAt(25000,10000)==2&&configAt(30000,10000)==0&&configAt(95000,10000)==0);
    assert(configAt(-5,10000)==0&&configAt(5,0)==0);
    // VR totals: atomics taken and reset
    Shared& S=shared();S.vr[1][2].add(90,1000000,360000);S.vr[1][2].gpu(2000);S.vr[1][2].gpu(4000);
    GameTotals t;t.collectVr();
    assert(t.vr[1][2].images==90&&t.vr[1][2].wallUs==1000000);
    assert(t.vr[1][2].fps()==90.&&t.vr[1][2].waitMs()==4.&&t.vr[1][2].gpuMs()==3.);
    assert(S.vr[1][2].images.load()==0);
    // bilan: only scenes / configurations with enough time, names of the A/B configurations
    for(int i=0;i<300;++i) t.hist[1][2].add(30.);   // 9 s of table at 33 fps in configuration C
    for(int i=0;i<10;++i) t.hist[0][0].add(30.);    // 0.3 s of menu: not shown
    const int n=t.bilan([](const char* f,auto... a){char b[1200];snprintf(b,sizeof(b),f,a...);lines.push_back(b);},true);
    assert(n==1&&lines.size()==1);
    assert(lines[0].find("BILAN PERF plateau")==0&&lines[0].find("C (VR 0, jeu 0 : priorites de 0.9.24)")!=std::string::npos&&lines[0].find("jeu 33.3 i/s")!=std::string::npos&&lines[0].find("casque 90.0 i/s")!=std::string::npos);
    assert(lines[0].find("menu")==std::string::npos);
    puts("PASS: PERF probe histogram/percentiles, A/B schedule, VR totals hand-over and BILAN lines (no headset/FPS claim).");
}
