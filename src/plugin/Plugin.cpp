// DK2VR 0.7.10 - stone strip of the bottom panel is interface again; bar key only removes pure black; minimap = exact opaque frame. 0.7.9 - minimap cut-out (medallion + corner buttons). 0.7.8 - fixed cut-out mask for the bars (no more per-draw guessing). 0.7.7 - interface 3D pieces must be square or tall and small (flat rock strips are dungeon). 0.7.6 - no hand window while the objectives page is shown. 0.7.5 - interface 3D pieces only in the lower part (dungeon specks at the bottom edge). 0.7.4 - focal floor .20 and finer smoothing: the table really fits between the bars (no dungeon drawn under them). 0.7.3 - only small 3D pieces may be interface (dungeon pieces in the transparent bars). 0.7.2 - hidden dungeon draws write no depth (hid the '0' button); bar test limited to small pieces (large dungeon draws went to the HUD). 0.7.1 - transparent bars again; rhw rules restored and native depth clear removed ('i' back); probe of the 'o'/'i' button area. 0.7.0.1 - hand window erases the bars entirely again (0.7.0: the pickaxe model box kept whole bars on the table). 0.7.0 - rebuilt from the 0.6.9.2 reference with only the validated/expected changes: sharp textures (LOD), render memory pool, crash log, pause-driven options panel, messages above the bars, fold button blocked, bars from the game image over stone (dungeon hidden in the game image, depth cleared before the interface), hand window keeps the held model, auto recenter, JPG captures, minimap/pedestal; draw-classification experiments removed. 0.6.9.17 - fix 0.6.9.16 regression (empty table: colour writes recorded off); no stone behind the message panel. 0.6.9.16 - dungeon hidden in the game image (colour writes off) so the bars come clean from it; hand window from the held model on; stone backing behind the panels. 0.6.9.15 - VR bars and minimap cut from the game final image (no draw classification); engine view kept between the bars. 0.6.9.14 - interface replayed with its own depth buffer (dungeon depth hid interface pieces); fold button rect measured; larger hand window. 0.6.9.13 - bar elements straddling two bars kept in the HUD; box text stays in the box (only pieces after the held model go to the hand); taller hand window. 0.6.9.12 - message box test limited to real boxes (0.6.9.11 regression: blank bars). 0.6.9.11 - hovered bar elements animated in 3D kept in the HUD; message panel = box only, box never in the interface texture (held creature back); panel fold button disabled. 0.6.9.10 - depth test restored for interface draws (0.6.9.9 regression: empty bars); message boxes detected under the cursor and erased from the hand; bottom-panel 3D pieces to the HUD. 0.6.9.9 - messages above all bars and steady when pointed, kept off the hand; interface draws replayed without depth test. 0.6.9.8 - render memory pool 32 -> 127 blocks (exit(-1) crash); game messages/objectives above the bars; auto recenter 5 s after launch. 0.6.9.7 - options panel follows the game pause (0x527270); game exit logged; 3D interface cards out of the table; minimap crop; auto recenter. 0.6.9.6 - texture detail fix with exact addresses; options screen closes again; UI-over-bars test tolerant; pedestal column under the plate. 0.6.9.5 - texture detail follows the focal factor (21 LOD compares redirected); F12 removed; pedestal plate restored. 0.6.9.4 - crash log (exception handler); interface over the bars kept out of the table while the table fits between them; F12 toggles whole-table picking. 0.6.9.3.1 - host linked to openxr_loader.dll (was libopenxr_loader.dll: start failure). 0.6.9.3 - options screen whole (stats bar included) and steady under the laser; pedestal plate removed; adaptive focal factor (sharper textures); 3D interface pieces over the bottom bars kept out of the table; JPG captures; system OpenXR runtime. 0.6.9.2 - whole-table picking fixed (view setup 0x575A00 hooked: tile selection uses the same focal); options screen stacked on the timer bar under the health bar; VDXR runtime preferred. 0.6.9.1 - picking off, options screen stacked. 0.6.9 - whole table pickable (game camera focal widened while the table is shown); wall screen from middle draws only; original room textures; glow hands; smaller pedestal button. 0.6.8 - wall screen detection ignores held model/cursor area; smaller held creature; rusted pedestal with side button, drawn before the tray; earthen room. 0.6.7.1 - wall screen only when a menu/options screen is open; held window only with the laser on the table, scaled like the hand. 0.6.7 - laser picks the dungeon triangles; hand size fixed; held window wider; wall = whole game area, bars stacked over the panel. 0.6.6.1 - interface-texture filter off (game texture cache blocks are shared by UI and world). 0.6.6 - laser on the table drives the game cursor (inverse projection), trigger/A = clicks. 0.6.5.3 - held things = HUD window around the cursor (panels erased); 3D interface cards out of the dungeon. 0.6.5.2 - held things only near the cursor and outside panels the cursor is not on. 0.6.5.1 - top-down: engine rectangle = projected table (exact); game screen panel off (F3); texture upload callers measured. 0.5.15 - far distance x3 with the top-down camera (cross-shaped table when zoomed out). 0.5.14 - no near-edge shift with the top-down camera, wider side margin. 0.5.13 - steep (top-down) game camera while the table is shown. 0.5.12 - table near edge follows the nearest geometry the game draws. 0.5.11 - texture cache patch address fixed, table near edge kept in the engine's reach. 0.5.10 - real texture cache (SurfHashList2 512 -> 1536), pan prediction limited. 0.5.9 - larger player-side / side margins (holes while dragging). 0.5.8 - table depth shared between far and player sides (player-side strip), far from the screen far edge. 0.5.7 - vertical enlargement on both screen edges again (player side filled in 0.5.2), far limited to the corner ratio. 0.5.6 - texture cache byte+hook+real count, far distance for the far corners. 0.5.5 - texture cache 256 blocks (constructor call), far edge fixed / sides+player side only. 0.5.4.1 - fix: lockBuf/unlockBuf wrappers used wrong stack sizes (crash at start). 0.5.4 - near corners (+30 % player side), surface-lock measurement. 0.5.3 - game texture shadow (no GPU wait on texture locks), no fresh-image wait, wider corner margin. 0.5.2 - table view exactly as validated in 0.4.7 (TileCover removed) + VR state cache, full resolution, ceiling clip. Was 0.5.0 (base 0.4.7) - VR state cache, full headset resolution, engine tiles = table (corners), ceiling clip. 0.4.7 - table shows the whole PC screen view (TableView), anisotropic x16 VR textures, table 0.50 m below the eyes.
// DK2VR 0.4.4 - wooden tray bottom under the dungeon (instead of the dark plate).
// DK2VR 0.4.3 - room prepared from the game start at normal priority, disk cache (Cache\DK2VR_room.cache),
//                SSE2 floating point for the generators (0.4.2 measured 18 s in the background).
// DK2VR 0.4.2 - room prepared on a background thread (0.4.1 blocked the VR thread 2.3 s -> the watchdog
//                turned the decoupled renderer off for good); watchdog now retries.
// DK2VR 0.4.1 - room and table rebuilt: per-pixel lighting (shaders), normal maps, rounded geometry
//                (moulded frame, turned legs, vault, ribs), two drop handles per side; adaptive host wait.
// DK2VR 0.4.0 - dungeon room and wooden table with rim and four hanging iron handles (VR thread),
//                procedural textures, torch lighting, 4x antialiasing on the headset images.
// DK2VR 0.3.2 - wider than deep table (TableAspect), metal handles: one hand turns the table on itself,
//                two hands carry it (host); grips on the handles are ignored by the map controls.
// DK2VR 0.3.1 - fixed physical table (GrabMode 2): two grips = native zoom + native camera rotation
//                (CCamera direction +0xD84 via 0x449F50); the dungeon turns and slides on a table that never moves.
// DK2VR 0.3.0 - decoupled VR rendering: a VR thread with its own D3D9Ex device redraws the newest game
//                frame at the headset rate (head, table grab/turn, pan prediction); textures shared.
// DK2VR 0.2.9 - table surface at the walking floor (highest strong height peak near the camera floor):
//                the camera floor is the bottom of the dungeon-heart pit, ~0.45 unit below the tiles.
// DK2VR 0.2.8 - table rectangle limited to the engine's real 3D render area (0x760AEC..0x760AF8,
//                excludes the bottom UI panel -> no empty base in front); floor never raised by measurement.
// DK2VR 0.2.7 - stable table levels: camera floor + smoothed, bounded measured offsets (0.2.6 per-frame
//                percentiles trembled and could jump); profiler reports wrapper and game call sites.
// DK2VR 0.2.6 - table levels measured on the geometry (dungeon sits on the base at every zoom),
//                non-blocking monitor Present (D3DPRESENT_DONOTWAIT), deeper profiler stacks.
// DK2VR 0.2.5 - no monitor vertical-blank wait (wrapper vtable), pipelined GPU sync (no spin in the
//                game thread), profiler call chains, ViewExpand 1.15 (diagonal cut corners).
// DK2VR 0.2.4 - table base drawn first without depth (no more grey squares over sunken floors),
//                profiler attributes kernel waits to their caller module.
// DK2VR 0.2.3 - back faces kept (holes fix) + sampling profiler of the game render thread.
// DK2VR 0.2.2 - diagnostic build: native game timing counters, F12 VR pause, ViewExpand off by default.
// DK2VR 0.2.1 - x86 bridge loaded by the dxwrapper fork inside DKII-DX.exe 1.70.
// Base: 0.2.0 (tested in headset 26/09/2026: table, grab, wall panel OK).
// 0.2.1: native pan/zoom with the controllers, wider native visibility (fewer holes),
//        batched stereo replay (2 render-target switches per eye instead of one pair per draw),
//        top-down camera fix, zoom-out extension no longer disabled by a transient zero.
#include <map>
#include <set>
#include "WaitProbe.hpp"
#include "ThreadState.hpp"   // 0.9.47.10
#include "../common/Ipc.hpp"
#include "../common/Math.hpp"
#include "../common/HeldModel.hpp"
#include "../common/Tabletop.hpp"
#include "../common/Decouple.hpp"
#include "../common/HeadingFollow.hpp"
#include "../common/TableModel.hpp"
#include "../common/HudLayout.hpp"
#include "../common/Comfort.hpp"
#include "../common/FrameGuard.hpp"
#include "../common/DrawBatch.hpp"
#include "../common/Cadence.hpp"
#include "../common/MotionFit.hpp"
#include "../common/FastCrc.hpp"
#include "../common/CubeFaces.hpp"
#include "../common/MenuSplit.hpp"
#include "../common/UprightSprite.hpp"
#include "../common/SelectionRibbon.hpp"
#include "../common/BadgePlane.hpp"
#include "../common/MenuCursor.hpp"
#include "MenuFaces.hpp"
#include "MenuMaterials.hpp"
#include "MenuChains.hpp"
#include "MenuDepthTarget.hpp"
#include "KeyboardLabels.hpp"   // 0.9.47.34
#include "../common/Keyboard.hpp"
#include <d3d9.h>
#include <wincodec.h>
#include <d3dcommon.h>
#include <wrl/client.h>
#include <vector>
#include <unordered_set>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <algorithm>
#include <filesystem>
#include <psapi.h>
#include <mmsystem.h>
#include <unordered_map>
#include <string>
#include <array>
#include <atomic>
#include <memory>
#include <mutex>
#include <functional>
#include <deque>
#include "CallTiming.hpp"
#include "PerfProbe.hpp"   // 0.9.25
#include "../common/FastMath.hpp"   // 0.9.47.8
#include "../common/NativeSkip.hpp"   // 0.9.47.10
#include "../common/PoolGuard.hpp"    // 0.9.47.17
#include "../common/IniRepair.hpp"    // 0.9.47.63
#include "../common/FanRule.hpp"      // 0.9.47.19
#include "../common/DigitNumber.hpp"   // 0.9.47.21
#include "../common/AffinityPolicy.hpp"   // 0.9.47.12
#include <tlhelp32.h>
#include "FastBlt.hpp"   // 0.9.47.13
// 0.9.47.13 FastBlt: the replacement row routine the entry stub calls (see FastBlt.hpp). Runs on the thread that calls the wrapper's software blit (the game thread).
// The first rows are checked against the original semantics on a copy of the destination row; on any difference the original semantics are used from then on.
namespace fastbltstat {
inline long long rows=0,pixels=0,stretchRows=0,mirrorRows=0,keyRows=0,verifiedRows=0;
inline volatile long fallback=0,mismatchWidth=0;
inline fastblt::IndexCache cache;   // 0.9.47.15: source index table of the current stretch geometry (used by the first calling thread only)
inline DWORD owner=0;
inline long long timedRows=0,timedPixels=0,ticksFast=0,ticksRef=0,tsc0=0,qpc0=0;   // 0.9.47.15: 1 row in 32 is run through BOTH routines and timed (same row, same moment)
struct Geo {int width=0,srcW=0;float scale=0;long long n=0;};
inline Geo geo[4];
inline uint32_t scratch[4096];
__attribute__((noinline)) inline void timedRow(uint32_t* dst,const uint32_t* src,int width,float scale,int mirror,int srcW,int ckOn,uint32_t key,fastblt::IndexCache* c) {
    const long long t0=static_cast<long long>(__builtin_ia32_rdtsc());
    fastblt::fastRow(dst,src,width,scale,mirror,srcW,ckOn,key,c);
    const long long t1=static_cast<long long>(__builtin_ia32_rdtsc());
    fastblt::referenceRow(scratch,src,width,scale,mirror,srcW,ckOn,key);
    const long long t2=static_cast<long long>(__builtin_ia32_rdtsc());
    ++timedRows;timedPixels+=width;ticksFast+=t1-t0;ticksRef+=t2-t1;
}
}
extern "C" __attribute__((used,force_align_arg_pointer)) void __attribute__((cdecl)) dk2vr_bltRow(uint32_t* dst,const uint32_t* src,int width,float scale,int mirror,int srcW,int ckOn,uint32_t key) {
    using namespace fastbltstat;
    ++rows;pixels+=width>0?width:0;if(scale!=1.0f) ++stretchRows;if(mirror) ++mirrorRows;if(ckOn) ++keyRows;
    if(fallback) {fastblt::referenceRow(dst,src,width,scale,mirror,srcW,ckOn,key);return;}
    {   // the few distinct geometries seen (diagnostic for the report)
        int k=0;for(;k<4;++k) if(geo[k].n&&geo[k].width==width&&geo[k].srcW==srcW&&geo[k].scale==scale) break;
        if(k<4) ++geo[k].n;
        else {int w=0;for(int q=0;q<4;++q) if(geo[q].n<geo[w].n) w=q;geo[w].width=width;geo[w].srcW=srcW;geo[w].scale=scale;geo[w].n=1;}
    }
    const DWORD me=GetCurrentThreadId();
    if(!owner) {owner=me;LARGE_INTEGER q;QueryPerformanceCounter(&q);qpc0=q.QuadPart;tsc0=static_cast<long long>(__builtin_ia32_rdtsc());}
    fastblt::IndexCache* c=me==owner?&cache:nullptr;
    if(verifiedRows<64&&width>0&&width<=4096&&!fastblt::overlap(dst,src,width)) {
        ++verifiedRows;
        uint32_t tmp[4096];std::memcpy(tmp,dst,size_t(width)*4u);
        fastblt::fastRow(dst,src,width,scale,mirror,srcW,ckOn,key,c);
        fastblt::referenceRow(tmp,src,width,scale,mirror,srcW,ckOn,key);
        if(std::memcmp(tmp,dst,size_t(width)*4u)!=0) {std::memcpy(dst,tmp,size_t(width)*4u);mismatchWidth=width;fallback=1;}
        return;
    }
    if(me==owner&&(rows&31)==0&&width>0&&width<=4096) {timedRow(dst,src,width,scale,mirror,srcW,ckOn,key,c);return;}
    fastblt::fastRow(dst,src,width,scale,mirror,srcW,ckOn,key,c);
}
// 0.9.26 PERF: where the mod's own time goes in the game thread's draw hook (game thread only, QueryPerformanceCounter).
// 0.9.47.60 PUBLIC BUILD (-DDK2VR_PUBLIC): every measurement probe, call timing, capture / dump, test launcher variable and key poll is OFF and cannot be switched on again
// (DK2VR.ini keys of the diagnostics are ignored): nothing in this build measures or records anything beyond the normal log.
#ifdef DK2VR_PUBLIC
constexpr bool kPublicBuild=true;
#else
constexpr bool kPublicBuild=false;
#endif
struct RecProf {
    static constexpr int N=8;   // 0 draw hook total, 1 reprojection, 2 sprites, 3 vertex copy, 4 state capture, 5 texture copies, 6 hand, 7 merge
    static inline long long t[N]{};static inline long draws=0;
    static void lap(int i,long long& mark) {if(kPublicBuild) return;const long long n=Clock::now();t[i]+=n-mark;mark=n;}
    static void clear() {for(auto& v:t) v=0;draws=0;}
};
using Microsoft::WRL::ComPtr;
using namespace dk2vr;

// 0.8.0: entry stub of the engine's primitive setup 0x58A970 (see EngineOrigin): hands the stack pointer at entry
// (return address + the 6 cdecl arguments) to the plugin, then continues into the original code (trampoline) with
// the stack untouched. Only eax/ecx/edx (free at a cdecl call) are used.
extern "C" {
void* dk2vr_beginTramp=nullptr;
__attribute__((used,force_align_arg_pointer)) void __cdecl dk2vr_beginNote(uint32_t* sp);
void dk2vr_beginStub();
}
asm(".text\n.globl _dk2vr_beginStub\n_dk2vr_beginStub:\n\tmovl %esp,%eax\n\tpushl %eax\n\tcall _dk2vr_beginNote\n\taddl $4,%esp\n\tjmp *_dk2vr_beginTramp\n");

namespace {
constexpr const char* Version="0.9.47.63";
// 0.9.2: the host tells where the mod folder is (DK2VR_DIR): the mod can sit INSIDE the game folder (DK2VR\ next to DKII-DX.exe).
// Without it (older layout: <mod>\Game\DKII-DX.exe) the settings are one level above the game folder, as before.
// 0.9.47.63: DK2VR.ini is healed before its first read (a UTF-8 byte-order mark made Windows ignore the whole first [VR] section: Possession360 and every
// default of the file were lost, and each saved setting appended a second [VR] section). See common/IniRepair.hpp.
dk2vr::inirepair::Result gIniRepair;bool gIniRepaired=false;
std::filesystem::path configurationPath() {
    static const std::filesystem::path path=[] {
        std::filesystem::path p;
        wchar_t dir[MAX_PATH]{};const DWORD n=GetEnvironmentVariableW(L"DK2VR_DIR",dir,MAX_PATH);
        if(n>0&&n<MAX_PATH) p=std::filesystem::path(dir)/L"DK2VR.ini";
        else {wchar_t module[MAX_PATH]{};GetModuleFileNameW(nullptr,module,MAX_PATH);p=std::filesystem::path(module).parent_path().parent_path()/L"DK2VR.ini";}
        try {gIniRepaired=dk2vr::inirepair::repairFile(p,gIniRepair);} catch(...) {}
        return p;
    }();
    return path;
}
bool profileEngine();
float iniFloat(const wchar_t* name,float fallback,float min,float max) {
    if(profileEngine()&&(!wcscmp(name,L"ViewExpand")||!wcscmp(name,L"ViewFarScale"))) return 1.f;   // 0.9.47.18 test profile 3 / 5: no wider engine view
    wchar_t value[64]{};GetPrivateProfileStringW(L"VR",name,L"",value,64,configurationPath().c_str());
    if(!value[0]) return fallback;
    wchar_t* end=nullptr;float parsed=wcstof(value,&end);
    return end!=value&&std::isfinite(parsed)?std::clamp(parsed,min,max):fallback;
}
// 0.9.47.17 TEST PROFILES (the launchers TEST_EAU_n set DK2VR_PROFILE): they switch features off for one run, whatever DK2VR.ini says, to find out
// which one a visual defect depends on. Without DK2VR_PROFILE nothing changes (DK2VR.ini decides, as before).
//   1 = the optimisations and probes added since 0.9.47.7;  2 = 1 + the older exact accelerations of 0.8.56 - 0.8.59.
// 0.9.47.18 (water): 3 = 2 + the settings that change WHAT THE ENGINE SENDS (back faces kept, forced high detail / texture quality / LOD, wider view);
//   4 = 2 + the REPLAY rules of 0.9.30 - 0.9.47 (depth write removed from translucent / decal / sprite draws, opaque first, upright effects, icons, digits, forge smoke fans);  5 = 3 + 4.
int testProfile() {
    if(kPublicBuild) return 0;
    static const int profile=[] {wchar_t v[16]{};const DWORD n=GetEnvironmentVariableW(L"DK2VR_PROFILE",v,16);if(n==0||n>=16) return 0;return (v[0]>=L'1'&&v[0]<=L'5')?int(v[0]-L'0'):0;}();
    return profile;
}
bool profileEngine() {const int p=testProfile();return p==3||p==5;}
bool profileRules() {const int p=testProfile();return p==4||p==5;}
// 0.9.47.20 MEASURE MODE (launcher LANCER_AVEC_MESURES.cmd sets DK2VR_MEASURE=1): the measurement probes, switched OFF by default in the clean build, are switched ON for one run.
bool measureMode() {
    if(kPublicBuild) return false;
    static const bool on=[] {wchar_t v[8]{};const DWORD n=GetEnvironmentVariableW(L"DK2VR_MEASURE",v,8);return n>0&&n<8&&v[0]==L'1';}();
    return on;
}
bool profileOverride(const wchar_t* name,int& value) {
    if(kPublicBuild) {   // 0.9.47.60: diagnostics permanently off, whatever DK2VR.ini or the environment says
        static const wchar_t* const off[]={L"PerfMesure",L"PerfMesureAlterne",L"PerfTest",L"WaitProbe",L"CpuSampler",L"LockDetail",L"LockDiagnostic",L"ThreadStateProbe",L"MotionProbe",L"ProbeAuto",
            L"Profiler",L"ProfilerSampling",L"EngineProfile",L"PossessionTrace",L"MenuDiagnostic",L"MenuUiDebug",L"MenuReplayCheck",L"Possession360Debug",L"KeyPollThread"};
        for(const wchar_t* k:off) if(!wcscmp(name,k)) {value=0;return true;}
        return false;   // CallTiming is NOT in the list: its hooks are needed by the main menu (CallTiming::Scope below does no measuring in this build)
    }
    if(measureMode()) {
        static const wchar_t* const probes[]={L"PerfMesure",L"ThreadStateProbe",L"MotionProbe",L"PossessionTrace",L"MenuDiagnostic",L"LockDetail"};
        for(const wchar_t* k:probes) if(!wcscmp(name,k)) {value=1;return true;}
    }
    if(!wcscmp(name,L"HeldCreature3D")) {   // 0.9.47.22: launcher LANCER_CREATURE_2D.cmd sets DK2VR_HELD2D=1 = the held creature stays the flat 2D image for one run
        static const bool flat=[] {wchar_t v[8]{};const DWORD n=GetEnvironmentVariableW(L"DK2VR_HELD2D",v,8);return n>0&&n<8&&v[0]==L'1';}();
        if(flat) {value=0;return true;}
    }
    if(!wcscmp(name,L"HeldDepthFlip")) {   // 0.9.47.23: launcher LANCER_PROFONDEUR_INVERSEE.cmd (DK2VR_HELDFLIP=1) turns the measured depth direction round for one run
        static const bool flip=[] {wchar_t v[8]{};const DWORD n=GetEnvironmentVariableW(L"DK2VR_HELDFLIP",v,8);return n>0&&n<8&&v[0]==L'1';}();
        if(flip) {value=1;return true;}
    }
    if(!wcscmp(name,L"TextureStaging")||!wcscmp(name,L"GameTextureShadow")||!wcscmp(name,L"GameSurfaceShadow")) {   // 0.9.47.27: launcher LANCER_TEXTURES_SURES.cmd (DK2VR_TEXSAFE=1): the game's texture locks stay native for one run
        static const bool safe=[] {wchar_t v[8]{};const DWORD n=GetEnvironmentVariableW(L"DK2VR_TEXSAFE",v,8);return n>0&&n<8&&v[0]==L'1';}();
        if(safe) {value=0;return true;}
    }
    const int profile=testProfile();if(!profile) return false;
    static const wchar_t* const since47[]={L"NativeWorldSkip",L"FastSnapshot",L"FastBlt",L"WorkerAffinity",L"InputAsync",L"MenuFastExp",L"PerfMesure",L"WaitProbe",L"CpuSampler",L"CallTiming",L"LockDetail",L"ThreadStateProbe",L"ProfilerSampling"};
    static const wchar_t* const older[]={L"TextureStaging",L"FastCrc",L"FastStretch",L"SyscallSavings"};
    static const wchar_t* const engine[]={L"DrawBackFaces",L"ForceHighDetail",L"TextureDetailFix",L"ForceTextureQuality"};
    static const wchar_t* const rules[]={L"FloorDecalNoDepth",L"TranslucentNoDepth",L"SpriteNoDepth",L"OpaqueFirst",L"UprightEffectsNoDepth",L"UprightEffects",L"ForgeSmokeFanHide",L"SpriteAlphaOnly",L"CreatureIconsFacePlayer",L"DamageDigitsUpright",L"DigitsDrawnLast",L"BadgeDiscUpright"};
    for(const wchar_t* k:since47) if(!wcscmp(name,k)) {value=0;return true;}
    if(profile>=2) for(const wchar_t* k:older) if(!wcscmp(name,k)) {value=0;return true;}
    if(profileEngine()) for(const wchar_t* k:engine) if(!wcscmp(name,k)) {value=0;return true;}
    if(profileRules()) for(const wchar_t* k:rules) if(!wcscmp(name,k)) {value=0;return true;}
    return false;
}
int iniInt(const wchar_t* name,int fallback) {int forced=0;if(profileOverride(name,forced)) return forced;return int(GetPrivateProfileIntW(L"VR",name,fallback,configurationPath().c_str()));}
// 0.6.8: choices made in VR (pedestal side...) live in DK2VR-user.ini next to DK2VR.ini: DK2VR.ini is refreshed
// from the mod folder at every launch, the user file is never overwritten.
std::wstring userConfigPath() {std::wstring p=configurationPath();size_t k=p.find_last_of(L"\\/");return (k==std::wstring::npos?std::wstring():p.substr(0,k+1))+L"DK2VR-user.ini";}
int userInt(const wchar_t* name,int fallback) {return int(GetPrivateProfileIntW(L"User",name,fallback,userConfigPath().c_str()));}
void setUserInt(const wchar_t* name,int value) {WritePrivateProfileStringW(L"User",name,std::to_wstring(value).c_str(),userConfigPath().c_str());}
double milliseconds() {
    static const double frequency=[] {LARGE_INTEGER f{};QueryPerformanceFrequency(&f);return double(f.QuadPart)/1000.;}();
    LARGE_INTEGER now{};QueryPerformanceCounter(&now);return double(now.QuadPart)/frequency;
}
// 0.3.0: called from the game thread and the VR thread.
CRITICAL_SECTION* logLock() {static CRITICAL_SECTION* c=[]{auto* x=new CRITICAL_SECTION;InitializeCriticalSection(x);return x;}();return c;}
void log(const char* format,...) {
    EnterCriticalSection(logLock());
    FILE* f=fopen("DK2VR-game.log","a");
    if(f) {
        fprintf(f,"[%llu] ",(unsigned long long)GetTickCount64());
        va_list a; va_start(a,format); vfprintf(f,format,a); va_end(a);
        fputc('\n',f); fclose(f);
    }
    LeaveCriticalSection(logLock());
}
// 0.8.62: the game is a 32-bit process of 2 GB (no large-address flag). A bigger texture cache must not eat the room it needs.
void reportAddressSpace() {
    MEMORYSTATUSEX ms{};ms.dwLength=sizeof(ms);if(!GlobalMemoryStatusEx(&ms)) return;
    SIZE_T largest=0;const unsigned char* a=nullptr;MEMORY_BASIC_INFORMATION m{};
    for(int guard=0;guard<100000&&VirtualQuery(a,&m,sizeof(m))==sizeof(m);++guard) {
        if(m.State==MEM_FREE) largest=std::max<SIZE_T>(largest,m.RegionSize);
        const unsigned char* next=static_cast<const unsigned char*>(m.BaseAddress)+m.RegionSize;if(next<=a) break;a=next;
    }
    PROCESS_MEMORY_COUNTERS_EX pmc{};pmc.cb=sizeof(pmc);GetProcessMemoryInfo(GetCurrentProcess(),reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc),sizeof(pmc));
    const double mb=1024.*1024.;
    log("Address space of the game process: %.0f MB of %.0f MB in use, %.0f MB free (largest free block %.0f MB); private memory %.0f MB",
        double(ms.ullTotalVirtual-ms.ullAvailVirtual)/mb,double(ms.ullTotalVirtual)/mb,double(ms.ullAvailVirtual)/mb,double(largest)/mb,double(pmc.PrivateUsage)/mb);
}
bool readable(const void* p,size_t size) {
    MEMORY_BASIC_INFORMATION m{};
    if(!VirtualQuery(p,&m,sizeof(m))||m.State!=MEM_COMMIT||(m.Protect&(PAGE_NOACCESS|PAGE_GUARD))) return false;
    return reinterpret_cast<uintptr_t>(p)+size<=reinterpret_cast<uintptr_t>(m.BaseAddress)+m.RegionSize;
}
bool writable(void* p,size_t size) {
    MEMORY_BASIC_INFORMATION m{};
    if(!VirtualQuery(p,&m,sizeof(m))||m.State!=MEM_COMMIT) return false;
    if(!(m.Protect&(PAGE_READWRITE|PAGE_EXECUTE_READWRITE))||(m.Protect&PAGE_GUARD)) return false;
    return reinterpret_cast<uintptr_t>(p)+size<=reinterpret_cast<uintptr_t>(m.BaseAddress)+m.RegionSize;
}
struct NativeCamera {
    uintptr_t base=0; bool supported=false;
    NativeCamera() {
        base=reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
        auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return;
        auto nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(base+dos->e_lfanew);
        const unsigned char signature[]={0x83,0xec,0x60,0x8b,0x44,0x24,0x64,0x56,0x57};
        supported=nt->FileHeader.Machine==IMAGE_FILE_MACHINE_I386&&nt->OptionalHeader.SizeOfImage>0x395710&&
            !memcmp(reinterpret_cast<void*>(base+0x18a6f0),signature,sizeof(signature));
        log("DK2VR %s - native camera signature: %s",Version,supported?"DKII 1.70 recognized":"unsupported; VR world disabled");
        (void)configurationPath();if(gIniRepaired) log("DK2VR.ini repaired (0.9.47.63): byte-order mark %s, %d repeated [VR] section(s) merged (%d settings); original kept as DK2VR.ini.avant_reparation",gIniRepair.bom?"removed":"absent",gIniRepair.mergedSections,gIniRepair.mergedKeys);
    }
    template<class T> T read(uintptr_t va) const { T v{}; memcpy(&v,reinterpret_cast<void*>(base+va-0x400000),sizeof(v)); return v; }
    bool lens(Projection& p) const {
        if(!supported) return false;
        // Verified against 0x58A6F0 / 0x58ADC0: focal, centers (pixel convention includes -0.5).
        p={read<float>(0x77f430),read<float>(0x77f430),read<float>(0x77f4f0),read<float>(0x77f930)};
        return std::isfinite(p.focalX)&&p.focalX>1&&p.focalX<100000&&std::isfinite(p.centerX)&&std::isfinite(p.centerY);
    }
    bool frontend() const {
        // MyResources(+0x2A07 GameCfg): useFe3d(+0x168), read only.
        // The native animated main menu runs a separate 3D level.
        return supported && read<uint32_t>(0x75aeaf)!=0;
    }
    // Engine view rectangle used for tile selection (read in MyDLVec2i_generate 0x574820): left,right,top,bottom.
    ScreenRect renderArea(const Projection& lens) const {
        ScreenRect full=fullScreen(lens);
        if(!supported) return full;
        ScreenRect r{read<float>(0x760aec),read<float>(0x760af4),read<float>(0x760af0),read<float>(0x760af8)};
        bool ok=std::isfinite(r.left)&&std::isfinite(r.right)&&std::isfinite(r.top)&&std::isfinite(r.bottom)&&
            r.left>=full.left-2&&r.right<=full.right+2&&r.top>=full.top-2&&r.bottom<=full.bottom+2&&
            r.right-r.left>64&&r.bottom-r.top>64;
        return ok?r:full;
    }
    Tabletop tabletop(const Projection& lens,float units,float crop) const {
        if(!supported||units<=0) return {};
        // 58A6F0 constructs the camera transform at 77F458. 594DB0 uses
        // row-vector multiplication; its last row is native +Z, which points
        // DOWN. makeNativeTabletop reverses it and cancels screen-axis scales.
        Vec3 nativeZ{read<float>(0x77f470),read<float>(0x77f474),read<float>(0x77f478)};
        Vec3 translation{read<float>(0x780940),read<float>(0x780944),read<float>(0x780948)};
        return makeNativeTabletop(nativeZ,translation,lens,read<float>(0x77f4cc),read<float>(0x78093c),units,crop,renderArea(lens));
    }
    Tabletop tabletopArea(const Projection& lens,float units,float crop,ScreenRect area,bool circumscribe) const {
        if(!supported||units<=0) return {};
        Vec3 nativeZ{read<float>(0x77f470),read<float>(0x77f474),read<float>(0x77f478)};
        Vec3 translation{read<float>(0x780940),read<float>(0x780944),read<float>(0x780948)};
        return makeNativeTabletop(nativeZ,translation,lens,read<float>(0x77f4cc),read<float>(0x78093c),units,crop,area,circumscribe);
    }
    // Flame mapping (DiaLight/Flame, DKII_EXE_v170.sgmap): g_pCBridge (0x795700) -> CCamera at +0x3E5.
    // CCamera: +0xD50 minZoomLevel, +0xD54 maxZoomLevel, +0xD80 curZoomLevel, +0xDA8 mode.
    unsigned char* camera() const {
        if(!supported) return nullptr;
        uintptr_t bridge=read<uint32_t>(0x795700);
        if(!bridge) return nullptr;
        auto* c=reinterpret_cast<unsigned char*>(bridge+0x3e5);
        return readable(c+0xd50,0x5c)?c:nullptr;
    }
    // 0.8.94.19: camera transition (verified in the 1.70 exe). setMode 0x44ACE0 starts a transition of 2000 ms between the table and a possession
    // (500 ms otherwise) through 0x44BD70(duration): +0xDDC = start time, +0xDD8 = end time, +0xDD4 = target mode. Every frame the camera update
    // 0x44BED0 interpolates zoom / position / tilt (+0xD80, +0xD68.., +0xD84..) towards the target and, once the time reaches +0xDD8, calls
    // 0x44BD70(0): the target pose and mode are applied and +0xDD8 goes back to 0. A duration of 0 is the game's own instant path.
    bool transition(uint32_t& end,uint32_t& start,uint32_t& target) const {
        auto* c=camera();end=start=target=0;if(!c||!readable(c+0xdd4,12)) return false;
        memcpy(&target,c+0xdd4,4);memcpy(&end,c+0xdd8,4);memcpy(&start,c+0xddc,4);return end!=0;
    }
    bool zoomRangeZero() const {auto* c=camera();if(!c) return false;uint32_t a=1,b=1;memcpy(&a,c+0xd50,4);memcpy(&b,c+0xd54,4);return a==0&&b==0;}
    bool finishTransition() const {   // the end time is put in the past: the game's next camera update completes the transition itself
        auto* c=camera();if(!c||!readable(c+0xdd8,4)) return false;uint32_t e=0;memcpy(&e,c+0xdd8,4);if(!e) return false;
        const uint32_t one=1;memcpy(c+0xdd8,&one,4);return true;
    }
    uint32_t mode() const {
        auto* c=camera(); uint32_t result=0;
        if(c) memcpy(&result,c+0xda8,4);
        return result<=22?result:0;
    }
};
// Extends the native zoom-out limit so the table can show a larger part of the dungeon.
// Only the maximum is changed; reversible by ZoomOutExtra=0. Reapplied if the game resets it.
struct NativeZoom {
    const NativeCamera& native;
    float extra=0; int32_t original=0,patched=0; bool logged=false,failed=false;
    NativeZoom(const NativeCamera& n):native(n) {extra=iniFloat(L"ZoomOutExtra",1.f,0.f,4.f);}
    // 0.8.94.15: the amount is now chosen live on the OPTIONS wall panel (DEZOOM TABLE slider). It is THE fluidity setting: the engine draws
    // every tile of the visible area (report 20261005_083809: 408 units^2 = 702 draws and 29 fps, 101 units^2 = 50 draws and 85 fps).
    void setExtra(float value) {
        if(!std::isfinite(value)) return;
        value=std::clamp(value,0.f,4.f);
        if(std::fabs(value-extra)<.005f) return;
        extra=value;patched=0;logged=false;
        auto* c=native.camera();
        if(c&&original>0&&writable(c+0xd54,4)) {int32_t back=original;memcpy(c+0xd54,&back,4);}   // back to the game's own limit, tick() re-extends
    }
    void tick() {
        if(failed||native.frontend()) return;
        auto* c=native.camera(); if(!c) return;
        if(extra<=0) {   // the game's own limit: put it back if we had extended it
            if(original>0&&patched&&writable(c+0xd54,4)) {int32_t back=original;memcpy(c+0xd54,&back,4);patched=0;log("Native zoom-out range back to the game's own limit (%d)",original);}
            return;
        }
        int32_t minimum=0,maximum=0;memcpy(&minimum,c+0xd50,4);memcpy(&maximum,c+0xd54,4);
        if(maximum==patched&&patched) return;
        // Level loading transiently reports 0/0: wait instead of disabling (0.2.0 defect).
        if(minimum<=0||maximum<=minimum||maximum>50000000) {if(!logged){log("Zoom range not ready (min=%d max=%d); retrying",minimum,maximum);logged=true;}return;}
        if(!writable(c+0xd54,4)) {log("Zoom range not writable; zoom extension disabled");failed=true;return;}
        original=maximum;patched=maximum+int32_t(float(maximum-minimum)*extra);
        memcpy(c+0xd54,&patched,4);
        log("Native zoom-out range extended: min=%d max=%d -> %d (ZoomOutExtra=%.2f)",minimum,original,patched,extra);
    }
};
// 0.8.66: thumbstick motion integrated at the HEADSET rate (VR thread), consumed by the game thread.
// The game applies the integrated motion to its native camera at its own (lower) rate, and the VR thread
// shows the part not yet applied (prediction), exactly as it does for a grip gesture. 'hand' is a virtual
// hand: the dungeon follows it (stick scroll = the dungeon slides opposite to the stick direction).
struct StickDrive {
    static inline std::mutex m;
    static inline Vec3 hand{0,0,0};static inline float angle=0;
    static inline double lastMs=0,updateMs=0;static inline std::atomic<uint64_t> scrolls{0},turns{0};
    static inline bool loaded=false,enabled=true,invert=false;static inline float speed=.6f,turnSpeed=90.f;
    static void load() {
        if(loaded) return;
        enabled=iniInt(L"StickPrediction",1)!=0;speed=iniFloat(L"StickScrollSpeed",.6f,.05f,5.f);turnSpeed=iniFloat(L"StickTurnSpeed",90.f,10.f,360.f);invert=iniInt(L"StickTurnInvert",0)!=0;loaded=true;
    }
    // VR thread, once per headset frame.
    static void integrate(const Request& r,double now) {
        load();
        if(!enabled) return;
        std::lock_guard<std::mutex> lock(m);
        const float dt=lastMs>0?float(std::min(std::max(now-lastMs,0.),50.)*.001):0.f;lastMs=now;updateMs=now;
        const bool left=(r.handFlags&HandLeftGrip)&&!(r.handFlags&HandLeftTable),right=(r.handFlags&HandRightGrip)&&!(r.handFlags&HandRightTable);
        if(left||right||dt<=0) return;
        if(std::isfinite(r.stickX)&&std::isfinite(r.stickY)&&(r.stickX!=0||r.stickY!=0)) {
            const Quat yaw=yawOnly(r.eyes[0].pose.orientation);
            Vec3 side=rotate(yaw,{1,0,0}),fwd=rotate(yaw,{0,0,-1});
            Vec3 delta=add(scale(side,r.stickX*speed*dt),scale(fwd,r.stickY*speed*dt));
            Vec3 onTable=rotate(conjugate(r.table.orientation),delta);onTable.y=0;delta=rotate(r.table.orientation,onTable);
            hand=sub(hand,delta);
        }
        if(std::isfinite(r.turnStick)&&r.turnStick!=0) angle+=r.turnStick*turnSpeed*dt*(Pi/180.f)*(invert?1.f:-1.f);
    }
    static bool fresh(double now) {std::lock_guard<std::mutex> lock(m);return enabled&&updateMs>0&&now-updateMs<150;}
    static void snapshot(Vec3& h,float& a) {std::lock_guard<std::mutex> lock(m);h=hand;a=angle;}
};
// Controller gestures applied to the native camera, from the game thread (Present).
// One grip: the dungeon follows the hand (CCamera::moveRel 0x449BD0, 4096 units per tile,
// verified in 0x598CD6: position * 1/4096). Two grips: hand spread = zoom (CCamera::setZoom 0x449C30).
struct NativeControl {
    using MoveRel=void(__thiscall*)(void*,const int32_t*);
    using SetZoom=void(__thiscall*)(void*,int32_t,int32_t);
    using SetDirection=void(__thiscall*)(void*,const int16_t*);
    const NativeCamera& native;
    MoveRel moveRel=nullptr;SetZoom setZoom=nullptr;SetDirection setDirection=nullptr;bool enabled=false;int grabMode=2;
    bool panning=false;int hand=0;Vec3 lastHand{};float restX=0,restY=0;
    bool zooming=false;float distance0=0;int32_t zoom0=0;
    // 0.3.1 rotation: the dungeon turns with the hands (yawQuat sense). The sign of the native direction
    // (2048 units per turn) is measured on the rendered camera (dungeon heading in the table frame).
    bool rotating=false;float angle0=0,lastAngle=0;int32_t direction0=0;int sign=1;bool calibrated=false;
    bool haveHeading=false;float lastHeading=0;int32_t lastDirection=0;
    uint64_t pans=0,zooms=0,turns=0,overridden=0;int32_t expected=-1;
    bool stickDriven=false;Vec3 appliedHand{};float appliedAngle=0;
    double lastStickMs=0;float stickRestX=0,stickRestY=0,stickSpeed=.6f;uint64_t stickPans=0;double stickPrevMs=0;float turnRest=0,turnSpeed=90.f,zoomRate=1.2f;int turnInvert=0;uint64_t stickTurns=0,stickZooms=0;
    NativeControl(const NativeCamera& n):native(n) {
        grabMode=iniInt(L"GrabMode",2);stickSpeed=iniFloat(L"StickScrollSpeed",.6f,.05f,5.f);turnSpeed=iniFloat(L"StickTurnSpeed",90.f,10.f,360.f);zoomRate=iniFloat(L"StickZoomRate",1.2f,.2f,4.f);turnInvert=iniInt(L"StickTurnInvert",0);
        if(!native.supported||grabMode==0) return;
        const unsigned char move[]={0xf6,0x81,0x92,0x0d,0x00,0x00,0x40};
        const unsigned char zoom[]={0x56,0x57,0x8b,0x7c,0x24,0x10,0x8b,0xf1};
        const unsigned char dir[]={0x8b,0x44,0x24,0x04,0x66,0x8b,0x10,0x66,0x89,0x91,0x84,0x0d,0x00,0x00,0xe8};
        auto* m=reinterpret_cast<unsigned char*>(native.base+0x49bd0);
        auto* z=reinterpret_cast<unsigned char*>(native.base+0x49c30);
        auto* d=reinterpret_cast<unsigned char*>(native.base+0x49f50);
        if(memcmp(m,move,sizeof(move))||memcmp(z,zoom,sizeof(zoom))) {log("Camera control signatures unsupported; controller pan/zoom disabled");return;}
        moveRel=reinterpret_cast<MoveRel>(m);setZoom=reinterpret_cast<SetZoom>(z);enabled=true;
        if(grabMode==2) {
            if(!memcmp(d,dir,sizeof(dir))) setDirection=reinterpret_cast<SetDirection>(d);
            else log("Camera direction signature unsupported; two-grip rotation disabled");
        }
        log("Controller control: one grip = pan, two grips = zoom%s (GrabMode=%d)",setDirection?" + native rotation, table fixed":"",grabMode);
    }
    static float distance2d(Vec3 a,Vec3 b) {float x=a.x-b.x,z=a.z-b.z;return std::sqrt(x*x+z*z);}
    int32_t direction(unsigned char* c) const {uint16_t v=0;memcpy(&v,c+0xd84,2);return int32_t(v&0x7ff);}
    // World X axis of the rendered native camera, as a heading in the table frame (yawQuat sense).
    bool heading(const Pose& camInTable,float& h) const {
        float sx=native.read<float>(0x77f4cc),sy=native.read<float>(0x78093c),focal=native.read<float>(0x77f430);
        if(!(focal>1)) return false;
        sx/=focal;sy/=focal;
        Vec3 axis{native.read<float>(0x77f458)*sx,-native.read<float>(0x77f45c)*sy,-native.read<float>(0x77f460)};
        if(!normalize(axis)) return false;
        Vec3 v=rotate(camInTable.orientation,axis);
        if(v.x*v.x+v.z*v.z<1e-4f) return false;
        h=std::atan2(-v.z,v.x);return true;
    }
    // Called once per game frame with the camera used to draw it (before this frame's commands).
    void measure(const Pose& camInTable,bool anchored) {
        auto* c=native.camera();float h=0;
        if(!c||!anchored||!heading(camInTable,h)) {haveHeading=false;return;}
        int32_t d=direction(c);
        if(expected>=0&&d!=expected) ++overridden;
        expected=-1;
        if(haveHeading) {
            int32_t dd=((d-lastDirection+1024)&2047)-1024;
            if(std::abs(dd)>=4&&std::abs(dd)<=400) {
                float r=wrapRadians(h-lastHeading)/(dd*Pi/1024);
                if(std::fabs(r)>.6f&&std::fabs(r)<1.4f) {
                    int s2=r>0?1:-1;
                    if(!calibrated||s2!=sign) log("Native rotation calibrated: dungeon heading %s with the camera direction (ratio %.2f)",s2>0?"increases":"decreases",r);
                    sign=s2;calibrated=true;
                }
            }
        }
        haveHeading=true;lastHeading=h;lastDirection=d;
    }
    void tick(const Request& r,const Pose& tableCamera,float units,bool anchored,uint32_t cameraMode) {
        // 0.3.2: a hand holding the table by a handle does not act on the map.
        bool left=(r.handFlags&HandLeftGrip)&&!(r.handFlags&HandLeftTable),right=(r.handFlags&HandRightGrip)&&!(r.handFlags&HandRightTable);
        int hands=int(left)+int(right);
        auto* c=native.camera();
        bool usable=enabled&&anchored&&r.tableAnchor&&c&&!native.frontend()&&cameraMode!=1&&cameraMode!=2&&units>0;
        uint32_t flags=0;
        if(usable) {uint32_t endTime=0;memcpy(&flags,c+0xd92,4);memcpy(&endTime,c+0xdd8,4);usable=!(flags&0x48)&&!endTime;}
        if(!usable||hands!=1) panning=false;
        if(!usable||hands!=2) {zooming=false;rotating=false;}
        const bool drive=usable&&hands==0&&StickDrive::fresh(milliseconds());
        if(!usable) {lastStickMs=0;stickDriven=false;return;}
        if(!drive) stickDriven=false;
        Vec3 driveHand{};float driveAngle=0;if(drive) StickDrive::snapshot(driveHand,driveAngle);
        // 0.6.2: thumbstick scrolling (all directions, relative to where the player looks), StickScrollSpeed table metres/s.
        {
            const double now=milliseconds();const float dt=lastStickMs>0?float(std::min(now-lastStickMs,50.)*.001):0.f;lastStickMs=now;
            bool haveDelta=false;Vec3 delta{0,0,0};
            if(drive) {
                // 0.8.66: the VR thread integrated the sticks at the headset rate; apply what is not yet applied.
                if(stickDriven) {delta=sub(appliedHand,driveHand);haveDelta=delta.x!=0||delta.y!=0||delta.z!=0;}
                appliedHand=driveHand;
            } else if(hands==0&&dt>0&&(r.stickX!=0||r.stickY!=0)&&std::isfinite(r.stickX)&&std::isfinite(r.stickY)) {
                const Quat yaw=yawOnly(r.eyes[0].pose.orientation);
                Vec3 side=rotate(yaw,{1,0,0}),fwd=rotate(yaw,{0,0,-1});
                delta=add(scale(side,r.stickX*stickSpeed*dt),scale(fwd,r.stickY*stickSpeed*dt));haveDelta=true;
            }
            if(haveDelta) {
                Vec3 onTable=rotate(conjugate(r.table.orientation),delta);onTable.y=0;delta=rotate(r.table.orientation,onTable);
                Vec3 d=scale(rotate(conjugate(tableCamera.orientation),delta),units);
                float sx=native.read<float>(0x77f4cc),sy=native.read<float>(0x78093c),focal=native.read<float>(0x77f430);
                if(focal>1) {
                    sx/=focal;sy/=focal;Vec3 axis[2];bool ok=true;
                    for(int i=0;i<2;++i) {uintptr_t row=0x77f458+uintptr_t(i)*12;axis[i]={native.read<float>(row)*sx,-native.read<float>(row+4)*sy,-native.read<float>(row+8)};if(!normalize(axis[i])) ok=false;}
                    if(ok) {
                        // The view travels in the stick direction (the dungeon slides the other way).
                        float wx=dot(axis[0],d)*4096+stickRestX,wy=dot(axis[1],d)*4096+stickRestY;
                        int32_t step[3]={int32_t(wx),int32_t(wy),0};stickRestX=wx-float(step[0]);stickRestY=wy-float(step[1]);
                        if(step[0]||step[1]) {moveRel(c,step);++stickPans;if(drive) ++StickDrive::scrolls;}
                    }
                }
            }
        }
        // 0.8.19: pointing-hand thumbstick (only with no grip held): left/right turns the dungeon (native rotation, StickTurnSpeed degrees/s
        // at full deflection), up = zoom in / down = zoom out (StickZoomRate, factor per second). Same native calls as the two-hand gesture.
        if(drive) {
            // 0.8.66: rotation integrated by the VR thread (same units and sign as the 0.8.20 path below).
            if(stickDriven&&setDirection&&!(flags&1)) {
                turnRest+=(driveAngle-appliedAngle)*(1024.f/Pi)*float(sign);
                const int32_t whole=int32_t(turnRest);
                if(whole) {turnRest-=float(whole);int16_t v=int16_t((direction(c)+whole)&2047);setDirection(c,&v);++stickTurns;++StickDrive::turns;}
            }
            appliedAngle=driveAngle;stickDriven=true;
        }
        if(hands==0&&std::isfinite(r.turnStick)&&std::isfinite(r.zoomStick)&&(r.turnStick!=0||r.zoomStick!=0)) {
            const float dt2=std::min(float(std::max(0.,milliseconds()-stickPrevMs)*.001),.05f);
            if(!drive&&r.turnStick!=0&&setDirection&&!(flags&1)&&dt2>0) {
                turnRest+=r.turnStick*turnSpeed*dt2*(2048.f/360.f)*float(sign)*(turnInvert?1.f:-1.f); // 0.8.20: direction reversed vs 0.8.19 (user)
                const int32_t whole=int32_t(turnRest);
                if(whole) {turnRest-=float(whole);int16_t v=int16_t((direction(c)+whole)&2047);setDirection(c,&v);++stickTurns;}
            }
            if(r.zoomStick!=0&&dt2>0) {
                int32_t cur=0;memcpy(&cur,c+0xd80,4);
                if(cur>0) {int32_t target=int32_t(std::clamp(double(cur)*std::exp(-double(r.zoomStick)*zoomRate*dt2),1.,5.e7));if(target==cur) target+=r.zoomStick<0?1:-1;setZoom(c,std::max(1,target),0);++stickZooms;}
            }
        } else if(!drive) turnRest=0;
        stickPrevMs=milliseconds();
        if(hands==1) {
            Vec3 hand=(right?r.hands[1]:r.hands[0]).position;
            if(!panning||this->hand!=(right?1:0)) {panning=true;this->hand=right?1:0;lastHand=hand;restX=restY=0;return;}
            Vec3 delta=sub(hand,lastHand);lastHand=hand;
            // Only the motion parallel to the table surface drags the dungeon.
            Vec3 onTable=rotate(conjugate(r.table.orientation),delta);onTable.y=0;
            delta=rotate(r.table.orientation,onTable);
            Vec3 d=scale(rotate(conjugate(tableCamera.orientation),delta),units);
            float sx=native.read<float>(0x77f4cc),sy=native.read<float>(0x78093c),focal=native.read<float>(0x77f430);
            if(!(focal>1)) return;
            sx/=focal;sy/=focal;
            Vec3 axis[2];
            for(int i=0;i<2;++i) {
                uintptr_t row=0x77f458+uintptr_t(i)*12;
                axis[i]={native.read<float>(row)*sx,-native.read<float>(row+4)*sy,-native.read<float>(row+8)};
                if(!normalize(axis[i])) return;
            }
            // The camera moves opposite to the hand so that the dungeon follows the hand.
            float wx=-dot(axis[0],d)*4096+restX,wy=-dot(axis[1],d)*4096+restY;
            int32_t step[3]={int32_t(wx),int32_t(wy),0};
            restX=wx-float(step[0]);restY=wy-float(step[1]);
            if(step[0]||step[1]) {moveRel(c,step);++pans;}
        } else if(hands==2) {
            float d=distance2d(r.hands[0].position,r.hands[1].position);
            float a=handsAngle(r.hands[0].position,r.hands[1].position);
            if(!zooming) {
                zooming=true;distance0=d;memcpy(&zoom0,c+0xd80,4);
                rotating=setDirection&&!(flags&1)&&d>.03f;angle0=lastAngle=a;direction0=direction(c);
                return;
            }
            if(distance0>=.03f&&d>=.01f&&zoom0>0) {
                // Hands apart = closer (like pinch zoom). setZoom clamps to the native range.
                int32_t target=int32_t(std::clamp(double(zoom0)*distance0/d,1.,5.e7));
                setZoom(c,target,0);++zooms;
            }
            if(rotating&&d>.03f) {
                // Absolute from the start of the gesture: the dungeon turns by the angle the hands turned.
                float theta=wrapRadians(a-angle0);
                int32_t target=(direction0+int32_t(std::lround(theta*1024/Pi))*sign)&2047;
                if(target!=direction(c)) {int16_t v=int16_t(target);setDirection(c,&v);++turns;}
                expected=target;lastAngle=a;
            }
        }
    }
};
// Mouse wheel zoom (absent from the original game, same approach as DiaLight/Flame
// use_wheel_to_zoom): WM_MOUSEWHEEL -> CCamera::zoomRel (0x449CA0), on the game thread.
// 0.8.94.7: the objectives page ('0') and the game's own menu are the only things that may open the middle panel besides a real notification box.
// Pixel heuristics were tried from 0.8.21 to 0.8.94.6 and every one of them ended up true for ever on something else (held creature sprite in report
// 20261004_160911, full-screen grid of 512x32 strips in report 20261004_165153). The key is read where the game reads it: the window procedure.
struct PageKeys {
    inline static volatile LONG objectivesOpen=0,toggles=0;
    static void note(UINT m,WPARAM w) {
        if(m==WM_KEYDOWN) {
            if(w=='0'||w==VK_NUMPAD0) {objectivesOpen=objectivesOpen?0:1;InterlockedIncrement(&toggles);}
            else if(w==VK_ESCAPE||w==VK_SPACE||w==VK_RETURN) objectivesOpen=0;
        }
    }
    static void reset() {objectivesOpen=0;}
};
struct WheelZoom {
    using ZoomRel=void(__thiscall*)(void*,int32_t);
    inline static WheelZoom* active=nullptr;
    const NativeCamera& native;ZoomRel zoomRel=nullptr;WNDPROC previous=nullptr;HWND window=nullptr;int step=50;
    // 0.9.47.62: RunInBackground=1 (default): the game keeps running when another window (OBS, the PC mirror window, a browser...) has the focus.
    // The game window procedure is told it is still active: WM_ACTIVATEAPP / WM_ACTIVATE "inactive", WM_NCACTIVATE false and WM_KILLFOCUS are not passed on.
    // The game window is also removed from the always-on-top layer when it loses the focus, so that the other windows can be seen.
    bool background=false;unsigned bgSwallowed=0;
    WheelZoom(const NativeCamera& n):native(n) {
        if(native.supported&&iniInt(L"RunInBackground",1)!=0) {background=true;active=this;}
        if(!native.supported||iniInt(L"WheelZoom",1)==0) return;
        const unsigned char sig[]={0xf6,0x81,0x92,0x0d,0x00,0x00,0x08};
        auto* f=reinterpret_cast<unsigned char*>(native.base+0x49ca0);
        if(memcmp(f,sig,sizeof(sig))) {log("Wheel zoom signature unsupported");return;}
        zoomRel=reinterpret_cast<ZoomRel>(f);step=int(iniFloat(L"WheelZoomStep",50,5,500));active=this;
    }
    static LRESULT CALLBACK proc(HWND h,UINT m,WPARAM w,LPARAM l) {
        PageKeys::note(m,w);   // 0.8.94.7
        if(active&&active->background) {   // 0.9.47.62
            bool swallow=false;
            if(m==WM_ACTIVATEAPP&&!w) {w=TRUE;}
            else if(m==WM_ACTIVATE&&LOWORD(w)==WA_INACTIVE) {w=WA_ACTIVE;}
            else if(m==WM_NCACTIVATE&&!w) {w=TRUE;}
            else if(m==WM_KILLFOCUS) swallow=true;
            if(m==WM_ACTIVATEAPP||m==WM_ACTIVATE||m==WM_NCACTIVATE||swallow) {
                static int seen=0;
                const bool deact=swallow||(m==WM_ACTIVATEAPP&&w==TRUE&&GetForegroundWindow()!=h)||(m==WM_ACTIVATE&&GetForegroundWindow()!=h)||(m==WM_NCACTIVATE&&GetForegroundWindow()!=h);
                if(deact) {
                    ++active->bgSwallowed;
                    if(m==WM_ACTIVATEAPP&&(GetWindowLongA(h,GWL_EXSTYLE)&WS_EX_TOPMOST)) SetWindowPos(h,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);   // let the other windows (OBS, mirror) be seen
                    if(seen<12) {++seen;log("Run in background (0.9.47.62): message 0x%04X made 'still active' (foreground window is not the game's), swallowed so far %u, window style 0x%08lX ex 0x%08lX, minimised %s",unsigned(m),active->bgSwallowed,(unsigned long)GetWindowLongA(h,GWL_STYLE),(unsigned long)GetWindowLongA(h,GWL_EXSTYLE),IsIconic(h)?"yes":"no");}
                }
                if(swallow) return 0;
            }
        }
        if((m==WM_KEYDOWN||m==WM_KEYUP||m==WM_CHAR)&&((w>='1'&&w<='6')||(m==WM_CHAR&&(w=='&'||w==0xE9||w=='"'||w=='\''||w=='('||w=='-')))) {   // 0.9.47.37 probe: do the digit keys reach the game window?
            static int seen=0;if(seen<40) {++seen;log("Touche chiffre recue par la fenetre du jeu : %s wParam=0x%02X scancode=0x%02X repetition=%d (fenetre au premier plan : %s)",m==WM_KEYDOWN?"WM_KEYDOWN":m==WM_KEYUP?"WM_KEYUP":"WM_CHAR",unsigned(w),unsigned((l>>16)&0xff),int(l&0xffff),GetForegroundWindow()==h?"oui":"NON");}
        }
        if(m==WM_MOUSEWHEEL&&active&&active->zoomRel&&!active->native.frontend()) {
            if(auto* c=active->native.camera()) active->zoomRel(c,-int32_t(GET_WHEEL_DELTA_WPARAM(w))*active->step);
        }
        return CallWindowProcA(active&&active->previous?active->previous:DefWindowProcA,h,m,w,l);   // 0.8.94.1: ANSI like the game window (see attach)
    }
    void attach(HWND w) {
        if((!zoomRel&&!background)||!w||window==w) return;
        if(window) return; // keep the first game window only
        // 0.8.94.1: ANSI subclass. Report 210345: crash when clicking the save-name field: the game then subclasses its window again with
        // SetWindowLongA (0x430D0E), keeps the previous procedure at +0x4C and CALLS it directly (0x4305E4 'call esi'). A procedure installed with
        // SetWindowLongPtrW is handed to an ANSI caller as a 0xFFFFxxxx handle (only valid through CallWindowProc): read at 0xFFFF1953 -> crash.
        previous=reinterpret_cast<WNDPROC>(SetWindowLongPtrA(w,GWLP_WNDPROC,reinterpret_cast<LONG_PTR>(&proc)));
        if(previous) {window=w;log("Mouse wheel zoom enabled (step %d)%s",step,background?"; run in background ON (0.9.47.62)":"");} else log("Mouse wheel zoom unavailable (%lu)",(unsigned long)GetLastError());
    }
};
// Monitor refresh wait: DKII asks the wrapper to wait for the monitor vertical blank; the headset has
// its own timing. The wrapper's m_IDirectDrawX::WaitForVerticalBlank (stdcall this+2 args, ret 0Ch,
// RVA 0x143470 in the shipped DDRAW.dll) is replaced in its vtable (RVA 0x76756C) by an immediate DD_OK.
struct NoVBlankWait {
    static HRESULT __stdcall immediate(void*,DWORD,HANDLE) {return 0;}
    static void install() {
        if(iniInt(L"NoMonitorVSync",1)==0) {log("Monitor vertical-blank wait kept");return;}
        auto dd=reinterpret_cast<uintptr_t>(GetModuleHandleW(L"ddraw.dll"));
        if(!dd) {log("NoMonitorVSync: wrapper not found");return;}
        auto* slot=reinterpret_cast<uintptr_t*>(dd+0x76756c);
        const unsigned char head[]={0x55,0x8b,0xec,0x6a,0xff};
        if(!readable(slot,4)||*slot!=dd+0x143470||memcmp(reinterpret_cast<void*>(dd+0x143470),head,sizeof(head))) {log("NoMonitorVSync: wrapper signature unsupported, wait kept");return;}
        DWORD old=0;if(!VirtualProtect(slot,4,PAGE_READWRITE,&old)) return;
        *slot=reinterpret_cast<uintptr_t>(&immediate);DWORD ignored=0;VirtualProtect(slot,4,old,&ignored);
        log("NoMonitorVSync: wrapper WaitForVerticalBlank disabled (vtable patched)");
    }
};
// Monitor presentation that never blocks the game: the real D3D9Ex device's Present/PresentEx are
// redirected to PresentEx(..., D3DPRESENT_DONOTWAIT). When the monitor queue is full the PC screen
// simply skips that image; the game thread (and therefore the headset) is no longer throttled by it.
struct NonBlockingPresent {
    using PresentFn=HRESULT(__stdcall*)(IDirect3DDevice9*,const RECT*,const RECT*,HWND,const RGNDATA*);
    using PresentExFn=HRESULT(__stdcall*)(IDirect3DDevice9Ex*,const RECT*,const RECT*,HWND,const RGNDATA*,DWORD);
    inline static PresentExFn originalEx=nullptr;inline static volatile LONG skipped=0,shown=0;inline static bool installed=false,tried=false;
    static HRESULT __stdcall present(IDirect3DDevice9* d,const RECT* a,const RECT* b,HWND w,const RGNDATA* r) {
        return presentEx(static_cast<IDirect3DDevice9Ex*>(d),a,b,w,r,0);
    }
    static HRESULT __stdcall presentEx(IDirect3DDevice9Ex* d,const RECT* a,const RECT* b,HWND w,const RGNDATA* r,DWORD flags) {
        CallTiming::Scope timed(CallTiming::Present);
        HRESULT hr=originalEx(d,a,b,w,r,flags|D3DPRESENT_DONOTWAIT);
        if(hr==D3DERR_WASSTILLDRAWING) {InterlockedIncrement(&skipped);return S_OK;}
        InterlockedIncrement(&shown);return hr;
    }
    static void install(IDirect3DDevice9* d) {
        if(tried) return;
        tried=true;
        if(iniInt(L"NonBlockingPresent",1)==0) {log("Monitor Present: original (blocking)");return;}
        ComPtr<IDirect3DDevice9Ex> ex;if(FAILED(d->QueryInterface(IID_PPV_ARGS(&ex)))) {log("Monitor Present: no D3D9Ex, unchanged");return;}
        void** vtable=*reinterpret_cast<void***>(d);
        // IDirect3DDevice9::Present = slot 17, IDirect3DDevice9Ex::PresentEx = slot 121.
        DWORD old=0;if(!VirtualProtect(&vtable[17],sizeof(void*)*(121-17+1),PAGE_READWRITE,&old)) {log("Monitor Present: vtable protected, unchanged");return;}
        originalEx=reinterpret_cast<PresentExFn>(vtable[121]);
        vtable[17]=reinterpret_cast<void*>(&present);vtable[121]=reinterpret_cast<void*>(&presentEx);
        DWORD ignored=0;VirtualProtect(&vtable[17],sizeof(void*)*(121-17+1),old,&ignored);
        installed=true;log("Monitor Present: non-blocking (D3DPRESENT_DONOTWAIT); skipped PC-screen images are counted in the log");
    }
};
// 0.4.5: DKII's texture cache (SurfHashList) holds 24 blocks of 256x256; when a view needs more textures,
// the engine uses reduced versions ("MM1".."MM3": blurry tiles next to sharp ones). The VR table shows a wider,
// closer view than the original screen. static_MyDirectDraw_devTexture_init (0x592EA0) builds the cache with
// 'push 0x18' at 0x592FAC; this becomes TextureCacheBlocks (default 96, as the Flame project's big-resolution fix).
// Applied before the game's 3D texture initialisation when possible; otherwise at its next initialisation.
// 0.5.10: the game's REAL texture cache. With Direct3D (hardware path) DKII does not use SurfHashList (0x792D60,
// patched by 0.4.5-0.5.9 for nothing: "-1 blocks") but SurfHashList2 pSurfHashList2_2 (0x7920D0), built at
// 0x593259 by SurfHashList2 constructor 0x592BD0(desc, minimum 32, count 512) with holders of 128x128 (verified:
// constructor loop creates 'count' SurfaceHolder_create(0x80,...)). 512 textures were not enough for the table's
// wide view: ~1300 texture reloads per second (0.5.9 log), each one waiting on the GPU in dxwrapper.
// The 'push 0x200' at 0x593247 becomes TextureCacheBlocks (default 1536 = 96 MB of 128x128 textures).
struct TextureCache {
    static int actual(const NativeCamera& native) {
        uintptr_t list=native.supported?native.read<uint32_t>(0x7920d0):0;
        if(!list||IsBadReadPtr(reinterpret_cast<void*>(list+0xdc),4)) return -1;
        return *reinterpret_cast<int*>(list+0xdc); // holder count (constructor 0x592C2C)
    }
    static void install(const NativeCamera& native) {
        int blocks=iniInt(L"TextureCacheBlocks",1536);
        if(!native.supported||blocks<=512) {log("Texture cache: original size (512 textures)");return;}
        blocks=std::min(blocks,4096);
        {   // 0.8.62: the game is 32-bit (2 GB, no large-address flag). Each holder is a 128x128 texture (64 KB) plus up to two system-memory copies kept by the
            // wrapper/driver; the cache is only enlarged as far as 700 MB of address space stay free for everything else the game allocates later.
            MEMORYSTATUSEX ms{};ms.dwLength=sizeof(ms);
            if(GlobalMemoryStatusEx(&ms)) {
                const double freeMb=double(ms.ullAvailVirtual)/1048576.,perBlock=0.1875;
                if(freeMb-blocks*perBlock<700.) {
                    int fit=int((freeMb-700.)/perBlock);if(fit<1536) fit=1536;
                    if(fit<blocks) {log("Texture cache: %d textures asked, only %.0f MB of address space free: reduced to %d",blocks,freeMb,fit);blocks=fit;}
                }
            }
        }
        auto* p=reinterpret_cast<unsigned char*>(native.base+0x193247);
        const unsigned char sig[]={0x68,0x00,0x02,0x00,0x00,0x6a,0x20,0x68,0x98,0x2d,0x79,0x00,0x89,0x0d,0xd0,0x20,0x79,0x00,0xe8};
        int32_t rel=0;memcpy(&rel,p+19,4);
        if(memcmp(p,sig,sizeof(sig))||uintptr_t(p+23+rel)!=native.base+0x192bd0) {log("Texture cache: signature unsupported, original size kept");return;}
        DWORD old=0;if(!VirtualProtect(p+1,4,PAGE_EXECUTE_READWRITE,&old)) return;
        const uint32_t v=uint32_t(blocks);memcpy(p+1,&v,4);DWORD ignored=0;VirtualProtect(p+1,4,old,&ignored);
        FlushInstructionCache(GetCurrentProcess(),p,5);
        log("Texture cache: %d textures of 128x128 instead of 512 (%s)",blocks,native.read<uint32_t>(0x7920d0)?"cache already built: applied at its next initialisation":"applied before the texture initialisation");
    }
};

// 0.8.59: exact speed-ups of two engine/wrapper hot loops found by the CPU sampler (0.8.56.3 profile, same
// 2D-interface cost in every game frame whatever the 3D load). Both keep the result bit-for-bit:
//  * FastCrc: DKII 0x590C30 checks, for every 128x32 strip of its 2D interface image (about 3 MB per frame), whether the
//    strip changed, with a 16-bit CRC computed byte by byte (14 % of the game thread). The loop (0x590C8A..0x590CCD) is
//    replaced by a call to a sliced evaluation of the same CRC (src/common/FastCrc.hpp, same value for every input).
//  * FastStretch: the wrapper's CopySurface (ddraw.dll RVA 0x117380) stretches the 2D image pixel by pixel and calls
//    __ftol3 (float->int, a function call) for every pixel (11 % of the game thread). The 12 calls become the single
//    instruction CVTTSS2SI (same truncation). Memory patch only: ddraw.dll on disk is unchanged.
struct FastCrc {
    inline static fastcrc::Tables* tables=nullptr;
    inline static uint64_t calls=0,bytes=0,ticks=0;
    inline static bool active=false;
    static uint32_t __cdecl compute(uint32_t rowBytes,const unsigned char* base,uint32_t rows,uint32_t stride) {
        if(!rowBytes||!rows) return 0xfbea;
        LARGE_INTEGER a,b;QueryPerformanceCounter(&a);
        const uint32_t c=fastcrc::rows(*tables,rowBytes,base,rows,stride);
        QueryPerformanceCounter(&b);
        ++calls;bytes+=uint64_t(rowBytes)*rows;ticks+=uint64_t(b.QuadPart-a.QuadPart);
        return c;
    }
    static void install(const NativeCamera& native) {
        if(!native.supported||iniInt(L"FastCrc",1)==0) {log("Fast change CRC: off (%s)",native.supported?"FastCrc=0":"game version not recognised");return;}
        auto* site=reinterpret_cast<unsigned char*>(native.base+0x190c88);
        static const unsigned char expected[0x47]={0x56,0x57,0x8b,0x5d,0xf0,0x8b,0x4d,0xf4,0xb8,0xea,0xfb,0x00,0x00,0x33,0xd2,0x8b,0x7d,0xf8,0x53,0x51,0x8b,0x4d,0x0c,0x8a,0x13,0x32,0xd0,0x43,0xc1,0xe8,0x08,0x49,0x8b,0x34,0x95,0x20,0xff,0x66,0x00,0x74,0x14,0x33,0xc6,0x8a,0x13,0x32,0xd0,0x43,0xc1,0xe8,0x08,0x49,0x8b,0x34,0x95,0x20,0xff,0x66,0x00,0x75,0xec,0x33,0xc6,0x59,0x5b,0x03,0xdf,0x49,0x75,0xcc,0x5f};
        if(IsBadReadPtr(site,sizeof(expected))||memcmp(site,expected,sizeof(expected))) {log("Fast change CRC: engine code differs from DKII 1.70, original loop kept");return;}
        const uint32_t* engine=reinterpret_cast<const uint32_t*>(native.base+0x26ff20);
        if(IsBadReadPtr(engine,1024)) {log("Fast change CRC: table unreadable, original loop kept");return;}
        auto* t=new fastcrc::Tables;
        if(!fastcrc::build(engine,*t)) {delete t;log("Fast change CRC: engine table is not the expected 16-bit CRC table, original loop kept");return;}
        // Self-test against the engine's own byte loop on pseudo-random data (several shapes) before patching.
        std::vector<unsigned char> sample(70000);uint32_t seed=0x12345678u;
        for(auto& v:sample) {seed=seed*1664525u+1013904223u;v=static_cast<unsigned char>(seed>>24);}
        bool ok=true;
        const uint32_t shapes[][3]={{512,32,4096},{256,32,2048},{512,1,512},{17,9,23},{3,5,3},{1,1,1}};
        for(auto& s:shapes) for(unsigned off=0;off<4;++off) {
            if(size_t(s[1])*s[2]+s[0]+off>sample.size()) continue;
            if(fastcrc::rows(*t,s[0],sample.data()+off,s[1],s[2])!=fastcrc::referenceRows(engine,s[0],sample.data()+off,s[1],s[2])) ok=false;
        }
        if(!ok) {delete t;log("Fast change CRC: self-test FAILED, original loop kept");return;}
        tables=t;
        // push [ebp-8] (pitch); push [ebp-0xC] (rows); push [ebp-0x10] (first row); push [ebp+0xC] (bytes per row);
        // call compute; add esp,0x10; jmp 0x590CCE (the original epilogue of the loop: pop edi / pop esi).
        unsigned char patch[22]={0xff,0x75,0xf8,0xff,0x75,0xf4,0xff,0x75,0xf0,0xff,0x75,0x0c,0xe8,0,0,0,0,0x83,0xc4,0x10,0xeb,0x2e};
        unsigned char* at=site+2;
        const int32_t rel=int32_t(reinterpret_cast<uintptr_t>(&compute)-reinterpret_cast<uintptr_t>(at+12+5));
        memcpy(patch+13,&rel,4);
        if(at+20+2+0x2e!=site+0x46) {log("Fast change CRC: internal address check failed, original loop kept");return;}
        DWORD old=0;if(!VirtualProtect(at,sizeof(patch),PAGE_EXECUTE_READWRITE,&old)) {log("Fast change CRC: memory protection refused, original loop kept");return;}
        memcpy(at,patch,sizeof(patch));DWORD ignored=0;VirtualProtect(at,sizeof(patch),old,&ignored);FlushInstructionCache(GetCurrentProcess(),at,sizeof(patch));
        active=true;
        log("Fast change CRC: engine loop at 0x590C8A replaced (sliced CRC, self-test OK, same values as the engine)");
    }
    static void report() {
        if(!active) return;
        static const double freq=[]{LARGE_INTEGER f;QueryPerformanceFrequency(&f);return double(f.QuadPart);}();
        const uint64_t c=calls,b=bytes,t=ticks;calls=bytes=ticks=0;
        log("Fast change CRC (last 5 s): %llu strips, %.1f MB, %.0f ms in total (%.2f ms per second)",(unsigned long long)c,b/1048576.0,t*1e3/freq,t*1e3/freq/5.0);
    }
};
struct FastStretch {
    inline static bool active=false;
    static void install() {
        if(iniInt(L"FastStretch",1)==0) {log("Fast stretch: off (FastStretch=0)");return;}
        HMODULE wrapper=GetModuleHandleW(L"ddraw.dll");
        if(!wrapper) {log("Fast stretch: wrapper module not found");return;}
        const uintptr_t base=reinterpret_cast<uintptr_t>(wrapper),ftol=base+0x1ca4e0;
        static const uint32_t sites[12]={0x11a50b,0x11a55b,0x11a5ac,0x11a6ab,0x11a6fb,0x11a74f,0x11a87d,0x11a90c,0x11a996,0x11aaab,0x11aafb,0x11ab4c};
        for(uint32_t rva:sites) {
            auto* c=reinterpret_cast<const unsigned char*>(base+rva);
            if(IsBadReadPtr(c-8,13)) {log("Fast stretch: wrapper code unreadable, kept as is");return;}
            int32_t rel=0;memcpy(&rel,c+1,4);
            const bool call=c[0]==0xe8&&uintptr_t(c+5+rel)==ftol;
            const bool mulReg=c[-4]==0xf3&&c[-3]==0x0f&&c[-2]==0x59&&c[-1]==0xc1;  // mulss xmm0,xmm1
            const bool mulMem=c[-8]==0xf3&&c[-7]==0x0f&&c[-6]==0x59&&c[-5]==0x85;  // mulss xmm0,[ebp-disp32]
            if(!call||!(mulReg||mulMem)) {log("Fast stretch: wrapper differs from the expected build (site 0x%x), kept as is",(unsigned)rva);return;}
        }
        static const unsigned char cvt[5]={0xf3,0x0f,0x2c,0xc0,0x90}; // cvttss2si eax,xmm0 ; nop
        int done=0;
        for(uint32_t rva:sites) {
            auto* c=reinterpret_cast<unsigned char*>(base+rva);DWORD old=0;
            if(!VirtualProtect(c,5,PAGE_EXECUTE_READWRITE,&old)) continue;
            memcpy(c,cvt,5);DWORD ignored=0;VirtualProtect(c,5,old,&ignored);FlushInstructionCache(GetCurrentProcess(),c,5);++done;
        }
        active=done==12;
        log("Fast stretch: %d of 12 float->int calls in the wrapper's 2D stretch replaced by one instruction (memory only, ddraw.dll unchanged)",done);
    }
};
// 0.4.6: texture quality. The 96-block cache (0.4.5) did not help: the game itself selects reduced
// textures ("MM1".."MM3"). Verified in DKII 1.70: g_ReductionLevel (0x7932DC, copied from the video setting
// 0x759AF4 by setTextureReductionLevel 0x566620) and MyDirectDraw devTexture.reductionLevel (0x792D8C) are read
// by SurfHashList::_probablySort (0x5948FC) each time a texture is chosen; the "high resolution textures"
// setting is 0x759AF0 (read at 0x59D968). ForceTextureQuality=1 keeps all of them at full quality.
struct TextureQuality {
    bool ok=false;uintptr_t base=0;int changes=0;
    int32_t* at(uintptr_t va) const {return reinterpret_cast<int32_t*>(base+va-0x400000);}
    void install(const NativeCamera& native) {
        if(!native.supported||iniInt(L"ForceTextureQuality",1)==0) {log("Texture quality: game setting kept");return;}
        base=native.base;
        const unsigned char s1[]={0x89,0x46,0x7c,0xa3,0xdc,0x32,0x79,0x00};
        const unsigned char s2[]={0x8b,0x0d,0xdc,0x32,0x79,0x00,0x8b,0x35,0x8c,0x2d,0x79,0x00};
        const unsigned char s3[]={0xa1,0xf0,0x9a,0x75,0x00};
        ok=!memcmp(reinterpret_cast<void*>(base+0x166641),s1,sizeof(s1))&&!memcmp(reinterpret_cast<void*>(base+0x1948fc),s2,sizeof(s2))
          &&!memcmp(reinterpret_cast<void*>(base+0x19d968),s3,sizeof(s3));
        if(!ok) {log("Texture quality: signature unsupported, game setting kept");return;}
        log("Texture quality at start: setting=%d reduction=%d devTexture=%d highRes=%d",*at(0x759af4),*at(0x7932dc),*at(0x792d8c),*at(0x759af0));
        enforce();
    }
    void enforce() {
        if(!ok) return;
        int32_t setting=*at(0x759af4),reduction=*at(0x7932dc),dev=*at(0x792d8c),high=*at(0x759af0);
        if(setting<=0&&reduction<=0&&dev<=0&&high!=0) return;
        if(changes<20) log("Texture quality forced to maximum (was setting=%d reduction=%d devTexture=%d highRes=%d)",setting,reduction,dev,high);
        ++changes;
        if(setting>0) *at(0x759af4)=0;
        if(reduction>0) *at(0x7932dc)=0;
        if(dev>0) *at(0x792d8c)=0;
        if(high==0) *at(0x759af0)=1;
    }
};
// 0.5.0: engine tiles limited to the table (table corners filled at no extra cost).
// Verified in DKII 1.70 (disassembly + 0.4.9.x probes on the user's PC):
// - MyDLVec2i_generate (0x574820) builds the visible volume as a list of WORLD points (MyLLVec3f: nodes
//   {x,y,z,next}), culled between the heights 0x760AA8/0x760B4C, then calls MyLLVec3f_doMatAndCalcMinMax
//   (0x575180, thiscall, call at 0x5750EC: depth range) and MyLLVec3f_toMyDLVec2i (0x5754D0: 2D convex hull of
//   the points in world X,Y = the tiles that are processed).
// - The same function computes 4 visibility planes (0x760B70/B38/B18/B28) used by Vec3f_static_sub_575D70 to
//   drop objects outside the view: they come from the enlarged screen rectangle (ViewExpand), so nothing of
//   the table is dropped.
// - Camera: position 0x760AB8, matrix 0x760AC4 = world -> camera, row vectors, orthonormal (probe:
//   [0.267 0.752 -0.602|-0.964 0.208 -0.167|0 0.625 0.781]), camera +Z = depth; world +Z points down, the floor
//   used by the table is z=0 (camera z = -camera height). Lens 0x77F430 (focal), 0x77F4F0/0x77F930 (centre).
// The plugin gives the table corners as SCREEN positions (its own units differ from world units: 0.4.9.1
// black screen). The hook casts them to the world floor with the game camera, after checking on the game's own
// points that its screen->world casting is right (bottom corners of the current rectangle must be found in the
// list, 0.3 unit tolerance); then the list is replaced by the table rectangle. Otherwise nothing is touched.
struct TileCover {
    using DoMat=void(__fastcall*)(void*,void*,float*,float*);
    using Link=void(__fastcall*)(void*,void*,const float*);
    struct Node {float x,y,z;Node* next;};
    inline static DoMat original=nullptr;inline static Link link=nullptr;inline static uintptr_t base=0;
    inline static bool installed=false,enabled=true;
    inline static volatile LONG tableValid=0,broken=0,replacedFrame=0;
    inline static volatile float sx[4]{},sy[4]{};                  // table corners on the screen (near L,R, far L,R)
    inline static LONG matched=0,used=0,noMatch=0,skipped=0,goodStreak=0;inline static float lastError=0;
    static bool ready() {return installed&&enabled&&!broken&&goodStreak>=10;}
    static float at(uintptr_t va) {return *reinterpret_cast<float*>(base+va-0x400000);}
    static void setTable(const Tabletop& box,const Projection& lens) {
        tableValid=0;if(!installed||!enabled||!box.valid) return;
        const float xs[4]={-box.halfWidth,box.halfWidth,-box.halfWidth,box.halfWidth};
        const float zs[4]={-box.halfDepth,-box.halfDepth,box.halfDepth,box.halfDepth};
        for(int i=0;i<4;++i) {
            Vec3 q=box.point(xs[i],zs[i]);float depth=-q.z;
            if(!(depth>.05f)) return;
            sx[i]=lens.centerX+lens.focalX*(q.x/depth);sy[i]=lens.centerY-lens.focalY*(q.y/depth);
            if(!std::isfinite(sx[i])||!std::isfinite(sy[i])) return;
        }
        tableValid=1;
    }
    struct Camera {float C[3],X[3],Y[3],Z[3],f,cx,cy;};
    // World floor point (z=0) seen at a screen position; ySign chooses the screen y direction.
    static bool cast(const Camera& c,float px,float py,float ySign,float out[2]) {
        float a=(px-c.cx)/c.f,b=ySign*(py-c.cy)/c.f,d[3];
        for(int i=0;i<3;++i) d[i]=c.Z[i]+a*c.X[i]+b*c.Y[i];
        if(!(d[2]>1e-4f)) return false;
        float t=-c.C[2]/d[2];if(!(t>0)||t>1000) return false;
        out[0]=c.C[0]+t*d[0];out[1]=c.C[1]+t*d[1];return true;
    }
    static void replace(void* list) {
        Node* head=*reinterpret_cast<Node**>(list);if(!head) {++skipped;return;}
        Camera c{};float m[9];for(int i=0;i<9;++i) m[i]=at(0x760ac4+4*i);
        for(int i=0;i<3;++i) {c.C[i]=at(0x760ab8+4*i);c.X[i]=m[3*i];c.Y[i]=m[3*i+1];c.Z[i]=m[3*i+2];}
        c.f=at(0x77f430);c.cx=at(0x77f4f0);c.cy=at(0x77f930);
        if(!(c.f>1)||!(c.C[2]<-.05f)) {++skipped;return;}
        // Screen rectangle the engine used for this list (enlarged by ViewExpand).
        const float left=at(0x760aec),right=at(0x760af0),top=at(0x760af4),bottom=at(0x760af8);
        // Screen y direction: the bottom of the screen is nearer to the camera than the top.
        float ySign=0;
        for(float sgn:{1.f,-1.f}) {float t0[2],b0[2];
            if(cast(c,(left+right)*.5f,top,sgn,t0)&&cast(c,(left+right)*.5f,bottom,sgn,b0)) {
                float dt=std::hypot(t0[0]-c.C[0],t0[1]-c.C[1]),db=std::hypot(b0[0]-c.C[0],b0[1]-c.C[1]);
                if(db<dt) {ySign=sgn;break;}
            }}
        if(ySign==0) {++skipped;return;}
        // Check: both bottom corners of the rectangle must be points of the game's own list (floor level).
        float zTop=0,err=0;int nodes=0;
        for(Node* p=head;p;p=p->next) {zTop=std::min(zTop,p->z);++nodes;}
        for(float px:{left,right}) {
            float w[2];if(!cast(c,px,bottom,ySign,w)) {++skipped;return;}
            float best=1e30f;for(Node* p=head;p;p=p->next) if(std::fabs(p->z)<.05f) best=std::min(best,std::hypot(p->x-w[0],p->y-w[1]));
            err=std::max(err,best);
        }
        lastError=err;
        // 0.5.1: tolerance 0.3 -> 0.8 unit. The 0.5.0 test measured a constant 0.46 (the corners are
        // found, with a small offset); the table rectangle keeps a 2.2 unit margin and the watchdog stays.
        if(!(err<.8f)) {++noMatch;goodStreak=0;return;}
        ++matched;if(goodStreak<1000) ++goodStreak;
        if(!tableValid||goodStreak<10) return;
        // Table rectangle on the world floor, one tile of margin (outward from its centre).
        float w[4][2];
        for(int i=0;i<4;++i) if(!cast(c,sx[i],sy[i],ySign,w[i])) {++skipped;return;}
        float mx=(w[0][0]+w[1][0]+w[2][0]+w[3][0])*.25f,my=(w[0][1]+w[1][1]+w[2][1]+w[3][1])*.25f;
        float pts[8][3];int k=0;
        for(float h:{0.f,zTop}) for(int i=0;i<4;++i) {
            float dx=w[i][0]-mx,dy=w[i][1]-my,l=std::hypot(dx,dy);float g=l>1e-3f?(l+2.2f)/l:1.f; // margin: wall tops beyond the floor edge stay visible
            pts[k][0]=mx+dx*g;pts[k][1]=my+dy*g;pts[k][2]=h;++k;
        }
        // Every point must stay in front of the camera.
        for(auto& q:pts) {float dep=(q[0]-c.C[0])*c.Z[0]+(q[1]-c.C[1])*c.Z[1]+(q[2]-c.C[2])*c.Z[2];if(!(dep>.3f)) {++skipped;return;}}
        // The game's nodes receive the 8 points; extra nodes become distinct points inside the rectangle
        // (they do not change the hull); missing nodes are added with the game's own MyLLVec3f_linkVec.
        int i=0;
        for(Node* p=head;p;p=p->next,++i) {
            if(i<8) {p->x=pts[i][0];p->y=pts[i][1];p->z=pts[i][2];}
            else {float o=.013f*float(i);p->x=mx+o;p->y=my-o*.5f;p->z=0;}
        }
        for(;i<8;++i) link(list,nullptr,pts[i]);
        replacedFrame=1;++used;
    }
    static void __fastcall hook(void* list,void*,float* mn,float* mx) {
        if(enabled&&!broken) replace(list);
        original(list,nullptr,mn,mx);
    }
    static void install(const NativeCamera& native) {
        enabled=iniInt(L"TableFillCorners",1)!=0;
        if(!native.supported||!enabled) {log("Table corners: off (TableFillCorners=0): the near corners stay empty");enabled=false;return;}
        auto* call=reinterpret_cast<unsigned char*>(native.base+0x1750ec);
        int32_t rel=0;memcpy(&rel,call+1,4);
        const unsigned char linkSig[]={0x56,0x8b,0xf1,0x6a,0x10,0xe8};
        const unsigned char doMatSig[]={0x83,0xec,0x18,0x53,0x8b,0x5c,0x24,0x24};
        if(call[0]!=0xe8||uintptr_t(call+5+rel)!=native.base+0x175180||memcmp(reinterpret_cast<void*>(native.base+0x175130),linkSig,sizeof(linkSig))||
           memcmp(reinterpret_cast<void*>(native.base+0x175180),doMatSig,sizeof(doMatSig))) {
            log("Table corners: signature unsupported; near corners stay empty");enabled=false;return;}
        base=native.base;original=reinterpret_cast<DoMat>(native.base+0x175180);link=reinterpret_cast<Link>(native.base+0x175130);
        DWORD old=0;if(!VirtualProtect(call,5,PAGE_EXECUTE_READWRITE,&old)) {enabled=false;return;}
        int32_t target=int32_t(reinterpret_cast<uintptr_t>(&hook)-reinterpret_cast<uintptr_t>(call+5));
        memcpy(call+1,&target,4);DWORD ignored=0;VirtualProtect(call,5,old,&ignored);FlushInstructionCache(GetCurrentProcess(),call,5);
        installed=true;log("Table corners: engine tile list hooked at 0x5750EC (active after 10 verified frames)");
    }
    static void report() {
        log("Table corners: %s; verified frames %ld, table tiles used %ld, check failed %ld (last error %.2f), skipped %ld",
            !installed?"off":broken?"DISABLED (watchdog)":ready()?"active":"waiting for verification",matched,used,noMatch,lastError,skipped);
        matched=used=noMatch=skipped=0;
    }
};
// 0.5.4 diagnostic: DKII's surface locks. After 0.5.3 the game thread still waits ~85 % of its time in the
// driver, now under the wrapper's shadow-surface lock (DDRAW+0x121317, IDirect3DSurface9::LockRect) and
// D3DXLoadSurfaceFromSurface (DDRAW+0x18764E). Both come from CEngineDDSurface::lockBuf / unlockBuf
// (vtable 0x6703C4 slots 6/7 = 0x590AC0 / 0x590B20, verified). The two slots are wrapped to measure, per
// calling site in the game, the number of calls, the surface size and the time spent. Log: "Surface locks".
struct LockSite {uintptr_t ret=0;uint32_t w=0,h=0,calls=0,unlocks=0;double lockMs=0,unlockMs=0;};
struct LockProbe {
    using Site=LockSite;
    using LockFn=void*(__fastcall*)(void*,void*);   // lockBuf 0x590AC0: thiscall, no stack argument (plain ret, verified)
    using UnlockFn=int(__fastcall*)(void*,void*,void*); // unlockBuf 0x590B20: thiscall, one stack argument (ret 4, verified)
    inline static LockFn lockOrig=nullptr;inline static UnlockFn unlockOrig=nullptr;
    inline static Site sites[16];inline static uintptr_t base=0;inline static void* lastSurface=nullptr;inline static int lastSite=-1;
    static int site(uintptr_t ret,void* self) {
        for(int i=0;i<16;++i) if(sites[i].ret==ret) return i;
        for(int i=0;i<16;++i) if(!sites[i].ret) {sites[i].ret=ret;sites[i].w=*reinterpret_cast<uint32_t*>(static_cast<char*>(self)+4);sites[i].h=*reinterpret_cast<uint32_t*>(static_cast<char*>(self)+8);return i;}
        return -1;
    }
    static void* __fastcall lock(void* self,void*) {
        uintptr_t ret=reinterpret_cast<uintptr_t>(__builtin_return_address(0));
        double t=milliseconds();void* r=lockOrig(self,nullptr);double dt=milliseconds()-t;
        int i=site(ret,self);if(i>=0) {++sites[i].calls;sites[i].lockMs+=dt;}
        lastSurface=self;lastSite=i;return r;
    }
    static int __fastcall unlock(void* self,void*,void* a) {
        double t=milliseconds();int r=unlockOrig(self,nullptr,a);double dt=milliseconds()-t;
        if(self==lastSurface&&lastSite>=0) {++sites[lastSite].unlocks;sites[lastSite].unlockMs+=dt;}
        return r;
    }
    static void install(const NativeCamera& native) {
        if(!native.supported||iniInt(L"LockDiagnostic",1)==0) return;
        base=native.base;
        auto* slot=reinterpret_cast<void**>(native.base+0x2703dc);
        if(slot[0]!=reinterpret_cast<void*>(native.base+0x190ac0)||slot[1]!=reinterpret_cast<void*>(native.base+0x190b20)) {log("Surface locks: vtable signature unsupported");return;}
        lockOrig=reinterpret_cast<LockFn>(slot[0]);unlockOrig=reinterpret_cast<UnlockFn>(slot[1]);
        DWORD old=0;if(!VirtualProtect(slot,8,PAGE_READWRITE,&old)) return;
        slot[0]=reinterpret_cast<void*>(&lock);slot[1]=reinterpret_cast<void*>(&unlock);
        DWORD ignored=0;VirtualProtect(slot,8,old,&ignored);
        log("Surface locks: CEngineDDSurface::lockBuf/unlockBuf measured (vtable 0x6703DC)");
    }
    static void report(double seconds) {
        if(!lockOrig) return;
        char line[1600];int n=snprintf(line,sizeof(line),"Surface locks (last %.0f s, game caller: calls, surface, lock ms, unlock ms):",seconds);
        for(auto& st:sites) if(st.ret&&st.calls&&n<1450)
            n+=snprintf(line+n,sizeof(line)-n," | %06X: %u, %ux%u, %.1f ms, %.1f ms",unsigned(st.ret-base),st.calls,st.w,st.h,st.lockMs,st.unlockMs);
        log("%s",line);
        for(auto& st:sites) {st.calls=st.unlocks=0;st.lockMs=st.unlockMs=0;}
    }
};
// 0.5.13: steep game camera for the table. The screen view of DKII is a perspective trapezoid: to cover a
// rectangular table the engine had to compute a much larger area (cost) and could not reach the player side
// under its camera when zoomed in (empty strip). With the camera looking almost straight down, the area it
// computes is nearly the table's rectangle. CCamera (bridge 0x795700 + 0x3E5) +0xD86 = tilt, clamped by the
// game to 0..0x1CC (rotateRel 0x449D40, verified); the camera matrix is rebuilt by 0x44D6D0 (thiscall, no
// argument, plain ret, verified; also called by setDirection 0x449F50). Which end looks down is measured: the
// depth axis of the camera matrix (0x760AC4, element [8] = downward component) must increase, otherwise the
// other end is used. The player's own tilt is restored when the table view stops (possession, menus).
// 0.6.9: the game can only point at what lies on its own screen, between its bars (top bar, statistics
// bar and panel). While the table is shown, the game camera's focal length (argument +0x30 of the projection
// setup 0x58A6F0, cdecl, verified: copied to 0x77F430, all screen projections derive from it) is scaled down on
// a COPY of the argument, so the whole table falls in the free middle of the game screen: every tile shown on
// the table can be pointed at. The table keeps the same part of the dungeon (the plugin scales the screen
// area it measures by the same factor), so nothing changes in the headset.
// 0.8.94.1: the game forwards unhandled window messages to the procedure it replaced with a DIRECT call (0x4305DD: mov ecx,[esp+44h] / push ecx /
// push edi / push ebp / call esi). Any procedure installed by another component in Unicode (or a handle) then crashes the game. The call is
// rerouted through CallWindowProcA (same arguments, stdcall, lParam already pushed), which accepts both pointers and handles.
struct SafeWindowCall {
    static void install(uintptr_t base) {
        if(iniInt(L"SafeWindowCall",1)==0) {log("Safe window call (0.8.94.1): off");return;}
        auto* at=reinterpret_cast<unsigned char*>(base-0x400000+0x4305dd);
        const unsigned char sig[9]={0x8b,0x4c,0x24,0x44,0x51,0x57,0x55,0xff,0xd6};
        if(memcmp(at,sig,9)) {log("Safe window call (0.8.94.1): signature unsupported, not installed");return;}
        FARPROC cwp=GetProcAddress(GetModuleHandleA("user32.dll"),"CallWindowProcA");if(!cwp) {log("Safe window call: CallWindowProcA not found");return;}
        auto* st=static_cast<unsigned char*>(VirtualAlloc(nullptr,32,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));if(!st) return;
        unsigned char* q=st;const uintptr_t back=base-0x400000+0x4305e6;
        *q++=0x51;*q++=0x57;*q++=0x55;*q++=0x56;                 // push ecx (wParam) / push edi (msg) / push ebp (hwnd) / push esi (previous procedure)
        *q++=0xb8;{uint32_t a=uint32_t(reinterpret_cast<uintptr_t>(cwp));memcpy(q,&a,4);q+=4;}   // mov eax,CallWindowProcA
        *q++=0xff;*q++=0xd0;                                     // call eax (stdcall, 5 arguments incl. lParam pushed by the game)
        *q++=0xe9;{int32_t r=int32_t(back-(reinterpret_cast<uintptr_t>(q)+4));memcpy(q,&r,4);q+=4;}
        FlushInstructionCache(GetCurrentProcess(),st,32);
        DWORD old=0;if(!VirtualProtect(at,9,PAGE_EXECUTE_READWRITE,&old)) {log("Safe window call: protection");return;}
        unsigned char j[9]={0x8b,0x4c,0x24,0x44,0xe9,0,0,0,0};int32_t r=int32_t(reinterpret_cast<uintptr_t>(st)-(reinterpret_cast<uintptr_t>(at)+9));memcpy(j+5,&r,4);
        memcpy(at,j,9);DWORD ig=0;VirtualProtect(at,9,old,&ig);FlushInstructionCache(GetCurrentProcess(),at,9);
        log("Safe window call (0.8.94.1): previous window procedure called through CallWindowProcA (0x4305DD)");
    }
};
// 0.8.94: GAME CINEMATICS ON THE TABLE (reference 0.8.92.1 + new approach; 0.8.93-0.8.93.2 are abandoned).
// Verified in DKII-DX.exe 1.70 (world render 0x598880, ebx = camera = g_pCBridge+0x3E5): the view handed to the view setup 0x575A00 is built
//   * camera mode 18 (cinematic): at 0x598E81 from the cinematic key frames (+0xEA2 matrices, +0xEA6 positions, +0xEAA/+0xEAE focal data,
//     +0xEB2 offset, +0xEC6 key, +0xECA blend) -> position, orientation AND focal length animated (the 0.8.93 hold of the camera fields could not act);
//   * every other mode: at 0x598CA0 from the player's camera fields (+0xD68 target, +0xD80 zoom, +0x4 orientation built from direction/tilt).
// The single branch 'cmp eax,0x12 / je 0x598E81' at 0x598C97 is redirected: while the table view runs, a cinematic frame takes the PLAYER'S path,
// so the dungeon keeps exactly the player's zoom, direction and tilt (flat, same size). Nothing of the game is written: the camera object, the
// cinematic script and its timing are untouched (only the image of the world differs). The place the cinematic looks at is computed from its key
// frames (position + forward axis hitting the ground plane of the player's target) and the player's view is moved sideways onto it (LensHack hook).
struct CinematicView {
    inline static bool enabled=false,follow=true,installed=false;
    inline static volatile LONG active=0,used=0,centred=0,kept=0,shifted=0,touched=0;
    inline static volatile LONG shiftReady=0;inline static float dx=0,dy=0;inline static bool haveShift=false;
    inline static volatile LONG takeCinematic=1;
    // 0.8.94.1: the 4 view flags (+0x50 bits 0-3, computed at 0x599040-0x59909D from the camera MODE by 0x44AA20 / 0x44AB60 / 0x44AC80 / 0x44AA80) still said
    // 'cinematic': bit 3 (and bit 1) make the tile selection 0x574B4D add the cave ceilings = roofs over the rooms (report 210345, captures 1 and 6).
    // A redirected frame gets the player's flags (mode 3: bit 2 only, = the engine's reset value 0x571990).
    inline static volatile LONG redirected=0,flagsFixed=0;
    __attribute__((force_align_arg_pointer)) static int __cdecl decide(unsigned char* cam) {
        shiftReady=0;redirected=0;
        if(!active||!cam||!readable(cam+0xd50,0x180)) {haveShift=false;return 1;}
        redirected=1;   // 0.8.94.3: table view = always the player's view flags (report 223726 capture 4: the camera transition into / out of possession
                        // sets the 'end time' +0xDD8 and the flags then follow the target mode 2 -> cave ceilings = roofs on the table)
        uint32_t m=0;memcpy(&m,cam+0xda8,4);
        if(m!=18) {haveShift=false;return 1;}
        InterlockedIncrement(&used);InterlockedExchange(&touched,1);redirected=1;
        if(follow&&lookAt(cam)) {InterlockedIncrement(&centred);shiftReady=1;}
        else if(follow&&haveShift) {InterlockedIncrement(&kept);shiftReady=1;}   // keep the last place shown when this frame gives no usable ground point
        return 0;
    }
    // ground point the cinematic camera looks at (view space of 0x575A00: x = X/4096, y = Y/4096, z = -Z/4096, z grows downwards)
    static bool lookAt(const unsigned char* cam) {
        int32_t key=0;float t=0,off[3];uint32_t pm=0,pp=0;
        memcpy(&key,cam+0xec6,4);memcpy(&t,cam+0xeca,4);memcpy(&pm,cam+0xea2,4);memcpy(&pp,cam+0xea6,4);memcpy(off,cam+0xeb2,12);
        if(key<0||key>100000||!std::isfinite(t)||!pm||!pp) return false;
        t=std::clamp(t,0.f,1.f);
        const unsigned char* P=reinterpret_cast<const unsigned char*>(uintptr_t(pp))+size_t(key)*12;const unsigned char* M=reinterpret_cast<const unsigned char*>(uintptr_t(pm))+size_t(key)*36;
        if(!readable(P,24)||!readable(M,72)) return false;
        float a[3],b[3],ma[9],mb[9];memcpy(a,P,12);memcpy(b,P+12,12);memcpy(ma,M,36);memcpy(mb,M+36,36);
        float c[3],f[3];
        for(int i=0;i<3;++i) {c[i]=a[i]+off[i]+(b[i]-a[i])*t;f[i]=ma[3*i+2]+(mb[3*i+2]-ma[3*i+2])*t;}   // forward = camera axis 2 (triplet j = world axis j)
        c[2]-=1.f;   // 0x599013: the cinematic view height is the key position minus 1
        const float n=std::sqrt(f[0]*f[0]+f[1]*f[1]+f[2]*f[2]);if(!(n>.5f)||!std::isfinite(c[0]+c[1]+c[2])) return false;
        for(float& v:f) v/=n;
        int32_t tg[3],sh[3];memcpy(tg,cam+0xd68,12);memcpy(sh,cam+0xd74,12);
        const float tx=float(tg[0]+sh[0])/4096.f,ty=float(tg[1]+sh[1])/4096.f,tz=-float(tg[2]+sh[2])/4096.f;
        if(!(f[2]>.2f)) return false;                // nearly level camera: no reliable ground point
        const float d=(tz-c[2])/f[2];if(!(d>0&&d<300)) return false;
        const float lx=c[0]+f[0]*d,ly=c[1]+f[1]*d;
        if(!std::isfinite(lx)||!std::isfinite(ly)||lx<-8||ly<-8||lx>400||ly>400) return false;
        dx=lx-tx;dy=ly-ty;haveShift=true;return true;
    }
    static void install(uintptr_t base) {
        enabled=iniInt(L"CinematicOnTable",1)!=0;follow=iniInt(L"CinematicFollow",1)!=0;
        if(!enabled) {log("Game cinematics on the table (0.8.94): off (CinematicOnTable=0)");return;}
        auto* at=reinterpret_cast<unsigned char*>(base-0x400000+0x598c97);
        const unsigned char sig[9]={0x83,0xf8,0x12,0x0f,0x84,0xe1,0x01,0x00,0x00};
        if(memcmp(at,sig,9)) {enabled=false;log("Game cinematics on the table (0.8.94): branch signature unsupported, off");return;}
        auto* st=static_cast<unsigned char*>(VirtualAlloc(nullptr,64,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));if(!st) {enabled=false;return;}
        const uintptr_t normal=base-0x400000+0x598ca0,cine=base-0x400000+0x598e81;
        unsigned char* q=st;
        auto rel=[&](uintptr_t to){int32_t r=int32_t(to-(reinterpret_cast<uintptr_t>(q)+4));memcpy(q,&r,4);q+=4;};
        *q++=0x60;                                   // pushad
        *q++=0x53;                                   // push ebx (camera)
        *q++=0xe8;rel(reinterpret_cast<uintptr_t>(&decide));
        *q++=0x83;*q++=0xc4;*q++=0x04;               // add esp,4
        *q++=0xa3;{uint32_t a=uint32_t(reinterpret_cast<uintptr_t>(&takeCinematic));memcpy(q,&a,4);q+=4;}   // mov [takeCinematic],eax
        *q++=0x61;                                   // popad
        *q++=0x83;*q++=0x3d;{uint32_t a=uint32_t(reinterpret_cast<uintptr_t>(&takeCinematic));memcpy(q,&a,4);q+=4;}*q++=0x00;   // cmp dword [takeCinematic],0
        *q++=0x0f;*q++=0x84;rel(normal);             // je normal path (player camera)
        *q++=0x83;*q++=0xf8;*q++=0x12;               // cmp eax,0x12 (original test)
        *q++=0x0f;*q++=0x84;rel(cine);               // je cinematic path
        *q++=0xe9;rel(normal);                       // jmp normal path
        FlushInstructionCache(GetCurrentProcess(),st,64);
        DWORD old=0;if(!VirtualProtect(at,9,PAGE_EXECUTE_READWRITE,&old)) {enabled=false;log("Game cinematics on the table (0.8.94): patch failed (protection)");return;}
        unsigned char j[9]={0xe9,0,0,0,0,0x90,0x90,0x90,0x90};int32_t r=int32_t(reinterpret_cast<uintptr_t>(st)-(reinterpret_cast<uintptr_t>(at)+5));memcpy(j+1,&r,4);
        memcpy(at,j,9);DWORD ig=0;VirtualProtect(at,9,old,&ig);FlushInstructionCache(GetCurrentProcess(),at,9);
        installed=true;
        log("Game cinematics on the table (0.8.94): installed at 0x598C97 - a cinematic keeps the player's camera (zoom, direction, tilt) on the table, %s",follow?"moved onto the place the cinematic shows (CinematicFollow=1)":"at the player's place (CinematicFollow=0)");
    }
};
struct LensHack {
    // 0.6.9.2: hooked one level higher, on the engine's view setup 0x575A00 (cdecl: view struct, float; verified:
    // copies +0x30 -> 0x760AE8 = focal used by the tile selection 0x574820 / screen picking 0x572920, then calls
    // the projection setup 0x58A6F0 with those globals). 0.6.9 scaled only 0x58A6F0: the tile selection kept the
    // unscaled focal with the shrunk rectangle -> tiles culled 3x too narrow = holes around the table.
    using Setup=int(__cdecl*)(void*,float);
    inline static Setup original=nullptr;inline static volatile float want=1.f,applied=1.f;inline static volatile LONG calls=0;
    // 0.8.84 360 VIEW = CUBE MAP (Possession360). SurroundHack (below) runs the game's whole world render 6 more times in ONE game frame; before each of these runs it sets
    // faceNext (1..6): the camera handed to the view setup is then a COPY with the orientation of that cube face (CubeFaces.hpp: level faces turned with the heading of the
    // game camera, front / right / back / left / up / down), a focal f and a SQUARE screen rectangle of half size f*guard about the original centre (a 90 degree face plus
    // a small guard band); the position, far distance and flags stay those of the game camera. The cube is built once per frame (face 1) from the game camera of that frame,
    // and Q / plane data are kept for the draws of the faces (Renderer::draw). The game's own camera object is never touched; the camera globals written by the view
    // setup (0x760AA0..0x760B90, 0x77F400..0x77F500, 0x78093C) are put back after each face, and the normal run comes last, unchanged.
    inline static volatile int faceNext=0,faceApplied=0;inline static CubeFaces cube{};inline static float cubeFocal=0,cubeRect[4]{};inline static bool cubeLensOk=false;
    inline static float cubeGuard=1.04f;
    inline static volatile bool cubeBuildNext=false;inline static CubeFaces cubeLast{};inline static bool cubeLastOk=false;   // 0.8.99.1: the cube is built on the FIRST face run of the frame (faces may be skipped); last valid cube kept for the face choice
    inline static uintptr_t gDelta=0;
    inline static bool gOk=false;   /* gDelta = load address - 0x400000 is 0 when the exe sits at its preferred base (the real case): 'is set' is gOk, never gDelta!=0 */ inline static volatile LONG auxCalls=0,auxRefused=0;
    inline static unsigned char savedA[0xF0],savedB[0x100],savedC[4];inline static bool savedValid=false;
    static void saveGlobals() {
        if(!gOk) return;
        memcpy(savedA,reinterpret_cast<void*>(gDelta+0x760aa0),sizeof(savedA));memcpy(savedB,reinterpret_cast<void*>(gDelta+0x77f400),sizeof(savedB));memcpy(savedC,reinterpret_cast<void*>(gDelta+0x78093c),4);
        savedValid=true;
    }
    static void restoreAux() {
        if(!savedValid||!gOk) return;
        memcpy(reinterpret_cast<void*>(gDelta+0x760aa0),savedA,sizeof(savedA));memcpy(reinterpret_cast<void*>(gDelta+0x77f400),savedB,sizeof(savedB));memcpy(reinterpret_cast<void*>(gDelta+0x78093c),savedC,4);
    }   // (the snapshot stays valid: it is put back before every view run)
    // game mode (1 / 2 = first-person possession) read like NativeCamera::mode(); the turned camera is only ever used in a possession frame
    static bool possessionNow() {
        if(!gOk) return false;
        uint32_t fe=0,b=0;memcpy(&fe,reinterpret_cast<void*>(gDelta+0x75aeaf),4);if(fe) return false;
        memcpy(&b,reinterpret_cast<void*>(gDelta+0x795700),4);if(!b) return false;
        auto* c=reinterpret_cast<unsigned char*>(uintptr_t(b)+0x3e5);if(!readable(c+0xd50,0x5c)) return false;
        uint32_t m=0;memcpy(&m,c+0xda8,4);return m==1||m==2;
    }
    static int __cdecl hook(unsigned char* p0,float extra) {
        InterlockedIncrement(&calls);
        unsigned char* p=p0;unsigned char moved[0x60];
        // 0.8.94: during a game cinematic the world render took the player's normal camera (CinematicView); the view is moved sideways onto the place
        // the cinematic looks at (same height, orientation and focal length: only the position follows the game).
        if(CinematicView::redirected&&p0&&readable(p0,0x54)&&faceNext==0) {
            CinematicView::redirected=0;
            memcpy(moved,p0,0x54);memset(moved+0x54,0,0xc);
            uint32_t fl=0;memcpy(&fl,moved+0x50,4);fl=(fl&~0xfu)|0x4u;memcpy(moved+0x50,&fl,4);InterlockedIncrement(&CinematicView::flagsFixed);
            if(CinematicView::shiftReady) {
                float pos[2];memcpy(pos,moved,8);pos[0]+=CinematicView::dx;pos[1]+=CinematicView::dy;
                if(std::isfinite(pos[0])&&std::isfinite(pos[1])) {memcpy(moved,pos,8);InterlockedIncrement(&CinematicView::shifted);}
            }
            CinematicView::shiftReady=0;p=moved;
        }
        const float k=want;const int fc=faceNext;faceNext=0;
        if(fc>=1&&fc<=6&&gOk&&p&&readable(p,0x54)&&possessionNow()) {
            // 0.8.84: one face of the cube map
            if(cubeBuildNext) {   // 0.8.99.1: was 'fc==1' (face 1 always ran first)
                cubeBuildNext=false;
                float A0[9],rect[4];memcpy(A0,p+0xc,36);memcpy(rect,p+0x34,16);
                buildCubeFaces(A0,cube);
                cubeLensOk=cubeFaceLens(rect,cubeGuard,cubeFocal,cubeRect);
                if(cube.valid) {cubeLast=cube;cubeLastOk=true;}
            }
            if(cube.valid&&cubeLensOk) {
                unsigned char copy[0x60];memcpy(copy,p,0x54);memset(copy+0x54,0,0xc);
                memcpy(copy+0xc,cube.A[fc-1],36);memcpy(copy+0x30,&cubeFocal,4);memcpy(copy+0x34,cubeRect,16);
                applied=1.f;lodThresholds(1.f);faceApplied=fc;InterlockedIncrement(&auxCalls);
                return original(copy,extra);
            }
            InterlockedIncrement(&auxRefused);
        }
        if(k<.999f&&p&&readable(p,0x54)) {
            unsigned char copy[0x60];memcpy(copy,p,0x54);memset(copy+0x54,0,0xc);
            float f;memcpy(&f,copy+0x30,4);
            if(std::isfinite(f)&&f>1) {
                f*=k;memcpy(copy+0x30,&f,4);applied=k;lodThresholds(k);
                return original(copy,extra);
            }
        }
        applied=1.f;lodThresholds(1.f);return original(p,extra);
    }
    // 0.6.9.5: texture / model detail. The engine picks one of 4 detail levels per mesh from
    // ratio = focal(0x760AE8) * size / depth / textureSize, compared with 1.0 / 0.5 / 0.25 (doubles 0x66FC00/08/10)
    // at 7 places (0x57F050, 0x57F92F, 0x580017, 0x5815DA, 0x585024, 0x58630B, 0x5871D6; verified in the 1.70 exe).
    // With the focal scaled by k every ratio is k times smaller -> lower-resolution textures (0.6.9.2-0.6.9.4 blur).
    // These 21 compares are redirected to our own thresholds k, k/2, k/4: same choice as the unscaled camera.
    inline static double thresholds[3]={1.0,.5,.25};
    // 0.8.28: ForceHighDetail=1 (default): select the best of these 4 texture/detail levels.
    // 0.8.32: the user's mannequin screenshots still show distance-dependent loss
    // despite all sites below being active; another native path remains unidentified.
    // (ratio = focal*size/depth/texture size > 0). The mesh reduction level (global 0x7932dc = number of steps down the chain of reduced meshes,
    // read at 0x5948FC: 'mov ecx,[0x7932dc]') is forced to 0 = full mesh every frame (see forceMeshDetail).
    inline static bool forceHigh=false,meshDetailOk=false;inline static int detailMode=1;
    static void lodThresholds(float k) {if(forceHigh) {thresholds[0]=thresholds[1]=thresholds[2]=0;return;}thresholds[0]=k;thresholds[1]=k*.5;thresholds[2]=k*.25;}
    static void forceMeshDetail(uintptr_t base,bool geometry=true) {
        // 0.8.29: real progressive geometry LOD is model+0x7c, not 0x7932dc (texture reduction).
        // The engine first sets it to zero at 0x5849f8. Skip only the distance/focal calculation,
        // before the unchanged renderer reads the per-face detail table at +4+LOD.
        unsigned char* lod=reinterpret_cast<unsigned char*>(base-0x400000+0x5849ff);
        const unsigned char lodSig[]={0x0f,0x85,0xab,0,0,0,0xd9,0x44,0x24,0x10};
        if(!geometry) log("Geometry LOD: native engine LOD kept (ForceHighDetail=2, diagnostic mode 0.8.53)");
        else if(!memcmp(lod,lodSig,sizeof(lodSig))) {
            DWORD old=0;
            if(VirtualProtect(lod,6,PAGE_EXECUTE_READWRITE,&old)) {
                const unsigned char jump[]={0xe9,0xac,0,0,0,0x90};memcpy(lod,jump,6);
                DWORD ignored=0;VirtualProtect(lod,6,old,&ignored);FlushInstructionCache(GetCurrentProcess(),lod,6);
                log("Geometry LOD: model+0x7c locked to 0 (maximum faces), distance calculation bypassed at 5849FF");
            } else log("Geometry LOD: patch failed (protection)");
        } else log("Geometry LOD: signature unsupported, geometry left unchanged");
        // 0.9.7 (user report 221111: training dummies simplified when zoomed out, the 'mannequin LOD' left open since 0.8.32): the ANIMATED KMF models
        // (renderer 0x580EC0) have their own copy of the same distance calculation, writing model+0x70 (set to 0 at 0x580FB8 just before).
        // Same bytes as 5849FF (verified in the 1.70 exe): the calculation is skipped, +0x70 stays 0 = complete mesh.
        if(geometry) {
            unsigned char* alod=reinterpret_cast<unsigned char*>(base-0x400000+0x580fbf);
            if(!memcmp(alod,lodSig,sizeof(lodSig))) {
                DWORD old=0;
                if(VirtualProtect(alod,6,PAGE_EXECUTE_READWRITE,&old)) {
                    const unsigned char jump[]={0xe9,0xac,0,0,0,0x90};memcpy(alod,jump,6);
                    DWORD ignored=0;VirtualProtect(alod,6,old,&ignored);FlushInstructionCache(GetCurrentProcess(),alod,6);
                    log("Geometry LOD (0.9.7): animated models (+0x70) locked to 0 (maximum faces), distance calculation bypassed at 580FBF");
                } else log("Geometry LOD (0.9.7): animated models: patch failed (protection)");
            } else log("Geometry LOD (0.9.7): animated models: signature unsupported, left unchanged");
        }
        // Some meshes enter the geometry renderers with their LOD already chosen.
        // Each native read below indexes the per-group faces/vertex table. KMF
        // level 0 is the complete mesh; force that index at the actual readers.
        struct Site {uint32_t va;unsigned char old[3],replacement[3];};
        static const Site sites[]={
            {0x583710,{0x8b,0x4f,0x7c},{0x31,0xc9,0x90}},
            {0x5837dc,{0x8b,0x4f,0x7c},{0x31,0xc9,0x90}},
            // The end-of-group face count also reads the LOD table. 0.8.30
            // missed this site, so distant meshes could still lose whole parts.
            {0x583c6a,{0x8b,0x51,0x7c},{0x31,0xd2,0x90}},
            {0x583e2c,{0x8b,0x4e,0x7c},{0x31,0xc9,0x90}},
            {0x583f05,{0x8b,0x4e,0x7c},{0x31,0xc9,0x90}},
            {0x58486f,{0x8b,0x4a,0x7c},{0x31,0xc9,0x90}},
            {0x584ced,{0x8b,0x4e,0x7c},{0x31,0xc9,0x90}},
            {0x584e75,{0x8b,0x56,0x7c},{0x31,0xd2,0x90}},
            {0x58508e,{0x8b,0x4e,0x7c},{0x31,0xc9,0x90}},
            {0x58520f,{0x8b,0x4e,0x7c},{0x31,0xc9,0x90}},
            {0x5855f8,{0x8b,0x5e,0x7c},{0x31,0xdb,0x90}},
            {0x585d00,{0x8b,0x47,0x7c},{0x31,0xc0,0x90}}
        };
        bool readersOk=geometry;
        if(geometry) for(const auto& s:sites) if(memcmp(reinterpret_cast<void*>(base-0x400000+s.va),s.old,3)) {
            log("Geometry LOD: reader %06X signature unsupported, reader patch skipped",unsigned(s.va));readersOk=false;break;
        }
        unsigned patched=0;
        if(readersOk) for(const auto& s:sites) {
            auto* at=reinterpret_cast<unsigned char*>(base-0x400000+s.va);DWORD old=0;
            if(!VirtualProtect(at,3,PAGE_EXECUTE_READWRITE,&old))break;
            memcpy(at,s.replacement,3);DWORD ignored=0;VirtualProtect(at,3,old,&ignored);
            FlushInstructionCache(GetCurrentProcess(),at,3);++patched;
        }
        if(readersOk) log("Geometry LOD: %u/%u native face-table readers forced to level 0 (complete KMF mesh)",patched,unsigned(sizeof(sites)/sizeof(sites[0])));
        const unsigned char* q=reinterpret_cast<const unsigned char*>(base-0x400000+0x5948fc);
        static const unsigned char sig[6]={0x8b,0x0d,0xdc,0x32,0x79,0x00}; // mov ecx,[0x7932DC] (verified in the 1.70 exe)
        uint32_t a;memcpy(&a,q+2,4);
        if(memcmp(q,sig,2)==0&&a==uint32_t(base-0x400000+0x7932dc)) {meshDetailOk=true;log("Detail: texture reduction forced to 0; texture levels forced to the best (ForceHighDetail=1)");}
        else log("Detail: mesh reduction site unsupported (exe version?), only the texture levels are forced");
    }
    static void applyMeshDetail(uintptr_t base) {if(meshDetailOk) *reinterpret_cast<volatile int32_t*>(base-0x400000+0x7932dc)=0;}
    static void installLod(uintptr_t base) {
        // 0.6.9.6: exact addresses (0.6.9.5 assumed the same spacing everywhere: 0x5815DA differs -> fix refused).
        const uint32_t addr[7][3]={{0x57f050,0x57f062,0x57f074},{0x57f92f,0x57f941,0x57f953},{0x580017,0x580029,0x58003b},
            {0x5815da,0x5815e9,0x5815fb},{0x585024,0x585033,0x585045},{0x58630b,0x58631d,0x58632f},{0x5871d6,0x5871e8,0x5871fa}};
        const uint32_t consts[3]={0x66fc00,0x66fc08,0x66fc10};
        // Every site checked first; nothing patched unless all 21 instructions are exactly as expected.
        for(auto& s:addr) for(int i=0;i<3;++i) {
            const unsigned char* q=reinterpret_cast<const unsigned char*>(base-0x400000+s[i]);
            uint32_t d;memcpy(&d,q+2,4);
            if(q[0]!=0xdc||q[1]!=(i==2?0x1d:0x15)||d!=uint32_t(base-0x400000+consts[i])) {log("Texture detail: site %06X unsupported, detail fix off",unsigned(s[i]));return;}
        }
        for(auto& s:addr) for(int i=0;i<3;++i) {
            unsigned char* q=reinterpret_cast<unsigned char*>(base-0x400000+s[i]);
            DWORD old=0;if(!VirtualProtect(q+2,4,PAGE_EXECUTE_READWRITE,&old)) continue;
            uint32_t d=uint32_t(reinterpret_cast<uintptr_t>(&thresholds[i]));memcpy(q+2,&d,4);
            DWORD ig=0;VirtualProtect(q+2,4,old,&ig);FlushInstructionCache(GetCurrentProcess(),q,6);
        }
        log("Texture detail: 21 level compares follow the focal factor (textures as sharp as with the normal camera)");
    }
    static void install(uintptr_t base) {
        if(iniInt(L"TablePickAll",1)==0) {log("Table picking: whole-table picking off (TablePickAll=0)");return;}
        auto* entry=reinterpret_cast<unsigned char*>(base+0x175a00);
        gDelta=base-0x400000;gOk=true;
        const unsigned char sig[]={0x8b,0x44,0x24,0x04,0x53,0x56,0x57};
        if(memcmp(entry,sig,7)) {log("Table picking: view setup signature unsupported");return;}
        auto* t=static_cast<unsigned char*>(VirtualAlloc(nullptr,16,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));if(!t) return;
        memcpy(t,entry,7);t[7]=0xe9;int32_t back=int32_t(reinterpret_cast<uintptr_t>(entry+7)-reinterpret_cast<uintptr_t>(t+12));memcpy(t+8,&back,4);
        DWORD old=0;VirtualProtect(t,16,PAGE_EXECUTE_READ,&old);original=reinterpret_cast<Setup>(t);
        if(!VirtualProtect(entry,7,PAGE_EXECUTE_READWRITE,&old)) {original=nullptr;return;}
        unsigned char jump[7]={0xe9,0,0,0,0,0x90,0x90};int32_t rel=int32_t(reinterpret_cast<uintptr_t>(&hook)-reinterpret_cast<uintptr_t>(entry+5));memcpy(jump+1,&rel,4);
        memcpy(entry,jump,7);DWORD ig=0;VirtualProtect(entry,7,old,&ig);
        FlushInstructionCache(GetCurrentProcess(),entry,7);FlushInstructionCache(GetCurrentProcess(),t,16);
        log("Table picking: game camera focal scaled while the table is shown (view setup 0x575A00 hooked: projection + tile selection)");
        detailMode=iniInt(L"ForceHighDetail",1);
        forceHigh=detailMode!=0;
        log("Detail mode (0.8.53): ForceHighDetail=%d (1 = textures + complete geometry [reference 0.8.41], 2 = best textures but native geometry LOD, 0 = native engine detail)",detailMode);
        if(forceHigh) {lodThresholds(1.f);forceMeshDetail(base,detailMode!=2);}
        if(iniInt(L"TextureDetailFix",1)!=0||forceHigh) installLod(base);
    }
    // Screen area of the unscaled camera expressed in the scaled camera (same ground footprint).
    static ScreenRect shrink(ScreenRect a,const Projection& lens) {
        const float k=applied;if(k>=.999f) return a;
        const float cx=lens.centerX+.5f,cy=lens.centerY+.5f;
        return {cx+(a.left-cx)*k,cy+(a.top-cy)*k,cx+(a.right-cx)*k,cy+(a.bottom-cy)*k};
    }
};
// 0.6.9.8: the game's render memory pool (allocator 0x57CD30; grows by 2 MB blocks in 0x57CF47..0x57D07F) holds at
// most 32 blocks (block headers 0x765B18[32], raw pointers 0x765DA0[32]); the 33rd block calls exit(-1) at 0x57CFBD
// ('GAME EXIT: ExitProcess(-1)' from 0x57CFC4 in the 0.6.9.7 report: zoomed-out table view, more to draw).
// Both arrays move to 128-entry arrays of the plugin (21 references, the only ones in the exe, verified) and the
// limit becomes 127 blocks (254 MB). Current contents are copied first; nothing is patched unless every byte matches.
struct PoolGrow {
    inline static uint32_t headers[129]{},raws[129]{};
    static void install(uintptr_t base) {
        struct Ref {uint32_t at;uint32_t off;uint32_t value;};
        static const Ref refs[21]={{0x57cc46,1,0x765da0},{0x57cc60,1,0x765b18},{0x57cc68,2,0x765b18},{0x57cc78,1,0x765b18},{0x57cc82,2,0x765b18},
            {0x57cc8b,1,0x765b18},{0x57cc93,2,0x765b18},{0x57cca3,2,0x765b18},{0x57ccbb,2,0x765b18},{0x57cd03,1,0x765da0},{0x57cf57,1,0x765da0},
            {0x57cf61,3,0x765da0},{0x57cfcd,3,0x765da0},{0x57cfdb,3,0x765b18},{0x57cfef,3,0x765b18},{0x57cfff,3,0x765b18},{0x57d00e,3,0x765b18},
            {0x57d01e,3,0x765b18},{0x57d02f,3,0x765b18},{0x57d046,3,0x765b18},{0x57d054,3,0x765b18}};
        const uintptr_t k=base-0x400000;
        auto* limit=reinterpret_cast<unsigned char*>(k+0x57cfb4);
        if(limit[0]!=0x83||limit[1]!=0xfa||limit[2]!=0x20) {log("Render memory: limit check unsupported, pool left at 64 MB");return;}
        for(auto& r:refs) {uint32_t v;memcpy(&v,reinterpret_cast<void*>(k+r.at+r.off),4);if(v!=uint32_t(k+r.value)) {log("Render memory: reference %06X unsupported, pool left at 64 MB",unsigned(r.at));return;}}
        memcpy(headers,reinterpret_cast<void*>(k+0x765b18),32*4);memcpy(raws,reinterpret_cast<void*>(k+0x765da0),32*4);
        for(auto& r:refs) {
            auto* q=reinterpret_cast<unsigned char*>(k+r.at+r.off);
            const uint32_t v=uint32_t(reinterpret_cast<uintptr_t>(r.value==0x765b18?headers:raws));
            DWORD old=0;if(!VirtualProtect(q,4,PAGE_EXECUTE_READWRITE,&old)) continue;
            memcpy(q,&v,4);DWORD ig=0;VirtualProtect(q,4,old,&ig);
        }
        DWORD old=0;if(VirtualProtect(limit+2,1,PAGE_EXECUTE_READWRITE,&old)) {limit[2]=0x7f;DWORD ig=0;VirtualProtect(limit+2,1,old,&ig);}
        FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(k+0x57cc00),0x500);
        log("Render memory: pool limit 32 -> 127 blocks of 2 MB (%lu in use)",(unsigned long)*reinterpret_cast<uint32_t*>(k+0x765b08));
    }
    static uint32_t inUse(uintptr_t base) {return *reinterpret_cast<uint32_t*>(base-0x400000+0x765b08);}
    // 0.8.94.6: the pool is watched live. Above 'ceiling' blocks everything the mod ADDS to the render list (view expansion, 360 cube map) stands
    // down until the pool falls back under 'release'; the game's own rendering is never touched. Report 20261004_160911: leaving a possession the
    // table view came back with the zoom-out range extended and the player-side expansion x8.70; the pool went 1 -> over 127 blocks and the engine
    // called exit(-1) at 0x57CFC4 ('GAME EXIT: ExitProcess(-1)' ... DKII-DX.exe+0x17CFC4). The limit is 127 blocks of 2 MB (254 MB).
    // 0.9.47.17: the guard works from a REFERENCE level instead of a fixed ceiling (src/common/PoolGuard.hpp, tests/PoolGuardTests.cpp). The engine never gives a
    // block back, so after one very wide table view (report 20261008_220935: 1 -> 44 blocks when the zoom-out reached its limit) the old fixed ceiling of 24 held the
    // 360 view and the view expansion back for the rest of the session. A plateau that stays unchanged for 8 s (80 blocks at most) now becomes the new reference;
    // growth of 23 blocks above it trips the guard again; above 96 blocks nothing is ever accepted.
    inline static uintptr_t watchBase=0;inline static poolguard::Guard guard{};inline static volatile LONG tripped=0;inline static uint32_t ceiling=40,release=24;inline static volatile LONG trips=0,peak=0;
    inline static int guardLogs=0;
    static void arm(uintptr_t base) {watchBase=base;ceiling=uint32_t(std::clamp(iniInt(L"RenderMemoryCeiling",24),8,120));release=std::max(4u,ceiling/2u);guard.ceiling=ceiling;}
    static bool safe() {   // false while the pool is too full: the mod must not enlarge anything
        if(!watchBase) return true;
        const uint32_t n=inUse(watchBase);
        const bool ok=guard.safe(n,uint64_t(GetTickCount64()));
        if(LONG(n)>peak) peak=LONG(n);
        tripped=guard.tripped?1:0;trips=LONG(guard.trips);
        if(guard.justTripped&&guardLogs<12) {++guardLogs;log("Render memory guard (0.9.47.17): %u blocks of 2 MB reach the trip level %u (reference %u): the view expansion and the 360 view stand down until the pool stays unchanged for 8 s",unsigned(n),unsigned(guard.tripLevel()),unsigned(guard.reference));}
        if(guard.justRebased&&guardLogs<12) {++guardLogs;log("Render memory guard (0.9.47.17): the pool stayed at %u blocks of 2 MB for 8 s: accepted as the new reference (trip level %u, 360 stop level %u, hard cap %u); the view expansion and the 360 view resume",unsigned(n),unsigned(guard.tripLevel()),unsigned(guard.offLevel()),unsigned(guard.hardCap));}
        return ok;
    }
};
// 0.6.9.7: game pause state. 0x527270 (thiscall, 1 arg: 1 = pause, 0 = resume; verified in the 1.70 exe) sets
// +0x284 (paused) of the game object; called by the interface pause command (options screen) and on resume.
// The options panel follows this flag instead of guessing from the draws (0.6.9.5/0.6.9.6: stayed open).
// 0.8.97: STEADY POSSESSION CAMERA - the creature's own first-person movement animation is switched off in the game itself, the same way for
// every creature (user: 'se deplacer comme n'importe quel jeu FPS, sans que la camera tangue, ni haut/bas ni gauche/droite').
// Found in DKII-DX.exe 1.70 (disassembly, verified byte for byte below): the possession control function 0x40E910 (this = player, possessed creature
// = [player+0x134], keys 0x10-0x13 = forward / back / strafe) reads three values of the creature type (stats object [creature+0x370], filled at
// 0x493C5B-0x493CFA from the Creatures.kwd record: OpenKeeper names firstPersonWaddleScale (record +0xDBC -> +0x863), firstPersonOscillateScale
// (+0xDC0 -> +0x867) and the flag CAMERA_ROLLS_WHEN_TURNING 0x10000000 (+0xEFC -> +0x686)):
//   0x40EC4C  waddle x sin(walk cycle) added to the sideways movement          -> scale read as 0 (side-to-side sway gone)
//   0x40F133 / 0x40F1D7  oscillate x sin(walk cycle) added to the height sent  -> scale read as 0 (up / down bob gone, both branches)
//   0x40F290  waddle x sin(walk cycle) sent as command 0x80 (wobble)           -> 0
//   0x40F5C4  'camera rolls when turning' (command 0x7F)                       -> never sent (je -> jmp)
// Movement speed, turning, attacks and the footstep sounds (timed by the walk cycle itself, not by these scales) are unchanged. The Creatures.kwd
// file is never touched: only these 5 instructions in memory, and only if all of them are exactly the expected bytes. INI PossessionSteadyCamera=0
// = the game's own animation.
struct PossessionSteady {
    static inline bool installed=false;
    static void install(uintptr_t base) {
        if(iniInt(L"PossessionSteadyCamera",1)==0) {log("Possession steady camera (0.8.97): off (PossessionSteadyCamera=0), the creatures keep their own camera animation");return;}
        struct Site {uint32_t va;unsigned char expect[10];unsigned char repl[10];int n;int check;const char* what;};
        static const Site sites[5]={
            {0x40ec4c,{0x8b,0x88,0x63,0x08,0x00,0x00},{0x31,0xc9,0x90,0x90,0x90,0x90},6,6,"sideways sway"},
            {0x40f133,{0x8b,0x88,0x67,0x08,0x00,0x00},{0x31,0xc9,0x90,0x90,0x90,0x90},6,6,"head bob (moving)"},
            {0x40f1d7,{0x0f,0xaf,0x81,0x67,0x08,0x00,0x00},{0x31,0xc0,0x90,0x90,0x90,0x90,0x90},7,7,"head bob (other branch)"},
            {0x40f290,{0x0f,0xaf,0x81,0x63,0x08,0x00,0x00},{0x31,0xc0,0x90,0x90,0x90,0x90,0x90},7,7,"wobble"},
            // the 'je' after 'test dword [eax+0x686],0x10000000' (10 bytes checked: test + je)
            {0x40f5ba,{0xf7,0x80,0x86,0x06,0x00,0x00,0x00,0x00,0x00,0x10},{0},0,10,"roll when turning"}};
        for(const auto& t:sites) {
            const unsigned char* q=reinterpret_cast<const unsigned char*>(base-0x400000+t.va);
            if(IsBadReadPtr(q,12)||memcmp(q,t.expect,size_t(t.check))) {log("Possession steady camera (0.8.97): NOT installed - code at %06X (%s) is not the expected one (game version?)",unsigned(t.va),t.what);return;}
        }
        const unsigned char* je=reinterpret_cast<const unsigned char*>(base-0x400000+0x40f5c4);
        if(IsBadReadPtr(je,2)||je[0]!=0x74||je[1]!=0x30) {log("Possession steady camera (0.8.97): NOT installed - code at 40F5C4 (roll when turning) is not the expected one");return;}
        int done=0;
        auto write=[&](uint32_t va,const unsigned char* b,int n){unsigned char* q=reinterpret_cast<unsigned char*>(base-0x400000+va);DWORD old=0;
            if(!VirtualProtect(q,size_t(n),PAGE_EXECUTE_READWRITE,&old)) {return;}
            memcpy(q,b,size_t(n));DWORD ig=0;VirtualProtect(q,size_t(n),old,&ig);FlushInstructionCache(GetCurrentProcess(),q,size_t(n));++done;};
        for(const auto& t:sites) if(t.n) write(t.va,t.repl,t.n);
        const unsigned char jmp[1]={0xeb};write(0x40f5c4,jmp,1);
        installed=done==5;
        log("Possession steady camera (0.8.97): %s - %d/5 instructions changed in the possession control (0x40E910): no sway, no head bob, no wobble, no roll when turning, for every creature",installed?"ON":"PARTIAL",done);
    }
};
struct PauseWatch {
    using Fn=void(__fastcall*)(void*,void*,int);
    inline static Fn original=nullptr;inline static volatile LONG paused=-1;inline static volatile LONG changes=0;inline static void* volatile session=nullptr;
    static void __fastcall hook(void* self,void* edx,int on) {original(self,edx,on);session=self;paused=on?1:0;InterlockedIncrement(&changes);}
    static void install(uintptr_t base) {
        auto* entry=reinterpret_cast<unsigned char*>(base+0x127270);
        const unsigned char sig[]={0x8b,0x44,0x24,0x04,0x85,0xc0,0x8b,0x81,0x84,0x02,0x00,0x00};
        if(memcmp(entry,sig,sizeof(sig))) {log("Pause watch: signature unsupported (options panel from the draws only)");return;}
        auto* t=static_cast<unsigned char*>(VirtualAlloc(nullptr,16,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));if(!t) return;
        memcpy(t,entry,6);t[6]=0xe9;int32_t back=int32_t(reinterpret_cast<uintptr_t>(entry+6)-reinterpret_cast<uintptr_t>(t+11));memcpy(t+7,&back,4);
        DWORD old=0;VirtualProtect(t,16,PAGE_EXECUTE_READ,&old);original=reinterpret_cast<Fn>(t);
        if(!VirtualProtect(entry,6,PAGE_EXECUTE_READWRITE,&old)) {original=nullptr;return;}
        unsigned char jump[6]={0xe9,0,0,0,0,0x90};int32_t rel=int32_t(reinterpret_cast<uintptr_t>(&hook)-reinterpret_cast<uintptr_t>(entry+5));memcpy(jump+1,&rel,4);
        memcpy(entry,jump,6);DWORD ig=0;VirtualProtect(entry,6,old,&ig);FlushInstructionCache(GetCurrentProcess(),entry,6);
        log("Pause watch: game pause function 0x527270 hooked");
    }
};
// 0.8.94.21: the game's TEXT BOX (notification 'i' boxes, tutorial messages). Verified in DKII-DX.exe 1.70: the GUI sprite table (0x6A7DF4, records of
// 0x45 bytes: id, name) holds 'gui\\windows\\text_box-00..02' (ids 0x10..0x12, background) and the 9-slice frame 'text_box-tl/t/tr/l/m/r/bl/b/br' (ids
// 0x13..0x1B). Every one of those sprites is drawn by ONE function, 0x411B30 (thiscall, 5 stack arguments, ret 0x14): rectangle left, top, right,
// bottom (screen pixels: the frame is placed at left+16 with 16 / 32 px tiles) and a style. Its 4 call sites are the GUI widgets that have a frame.
// The mod reads that rectangle each frame: a box is open exactly when the game draws one, and its rectangle is the game's own (no pixel guessing).
struct TextBoxWatch {
    using Fn=void(__fastcall*)(void*,void*,int,int,int,int,int);
    inline static Fn original=nullptr;inline static bool installed=false;
    struct R {int l,t,r,b;};
    inline static R cur[8]{},prev[8]{};inline static int nCur=0,nPrev=0;inline static uint64_t calls=0;inline static int logs=0;
    static void __fastcall hook(void* self,void* edx,int l,int t,int r,int b,int style) {
        ++calls;
        if(nCur<8) cur[nCur++]={l,t,r,b};
        if(logs<20) {++logs;log("Text box (0.8.94.21): the game draws a framed text box %d,%d - %d,%d (style %d)",l,t,r,b,style);}
        original(self,edx,l,t,r,b,style);
    }
    static void endFrame() {memcpy(prev,cur,sizeof(cur));nPrev=nCur;nCur=0;}
    static void install(uintptr_t base) {
        auto* entry=reinterpret_cast<unsigned char*>(base+0x11b30);
        const unsigned char sig[]={0x6a,0xff,0x68,0x2b,0x94,0x64,0x00,0x64,0xa1,0x00,0x00,0x00,0x00};
        if(!readable(entry,sizeof(sig))||memcmp(entry,sig,sizeof(sig))) {log("Text box watch: signature unsupported (boxes found from the draws only)");return;}
        auto* t=static_cast<unsigned char*>(VirtualAlloc(nullptr,32,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));if(!t) return;
        memcpy(t,entry,7);t[7]=0xe9;int32_t back=int32_t(reinterpret_cast<uintptr_t>(entry+7)-reinterpret_cast<uintptr_t>(t+12));memcpy(t+8,&back,4);
        DWORD old=0;VirtualProtect(t,32,PAGE_EXECUTE_READ,&old);original=reinterpret_cast<Fn>(t);
        if(!VirtualProtect(entry,7,PAGE_EXECUTE_READWRITE,&old)) {original=nullptr;return;}
        unsigned char jump[7]={0xe9,0,0,0,0,0x90,0x90};int32_t rel=int32_t(reinterpret_cast<uintptr_t>(&hook)-reinterpret_cast<uintptr_t>(entry+5));memcpy(jump+1,&rel,4);
        memcpy(entry,jump,7);DWORD ig=0;VirtualProtect(entry,7,old,&ig);FlushInstructionCache(GetCurrentProcess(),entry,7);
        installed=true;log("Text box watch: the game's text box function 0x411B30 hooked (notification boxes read from the game)");
    }
    // a real notification box (tooltips are small): at least 20 % of the width and 6 % of the height of the interface
    static bool big(const R& q,float W,float H) {return float(q.r-q.l)>=.20f*W&&float(q.b-q.t)>=.06f*H;}
    static bool inside(float x,float y,float W,float H,R* out=nullptr) {
        for(int k=0;k<nCur+nPrev;++k) {const R& q=k<nCur?cur[k]:prev[k-nCur];if(!big(q,W,H)) continue;
            if(x>=q.l-2&&x<=q.r+2&&y>=q.t-2&&y<=q.b+2) {if(out) *out=q;return true;}}
        return false;
    }
    static bool any(float W,float H,R& u) {   // union of this frame's big boxes
        bool f=false;for(int k=0;k<nCur;++k) {const R& q=cur[k];if(!big(q,W,H)) continue;
            if(!f) {u=q;f=true;} else {u.l=std::min(u.l,q.l);u.t=std::min(u.t,q.t);u.r=std::max(u.r,q.r);u.b=std::max(u.b,q.b);}}
        return f;
    }
};
// 0.8.0: ORIGIN of every 3D piece, read inside the game engine (no screen position / depth guess any more).
// Verified in the 1.70 exe: every engine primitive (CEngine2D* interface sprites/meshes and CEngine* world meshes,
// sprites, height fields) starts with the setup 0x58A970 (cdecl, 6 arguments, 16 call sites, all inside the primitive
// render methods); every 3D vertex is projected by 0x58AF70 or 0x58ADC0 (cdecl: pool index, camera-space point) into
// the vertex pool 0x76F358 (40 bytes: +0x0C..+0x18 screen x, y, z, rhw copied verbatim into the Direct3D vertex by the
// triangle writers, e.g. 0x58B574; +0x1C clip code). The 2D interface classes project with 0x58B190 (rhw = 1: flat).
// The game sets up ONE camera per frame (0x575A00, 1 call per frame in the 0.7.10 log): the interface pieces drawn in
// 3D use the world camera, and the Direct3D draws come out sorted together with the dungeon (0.7.10 probe: interface
// pieces between dungeon draws) -> neither screen position, depth nor order can separate them (0.6.9.x-0.7.10 loop).
// What differs is WHO renders them. At 0x58A970 the caller chain (return addresses on the stack) identifies the
// origin (id 1..252). The vertices of INTERFACE origins get marker 0x9 + id in the 12 low mantissa bits of their rhw
// (relative change < 5e-4, invisible); dungeon vertices are left untouched. At the Direct3D draw the mark of each
// vertex says interface or dungeon, triangle by triangle. Automatic decision (no address list to maintain): a chain frame is an 'interface root' when it renders
// at least half of the flat 2D interface pieces and at most 10 % of the 3D pieces; a 'world root' renders at least half
// of the 3D pieces and at most 30 % of the 2D ones. A 3D origin is interface when its chain holds an interface root and
// no world root. Every doubt (untagged vertex, unknown origin, too many origins) = dungeon: never the other way.
constexpr int OriginChain=32;
struct OriginRecord {uint32_t ret0=0;uint32_t chain[OriginChain]{};uint8_t n=0;bool is2D=false,ui=false,cand=false;uint32_t prims=0,lastPrims=0;uint64_t total=0;uint32_t accDraws=0,accEdge=0,winDraws=0,winEdge=0;
    uint32_t draws=0,tris=0,uiTris=0;float box[4]={1e9f,1e9f,-1e9f,-1e9f};};
struct OriginSig {uint32_t hash=0;uint8_t id=0;};
struct OriginCache {uint32_t ret0=0,sp=0;uint8_t n=0,id=0;uint16_t off[OriginChain]{};uint32_t val[OriginChain]{};};
struct EngineOrigin {
    // ids 1..252: recorded origins; 253 / 254: origin table full, decided on the fly (interface / dungeon).
    static constexpr int MaxChain=OriginChain,MaxIds=253,Pool=1024,FullCand=253,FullWorld=254,SigFrames=6;
    using Origin=OriginRecord;using Sig=OriginSig;using Cache=OriginCache;
    using Proj=uint32_t(__cdecl*)(int,const float*);
    inline static Origin origins[256];inline static Sig table[1024];inline static Cache cache[128];inline static int used=1;
    inline static volatile uint8_t current=0;inline static volatile bool candCurrent=false;
    inline static uintptr_t k=0,textLo=0,textHi=0,lo2D=0,hi2D=0;
    inline static bool installed=false,enabled=true,ready=false;
    inline static Proj projA=nullptr,projB=nullptr;
    inline static bool preservePossessionDepth=false;inline static uint64_t possessionDepthProtected=0;
    inline static uint64_t begins=0,scans=0,hits=0,tagged=0,overflow=0,valve=0,windows=0,resets=0;
    inline static std::vector<uint32_t> worldRoots;inline static uint64_t lastTot2=0,lastTot3=0;
    static bool isRet(uint32_t a) {
        if(a<textLo+8||a>=textHi) return false;
        const unsigned char* p=reinterpret_cast<const unsigned char*>(uintptr_t(a));
        if(p[-5]==0xe8) {int32_t rel;memcpy(&rel,p-4,4);const uint32_t t=a+uint32_t(rel);return t>=textLo&&t<textHi;}
        if(p[-6]==0xff&&(p[-5]==0x15||(p[-5]&0xf8)==0x90)) return true;
        if(p[-7]==0xff&&p[-6]==0x94) return true;
        if(p[-3]==0xff&&(p[-2]&0xf8)==0x50) return true;
        if(p[-4]==0xff&&p[-3]==0x54) return true;
        if(p[-2]==0xff&&((p[-1]&0xf8)==0xd0||(p[-1]&0xf8)==0x10)) return true;
        return false;
    }
    static bool has(const Origin& o,const std::vector<uint32_t>& set) {for(int j=0;j<o.n;++j) for(uint32_t a:set) if(o.chain[j]==a) return true;return false;}
    // Candidate = its call chain holds no frame of the dungeon traversal (roots learned, kept across windows).
    // Interface = candidate whose 3D pieces keep lying on the bars / near the cursor (decideAll).
    static void decideOne(Origin& o) {o.cand=!has(o,worldRoots);if(!o.cand) o.ui=false;}
    static uint8_t idFor(uint32_t ret0,const uint32_t* chain,int n) {
        const int sn=std::min(n,int(SigFrames));uint32_t h=2166136261u^ret0;h*=16777619u;for(int j=0;j<sn;++j) {h^=chain[j];h*=16777619u;}
        for(int probe=0;probe<1024;++probe) {
            Sig& s=table[(h+uint32_t(probe))&1023];
            if(!s.id) {
                if(used>=MaxIds) break;
                Origin& o=origins[used];o=Origin{};o.ret0=ret0;o.n=uint8_t(n);for(int j=0;j<n;++j) o.chain[j]=chain[j];
                o.is2D=ret0>=lo2D&&ret0<hi2D;decideOne(o);s.hash=h;s.id=uint8_t(used);return uint8_t(used++);
            }
            if(s.hash==h) {const Origin& o=origins[s.id];if(o.ret0==ret0&&std::equal(chain,chain+sn,o.chain)) return s.id;}
        }
        // Table full until the next window: same decision, not recorded.
        ++overflow;Origin tmp{};tmp.ret0=ret0;tmp.n=uint8_t(n);for(int j=0;j<n;++j) tmp.chain[j]=chain[j];tmp.is2D=ret0>=lo2D&&ret0<hi2D;decideOne(tmp);
        return uint8_t(tmp.cand?FullCand:FullWorld);
    }
    // Game render thread, at every primitive setup (sp: stack at the entry of 0x58A970 = return address + 6 arguments).
    static void note(uint32_t* sp) {
        ++begins;
        const uint32_t ret0=sp[0];const uint32_t* frame=sp+7;const uint32_t fv=uint32_t(uintptr_t(frame));
        Cache& c=cache[((ret0>>2)^(fv>>2))&127];
        if(c.ret0==ret0&&c.sp==fv) {
            bool same=true;for(int j=0;j<c.n&&same;++j) same=frame[c.off[j]]==c.val[j];
            if(same) {current=c.id;candCurrent=candId(c.id);++origins[c.id].prims;++hits;return;}
        }
        ++scans;
        const uintptr_t top=uintptr_t(reinterpret_cast<NT_TIB*>(NtCurrentTeb())->StackBase);
        const size_t limit=std::min<size_t>(1536,top>uintptr_t(frame)?(top-uintptr_t(frame))/4:0);
        uint32_t chain[MaxChain];uint16_t off[MaxChain];int n=0;
        for(size_t i=0;i<limit&&n<MaxChain;++i) {
            const uint32_t v=frame[i];if(!isRet(v)) continue;
            bool dup=false;for(int j=0;j<n;++j) dup|=chain[j]==v;
            if(!dup) {chain[n]=v;off[n]=uint16_t(i);++n;}
        }
        const uint8_t id=idFor(ret0,chain,n);
        c.ret0=ret0;c.sp=fv;c.n=uint8_t(n);c.id=id;for(int j=0;j<n;++j) {c.off[j]=off[j];c.val[j]=chain[j];}
        current=id;candCurrent=candId(id);++origins[id].prims;
    }
    // After a 3D projection: origin id into the low bits of the pool vertex rhw (only vertices inside the view).
    // Only INTERFACE origins are marked: the dungeon's vertices stay bit for bit what the game computed (coplanar floor
    // overlays keep exactly the same depth on the table).
    static void tag(int idx) {
        // A UI provenance marker changes the low RHW bits. Animated world
        // meshes must retain their exact native depth in possession.
        if(preservePossessionDepth) {++possessionDepthProtected;return;}
        if(!enabled||!candCurrent||uint32_t(idx)>=uint32_t(Pool)) return;
        unsigned char* e=reinterpret_cast<unsigned char*>(k+0x76f358)+size_t(idx)*40;
        uint32_t clip;memcpy(&clip,e+0x1c,4);if(clip) return;
        uint32_t b;memcpy(&b,e+0x18,4);b=(b&~0xfffu)|0x900u|current;memcpy(e+0x18,&b,4);++tagged;
    }
    __attribute__((force_align_arg_pointer)) static uint32_t __cdecl projHookA(int idx,const float* v) {const uint32_t r=projA(idx,v);tag(idx);return r;}
    __attribute__((force_align_arg_pointer)) static uint32_t __cdecl projHookB(int idx,const float* v) {const uint32_t r=projB(idx,v);tag(idx);return r;}
    // Vertex of a Direct3D draw: origin id (0 = none / untagged).
    static int idOf(const unsigned char* vertex) {uint32_t b;memcpy(&b,vertex+12,4);return ((b>>8)&15)==9?int(b&255):0;}
    static bool uiId(int id) {return id>0&&id<used&&origins[id].ui;}
    static bool candId(int id) {return id==FullCand||(id>0&&id<used&&origins[id].cand);}
    static void* detour(uintptr_t at,const unsigned char* sig,size_t n,void* hook) {
        auto* entry=reinterpret_cast<unsigned char*>(at);
        if(memcmp(entry,sig,n)) return nullptr;
        auto* t=static_cast<unsigned char*>(VirtualAlloc(nullptr,32,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));if(!t) return nullptr;
        memcpy(t,entry,n);t[n]=0xe9;int32_t back=int32_t(reinterpret_cast<uintptr_t>(entry+n)-reinterpret_cast<uintptr_t>(t+n+5));memcpy(t+n+1,&back,4);
        DWORD old=0;VirtualProtect(t,32,PAGE_EXECUTE_READ,&old);
        if(!VirtualProtect(entry,n,PAGE_EXECUTE_READWRITE,&old)) return nullptr;
        unsigned char jump[16];memset(jump,0x90,sizeof(jump));jump[0]=0xe9;int32_t rel=int32_t(reinterpret_cast<uintptr_t>(hook)-reinterpret_cast<uintptr_t>(entry+5));memcpy(jump+1,&rel,4);
        memcpy(entry,jump,n);DWORD ig=0;VirtualProtect(entry,n,old,&ig);
        FlushInstructionCache(GetCurrentProcess(),entry,n);FlushInstructionCache(GetCurrentProcess(),t,32);
        return t;
    }
    static void install(uintptr_t base) {
        enabled=iniInt(L"InterfaceFromEngine",1)!=0;
        if(!enabled) {log("Interface origin: off (InterfaceFromEngine=0), 0.7.10 screen rules used");return;}
        k=base-0x400000;textLo=k+0x401000;textHi=k+0x66c000;lo2D=k+0x5767e0;hi2D=k+0x578d00;
        const unsigned char sBegin[]={0x83,0xec,0x3c,0x33,0xc9,0x53,0x55},sA[]={0x83,0xec,0x08,0x8b,0x44,0x24,0x10},sB[]={0x51,0x8b,0x4c,0x24,0x0c,0x56};
        // All three signatures are checked before anything is patched.
        if(memcmp(reinterpret_cast<void*>(k+0x58a970),sBegin,7)||memcmp(reinterpret_cast<void*>(k+0x58af70),sA,7)||memcmp(reinterpret_cast<void*>(k+0x58adc0),sB,6)) {
            enabled=false;log("Interface origin: engine signature unsupported, 0.7.10 screen rules used");return;}
        projA=reinterpret_cast<Proj>(detour(k+0x58af70,sA,7,reinterpret_cast<void*>(&projHookA)));
        projB=reinterpret_cast<Proj>(detour(k+0x58adc0,sB,6,reinterpret_cast<void*>(&projHookB)));
        dk2vr_beginTramp=detour(k+0x58a970,sBegin,7,reinterpret_cast<void*>(&dk2vr_beginStub));
        installed=projA&&projB&&dk2vr_beginTramp;
        if(!installed) {enabled=false;log("Interface origin: hook failed (begin %p, projections %p %p), 0.7.10 screen rules used",dk2vr_beginTramp,(void*)projA,(void*)projB);return;}
        log("Interface origin: engine hooks installed (primitive setup 0x58A970, 3D projections 0x58AF70 / 0x58ADC0)");
    }
    // Game thread, at Present, every 10 frames: dungeon roots (kept), then the interface decision per origin.
    static void decideAll() {
        if(!installed) return;
        uint64_t tot2=0,tot3=0;std::unordered_map<uint32_t,std::pair<uint64_t,uint64_t>> f;
        for(int i=1;i<used;++i) {const Origin& o=origins[i];if(!o.prims) continue;(o.is2D?tot2:tot3)+=o.prims;
            for(int j=0;j<o.n;++j) {auto& c=f[o.chain[j]];(o.is2D?c.first:c.second)+=o.prims;}}
        if(tot2>=10&&tot3>=50) {
            for(auto& [a,c]:f) if(c.second*2>=tot3&&c.first*10<=tot2*3&&worldRoots.size()<64&&std::find(worldRoots.begin(),worldRoots.end(),a)==worldRoots.end()) worldRoots.push_back(a);
            ready=true;++windows;lastTot2=tot2;lastTot3=tot3;
        }
        // Interface = no dungeon frame in the chain AND the origin's 3D pieces (over the last ~20 frames) keep lying on the
        // bars / at the cursor (buttons, portraits, cards, the held creature): >= 80 % of at least 4 draws. A dungeon piece
        // (even one without a known root) roams over the whole screen.
        uint64_t uiPrims=0;
        for(int i=1;i<used;++i) {
            Origin& o=origins[i];decideOne(o);
            o.accDraws=o.accDraws/2+o.winDraws;o.accEdge=o.accEdge/2+o.winEdge;o.winDraws=o.winEdge=0;
            o.ui=false&&o.cand&&o.accDraws>=4&&o.accEdge*10>=o.accDraws*8;
            if(o.ui) uiPrims+=o.prims;
        }
        // Safety valve: the interface is a few pieces; beyond 15 % of the 3D pieces the chains are not understood.
        if(uiPrims*100>tot3*15&&tot3>=50) {for(int i=1;i<used;++i) origins[i].ui=false;++valve;}
        for(int i=1;i<256;++i) {origins[i].total+=origins[i].prims;origins[i].lastPrims=origins[i].prims;origins[i].prims=0;}
        for(auto& c:cache) c=Cache{};
        if(used>235) {for(auto& s:table) s=Sig{};for(int i=1;i<256;++i) origins[i]=Origin{};used=1;++resets;}
    }
    static void report() {
        if(!enabled) return;
        auto rva=[](uint32_t a) {return unsigned(a-k);};
        std::string roots;char b[64];
        roots+=" (rule: no dungeon frame + stays on bars/cursor) | dungeon roots:";for(uint32_t a:worldRoots) {snprintf(b,sizeof(b)," %06X",rva(a));roots+=b;}
        log("Interface origin: installed=%d ready=%d windows=%llu primitive setups=%llu (stack scans %llu, cache %llu), 3D vertices tagged=%llu, origins=%d (overflow %llu, table resets %llu), last window 2D=%llu 3D=%llu pieces, valve=%llu; interface roots:%s",
            int(installed),int(ready),(unsigned long long)windows,(unsigned long long)begins,(unsigned long long)scans,(unsigned long long)hits,(unsigned long long)tagged,used-1,(unsigned long long)overflow,(unsigned long long)resets,
            (unsigned long long)lastTot2,(unsigned long long)lastTot3,(unsigned long long)valve,roots.c_str());
        begins=scans=hits=tagged=0;
        // Origins with their chain (game RVAs), pieces of the last window, and where their Direct3D triangles landed.
        int order[256];int m=0;for(int i=1;i<used;++i) order[m++]=i;
        std::sort(order,order+m,[](int a,int c){return origins[a].lastPrims+origins[a].draws>origins[c].lastPrims+origins[c].draws;});
        for(int q=0;q<m&&q<28;++q) {
            Origin& o=origins[order[q]];std::string ch;
            for(int j=0;j<o.n&&j<16;++j) {snprintf(b,sizeof(b)," %06X",rva(o.chain[j]));ch+=b;}
            log("  origin %3d %s %s pieces=%u draws=%u tris=%u (interface %u) box %.2f,%.2f-%.2f,%.2f setup at %06X chain:%s",order[q],o.is2D?"2D":"3D",o.ui?"INTERFACE":"dungeon  ",
                o.lastPrims,o.draws,o.tris,o.uiTris,o.box[0],o.box[1],o.box[2],o.box[3],rva(o.ret0),ch.c_str());
        }
        for(int i=1;i<256;++i) {Origin& o=origins[i];o.draws=o.tris=o.uiTris=0;o.box[0]=o.box[1]=1e9f;o.box[2]=o.box[3]=-1e9f;}
    }
};
void engineOriginNote(uint32_t* sp) {EngineOrigin::note(sp);}
// 0.8.4: the white view frame of the minimap. The game computes it by projecting the 4 screen corners onto the ground
// (0x572920, cdecl 4 args, returns a pointer to the result; reads the view focal 0x760AE8) in the function at 0x41B180
// (calls at 0x41B3DD / 41B418 / 41B447 / 41B47F, then rotates the corners with the camera direction). While the table is
// shown the focal is scaled by LensHack (k ~ .2-.25) so the game's view is 1/k times larger than what the table shows:
// the frame came out several times too big (0.8.x report). For those four calls the focal is put back to its unscaled
// value (focal / k); every other caller (mouse picking, tile selection) keeps the scaled one.
struct MinimapFrame {
    using Fn=void*(__cdecl*)(void*,uint32_t,uint32_t,uint32_t);
    inline static Fn original=nullptr;inline static uintptr_t k=0;inline static uint64_t fixedCalls=0;inline static bool installed=false;
    __attribute__((force_align_arg_pointer)) static void* __cdecl hook(void* a,uint32_t b,uint32_t c,uint32_t d) {
        const uintptr_t ret=reinterpret_cast<uintptr_t>(__builtin_return_address(0));
        const float scale=LensHack::applied;
        if(ret>=k+0x41b3d0&&ret<k+0x41b500&&scale>.05f&&scale<.999f) {
            volatile float* focal=reinterpret_cast<volatile float*>(k+0x760ae8);
            const float saved=*focal;*focal=saved/scale;
            void* r=original(a,b,c,d);
            *focal=saved;++fixedCalls;return r;
        }
        return original(a,b,c,d);
    }
    static void install(uintptr_t base) {
        if(iniInt(L"MinimapFrameFix",1)==0) {log("Minimap frame: fix off (MinimapFrameFix=0)");return;}
        k=base-0x400000;
        const unsigned char sig[]={0x83,0xec,0x1c,0xd9,0x05,0xe8,0x0a,0x76,0x00};
        if(memcmp(reinterpret_cast<void*>(k+0x572920),sig,3)||memcmp(reinterpret_cast<void*>(k+0x572920+5),sig+5,4)) {log("Minimap frame: screen-to-ground signature unsupported");return;}
        // the fld operand is an absolute address of the exe at its preferred base: only valid when loaded there
        if(base!=0x400000) {log("Minimap frame: exe not at its preferred base, fix off");return;}
        original=reinterpret_cast<Fn>(EngineOrigin::detour(k+0x572920,sig,9,reinterpret_cast<void*>(&hook)));
        installed=original!=nullptr;log("Minimap frame: view frame corrected while the table is shown (%s)",installed?"hooked 0x572920":"hook failed");
    }
};
struct TopDownCamera {
    using Rebuild=void(__thiscall*)(void*);
    inline static Rebuild rebuild=nullptr;
    inline static volatile LONG live=1; // 0.8.25: 1 while the top-down camera is applied (the enlarged view rectangle is only valid then)
    bool enabled=true,active=false,flipped=false,checked=false,haveKnown=false;int16_t original=0,known=0,target=0x1cc;float baseline=0;int frames=0,lowFrames=0,flips=0,sinceFlip=-1;bool watching=true;uint32_t rewrites=0;int hold=0;   // 0.8.94.17: frames during which an early (pre-render) application may not be undone by the Present-time call
    void init(const NativeCamera& native) {
        enabled=iniInt(L"TopDownCamera",1)!=0&&native.supported;
        const unsigned char sig[]={0x8b,0x44,0x24,0x04,0x66,0x8b,0x10,0x66,0x89,0x91,0x84,0x0d,0x00,0x00,0xe8};
        if(enabled&&memcmp(reinterpret_cast<void*>(native.base+0x49f50),sig,sizeof(sig))) {enabled=false;log("Top-down camera: signature unsupported");}
        if(enabled) {rebuild=reinterpret_cast<Rebuild>(native.base+0x4d6d0);live=0;log("Top-down camera: on (TopDownCamera=1)");}
    }
    static float down(const NativeCamera& native) {return native.read<float>(0x760ac4+8*4);}
    void apply(const NativeCamera& native,bool want) {
        if(!enabled) return;
        unsigned char* c=native.camera();if(!c) return;
        int16_t& tilt=*reinterpret_cast<int16_t*>(c+0xd86);
        if(!want&&active&&hold>0) {--hold;return;}   // 0.8.94.17: applied before the render of the first table frame after a possession; the Present-time state catches up
        if(want) {hold=0;
            if(!active) {active=true;live=1;
                // 0.8.94.5.1: the player's tilt is remembered ONCE (first read, before any possession). Re-reading it at each return to the table picked up the
                // possession camera's own tilt (512 / 509 in report 20261004_015156): it was then 'restored' at the next possession entry -> camera looking
                // straight down, no first-person projection (black screen), huge lag, tilted dungeon.
                if(!haveKnown) {known=tilt;haveKnown=true;} else if(tilt!=known) log("Top-down camera: tilt read at the return to the table (%d) ignored, player tilt %d kept",int(tilt),int(known));
                original=known;baseline=down(native);frames=0;lowFrames=0;checked=false;log("Top-down camera: tilt %d -> %d (downward %.3f)",int(original),int(target),baseline);}
            if(tilt!=target) {if(frames>2) ++rewrites;tilt=target;rebuild(c);}
            if(++frames==4&&!checked) {
                checked=true;float now=down(native);
                if(now<baseline-.01f&&!flipped) {flipped=true;++flips;target=int16_t(target==0?0x1cc:0);tilt=target;rebuild(c);log("Top-down camera: the other end looks down (downward %.3f < %.3f): tilt %d",now,baseline,int(target));}
                else log("Top-down camera: downward component %.3f (was %.3f)",now,baseline);
            }
            // 0.8.90.9: the check above trusted the very first readings. Coming from the menu room they can be stale (journal 142910: "tilt 220 -> 460
            // (downward 1.000)" then "downward component 1.000 (was 1.000)": no flip, the camera stayed at the wrong end and the dungeon was tilted on the
            // table). From now on the camera is watched for as long as it is applied: a view that does not look down (< 0.5) for 3 frames in a row
            // means the wrong end of the tilt range; the other end is taken. Whatever the game animates, the dungeon ends up flat on the table.
            if(frames>4&&watching) {
                const float now=down(native);
                if(sinceFlip>=0) {   // a flip made by the watch: kept when the view then looks down, undone (and the watch stops) when it did not help
                    ++sinceFlip;
                    if(now>=.9f) {sinceFlip=-1;lowFrames=0;log("Top-down camera (0.8.90.9 watch): the view looks down after the change (downward %.3f), tilt %d kept",now,int(target));}
                    else if(sinceFlip>=30) {target=int16_t(target==0?0x1cc:0);tilt=target;rebuild(c);watching=false;sinceFlip=-1;
                        log("Top-down camera (0.8.90.9 watch): the change did not help (downward %.3f), tilt %d restored, watch stopped",now,int(target));}
                } else if(now<.5f) {
                    if(++lowFrames>=3&&flips<6) {++flips;lowFrames=0;sinceFlip=0;target=int16_t(target==0?0x1cc:0);tilt=target;rebuild(c);
                        log("Top-down camera (0.8.90.9 watch): the view does not look down (downward %.3f): tilt %d",now,int(target));}
                } else lowFrames=0;
            }
        } else if(active) {
            active=false;live=0;tilt=original;rebuild(c);log("Top-down camera: player tilt %d restored (rewrites while active: %u)",int(original),rewrites);rewrites=0;
        }
    }
};
// 0.5.16 diagnostic: who uploads textures. paintSurf (0x590450, thiscall(src,x,y), ret 0xC, verified) copies a
// texture into the game's texture cache; 0.5.15 measured ~2200 uploads per second even with a still view.
// Its 7 direct callers (verified: call rel32 to 0x590450) are counted separately. Log: "Texture uploads".
struct PaintProbe {
    using Paint=void(__fastcall*)(void*,void*,void*,int,int);
    inline static Paint original=nullptr;inline static volatile LONG counts[7]{},withLocks[7]{};inline static volatile LONG64 ticks[7]{};inline static thread_local int depth=0;
    inline static const uint32_t sites[7]={0x590d3f,0x5912b4,0x591bc9,0x5939d2,0x593ddf,0x5940b8,0x594771};
    inline static const char* names[7]={"paint","loadPrescaled","copyScaledWithType","SurfHashList2 fill A","SurfHashList2 fill B","_probablySort (eviction)","expandPut (new texture)"};
    // 0.8.60: time of each paintSurf call and whether it took any texture/surface lock (calls without lock return early: nothing to repaint).
    template<int N> static void __fastcall thunk(void* self,void*,void* src,int x,int y) {
        InterlockedIncrement(&counts[N]);
        if(depth++!=0) {original(self,nullptr,src,x,y);--depth;return;}
        CallTiming::paintActive=1;CallTiming::paintLocks=0;CallTiming::paintKind=N;const long long t0=Clock::now();
        original(self,nullptr,src,x,y);
        const long long dt=Clock::now()-t0;CallTiming::paintActive=0;CallTiming::paintKind=-1;--depth;
        InterlockedExchangeAdd64(&ticks[N],dt);if(CallTiming::paintLocks) InterlockedIncrement(&withLocks[N]);
    }
    static void install(const NativeCamera& native) {
        if(!native.supported||iniInt(L"LockDiagnostic",1)==0) return;
        void* thunks[7]={(void*)&thunk<0>,(void*)&thunk<1>,(void*)&thunk<2>,(void*)&thunk<3>,(void*)&thunk<4>,(void*)&thunk<5>,(void*)&thunk<6>};
        original=reinterpret_cast<Paint>(native.base+0x190450);int ok=0;
        for(int i=0;i<7;++i) {
            auto* c=reinterpret_cast<unsigned char*>(native.base+sites[i]-0x400000);int32_t rel=0;memcpy(&rel,c+1,4);
            if(c[0]!=0xe8||uintptr_t(c+5+rel)!=native.base+0x190450) continue;
            DWORD old=0;if(!VirtualProtect(c,5,PAGE_EXECUTE_READWRITE,&old)) continue;
            int32_t t=int32_t(reinterpret_cast<uintptr_t>(thunks[i])-reinterpret_cast<uintptr_t>(c+5));memcpy(c+1,&t,4);
            DWORD ig=0;VirtualProtect(c,5,old,&ig);FlushInstructionCache(GetCurrentProcess(),c,5);++ok;
        }
        log("Texture uploads: %d of 7 callers of paintSurf measured",ok);
    }
    inline static double lastEvictMs=0;inline static LONG lastEvictN=0;   // 0.9.25 PERF: eviction paints of the last 5 s window (ms in total)
    static void report() {
        if(!original) return;
        char line[1000];int n=snprintf(line,sizeof(line),"Texture uploads (last 5 s; paint calls, of which took a lock, total time):");
        for(int i=0;i<7;++i) {
            LONG c=InterlockedExchange(&counts[i],0),w=InterlockedExchange(&withLocks[i],0);long long t=InterlockedExchange64(&ticks[i],0);
            if(i==5) {lastEvictMs=Clock::micros(t)/1000.;lastEvictN=c;}
            if(c) n+=snprintf(line+n,sizeof(line)-n," | %s %ld (%ld with lock, %.0f ms)",names[i],(long)c,(long)w,Clock::micros(t)/1000.);
        }
        log("%s",line);
    }
};
// Holes fix: DKII removes back-facing triangles itself (D3D cull mode stays NONE).
// addTriangleToRender1 (0x58B9D0) computes the signed screen area at 0x58BA0E and skips the
// triangle when it is negative (jne at 0x58BA43). Every triangle, clipped or not, passes here.
// The jump is replaced by NOPs: the native view is unchanged (depth test) and VR sees all faces.
struct BackFaces {
    static void install(const NativeCamera& native) {
        if(!native.supported||iniInt(L"DrawBackFaces",1)==0) {log("Back faces: native culling kept");return;}
        auto* p=reinterpret_cast<unsigned char*>(native.base+0x18ba36);
        const unsigned char sig[]={0xde,0xe9,0xd8,0x1d,0x40,0xfe,0x66,0x00,0xdf,0xe0,0xf6,0xc4,0x01,0x0f,0x85,0x05,0x01,0x00,0x00};
        if(memcmp(p,sig,sizeof(sig))) {log("Back faces: signature unsupported, native culling kept");return;}
        DWORD old=0;if(!VirtualProtect(p+13,6,PAGE_EXECUTE_READWRITE,&old)) return;
        memset(p+13,0x90,6);DWORD ignored=0;VirtualProtect(p+13,6,old,&ignored);
        FlushInstructionCache(GetCurrentProcess(),p+13,6);
        log("Back faces: kept (software back-face culling at 0x58BA43 disabled)");
    }
};
// 0.6.9.4: crash diagnosis. 0.6.9.3.1 report: the game quit (exit code -1, wrapper unloaded normally) after a
// selection click, with nothing in the log. A vectored handler writes the first serious exceptions (even those
// the game catches itself before quitting): code, address (module+offset), access address, code addresses on the stack.
struct CrashLog {
    static inline volatile LONG count=0;
    static std::string where(uintptr_t a);
    // 0.8.94.26: GAME BUG fixed (reports 20261005_105751 and 20261005_143033: crash at the launch of a NEW game, DKII-DX.exe+0x1FCB80, read
    // 0x03E7F000 with esi=0x03E7EFFD). The game's pixel converter 0x5FC9xx-0x5FCBxx reads every source pixel with 'mov ecx,[esi]' (8B 0E, 4 bytes)
    // even for 3-byte pixels: at the last pixel of an image it reads 1 byte past the image, which crashes when the image ends exactly at a page
    // boundary. Only that case is handled: that instruction, in that function, a read that runs 1-3 bytes past the readable memory. The readable
    // bytes are read, the missing one is 0 (outside the 24-bit pixel: masked away by the converter), and the game goes on. Anything else is logged
    // as before.
    static inline uintptr_t fixBase=0;static inline volatile LONG overreads=0;
    static LONG CALLBACK handler(EXCEPTION_POINTERS* e) {
        const DWORD code=e->ExceptionRecord->ExceptionCode;
        if(code==EXCEPTION_ACCESS_VIOLATION&&fixBase&&e->ExceptionRecord->NumberParameters>=2&&e->ExceptionRecord->ExceptionInformation[0]==0) {
            CONTEXT* c=e->ContextRecord;const uintptr_t ip=c->Eip,lo=fixBase-0x400000+0x5fc900,hi=fixBase-0x400000+0x5fcd00;
            if(ip>=lo&&ip<hi) {const unsigned char* op=reinterpret_cast<const unsigned char*>(ip);
                const uintptr_t src=c->Esi,bad=e->ExceptionRecord->ExceptionInformation[1];
                if(op[0]==0x8b&&op[1]==0x0e&&bad>src&&bad<=src+3) {
                    DWORD v=0;memcpy(&v,reinterpret_cast<const void*>(src),bad-src);c->Ecx=v;c->Eip+=2;
                    if(InterlockedIncrement(&overreads)<=5) log("Game bug avoided (0.8.94.26): the pixel converter read %u byte(s) past an image at %08lX (end of memory page); missing byte(s) read as 0, game continues",unsigned(src+4-bad),(unsigned long)src);
                    return EXCEPTION_CONTINUE_EXECUTION;}}}
        if(code!=EXCEPTION_ACCESS_VIOLATION&&code!=EXCEPTION_ILLEGAL_INSTRUCTION&&code!=EXCEPTION_INT_DIVIDE_BY_ZERO&&code!=EXCEPTION_STACK_OVERFLOW&&code!=EXCEPTION_PRIV_INSTRUCTION&&code!=EXCEPTION_ARRAY_BOUNDS_EXCEEDED) return EXCEPTION_CONTINUE_SEARCH;
        if(InterlockedIncrement(&count)>8) return EXCEPTION_CONTINUE_SEARCH;
        const CONTEXT* c=e->ContextRecord;
        std::string line;char b[160];
        snprintf(b,sizeof(b),"CRASH? exception %08lX thread %lu at ",(unsigned long)code,(unsigned long)GetCurrentThreadId());line=b;
        line+=where(uintptr_t(e->ExceptionRecord->ExceptionAddress));
        if(code==EXCEPTION_ACCESS_VIOLATION&&e->ExceptionRecord->NumberParameters>=2) {snprintf(b,sizeof(b)," (%s 0x%08lX)",e->ExceptionRecord->ExceptionInformation[0]==1?"write":e->ExceptionRecord->ExceptionInformation[0]==8?"execute":"read",(unsigned long)e->ExceptionRecord->ExceptionInformation[1]);line+=b;}
        snprintf(b,sizeof(b)," eax=%08lX ebx=%08lX ecx=%08lX edx=%08lX esi=%08lX edi=%08lX ebp=%08lX esp=%08lX ; stack:",c->Eax,c->Ebx,c->Ecx,c->Edx,c->Esi,c->Edi,c->Ebp,c->Esp);line+=b;
        MEMORY_BASIC_INFORMATION m{};const uintptr_t sp=c->Esp;
        if(code!=EXCEPTION_STACK_OVERFLOW&&VirtualQuery(reinterpret_cast<void*>(sp),&m,sizeof(m))&&m.State==MEM_COMMIT&&!(m.Protect&(PAGE_GUARD|PAGE_NOACCESS))) {
            const size_t got=std::min<size_t>(256,(reinterpret_cast<uintptr_t>(m.BaseAddress)+m.RegionSize-sp)/4);
            const DWORD* s=reinterpret_cast<const DWORD*>(sp);int n=0;
            for(size_t i=0;i<got&&n<20;++i) {std::string w=where(s[i]);if(!w.empty()&&w!="?") {line+=" "+w;++n;}}
        }
        log("%s",line.c_str());
        return EXCEPTION_CONTINUE_SEARCH;
    }
    // 0.6.9.7: the game quits by itself (exit code -1, no exception) after a click on the dungeon: its own exit path
    // (CRT 0x637368 / 0x638640 ExitProcess, 0x6372DE TerminateProcess) is logged with the code addresses on the stack.
    using ExitFn=void(WINAPI*)(UINT);using TermFn=BOOL(WINAPI*)(HANDLE,UINT);
    static inline ExitFn realExit=nullptr;static inline TermFn realTerminate=nullptr;
    static void stackLine(const char* what,UINT code,const void* sp) {
        std::string line;char b[128];snprintf(b,sizeof(b),"%s(%d) thread %lu ; stack:",what,int(code),(unsigned long)GetCurrentThreadId());line=b;
        MEMORY_BASIC_INFORMATION m{};
        if(VirtualQuery(sp,&m,sizeof(m))&&m.State==MEM_COMMIT) {
            const size_t got=std::min<size_t>(1024,(reinterpret_cast<uintptr_t>(m.BaseAddress)+m.RegionSize-reinterpret_cast<uintptr_t>(sp))/4);
            const DWORD* st=static_cast<const DWORD*>(sp);int n=0;
            for(size_t i=0;i<got&&n<40;++i) {std::string w=where(st[i]);if(!w.empty()&&w!="?") {line+=" "+w;++n;}}
        }
        log("%s",line.c_str());
    }
    static void WINAPI exitHook(UINT code) {int marker=0;stackLine("GAME EXIT: ExitProcess",code,&marker);realExit(code);}
    static BOOL WINAPI terminateHook(HANDLE h,UINT code) {int marker=0;if(h==GetCurrentProcess()||GetProcessId(h)==GetCurrentProcessId()) stackLine("GAME EXIT: TerminateProcess",code,&marker);return realTerminate(h,code);}
    static void hookIat(uintptr_t base,uint32_t slot,void* hook,void** real,const char* name) {
        void** entry=reinterpret_cast<void**>(base-0x400000+slot);
        HMODULE k=GetModuleHandleA("kernel32.dll");void* expected=k?reinterpret_cast<void*>(GetProcAddress(k,name)):nullptr;
        if(!expected||*entry!=expected) {log("Crash log: %s import not recognized, not hooked",name);return;}
        DWORD old=0;if(!VirtualProtect(entry,4,PAGE_READWRITE,&old)) return;
        *real=*entry;*entry=hook;DWORD ig=0;VirtualProtect(entry,4,old,&ig);
    }
    static void install(uintptr_t base=0) {
        if(base) fixBase=base;
        if(AddVectoredExceptionHandler(1,&handler)) log("Crash log: exception handler installed (game pixel-converter over-read guarded: %s)",fixBase?"yes":"no");
        if(base) {hookIat(base,0x66c234,reinterpret_cast<void*>(&exitHook),reinterpret_cast<void**>(&realExit),"ExitProcess");
                  hookIat(base,0x66c1f8,reinterpret_cast<void*>(&terminateHook),reinterpret_cast<void**>(&realTerminate),"TerminateProcess");
                  log("Crash log: game exit paths watched");}
    }
};
struct Profiler;
// Sampling profiler (diagnostic): every ~2 ms the game render thread is briefly suspended and its
// instruction pointer recorded (no allocation while suspended). Summary every 10 s in the log.
struct Profiler {
    HANDLE thread=nullptr,worker=nullptr;volatile LONG running=0,count=0;
    static constexpr LONG Capacity=8192;static constexpr int Depth=320;DWORD samples[Capacity]{};
    std::vector<DWORD> stacks; // Depth dwords from ESP per sample, copied while the thread is suspended
    bool enabled=false,sampling=false;uint64_t lastReport=0;   // 0.9.47.9: sampling=false keeps only the stall watchdog (ProfilerSampling=1 restores the 500 samples/s)
    // 0.6.1.3: stall watchdog. If the render thread does not reach Present for 3 s, its instruction pointer
    // and the code addresses found on its stack are written to the log once (black screen diagnosis).
    volatile uint64_t lastPresentTick=0;bool stallLogged=false;
    static std::string where(uintptr_t a) {
        HMODULE mods[512];DWORD need=0;
        if(!EnumProcessModules(GetCurrentProcess(),mods,sizeof(mods),&need)) return "?";
        for(DWORD i=0;i<std::min<DWORD>(need/sizeof(HMODULE),512);++i) {
            MODULEINFO mi{};if(!GetModuleInformation(GetCurrentProcess(),mods[i],&mi,sizeof(mi))) continue;
            uintptr_t b=reinterpret_cast<uintptr_t>(mi.lpBaseOfDll);
            if(a>=b&&a<b+mi.SizeOfImage) {char n[64]{};GetModuleBaseNameA(GetCurrentProcess(),mods[i],n,64);char out[96];snprintf(out,96,"%s+0x%X",n,unsigned(a-b));return out;}
        }
        return "";
    }
    void stallDump() {
        if(SuspendThread(thread)==DWORD(-1)) return;
        CONTEXT c{};c.ContextFlags=CONTEXT_CONTROL;BOOL ok=GetThreadContext(thread,&c);
        DWORD stack[256]{};size_t got=0;
        if(ok) {
            MEMORY_BASIC_INFORMATION m{};uintptr_t sp=c.Esp;
            if(VirtualQuery(reinterpret_cast<void*>(sp),&m,sizeof(m))&&m.State==MEM_COMMIT&&!(m.Protect&(PAGE_GUARD|PAGE_NOACCESS))) {
                got=std::min<size_t>(256,(reinterpret_cast<uintptr_t>(m.BaseAddress)+m.RegionSize-sp)/4);memcpy(stack,reinterpret_cast<void*>(sp),got*4);
            }
        }
        ResumeThread(thread);
        std::string line="STALL: no Present for 3 s; render thread at "+(ok?where(c.Eip):std::string("?"))+" ; stack:";
        int n=0;for(size_t i=0;i<got&&n<24;++i) {std::string w=where(stack[i]);if(!w.empty()&&w!="?") {line+=" "+w;++n;}}
        log("%s",line.c_str());
    }
    void start(DWORD threadId) {
        static bool crashLog=false;if(!crashLog) {crashLog=true;CrashLog::install(reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr)));}
        if(thread||iniInt(L"Profiler",1)==0) return;
        thread=OpenThread(THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_QUERY_INFORMATION,FALSE,threadId);
        if(!thread) return;
        stacks.assign(size_t(Capacity)*Depth,0);
        sampling=iniInt(L"ProfilerSampling",0)!=0;
        running=1;enabled=true;lastReport=GetTickCount64();
        worker=CreateThread(nullptr,0,&loop,this,0,nullptr);
        log("Profiler started on render thread %lu",(unsigned long)threadId);
    }
    static DWORD WINAPI loop(void* self) {
        auto* p=static_cast<Profiler*>(self);
        timeBeginPeriod(1); // finer Sleep for ~500 samples/s; released at process exit
        while(p->running) {
            Sleep(p->sampling?2:250);
            if(p->lastPresentTick&&GetTickCount64()-p->lastPresentTick>3000) {if(!p->stallLogged) {p->stallLogged=true;p->stallDump();}continue;}
            if(!p->sampling) continue;
            if(SuspendThread(p->thread)==DWORD(-1)) continue;
            CONTEXT c{};c.ContextFlags=CONTEXT_CONTROL;BOOL ok=GetThreadContext(p->thread,&c);
            LONG i=ok?InterlockedIncrement(&p->count)-1:Capacity;
            if(ok&&i<Capacity) {
                p->samples[i]=c.Eip;
                DWORD* out=p->stacks.data()+size_t(i)*Depth;
                MEMORY_BASIC_INFORMATION m{};
                uintptr_t sp=c.Esp,end=sp+Depth*4;
                if(VirtualQuery(reinterpret_cast<void*>(sp),&m,sizeof(m))&&m.State==MEM_COMMIT&&!(m.Protect&(PAGE_GUARD|PAGE_NOACCESS))) {
                    uintptr_t limit=std::min<uintptr_t>(end,reinterpret_cast<uintptr_t>(m.BaseAddress)+m.RegionSize);
                    memcpy(out,reinterpret_cast<void*>(sp),limit-sp);
                    if(limit<end) memset(reinterpret_cast<unsigned char*>(out)+(limit-sp),0,end-limit);
                } else memset(out,0,Depth*4);
            }
            ResumeThread(p->thread);
        }
        return 0;
    }
    static std::string exportName(uintptr_t base,uint32_t rva) {
        auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(base+dos->e_lfanew);
        auto& dir=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        if(!dir.VirtualAddress) return "?";
        auto* ex=reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(base+dir.VirtualAddress);
        auto* names=reinterpret_cast<const DWORD*>(base+ex->AddressOfNames);
        auto* ords=reinterpret_cast<const WORD*>(base+ex->AddressOfNameOrdinals);
        auto* funcs=reinterpret_cast<const DWORD*>(base+ex->AddressOfFunctions);
        uint32_t best=0;const char* name="?";
        for(DWORD i=0;i<ex->NumberOfNames;++i) {uint32_t f=funcs[ords[i]];if(f<=rva&&f>best) {best=f;name=reinterpret_cast<const char*>(base+names[i]);}}
        char buf[160];snprintf(buf,sizeof(buf),"%s+0x%X",name,rva-best);return buf;
    }
    static bool systemModule(const std::string& n) {
        std::string l=n;for(auto& ch:l) ch=char(tolower(ch));
        for(const char* s:{"ntdll.dll","win32u.dll","kernel32.dll","kernelbase.dll","user32.dll","gdi32.dll","gdi32full.dll","wow64","msvcrt.dll","ucrtbase.dll","combase.dll","rpcrt4.dll"})
            if(l.rfind(s,0)==0) return true;
        return false;
    }
    void report() {
        if(!enabled||!sampling||GetTickCount64()-lastReport<10000) return;
        lastReport=GetTickCount64();
        LONG n=std::min<LONG>(InterlockedExchange(&count,0),Capacity);
        if(n<=0) return;
        HMODULE modules[512];DWORD needed=0;
        if(!EnumProcessModules(GetCurrentProcess(),modules,sizeof(modules),&needed)) return;
        struct Mod {uintptr_t base,size;std::string name;uint32_t hits=0;std::unordered_map<uint32_t,uint32_t> buckets;};
        std::vector<Mod> mods;
        for(DWORD i=0;i<needed/sizeof(HMODULE)&&i<512;++i) {
            MODULEINFO mi{};char name[MAX_PATH]{};
            if(GetModuleInformation(GetCurrentProcess(),modules[i],&mi,sizeof(mi))&&GetModuleBaseNameA(GetCurrentProcess(),modules[i],name,MAX_PATH))
                mods.push_back({reinterpret_cast<uintptr_t>(mi.lpBaseOfDll),mi.SizeOfImage,name});
        }
        uint32_t unknown=0;
        auto find=[&](uintptr_t a)->Mod* {for(auto& m:mods) if(a>=m.base&&a<m.base+m.size) return &m;return nullptr;};
        // For samples inside Windows (waits, syscalls): first non-system module found on the stack.
        std::unordered_map<std::string,uint32_t> callers;std::unordered_map<std::string,uint32_t> callerSites;uint32_t waits=0;
        std::unordered_map<std::string,uint32_t> waitFunctions;std::unordered_map<uintptr_t,std::string> names;
        std::unordered_map<std::string,uint32_t> wrapperSites,gameSites;
        for(LONG i=0;i<n;++i) {
            uintptr_t eip=samples[i];Mod* m=find(eip);
            if(!m) {++unknown;continue;}
            ++m->hits;++m->buckets[uint32_t(eip-m->base)&~0x3fu];
            if(!systemModule(m->name)) continue;
            uintptr_t key=eip&~uintptr_t(0x3f);auto cached=names.find(key);
            if(cached==names.end()) cached=names.emplace(key,m->name+"!"+exportName(m->base,uint32_t(eip-m->base)).substr(0,48)).first;
            ++waits;++waitFunctions[cached->second];
            const DWORD* st=stacks.data()+size_t(i)*Depth;std::string who;int links=0;bool firstSite=true;
            // Call chain of distinct non-system modules, innermost first (e.g. nvd3dum<d3d9<DDRAW<DKII-DX).
            for(int k=0;k<Depth&&links<5;++k) {
                Mod* c=find(st[k]);
                if(!c||systemModule(c->name)) continue;
                std::string shortName=c->name.substr(0,c->name.find('.'));
                if(firstSite||who.rfind(shortName)!=who.size()-shortName.size()) {
                    char site[96];snprintf(site,sizeof(site),"%s+%06X",c->name.c_str(),unsigned(st[k]-c->base));
                    if(!firstSite) {char chain[120];snprintf(chain,sizeof(chain),"%s+%06X",shortName.c_str(),unsigned(st[k]-c->base));}
                    if(firstSite) {++callerSites[site];firstSite=false;}
                    if(who.empty()||who.size()<shortName.size()||who.compare(who.size()-shortName.size(),shortName.size(),shortName)) {who+=who.empty()?shortName:"<"+shortName;++links;}
                }
            }
            if(who.empty()) who="(none)";
            // First return address inside the wrapper and inside the game, for offline mapping.
            bool gotWrapper=false,gotGame=false;
            for(int k=0;k<Depth&&!(gotWrapper&&gotGame);++k) {
                Mod* c=find(st[k]);if(!c) continue;
                std::string l=c->name;for(auto& ch:l) ch=char(tolower(ch));
                char site[64];snprintf(site,sizeof(site),"%06X",unsigned(st[k]-c->base));
                if(!gotWrapper&&l=="ddraw.dll") {++wrapperSites[site];gotWrapper=true;}
                else if(!gotGame&&l=="dkii-dx.exe") {++gameSites[site];gotGame=true;}
            }
            ++callers[who];
        }
        std::sort(mods.begin(),mods.end(),[](const Mod& a,const Mod& b){return a.hits>b.hits;});
        std::string line;char buf[128];
        for(auto& m:mods) {if(!m.hits) break;snprintf(buf,sizeof(buf)," | %s %.1f%%",m.name.c_str(),100.*m.hits/n);line+=buf;}
        snprintf(buf,sizeof(buf)," | other %.1f%%",100.*unknown/n);line+=buf;
        log("Profile (render thread, %ld samples/10 s):%s",n,line.c_str());
        auto topOf=[&](std::unordered_map<std::string,uint32_t>& map,size_t count,uint32_t total) {
            std::vector<std::pair<std::string,uint32_t>> v(map.begin(),map.end());
            std::sort(v.begin(),v.end(),[](auto& a,auto& b){return a.second>b.second;});
            std::string t;char b2[160];
            for(size_t j=0;j<v.size()&&j<count;++j) {snprintf(b2,sizeof(b2)," | %s %.1f%%",v[j].first.c_str(),100.*v[j].second/std::max<uint32_t>(1,total));t+=b2;}
            return t;
        };
        if(waits) {
            log("Profile Windows functions (%.1f%% of samples):%s",100.*waits/n,topOf(waitFunctions,8,n).c_str());
            log("Profile call chains into Windows (innermost<outer):%s",topOf(callers,10,n).c_str());
            log("Profile calling sites:%s",topOf(callerSites,14,n).c_str());
            log("Profile wrapper (DDRAW.dll) return sites during waits:%s",topOf(wrapperSites,12,n).c_str());
            log("Profile game (DKII-DX.exe) return sites during waits:%s",topOf(gameSites,12,n).c_str());
        }
        for(size_t k=0;k<mods.size()&&k<3;++k) {
            auto& m=mods[k];if(!m.hits) break;
            std::vector<std::pair<uint32_t,uint32_t>> top(m.buckets.begin(),m.buckets.end());
            std::sort(top.begin(),top.end(),[](auto& a,auto& b){return a.second>b.second;});
            std::string t;
            for(size_t j=0;j<top.size()&&j<25;++j) {snprintf(buf,sizeof(buf)," %06X:%.1f%%",top[j].first,100.*top[j].second/n);t+=buf;}
            log("Profile top %s (RVA/64 bytes):%s",m.name.c_str(),t.c_str());
        }
    }
};
std::string CrashLog::where(uintptr_t a) {return Profiler::where(a);}
// Wider native visibility: the engine selects visible tiles in MyDLVec2i_generate (0x574820)
// from the screen rectangle (0x760AEC..0x760AF8) and a far distance (0x760B04).
// Its single call (0x575C0B) is redirected: the rectangle and distance are enlarged only during
// the tile selection, then restored before the scene is projected.
struct ViewExpand {
    using Generate=void*(__cdecl*)();
    inline static Generate original=nullptr;
    inline static float expand=1,farScale=1;inline static uintptr_t base=0;
    // 0.4.7 (TableView=1): extra expansion computed each frame so the tiles of the whole table exist.
    inline static volatile float autoX=1,autoBottom=1,autoFar=1; // 0.5.5: sides and player side only (top and far unchanged)
    // 0.5.16: exact screen rectangle that contains the whole table (projected corners), used with the top-down camera.
    inline static volatile LONG rectValid=0;inline static volatile float rectL=0,rectT=0,rectR=0,rectB=0;
    static float& at(uintptr_t va) {return *reinterpret_cast<float*>(base+va-0x400000);}
    inline static const NativeCamera* nativeCam=nullptr;inline static volatile LONG skipped=0,transitionSkips=0;
    // 0.8.25: the enlarged view rectangle / far distance are only valid with the top-down table camera. Without it (possession, menus, the
    // frames around a pause: 0.8.24 and before) the engine built the render list of the NORMAL camera with a rectangle up to x5 / x6 and a far
    // distance x4: render memory 7 -> 115 blocks of 2 MB and one game frame of ~2.7 s (freeze / black screen, crash risk) - seen in the logs.
    // 0.8.94.6: 'settle' frames after a possession (the camera fields and the zoom range are still being rebuilt: report 20261004_160911 showed
    // 'zoom cur=-1' then the zoom-out range extended on the very frame of the return, with the player-side expansion x8.70 -> pool exhausted).
    inline static volatile LONG settle=0;
    static bool expansionAllowed() {
        if(!TopDownCamera::live) return false;
        if(nativeCam) {if(nativeCam->frontend()) return false;const uint32_t m=nativeCam->mode();if(m==1||m==2) {settle=24;return false;}
            // 0.8.94.19: never enlarge the engine view while the game moves its camera (report 20261005_100309: a possession dive in table mode with
            // the expansion x8.70 / far x2 -> 3 s stall in the tile generation, render memory exhausted, exit(-1) at 0x57CFC4).
            // Only a flight to / from a creature (target 1 / 2, or the possession camera limits already loaded: zoom range 0..0); the game's short jumps
            // to an event (500 ms, table to table) keep the expansion.
            uint32_t e,st,t;const bool toCreature=nativeCam->transition(e,st,t)&&(t==1||t==2);
            if(toCreature||(m==3&&nativeCam->zoomRangeZero())) {settle=12;InterlockedIncrement(&transitionSkips);return false;}}
        if(settle>0) {--settle;return false;}
        if(!PoolGrow::safe()) return false;
        return true;
    }
    static void* __cdecl generate() {
        if(!expansionAllowed()) {InterlockedIncrement(&skipped);return original();}
        if(rectValid) {
            // 0.5.16: the engine's rectangle becomes (original rectangle) U (table rectangle on the screen).
            float saved[5]={at(0x760aec),at(0x760af0),at(0x760af4),at(0x760af8),at(0x760b04)};
            // 0.8.58: the union with the game's normal screen rectangle made the engine build and transform the WHOLE screen
            // (the table's rectangle is ~17 % of it: 70-89 % of the triangles were cut away afterwards by the table planes, 0.8.57.1
            // measurement). With TableRectReplace=1 the engine's rectangle IS the table's rectangle (never smaller than one pixel).
            static const bool replace=iniInt(L"TableRectReplace",1)!=0;
            if(replace&&rectR-rectL>8.f&&rectB-rectT>8.f) {
                at(0x760aec)=float(rectL);at(0x760af0)=float(rectR);at(0x760af4)=float(rectT);at(0x760af8)=float(rectB);
            } else {
            at(0x760aec)=std::min(saved[0],float(rectL));at(0x760af0)=std::max(saved[1],float(rectR));
            at(0x760af4)=std::min(saved[2],float(rectT));at(0x760af8)=std::max(saved[3],float(rectB));}
            at(0x760b04)=saved[4]*std::max(farScale,float(autoFar));
            void* result=original();
            at(0x760aec)=saved[0];at(0x760af0)=saved[1];at(0x760af4)=saved[2];at(0x760af8)=saved[3];at(0x760b04)=saved[4];
            return result;
        }
        float ex=std::max(expand,float(autoX)),ey=expand,eb=std::max(expand,float(autoBottom)),fs=std::max(farScale,float(autoFar));
        if(ex<=1.001f&&eb<=1.001f&&fs<=1.001f) return original();
        float saved[5]={at(0x760aec),at(0x760af0),at(0x760af4),at(0x760af8),at(0x760b04)};
        float cx=(saved[0]+saved[1])*.5f,cy=(saved[2]+saved[3])*.5f;
        at(0x760aec)=cx+(saved[0]-cx)*ex;at(0x760af0)=cx+(saved[1]-cx)*ex;
        at(0x760af4)=cy+(saved[2]-cy)*std::max(ey,eb);at(0x760af8)=cy+(saved[3]-cy)*std::max(ey,eb); // 0.5.7: both edges (0.5.5/0.5.6: the player side stayed empty when only 0x760AF8 was moved; 0.4.7-0.5.4 moved both and the player side was filled)
        at(0x760b04)=saved[4]*fs;
        void* result=original();
        at(0x760aec)=saved[0];at(0x760af0)=saved[1];at(0x760af4)=saved[2];at(0x760af8)=saved[3];at(0x760b04)=saved[4];
        return result;
    }
    static void install(const NativeCamera& native) {
        expand=iniFloat(L"ViewExpand",1.15f,1.f,4.f);farScale=iniFloat(L"ViewFarScale",1.f,1.f,4.f);
        if(!native.supported||(expand<=1.001f&&farScale<=1.001f&&iniInt(L"TableView",1)==0)) {log("View expansion off");return;}
        base=native.base;nativeCam=&native;
        auto* call=reinterpret_cast<unsigned char*>(native.base+0x175c0b);
        int32_t rel=0;memcpy(&rel,call+1,4);
        if(call[0]!=0xe8||uintptr_t(call+5+rel)!=native.base+0x174820) {log("View expansion signature unsupported; native visibility kept");return;}
        original=reinterpret_cast<Generate>(native.base+0x174820);
        DWORD old=0;if(!VirtualProtect(call,5,PAGE_EXECUTE_READWRITE,&old)) return;
        int32_t target=int32_t(reinterpret_cast<uintptr_t>(&generate)-reinterpret_cast<uintptr_t>(call+5));
        memcpy(call+1,&target,4);DWORD ignored=0;VirtualProtect(call,5,old,&ignored);
        FlushInstructionCache(GetCurrentProcess(),call,5);
        log("View expansion installed: rectangle x%.2f, far distance x%.2f",expand,farScale);
    }
};
// The old relative DirectInput cursor can drift from the Windows cursor when
// Virtual Desktop supplies absolute mouse positions. Correct only the 2D cursor.
// 0.9.47.8: SendInput of the possession controls (mouse turn every frame, keys, buttons) was measured at ~25 % of the game thread's CPU time in
// possession (report 133308: ~3 ms per game frame inside win32u). The events now go through one worker thread, in the same order (adjacent mouse
// moves are added up). InputAsync=0 sends them from the game thread as before.
// 0.9.47.10 KeyPoll: F11 / F12 were read with GetAsyncKeyState on the game thread every 4th frame; in the 150258 report each call cost ~1 to 2.4 ms
// (win32k input lock shared with the other input users of the desktop: 2.2 % of the game thread's samples in the dungeon). A small thread reads them
// every 20 ms instead and hands over RISING EDGES (a short tap is never missed); the game thread only reads two atomics. KeyPollThread=0 = old polling.
struct KeyPoll {
    inline static volatile LONG state=0;inline static bool enabled=false;
    inline static volatile LONG edge11=0,edge12=0;inline static volatile LONG64 polls=0;
    static DWORD WINAPI worker(void*) {
        bool d11=false,d12=false;
        for(;;) {
            const bool a=(GetAsyncKeyState(VK_F11)&0x8000)!=0,b=(GetAsyncKeyState(VK_F12)&0x8000)!=0;
            if(a&&!d11) InterlockedExchange(&edge11,1);
            if(b&&!d12) InterlockedExchange(&edge12,1);
            d11=a;d12=b;InterlockedIncrement64(&polls);Sleep(20);
        }
        return 0;
    }
    static bool ready() {
        if(kPublicBuild) return false;   // 0.9.47.60: no key thread in the public build
        static const bool want=iniInt(L"KeyPollThread",1)!=0;
        if(!want) return false;
        if(state==0&&InterlockedCompareExchange(&state,1,0)==0) {
            HANDLE t=CreateThread(nullptr,0,&worker,nullptr,0,nullptr);
            if(t) {SetThreadPriority(t,THREAD_PRIORITY_BELOW_NORMAL);CloseHandle(t);enabled=true;log("Key poll thread (KeyPollThread=1, 0.9.47.10): F11 / F12 read every 20 ms outside the game thread");}
            else log("Key poll thread: not created, F11 / F12 read on the game thread");
            InterlockedExchange(&state,2);
        }
        return enabled;
    }
    static bool take11() {return InterlockedExchange(&edge11,0)!=0;}
    static bool take12() {return InterlockedExchange(&edge12,0)!=0;}
};
// 0.9.47.12 WorkerAffinity (see AffinityPolicy.hpp). The game process is restricted to ONE core (0.9.47.11 report: affinity 0x1 of 0xFFFF). The process mask is
// widened to every core, the game's main thread and every thread that is not the driver / Direct3D / DK2VR keep the ORIGINAL mask, and the threads of the
// driver, of Direct3D and of DK2VR may use every core. A keeper thread repeats the pass every 2 s (threads created later inherit the wide mask; a wrapper
// that narrows the process mask again is widened again). WorkerAffinity=0 in DK2VR.ini = untouched.
struct ProcessAffinity {
    inline static bool tried=false,active=false;
    inline static DWORD_PTR original=0,system=0;
    inline static DWORD gameTid=0;
    inline static volatile LONG widenedAgain=0,freed=0,kept=0,failures=0,passes=0;
    inline static volatile LONG logs=0;
    typedef LONG (NTAPI *QueryThreadFn)(HANDLE,ULONG,PVOID,ULONG,PULONG);
    static std::string moduleOf(HANDLE h,QueryThreadFn q) {
        PVOID start=nullptr;ULONG ret=0;
        if(!q||q(h,9,&start,sizeof(start),&ret)<0||!start) return std::string();
        HMODULE m=nullptr;
        if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(start),&m)||!m) return std::string();
        wchar_t w[MAX_PATH]{};if(!GetModuleFileNameW(m,w,MAX_PATH)) return std::string();
        std::string out;for(const wchar_t* c=w;*c;++c) out.push_back(*c<128?char(*c):'?');
        return out;
    }
    // One pass over the threads of the process. First = true writes a summary to the log.
    static void pass(bool first) {
        static QueryThreadFn q=reinterpret_cast<QueryThreadFn>(GetProcAddress(GetModuleHandleA("ntdll.dll"),"NtQueryInformationThread"));
        DWORD_PTR pm=0,sm=0;
        if(GetProcessAffinityMask(GetCurrentProcess(),&pm,&sm)&&pm!=sm&&pm!=0&&sm!=0) {   // somebody narrowed it again
            if(SetProcessAffinityMask(GetCurrentProcess(),system)) {InterlockedIncrement(&widenedAgain);if(logs<6) {InterlockedIncrement(&logs);log("WorkerAffinity: the process mask was 0x%llX again, widened to 0x%llX",(unsigned long long)pm,(unsigned long long)system);}}
        }
        HANDLE snap=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
        if(snap==INVALID_HANDLE_VALUE) {InterlockedIncrement(&failures);return;}
        struct Seen {std::string module;int freed=0,kept=0;};
        std::map<std::string,Seen> seen;
        THREADENTRY32 te{};te.dwSize=sizeof(te);
        const DWORD me=GetCurrentProcessId();
        if(Thread32First(snap,&te)) do {
            if(te.th32OwnerProcessID!=me) continue;
            HANDLE h=OpenThread(THREAD_SET_INFORMATION|THREAD_QUERY_INFORMATION,FALSE,te.th32ThreadID);
            if(!h) {InterlockedIncrement(&failures);continue;}
            const bool isGame=te.th32ThreadID==gameTid;
            const std::string mod=isGame?std::string("(game thread)"):moduleOf(h,q);
            const DWORD_PTR want=DWORD_PTR(affinity::maskFor(isGame,mod,original,system));
            const DWORD_PTR prev=SetThreadAffinityMask(h,want);
            if(!prev) InterlockedIncrement(&failures);
            else if(want==system&&prev!=system) InterlockedIncrement(&freed);
            else if(want!=system) InterlockedIncrement(&kept);
            if(first) {Seen& e=seen[affinity::baseNameLower(mod)];e.module=mod;if(want==system) ++e.freed;else ++e.kept;}
            CloseHandle(h);
        } while(Thread32Next(snap,&te));
        CloseHandle(snap);
        InterlockedIncrement(&passes);
        if(first) for(auto& [k,e]:seen) log("WorkerAffinity thread group: start module %s -> %d thread(s) on every core, %d on the original core(s)",k.empty()?"(unknown)":k.c_str(),e.freed,e.kept);
    }
    static DWORD WINAPI keeper(void*) {
        for(;;) {Sleep(2000);pass(false);}
        return 0;
    }
    template<class Log> static void apply(DWORD gameThreadId,Log logf) {
        if(tried) return;
        tried=true;
        if(iniInt(L"WorkerAffinity",1)==0) {logf("WorkerAffinity (0.9.47.12): off (WorkerAffinity=0)");return;}
        DWORD_PTR pm=0,sm=0;
        if(!GetProcessAffinityMask(GetCurrentProcess(),&pm,&sm)) {logf("WorkerAffinity: process affinity unreadable, untouched");return;}
        if(!affinity::restricted(uint64_t(pm),uint64_t(sm))) {logf("WorkerAffinity (0.9.47.12): the process may already use mask 0x%llX of 0x%llX, nothing to do",(unsigned long long)pm,(unsigned long long)sm);return;}
        original=pm;system=sm;gameTid=gameThreadId;
        if(!SetProcessAffinityMask(GetCurrentProcess(),system)) {logf("WorkerAffinity: widening the process mask refused (error %lu), untouched",(unsigned long)GetLastError());return;}
        logf("WorkerAffinity (0.9.47.12): the game process was limited to mask 0x%llX of 0x%llX; widened to every core, the game's main thread and the game's own threads stay on 0x%llX, the threads of the graphics driver / Direct3D / DK2VR may use every core",(unsigned long long)pm,(unsigned long long)sm,(unsigned long long)pm);
        pass(true);
        active=true;
        HANDLE t=CreateThread(nullptr,0,&keeper,nullptr,0,nullptr);
        if(t) {SetThreadPriority(t,THREAD_PRIORITY_BELOW_NORMAL);CloseHandle(t);}
        else logf("WorkerAffinity: keeper thread not created (threads created later keep the wide mask)");
    }
    static void statusLine() {
        if(!active) return;
        DWORD_PTR pm=0,sm=0;GetProcessAffinityMask(GetCurrentProcess(),&pm,&sm);
        log("WorkerAffinity (0.9.47.12): process mask now 0x%llX of 0x%llX (original 0x%llX) | passes %ld, threads moved to every core %ld, kept on the original core(s) %ld, refusals %ld, mask narrowed again by others %ld",
            (unsigned long long)pm,(unsigned long long)sm,(unsigned long long)original,long(passes),long(freed),long(kept),long(failures),long(widenedAgain));
    }
};
// 0.9.47.13 FastBlt installer: finds the DxWrapper's ddraw.dll among the loaded modules, checks that the 180 bytes of its software blit row loop are EXACTLY the
// bytes this patch was written for, and only then replaces the first 5 bytes by a jump to dk2vr_bltStub (one atomic 8-byte write). Unknown wrapper = untouched.
struct FastBlt {
    inline static bool tried=false,available=false,patched=false,ab=true;
    inline static unsigned char* site=nullptr;
    inline static unsigned char original[8]{};
    inline static unsigned long long togglesOn=0,togglesOff=0;
    inline static long long lastRows=0,lastPixels=0;
    static bool write(const unsigned char* bytes) {
        DWORD old=0;
        if(!VirtualProtect(site,8,PAGE_EXECUTE_READWRITE,&old)) return false;
        long long v;std::memcpy(&v,bytes,8);
        InterlockedExchange64(reinterpret_cast<volatile long long*>(site),v);
        FlushInstructionCache(GetCurrentProcess(),site,8);
        DWORD o2=0;VirtualProtect(site,8,old,&o2);
        return true;
    }
    static bool set(bool on) {
        if(!available||on==patched) return true;
        unsigned char b[8];
        if(on) {
            std::memcpy(b,original,8);b[0]=0xE9;
            const int32_t rel=int32_t(reinterpret_cast<intptr_t>(&dk2vr_bltStub)-(reinterpret_cast<intptr_t>(site)+5));
            std::memcpy(b+1,&rel,4);
        } else std::memcpy(b,original,8);
        if(!write(b)) return false;
        patched=on;if(on) ++togglesOn;else ++togglesOff;
        return true;
    }
    template<class Log> static void install(Log logf) {
        if(tried) return;
        tried=true;
        if(iniInt(L"FastBlt",1)==0) {logf("FastBlt (0.9.47.16): off (FastBlt=0)");return;}
        ab=iniInt(L"FastBltAB",1)!=0;
        HMODULE mods[512];DWORD need=0;
        if(!EnumProcessModules(GetCurrentProcess(),mods,sizeof(mods),&need)) {logf("FastBlt: module list unreadable, untouched");return;}
        const size_t n=std::min<size_t>(need/sizeof(HMODULE),512);
        int checked=0;
        for(size_t i=0;i<n;++i) {
            unsigned char* base=reinterpret_cast<unsigned char*>(mods[i]);
            wchar_t name[MAX_PATH]{};GetModuleFileNameW(mods[i],name,MAX_PATH);
            const wchar_t* b=wcsrchr(name,L'\\');b=b?b+1:name;
            if(_wcsicmp(b,L"ddraw.dll")!=0) continue;
            ++checked;
            const IMAGE_DOS_HEADER* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
            if(dos->e_magic!=IMAGE_DOS_SIGNATURE) continue;
            const IMAGE_NT_HEADERS32* nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(base+dos->e_lfanew);
            if(nt->Signature!=IMAGE_NT_SIGNATURE||nt->FileHeader.Machine!=IMAGE_FILE_MACHINE_I386) {logf("FastBlt: %ls is not a 32-bit module, skipped",name);continue;}
            const uint32_t size=nt->OptionalHeader.SizeOfImage;
            if(size<fastblt::PatchRva+sizeof(fastblt::expected)) {logf("FastBlt: %ls (image %u bytes) is too small for the blit loop, untouched",name,unsigned(size));continue;}
            unsigned char* at=base+fastblt::PatchRva;
            MEMORY_BASIC_INFORMATION mbi{};
            if(!VirtualQuery(at,&mbi,sizeof(mbi))||mbi.State!=MEM_COMMIT||(reinterpret_cast<unsigned char*>(mbi.BaseAddress)+mbi.RegionSize)<at+sizeof(fastblt::expected)) {logf("FastBlt: %ls code not readable at the expected place, untouched",name);continue;}
            if(!fastblt::codeMatches(at)) {
                char hex[64]{};for(int k=0;k<16;++k) snprintf(hex+k*3,4,"%02X ",at[k]);
                logf("FastBlt (0.9.47.16): %ls at 0x%p: the code of the blit loop is NOT the one this patch knows (first bytes %s) -> untouched, the original loop keeps running",name,(void*)base,hex);
                continue;
            }
            site=at;std::memcpy(original,at,8);
            dk2vr_bltResume=base+fastblt::ResumeRva;
            available=true;
            if(set(true)) logf("FastBlt (0.9.47.16): %ls at 0x%p - blit loop (RVA 0x%X, %u bytes) verified byte for byte (the two float->int calls may already be rewritten by FastStretch) and replaced by the fast row copy; A/B windows %s",name,(void*)base,fastblt::PatchRva,unsigned(fastblt::PatchLength),ab?"ON (FastBltAB=1: 10 s with / 10 s without, for the report)":"off (always fast)");
            else {available=false;logf("FastBlt: the code page could not be written (error %lu), untouched",(unsigned long)GetLastError());}
            return;
        }
        logf("FastBlt (0.9.47.16): no matching ddraw.dll found (%d module(s) named ddraw.dll examined), untouched",checked);
    }
    static void statusLine() {
        if(!available) return;
        using namespace fastbltstat;
        const long long dr=rows-lastRows,dp=pixels-lastPixels;lastRows=rows;lastPixels=pixels;
        log("FastBlt (0.9.47.16): %s | blit rows in the last 5 s: %lld (%lld pixels, %.0f px/row) | total %lld rows, %lld pixels (stretched %.1f %%, mirrored %.1f %%, colour key %.1f %%) | checked rows %lld, %s | switches on/off %llu/%llu",
            patched?"ACTIVE":"switched off (A/B window)",dr,dp,dr?double(dp)/double(dr):0.,rows,pixels,rows?100.*stretchRows/double(rows):0.,rows?100.*mirrorRows/double(rows):0.,rows?100.*keyRows/double(rows):0.,verifiedRows,
            fallback?"MISMATCH FOUND -> original semantics used":"all identical to the original",togglesOn,togglesOff);
        {   // geometry of the blits and the measured cost of the fast routine against the plain reference routine (same rows)
            std::string g;char b[96];for(auto& e:geo) if(e.n) {snprintf(b,sizeof(b)," [%d px from %d, scale %.4f: %lld rows]",e.width,e.srcW,e.scale,e.n);g+=b;}
            double nsFast=0,nsRef=0;
            if(timedPixels>0&&owner) {
                LARGE_INTEGER q,f;QueryPerformanceCounter(&q);QueryPerformanceFrequency(&f);
                const double sec=double(q.QuadPart-qpc0)/double(f.QuadPart),tscHz=sec>0.5?double(static_cast<long long>(__builtin_ia32_rdtsc())-tsc0)/sec:0.;
                if(tscHz>1e6) {nsFast=1e9*double(ticksFast)/tscHz/double(timedPixels);nsRef=1e9*double(ticksRef)/tscHz/double(timedPixels);}
            }
            log("FastBlt geometry (0.9.47.16):%s | cost per pixel on %lld timed rows: fast routine %.3f ns, plain reference routine %.3f ns (ratio %.2f) | source index table: %llu reuses, %llu rebuilds | estimated time spent in this routine: %.1f ms per second of play (rows %lld/s); the original loop would cost about %.1f ms per second at the reference routine speed",
                g.empty()?" none":g.c_str(),timedRows,nsFast,nsRef,nsRef>0?nsFast/nsRef:0.,(unsigned long long)cache.hits,(unsigned long long)cache.builds,nsFast*double(dp)*1e-6/5.,dr/5,nsRef*double(dp)*1e-6/5.);
        }
    }
};
struct InputPump {
    struct Item {INPUT in;bool cursor;int x,y;};
    inline static bool enabled=false;inline static volatile LONG state=0;   // 0 not started, 1 starting, 2 ready
    inline static CRITICAL_SECTION cs;inline static HANDLE wake=nullptr;
    inline static std::vector<Item> queue;
    inline static uint64_t events=0,batches=0,merged=0,maxBatch=0,cursors=0;inline static double sendMs=0,maxSendMs=0;
    static DWORD WINAPI worker(void*) {
        std::vector<Item> mine;std::vector<INPUT> run;
        for(;;) {
            WaitForSingleObject(wake,INFINITE);
            EnterCriticalSection(&cs);mine.swap(queue);queue.clear();LeaveCriticalSection(&cs);
            if(mine.empty()) continue;
            const double t0=milliseconds();uint64_t nIn=0,nCur=0,nBatch=0;
            auto flush=[&]{if(!run.empty()) {SendInput(UINT(run.size()),run.data(),sizeof(INPUT));nIn+=run.size();++nBatch;run.clear();}};
            for(const Item& it:mine) {
                if(it.cursor) {flush();SetCursorPos(it.x,it.y);++nCur;}
                else run.push_back(it.in);
            }
            flush();
            const double dt=milliseconds()-t0;
            EnterCriticalSection(&cs);events+=nIn;cursors+=nCur;batches+=nBatch;maxBatch=std::max<uint64_t>(maxBatch,mine.size());sendMs+=dt;maxSendMs=std::max(maxSendMs,dt);LeaveCriticalSection(&cs);
            mine.clear();
        }
        return 0;
    }
    static void start() {
        if(InterlockedCompareExchange(&state,1,0)!=0) {while(state==1) Sleep(0);return;}
        InitializeCriticalSection(&cs);
        wake=CreateEventW(nullptr,FALSE,FALSE,nullptr);
        HANDLE t=wake?CreateThread(nullptr,0,&worker,nullptr,0,nullptr):nullptr;
        if(t) {SetThreadPriority(t,THREAD_PRIORITY_ABOVE_NORMAL);CloseHandle(t);enabled=true;log("Input pump (InputAsync=1): possession controls, VR pointer clicks and cursor placement sent from a worker thread");}
        else log("Input pump: worker thread not created, controls sent from the game thread");
        InterlockedExchange(&state,2);
    }
    static bool ready() {
        static const bool want=iniInt(L"InputAsync",1)!=0;
        if(want&&state==0) start();
        return enabled;
    }
    static void push(const Item& it) {
        EnterCriticalSection(&cs);
        if(!queue.empty()) {
            Item& b=queue.back();
            if(it.cursor&&b.cursor) {b.x=it.x;b.y=it.y;++merged;LeaveCriticalSection(&cs);SetEvent(wake);return;}   // only the last position matters
            if(!it.cursor&&!b.cursor&&it.in.type==INPUT_MOUSE&&it.in.mi.dwFlags==MOUSEEVENTF_MOVE&&b.in.type==INPUT_MOUSE&&b.in.mi.dwFlags==MOUSEEVENTF_MOVE) {
                b.in.mi.dx+=it.in.mi.dx;b.in.mi.dy+=it.in.mi.dy;++merged;LeaveCriticalSection(&cs);SetEvent(wake);return;
            }
        }
        queue.push_back(it);
        LeaveCriticalSection(&cs);
        SetEvent(wake);
    }
    static void send(const INPUT& in) {
        if(!ready()) {INPUT copy=in;SendInput(1,&copy,sizeof(copy));return;}
        push(Item{in,false,0,0});
    }
    static void setCursor(int x,int y) {
        if(!ready()) {SetCursorPos(x,y);return;}
        push(Item{INPUT{},true,x,y});
    }
    template<class Log> static void report(Log logf) {
        if(!enabled) return;
        EnterCriticalSection(&cs);
        const uint64_t e=events,b=batches,m=merged,mb=maxBatch,c=cursors;const double ms=sendMs,mx=maxSendMs;events=batches=merged=maxBatch=cursors=0;sendMs=maxSendMs=0;
        LeaveCriticalSection(&cs);
        if(e||c) logf("Input pump (last 5 s): %llu input events in %llu batches + %llu cursor placements (%llu merged, largest queue %llu); worker thread time %.2f ms in total, worst round %.2f ms (no longer spent by the game thread)",
            (unsigned long long)e,(unsigned long long)b,(unsigned long long)c,(unsigned long long)m,(unsigned long long)mb,ms,mx);
    }
};
struct NativeMouse {
    using Update = void (__thiscall*)(void*);
    inline static NativeMouse* active=nullptr;
    const NativeCamera& camera;
    HWND window=nullptr;
    Update original=nullptr;
    bool enabled=true;
    NativeMouse(const NativeCamera& c):camera(c) {
        savings=iniInt(L"SyscallSavings",1)!=0;
        auto ini=configurationPath();
        enabled=GetPrivateProfileIntW(L"VR",L"MouseCoordinates",1,ini.c_str())!=0;
        if(!enabled||!camera.supported) return;
        auto* entry=reinterpret_cast<unsigned char*>(camera.base+0x1dd630);
        // Complete instructions: push esi; mov esi,ecx; fild [esi+34h].
        const unsigned char expected[]={0x56,0x8b,0xf1,0xdb,0x46,0x34};
        if(memcmp(entry,expected,sizeof(expected))) {log("Mouse signature unsupported; native cursor kept");return;}
        auto* trampoline=static_cast<unsigned char*>(VirtualAlloc(nullptr,11,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
        if(!trampoline) return;
        memcpy(trampoline,entry,6);trampoline[6]=0xe9;
        auto back=int32_t(reinterpret_cast<uintptr_t>(entry+6)-reinterpret_cast<uintptr_t>(trampoline+11));
        memcpy(trampoline+7,&back,4);
        DWORD old=0;
        if(!VirtualProtect(trampoline,11,PAGE_EXECUTE_READ,&old)) {VirtualFree(trampoline,0,MEM_RELEASE);return;}
        original=reinterpret_cast<Update>(trampoline);active=this;
        if(!VirtualProtect(entry,6,PAGE_EXECUTE_READWRITE,&old)) {original=nullptr;active=nullptr;VirtualFree(trampoline,0,MEM_RELEASE);return;}
        unsigned char jump[6]={0xe9,0,0,0,0,0x90};
        auto relative=int32_t(reinterpret_cast<uintptr_t>(&hook)-reinterpret_cast<uintptr_t>(entry+5));
        memcpy(jump+1,&relative,4);memcpy(entry,jump,6);
        DWORD ignored=0;VirtualProtect(entry,6,old,&ignored);
        FlushInstructionCache(GetCurrentProcess(),entry,6);FlushInstructionCache(GetCurrentProcess(),trampoline,11);
        log("Windows mouse coordinates enabled (letterbox-aware, native possession input preserved)");
    }
    inline static bool savings=true; // 0.8.56.1: SyscallSavings=0 restores the 0.8.54.2 behaviour exactly
    struct Cache {ULONGLONG t=0;bool valid=false,fg=false,mid=false,alt=false,clientOk=false;RECT client{};} cache;
    void correct(void* object) {
        if(!object||!window||!readable(object,0x64)) return;
        // 0.8.55: window/keys/client rectangle refreshed every 50 ms instead of 5 system calls per call (measured syscall cost).
        {const ULONGLONG now=GetTickCount64();
         if(!savings||now-cache.t>=50||!cache.valid) {cache.t=now;cache.valid=true;cache.fg=GetForegroundWindow()==window&&!IsIconic(window);
             cache.mid=(GetAsyncKeyState(VK_MBUTTON)&0x8000)!=0;cache.alt=(GetAsyncKeyState(VK_MENU)&0x8000)!=0;cache.clientOk=GetClientRect(window,&cache.client)!=0;}}
        if(!cache.fg) return;
        auto mode=camera.mode();
        if(mode==1||mode==2||cache.mid||cache.alt) return;
        auto* bytes=static_cast<unsigned char*>(object);
        int bounds[4]{};memcpy(bounds,bytes+0x34,sizeof(bounds));
        float w=float(bounds[2]-bounds[0]),h=float(bounds[3]-bounds[1]);
        RECT client=cache.client;POINT cursor{};
        if(w<=1||h<=1||!cache.clientOk) return;
        if(pointerOn) {
            // 0.6.0: VR pointer on a HUD panel = fraction of the back buffer (= client area).
            cursor.x=LONG(pointerX*client.right);cursor.y=LONG(pointerY*client.bottom);
        } else if(!GetCursorPos(&cursor)||!ScreenToClient(window,&cursor)) return;
        float scaleFactor=std::min(float(client.right)/w,float(client.bottom)/h);
        if(scaleFactor<=0) return;
        float x=std::clamp((cursor.x-(client.right-w*scaleFactor)*.5f)/scaleFactor+bounds[0],float(bounds[0]),float(bounds[2]));
        float y=std::clamp((cursor.y-(client.bottom-h*scaleFactor)*.5f)/scaleFactor+bounds[1],float(bounds[1]),float(bounds[3]));
        // Verified on this executable: floating position +20h/+24h, not stale
        // field-name prefixes in the reverse-engineering map (+1Ch/+20h).
        memcpy(bytes+0x20,&x,4);memcpy(bytes+0x24,&y,4);
    }
    // 0.6.1: position of the game cursor in back-buffer pixels (same letterbox as correct()).
    bool cursorPixel(float W,float H,float& px,float& py,float* scaleOut=nullptr) const {
        if(!camera.supported) return false;
        auto* o=reinterpret_cast<unsigned char*>(uintptr_t(camera.read<uint32_t>(0x79cff4)));
        if(!o||!readable(o,0x64)) return false;
        int b[4]{};memcpy(b,o+0x34,sizeof(b));float w=float(b[2]-b[0]),h=float(b[3]-b[1]);
        if(w<=1||h<=1) return false;
        float x=0,y=0;memcpy(&x,o+0x20,4);memcpy(&y,o+0x24,4);
        float k=std::min(W/w,H/h);px=(x-b[0])*k+(W-w*k)*.5f;py=(y-b[1])*k+(H-h*k)*.5f;if(scaleOut) *scaleOut=k;
        return std::isfinite(px)&&std::isfinite(py);
    }
    static void __fastcall hook(void* self,void*) {
        if(active) {active->correct(self);active->original(self);}
    }
    // 0.6.0: VR pointer (host ray on a HUD panel). The Windows cursor follows it so that the
    // synthetic clicks land in the game window; buttons are sent with SendInput on edges only.
    bool pointerOn=false,leftDown=false,rightDown=false;float pointerX=0,pointerY=0;LONG lastX=-1,lastY=-1;uint32_t reassert=0;
    uint32_t pointerLogged=0;
    struct PtrCache {ULONGLONG t=0;bool valid=false;RECT client{};POINT origin{};} ptrCache;   // 0.9.47.8
    void pointer(const Request& r) {
        bool on=(r.pointerFlags&PointerActive)&&!(r.pointerFlags&PointerComfort)&&std::isfinite(r.pointerX)&&std::isfinite(r.pointerY);
        bool wantLeft=on&&(r.pointerFlags&PointerLeft),wantRight=on&&(r.pointerFlags&PointerRight);
        if(on) {
            pointerX=std::clamp(r.pointerX,0.f,1.f);pointerY=std::clamp(r.pointerY,0.f,1.f);
            RECT client{};
            // 0.9.47.8: client size and screen origin refreshed every 50 ms (two system calls per game frame before); the window never moves between two frames in practice
            bool haveClient=false;POINT origin{0,0};
            if(window) {
                const ULONGLONG nowT=GetTickCount64();
                if(!savings||!ptrCache.valid||nowT-ptrCache.t>=50) {
                    RECT c{};POINT o{0,0};
                    if(GetClientRect(window,&c)&&ClientToScreen(window,&o)) {ptrCache.client=c;ptrCache.origin=o;ptrCache.valid=true;ptrCache.t=nowT;}
                    else ptrCache.valid=false;
                }
                if(ptrCache.valid) {client=ptrCache.client;origin=ptrCache.origin;haveClient=true;}
            }
            if(haveClient) {
                POINT p{origin.x+LONG(pointerX*client.right),origin.y+LONG(pointerY*client.bottom)};
                // 0.8.55: SetCursorPos only when the target changed (or every 8th call, to re-assert it).
                if(!savings||p.x!=lastX||p.y!=lastY||(++reassert&7)==0) {InputPump::setCursor(p.x,p.y);lastX=p.x;lastY=p.y;}
            }
        }
        if(on!=pointerOn&&pointerLogged<20) {++pointerLogged;log("VR pointer %s",on?"on a HUD panel":"off");}
        if(!on&&pointerOn&&window) {
            // 0.6.2: leaving a panel parks the cursor in the middle of the game view (a cursor left on an
            // edge made the game scroll the map continuously).
            RECT client{};if(GetClientRect(window,&client)) {POINT p{client.right/2,client.bottom/2};if(ClientToScreen(window,&p)) InputPump::setCursor(p.x,p.y);}
        }
        if(!on) lastX=lastY=-1;
        pointerOn=on;
        auto send=[](DWORD flag){INPUT in{};in.type=INPUT_MOUSE;in.mi.dwFlags=flag;InputPump::send(in);};   // 0.9.47.9: after SetCursorPos, same queue = same order
        if(wantLeft!=leftDown) {send(wantLeft?MOUSEEVENTF_LEFTDOWN:MOUSEEVENTF_LEFTUP);leftDown=wantLeft;}
        if(wantRight!=rightDown) {send(wantRight?MOUSEEVENTF_RIGHTDOWN:MOUSEEVENTF_RIGHTUP);rightDown=wantRight;}
    }
    void tick(IDirect3DDevice9* device) {
        if(!original) return;
        D3DDEVICE_CREATION_PARAMETERS cp{};
        if(SUCCEEDED(device->GetCreationParameters(&cp))) window=cp.hFocusWindow;
        auto* mouse=reinterpret_cast<void*>(camera.read<uint32_t>(0x79cff4));
        if(mouse&&readable(mouse,0x74)) {correct(mouse);original(mouse);}
    }
};
// OpenXR possession controls are translated to the game's original first-person
// keyboard/mouse controls. No permanent rebinding or change to the game files.
struct PossessionInput {
    bool keys[5]{},attack=false,exitDown=false,active=false;
    uint32_t slot=0;int slotFrames=0;float lastTurn=0;
    // 0.9.47.39: A = next skill, B = previous skill, right trigger = attack with the chosen skill, left X = leave. The skill is sent as the game's own key 1..6
    // (the pattern that worked since 0.9: key for two game frames + left click). Count: DK2VR.ini PossessionAbilityCount (1..6, default 6).
    int sel=1;bool aPrev=false,bPrev=false;
    inline static std::atomic<int> shownSel{0},shownCount{0},detected{0};   // 0.9.47.41: detected = number of skill icons the game draws (0 = unknown)
    float pixelsPerRadian=iniFloat(L"PossessionMousePixelsPerRadian",420.f,50.f,2000.f);
    // 0.8.67: the turn is sent as whole mouse pixels. 'injected' is the commanded turn really delivered; the sub-pixel remainder stays
    // in (turn - injected) and is sent later (0.8.66 dropped it: slow turns stalled and fast ones jittered).
    bool carry=iniInt(L"PossessionTurnCarry",1)!=0;
    // 0.8.80: closed loop 'the game follows the view': extra mouse pixels (never part of the commanded turn) until the game's heading agrees with the stick-driven view
    HeadingFollow follow;LARGE_INTEGER followT{};uint64_t followPixels=0;double followAbs=0,followMax=0;uint64_t followTicks=0,followActive=0;
    float injected=0;float cum[8]{};int skipped=0;           // cum[0] = injected after the previous game frame's tick, cum[1] = the frame before, ...
    // Response of the game to the injected turn, measured on its camera heading: delay in game frames and gain (diagnostic + prediction).
    struct Sample {float dm;float dc[4];};
    Sample ring[48]{};int ringCount=0,ringHead=0,sinceFit=0;float prevYaw=0;bool prevValid=false;
    int lag=0;float gain=1.f,quality=0.f;bool fitted=false;uint64_t fits=0,fitFailures=0;
    double predSum=0,predMax=0;uint64_t predFrames=0;
    // 0.8.80 360 VIEW: a game frame drawn with the camera turned (side / rear view) has no usable heading: the next measurement then spans 2 game frames
    void skipObserve() {if(active&&prevValid) ++skipped;}
    void observe(bool ok,float yaw) {
        if(!active||!ok) {prevValid=false;skipped=0;return;}
        if(prevValid) {
            const int span=std::clamp(1+skipped,1,3);
            Sample s{};s.dm=wrapRadians(yaw-prevYaw);for(int i=0;i<4;++i) s.dc[i]=cum[i]-cum[i+span];
            ring[ringHead]=s;ringHead=(ringHead+1)%48;ringCount=std::min(ringCount+1,48);
        }
        prevYaw=yaw;prevValid=true;skipped=0;
        if(++sinceFit>=12) {sinceFit=0;fit();}
    }
    void fit() {
        double sdc2[4]{},sdmdc[4]{},sdm2=0,moved=0;
        for(int k=0;k<ringCount;++k) {
            const Sample& s=ring[k];sdm2+=double(s.dm)*s.dm;moved+=std::fabs(s.dc[0]);
            for(int i=0;i<4;++i) {sdc2[i]+=double(s.dc[i])*s.dc[i];sdmdc[i]+=double(s.dm)*s.dc[i];}
        }
        if(ringCount<12||moved<.15||sdm2<1e-6) return; // not turning: keep the previous estimate
        int best=-1;double bestErr=1e30,bestGain=0;
        for(int i=0;i<4;++i) {
            if(sdc2[i]<1e-8) continue;
            const double g=sdmdc[i]/sdc2[i],err=sdm2-2*g*sdmdc[i]+g*g*sdc2[i];
            if(err<bestErr) {bestErr=err;best=i;bestGain=g;}
        }
        ++fits;
        const double q=best>=0?1.-bestErr/sdm2:0.;
        const double mag=std::fabs(bestGain);
        if(best<0||q<.5||mag<.6||mag>1.6) {++fitFailures;return;}
        quality=float(q);lag=best;gain=fitted?gain*.7f+float(mag)*.3f:float(mag);fitted=true;
    }
    float content() const {return cum[std::clamp(lag,0,4)];}
    void shiftHistory() {for(int i=7;i>0;--i) cum[i]=cum[i-1];cum[0]=injected;}
    static void key(int vk,bool down) {
        INPUT in{};in.type=INPUT_KEYBOARD;in.ki.wScan=WORD(MapVirtualKeyW(UINT(vk),MAPVK_VK_TO_VSC));
        in.ki.dwFlags=KEYEVENTF_SCANCODE|(down?0:KEYEVENTF_KEYUP)|((vk==VK_UP||vk==VK_DOWN||vk==VK_LEFT||vk==VK_RIGHT)?KEYEVENTF_EXTENDEDKEY:0);
        if(in.ki.wScan) InputPump::send(in);
    }
    static void mouse(DWORD flag) {INPUT in{};in.type=INPUT_MOUSE;in.mi.dwFlags=flag;InputPump::send(in);}
    void release() {
        static const int vk[5]={VK_UP,VK_DOWN,VK_LEFT,VK_RIGHT,VK_SHIFT};
        for(int i=0;i<5;++i) if(keys[i]) {key(vk[i],false);keys[i]=false;}
        if(attack) mouse(MOUSEEVENTF_LEFTUP);
        if(exitDown) mouse(MOUSEEVENTF_RIGHTUP);
        attack=exitDown=active=false;follow.clear();
        if(slotFrames&&slot>=1&&slot<=6) key('0'+int(slot),false);
        slotFrames=0;slot=0;aPrev=bPrev=false;shownSel=0;shownCount=0;detected=0;
    }
    void tick(const Request& r,HWND window,bool possessed,bool paused) {
        if(!possessed||paused||!window||GetForegroundWindow()!=window||IsIconic(window)||!r.valid) {release();return;}
        if(!active) {active=true;lastTurn=r.possessionTurn;injected=r.possessionTurn;for(float& c:cum) c=injected;ringCount=ringHead=sinceFit=0;prevValid=false;follow.clear();followT.QuadPart=0;sel=1;aPrev=bPrev=false;slot=0;log("Possession input (0.9.47.39): stable tracked head, arrows, left stick click=run, right trigger=attack, A=next skill, B=previous skill, left X=leave");}
        else {
            const float body=std::clamp(r.possessionTurn-(carry?injected:lastTurn),-Pi/2,Pi/2);
            const int dxBody=int(std::lround(body*pixelsPerRadian));int dx=dxBody;
            {   // 0.8.80: the extra turn that brings the game's heading back to the view's (separate sub-pixel carry)
                LARGE_INTEGER now{},fq{};QueryPerformanceCounter(&now);QueryPerformanceFrequency(&fq);
                const double dtS=followT.QuadPart&&fq.QuadPart?double(now.QuadPart-followT.QuadPart)/double(fq.QuadPart):0.;followT=now;
                float e=0,g=2.7f;const bool ok=!(r.comfortFlags&ComfortSnapTurn)&&HeadingFollowShared::read(e,g);
                follow.loadShared();const int ex=follow.pixels(follow.step(ok,e,g,dtS),pixelsPerRadian);
                ++followTicks;if(ok&&std::fabs(follow.lp)>follow.deadDeg*.01745f) ++followActive;
                followAbs+=std::fabs(double(e))*57.29578;followMax=std::max(followMax,std::fabs(double(e))*57.29578);
                if(ex) {dx+=ex;followPixels+=uint64_t(std::abs(ex));}
            }
            if(dx) {INPUT in{};in.type=INPUT_MOUSE;in.mi.dwFlags=MOUSEEVENTF_MOVE;in.mi.dx=dx;InputPump::send(in);}
            if(carry) injected+=float(dxBody)/pixelsPerRadian; else injected=r.possessionTurn;
            lastTurn=r.possessionTurn;
        }
        const bool want[5]={r.possessionMoveY>.32f,r.possessionMoveY<-.32f,r.possessionMoveX<-.32f,r.possessionMoveX>.32f,(r.possessionFlags&PossessionRun)!=0};
        static const int vk[5]={VK_UP,VK_DOWN,VK_LEFT,VK_RIGHT,VK_SHIFT};
        for(int i=0;i<5;++i) if(want[i]!=keys[i]) {key(vk[i],want[i]);keys[i]=want[i];}
        static const int abilitiesIni=std::clamp(iniInt(L"PossessionAbilityCount",6),1,6);
        static const bool autoCount=iniInt(L"PossessionSkillAutoCount",1)!=0;
        const int abilities=(autoCount&&detected.load()>0)?std::clamp(detected.load(),1,6):abilitiesIni;   // 0.9.47.41: only the skills the creature has
        if(sel>abilities) sel=abilities;
        const bool first=(r.possessionFlags&PossessionAttack)!=0,nextBtn=(r.possessionFlags&PossessionAbilityA)!=0,prevBtn=(r.possessionFlags&PossessionAbilityB)!=0;
        bool cycled=false;
        if(nextBtn&&!aPrev) {sel=sel%abilities+1;cycled=true;}
        if(prevBtn&&!bPrev) {sel=(sel+abilities-2)%abilities+1;cycled=true;}
        aPrev=nextBtn;bPrev=prevBtn;
        if(cycled) {static int logs=0;if(logs++<30) log("Possession skill (0.9.47.39): skill %d of %d chosen",sel,abilities);}
        shownSel=sel;shownCount=abilities;
        const uint32_t skillWanted=(first||cycled)?uint32_t(sel):0u;
        if(skillWanted&&(skillWanted!=slot||cycled)) {
            if(slotFrames&&slot>=1&&slot<=6) key('0'+int(slot),false);
            slot=skillWanted;key('0'+int(slot),true);slotFrames=2;
        } else if(slotFrames>0&&--slotFrames==0) key('0'+int(slot),false);
        if(first!=attack) {mouse(first?MOUSEEVENTF_LEFTDOWN:MOUSEEVENTF_LEFTUP);attack=first;}
        const bool leave=(r.possessionFlags&PossessionLeave)!=0;
        if(leave!=exitDown) {mouse(leave?MOUSEEVENTF_RIGHTDOWN:MOUSEEVENTF_RIGHTUP);exitDown=leave;}
        shiftHistory();
    }
};
// 0.9.47.39: the interface of the table is drawn again after a possession. Report 20261009_143345: after the FIRST return from a possession the game
// drew NO interface at all (UI=0, HUD texture 1.2 % covered, wall options and map hidden) until the player pressed ESCAPE (pause menu opened and closed:
// everything came back); the second return was normal. Verified: the pause flag stayed 0 and no menu was open, the game simply did not draw its bars.
// Repair: after a possession, if the table is shown (not paused, not a menu) and the game draws no interface for ~2.5 s, ESCAPE is tapped ONCE (what the
// player did by hand); if that opens the pause menu, a second tap closes it again. At most one repair per return, only while the game has the focus.
struct UiWatch {
    bool wasPoss=false,armed=false,tapped=false,second=false,done=false;ULONGLONG exitAt=0,tapAt=0;int noUi=0,hold=0;uint32_t repairs=0;
    static void tap(bool down) {PossessionInput::key(VK_ESCAPE,down);}
    void inPossession() {if(hold>0) {tap(false);hold=0;}wasPoss=true;armed=false;tapped=second=done=false;noUi=0;}
    void frame(bool possessed,bool table,bool uiDrawn,HWND window,bool valid) {
        if(possessed) {if(hold>0) {tap(false);hold=0;}wasPoss=true;armed=false;tapped=second=done=false;noUi=0;return;}
        if(wasPoss) {wasPoss=false;armed=iniInt(L"PossessionExitUiRepair",1)!=0;tapped=second=done=false;exitAt=GetTickCount64();noUi=0;}
        if(hold>0) {if(--hold==0) tap(false);return;}
        if(!armed||done) return;
        const ULONGLONG now=GetTickCount64();
        const bool focus=valid&&window&&GetForegroundWindow()==window&&!IsIconic(window);
        if(!tapped) {
            if(now-exitAt>25000) {armed=false;return;}
            if(!table||PauseWatch::paused==1||uiDrawn) {noUi=0;return;}
            if(++noUi<110||!focus) return;
            tap(true);hold=3;tapped=true;tapAt=now;++repairs;
            log("Interface repair (0.9.47.39): no game interface for %d frames after the return from a possession, ESCAPE tapped once (repair %u)",noUi,repairs);
            return;
        }
        if(now-tapAt<900) return;
        if(!second&&PauseWatch::paused==1&&focus) {tap(true);hold=3;second=true;log("Interface repair (0.9.47.39): the pause menu opened by the repair is closed again (second ESCAPE)");return;}
        done=true;log("Interface repair (0.9.47.39): finished, interface drawn %d, pause flag %ld",int(uiDrawn),long(PauseWatch::paused));
    }
};
// 0.9.1: SPACE and ESCAPE from the controller (Request::handFlags HandKeySpace / HandKeyEscape, set by the host): key down on press,
// key up on release, only while the game window has the focus (released at once otherwise). Menu, table and possession alike.
struct KeyButtons {
    bool down[2]{};uint32_t presses[2]{};
    void tick(const Request& r,HWND window) {
        const bool focus=r.valid&&window&&GetForegroundWindow()==window&&!IsIconic(window);
        const bool want[2]={focus&&(r.handFlags&HandKeySpace)!=0,focus&&(r.handFlags&HandKeyEscape)!=0};
        static const int vk[2]={VK_SPACE,VK_ESCAPE};
        for(int i=0;i<2;++i) if(want[i]!=down[i]) {
            PossessionInput::key(vk[i],want[i]);down[i]=want[i];
            if(want[i]&&presses[i]++<3) log("Controller key: %s pressed (button %s)",i?"ESCAPE":"SPACE",i?"Y":"X");
        }
    }
};
// 0.9.47.34: VR virtual keyboard (Keyboard.hpp). The host holds Request::keyboardPressed while the trigger is down on a key; here the key goes
// to the game as a REAL scan code (SendInput, like a physical keyboard: works for WM_CHAR text fields and for DirectInput alike), only while the
// game window has the focus. SHIFT is wrapped around the key when armed (or for the AZERTY digit row). Held keys repeat like a real keyboard.
struct KeyboardInput {
    int held=-1;bool shiftDown=false;double nextRepeat=0;uint64_t typed=0,repeats=0,skippedNoFocus=0;uint32_t logged=0;
    static void scan(uint8_t sc,bool down) {
        INPUT in{};in.type=INPUT_KEYBOARD;in.ki.wScan=sc;in.ki.dwFlags=KEYEVENTF_SCANCODE|(down?0:KEYEVENTF_KEYUP);InputPump::send(in);
    }
    void releaseAll(const KbKey* keys,int n) {
        if(held>=0&&held<n) scan(keys[held].scan,false);
        if(shiftDown) scan(0x2A,false);
        held=-1;shiftDown=false;
    }
    void tick(const Request& r,HWND window) {
        KbKey keys[KbMaxKeys];const int n=kbLayout(keys);
        const bool open=r.valid&&(r.keyboardFlags&KeyboardOpen)!=0;
        const bool focus=open&&window&&GetForegroundWindow()==window&&!IsIconic(window);
        int want=(focus&&r.keyboardPressed>=1&&int(r.keyboardPressed)<=n)?int(r.keyboardPressed)-1:-1;
        if(open&&!focus&&r.keyboardPressed) ++skippedNoFocus;
        if(want!=held) {
            releaseAll(keys,n);
            if(want>=0) {
                const KbKey& k=keys[want];
                const HKL hkl=GetKeyboardLayout(window?GetWindowThreadProcessId(window,nullptr):0);
                const bool armed=(r.keyboardFlags&KeyboardShift)!=0;
                const bool autoShift=k.kind==KbNormal&&kbDigitAuto(hkl,k.scan);
                const bool needShift=k.kind==KbNormal&&(autoShift?!armed:armed);
                if(needShift) {scan(0x2A,true);shiftDown=true;}
                scan(k.scan,true);held=want;++typed;nextRepeat=milliseconds()+KbRepeatDelayMs;
                if(logged++<8) log("Clavier VR : touche %d scancode 0x%02X envoyee au jeu (layout %08X, maj=%d)",want,unsigned(k.scan),unsigned(uintptr_t(hkl)&0xffffffffu),needShift?1:0);
            }
        } else if(held>=0) {
            const uint8_t kind=keys[held].kind;
            if((kind==KbNormal||kind==KbBackspace||kind==KbSpace)&&milliseconds()>=nextRepeat) {scan(keys[held].scan,true);nextRepeat=milliseconds()+KbRepeatEveryMs;++repeats;}
        }
    }
};
// 0.8.29: recover true orthographic model depth BEFORE the interface projection flattens RHW to 1.
// Provenance comes from the native model renderer, not texture identity or cursor proximity.
struct HeldGeometry {
    struct Key {uint32_t x,y,z;bool operator==(const Key& o)const{return x==o.x&&y==o.y&&z==o.z;}};
    struct Hash {size_t operator()(const Key& q)const{return (q.x*16777619u^q.y)*16777619u^q.z;}};
    using Model=uint32_t(__thiscall*)(void*,uint32_t,uint32_t);
    using Project=uint32_t(__cdecl*)(int,const float*);
    inline static Model model=nullptr;inline static Project project=nullptr;
    inline static uintptr_t k=0;inline static bool enabled=false,scope=false;
    inline static float ratio=0,originZ=0;
    struct Depth {float inputZ,originZ,ratio;};
    inline static std::unordered_map<Key,Depth,Hash> depths;
    inline static uint64_t calls=0,vertices=0,zeroScaleCalls=0,clampedVertices=0,projectedAll=0;inline static int reports=0,zeroScaleLogs=0;inline static bool inModel=false;
    static Key key(const void* v){Key q;memcpy(&q,v,12);return q;}
    __attribute__((force_align_arg_pointer)) static uint32_t __fastcall modelHook(void* self,void*,uint32_t a,uint32_t b) {
        const bool oldScope=scope;const float oldRatio=ratio,oldZ=originZ;
        float xy,z;memcpy(&xy,static_cast<unsigned char*>(self)+0x38,4);memcpy(&z,static_cast<unsigned char*>(self)+0x3c,4);
        memcpy(&originZ,static_cast<unsigned char*>(self)+0x10,4);
        ratio=std::fabs(z)>1e-8f?xy/z:0;scope=enabled&&std::isfinite(ratio)&&std::fabs(ratio)<1e6f&&ratio!=0;
        if(scope) ++calls;
        else if(enabled) {++zeroScaleCalls;if(zeroScaleLogs<6) {++zeroScaleLogs;log("Held 3D: a model is drawn with a Z scale of %g (XY scale %g): no depth can be read from it, its draws stay flat",double(z),double(xy));}}
        const bool oldIn=inModel;inModel=true;
        uint32_t r=model(self,a,b);inModel=oldIn;scope=oldScope;ratio=oldRatio;originZ=oldZ;return r;
    }
    __attribute__((force_align_arg_pointer)) static uint32_t __cdecl projectHook(int idx,const float* v) {
        float dz=0;const bool save=scope&&heldDepthPixels(v[2],originZ,ratio,1.f,dz);
        if(inModel) {++projectedAll;if(!(v[2]>6.1035e-5f&&v[2]<1.f)) ++clampedVertices;}   // the game clamps z to [6.1e-5, 1] in 0x58B190: such a vertex has lost its depth
        uint32_t r=project(idx,v);
        if(save&&uint32_t(idx)<1024&&depths.size()<65536) {
            auto* e=reinterpret_cast<unsigned char*>(k+0x76f358)+size_t(idx)*40;
            depths[key(e+12)]={v[2],originZ,ratio};++vertices;
        }
        return r;
    }
    static void install(uintptr_t base) {
        if(iniInt(L"HeldCreature3D",1)==0) return;   // 0.9.47.22: ON by default (was 0 since 0.8.31)
        k=base-0x400000;
        const unsigned char sm[]={0x81,0xec,0xa4,0,0,0,0x53},sp[]={0x83,0xec,0x14,0x8b,0x44,0x24,0x1c};
        if(memcmp(reinterpret_cast<void*>(k+0x577f10),sm,7)||memcmp(reinterpret_cast<void*>(k+0x58b190),sp,7)) {log("Held 3D: native signatures unsupported, complete 2D fallback");return;}
        model=reinterpret_cast<Model>(EngineOrigin::detour(k+0x577f10,sm,7,reinterpret_cast<void*>(&modelHook)));
        project=reinterpret_cast<Project>(EngineOrigin::detour(k+0x58b190,sp,7,reinterpret_cast<void*>(&projectHook)));
        enabled=model&&project;log("Held 3D: native model/projection hooks %s (577F10 / 58B190)",enabled?"ON":"failed, 2D fallback");
    }
};
// 0.6.4: the Keeper's hand cursor. DK2 draws its cursor in software (CursorDrawer 0x79F638, flame names):
// setCursorImage (0x5DC120, thiscall, 4 args) blits the current image into cursorSurf[0] (+0x54, a
// MyDdSurfaceEx) at (0,0), size +0x14/+0x18, hotspot = -(+0x40,+0x44). The pixels are read there (game lock
// 0x5B5BE0 / unlock 0x5B5C90), the colour key (+0x10 of that surface, as used by drawCursor) becomes alpha,
// and the image is shown on the controller. Every animation frame / held-creature icon goes through here.
struct KeeperCursor {
    using SetImage=int*(__thiscall*)(void*,int*,void*,const int*,const int*);
    using Resolve=int*(__cdecl*)(int*,void*,void**);
    using Unlock=void(__cdecl*)(void*);
    inline static KeeperCursor* active=nullptr;
    const NativeCamera& camera;
    SetImage original=nullptr;Resolve resolve=nullptr;Unlock unlock=nullptr;
    std::mutex m;std::vector<uint32_t> pixels;int w=0,h=0,hotX=0,hotY=0;uint32_t serial=0;
    uint64_t calls=0,captures=0,failures=0;uint32_t lastKey=0,lastBits=0,lastOpaque=0;
    static constexpr int MaxSize=256;
    KeeperCursor(const NativeCamera& c):camera(c) {
        if(!camera.supported||iniInt(L"KeeperHand",1)==0) return;
        auto at=[&](uint32_t va){return reinterpret_cast<unsigned char*>(camera.base+va-0x400000);};
        unsigned char* entry=at(0x5DC120);
        const unsigned char expected[]={0x8b,0x44,0x24,0x08,0x83,0xec,0x08};
        const unsigned char lockSig[]={0x83,0xec,0x6c,0xa0},unlockSig[]={0x8b,0x44,0x24,0x04,0x6a,0x00};
        if(memcmp(entry,expected,7)||memcmp(at(0x5B5BE0),lockSig,4)||memcmp(at(0x5B5C90),unlockSig,6)) {log("Keeper hand: cursor signatures unsupported");return;}
        resolve=reinterpret_cast<Resolve>(at(0x5B5BE0));unlock=reinterpret_cast<Unlock>(at(0x5B5C90));
        auto* t=static_cast<unsigned char*>(VirtualAlloc(nullptr,16,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
        if(!t) return;
        memcpy(t,entry,7);t[7]=0xe9;int32_t back=int32_t(reinterpret_cast<uintptr_t>(entry+7)-reinterpret_cast<uintptr_t>(t+12));memcpy(t+8,&back,4);
        DWORD old=0;VirtualProtect(t,16,PAGE_EXECUTE_READ,&old);
        original=reinterpret_cast<SetImage>(t);active=this;
        if(!VirtualProtect(entry,7,PAGE_EXECUTE_READWRITE,&old)) {active=nullptr;original=nullptr;return;}
        unsigned char jump[7]={0xe9,0,0,0,0,0x90,0x90};int32_t rel=int32_t(reinterpret_cast<uintptr_t>(&hook)-reinterpret_cast<uintptr_t>(entry+5));memcpy(jump+1,&rel,4);
        memcpy(entry,jump,7);DWORD ignored=0;VirtualProtect(entry,7,old,&ignored);
        FlushInstructionCache(GetCurrentProcess(),entry,7);FlushInstructionCache(GetCurrentProcess(),t,16);
        log("Keeper hand: software cursor images captured (CursorDrawer::setCursorImage hooked)");
    }
    static int* __fastcall hook(void* self,void*,int* res,void* src,const int* rect,const int* hot) {
        int* r=active->original(self,res,src,rect,hot);
        active->capture(self);
        return r;
    }
    static uint8_t channel(uint32_t v,uint32_t mask) {
        if(!mask) return 255;
        int shift=0;while(!((mask>>shift)&1)) ++shift;
        uint32_t bits=mask>>shift,value=(v&mask)>>shift;
        return uint8_t(bits?value*255/bits:0);
    }
    void capture(void* self) {
        ++calls;
        auto* cd=static_cast<unsigned char*>(self);
        if(!readable(cd,0x230)||!cd[5]) return;
        int cw=0,ch=0,ox=0,oy=0;memcpy(&cw,cd+0x14,4);memcpy(&ch,cd+0x18,4);memcpy(&ox,cd+0x40,4);memcpy(&oy,cd+0x44,4);
        if(cw<=0||ch<=0) return;
        cw=std::min(cw,MaxSize);ch=std::min(ch,MaxSize);
        unsigned char* surf=cd+0x54;
        if(!readable(surf,0x50)||!*reinterpret_cast<void**>(surf+0x24)) return;
        int result=0;void* bits=nullptr;
        resolve(&result,surf,&bits);
        if(result<0||!bits) {++failures;return;}
        {int pp=0,bb=0;memcpy(&pp,surf+0x4c,4);memcpy(&bb,surf+0x38,4);
         if(pp<=0||!readable(bits,size_t(pp)*(ch-1)+size_t(cw)*(bb/8))) {unlock(surf);++failures;return;}}
        uint32_t mR,mG,mB,mA,bpp;int pitch=0;
        memcpy(&mR,surf+0x28,4);memcpy(&mG,surf+0x2c,4);memcpy(&mB,surf+0x30,4);memcpy(&mA,surf+0x34,4);memcpy(&bpp,surf+0x38,4);memcpy(&pitch,surf+0x4c,4);
        uint32_t key=0;memcpy(&key,surf+0x10,4);
        const uint8_t kr=uint8_t(key>>16),kg=uint8_t(key>>8),kb=uint8_t(key);
        std::vector<uint32_t> out(size_t(cw)*ch,0);uint32_t opaque=0;
        const bool ok=(bpp==16||bpp==32)&&pitch>0;
        if(ok) for(int y=0;y<ch;++y) {
            const unsigned char* row=static_cast<const unsigned char*>(bits)+size_t(y)*pitch;
            for(int x=0;x<cw;++x) {
                uint32_t v=bpp==16?uint32_t(reinterpret_cast<const uint16_t*>(row)[x]):reinterpret_cast<const uint32_t*>(row)[x];
                uint8_t r=channel(v,mR),g=channel(v,mG),b=channel(v,mB);
                // Colour key compared at the precision of the surface (5/6 bit channels in 16-bit modes).
                const int tol=bpp==16?9:1;
                if(std::abs(r-kr)<tol&&std::abs(g-kg)<tol&&std::abs(b-kb)<tol) continue;
                out[size_t(y)*cw+x]=0xff000000u|(uint32_t(r)<<16)|(uint32_t(g)<<8)|b;++opaque;
            }
        }
        unlock(surf);
        if(!ok) {++failures;lastBits=bpp;return;}
        std::lock_guard<std::mutex> g(m);
        // 0.9.31: keep the recent DISTINCT hand images (the game alternates between several), newest last
        {bool known=false;for(auto& e:history)if(e.w==cw&&e.h==ch&&e.hotX==-ox&&e.hotY==-oy&&e.pixels==out){known=true;break;}
         if(!known&&opaque>=20){if(history.size()>=8)history.erase(history.begin());history.push_back(HistoryImage{out,cw,ch,-ox,-oy});++historySerial;}}
        pixels.swap(out);w=cw;h=ch;hotX=-ox;hotY=-oy;++serial;++captures;lastKey=key;lastBits=bpp;lastOpaque=opaque;
    }
    struct HistoryImage {std::vector<uint32_t> pixels;int w=0,h=0,hotX=0,hotY=0;};
    std::vector<HistoryImage> history,pinned;uint32_t historySerial=0;   // 0.9.47.26: 'pinned' = hand images seen while the menu was shown (the in-game cursors, 726 per level, pushed them out of 'history': after a game was loaded the menu hand had no image to match, halo around it - report 090200)
};
// Saves only the device states that the stereo redraw modifies. Replaces a full
// D3DSBT_ALL capture+apply per game draw (the main CPU cost measured in 0.1.0).
struct DrawStates {
    static constexpr D3DRENDERSTATETYPE names[]={D3DRS_LIGHTING,D3DRS_CULLMODE,D3DRS_SCISSORTESTENABLE,D3DRS_CLIPPLANEENABLE,
        D3DRS_CLIPPING,D3DRS_FOGENABLE,D3DRS_ZENABLE,D3DRS_ZFUNC,D3DRS_DEPTHBIAS,D3DRS_SLOPESCALEDEPTHBIAS};
    DWORD values[sizeof(names)/sizeof(names[0])]{};
    DWORD fvf=0; ComPtr<IDirect3DVertexShader9> shader;
    D3DMATRIX world{},view{},projection{};
    float planes[4][4]{};
    bool capture(IDirect3DDevice9* d) {
        shader.Reset();
        if(FAILED(d->GetFVF(&fvf))||FAILED(d->GetVertexShader(&shader))) return false;
        if(FAILED(d->GetTransform(D3DTS_WORLD,&world))||FAILED(d->GetTransform(D3DTS_VIEW,&view))||FAILED(d->GetTransform(D3DTS_PROJECTION,&projection))) return false;
        for(size_t i=0;i<sizeof(names)/sizeof(names[0]);++i) if(FAILED(d->GetRenderState(names[i],&values[i]))) return false;
        for(DWORD i=0;i<4;++i) d->GetClipPlane(i,planes[i]);
        return true;
    }
    void apply(IDirect3DDevice9* d) {
        d->SetVertexShader(shader.Get());d->SetFVF(fvf);
        d->SetTransform(D3DTS_WORLD,&world);d->SetTransform(D3DTS_VIEW,&view);d->SetTransform(D3DTS_PROJECTION,&projection);
        for(DWORD i=0;i<4;++i) d->SetClipPlane(i,planes[i]);
        for(size_t i=0;i<sizeof(names)/sizeof(names[0]);++i) d->SetRenderState(names[i],values[i]);
        shader.Reset();
    }
};
// Batched stereo: each native world draw is recorded (world-space vertices + the device
// states the game may change between draws) and replayed once per eye at Present.
#include "ReplaySnapshot.hpp"
struct DrawRecord {
    D3DPRIMITIVETYPE type;UINT primitives,count,stride;DWORD fvf;
    size_t vertices,indices;D3DFORMAT indexFormat;bool indexed;uint32_t snapshot;
    bool hand=false; // 0.6.3: part of the Keeper's hand (3D cursor) -> drawn on the controller
    bool held3d=false;
    size_t screenVertices=0; // 0.8.90.6: menu only: offset of the ORIGINAL pre-transformed vertices (screenArena)
    uint8_t view=0; // 0.8.81: 0 = the normal (front) view, 1..3 = side / rear views drawn in the same game frame (Possession360)
    bool top=false; // 0.9.47.6: damage digits are replayed after every other transparent draw (the badge discs drawn after them hid them)
    bool overlay=false; // 0.8.99: attack effect of the possessed creature (2D additive piece of the game): drawn over everything in the headset, kept out of the motion fit
    std::vector<ReplaySprite> sprites; // 0.9.24: icon axes recomputed at headset cadence
    std::vector<digitnum::DigitVertex> digitVerts; // 0.9.47.31: damage-digit vertices turned to the CURRENT headset position at every VR frame
    MenuDrawState menuState{}; // 0.9.10: native depth/fog, menu only
    float depthBias=0; // 0.9.8: menu only: the game's own depth bias of this draw (D3D7 ZBIAS through the wrapper), replayed
};
struct Images {
    ComPtr<IDirect3DTexture9> color[3];
    ComPtr<IDirect3DSurface9> target[3],depth[2];
    HANDLE handles[3]{};
    uint32_t w=0,h=0,dw=0,dh=0,generation=0;
};
struct OverlayVertex {Vec3 position;DWORD color;};
UINT indexCountFor(D3DPRIMITIVETYPE type,UINT primitives) {
    switch(type) {
    case D3DPT_TRIANGLELIST: return primitives*3;
    case D3DPT_TRIANGLESTRIP:case D3DPT_TRIANGLEFAN: return primitives+2;
    case D3DPT_LINELIST: return primitives*2;
    case D3DPT_LINESTRIP: return primitives+1;
    default: return primitives;
    }
}
#include "Decoupled.inl"
// ---------------------------------------------------------------------------------------------
// 0.8.84 SURROUND = CUBE MAP (Possession360). Verified in the 1.70 exe (disassembly + emulation of the real code, tools/re/cube_engine_check.py): the main loop
// (0x526590) renders a frame as
//   renderer->[0x38] (0x59D760: frame begin, 0x575700)  ->  renderer->[0x2C] (0x598880: the WORLD render of the view: camera, view setup 0x575A00, ...)
//   ->  overlay (2D interface)  ->  renderer->[0x3C] (0x59D7F0: frame end -> 0x575780 = 0x589C90 viewport, 0x58F640 sort / merge of the queued items, 0x5898F0 the Direct3D draws,
//   0x578E80 frees the items).
// The world render is run here 6 more times in the SAME game frame (the vtable slot 0x6705A0 is redirected), once per cube face (LensHack::faceNext): the world is rendered with
// that face's camera, and what it queued is drawn at once (0x589C90, 0x58F640, 0x5898F0) while every draw is tagged with its face (drawView 1..6 -> DrawRecord::view) and hidden
// in the game's own image. The normal view runs LAST, unchanged, and the game's per-frame counters / camera globals are put back before it: the game (logic, picking, interface)
// only ever sees the normal view. The 6 faces have the same game time, frame rate and quality, and together they cover the whole sphere (ceiling and floor included).
struct SurroundHack {
    using WFn=int(__fastcall*)(void*,void*,void*);
    inline static WFn original=nullptr;inline static uint32_t* slotPtr=nullptr;
    inline static bool installed=false,disabled=false;inline static const char* disabledReason="";
    inline static volatile int drawView=0;                       // read by Renderer::draw (same thread): 0 = the normal view, 1..6 = cube face
    inline static bool frameOk[6]{};inline static bool frameCube=false;   // result of the last run (read by present, same thread)
    inline static unsigned frameWant=63;inline static uint64_t sFacesRun=0,sFaceRun[6]{};   // 0.8.99.1: faces asked for this frame (bit k = face k+1) and faces run
    inline static uint64_t sFrames=0,sSkipped=0,sRefused=0,sQueued[7]{};inline static double msFaces=0,msFlush=0,msMain=0;
    inline static uint32_t badStreak=0,memPeak=0;inline static uint64_t sNodesKept=0;inline static uint64_t sPoolHeld=0;inline static int sPoolHeldLogs=0;inline static volatile LONG sCubeOff=0;inline static int sCubeTrips=0;   // 0.8.94.6 / 0.8.94.8
    static void install(uintptr_t base);
    static int __fastcall hook(void* self,void* edx,void* arg);
};
struct Renderer {
    Cadence tableCadence,possessionCadence;
    uint64_t mergedDraws=0,avoidedReplayStates=0;
    PossessionFrameGuard possessionCoverageGuard;float possessionCoverage=0;
    TextureQuality textureQuality;
    Ipc ipc; NativeCamera native; NativeMouse mouse{native};KeeperCursor keeper{native}; NativeZoom zoom{native}; NativeControl control{native}; WheelZoom wheel{native};PossessionInput possession;KeyButtons keyButtons;UiWatch uiWatch;KeyboardInput keyboard;
    HWND gameWindow=nullptr;float possessionFovFactor=.70f;
    bool batched=true;std::vector<unsigned char> arena,eyeArena,indexArena,screenArena;digitnum::Frame digitFrame;std::vector<DrawRecord> records;std::vector<Snapshot> snapshots;
    double recordMilliseconds=0,replayMilliseconds=0,statsReplay=0;
    // F12: VR pause (the mod stops recording/replaying) to measure the game alone in the same scene.
    bool bypass=false,f12Down=false;double presentMilliseconds=0,statsPresent=0;
    bool measureTaint=false,bypassSettle=false;   // 0.9.47.11: the first 5 s window after every F12 press is never counted (see the F12 block in present)
    uint32_t lastWorldTime=0,lastWorldCount=0,lastEngineTime=0,lastEngineCount=0,lastEngineHighest=0;bool haveCounters=false;
    bool loggedCull=false;Profiler profiler;
    // Measured dungeon levels (native units along the floor normal, relative to the native camera):
    // low = 2nd percentile (deepest floors, pits, water) -> table base; high = 97th percentile
    // (wall tops) -> clip rectangle, so that it stays inside the drawn area at every height.
    bool measuredLevels=true,haveLevels=false;std::vector<float> heights;float levelLow=0,levelHigh=0;uint32_t levelSamples=0;
    // 0.2.7: offsets relative to the camera-derived floor (smooth), low-pass filtered and bounded.
    float frameFloor=0,floorForHeights=0,lowOffset=0.f,highOffset=2.f,walkOffset=0.f;bool haveWalk=false;bool haveOffsets=false;uint32_t rejectedLevels=0;
    // Pipelined sync: frame N is published at Present N+1 (its GPU work is then normally done),
    // so the game thread no longer spins waiting for the GPU. One game frame of extra latency.
    bool pipelined=true;ComPtr<IDirect3DQuery9> fences[SlotCount];
    struct Pending {int slot=-1;Eye eyes[2]{};Pose anchor{};uint32_t world=0,ui=0,rejected=0,mode=0;float turn=0;} pending;
    float contentTurnNow=0; // 0.8.67: commanded possession turn contained in the frame being drawn
    int32_t possCamNow[3]{};float possCamZNow=0;bool possCamNowOk=false; // 0.8.73: position of the native camera of the frame being drawn
    float possYawNow=0;bool possYawNowOk=false; // 0.8.72: heading of the native camera of the frame being drawn (read from the game, goes into the package)
    void dropPending() {if(pending.slot>=0&&ipc.data) InterlockedExchange(&ipc.data->slots[pending.slot].state,0);pending.slot=-1;}
    bool publishPending(IDirect3DDevice9* d,bool block) {
        if(pending.slot<0||!ipc.data) return false;
        auto& q=fences[pending.slot];HRESULT hr=q?q->GetData(nullptr,0,0):E_FAIL;
        if(hr==S_FALSE&&block) {
            uint64_t deadline=GetTickCount64()+250;
            do {hr=q->GetData(nullptr,0,D3DGETDATA_FLUSH);if(hr==S_FALSE) Sleep(0);} while(hr==S_FALSE&&GetTickCount64()<deadline);
        }
        if(hr==S_FALSE) return false;
        if(hr!=S_OK) {log("Pipelined GPU synchronization failed %08lX",(unsigned long)hr);dropPending();return false;}
        auto& im=images[pending.slot];auto& out=ipc.data->slots[pending.slot];
        out.generation=im.generation;out.frame=frame=++sharedFrames;
        for(int i=0;i<3;++i) out.handles[i]=reinterpret_cast<uintptr_t>(im.handles[i]);
        out.width=im.w;out.height=im.h;out.desktopWidth=im.dw;out.desktopHeight=im.dh;
        memcpy(out.eyes,pending.eyes,sizeof(out.eyes));out.anchor=pending.anchor;
        out.worldDraws=pending.world;out.uiDraws=pending.ui;out.rejectedDraws=pending.rejected;out.cameraMode=pending.mode;out.possessionTurn=pending.turn;
        MemoryBarrier();InterlockedExchange(&out.state,2);SetEvent(ipc.ready);pending.slot=-1;
        (void)d;return true;
    }
    // 0.8.94.17: report 20261005_090346. The top-down tilt was applied / released at Present, i.e. AFTER the game had already drawn the first frame of the new
    // state: the first possession frame was drawn looking straight down (nativeUp 0.174 at every entry) and the first table frame after a possession with the
    // creature's 45-degree camera ('downward 0.707' then 1.000 one frame later). Those transition frames last up to 180-210 ms: the dungeon was seen tilted.
    // Here, at the start of the world render, on the very frame where the game's camera mode changes between the table (3) and a possession (1 / 2), the same
    // write the Present-time call would make one frame later is made now. Nothing else changes; Present keeps driving the camera afterwards.
    int preMode=-1;uint64_t preEntries=0,preExits=0;int preLogs=0;
    // 0.8.94.22: MIDDLE PANEL FROM WHAT IS VISIBLE. Every per-piece rule (0.8.21 .. 0.8.94.21) failed somewhere: boxes split between layers, X / check
    // left behind, the box taken by the hand window, panels stuck open. The interface layer, once nothing is taken out of it, IS the game's interface
    // without the dungeon (what the PC shows). Each frame it is reduced to 256 x 192 on the GPU and read back 2 frames later (event query, no stall):
    // visible pixels between the health bar and the bottom panel, away from the hand on the table, give the panel's rectangle. Box, '0' page, 'i'
    // page: one path, the game's own pixels, nothing to classify.
    struct PanelScan {
        ComPtr<IDirect3DSurface9> rt[3],sys[3];ComPtr<IDirect3DQuery9> q[3];bool busy[3]{};float ex[3][4]{};int idx=0;UINT w=256,h=192;
        bool ready=false,failed=false,open=false,stats=true;int statsOn=0,statsOff=0;uint64_t statsChanges=0;float statsCover=1;float rect[4]{};int onRun=0,offRun=0;uint64_t scans=0,positives=0,opens=0;int logs=0;float lastArea=0;uint64_t lastCount=0;
    } scan;
    bool panelFromPixels=true;
    bool scanOnStats() const {   // 0.8.94.26: the open panel reaches the timer bar's band (the '0' page's 'Quitter' row is drawn over it by the game)
        if(!(panelFromPixels&&scan.ready&&!scan.failed&&scan.open&&scan.stats)||!uiW||!uiH) return false;
        const float ih=std::min(float(uiH),uiW*.75f),oy=(uiH-ih)*.5f;return (scan.rect[3]-oy)/ih>.693f;}
    bool statsShownNow() const {return (!(panelFromPixels&&scan.ready&&!scan.failed)||scan.stats)&&!scanOnStats();}   // 0.8.94.23 / 0.8.94.26: the panel then shows that band itself
    void scanAnalyse(int k) {
        D3DLOCKED_RECT lr{};if(FAILED(scan.sys[k]->LockRect(&lr,nullptr,D3DLOCK_READONLY))) return;
        const float iw=std::min(float(uiW),uiH*4.f/3.f),ih=std::min(float(uiH),uiW*.75f),ox=(uiW-iw)*.5f,oy=(uiH-ih)*.5f;
        const float sx=float(uiW)/scan.w,sy=float(uiH)/scan.h;const float* e=scan.ex[k];
        float x0=1e9f,y0=1e9f,x1=-1e9f,y1=-1e9f;uint64_t n=0;
        std::vector<uint8_t> row(scan.h,0);std::vector<float> rmin(scan.h,1e9f),rmax(scan.h,-1e9f);   // content per row, and its horizontal extent
        for(UINT y=0;y<scan.h;++y) {
            const float v=(y+.5f)*sy,fv=(v-oy)/ih;if(fv<.095f||fv>.765f) continue;
            const DWORD* line=reinterpret_cast<const DWORD*>(static_cast<const unsigned char*>(lr.pBits)+y*lr.Pitch);
            for(UINT x=0;x<scan.w;++x) {
                if((line[x]>>24)<24) continue;
                {const DWORD c=line[x];if(std::max({(c>>16)&255u,(c>>8)&255u,c&255u})<24u) continue;}   // 0.8.94.24: black (the '0' page's opaque background) is not content: same rule as the offline test on the 46 captures
                const float u=(x+.5f)*sx;if(u<ox||u>ox+iw) continue;
                if(e[2]>e[0]&&u>=e[0]&&u<=e[2]&&v>=e[1]&&v<=e[3]) continue;
                row[y]=1;rmin[y]=std::min(rmin[y],u);rmax[y]=std::max(rmax[y],u);
                if(fv<=.685f) {++n;x0=std::min(x0,u-sx*.5f);x1=std::max(x1,u+sx*.5f);y0=std::min(y0,v-sy*.5f);y1=std::max(y1,v+sy*.5f);}
            }
        }
        // extend the bottom through the rows just under the core band while they hold something (new game: the box goes down to the bottom panel)
        // a row that reaches outside the box (the statistics bar of a loaded game, 0-73 % wide) stops it (tested on every capture of the reports)
        if(n) {UINT y=UINT(std::min<float>(scan.h-1,y1/sy));while(y+1<scan.h&&row[y+1]&&rmin[y+1]>=x0-8&&rmax[y+1]<=x1+8) {++y;y1=std::max(y1,(y+1.f)*sy);}
            // the '0' page starts at the left edge: its 'Quitter' row lies on the statistics bar -> bottom at least .7135 (the page band validated before)
            if(x0-ox<=.03f*iw) y1=std::max(y1,oy+.7135f*ih);}
        // 0.8.94.23: is the game's statistics bar there? Its left part (timer box, x 1-17 %, rows 72.5-75.5 %, below the page's 'Quitter' row) is never
        // covered by a box (>= 39 %): opaque there = present. Absent at the start of a new game: its slot is then closed in VR.
        {uint64_t on=0,all=0;
         for(UINT y=0;y<scan.h;++y) {const float v=(y+.5f)*sy,fv=(v-oy)/ih;if(fv<.725f||fv>.755f) continue;
             const DWORD* line=reinterpret_cast<const DWORD*>(static_cast<const unsigned char*>(lr.pBits)+y*lr.Pitch);
             for(UINT x=0;x<scan.w;++x) {const float u=(x+.5f)*sx,fu=(u-ox)/iw;if(fu<.01f||fu>.17f) continue;++all;const DWORD c=line[x];if((c>>24)>=64&&std::max({(c>>16)&255u,(c>>8)&255u,c&255u})>=24u) ++on;}}   // 0.8.94.24: coloured, not the page's black background
         scan.statsCover=all?float(on)/all:1.f;
         if(scan.statsCover>=.5f) {scan.statsOff=0;if(!scan.stats&&++scan.statsOn>=3) {scan.stats=true;++scan.statsChanges;if(scan.logs<40) {++scan.logs;log("Statistics bar (0.8.94.23): shown by the game -> its VR slot is back under the health bar");}}}
         else if(scan.statsCover<.10f) {scan.statsOn=0;if(scan.stats&&++scan.statsOff>=30) {scan.stats=false;++scan.statsChanges;if(scan.logs<40) {++scan.logs;log("Statistics bar (0.8.94.23): not shown by the game (new game) -> the health bar sits directly on the bottom panel");}}}
         else {scan.statsOn=0;scan.statsOff=0;}}
        scan.sys[k]->UnlockRect();
        // not in the 1.5 s after a possession: the statistics bar comes back with an animation through the band (report 20261005_010940 capture 2)
        const bool hit=n>=150&&x1-x0>=.12f*iw&&y1-y0>=.06f*ih&&!(lastPossessionTick&&GetTickCount64()-lastPossessionTick<1500);
        ++scan.scans;scan.lastCount=n;scan.lastArea=(x1>x0&&y1>y0)?(x1-x0)*(y1-y0)/(iw*ih):0.f;
        if(hit) {++scan.positives;scan.offRun=0;
            if(++scan.onRun>=2) {if(!scan.open) {scan.open=true;++scan.opens;
                    if(scan.logs<40) {++scan.logs;log("Middle panel (0.8.94.22): OPEN from the visible interface: %.0f,%.0f - %.0f,%.0f (%llu pixels of %ux%u)",x0,y0,x1,y1,(unsigned long long)n,scan.w,scan.h);}}
                scan.rect[0]=x0;scan.rect[1]=y0;scan.rect[2]=x1;scan.rect[3]=y1;}}
        else {scan.onRun=0;if(scan.open&&++scan.offRun>=4) {scan.open=false;if(scan.logs<40) {++scan.logs;log("Middle panel (0.8.94.22): closed (nothing visible between the bars any more)");}}}
    }
    void scanStep(IDirect3DDevice9* d) {
        if(!panelFromPixels||scan.failed||!uiSurf) return;
        if(!scan.ready) {
            for(int i=0;i<3;++i) {
                if(FAILED(d->CreateRenderTarget(scan.w,scan.h,D3DFMT_A8R8G8B8,D3DMULTISAMPLE_NONE,0,FALSE,&scan.rt[i],nullptr))||
                   FAILED(d->CreateOffscreenPlainSurface(scan.w,scan.h,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&scan.sys[i],nullptr))||
                   FAILED(d->CreateQuery(D3DQUERYTYPE_EVENT,&scan.q[i]))) {scan.failed=true;log("Middle panel (0.8.94.22): read-back surfaces unavailable, panel from the draws");return;}
            }
            scan.ready=true;log("Middle panel (0.8.94.22): panel found from the visible interface (%ux%u read-back, 2 frames late, no stall)",scan.w,scan.h);
        }
        // the oldest slot: analysed once the GPU is done with it (never waits)
        for(int j=1;j<=2;++j) {const int k=(scan.idx+j)%3;
            if(scan.busy[k]&&scan.q[k]->GetData(nullptr,0,0)==S_OK) {
                if(SUCCEEDED(d->GetRenderTargetData(scan.rt[k].Get(),scan.sys[k].Get()))) scanAnalyse(k);
                scan.busy[k]=false;}}
        const int k=scan.idx;if(scan.busy[k]) return;   // GPU still busy with it: skip this frame
        if(FAILED(d->StretchRect(uiSurf.Get(),nullptr,scan.rt[k].Get(),nullptr,D3DTEXF_LINEAR))) {scan.failed=true;log("Middle panel (0.8.94.22): reduction refused by the driver, panel from the draws");return;}
        // what sits at the hand on the table (held creature, tooltip) is not a panel
        float* e=scan.ex[k];e[0]=e[1]=1e9f;e[2]=e[3]=-1e9f;
        if(cursorKnown&&(lastPointerOnTable||(heldOffTable&&keeperW>=heldMin(100)&&keeperH>=heldMin(60)))) {const float R=heldRadius*uiW/1024.f;   // 0.9.47.3: also a creature held with the laser on the interface (report 114545: the creature was taken for the wall panel)
            e[0]=cursorPx-.75f*R;e[1]=cursorPy-.75f*R;e[2]=cursorPx+1.6f*R;e[3]=cursorPy+1.25f*R;
            if(heldKeep[2]>heldKeep[0]) {e[0]=std::min(e[0],heldKeep[0]-8);e[1]=std::min(e[1],heldKeep[1]-8);e[2]=std::max(e[2],heldKeep[2]+8);e[3]=std::max(e[3],heldKeep[3]+8);}}
        scan.q[k]->Issue(D3DISSUE_END);scan.busy[k]=true;scan.idx=(k+1)%3;
    }
    int possessionEntryFrames=0,possessionEntryWindow=12,entryLevelLogs=0;   // 0.8.94.18
    // 0.8.94.19: the REAL cause of the tilted dungeon / lag / crash at possession entry and exit (report 20261005_100309 + the 1.70 exe): the game
    // moves its camera for 2 s (transition 0x44ACE0 / 0x44BD70) between the table and the creature. During those 2 s the mod was treating the frames
    // as table frames: top-down tilt rewritten on every frame against the game's own tilt animation ('rewrites while active: 34..106'), the table
    // anchored on a camera flying down, the engine view enlarged around it (one dive ended in a 3 s stall and exit(-1)). A transition that goes to or
    // comes from a possession is now completed at once by the game itself (its own end-of-transition path, as with a duration of 0).
    bool instantPossessionCamera=true;uint64_t lastPossessionTick=0,ctFinished=0,ctOther=0;int ctLogs=0;uint32_t ctLastEnd=0;
    void possessionTransition(const char* where) {
        if(!native.supported) return;
        const uint32_t m=native.mode();const uint64_t now=GetTickCount64();
        if(m==1||m==2) lastPossessionTick=now;
        uint32_t end,start,target;if(!native.transition(end,start,target)) return;
        if(end==1) return;   // completion already requested: the game's next camera update applies it
        const bool poss=target==1||target==2||m==1||m==2||(lastPossessionTick&&now-lastPossessionTick<1500);
        if(end==ctLastEnd) return;   // already logged
        ctLastEnd=end;
        if(!poss) {++ctOther;if(ctLogs<30) {++ctLogs;log("Camera transition (0.8.94.19): mode %u -> %u over %u ms, not a possession: left to the game",m,target,end-start);}return;}
        if(!instantPossessionCamera) {if(ctLogs<30) {++ctLogs;log("Camera transition (0.8.94.19): possession mode %u -> %u over %u ms kept (PossessionCameraInstant=0)",m,target,end-start);}return;}
        if(native.finishTransition()) {++ctFinished;
            if(ctLogs<30) {++ctLogs;log("Camera transition (0.8.94.19): possession mode %u -> %u over %u ms completed at once (%s)",m,target,end-start,where);}}
    }
    void preWorld() {
        possessionTransition("before the world render");
        if(!native.supported||!topDown.enabled) return;
        const int m=int(native.mode());
        if(preMode>=0&&m!=preMode&&!native.frontend()) {
            if((m==1||m==2)&&preMode==3&&topDown.active) {topDown.hold=0;topDown.apply(native,false);++preEntries;
                if(preLogs<12) {++preLogs;log("Top-down camera (0.8.94.17): possession entry seen before the world render (mode %d -> %d): player tilt restored before the first possession frame",preMode,m);}}
            else if(m==3&&(preMode==1||preMode==2)&&!topDown.active) {topDown.apply(native,true);topDown.hold=4;++preExits;
                if(preLogs<12) {++preLogs;log("Top-down camera (0.8.94.17): return to the table seen before the world render (mode %d -> 3): top-down tilt set before the first table frame",preMode);}}
        }
        preMode=m;
    }
    bool attempted=false;
    IDirect3DDevice9* device=nullptr;
    HANDLE hostProcess=nullptr;
    Images images[SlotCount];
    ComPtr<IDirect3DStateBlock9> state;
    ComPtr<IDirect3DQuery9> fence;
    DrawStates saved;
    Request request{};
    int slot=-1;
    uint64_t frame=0,lastStatus=0;
    uint32_t world=0,ui=0,rejected=0,mode=0,generation=0,blankFrames=0,anchoredFrames=0;bool hideFloatingUi=true;
    // 0.6.1: transparent interface target (the game's 2D draws replayed without the dungeon) and cursor capture.
    ComPtr<IDirect3DTexture9> uiTex;ComPtr<IDirect3DSurface9> uiSurf,cursorBackup,uiDepth;bool uiDepthFailed=false;UINT uiW=0,uiH=0;bool uiCleared=false,uiFailed=false,hudErase=true;
    bool cursorFound=false,cursorBacked=false;RECT cursorRect{},cursorSaved{};uint32_t cursorCopy=0;float cursorUv[4]{},cursorW=0,cursorH=0,cursorHotX=0,cursorHotY=0;
    float cursorPx=-1,cursorPy=-1;bool cursorKnown=false;bool holdingThings=false,heldModel=false;float heldBox[4]{};bool heldCleared=false,keeperHeld=true;float heldRadius=200,heldPx=0,heldPy=0;ComPtr<IDirect3DTexture9> heldTex;ComPtr<IDirect3DSurface9> heldSurf;bool heldWindow=true,uiTexFilter=false;std::unordered_set<void*> uiTextures;uint64_t statUiIn3d=0;uint64_t statHeldDraws=0,statHeldFrames=0,statHeldFar=0,statHeldPanel=0,statHoldCount=0,statUiFrames=0;bool handToController=true,cursorFromUi=false;int firstUiLogs=0;UINT backW=0,backH=0;uint64_t lastUiProbe=0;DWORD probeCursorInfo[4]{};HudSizes hudSizes{};
    float tipX0=1e9f,tipY0=1e9f,tipX1=-1e9f,tipY1=-1e9f,lastTip[4]={1e9f,1e9f,-1e9f,-1e9f},lastKeep[4]={1e9f,1e9f,-1e9f,-1e9f},creatureKeep[4]={1e9f,1e9f,-1e9f,-1e9f};uint64_t statCreatureKeep=0;float handBox[4]={1e9f,1e9f,-1e9f,-1e9f};uint64_t statHandBoxPieces=0,statHandBoxFrames=0;int handBoxLogs=0;   // 0.8.94.34
    // 0.8.94.33: tooltip rectangle of the hand window. Report 20261005_204522: the 0.8.94.31 union of every small flat piece right of the cursor also took
    // pieces of notification boxes / strips (rects 6..134, 260..340, 31..340 x 177..209) -> fragments of the box moved beside the hand. The game draws its
    // hover tooltip (frame + icon rows) at a FIXED place: left edge +128..+140 px right of the cursor, frame top -32, icons top +15 (every report since
    // 150511). Now: anchor = a candidate whose top-left lies there; the rectangle grows only through candidates touching it (gap <= 16 px); a cluster
    // larger than 330 x 140 px is not a tooltip -> no rectangle (the window stays exactly as the game draws it, like 0.8.94.26).
    struct TipCand {float x0,y0,x1,y1,rx,ry,kk;};std::vector<TipCand> tipCands;uint64_t statTipFrames=0,statTipRejected=0;int tipLogs=0;
    void tipCluster() {
        tipX0=tipY0=1e9f;tipX1=tipY1=-1e9f;
        const size_t n=tipCands.size();if(n==0) return;
        std::vector<uint8_t> in(n,0);bool any=false;
        for(size_t i=0;i<n;++i) {const auto& c=tipCands[i];if(c.rx>=110.f*c.kk&&c.rx<=155.f*c.kk&&c.ry>=-45.f*c.kk&&c.ry<=30.f*c.kk) {in[i]=1;any=true;}}
        if(!any) return;
        const float gap=16.f*tipCands[0].kk;
        for(bool grew=true;grew;) {grew=false;
            for(size_t i=0;i<n;++i) if(!in[i]) {const auto& a=tipCands[i];
                for(size_t j=0;j<n;++j) if(in[j]) {const auto& b=tipCands[j];
                    if(a.x0<=b.x1+gap&&a.x1>=b.x0-gap&&a.y0<=b.y1+gap&&a.y1>=b.y0-gap) {in[i]=1;grew=true;break;}}}}
        float x0=1e9f,y0=1e9f,x1=-1e9f,y1=-1e9f;
        for(size_t i=0;i<n;++i) if(in[i]) {x0=std::min(x0,tipCands[i].x0);y0=std::min(y0,tipCands[i].y0);x1=std::max(x1,tipCands[i].x1);y1=std::max(y1,tipCands[i].y1);}
        const float kk=tipCands[0].kk;
        if(x1-x0>330.f*kk||y1-y0>140.f*kk) {++statTipRejected;if(tipLogs<6) {++tipLogs;log("Held window tooltip (0.8.94.34): cluster %.0fx%.0f px too big, not moved (%u candidates)",x1-x0,y1-y0,unsigned(n));}return;}
        tipX0=x0;tipY0=y0;tipX1=x1;tipY1=y1;++statTipFrames;
    }   // 0.8.94.3; 0.8.94.31: lastKeep = held creature box of the finished frame
    HudRegion msgRegion{0,0,0,0};float heldKeep[4]={1e9f,1e9f,-1e9f,-1e9f};bool nativeZDirty=false,probeFrame=false,zOverridden=false;DWORD gameZWrite=TRUE;int probeLines=0;bool hudFromImage=true,hideNativeWorld=true,hideNative=false,uiWentToHeld=false;bool nativeMaskOpen=true;uint64_t skipDrops0=0,skipFrames=0;bool heldModelSeen=false,foldBlocked=false,ui3dFilter=true,pickFits=false,screenWhole=true,msgFrame=false,msgShown=false;bool objTextNeedsPage=true;int msgHold=0;float msgX0=1e9f,msgY0=1e9f,msgX1=-1e9f,msgY1=-1e9f,boxX0=1e9f,boxY0=1e9f,boxX1=-1e9f,boxY1=-1e9f,boxTailY1=-1e9f,prevTail=-1e9f,prevBox[4]={1e9f,1e9f,-1e9f,-1e9f};int uiDiagLogs=0;uint64_t statCards=0,statBoxTail=0;int boxTailLogs=0,boxMissLogs=0;float worldRhw=0;uint64_t statUi3d=0;float pickMin=.30f,pickTop=.07f,pickBottom=.68f,pickWant=1.f;uint64_t pickLogs=0;
    float pickScale=.34f;ComPtr<IDirect3DTexture9> screenTex;ComPtr<IDirect3DSurface9> screenSurf;bool screenCleared=false,screenFailed=false;int pedestalSide=0;bool buttonHover=false;uint64_t statButtonClicks=0;bool screenActive=false,lastPointerOnTable=false;int screenHold=0;float midX0=1e9f,midY0=1e9f,midX1=-1e9f,midY1=-1e9f;uint64_t statScreenFrames=0;bool pickActive=false,pickFound=false,lastPickValid=false;float pickT=0,lastPickDist=0,lastPickFx=0,lastPickFy=0;uint64_t statPickHits=0,statPlaneHits=0;Vec3 pickO{},pickD{};Projection pointerLens{};bool pointerLensOk=false,tablePointer=true;uint64_t statTableHits=0,statTableClicks=0;float lastTableX=0,lastTableY=0;UINT lastHudW=0,lastHudH=0;float lastHudTop=0;bool hudLatchL=false,hudLatchR=false,hudTrigBefore=false,hudSecondBefore=false;int hudTarget=-1;
    uint64_t statUiDraws=0,statCursorFrames=0,statHudFrames=0,statHudHits=0,statHudClicks=0;float lastPointerX=-1,lastPointerY=-1;bool minimapZoomButton=false,tableEdgeFill=false;std::vector<std::array<float,4>> allPieces;int objLogs=0;float skirtTopGap=.04f;uint64_t statZoomBlocked=0;float ceilingMargin=1.f;
    bool loggedDepthState=false;
    bool tableEnabled=true,clipSupported=false,tablePrepared=false,loggedTable=false,warnedTable=false;
    bool fastStates=true,drawHands=true;
    float tableCrop=.92f,tableAspect=.75f;bool environmentOn=true;int tableView=1;uint64_t expandLogs=0;float loggedEx=0,loggedEy=0;
    // 0.5.5: the table's far edge is the screen's far edge (its extra depth is on the player side), so the engine's
    // view is never raised nor pushed further (0.4.7-0.5.4: top x2, far x4: many distant tiles computed for the far
    // corners and then cut). The view is enlarged on the sides and towards the player only, with a margin (tiles
    // are world squares: the near corners need more than the exact rectangle).
    TopDownCamera topDown;Tabletop screenFoot{};float frameNearGeom=1e30f,nearGeom=0;bool haveNearGeom=false;uint64_t nearShiftLogs=0;
    // 0.5.16: with the top-down camera every point of the table is in front of the camera, so the table can be
    // PROJECTED on the screen: the engine's rectangle is set to contain the table's corners at the floor, at the
    // wall tops and below the floor, plus a margin (drag prediction + turn + one tile). No factor search: the
    // 0.5.5-0.5.15 searches assumed a fixed top edge and ran to their limits (x8.7) while the table still lacked tiles.
    float panMarginMeters=.05f;uint64_t rectLogs=0;int tableRangeLogs=0;int32_t lastCamState[4]{};int busyHold=0;bool lastBusy=true;uint32_t marginLogs=0;uint64_t statBusy=0,statIdle=0;
    bool projectTable(const Projection& lens,ScreenRect area,const Tabletop& box) {
        const float busyMargin=1.5f+panMarginMeters*std::max(activeUnits,1.f)+.08f*std::max(box.halfWidth,box.halfDepth);
        // 0.8.57.2: the big margin only serves a map being moved (pan prediction, turn, partly drawn tiles at the edges). 70-89 % of the
        // triangles the engine computed were thrown away by the table planes (0.8.57.1 measurement). While nothing moves the margin is one tile.
        // "Moving" = camera position/zoom changed, a grip is held, the stick scrolls, plus a hold of TableIdleFrames game frames afterwards.
        static const float idleMargin=iniFloat(L"TableIdleMargin",1.0f,-1.f,10.f);static const int idleFrames=int(iniFloat(L"TableIdleFrames",30.f,0.f,600.f));
        {
            int32_t cs[4]{};if(auto* c=native.camera()) {memcpy(cs,c+0xd68,12);memcpy(&cs[3],c+0xd80,4);}
            const bool moved=memcmp(cs,lastCamState,sizeof(cs))!=0||InterlockedExchange(&CinematicView::touched,0)!=0;memcpy(lastCamState,cs,sizeof(cs));   // 0.8.94: a cinematic moves the view without moving the camera fields
            const bool grip=(request.handFlags&(HandLeftGrip|HandRightGrip))!=0,stick=request.stickX!=0||request.stickY!=0;
            if(moved||grip||stick) busyHold=idleFrames; else if(busyHold>0) --busyHold;
            const bool nowBusy=busyHold>0;
            if(nowBusy!=lastBusy) {lastBusy=nowBusy;if(marginLogs<40) {++marginLogs;log("Engine view margin: now %s (%.1f units; busy %.1f, idle %.1f)",nowBusy?"busy":"idle",nowBusy||idleMargin<0?busyMargin:std::min(busyMargin,idleMargin),busyMargin,idleMargin);}}
            (nowBusy?statBusy:statIdle)++;
        }
        const float margin=(busyHold>0||idleMargin<0)?busyMargin:std::min(busyMargin,idleMargin);
        const float hw=box.halfWidth+margin,hd=box.halfDepth+margin;
        const float heights[3]={std::min(lowOffset,0.f)-.5f,0.f,std::max(highOffset,2.5f)+.5f};
        float l=1e30f,t=1e30f,r=-1e30f,b=-1e30f;
        for(float h:heights) for(float x:{-hw,hw}) for(float z:{-hd,hd}) {
            Vec3 q=box.point(x,z,h);float depth=-q.z;
            if(!(depth>.05f)) {ViewExpand::rectValid=0;return false;}
            float sx=lens.centerX+lens.focalX*(q.x/depth),sy=lens.centerY-lens.focalY*(q.y/depth);
            if(!std::isfinite(sx)||!std::isfinite(sy)) {ViewExpand::rectValid=0;return false;}
            l=std::min(l,sx);r=std::max(r,sx);t=std::min(t,sy);b=std::max(b,sy);
        }
        // 0.6.9.15: while the whole table lies between the bars, the engine computes nothing under them: the game's
        // final image (source of the VR bars) then holds only interface there (dungeon pieces moved in the bars).
        if(pickFits&&hudFromImage) {const ScreenRect fs=fullScreen(lens);const float H=fs.bottom-fs.top;
            t=std::max(t,fs.top+std::min(pickTop,.07f)*H);b=std::min(b,fs.top+.69f*H);if(b<=t) {ViewExpand::rectValid=0;return false;}}
        ViewExpand::rectL=l;ViewExpand::rectT=t;ViewExpand::rectR=r;ViewExpand::rectB=b;ViewExpand::rectValid=1;
        // The engine's distance limit (sphere around the camera): with the camera looking down, the rays reach
        // the floor at a bounded distance, so a large limit adds no tile beyond the rectangle.
        ViewExpand::autoFar=std::min(6.f,viewFarMax());   // 0.8.94.11: the top-down path set 6.0 flat, above the ViewFarMax ceiling of 0.8.94.9
        if(rectLogs<8) {
            float aw=area.right-area.left,ah=area.bottom-area.top;
            if(rectLogs==0||std::fabs((r-l)/aw-loggedEx)>.2f||std::fabs((b-t)/ah-loggedEy)>.2f) {
                ++rectLogs;loggedEx=(r-l)/aw;loggedEy=(b-t)/ah;
                log("TableView (top-down): table %.1f x %.1f units + margin %.1f; engine rectangle %.0f,%.0f - %.0f,%.0f (x%.2f wide, x%.2f high of the screen)",
                    box.halfWidth*2,box.halfDepth*2,margin,l,t,r,b,loggedEx,loggedEy);
            }
        }
        return true;
    }
    static float viewFarMax() {static const float v=std::clamp(iniFloat(L"ViewFarMax",2.f,1.f,6.f),1.f,6.f);return v;}
    void updateExpansion(const Projection& lens,ScreenRect area,const Tabletop& box) {
        // 0.8.94.11: coming back from a possession the table box was computed as 83.2 x 63.6 units instead of about 6 x 5 (report 20261005_001910,
        // logged one frame before the game ran out of render memory and called exit(-1) at 0x57CFC4). The camera fields are still being rebuilt at
        // that moment. A box that big is refused: this frame gets no enlargement at all, and the engine keeps its own view.
        static const float tableMax=std::clamp(iniFloat(L"TableMaxUnits",30.f,8.f,200.f),8.f,200.f);
        const float tw=box.halfWidth*2,td=box.halfDepth*2;
        if(!(std::isfinite(tw)&&std::isfinite(td)&&tw>0&&td>0&&tw<=tableMax&&td<=tableMax)) {
            ViewExpand::rectValid=0;ViewExpand::autoX=1;ViewExpand::autoBottom=1;ViewExpand::autoFar=1;
            if(tableRangeLogs<8) {++tableRangeLogs;log("TableView: table %.1f x %.1f units out of range (limit %.0f): the engine view is NOT enlarged for this frame",tw,td,tableMax);}
            return;
        }
        if(topDown.active&&projectTable(lens,area,box)) return;
        ViewExpand::rectValid=0;
        float cx=(area.left+area.right)*.5f,cy=(area.top+area.bottom)*.5f;
        auto grown=[&](float ex,float eb){return ScreenRect{cx+(area.left-cx)*ex,area.top,cx+(area.right-cx)*ex,cy+(area.bottom-cy)*eb};};
        auto inside=[&](float ex,float eb,bool& x,bool& z){auto t=native.tabletopArea(lens,1.f,1.f,grown(ex,eb),false);covers(t,box,x,z);return t.valid;};
        float eb=1,ex=1;bool x=false,z=false;
        for(float e=1;e<=6.001f;e+=.05f) {if(!inside(1.6f,e,x,z)) break;eb=e;if(z) break;}
        for(float e=1;e<=5.001f;e+=.05f) {if(!inside(e,eb,x,z)) break;ex=e;if(x) break;}
        // 0.5.9: +45 % towards the player, +30 % on the sides: while the map is dragged, the headset shows it
        // ahead of the game (pan prediction), the player side needs spare tiles (the far side has them already).
        ex*=topDown.active?1.45f:1.30f;eb*=1.45f; // 0.5.14: +45 % sides with the top-down camera (sides filled late while dragging)
        // 0.5.6: the engine drops tiles beyond a distance (0x760B04) measured from the camera: the far CORNERS of the
        // table are further than the middle of the far edge (0.5.5: far corners empty). Far distance scaled by the
        // ratio corner / middle (3D distance from the camera), +10 %.
        {Vec3 mid=screenFoot.point(0,screenFoot.halfDepth),corner=box.point(box.halfWidth,box.halfDepth); // engine far = screen far edge
         float dm=std::sqrt(dot(mid,mid)),dc=std::max(std::sqrt(dot(corner,corner)),std::sqrt(dot(box.point(-box.halfWidth,box.halfDepth),box.point(-box.halfWidth,box.halfDepth))));
         // 0.8.94.9: the far distance is THE cost. Report 20261004_234537: in the table view alone (no possession), one single frame with the view
         // rectangle replaced by the table and the far distance x3 filled the whole render pool (STALL 3 s, then exit(-1) at 0x57CFC4). 0.8.25 had
         // already measured 'far x4 -> 7 to 115 blocks'. The ceiling is now ViewFarMax (2.0 by default) instead of 4.
         const float farMax=viewFarMax();
         ViewExpand::autoFar=dm>1e-3f?std::clamp(dc/dm*1.10f,1.f,farMax):1.f;
         // 0.5.15: with the top-down camera the engine's distance limit is a sphere around a HIGH camera: the table's
         // corners were cut (0.5.14: only a cross of tiles when zoomed out). The tiles stay limited by the view
         // rectangle, so a larger distance costs little.
         if(topDown.active) ViewExpand::autoFar=std::max(float(ViewExpand::autoFar),std::min(farMax,2.f));}
        ViewExpand::autoX=std::min(ex,5.f);ViewExpand::autoBottom=std::min(eb,6.f);
        if(expandLogs<8&&(std::fabs(ex-loggedEx)>.15f||std::fabs(eb-loggedEy)>.15f)) {
            ++expandLogs;loggedEx=ex;loggedEy=eb;
            log("TableView: table %.1f x %.1f, engine view x%.2f on the sides, x%.2f towards the player, far distance x%.2f (top unchanged)",box.halfWidth*2,box.halfDepth*2,ex,eb,float(ViewExpand::autoFar));
        }
    }
    Tabletop table{};Pose tableCamera{},possessionAnchor{};Quat possessionLevel{};float activeUnits=10,possessionPitch=0;bool anchored=false,possessedFrame=false,possessionAnchorValid=false,possessionLevelFinal=false,possessionFrameReady=false;uint32_t possessionRecenter=0;
    // Last valid table anchor, reused while the native camera transiently gives no floor.
    bool haveAnchor=false;Tabletop anchorUnits{};bool loggedAnchor=false;uint64_t anchorFallbacks=0;
    bool probeAuto=iniInt(L"ProbeAuto",0)!=0;int probeWindows=0,probeStep=0;
    double drawMilliseconds=0,syncMilliseconds=0,statsDraw=0,statsSync=0,statsStarted=0;
    uint64_t statsFrames=0,statsVertices=0,statsWorld=0,sampledFrames=0;
    std::vector<unsigned char> converted;
    std::vector<Vec3> positions;
    // 0.8.0: triangle split of the 3D draws by engine origin (interface / dungeon).
    std::vector<uint8_t> originFlags;std::vector<uint32_t> originUi,originWorld,originRemap;std::vector<unsigned char> originUiVerts,originUiIdx,originWorldIdx;
    int smallLogs=0,smallLogsFrame=0;bool f11Down=false,uiDumpPending=false;int uiDumpCount=0;float rhwWorldEma=0;int objHold=0,pageHold=0,boxLogs=0;int panelFrames=0,panelMaxFrames=240,panelCuts=0,panelCool=0,pageWideHold=0,pageRun=0,pageLogs=0;bool pageWasOpen=false;bool pageShown=false;float pgX0=1e9f,pgY0=1e9f,pgX1=-1e9f,pgY1=-1e9f;bool heldPrev=false,handAll=true;uint64_t statTipPieces=0;bool pageSeen=false;uint64_t statPageFrames=0,statObjFrames=0,statMsgFrames=0,statHandGateOff=0;std::vector<float> sprHv;std::vector<uint32_t> bbParent,bbOrder;std::vector<uint8_t> bbUsed;bool billboards=false,creatureIcons=false,uprightEffects=true,spriteAlphaOnly=true;int spriteCensus=0,spriteCensusLogs=0;uint64_t statSpriteOpaqueSkipped=0,statUprightNoDepth=0;bool uprightEffectNoDepth=false,menuTessMemo=false;float effectMinHeight=.2f;float billboardMinHeight=.06f;uint64_t statBillboards=0,statBillboardSeen=0;int billboardLogs=0;float allX0=1e9f,allY0=1e9f,allX1=-1e9f,allY1=-1e9f;int nearDungeonDraws=0,nearDungeonHold=0;uint64_t statPlaneOff=0,statUnderDungeon=0;float dungeonBottomNow=0,dungeonBottom=1.f;int panelProbeLines=0;bool pgTopLeft=false,panelWhole=false,panelUi=false;uint64_t statPanelWholeFrames=0,statWholePieces=0,statGameBoxPieces=0,statGameBoxFrames=0;int pgPieces=0;uint64_t statHeldPieces=0,statButtonRect=0,statOriginUi=0,statOriginMixed=0,statOriginUiTris=0,statOriginWorldTris=0,statOriginWorld=0,statOriginUntagged=0;
    // 0.3.0 decoupled rendering (DecoupledRendering=1). 0 = the 0.2.9 path, which also stays in use for
    // menus, possession and the flat-screen mode.
    bool decoupledEnabled=true,decoupledFrame=false,frameOpen=false,lastAnchored=false;int anchorLost=0;double lastPresentMs=0;uint32_t hitchLogs=0;
    // 0.8.68: the first-person world can also be drawn by the VR thread (PossessionDecoupled=1). decoupledKind: 0 none, 1 table, 2 possession.
    bool possessionDecoupledOn=true,lastPossessionReady=false,menuRoomOn=true,menuCinematics=true,scriptedOnWall=false;int zeroWorldRun=0,menuLogged=-1;int decoupledKind=0,activeKind=0,possessionBadFrames=0;double frameStartMs=0;uint64_t statPossPackages=0,statPossDropped=0,statPossEntered=0;
    // 0.8.84 360 VIEW (Possession360=1): CUBE MAP. The 6 faces are drawn in the SAME game frame as the normal view (SurroundHack); every draw record carries its face
    // (DrawRecord::view 1..6). A face's vertices are mapped EXACTLY into the space of the normal (front) camera with Q_k = S A0^T A_k S (CubeFaces.hpp) and then follow the
    // reference pipeline (anchor x reference level): the VR thread draws them with the same correction as the reference view, each limited to its own pyramid (4 clip planes).
    bool possession360=true,viewReady=true;int viewApplied=-1;
    uint64_t statViewRecs[7]{},statViewSkipped=0,statViewFrames=0,statViewMissing=0,statCubeFrames=0;
    bool faceDropped=false,facePoseOk=false;Pose facePose{};   // per game frame (reset by SurroundHack::hook): a face draw was not recorded / the pose its vertices were placed with
    bool surroundWanted() const {return possession360&&vr&&!vr->failed&&vr->active&&activeKind==2&&lastPossessionReady&&possessionDecoupledOn&&!bypass&&request.valid;}
    // 0.8.75: live level. The game tilts its first-person camera while walking / turning (native up vector: x varied -0.011..+0.008 in the 0.8.74 report = about
    // +-2 degrees of roll once scaled to the floor normal) but the level was frozen at the possession entry: the whole VR world rocked with the creature.
    // With PossessionLiveLevel=1 the level is recomputed from the live up vector for every game frame (the floor stays level).
    bool possessionLiveLevel=true,tiltRefValid=false;Vec3 tiltRef{};uint64_t tiltN=0,tiltLiveUsed=0;double tiltRollSq=0,tiltPitchSq=0,tiltRollMax=0,tiltPitchMax=0,tiltRollMin=0,tiltRollMean=0;float possUpNow[3]{};bool possUpOk=false;
    VrThread* vr=nullptr;CopyCache copies;ShaderCache shaders;std::vector<std::array<uint32_t,4>> snapIds;
    uint64_t packageId=0,handed=0,dropped=0,statPackages=0;std::shared_ptr<Package> pendingPackage;ComPtr<IDirect3DQuery9> packageFences[2];
    // 0.8.64 diagnostic: where the time of the package hand-over goes (ms summed over the frames of a 5 s window).
    double presParts[5]={0,0,0,0,0};double pollBlockedMs=0;uint64_t pollBlockedCount=0,pollCalls=0;
    double decoupledSince=0,retryAt=0;int watchdogTrips=0;
    bool ensureVr(IDirect3DDevice9* d) {
        if(!decoupledEnabled) return false;
        if(vr) return !vr->failed;
        vr=new VrThread();
        TextureWatch::installDevice(d);
        const bool created=vr->create(d,&ipc);
        // 0.8.62: device-level hooks AFTER the VR thread's device exists (its creation refills the shared vtable and silenced them in 0.8.61).
        TextureWatch::installDeviceSlots(d);WrapperUpload::install();ReadbackLag::install();
        if(!created) return false;
        copies.mips=vr->mips;
        return true;
    }
    // Hands the previous game frame to the VR thread as soon as its GPU work is complete.
    void pollPackage(bool block) {
        if(!pendingPackage) return;
        auto& q=packageFences[pendingPackage->id&1];
        HRESULT hr=q?q->GetData(nullptr,0,0):E_FAIL;
        if(block) ++pollCalls;
        if(hr==S_FALSE&&block) {
            const double blockedFrom=milliseconds();
            uint64_t deadline=GetTickCount64()+250;
            do {hr=q->GetData(nullptr,0,D3DGETDATA_FLUSH);if(hr==S_FALSE) Sleep(0);} while(hr==S_FALSE&&GetTickCount64()<deadline);
            pollBlockedMs+=milliseconds()-blockedFrom;++pollBlockedCount;
        }
        if(hr==S_FALSE) return;
        if(hr==S_OK&&vr&&!vr->failed) {vr->handOff(std::move(pendingPackage));++handed;}
        else {++dropped;if(hr!=S_OK) log("Package GPU synchronization failed %08lX",(unsigned long)hr);}
        pendingPackage.reset();
    }
    Renderer() {
        log("0.8.37 safety: 360 tile/visibility/projection hooks NOT installed; 0.8.36 no-empty-eye guard retained (INI cannot reactivate 360)");
        possessionFovFactor=iniFloat(L"PossessionFovFactor",.70f,.55f,1.f);
        possession360=iniInt(L"Possession360",0)!=0;cubeAdaptive=iniInt(L"Possession360Adaptive",1)!=0;cubeMarginDeg=iniFloat(L"Possession360MarginDeg",15.f,0.f,90.f);cubeHoldMs=iniFloat(L"Possession360HoldMs",300.f,0.f,5000.f);menuRoomOn=iniInt(L"MenuRoom",1)!=0;menuDiag=iniInt(L"MenuDiagnostic",0)!=0;menuRelief=iniInt(L"MenuRelief",1)!=0;menuPillarLight=iniInt(L"MenuPillarLight",1)!=0;floorDecalNoDepth=iniInt(L"FloorDecalNoDepth",1)!=0;softNoDepth=iniInt(L"SmokeNoDepth",0)!=0;spriteMaxScreen=iniFloat(L"SpriteMaxScreen",.12f,.05f,.6f);hazeSkip=iniInt(L"HazeSkip",0)!=0;menuHandSolid=iniInt(L"MenuHandSolid",1)!=0;effectPull=iniFloat(L"EffectPullToEye",.5f,0.f,2.f);menuPillarMin=iniFloat(L"MenuPillarLightMin",176.f,0.f,255.f);menuDepthK=iniFloat(L"MenuReliefDepth",.45f,.05f,1.f);menuFarK=iniFloat(L"MenuReliefFar",2.f,.2f,6.f);menuNearK=iniFloat(L"MenuReliefNear",.15f,0.f,.8f);menuEdge=iniFloat(L"MenuReliefEdge",.28f,0.f,.45f);menuEdgeMin=iniFloat(L"MenuReliefEdgeMin",.35f,.05f,1.f);menuTessPx=iniFloat(L"MenuReliefTessellation",16.f,0.f,200.f);menuTessTol=iniFloat(L"MenuReliefTessellationError",1.2f,.05f,20.f)*.001f/1.36f;menuOrderK=iniFloat(L"MenuReliefOrder",.06f,0.f,.5f);if(menuNearK<.01f) menuNearK=.01f;menuUiCpu=iniInt(L"MenuUiCpu",1)!=0;menuSplit.tolerance=iniInt(L"MenuUiTolerance",3);menuSplit.alphaGain=iniFloat(L"MenuUiAlphaGain",1.5f,1.f,4.f);menuSplit.learn=iniInt(L"MenuUiLearn",0)!=0;menuCinematics=iniInt(L"MenuRoomCinematics",1)!=0;possessionEffects=iniInt(L"PossessionEffects",1)!=0;possessionEffectsDistance=iniFloat(L"PossessionEffectsDistance",1.f,.3f,5.f);scriptedOnWall=iniInt(L"ScriptedCameraOnWall",0)!=0;
        log("Possession 360 view (0.8.84): %s (Possession360): cube map of 6 faces of 90 degrees drawn by the game in the same frame, placed exactly (no heading readout)",possession360?"ON":"off");
        tableEnabled=iniInt(L"TableBoundary",1)!=0;
        measuredLevels=iniInt(L"MeasuredLevels",1)!=0;billboards=iniInt(L"FaceSprites",0)!=0;billboardMinHeight=iniFloat(L"FaceIconsMinHeight",.06f,.02f,3.f);creatureIcons=!billboards&&iniInt(L"CreatureIconsFacePlayer",1)!=0;if(creatureIcons) {billboards=true;billboardMinHeight=iniFloat(L"CreatureIconsMinHeight",.1f,.002f,1.f);}iconTopRule=iniInt(L"CreatureIconsTopRule",1)!=0;iconScreenAxes=iniInt(L"CreatureIconsScreenAxes",1)!=0;overlayReproject=iniInt(L"OverlayOnSurface",0)!=0;forgeFanSkip=iniInt(L"ForgeSmokeFanHide",1)!=0;effectScreenAxes=iniInt(L"EffectsScreenAxes",0)!=0;decalSnap=iniInt(L"DecalOnSurface",0)!=0;decalSnapMax=1.f+iniFloat(L"DecalOnSurfaceMax",.15f,.01f,.5f);effectUpsideFix=iniInt(L"EffectsUpsideDownFix",0)!=0;   /* 0.9.38: OFF - report 013829: 392-vertex single-depth layers covering the whole screen (darkness / fog) were laid on the dungeon: dark patches everywhere */effectAnchorCenter=iniInt(L"EffectAnchorCenter",1)!=0;translucentNoDepth=iniInt(L"TranslucentNoDepth",1)!=0;spriteNoDepth=iniInt(L"SpriteNoDepth",1)!=0;damageFacing=iniInt(L"DamageNumbersFacing",0)!=0;damageDigits=iniInt(L"DamageDigitsUpright",1)!=0;digitMirror=iniInt(L"DamageDigitsMirror",1)!=0;digitScreen=iniInt(L"DamageDigitsScreenCopy",1)!=0;digitFrame.screenCopy=digitScreen;digitFrame.reach=iniFloat(L"DamageDigitsReach",4.5f,0.f,12.f);digitFrame.worldReach=iniFloat(L"DamageDigitsWorldReach",6.f,0.f,30.f);
        // 0.9.47.28: reading frame (glyphs rebuilt from their own texture coordinates, one row in text order, square cells) + size under the player's control
        digitFrame.reading=iniInt(L"DamageDigitsReading",1)!=0;digitFrame.useTracks=iniInt(L"DamageDigitsBirthOrder",1)!=0;
        {float sc=iniFloat(L"DamageDigitsScale",1.f,.2f,6.f);wchar_t ev[24]{};const DWORD en=GetEnvironmentVariableW(L"DK2VR_DIGITSCALE",ev,24);if(en>0&&en<24){const float v=float(_wtof(ev));if(v>=.2f&&v<=6.f) sc=v;}digitFrame.sizeScale=sc;}
        digitFrame.fixedWorld=iniFloat(L"DamageDigitsFixedMM",0.f,0.f,200.f)*0.001f;
        log("Damage digits (0.9.47.28): reading frame %s, scale x%.2f, fixed cell %.1f mm (0 = the game's size)",digitFrame.reading?"ON":"off (screen copy)",digitFrame.sizeScale,digitFrame.fixedWorld*1000.f);badgeDiscFix=iniInt(L"BadgeDiscUpright",1)!=0;digitsLast=iniInt(L"DigitsDrawnLast",1)!=0;menuHandHalo=iniInt(L"MenuHandHaloClean",1)!=0;heldOffTable=iniInt(L"HeldOffTable",1)!=0;uprightEffects=iniInt(L"UprightEffects",1)!=0;spriteAlphaOnly=iniInt(L"SpriteAlphaOnly",1)!=0;uprightEffectNoDepth=iniInt(L"UprightEffectsNoDepth",1)!=0;menuTessMemo=iniInt(L"MenuTessMemo",0)!=0;/* 0.9.29: no gain measured (10-11 ms vs 8-9) -> off */effectMinHeight=iniFloat(L"UprightEffectsMinHeight",.2f,-1.f,3.f);heights.reserve(40000);
        tableCrop=iniFloat(L"TableCrop",.92f,.5f,1.f);
        tableAspect=iniFloat(L"TableAspect",.75f,0.f,3.f);
        tableView=iniInt(L"TableView",1);
        environmentOn=iniInt(L"Environment",1)!=0;
        // 0.4.3: the room is prepared from the game start (menus), cached in the mod's Cache folder.
        if(environmentOn&&iniInt(L"DecoupledRendering",1)!=0) {
            std::error_code ec;auto dir=configurationPath().parent_path()/L"Cache";std::filesystem::create_directories(dir,ec);
            {   // 0.8.87: user textures of the room (Textures\mur_color / mur_height, sol_*, poutre_*), next to DK2VR.ini
                auto& o=env::globalScene().options;
                o.dir=(configurationPath().parent_path()/L"Textures").wstring();o.enabled=iniInt(L"RoomTextures",1);o.parallax=iniInt(L"RoomParallax",1);
                o.maxSize=int(iniFloat(L"RoomTextureMax",2048.f,256.f,4096.f));
                o.tile[0]=iniFloat(L"RoomWallTile",1.9f,.2f,20.f);o.tile[1]=iniFloat(L"RoomFloorTile",1.6f,.2f,20.f);o.tile[2]=iniFloat(L"RoomBeamTile",1.9f,.2f,20.f);
                o.depth[0]=iniFloat(L"RoomWallDepth",.05f,0.f,.3f);o.depth[1]=iniFloat(L"RoomFloorDepth",.03f,0.f,.3f);o.depth[2]=iniFloat(L"RoomBeamDepth",.025f,0.f,.3f);
            }
            env::globalScene().startBackground((dir/L"DK2VR_room.cache").wstring());
        }
        fastStates=iniInt(L"FastStates",1)!=0;
        drawHands=iniInt(L"ShowControllers",1)!=0;
        batched=iniInt(L"BatchedRendering",1)!=0;
        ViewExpand::install(native);
        topDown.init(native);instantPossessionCamera=iniInt(L"PossessionCameraInstant",1)!=0;panelFromPixels=iniInt(L"MiddlePanelFromPixels",1)!=0;log("Middle panel (0.8.94.22): %s (MiddlePanelFromPixels)",panelFromPixels?"found from the visible interface between the bars":"found from the draws (old rules)");log("Possession camera transitions (0.8.94.19): %s (PossessionCameraInstant)",instantPossessionCamera?"instant (the game's 2 s camera flight to / from the creature is completed at once)":"the game's own 2 s flight");
        if(native.supported) CinematicView::install(native.base);
        if(native.supported) SafeWindowCall::install(native.base);
        PauseWatch::install(native.base);TextBoxWatch::install(native.base);   // 0.8.94.21
        if(native.supported) PossessionSteady::install(native.base);   // 0.8.97
        PoolGrow::install(native.base);
        PoolGrow::arm(native.base);   // 0.8.94.6: live watch of the pool (the engine calls exit(-1) when it is full)
        MinimapFrame::install(native.base);
        EngineOrigin::install(native.base); // 0.8.0: interface / dungeon told apart by the engine (origin of each piece)
        HeldGeometry::install(native.base);heldDepthFlip=iniInt(L"HeldDepthFlip",0)!=0;
        LensHack::install(native.base);SurroundHack::install(native.base);pickScale=iniFloat(L"TablePickScale",.20f,.12f,1.f); // 0.7.4: floor .30 -> .20 (at .30 the table overlapped the bars: pickFits never true)
        // 0.6.9.3: the focal factor is now the LARGEST one that still puts the whole table between the bars
        // (TablePickScale = lower limit): x0.30 fixed made the game choose low-resolution textures.
        pickMin=pickScale;pickTop=iniFloat(L"TablePickTop",.07f,0.f,.4f);pickBottom=iniFloat(L"TablePickBottom",.68f,.4f,1.f);
        panelMaxFrames=std::clamp(iniInt(L"MiddlePanelMaxFrames",240),30,3000);   // 0.8.94.7 failsafe
        objTextNeedsPage=iniInt(L"ObjectivesTextRule",0)==0;   // 0.8.94.6: 0 (default) = only text lines (wider than tall) feed the objectives union; 1 = the old behaviour
        ui3dFilter=iniInt(L"Ui3DFilter",1)!=0;hudFromImage=iniInt(L"HudFromGameImage",1)!=0;hideNativeWorld=hudFromImage&&iniInt(L"HideNativeWorld",1)!=0;
        TimedCalls::skipWanted=hideNativeWorld&&iniInt(L"NativeWorldSkip",1)!=0;   // 0.9.47.10
        LockProbe::install(native);
        PaintProbe::install(native);
        hudSizes.wall=iniFloat(L"HudWallWidth",1.8f,.3f,5.f);hudSizes.bottom=iniFloat(L"HudBottomWidth",.80f,.15f,3.f);
        minimapZoomButton=iniInt(L"MinimapZoomButton",0)!=0;tableEdgeFill=iniInt(L"TableEdgeFill",0)!=0;skirtTopGap=iniFloat(L"TableEdgeFillGap",.04f,0.f,2.f);ceilingMargin=iniFloat(L"CeilingMargin",1.f,.2f,4.f);
        hudSizes.minimap=iniFloat(L"HudMinimapWidth",.30f,.05f,1.f);hudSizes.panel=iniFloat(L"HudPanelScale",1.6f,1.f,3.f);hudSizes.cursor=iniFloat(L"CursorSize",.10f,.02f,.5f);
        hudErase=iniInt(L"HudHideCursor",1)!=0;handToController=iniInt(L"HandFrom3D",0)!=0;keeperHeld=iniInt(L"HeldThingsOnHand",1)!=0;tablePointer=iniInt(L"TablePointer",1)!=0;pedestalSide=std::clamp(userInt(L"MinimapPedestalSide",0),0,2);heldWindow=iniInt(L"HeldWindow",1)!=0;handAll=iniInt(L"HandTakesAllNearby",1)!=0;uiTexFilter=iniInt(L"InterfaceCardsOutOfDungeon",0)!=0;heldRadius=iniFloat(L"HeldRadius",200.f,40.f,400.f);cursorFromUi=iniInt(L"CursorFromInterface",0)!=0;
        hideFloatingUi=iniInt(L"HideFloatingUi",1)!=0;panMarginMeters=iniFloat(L"PanMarginMeters",.10f,0.f,.5f); // 0.8.67: engine margin kept apart from the visual limit PanPredictionLimit
        BackFaces::install(native);
        TextureCache::install(native);
        FastCrc::install(native);
        FastStretch::install();
        textureQuality.install(native);
        NoVBlankWait::install();
        pipelined=iniInt(L"PipelinedSync",1)!=0;
        decoupledEnabled=iniInt(L"DecoupledRendering",1)!=0;
        possessionDecoupledOn=iniInt(L"PossessionDecoupled",1)!=0;possessionLiveLevel=iniInt(L"PossessionLiveLevel",0)!=0;log("Possession live level (0.8.75): %s (PossessionLiveLevel)",possessionLiveLevel?"ON: the horizon is levelled from the game camera of every frame":"off: level frozen at the entry as in 0.8.74");
        log("Possession drawn by the VR thread at the headset rate: %s (PossessionDecoupled)",possessionDecoupledOn?"enabled":"disabled (frame-by-frame replay of 0.8.66)");
        copies.refreshFrames=int(iniFloat(L"TextureRefreshFrames",180,0,100000));
        log("Decoupled VR rendering: %s (TextureRefreshFrames=%d)",decoupledEnabled?"enabled":"disabled (0.2.9 path)",copies.refreshFrames);
        statsStarted=milliseconds();
        log("Settings: TableBoundary=%d TableCrop=%.2f FastStates=%d BatchedRendering=%d ShowControllers=%d ZoomOutExtra=%.2f ViewExpand=%.2f ViewFarScale=%.2f",
            tableEnabled,tableCrop,fastStates,batched,drawHands,zoom.extra,ViewExpand::expand,ViewExpand::farScale);
    }
    ~Renderer() { if(hostProcess) CloseHandle(hostProcess); }
    void clearBatch() {digitGroupsNow.clear();heldRefKnown=false;for(auto& sn:snapshots) sn.release();snapshots.clear();snapIds.clear();records.clear();arena.clear();screenArena.clear();indexArena.clear();digitFrame.clear();HeldGeometry::depths.clear();}
    bool connect() {
        if(attempted) return ipc.data&&ipc.data->hostAlive&&hostProcess&&WaitForSingleObject(hostProcess,0)==WAIT_TIMEOUT;
        attempted=true;
        wchar_t token[80]{};
        if(!GetEnvironmentVariableW(L"DK2VR_SESSION",token,80)||!ipc.open(token,false)) { log("IPC connection failed (host and DLL must both be version %s)",Version); return false; }
        hostProcess=OpenProcess(SYNCHRONIZE,FALSE,ipc.data->hostPid);
        InterlockedExchange(&ipc.data->gameAlive,1);
        log("DK2VR %s x86 bridge attached to OpenXR host %lu (protocol %u)",Version,(unsigned long)ipc.data->hostPid,ProtocolVersion);
        return hostProcess!=nullptr;
    }
    void reset() {
        perfAppliedGame=-100;   // 0.9.25: a new game device starts at priority 0: apply the A/B value again
        if(slot>=0&&ipc.data) InterlockedExchange(&ipc.data->slots[slot].state,0);
        clearBatch();dropPending();possessionCoverageGuard.reset();for(auto& f:fences) f.Reset();
        if(vr) {vr->active=false;vr->dropAll();}
        pendingPackage.reset();for(auto& f:packageFences) f.Reset();copies.releaseAll();shaders.clear();frameOpen=false;decoupledFrame=false;decoupledKind=0;activeKind=0;lastPossessionReady=false;
        slot=-1; state.Reset(); fence.Reset();
        for(auto& im:images) im=Images{};
        device=nullptr; world=ui=rejected=0;
        log("D3D resources released for device reset");
    }
    bool start(IDirect3DDevice9* d) {
        if(!connect()) return false;
        if(device&&device!=d) reset();
        device=d;
        if(frameOpen) return true;
        if(!ipc.lock()) return false;
        request=ipc.data->request;
        ipc.unlock();
        if(!request.valid||request.width<64||request.height<64||request.width>4096||request.height>4096) return false;
        // 0.3.0: in the anchored table view the VR thread draws the headset images; this thread only records.
        mode=native.mode();
        const bool possessedMode=request.mode==2||(request.mode==0&&(mode==1||mode==2));
        EngineOrigin::preservePossessionDepth=possessedMode; // cached once per frame, never query memory per vertex
        // 0.8.91: camera mode 18 = the game's scripted camera (level start fly-in, story sequences; measured in journal 150000): the game moves and
        // tilts its own camera, the dungeon on the table would follow it (tilted, off centre). Shown as a cinematic on the wall screen instead.
        const bool scriptedCamera=scriptedOnWall&&mode==18&&!native.frontend();
        CinematicView::active=(CinematicView::enabled&&!scriptedOnWall&&request.tableAnchor&&request.mode!=3&&!possessedMode&&!native.frontend())?1:0;   // 0.8.94
        const bool tableKind=lastAnchored&&request.tableAnchor&&request.mode!=3&&!possessedMode&&!native.frontend()&&!scriptedCamera&&milliseconds()>=retryAt;
        // 0.8.68: first-person world at the headset rate once the previous game frame proved the possession view ready (anchor, projection, coverage).
        const bool possessionKind=!tableKind&&possessionDecoupledOn&&lastPossessionReady&&possessedMode&&request.mode!=3&&!native.frontend()&&milliseconds()>=retryAt;
        // 0.8.88: main menu, cinematics, level end screens (frontend, or a long run of frames without any world draw): the VR thread draws the empty room
        // and the host hangs the game screen on its front wall.
        const bool menuLike=menuRoomOn&&request.mode!=3&&!possessedMode&&(native.frontend()||(menuCinematics&&zeroWorldRun>=10)||scriptedCamera);
        const bool menuKind=!tableKind&&!possessionKind&&menuLike&&milliseconds()>=retryAt;
        decoupledFrame=(tableKind||possessionKind||menuKind)&&ensureVr(d);
        decoupledKind=decoupledFrame?(tableKind?1:possessionKind?2:3):0;
        if(int(menuKind&&decoupledFrame)!=menuLogged) {menuLogged=int(menuKind&&decoupledFrame);log("Menu room (0.8.88): %s (frontend=%d, frames without world draws=%d, camera mode %u)",menuLogged?"game screen on the front wall of the room":"left",int(native.frontend()),zeroWorldRun,unsigned(mode));}
        HRESULT hr=S_OK;
        auto fail=[&](const char* s,HRESULT hr) { log("%s failed: %08lX",s,(unsigned long)hr); if(slot>=0) InterlockedExchange(&ipc.data->slots[slot].state,0);slot=-1; return false; };
        if(decoupledFrame) {
            if(vr->active&&activeKind!=decoupledKind) {
                // table <-> possession: the VR thread must never draw a package of the other kind
                vr->active=false;vr->dropAll();pendingPackage.reset();for(auto& f:packageFences) f.Reset();
                log("Decoupled VR rendering: switching to the %s package kind",decoupledKind==3?"menu room":decoupledKind==2?"first-person world":"table");
                if(decoupledKind==1) resetHudStates();   // 0.8.92: a new table session starts with clean message / page states
            }
            if(!vr->active) {vr->dropMailbox();pendingPackage.reset();vr->active=true;activeKind=decoupledKind;decoupledSince=milliseconds();if(decoupledKind==2) ++statPossEntered;log("Decoupled VR rendering: ON (%s drawn by the VR thread)",decoupledKind==2?"first-person world":"headset images");}
            pollPackage(false);
            uint64_t safe=std::min(vr->safeBelow(),pendingPackage?pendingPackage->id:NoPackage);
            ++packageId;
            copies.beginFrame(packageId,std::min(safe,packageId));
        } else {
        if(vr&&vr->active) {vr->active=false;activeKind=0;pendingPackage.reset();log("Decoupled VR rendering: paused (menu, possession, flat mode or no table): lastAnchored=%d tableAnchor=%d requestMode=%u gameMode=%u possessed=%d frontend=%d retryWait=%d",int(lastAnchored),int(request.tableAnchor),unsigned(request.mode),unsigned(mode),int(possessedMode),int(native.frontend()),int(milliseconds()<retryAt));}
        for(unsigned i=0;i<SlotCount;++i) if(InterlockedCompareExchange(&ipc.data->slots[i].state,1,0)==0) { slot=int(i); break; }
        if(slot<0) return false;
        ComPtr<IDirect3DDevice9Ex> ex;
        hr=d->QueryInterface(IID_PPV_ARGS(&ex));
        if(FAILED(hr)) return fail("D3D9Ex (required for sharing)",hr);
        ComPtr<IDirect3DSurface9> back;
        if(FAILED(hr=d->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&back))) return fail("GetBackBuffer",hr);
        D3DSURFACE_DESC desc{}; back->GetDesc(&desc);
        auto& im=images[slot];
        if(im.w!=request.width||im.h!=request.height||im.dw!=desc.Width||im.dh!=desc.Height) {
            im=Images{}; im.w=request.width;im.h=request.height;im.dw=desc.Width;im.dh=desc.Height;
            for(int j=0;j<3;++j) {
                if(FAILED(hr=d->CreateTexture(j==2?im.dw:im.w,j==2?im.dh:im.h,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&im.color[j],&im.handles[j]))) {
                    im=Images{}; return fail("Create shared texture",hr);
                }
                if(FAILED(hr=im.color[j]->GetSurfaceLevel(0,&im.target[j]))) { im=Images{};return fail("GetSurfaceLevel",hr); }
            }
            for(int e=0;e<2;++e)
                if(FAILED(hr=d->CreateDepthStencilSurface(im.w,im.h,D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,TRUE,&im.depth[e],nullptr))) {
                    im=Images{}; return fail("CreateDepthStencilSurface",hr);
                }
            im.generation=++sharedGenerations;
            log("Shared stereo textures %ux%u; desktop %ux%u",im.w,im.h,im.dw,im.dh);
        }
        }
        if(!state) {
            if(FAILED(hr=d->CreateStateBlock(D3DSBT_ALL,&state))) return fail("CreateStateBlock",hr);
            D3DCAPS9 caps{};clipSupported=SUCCEEDED(d->GetDeviceCaps(&caps))&&caps.MaxUserClipPlanes>=4;
            log("Rectangular tabletop: enabled=%u clipPlanes=%lu crop=%.2f tableAnchor=%u",tableEnabled,(unsigned long)caps.MaxUserClipPlanes,tableCrop,request.tableAnchor);
        }
        if(!fence&&FAILED(hr=d->CreateQuery(D3DQUERYTYPE_EVENT,&fence))) return fail("CreateQuery",hr);
        // Watchdog (0.5.0): judged only while the table has been shown for 120 frames; 45 frames in a row with the
        // table tiles and no dungeon drawn -> table corners disabled for the session (back to empty corners).
        if(anchored&&tablePrepared) ++anchoredFrames; else anchoredFrames=0;
        if(TileCover::replacedFrame&&anchoredFrames>120) {
            if(world<3) {if(++blankFrames>45&&!TileCover::broken) {TileCover::broken=1;log("Table corners DISABLED by the watchdog: no dungeon drawn with the table tiles (world=%u)",world);}}
            else blankFrames=0;
        }
        TileCover::replacedFrame=0;
        // 0.5.12: player-side reach of the game, measured on the geometry it really drew (smoothed, slowly released).
        if(frameNearGeom<1e29f) {nearGeom=haveNearGeom?std::max(frameNearGeom,nearGeom-.02f):frameNearGeom;haveNearGeom=true;}
        frameNearGeom=1e30f;
        if(!tablePrepared||native.frontend()) LensHack::want=1.f; // no table this frame: the game camera is left alone
        world=ui=rejected=0;possessionCoverage=0;viewApplied=-1;tiltDone=false; mode=native.mode();table={};tablePrepared=false;anchored=false;possessedFrame=false;
        if(!possessionWorld(request.mode,mode)) possessionCoverageGuard.reset();
        clearBatch();
        if(heights.size()>=2000&&floorForHeights<-.05f) {
            size_t lo=heights.size()*2/100,hi=heights.size()*97/100;
            std::nth_element(heights.begin(),heights.begin()+lo,heights.end());levelLow=heights[lo];
            std::nth_element(heights.begin(),heights.begin()+hi,heights.end());levelHigh=heights[hi];
            levelSamples=uint32_t(heights.size());
            // Walking floor: histogram of heights within +-1.5 units of the camera floor (0.02 bins);
            // the highest bin holding at least 35 % of the strongest one (floor tiles are flat and dense;
            // pits, water and lava lie lower, wall tops are outside the band).
            {
                constexpr int Bins=150;uint32_t hist[Bins]{};
                for(float h:heights) {int b=int((h-floorForHeights+1.5f)/.02f);if(b>=0&&b<Bins) ++hist[b];}
                uint32_t peak=*std::max_element(hist,hist+Bins);
                if(peak>=50) {
                    int chosen=-1;
                    for(int b=Bins-1;b>=0;--b) if(hist[b]*100>=peak*35) {chosen=b;break;}
                    float walkRaw=std::clamp(-1.5f+(chosen+.5f)*.02f,-1.f,1.f);
                    if(!haveWalk) {walkOffset=walkRaw;haveWalk=true;} else walkOffset+=.05f*(walkRaw-walkOffset);
                }
            }
            float lowRaw=std::clamp(levelLow-floorForHeights,-1.f,0.f),highRaw=std::clamp(levelHigh-floorForHeights,.5f,4.f);
            if(!haveOffsets) {lowOffset=lowRaw;highOffset=highRaw;haveOffsets=true;}
            else {lowOffset+=.03f*(lowRaw-lowOffset);highOffset+=.03f*(highRaw-highOffset);}
        } else if(!heights.empty()) ++rejectedLevels;
        haveLevels=haveOffsets;
        heights.clear();
        drawMilliseconds=syncMilliseconds=0;
        frameOpen=true;frameStartMs=milliseconds(); // 0.8.68: time of the game camera this frame shows (first draw)
        if(decoupledFrame) return true;
        auto& im=images[slot];
        // Clear eyes without disturbing the game target, depth surface, or viewport.
        ComPtr<IDirect3DSurface9> rt,ds; D3DVIEWPORT9 vp{};
        d->GetRenderTarget(0,&rt);d->GetDepthStencilSurface(&ds);d->GetViewport(&vp);
        for(int e=0;e<2;++e) {
            d->SetDepthStencilSurface(nullptr);d->SetRenderTarget(0,im.target[e].Get());d->SetDepthStencilSurface(im.depth[e].Get());
            d->Clear(0,nullptr,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,0xff080b10,1,0);
        }
        d->SetDepthStencilSurface(nullptr); d->SetRenderTarget(0,rt.Get());d->SetDepthStencilSurface(ds.Get());d->SetViewport(&vp);
        return true;
    }
    bool tiltDone=false;
    void possessionTilt(const Vec3& nativeUp,float screenX,float screenY,const Projection& lens) {
    possUpOk=false;
    if(possessionFrameReady&&possessionLevelFinal) {
        // floor normal in the camera frame (same formula as possessionLevelRotation): tilt statistics against the entry reference, and the live level
        Vec3 up{-nativeUp.x*screenX/lens.focalX,nativeUp.y*screenY/lens.focalY,nativeUp.z};
        if(normalize(up)&&up.y>.1f) {
            possUpNow[0]=up.x;possUpNow[1]=up.y;possUpNow[2]=up.z;possUpOk=true;
            if(!tiltRefValid) {tiltRef=up;tiltRefValid=true;}
            const double roll=(std::atan2(up.x,up.y)-std::atan2(tiltRef.x,tiltRef.y))*180./Pi;
            const double pit=(std::atan2(up.z,std::hypot(up.x,up.y))-std::atan2(tiltRef.z,std::hypot(tiltRef.x,tiltRef.y)))*180./Pi;
            ++tiltN;tiltRollSq+=roll*roll;tiltPitchSq+=pit*pit;tiltRollMax=std::max(tiltRollMax,std::fabs(roll));tiltPitchMax=std::max(tiltPitchMax,std::fabs(pit));tiltRollMean+=roll;
            if(possessionLiveLevel) {possessionLevel=possessionLevelRotation(nativeUp,screenX,screenY,lens);++tiltLiveUsed;}
        }
    }
    }
    // The camera of the normal (front) view: the reference level of the possession entry (0.8.74). Cube faces never come here: they are mapped into this view's space.
    void selectView(int v,const Projection& lens) {
        if(v!=0||viewApplied==0) return;
        viewApplied=0;viewReady=true;
        const Vec3 nativeUp{native.read<float>(0x77f470),native.read<float>(0x77f474),native.read<float>(0x77f478)};
        const float screenX=native.read<float>(0x77f4cc),screenY=native.read<float>(0x78093c);
        const bool ready=possessionProjectionReady(nativeUp,screenX,screenY,lens);
        possessionFrameReady=ready&&possessionAnchorValid;
        if(!tiltDone) {tiltDone=true;possessionTilt(nativeUp,screenX,screenY,lens);}
        Pose c=possessionAnchor;c.orientation=normalized(multiply(c.orientation,possessionLevel));tableCamera=c;
    }
    // Chooses, once per game frame, where the native camera sits in VR space.
    void prepareFrame(const Projection& lens,bool possessed) {
        tablePrepared=true;possessedFrame=possessed;TileCover::tableValid=0;ViewExpand::rectValid=0;
        pointerLens=lens;pointerLensOk=true;
        pickActive=tablePointer&&!possessed&&(request.pointerFlags&PointerAimValid)&&!(request.pointerFlags&PointerActive)&&!native.frontend();
        pickFound=false;pickT=6.f;pickO=request.rayOrigin;pickD=request.rayDirection;
        activeUnits=possessed?request.possessionUnitsPerMeter:request.unitsPerMeter;
        Pose camera=request.anchor;
        // Freeze the world frame on possession entry. Tracking belongs only to
        // the eye view; feeding head movement back into the game camera made
        // the whole world swim and caused severe visual/vestibular conflict.
        if(possessed) {
            const Vec3 nativeUp{native.read<float>(0x77f470),native.read<float>(0x77f474),native.read<float>(0x77f478)};
            const float screenX=native.read<float>(0x77f4cc),screenY=native.read<float>(0x78093c);
            possessionFrameReady=possessionProjectionReady(nativeUp,screenX,screenY,lens);
            if(!possessionAnchorValid||possessionRecenter!=request.recenterSerial) {
                possessionAnchor={yawOnly(request.eyes[0].pose.orientation),request.eyes[0].pose.position};
                // The native first-person camera has a persistent downward pitch
                // (about 30 degrees in the 0.8.32 report). Counter-rotate its
                // reconstructed geometry once; do not drive it from head motion.
                possessionLevel=possessionLevelRotation(nativeUp,screenX,screenY,lens);
                possessionLevelFinal=possessionFrameReady;
                possessionPitch=2*std::atan2(-possessionLevel.x,possessionLevel.w);
                possessionAnchorValid=true;possessionRecenter=request.recenterSerial;
                log("Possession VR: world anchor; floor %.1f degrees, projection %s; nativeUp %.4f %.4f %.4f screen/lens %.3f %.3f",possessionPitch*180/Pi,possessionFrameReady?"READY":"WAIT (table camera rejected)",nativeUp.x,nativeUp.y,nativeUp.z,screenX/lens.focalX,screenY/lens.focalY);
            }
            // 0.8.94.18: report 20261005_093913 (V0.8.94.17): the entry reference was taken on the FIRST possession frame, still drawn with the table tilt
            // (51.3 degrees) while the game's possession camera is 45 degrees from the next frame on: the whole possession was 6.3 degrees off ('pitch rms
            // 6.33'). During the first frames of a possession each frame is levelled with ITS OWN camera (the transition frame included), then the reference
            // is frozen as before. Same function as the manual recenter; the game camera is steady (trace: 45.0 +/- 0.05 deg) once the transition is over.
            if(possessionEntryFrames<possessionEntryWindow) {++possessionEntryFrames;
                if(possessionFrameReady) {const Quat lv=possessionLevelRotation(nativeUp,screenX,screenY,lens);const float pp=2*std::atan2(-lv.x,lv.w);
                    if(std::fabs(pp-possessionPitch)>.2f*Pi/180&&entryLevelLogs<12) {++entryLevelLogs;log("Possession VR (0.8.94.18): entry frame %d levelled with its own camera: %.1f -> %.1f degrees",possessionEntryFrames,possessionPitch*180/Pi,pp*180/Pi);}
                    possessionLevel=lv;possessionPitch=pp;possessionLevelFinal=true;tiltRefValid=false;}}
            if(!possessionLevelFinal&&possessionFrameReady) {
                possessionLevel=possessionLevelRotation(nativeUp,screenX,screenY,lens);
                possessionPitch=2*std::atan2(-possessionLevel.x,possessionLevel.w);possessionLevelFinal=true;
                log("Possession VR: automatic floor reference READY %.1f degrees (same reference as manual stick recenter, no head anchor recapture)",possessionPitch*180/Pi);
            }
            if(SurroundHack::drawView==0) {possessionTilt(nativeUp,screenX,screenY,lens);tiltDone=true;}   // a side view must not feed the statistics / the live level of the front view (selectView does it when the front view starts)
            // 0.8.81: the camera of each view is chosen per view (selectView): this is the reference level of the front view
            camera=possessionAnchor;camera.orientation=normalized(multiply(camera.orientation,possessionLevel));
        } else {possessionEntryFrames=0;possessionAnchorValid=false;possessionLevelFinal=false;possessionFrameReady=false;tiltRefValid=false;possUpOk=false;camera.orientation=multiply(camera.orientation,pitch(-request.tablePitchDegrees*Pi/180));}
        table={};
        if(!possessed&&request.tableAnchor) {
            // Demeo-style: the dungeon floor rectangle is fitted to a fixed table whose pose is
            // chosen by the player (grab, rotate, scale). Native zoom changes the visible area.
            auto units=native.tabletop(lens,1.f,tableCrop);
            if(units.valid) {frameFloor=dot(units.up,units.center);floorForHeights=frameFloor;}
            if(units.valid&&tableView==1) {
                // 0.4.7: the table shows at least everything the PC screen shows (rectangle around the
                // screen footprint, widened to the table shape); the engine view is expanded to fill it.
                ScreenRect area=LensHack::shrink(native.renderArea(lens),lens); // 0.6.9: same footprint with the widened camera
                auto box=native.tabletopArea(lens,1.f,1.f,area,true);
                if(box.valid) {
                    // 0.5.8: the extra depth is shared again between both sides (0.5.5-0.5.7 put it all on the player
                    // side: the table's near edge fell behind the engine's near limit -> empty strip on the player side).
                    screenFoot=box;
                    box=fitAspect(box,tableAspect>0?tableAspect:.75f);
                    // 0.5.11: the engine cannot draw what lies under / behind its camera. The player-side edge of the
                    // table is kept within the floor seen through a screen enlarged 2x towards the player (below that the
                    // rays leave the floor: 0.5.10 empty strip on the player side when zoomed in); the depth taken off
                    // there goes to the far side (the far distance follows the far corners).
                    float nearShift=0;
                    // 0.5.14: not with the top-down camera (it sees the whole table; 0.5.13 moved the table 4.6 units away: cut table).
                    if(!topDown.active) {float cy=(area.top+area.bottom)*.5f;
                     auto reach=native.tabletopArea(lens,1.f,1.f,ScreenRect{area.left,area.top,area.right,cy+(area.bottom-cy)*2.f},true);
                     if(reach.valid) {
                         float nearReach=dot(reach.forward,reach.center)-reach.halfDepth,nearBox=dot(box.forward,box.center)-box.halfDepth;
                         // 0.5.12: also never nearer than the geometry the game actually produced (+0.3 unit): the table's
                         // player side (the game's near side) showed empty tiles / a staircase edge in 0.5.11.
                         if(haveNearGeom) nearReach=std::max(nearReach,nearGeom+.3f);
                         if(nearBox<nearReach) {
                             nearShift=nearReach-nearBox;
                             box.center=add(box.center,scale(box.forward,nearReach-nearBox));
                             if(nearShiftLogs<5&&nearReach-nearBox>.5f) {++nearShiftLogs;log("Table near edge moved %.2f units away from the player (game near limit %.2f)",nearReach-nearBox,nearGeom);}
                         }
                     }}
                    // The engine keeps computing down to the unshifted near edge (otherwise the measured near limit
                    // would follow the table and push it further and further away).
                    {Tabletop cover=box;cover.halfDepth+=nearShift*.5f;cover.center=add(cover.center,scale(cover.forward,-nearShift*.5f));
                     updateExpansion(lens,area,cover);}
                    if(measuredLevels&&haveLevels&&haveWalk) box.center=add(box.center,scale(box.up,walkOffset));
                    units=box;
                }
            }
            else if(units.valid&&measuredLevels&&haveLevels) {
                ScreenRect area=native.renderArea(lens);
                float rectLow=frameFloor+std::min(lowOffset,haveWalk?walkOffset:0.f);
                auto leveled=makeLeveledTabletop(units.up,rectLow,frameFloor+highOffset,lens,area.left,area.top,area.right,area.bottom,tableCrop);
                // Table surface (and base) at the walking floor; deeper geometry shows as holes in the table.
                if(leveled.valid&&haveWalk) leveled.center=add(leveled.center,scale(leveled.up,frameFloor+walkOffset-rectLow));
                if(leveled.valid) units=leveled;
            }
            if(units.valid&&tableAspect>0&&tableView!=1) units=limitDepth(units,tableAspect);
            if(units.valid) {anchorUnits=units;haveAnchor=true;}
            else if(haveAnchor) ++anchorFallbacks;
            if(haveAnchor) {
                float width=std::clamp(request.tableWidthMeters,.1f,20.f);
                activeUnits=2*anchorUnits.halfWidth/width;
                table=scaled(anchorUnits,1/activeUnits);
                camera=compose(request.table,nativeCameraInTable(table));
                anchored=true;
                if(!loggedAnchor) {
                    log("Table anchor active: visible dungeon %.2f x %.2f game units on a %.2f x %.2f m table (%.2f units/m)",
                        anchorUnits.halfWidth*2,anchorUnits.halfDepth*2,table.halfWidth*2,table.halfDepth*2,activeUnits);
                    loggedAnchor=true;
                }
            }
        }
        if(!anchored&&!possessed&&tableEnabled&&clipSupported) table=native.tabletop(lens,activeUnits,tableCrop);
        tableCamera=camera;
        if(anchored&&topDown.active&&!possessed&&!native.frontend()&&true) {
            // 0.6.9.3: largest focal factor keeping the table's dungeon (floor and wall tops) between the bars.
            // The corners are projected with the CURRENT (already scaled by 'applied') lens, then rescaled.
            const ScreenRect fs=fullScreen(lens);const float W=fs.right-fs.left,H=fs.bottom-fs.top;
            const float L=fs.left+.01f*W,R=fs.right-.01f*W,T=fs.top+pickTop*H,B=fs.top+pickBottom*H;
            const float a=LensHack::applied;float s=1e9f;bool ok=W>64&&H>64;
            for(float h:{0.f,std::max(highOffset-(haveWalk?walkOffset:0.f),0.f)}) for(float x:{-1.f,1.f}) for(float z:{-1.f,1.f}) {
                if(!ok) break;
                Vec3 q=anchorUnits.point(x*anchorUnits.halfWidth,z*anchorUnits.halfDepth,h);const float depth=-q.z;
                if(!(depth>.05f)) {ok=false;break;}
                const float dx=lens.focalX*(q.x/depth),dy=-lens.focalY*(q.y/depth);
                if(dx>1e-3f) s=std::min(s,(R-lens.centerX)/dx); else if(dx<-1e-3f) s=std::min(s,(L-lens.centerX)/dx);
                if(dy>1e-3f) s=std::min(s,(B-lens.centerY)/dy); else if(dy<-1e-3f) s=std::min(s,(T-lens.centerY)/dy);
            }
            float target=ok&&std::isfinite(s)&&s>0?std::clamp(a*s,pickMin,1.f):pickMin;
            pickFits=ok&&std::isfinite(s)&&s>=.90f&&a<.999f; // 0.6.9.6: .98 was stricter than the 3 % smoothing step -> often off // table currently between the bars
            if(std::fabs(target-pickWant)>.01f) pickWant+=(target-pickWant)*.15f; // 0.7.4: .03 dead band kept x0.33 while x0.30 was wanted
            pickWant=std::clamp(pickWant,pickMin,1.f);
            if(pickLogs<12&&std::fabs(pickWant-target)<.01f&&std::fabs(target-float(LensHack::applied))>.05f) {++pickLogs;log("Table picking: focal factor -> x%.2f (min x%.2f, bars %.2f-%.2f)",target,pickMin,pickTop,pickBottom);}
            LensHack::want=pickWant;
        } else {LensHack::want=possessed?possessionFovFactor:1.f;pickFits=false;}
        if(!anchored&&table.valid&&!loggedTable) {
            log("Table bounds: %.3fm x %.3fm, center=(%.3f,%.3f,%.3f), up=(%.3f,%.3f,%.3f), scale=%.3f",
                table.halfWidth*2,table.halfDepth*2,table.center.x,table.center.y,table.center.z,table.up.x,table.up.y,table.up.z,activeUnits);
            loggedTable=true;
        }
        if(!possessed&&!table.valid&&!warnedTable&&(request.tableAnchor||(tableEnabled&&clipSupported))) {
            log("Table footprint unavailable: native Z row=(%.6f,%.6f,%.6f), translation=(%.3f,%.3f,%.3f). Original stereo view retained.",
                native.read<float>(0x77f470),native.read<float>(0x77f474),native.read<float>(0x77f478),
                native.read<float>(0x780940),native.read<float>(0x780944),native.read<float>(0x780948));
            warnedTable=true;
        }
    }
    // 0.6.3: the Keeper's hand is a native 3D mesh drawn under the cursor, much closer to the camera than the
    // dungeon (it floats above the table and was hidden by the ceiling plane). Draws whose screen box holds the
    // cursor and whose nearest depth is well above the frame's typical depth are tagged as the hand.
    Vec3 handMin{},handMax{};uint32_t handDraws=0,handVerts=0;bool handFrameCursor=false;float handCeiling=1e9f;int handFrameState=0;float rhwTypical=0;
    uint64_t statHandFrames=0,statHandDraws=0;float lastHandRhw=0,lastHandTypical=0;UINT gameW=0,gameH=0;
    struct HandProbe {uint32_t index,count;float rmin,rmax,above,w,h;bool tagged;};
    std::vector<HandProbe> handProbe,handProbeShown;uint32_t frameWorldIndex=0;uint64_t lastHandProbeLog=0;
    void classifyHand(IDirect3DDevice9* d,DrawRecord& rec,const unsigned char* input,UINT stride,UINT count) {
        const uint32_t index=frameWorldIndex++;
        if(!handToController) return;
        if(handFrameState==0) {
            handFrameState=1;
            ComPtr<IDirect3DSurface9> rt;D3DSURFACE_DESC dd{};
            if(SUCCEEDED(d->GetRenderTarget(0,&rt))&&rt&&SUCCEEDED(rt->GetDesc(&dd))) {gameW=dd.Width;gameH=dd.Height;}
            handFrameCursor=gameW>0&&mouse.cursorPixel(float(gameW),float(gameH),cursorPx,cursorPy);
            handCeiling=(haveLevels&&activeUnits>0&&table.valid)?dot(table.up,table.center)+(highOffset-(haveWalk?walkOffset:0.f)+.4f)/activeUnits:1e9f;
        }
        float x0=1e9f,y0=1e9f,x1=-1e9f,y1=-1e9f,rmax=0,rmin=1e9f,rsum=0;
        for(UINT i=0;i<count;++i) {float v[4];memcpy(v,input+i*stride,16);x0=std::min(x0,v[0]);y0=std::min(y0,v[1]);x1=std::max(x1,v[0]);y1=std::max(y1,v[1]);rmax=std::max(rmax,v[3]);rmin=std::min(rmin,v[3]);rsum+=v[3];}
        const float mean=rsum/std::max<UINT>(count,1);
        if(!(rmax>0)) return;
        const bool isNear=rhwTypical>0&&rmax>rhwTypical*1.8f;
        if(!isNear) rhwTypical=rhwTypical>0?rhwTypical+.002f*(mean-rhwTypical):mean;
        if(!handFrameCursor) return;
        const float m=32;
        if(cursorPx<x0-m||cursorPx>x1+m||cursorPy<y0-m||cursorPy>y1+m) return;
        // Height of the draw above the ceiling plane (camera space, metres), like the HideFloatingUi clip.
        float above=-1e9f;
        if(handCeiling<1e8f) for(UINT i=0;i<count;++i) {Vec3 q=toLocal(tableCamera,positions[i]);above=std::max(above,dot(table.up,q)-handCeiling);}
        const bool small=x1-x0<=gameW*.35f&&y1-y0<=gameH*.45f;
        const bool tagged=small&&(above>0||isNear);
        if(handProbe.size()<24) handProbe.push_back({index,count,rmin,rmax,above,x1-x0,y1-y0,tagged});
        if(!tagged) return;
        rec.hand=true;++handDraws;handVerts+=count;lastHandRhw=rmax;lastHandTypical=rhwTypical;
        for(UINT i=0;i<count;++i) {Vec3 p=positions[i];
            if(handDraws==1&&i==0) {handMin=handMax=p;}
            handMin={std::min(handMin.x,p.x),std::min(handMin.y,p.y),std::min(handMin.z,p.z)};handMax={std::max(handMax.x,p.x),std::max(handMax.y,p.y),std::max(handMax.z,p.z)};}
    }
    // 0.6.4: the captured cursor image goes into a shared texture (256x256) read by the VR thread.
    ComPtr<IDirect3DTexture9> keeperSys,keeperTex;HANDLE keeperHandle=nullptr;uint32_t keeperSerial=0;int keeperW=0,keeperH=0,keeperHotX=0,keeperHotY=0;
    // 0.9.47.55 (report 231950, Quest Pro friend at 800x600): the cursor images scale with the game resolution (held creature 118x112 at 1024x768 = 92x87 at 800x600);
    // the 'something is held' size thresholds were written for 1024 and refused every creature below them (2D forever). Proportional to the game width (1024 unchanged).
    int heldMin(int base) const {const float sc=uiW>0?std::clamp(float(uiW)/1024.f,.5f,2.f):1.f;return int(base*sc+.5f);}   // 0.9.47.56: both ways (1280x1024 / 1600x1200 enlarge the normal hand past 100 px: it would pass for a held creature)
    std::vector<digitnum::DigitGroup> digitGroupsNow;   // 0.9.47.31
    void attachDigitReplay() {   // 0.9.47.31: records are final here: every digit vertex is linked to its record, with its offsets in the reading frame
        digitGroupsNow.clear();
        if(!digitFrame.reading||digitFrame.pieces.empty()||records.empty()) return;
        int groups=0;for(size_t i=0;i<digitFrame.lastGroup.size();++i) groups=std::max(groups,digitFrame.lastGroup[i]+1);
        digitGroupsNow.assign(size_t(groups),digitnum::DigitGroup{});std::vector<char> have(size_t(groups),0);
        unsigned linked=0;
        for(size_t i=0;i<digitFrame.pieces.size()&&i<digitFrame.lastGroup.size();++i) {
            const auto& pc=digitFrame.pieces[i];const int g=digitFrame.lastGroup[i];
            if(!pc.gHave||g<0||pc.ox.size()!=pc.ids.size()||pc.oy.size()!=pc.ids.size()) continue;
            if(!have[size_t(g)]) {have[size_t(g)]=1;digitnum::DigitGroup& dg=digitGroupsNow[size_t(g)];dg.centre=pc.gC;dg.up=pc.up;dg.right=pc.gRight;dg.pull=pc.gPull;}
            for(size_t j=0;j<pc.ids.size();++j) {
                const size_t at=pc.base+size_t(pc.ids[j])*pc.stride;
                size_t lo=0,hi=records.size();   // last record whose first vertex is <= at
                while(lo+1<hi) {const size_t mid=(lo+hi)/2;if(records[mid].vertices<=at) lo=mid;else hi=mid;}
                DrawRecord& rc=records[lo];
                if(at<rc.vertices||rc.stride==0||at>=rc.vertices+size_t(rc.count)*rc.stride||(at-rc.vertices)%rc.stride!=0) continue;
                digitnum::DigitVertex dv;dv.index=uint32_t((at-rc.vertices)/rc.stride);dv.ox=pc.ox[j];dv.oy=pc.oy[j];dv.group=uint32_t(g);rc.digitVerts.push_back(dv);++linked;
            }
        }
        static DWORD lastLog=0;if(linked&&DWORD(GetTickCount()-lastLog)>5000){lastLog=GetTickCount();log("Damage digits facing (0.9.47.31): %u vertices of %d groups turned to the headset at every VR frame",linked,groups);}
    }
    void joinRecordedBadges() {
        attachDigitReplay();
        std::vector<ReplaySprite*> pieces;for(auto& rec:records)for(auto& sprite:rec.sprites)pieces.push_back(&sprite);
        unsigned joined=joinBadgePlanes(pieces);static unsigned reports=0;
        if(joined&&reports++<8)log("Badge shared plane (0.9.24): %u above-floor components, native UV axes, exact original shape, depth tolerance 0.2 percent; no low-floor joining",joined);
    }
    void uploadKeeper(IDirect3DDevice9* d,Package& p) {
        if(!keeper.original) return;
        uint32_t serial;{std::lock_guard<std::mutex> g(keeper.m);serial=keeper.serial;}
        if(serial!=keeperSerial) {
            if(!keeperTex) {
                keeperHandle=nullptr;
                if(FAILED(d->CreateTexture(KeeperCursor::MaxSize,KeeperCursor::MaxSize,1,0,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&keeperTex,&keeperHandle))||!keeperHandle||
                   FAILED(d->CreateTexture(KeeperCursor::MaxSize,KeeperCursor::MaxSize,1,0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&keeperSys,nullptr))) {
                    keeperTex.Reset();keeperSys.Reset();keeperHandle=nullptr;log("Keeper hand: shared texture failed");keeper.original=nullptr;return;
                }
            }
            D3DLOCKED_RECT lr{};
            if(SUCCEEDED(keeperSys->LockRect(0,&lr,nullptr,0))) {
                std::lock_guard<std::mutex> g(keeper.m);
                for(int y=0;y<KeeperCursor::MaxSize;++y) {
                    auto* row=reinterpret_cast<uint32_t*>(static_cast<unsigned char*>(lr.pBits)+size_t(y)*lr.Pitch);
                    for(int x=0;x<KeeperCursor::MaxSize;++x) row[x]=(y<keeper.h&&x<keeper.w)?keeper.pixels[size_t(y)*keeper.w+x]:0;
                }
                keeperW=keeper.w;keeperH=keeper.h;keeperHotX=keeper.hotX;keeperHotY=keeper.hotY;
                keeperSys->UnlockRect(0);
                if(SUCCEEDED(d->UpdateTexture(keeperSys.Get(),keeperTex.Get()))) keeperSerial=serial;
            }
        }
        if(keeperTex&&keeperW>0) {p.keeperHandle=keeperHandle;p.keeperW=keeperW;p.keeperH=keeperH;p.keeperHotX=keeperHotX;p.keeperHotY=keeperHotY;}
    }
    bool hudWanted() const {
        return !uiFailed&&decoupledFrame&&decoupledKind==1&&(request.handFlags&RequestHudShown)&&request.mode!=3&&!native.frontend();
    }
    uint64_t held3dDraws=0,held3dTriangles=0,heldUprightFrames=0,held3dMissingFrames=0;
    // 0.9.47.24 (held creature in real 3D): see CHANGEMENTS_0.9.47.24.md. Report 081721 (0.9.47.23) showed creatures that were part flat window, part
    // polygons: only the triangles whose vertices matched a native hook record were converted. Now every vertex of a model draw gets its depth from the z
    // the game itself writes in the vertex (linear in the model depth); the hooks only measure the slope (pixels of depth per unit of z).
    HeldSignDecider heldDecider;int heldSignLogs=0;uint32_t heldUndecidedDraws=0;bool heldDepthFlip=false;   // sign +1 = a larger model depth is FARTHER (overlap of front and back faces, HeldModel.hpp)
    double heldSlope=0;bool heldSlopeKnown=false;int heldSlopeLogs=0;bool heldRefKnown=false;float heldVzRef=0;std::vector<unsigned char> heldUiRest;
    uint64_t heldGate[8]={0,0,0,0,0,0,0,0},heldNearHand=0;   // 0.9.47.53 diagnostics: draws turned away before the held-creature test (cursor unknown / laser off the table / paused / small keeper image / not a triangle list), draws examined, flat draws, near the hand
    uint64_t heldDefaultSlopeDraws=0,heldRej[5]={0,0,0,0,0},heldFilledVertices=0,heldFilledTriangles=0,heldFlatDraws=0,heldAnalyticDraws=0;   // refused: no depth scale / off window / sign undecided / texture not shareable / no triangle
    bool recordHeld3D(IDirect3DDevice9* d,D3DPRIMITIVETYPE& type,UINT& primitives,const void*& verts,UINT stride,const void*& indices,D3DFORMAT& indexFormat,UINT& count,DWORD fvf) {
        if(!HeldGeometry::enabled) return false;
        if(keeperW<heldMin(100)||keeperH<heldMin(100)) return false;   // no held thing: nothing to count
        if(!cursorKnown) {++heldGate[0];return false;}
        if(!lastPointerOnTable) {++heldGate[1];return false;}
        if(PauseWatch::paused==1) {++heldGate[2];return false;}
        if(type!=D3DPT_TRIANGLELIST&&type!=D3DPT_TRIANGLESTRIP&&type!=D3DPT_TRIANGLEFAN) {++heldGate[3];return false;}
        if(stride<20||count<3) {++heldGate[4];return false;}
        ++heldGate[5];
        const auto* input=static_cast<const unsigned char*>(verts);
        float bx0=1e9f,by0=1e9f,bx1=-1e9f,by1=-1e9f,zmin=1e30f,zmax=-1e30f;
        for(UINT i=0;i<count;++i) {float v[3];memcpy(v,input+size_t(i)*stride,12);if(!std::isfinite(v[0])||!std::isfinite(v[1])||!std::isfinite(v[2])) return false;
            bx0=std::min(bx0,v[0]);bx1=std::max(bx1,v[0]);by0=std::min(by0,v[1]);by1=std::max(by1,v[1]);zmin=std::min(zmin,v[2]);zmax=std::max(zmax,v[2]);}
        // keys: vertices whose native depth was recorded by the hooks (exact screen values)
        std::vector<HeldGeometry::Depth> depth(count);std::vector<uint8_t> known(count,0);UINT hits=0;
        for(UINT i=0;i<count;++i) {auto it=HeldGeometry::depths.find(HeldGeometry::key(input+size_t(i)*stride));if(it!=HeldGeometry::depths.end()) {known[i]=1;depth[i]=it->second;++hits;}}
        const bool modelDraw=hits>=1||(zmax-zmin)>=2e-6f;   // interface boxes and icons are flat in z
        if(!modelDraw) {++heldFlatDraws;return false;}
        ++heldGate[6];
        const float R=heldRadius*uiW/1024.f,mx=(bx0+bx1)*.5f,my=(by0+by1)*.5f;
        if(!(mx>cursorPx-2.5f*R&&mx<cursorPx+3.5f*R&&my>cursorPy-2.f*R&&my<cursorPy+5.f*R)) {++heldRej[1];return false;}   // a model drawn elsewhere (creature panel...), not the one in the hand
        ++heldNearHand;
        // slope of the depth against the game's z, measured on the hooked vertices (several hundred per creature)
        {
            std::vector<float> fz,fd;float originZ=0;bool haveOrigin=false;
            for(UINT i=0;i<count&&fz.size()<4000;++i) if(known[i]) {float dz=0;if(!haveOrigin){originZ=depth[i].originZ;haveOrigin=true;}
                if(heldDepthPixels(depth[i].inputZ,originZ,depth[i].ratio,1.f,dz)) {float v[3];memcpy(v,input+size_t(i)*stride,12);fz.push_back(v[2]);fd.push_back(dz);}}
            double slope=0,corr=0;
            if(heldSlopeFit(fz.data(),fd.data(),fz.size(),slope,corr)&&std::fabs(corr)>.98) {
                const bool first=!heldSlopeKnown;const double old=heldSlope;
                heldSlope=first||(slope*old<0)?slope:old*.9+slope*.1;heldSlopeKnown=true;
                if((first||std::fabs(slope-old)>.25*std::fabs(old))&&heldSlopeLogs<10) {++heldSlopeLogs;
                    float kk=0;memcpy(&kk,reinterpret_cast<const void*>(HeldGeometry::k+0x77f490),4);
                    log("Held 3D depth scale (0.9.47.24): %.1f px of depth per unit of game z (correlation %.4f over %zu hooked vertices); analytic check ratio/k = %.1f (call ratio %.1f, k %g)",slope,corr,fz.size(),
                        std::isfinite(kk)&&kk!=0.f?double(depth[0].ratio)/kk:0.0,double(depth[0].ratio),double(kk));}
            }
        }
        double slope=heldSlope;
        if(!heldSlopeKnown) {   // no creature measured yet: analytic value (xy/z scale of the call over the game's z factor 0x77F490)
            float kk=0;memcpy(&kk,reinterpret_cast<const void*>(HeldGeometry::k+0x77f490),4);float ratio=0;for(UINT i=0;i<count;++i) if(known[i]) {ratio=depth[i].ratio;break;}
            if(ratio!=0.f&&std::isfinite(kk)&&std::fabs(kk)>1e-9f) {slope=double(ratio)/kk;++heldAnalyticDraws;}
            else {   // 0.9.47.27: no native hook saw this creature (reports 0.9.47.24 and 0.9.47.26: the first creatures of a game were flat until the game happened to project a model through the hooks); the scale measured on every report so far (call ratio 6400 / k 0.1) is used until a measurement exists
                slope=64000.0;++heldAnalyticDraws;++heldDefaultSlopeDraws;
                if(heldDefaultSlopeDraws==1) log("Held 3D (0.9.47.27): no native hook data for the held creature yet: default depth scale %.0f px per unit of game z used until measured",slope);
            }
        }
        auto at=[&](UINT n)->uint32_t{return !indices?n:indexFormat==D3DFMT_INDEX32?static_cast<const uint32_t*>(indices)[n]:static_cast<const uint16_t*>(indices)[n];};
        std::vector<uint32_t> modelIds;heldUiRest.clear();
        float heldMaxTriRange=0;UINT heldTriFlat=0,heldTriModel=0;   // 0.9.47.54 diagnostics
        for(UINT t=0;t<primitives;++t) {
            uint32_t a,b,c;if(type==D3DPT_TRIANGLELIST){a=at(t*3);b=at(t*3+1);c=at(t*3+2);}
            else if(type==D3DPT_TRIANGLESTRIP){a=at(t);b=at(t+1);c=at(t+2);if(t&1)std::swap(a,b);}
            else {a=at(0);b=at(t+1);c=at(t+2);}
            if(a>=count||b>=count||c>=count) return false;
            float za,zb,zc;memcpy(&za,input+size_t(a)*stride+8,4);memcpy(&zb,input+size_t(b)*stride+8,4);memcpy(&zc,input+size_t(c)*stride+8,4);
            const bool model=known[a]||known[b]||known[c]||std::max({za,zb,zc})-std::min({za,zb,zc})>=2e-6f;
            heldMaxTriRange=std::max(heldMaxTriRange,std::max({za,zb,zc})-std::min({za,zb,zc}));if(model)++heldTriModel;else ++heldTriFlat;
            if(model) modelIds.insert(modelIds.end(),{a,b,c});
            else for(uint32_t i:{a,b,c}) heldUiRest.insert(heldUiRest.end(),input+size_t(i)*stride,input+size_t(i+1)*stride);
        }
        if(modelIds.empty()) {
            ++heldRej[4];static uint32_t emptyN=0;++emptyN;
            if(emptyN<=8||emptyN%400==0) log("Held 3D refused draw #%u (0.9.47.54): no triangle of the draw looks like a model: type %d, %u primitives, %u vertices, stride %u, fvf %08lX, indices %d, z %.6f..%.6f (range %.6f), hooked vertices %u, largest z range inside one triangle %.8f, flat triangles %u, box %.0f,%.0f..%.0f,%.0f, cursor %.0f,%.0f, slope %.0f",
                emptyN,int(type),unsigned(primitives),unsigned(count),unsigned(stride),(unsigned long)fvf,indices?1:0,double(zmin),double(zmax),double(zmax-zmin),unsigned(hits),double(heldMaxTriRange),unsigned(heldTriFlat),double(bx0),double(by0),double(bx1),double(by1),double(cursorPx),double(cursorPy),heldSlope);
            return false;}
        // ONE reference for the whole frame (all parts of the creature share it)
        if(!heldRefKnown) {double sum=0;for(UINT i:modelIds){float v[3];memcpy(v,input+size_t(i)*stride,12);sum+=v[2];}heldVzRef=float(sum/double(modelIds.size()));heldRefKnown=true;}
        std::vector<float> z(count,0.f);
        for(UINT i:modelIds) {float v[3];memcpy(v,input+size_t(i)*stride,12);z[i]=float(slope*double(v[2]-heldVzRef));if(!std::isfinite(z[i])||std::fabs(z[i])>100000.f) {++heldRej[4];static uint32_t zN=0;if(++zN<=6) log("Held 3D refused draw (0.9.47.54): depth out of range %.1f (reference %.6f, slope %.0f)",double(z[i]),double(heldVzRef),slope);return false;}}
        // depth direction: where a front triangle and a back triangle cover the same screen point, the front one is nearer
        std::vector<HeldTri> tris;tris.reserve(modelIds.size()/3);
        for(size_t n=0;n+2<modelIds.size();n+=3) {HeldTri h;for(int k=0;k<3;++k) {float v[2];memcpy(v,input+size_t(modelIds[n+k])*stride,8);h.x[k]=v[0];h.y[k]=v[1];h.d[k]=z[modelIds[n+k]];}tris.push_back(h);}
        HeldOverlapVote vote;heldOverlapVote(tris,vote);
        const int oldSign=heldDecider.sign;heldDecider.add(vote);
        if(heldDecider.sign!=oldSign&&heldSignLogs<12) {++heldSignLogs;log("Held 3D depth direction (0.9.47.24): %s (overlap of front and back faces: front nearer at the smaller depth %.0f times, at the larger depth %.0f times; evidence %.2f)",heldDecider.sign>0?"larger depth = farther":"larger depth = nearer",vote.nearSmaller,vote.nearLarger,heldDecider.weight>0?heldDecider.score/heldDecider.weight:0.);}
        int signNow=heldDecider.sign;
        if(signNow==0) {   // 0.9.47.27: not decided yet: the usual direction ('larger depth = farther', the only one seen in every report) is used at once instead of 150 flat draws; the evidence still decides afterwards
            ++heldUndecidedDraws;signNow=1;
            if(heldUndecidedDraws==1) log("Held 3D depth direction (0.9.47.27): provisional 'larger depth = farther' until the overlap evidence decides");
        }
        const float depthFlip=float(signNow)*(heldDepthFlip?-1.f:1.f);
        Snapshot current{};if(!current.capture(d)){current.release();return false;}
        std::array<uint32_t,4> ids{};
        {CopyCache::Guard guard;guard.d=d;guard.cache=&copies;for(int i=0;i<Snapshot::Samplers;++i)ids[i]=copies.copyFor(d,guard,current.texture[i]);ids[3]=shaders.idFor(current.pixel);}
        // If a bound texture cannot be shared, keep the original complete 2D rendering.
        for(int i=0;i<Snapshot::Samplers;++i)if(current.texture[i]&&!ids[i]){current.release();++heldRej[3];return false;}
        DrawRecord rec{D3DPT_TRIANGLELIST,UINT(modelIds.size()/3),UINT(modelIds.size()),stride-4,(fvf&~D3DFVF_POSITION_MASK)|D3DFVF_XYZ,arena.size(),0,D3DFMT_INDEX16,false,uint32_t(snapshots.size())};rec.held3d=true;
        snapshots.push_back(current);snapIds.push_back(ids);arena.resize(arena.size()+modelIds.size()*rec.stride);
        for(size_t n=0;n<modelIds.size();++n){const UINT i=modelIds[n];float xy[2];memcpy(xy,input+size_t(i)*stride,8);
            Vec3 q=heldModelPoint(xy[0],xy[1],z[i]*depthFlip,cursorPx,cursorPy);auto* out=arena.data()+rec.vertices+n*rec.stride;
            memcpy(out,&q,12);memcpy(out+12,input+size_t(i)*stride+16,stride-16);}
        records.push_back(rec);++held3dDraws;held3dTriangles+=rec.primitives;
        UINT unhooked=0;for(UINT i:modelIds) if(!known[i]) ++unhooked;heldFilledVertices+=unhooked;
        if(HeldGeometry::reports<12){++HeldGeometry::reports;float lo=1e9f,hi=-1e9f;for(UINT i:modelIds){lo=std::min(lo,z[i]);hi=std::max(hi,z[i]);}
            log("Held 3D capture (0.9.47.24): %u triangles (%u of %zu vertices NOT seen by the native hooks, depth taken from the game's z), depth %.1f..%.1f px, overlap votes %.0f/%.0f, sign %d%s, slope %.1f%s, cursor %.0f,%.0f, remaining UI triangles %u",
                rec.primitives,unhooked,modelIds.size(),lo,hi,vote.nearSmaller,vote.nearLarger,heldDecider.sign,heldDepthFlip?" INVERTED by launcher":"",slope,heldSlopeKnown?"":" (analytic)",cursorPx,cursorPy,UINT(heldUiRest.size()/stride/3));}
        // 0.9.47.24: small dumps of what was captured: screen x, y, model depth, the game's z, front/back as the game's culling sees it (offline check of shape and direction)
        {static unsigned heldDumps=0;static ULONGLONG heldDumpAt=0;const ULONGLONG nowMs=GetTickCount64();
         if(!kPublicBuild&&heldDumps<3&&rec.primitives>=120&&nowMs>heldDumpAt+4000) {++heldDumps;heldDumpAt=nowMs;
            std::error_code hec;auto folder=configurationPath().parent_path()/L"Captures";std::filesystem::create_directories(folder,hec);
            wchar_t hn[64];swprintf(hn,64,L"DK2VR_held_%03u.csv",heldDumps);FILE* hf=_wfopen((folder/hn).c_str(),L"w");
            if(hf) {fprintf(hf,"H,version,0.9.47.24,sign,%d,flip,%d,vzRef,%.6f,slope,%.4f,slopeMeasured,%d,cursor,%.1f,%.1f,triangles,%u,hooked,%u,of,%zu\n",heldDecider.sign,int(heldDepthFlip),double(heldVzRef),slope,int(heldSlopeKnown),cursorPx,cursorPy,rec.primitives,UINT(modelIds.size()-unhooked),modelIds.size());
                for(size_t n=0;n+2<modelIds.size()&&n<3*5000;n+=3) {float P[3][3];for(int k=0;k<3;++k) memcpy(P[k],input+size_t(modelIds[n+k])*stride,12);
                    fprintf(hf,"T,%.2f,%.2f,%.2f,%.6f,%.2f,%.2f,%.2f,%.6f,%.2f,%.2f,%.2f,%.6f,%.2f,%d\n",P[0][0],P[0][1],z[modelIds[n]],P[0][2],P[1][0],P[1][1],z[modelIds[n+1]],P[1][2],P[2][0],P[2][1],z[modelIds[n+2]],P[2][2],double(heldTriArea(P[0],P[1],P[2])),int(known[modelIds[n]])+10*int(known[modelIds[n+1]])+100*int(known[modelIds[n+2]]));}
                fclose(hf);}}}
        // Only extracted geometry leaves the 2D hand window; icons/tooltips remain there.
        type=D3DPT_TRIANGLELIST;primitives=UINT(heldUiRest.size()/stride/3);count=primitives*3;indices=nullptr;verts=heldUiRest.data();indexFormat=D3DFMT_INDEX16;
        return true;
    }
    // Replays one native 2D draw into the transparent interface target. RGB keeps the game's own blending
    // (premultiplied result over transparent black), alpha accumulates coverage (ONE, INVSRCALPHA).
    void uiReplay(IDirect3DDevice9* d,D3DPRIMITIVETYPE type,UINT primitives,const void* verts,UINT stride,const void* indices,D3DFORMAT indexFormat,UINT count,DWORD fvf) {
        // 0.6.1.2: the game draws into its own render target (game resolution, scaled to the back buffer at
        // Present by the wrapper). The interface target must match THAT surface: a larger target than the
        // game's depth buffer makes every draw a silent no-op (0.6.1/0.6.1.1: 0 % covered).
        ComPtr<IDirect3DSurface9> current,back;
        if(FAILED(d->GetRenderTarget(0,&current))||!current) return;
        D3DSURFACE_DESC bd{};current->GetDesc(&bd);
        if(SUCCEEDED(d->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&back))) {D3DSURFACE_DESC b{};back->GetDesc(&b);backW=b.Width;backH=b.Height;}
        if(!uiSurf||uiW!=bd.Width||uiH!=bd.Height) {
            uiSurf.Reset();uiTex.Reset();cursorBackup.Reset();uiDepth.Reset();uiDepthFailed=false;
            HRESULT hr=d->CreateTexture(bd.Width,bd.Height,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&uiTex,nullptr);
            if(SUCCEEDED(hr)) hr=uiTex->GetSurfaceLevel(0,&uiSurf);
            if(SUCCEEDED(hr)) hr=d->CreateRenderTarget(512,512,D3DFMT_A8R8G8B8,D3DMULTISAMPLE_NONE,0,FALSE,&cursorBackup,nullptr);
            if(FAILED(hr)) {uiFailed=true;uiSurf.Reset();uiTex.Reset();log("HUD: interface target failed %08lX; HUD disabled",(unsigned long)hr);return;}
            uiW=bd.Width;uiH=bd.Height;uiCleared=false;log("HUD: transparent interface target %ux%u (game render target), back buffer %ux%u",uiW,uiH,backW,backH);
        }
        // Game cursor candidate: small textured quad containing the cursor position. The last one of the
        // frame is the cursor (drawn after the buttons it covers).
        bool candidate=false;RECT box{};float uvA[2]{},uvB[2]{};
        ComPtr<IDirect3DBaseTexture9> tex0;d->GetTexture(0,&tex0);
        if(!uiCleared) cursorKnown=mouse.cursorPixel(float(uiW),float(uiH),cursorPx,cursorPy);
        recordHeld3D(d,type,primitives,verts,stride,indices,indexFormat,count,fvf);
        if(count==0) return;
        // 0.6.7.1: size of what the game draws between the bars (options screen / menus are wide and tall;
        // tooltips and held things are small). Not counted while the laser is on the table (held things).
        // 0.6.8: also never the held creature (its pieces carry varying depths) nor anything drawn near the cursor
        // (tooltips, held icons): 0.6.7.1 showed the wall during play while the laser was off the table.
        bool middleDraw=false,middleToScreen=false,boxOnly=false;
        if(count>0) {
            const auto* in=static_cast<const unsigned char*>(verts);float x0=1e9f,y0=1e9f,x1=-1e9f,y1=-1e9f,z0=1e9f,z1=-1e9f;
            for(UINT i=0;i<count;++i) {float v[3];memcpy(v,in+i*stride,12);x0=std::min(x0,v[0]);y0=std::min(y0,v[1]);x1=std::max(x1,v[0]);y1=std::max(y1,v[1]);z0=std::min(z0,v[2]);z1=std::max(z1,v[2]);}
            const float ih=std::min(float(uiH),uiW*.75f),oy=(uiH-ih)*.5f,cy=((y0+y1)*.5f-oy)/ih;
            const float R=heldRadius*uiW/1024.f,cx=(x0+x1)*.5f,cyy=(y0+y1)*.5f;
            const bool nearCursor=cursorKnown&&cx>cursorPx-1.5f*R&&cx<cursorPx+3.f*R&&cyy>cursorPy-1.5f*R&&cyy<cursorPy+2.5f*R;
            middleDraw=cy>.10f&&(cy<.69f||(objHold>0&&cy<.706f&&(y1-oy)/ih<=.724f))&&z1-z0<=1e-4f;
            // 0.8.94.16: NEW GAME, no statistics bar yet: the game draws its notification box lower (report 20261005_090346, box 333-571 of 768). Its last row
            // and its X / check buttons then start INSIDE last frame's box but end below the middle band (.69) -> they stayed in the bars' texture (X / check
            // shown in the statistics slot, cut from the box). A flat piece that starts inside last frame's box, horizontally within it, and ends above the
            // bottom bar (.775) belongs to the box. With a statistics bar (loaded game) the box ends above it and that bar starts below the box: unchanged.
            // 0.8.94.21: a flat piece whose centre lies in a text box the game drew (this frame or the last one) belongs to that box: frame, text, X / check.
            const bool inGameBox=TextBoxWatch::installed&&PauseWatch::paused!=1&&z1-z0<=1e-4f&&TextBoxWatch::inside(cx,cyy,float(uiW),float(uiH));
            if(inGameBox) {middleDraw=true;++statGameBoxPieces;}
            const bool boxTail=!TextBoxWatch::installed&&msgShown&&PauseWatch::paused!=1&&prevBox[2]>prevBox[0]&&z1-z0<=1e-4f&&cx>=prevBox[0]&&cx<=prevBox[2]&&y0>=prevBox[1]-2.f&&y0<=std::max(prevBox[3],prevTail)+2.f&&cy>.10f&&(y1-oy)/ih<=.775f;   // 0.8.94.18: also inside last frame's tail (the X / check start 0-3 px under the tiles' bottom: missed on most frames)
            if(boxTail&&!middleDraw) {middleDraw=true;boxTailY1=std::max(boxTailY1,y1);++statBoxTail;}
            else if(!boxTail&&msgShown&&prevBox[2]>prevBox[0]&&cx>=prevBox[0]&&cx<=prevBox[2]&&y0>prevBox[1]&&y0<std::max(prevBox[3],prevTail)+40.f&&y1<=oy+.80f*ih&&boxMissLogs<16) {++boxMissLogs;
                log("Box tail (0.8.94.18 diagnostic): piece %.0f,%.0f-%.0f,%.0f NOT taken (box %.0f-%.0f, tail %.0f, z span %.6f, middle %d)",x0,y0,x1,y1,prevBox[1],prevBox[3],prevTail,z1-z0,int(middleDraw));} // 0.8.23: objectives page open: its bottom row ('Quitter' button, centre .695-.70, bottom .719) belongs to the page, not to the statistics bar
            // 0.6.9.3: once the options screen is shown, draws near the cursor count again: with the laser on the
            // menu the cursor lies among its items, the measured size fell under the threshold -> menu blinking.
            // 0.6.9.6: 0.6.9.5 kept the screen 'open' after the options closed (cursor + tooltip counted). While shown,
            // only the cursor sprite's close surroundings are ignored (the menu items near the cursor count again).
            const bool nearSprite=cursorKnown&&cx>cursorPx-.5f*R&&cx<cursorPx+1.f*R&&cyy>cursorPy-.5f*R&&cyy<cursorPy+.8f*R;
            // 0.6.9.8: a real message / objectives page: a draw of some size between the bars, away from the cursor.
            // 0.6.9.9: a message box / objectives page = one draw of 8 %x4 % or a union of 30 %x10 % between the bars.
            // Draws at the cursor count while the laser is on the message itself (0.6.9.8: the box vanished when pointed).
            // 0.6.9.10: a box of 8 %x8 % or more is never a held thing / tooltip: counted even under the cursor
            // (with the laser on the table the game cursor often lies on the box -> 0.6.9.9 hid it and put it on the hand).
            // 0.6.9.12: a message box lies ENTIRELY between the top bar and the statistics bar and is not a full-screen layer
            // (0.6.9.11: a 1024x619 interface layer was taken for a box -> every bar and the minimap went blank).
            const float ihb=std::min(float(uiH),uiW*.75f),oyb=(uiH-ihb)*.5f,tyb=(y0-oyb)/ihb,byb=(y1-oyb)/ihb;
            const bool bigBoxRaw=middleDraw&&x1-x0>=.08f*uiW&&y1-y0>=.08f*uiH&&x1-x0<=.70f*uiW&&y1-y0<=.55f*ihb&&tyb>=.085f&&byb<=.72f;
            // 0.8.27: the hover tooltip (creature name + icon row, e.g. 'Elfe noire') is a small dark quad next to the cursor: it counted as a notification box
            // (bigBox -> boxOnly -> wall panel). A tooltip = small (<= 24 % x 16 %) box AT the cursor while the laser is on the table: it stays in the interface
            // texture so that it appears to the right of the hand (hand window). Notification boxes are wider / not under the cursor.
            // 0.8.28: also any bigger flat piece at the cursor while a held model is being drawn (this frame or the last one): a flat-z part of the held creature
            // (a low-detail mesh has big flat faces) was taken for a notification box and left out of the hand window -> rectangular 'hole' in the creature.
            const bool holdingModel=heldModelSeen||heldPrev;
            const bool heldOff47=heldOffTable&&holdingModel&&!lastPointerOnTable&&keeperW>=heldMin(100)&&keeperH>=heldMin(60);   // 0.9.47 (report 105954): creature held with the pointer off the table
            const bool tipLike=bigBoxRaw&&nearCursor&&(lastPointerOnTable||heldOff47)&&((x1-x0<=.24f*uiW&&y1-y0<=.16f*ihb)||(holdingModel&&x1-x0<=.45f*uiW&&y1-y0<=.40f*ihb));
            const bool bigBox=bigBoxRaw&&!tipLike&&!TextBoxWatch::installed;   // 0.8.94.21: with the game's own text box rectangle, no size guessing
            if(bigBoxRaw&&boxLogs<24) {++boxLogs;log("Box candidate: %.0fx%.0f px at %.0f,%.0f (interface %ux%u), near cursor %d, pointer on table %d -> %s",x1-x0,y1-y0,x0,y0,uiW,uiH,int(nearCursor),int(lastPointerOnTable),tipLike?"TOOLTIP (kept for the hand)":"notification box (wall)");}
            // 0.8.94.31: tooltip rectangle for the hand window = every small FLAT 2D piece (all vertices at rhw 1.0: the engine's 2D interface) drawn to the right
            // of the cursor while the laser is on the table: the tooltip frame, its text AND the small icons under it (report 20261005_150511 capture 4: the
            // 32 x 32 skull icon at +131 px was not part of the rectangle -> left under the hand; the rectangle came from one quad -> half-moved frame).
            // The held creature is a projected 3D model (rhw < 1): never part of this rectangle. Only the rectangle is measured: no piece changes layer.
            bool tipPiece=false;
            if(cursorKnown&&lastPointerOnTable&&PauseWatch::paused!=1&&!screenWhole&&(fvf&D3DFVF_POSITION_MASK)==D3DFVF_XYZRHW&&stride>=16&&z1-z0<=1e-4f&&!inGameBox&&!boxTail) {
                const float pw=x1-x0,ph=y1-y0,cyRel=(cyy-oy)/ih;
                const bool sized=pw>0&&ph>0&&pw<=.30f*uiW&&ph<=.12f*ih;
                const bool zone=cx>=cursorPx+.30f*R&&cx<=cursorPx+2.2f*R&&x1<=cursorPx+2.5f*R&&cyy>=cursorPy-.6f*R&&cyy<=cursorPy+1.f*R&&cyRel>=.085f&&cyRel<=.72f;
                const bool cursorSprite=x0<=cursorPx&&x1>=cursorPx&&y0<=cursorPy&&y1>=cursorPy&&pw<=120.f&&ph<=120.f;
                const bool inPanel=scan.open&&cx>=scan.rect[0]&&cx<=scan.rect[2]&&cyy>=scan.rect[1]&&cyy<=scan.rect[3];
                const bool inPrevBox=prevBox[2]>prevBox[0]&&cx>=prevBox[0]&&cx<=prevBox[2]&&cyy>=prevBox[1]&&cyy<=prevBox[3];
                if(sized&&zone&&!cursorSprite&&!inPanel&&!inPrevBox) {
                    bool flat2d=true;
                    for(UINT i=0;i<count&&flat2d;++i) {float w;memcpy(&w,in+i*stride+12,4);flat2d=w>=.999f&&w<=1.001f;}
                    tipPiece=flat2d;
                }
            }
            // 0.8.94.33: candidates only; the tooltip rectangle is built at the end of the frame (cluster grown from the tooltip's anchor, see tipCluster).
            if(tipPiece&&tipCands.size()<96) {++statTipPieces;tipCands.push_back({x0,y0,x1,y1,x0-cursorPx,y0-cursorPy,R/200.f});}
            // 0.8.94.34: background of a notification box (report 20261005_212232 capture 6: 'Premiere etape' box drawn at the hand, cut by the hand window).
            // The game draws each box on ONE flat 2D background quad (492 x 224 px in the log: 'Box candidate 492x224'). A flat 2D quad (rhw 1.0) of at least
            // 30 % x 12 % between the bars, while the laser is on the table, is such a background: it is erased from the HAND WINDOW only (not from the
            // interface layer, not from the panel scan). Tooltips (<= 260 px wide, 36 px high) and the held creature (3D, rhw < 1) never qualify.
            if(cursorKnown&&lastPointerOnTable&&PauseWatch::paused!=1&&!screenWhole&&(fvf&D3DFVF_POSITION_MASK)==D3DFVF_XYZRHW&&stride>=16&&z1-z0<=1e-4f) {
                const float pw=x1-x0,ph=y1-y0,cyRel=(cyy-oy)/ih;
                if(pw>=.30f*uiW&&ph>=.12f*ih&&pw<=.80f*uiW&&ph<=.60f*ih&&cyRel>=.085f&&cyRel<=.76f) {
                    bool flat2d=true;
                    for(UINT i=0;i<count&&flat2d;++i) {float w;memcpy(&w,in+i*stride+12,4);flat2d=w>=.999f&&w<=1.001f;}
                    if(flat2d) {++statHandBoxPieces;handBox[0]=std::min(handBox[0],x0);handBox[1]=std::min(handBox[1],y0);handBox[2]=std::max(handBox[2],x1);handBox[3]=std::max(handBox[3],y1);}
                }
            }
            // 0.8.26: the objectives page has a dark background quad spanning the whole width (>= 90 %) and 30-70 % of the height, between the bars;
            // a notification box is <= 70 % wide, the interface layer is 81 % high. This is the page signal (the text-union rules, active since 0.8.19,
            // were 'on' in almost every frame -> the hand window (held creature) was cut / erased).
            // 0.8.27: measured on report 175651: the dark page background spans y 72..583 of 768 (.094-.759): its bottom lies BEHIND the statistics bar. 0.8.26 required <= .73 -> page never seen.
            if(middleDraw&&x1-x0>=.90f*uiW&&y1-y0>=.45f*ihb&&y1-y0<=.72f*ihb&&tyb>=.085f&&tyb<=.13f&&byb<=.80f) {if(!pageSeen&&pageHold<=0) log("Objectives page detected (background %.0fx%.0f px at %.0f,%.0f)",x1-x0,y1-y0,x0,y0);pageSeen=true;}
            // 0.8.94.8: the objectives / information page ('0' key, 'i' button) is ONE flat draw covering most of the band. Nothing else in the game is
            // both that wide and that tall: a notification box is <= 70 % wide (bigBoxRaw), the held creature was 271 px, the false grid of report
            // 20261004_165153 was 512 px wide strips. This is the page signal the panel uses, next to the '0' key.
            // 0.8.94.10: the '0' page has no single background quad: the game paints it as a few BIG flat strips (report 20261004_165153 logged
            // them as 512x32 tiles covering 0,72-1024,552 - that WAS the page, mistaken for a false positive at the time). Only pieces at least a
            // quarter of the width join this union, so the held creature (271 px) and the square dungeon pieces cannot build it.
            if(middleDraw&&x1-x0>=.25f*uiW&&y1-y0<=.15f*ih) {pgX0=std::min(pgX0,x0);pgY0=std::min(pgY0,y0);pgX1=std::max(pgX1,x1);pgY1=std::max(pgY1,y1);++pgPieces;}   // 0.8.94.21: thin strips only (a 1024 x 735 full-screen layer at load counted as the page)
            // 0.8.94.19: report 20261005_100309 capture 3: in a NEW game the objectives page has few lines (strips 526 x 241): never 85 % wide. Its first
            // line always starts at the left edge just under the top bar ([0,72-512,104] in every report); a notification box starts at >= 10 %.
            if(middleDraw&&z1-z0<=1e-4f&&x0<=.02f*uiW&&x1-x0>=.40f*uiW&&(y0-oy)/ih>=.085f&&(y0-oy)/ih<=.12f) pgTopLeft=true;
            // 0.8.94.8: a real notification box is never glued to the left edge. Report 20261004_202042: 256x64 panels at x=8 passed for boxes on
            // every frame and kept the middle panel open until the failsafe cut it.
            if(bigBox&&x0>=.10f*uiW&&x1-x0>=.18f*uiW) {boxX0=std::min(boxX0,x0);boxY0=std::min(boxY0,y0);boxX1=std::max(boxX1,x1);boxY1=std::max(boxY1,y1);}
            // 0.8.20: union of ALL flat draws between the bars except small ones at the cursor (tooltips): with the laser on the TABLE the game cursor
            // lies in the middle of the screen and the text lines of the objectives page (0 button) were 'near the cursor' -> panel cut to the top lines.
            // 0.8.94.6: a line of the objectives page is always much WIDER than tall. The held creature's sprite (271 x 731 px in report 20261004_160911)
            // and the square dungeon pieces are not: they used to join this union on every frame and kept the objectives rule true for ever.
            const bool textLike=!objTextNeedsPage||(x1-x0)>=1.6f*(y1-y0);
            if(middleDraw&&textLike&&x1-x0<=.80f*uiW&&!(nearCursor&&x1-x0<.14f*uiW)) {allX0=std::min(allX0,x0);allY0=std::min(allY0,y0);allX1=std::max(allX1,x1);allY1=std::max(allY1,y1);
                if(allPieces.size()<10) allPieces.push_back({x0,y0,x1,y1});}   // 0.8.92 diagnostic
            if(middleDraw&&(!nearCursor||(!lastPointerOnTable&&cy<.60f)||(msgShown&&!lastPointerOnTable))) { // 0.8.19: laser on a bar (the '0' button): the page's text lines lie 'near the cursor' -> panel cut at the right
                msgX0=std::min(msgX0,x0);msgY0=std::min(msgY0,y0);msgX1=std::max(msgX1,x1);msgY1=std::max(msgY1,y1);
            }
            // Text / buttons of last frame's box (not the held things at the cursor while the laser is on the table).
            // 0.6.9.13: the held creature (pieces with varying depth near the cursor) is drawn after the interface: what is
            // drawn inside the box after it (count of things held) belongs to the hand, the rest (text, X/check) to the box.
            if(nearCursor&&z1-z0>1e-4f) {heldModelSeen=true; // 0.7.0: bounding box of the held model (kept when the bars are erased)
                heldKeep[0]=std::min(heldKeep[0],x0);heldKeep[1]=std::min(heldKeep[1],y0);heldKeep[2]=std::max(heldKeep[2],x1);heldKeep[3]=std::max(heldKeep[3],y1);
                // 0.8.94.32: box of the held CREATURE only, for the hand window. heldKeep also gathers the 3D portraits / buttons of the bottom bars when the
                // cursor is near them (report 20261005_165049: box -300..340 x -12..448 = whole window -> the tooltip rectangle always lay 'inside the
                // creature' and never moved). Here: 3D pieces hanging at the hand only (centre -1.2R..+1.2R, -0.8R..+2.2R), above the bars band (.73).
                const float pcy=(cyy-oy)/ih;
                if(cx>=cursorPx-1.2f*R&&cx<=cursorPx+1.2f*R&&cyy>=cursorPy-.8f*R&&cyy<=cursorPy+2.2f*R&&pcy<=.73f&&x1-x0<=2.4f*R&&y1-y0<=3.f*R) {
                    ++statCreatureKeep;creatureKeep[0]=std::min(creatureKeep[0],x0);creatureKeep[1]=std::min(creatureKeep[1],y0);creatureKeep[2]=std::max(creatureKeep[2],x1);creatureKeep[3]=std::max(creatureKeep[3],y1);}}
            const bool inBox=inGameBox||(!TextBoxWatch::installed&&(boxTail||(cx>=prevBox[0]&&cx<=prevBox[2]&&cyy>=prevBox[1]&&cyy<=prevBox[3]))&&(msgShown||!(heldModelSeen&&nearCursor))); // 0.8.16: while a message is shown its text / X / check stay in the box (laser on the table hid them)
            // 0.6.9.11: message box pieces only go to the message panel, never to the interface texture (the hand's
            // window is cut from it: erasing the box there also erased the held creature drawn over it).
            boxOnly=!panelFromPixels&&PauseWatch::paused!=1&&(bigBox||inBox);   // 0.8.94.22: with the panel found from the pixels, the interface layer keeps every piece
            // 0.8.28 general pass (HandTakesAllNearby=1): with the laser on the table and no page / options screen open, EVERY flat piece the game draws between the bars
            // (tooltips, counters, icons of any kind) stays in the interface texture -> hand window; only real notification boxes (bigBox / inBox) go to the wall.
            // Before: only pieces 'near the cursor' stayed, the others (icons drawn further away) were projected on the wall panel.
            const bool handShare=handAll&&pageHold<=0&&!pageSeen&&!screenWhole;
            middleToScreen=middleDraw&&(pageShown||bigBox||inBox||!((lastPointerOnTable&&PauseWatch::paused!=1&&(nearCursor||handShare))||(heldOff47&&PauseWatch::paused!=1&&nearCursor)));   // 0.9.47: held creature with the laser on the interface: its pieces near the cursor stay in the hand window   // 0.8.94.10: while the '0' page is open every middle draw belongs to the panel (its text was landing on the dungeon)
            // 0.8.94.20: while a box / the page is shown in the whole band, every FLAT piece between the top bar (.09) and the bottom panel (.765) belongs to
            // that panel, statistics bar and X / check included. Only what sits at the cursor while the laser is on the table (held creature, tooltip) stays out.
            if(panelWhole&&PauseWatch::paused!=1&&z1-z0<=1e-4f&&cy>.09f&&(y1-oy)/ih<=.765f&&!(lastPointerOnTable&&nearCursor&&!bigBox&&!inBox)) {middleToScreen=true;++statWholePieces;}
            if(middleDraw&&(screenActive?!nearSprite:!nearCursor)) {midX0=std::min(midX0,x0);midY0=std::min(midY0,y0);midX1=std::max(midX1,x1);midY1=std::max(midY1,y1);}
        }
        if(cursorKnown&&count>0&&count<=64) {
            const auto* in=static_cast<const unsigned char*>(verts);float x0=1e9f,y0=1e9f,x1=-1e9f,y1=-1e9f;
            for(UINT i=0;i<count;++i) {float v[2];memcpy(v,in+i*stride,8);x0=std::min(x0,v[0]);y0=std::min(y0,v[1]);x1=std::max(x1,v[0]);y1=std::max(y1,v[1]);}
            if(cursorPx>=x0-6&&cursorPx<=x1+6&&cursorPy>=y0-6&&cursorPy<=y1+6) {probeCursorInfo[0]=count|(indices?0x10000u:0u);probeCursorInfo[1]=fvf;probeCursorInfo[2]=DWORD(x1-x0)|(DWORD(y1-y0)<<16);probeCursorInfo[3]=tex0?1:0;}
        }
        if(cursorFromUi&&cursorKnown&&tex0&&count>=3&&count<=6&&(fvf&D3DFVF_TEXCOUNT_MASK)) {
            const UINT uvOffset=16+((fvf&D3DFVF_DIFFUSE)?4:0)+((fvf&D3DFVF_SPECULAR)?4:0);
            if(uvOffset+8<=stride) {
                const auto* in=static_cast<const unsigned char*>(verts);
                float x0=1e9f,y0=1e9f,x1=-1e9f,y1=-1e9f;float bestA=1e9f,bestB=-1e9f;
                for(UINT i=0;i<count;++i) {
                    float v[2],t[2];memcpy(v,in+i*stride,8);memcpy(t,in+i*stride+uvOffset,8);
                    x0=std::min(x0,v[0]);y0=std::min(y0,v[1]);x1=std::max(x1,v[0]);y1=std::max(y1,v[1]);
                    if(v[0]+v[1]<bestA) {bestA=v[0]+v[1];uvA[0]=t[0];uvA[1]=t[1];}
                    if(v[0]+v[1]>bestB) {bestB=v[0]+v[1];uvB[0]=t[0];uvB[1]=t[1];}
                }
                const float w=x1-x0,h=y1-y0,m=6;
                if(w>=8&&h>=8&&w<=std::min(100.f,uiW*.10f)&&h<=std::min(100.f,uiH*.14f)&&cursorPx>=x0-m&&cursorPx<=x1+m&&cursorPy>=y0-m&&cursorPy<=y1+m) {
                    candidate=true;
                    box={LONG(std::floor(std::max(0.f,x0))),LONG(std::floor(std::max(0.f,y0))),LONG(std::ceil(std::min(float(uiW),x1))),LONG(std::ceil(std::min(float(uiH),y1)))};
                    if(box.right<=box.left||box.bottom<=box.top) candidate=false;
                    else {
                        CopyCache::Guard guard;guard.d=d;guard.cache=&copies;
                        uint32_t id=copies.copyFor(d,guard,tex0.Get());
                        if(!id) candidate=false;
                        else {
                            cursorFound=true;cursorCopy=id;cursorUv[0]=uvA[0];cursorUv[1]=uvA[1];cursorUv[2]=uvB[0];cursorUv[3]=uvB[1];
                            cursorW=w;cursorH=h;cursorHotX=(cursorPx-x0)/w;cursorHotY=(cursorPy-y0)/h;cursorRect=box;
                        }
                    }
                }
            }
        }
        ComPtr<IDirect3DSurface9> old;d->GetRenderTarget(0,&old);
        D3DVIEWPORT9 vp{};d->GetViewport(&vp);
        // 0.6.9.14: the interface has its OWN depth buffer, cleared every frame. With the game's depth buffer the
        // dungeon depth hid the interface pieces the game places in depth ('0' button, portrait behind the counters:
        // visible only when hovered = brought nearer). The pieces still sort among themselves (0.6.9.9 lesson).
        if(!uiDepth&&!uiDepthFailed&&FAILED(d->CreateDepthStencilSurface(uiW,uiH,D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,FALSE,&uiDepth,nullptr))) {uiDepthFailed=true;log("HUD: own depth buffer failed, game depth used");}
        ComPtr<IDirect3DSurface9> oldDepth;d->GetDepthStencilSurface(&oldDepth);
        struct DepthBack {IDirect3DDevice9* d;IDirect3DSurface9* s;~DepthBack(){d->SetDepthStencilSurface(s);}} depthBack{d,oldDepth.Get()};
        if(FAILED(d->SetRenderTarget(0,uiSurf.Get()))) return;
        if(uiDepth) d->SetDepthStencilSurface(uiDepth.Get());
        if(!uiCleared) {
            DWORD scissor=0;d->GetRenderState(D3DRS_SCISSORTESTENABLE,&scissor);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);
            D3DVIEWPORT9 full{0,0,uiW,uiH,0,1};d->SetViewport(&full);
            d->Clear(0,nullptr,D3DCLEAR_TARGET|(uiDepth?D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL:0),0x00000000,1,0);
            d->SetRenderState(D3DRS_SCISSORTESTENABLE,scissor);uiCleared=true;cursorBacked=false;
            // 0.6.5: things held by the Keeper's hand (creature, gold, spell icons) are drawn by the game as 2D
            // pieces next to the cursor while CDefaultPlayerInterface (0x6CEA10) holds things (+0xEF8 count).
            LensHack::applyMeshDetail(native.base); // 0.8.28
            int held=0;memcpy(&held,reinterpret_cast<void*>(native.base+0x6CF908-0x400000),4);
            holdingThings=keeperHeld&&held>0&&held<=64&&cursorKnown;
            ++statUiFrames;if(held>0&&held<=64) ++statHoldCount;
        }
        // Held things: pieces whose centre is within HeldRadius of the cursor go to their own target.
        // 0.6.5.1: the creature in the hand is a 3D model the game projects itself onto the screen: its 2D pieces
        // carry varying depths (z), unlike every panel piece (flat z). Those pieces, and the small icons drawn
        // right after them next to the model (count of things held), go to the hand. No position guess.
        bool toHeld=false;
        if(keeperHeld&&!heldWindow) {
            const auto* in=static_cast<const unsigned char*>(verts);float x0=1e9f,y0=1e9f,x1=-1e9f,y1=-1e9f,z0=1e9f,z1=-1e9f;
            for(UINT i=0;i<count;++i) {float v[3];memcpy(v,in+i*stride,12);x0=std::min(x0,v[0]);y0=std::min(y0,v[1]);x1=std::max(x1,v[0]);y1=std::max(y1,v[1]);z0=std::min(z0,v[2]);z1=std::max(z1,v[2]);}
            const float k=uiW/1024.f;
            // 0.6.5.2: a piece goes to the hand only when it lies near the cursor and not inside a HUD panel
            // that the cursor is not over (panel portraits / 3D panel items also carry varying depths).
            const float cx=(x0+x1)*.5f,cy=(y0+y1)*.5f,nearPx=heldRadius*k;
            bool panelOk=true;
            for(int r=0;r<HudCount&&panelOk;++r) {
                float a0,b0,a1,b1;hudPixels(r,float(uiW),float(uiH),a0,b0,a1,b1);
                const bool pieceIn=cx>=a0&&cx<=a1&&cy>=b0&&cy<=b1;
                const bool cursorIn=cursorKnown&&cursorPx>=a0&&cursorPx<=a1&&cursorPy>=b0&&cursorPy<=b1;
                if(pieceIn&&!cursorIn) panelOk=false;
            }
            const bool nearCursor=cursorKnown&&x1>cursorPx-nearPx&&x0<cursorPx+nearPx&&y1>cursorPy-nearPx&&y0<cursorPy+nearPx;
            const bool model=z1-z0>1e-4f&&x1-x0<400*k&&y1-y0<400*k;
            if(model&&!(nearCursor&&panelOk)) {if(panelOk) ++statHeldFar; else ++statHeldPanel;}
            if(model&&nearCursor&&panelOk) {
                toHeld=true;
                if(!heldModel) {heldBox[0]=x0;heldBox[1]=y0;heldBox[2]=x1;heldBox[3]=y1;heldModel=true;}
                else {heldBox[0]=std::min(heldBox[0],x0);heldBox[1]=std::min(heldBox[1],y0);heldBox[2]=std::max(heldBox[2],x1);heldBox[3]=std::max(heldBox[3],y1);}
            } else if(heldModel&&x1-x0<=48*k&&y1-y0<=48*k) {
                const float m=heldRadius*k;
                toHeld=panelOk&&x0>heldBox[0]-m&&x1<heldBox[2]+m&&y0>heldBox[1]-m&&y1<heldBox[3]+m;
            }
        }
        if(toHeld) {
            if(!heldSurf) {
                if(FAILED(d->CreateTexture(uiW,uiH,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&heldTex,nullptr))||FAILED(heldTex->GetSurfaceLevel(0,&heldSurf))) {heldTex.Reset();heldSurf.Reset();keeperHeld=false;toHeld=false;}
            }
            if(toHeld&&SUCCEEDED(d->SetRenderTarget(0,heldSurf.Get()))) {
                if(!heldCleared) {
                    DWORD scissor=0;d->GetRenderState(D3DRS_SCISSORTESTENABLE,&scissor);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);
                    D3DVIEWPORT9 full{0,0,uiW,uiH,0,1};d->SetViewport(&full);d->Clear(0,nullptr,D3DCLEAR_TARGET,0,1,0);
                    d->SetRenderState(D3DRS_SCISSORTESTENABLE,scissor);heldCleared=true;heldPx=cursorKnown?cursorPx:(heldBox[0]+heldBox[2])*.5f;heldPy=cursorKnown?cursorPy:(heldBox[1]+heldBox[3])*.5f;
                }
                candidate=false;++statHeldDraws;uiWentToHeld=true;
            } else if(toHeld) {d->SetRenderTarget(0,uiSurf.Get());}
        }
        d->SetViewport(&vp);
        if(candidate) {
            // Keep what lies under the cursor so that it can be removed from the panels at the end of the frame.
            RECT dst{0,0,std::min<LONG>(512,box.right-box.left),std::min<LONG>(512,box.bottom-box.top)};
            RECT src{box.left,box.top,box.left+dst.right,box.top+dst.bottom};
            cursorBacked=SUCCEEDED(d->StretchRect(uiSurf.Get(),&src,cursorBackup.Get(),&dst,D3DTEXF_NONE));cursorSaved=src;
        }
        const D3DRENDERSTATETYPE keep[]={D3DRS_ALPHABLENDENABLE,D3DRS_SRCBLEND,D3DRS_DESTBLEND,D3DRS_SEPARATEALPHABLENDENABLE,D3DRS_SRCBLENDALPHA,D3DRS_DESTBLENDALPHA,D3DRS_BLENDOPALPHA,D3DRS_BLENDFACTOR,D3DRS_COLORWRITEENABLE,D3DRS_ZENABLE,D3DRS_ZWRITEENABLE};
        constexpr int K=sizeof(keep)/sizeof(keep[0]);
        DWORD saved[K]{};for(int i=0;i<K;++i) d->GetRenderState(keep[i],&saved[i]);
        d->SetRenderState(D3DRS_COLORWRITEENABLE,15);
        // (0.6.9.9 turned the depth test off here: REGRESSION, the game orders its panel pieces by depth -> portraits
        // hidden by the stone background. Reverted in 0.6.9.10: the game's own depth states are kept.)
        d->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,TRUE);d->SetRenderState(D3DRS_BLENDOPALPHA,D3DBLENDOP_ADD);
        if(!saved[0]) {
            // 0.6.1.1: opaque interface draws are fully covering whatever alpha their texture/vertex carries
            // (0.6.1: alpha 0 from the vertex colour made the whole interface invisible).
            d->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
            d->SetRenderState(D3DRS_BLENDFACTOR,0xffffffff);d->SetRenderState(D3DRS_SRCBLENDALPHA,D3DBLEND_BLENDFACTOR);d->SetRenderState(D3DRS_DESTBLENDALPHA,D3DBLEND_ZERO);
        } else {
            // Blended pieces: coverage = source alpha; additive glows (dest ONE) keep the covering alpha.
            DWORD sb=saved[1];
            d->SetRenderState(D3DRS_SRCBLENDALPHA,(sb==D3DBLEND_SRCALPHA)?D3DBLEND_ONE:D3DBLEND_ZERO);
            d->SetRenderState(D3DRS_DESTBLENDALPHA,(sb==D3DBLEND_SRCALPHA)?D3DBLEND_INVSRCALPHA:D3DBLEND_ONE);
        }
        HRESULT hr=S_OK;
        if(!boxOnly) {hr=indices?d->DrawIndexedPrimitiveUP(type,0,count,primitives,indices,indexFormat,verts,stride):d->DrawPrimitiveUP(type,primitives,verts,stride);
        if(SUCCEEDED(hr)) ++statUiDraws;}
        // 0.6.9: the wall screen only receives what is drawn between the bars (options screen, menus), never
        // the bars themselves ('Continue game' overlaps the statistics bar: a region cut showed part of it).
        if(middleToScreen) { // 0.6.9.8: held things / tooltips at the cursor stay out of the message panel
            if(!screenSurf&&!screenFailed) {
                if(FAILED(d->CreateTexture(uiW,uiH,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&screenTex,nullptr))||FAILED(screenTex->GetSurfaceLevel(0,&screenSurf))) {screenTex.Reset();screenSurf.Reset();screenFailed=true;}
            }
            if(screenSurf&&SUCCEEDED(d->SetRenderTarget(0,screenSurf.Get()))) {
                if(!screenCleared) {DWORD sc=0;d->GetRenderState(D3DRS_SCISSORTESTENABLE,&sc);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);
                    D3DVIEWPORT9 full{0,0,uiW,uiH,0,1};d->SetViewport(&full);d->Clear(0,nullptr,D3DCLEAR_TARGET,0,1,0);d->SetRenderState(D3DRS_SCISSORTESTENABLE,sc);screenCleared=true;}
                d->SetViewport(&vp);
                if(indices) d->DrawIndexedPrimitiveUP(type,0,count,primitives,indices,indexFormat,verts,stride); else d->DrawPrimitiveUP(type,primitives,verts,stride);
            }
        }
        for(int i=0;i<K;++i) d->SetRenderState(keep[i],saved[i]);
        if(firstUiLogs<12) {++firstUiLogs;log("HUD draw: blend=%lu src=%lu dst=%lu fvf=%08lX count=%u indexed=%u hr=%08lX",saved[0],saved[1],saved[2],fvf,count,indices!=nullptr,(unsigned long)hr);}
        d->SetRenderTarget(0,old.Get());d->SetViewport(&vp);
    }
    // Laser on the HUD modules (game thread): same layout as the VR thread, drives the Windows/game cursor.
    Request pointerRequest() {
        Request r=request;
        const uint32_t raw=r.pointerFlags;
        const bool trig=raw&PointerTriggerRaw,second=raw&PointerSecondRaw;
        hudTarget=-1;buttonHover=false;
        if(!(raw&PointerActive)&&(raw&PointerAimValid)&&lastHudW>=64&&(r.handFlags&RequestHudShown)&&!native.frontend()) {
            const float depthRatio=tableAspect>0?tableAspect:(table.halfWidth>0?table.halfDepth/table.halfWidth:.75f);
            HudLayout L=layoutHud(r,hudSizes,depthRatio,lastHudTop,float(lastHudW),float(lastHudH),screenActive,pedestalSide,screenWhole||panelWhole,&msgRegion,statsShownNow());   // 0.8.94.3: back to the 0.8.94 layout (one whole band only for the game's own menu)
            float t=0,fx=0,fy=0;hudTarget=hitHud(L,r.rayOrigin,r.rayDirection,float(lastHudW),float(lastHudH),t,fx,fy);
            if(hudTarget>=0&&lastPickValid&&lastPickDist>0&&lastPickDist<t) hudTarget=-1; // 0.6.7: the dungeon is nearer than the panel
            // 0.6.8: pedestal button (nearest hit wins); the trigger toggles the side, the game gets no click.
            {float tb=0;const float limit=hudTarget>=0?t:(lastPickValid&&lastPickDist>0?lastPickDist:4.f);
             buttonHover=hitButton(L,r.rayOrigin,r.rayDirection,tb,limit);
             if(buttonHover) {
                 hudTarget=-1;
                 if(trig&&!hudTrigBefore) {
                     const bool nowRight=!L.arrowRight;pedestalSide=nowRight?1:2;setUserInt(L"MinimapPedestalSide",pedestalSide);++statButtonClicks;
                     log("Minimap pedestal moved to the %s of the table",pedestalSide==2?"right":"left");
                 }
             }}
            // 0.6.2: never closer than 2 % to the game area edges (the game scrolls the map there).
            // 0.6.9.11: the panel fold button (arrows between the tabs and the 'o' button, measured on the VR capture:
            // interface x .365-.415, y .760-.835) folds the bottom panel: useless in VR and broken once folded -> no clicks.
            foldBlocked=(hudTarget==HudBottom||hudTarget==HudStats)&&fx>=.390f&&fx<=.436f&&fy>=.755f&&fy<=.832f; // 0.6.9.14: measured on the interface texture (arrows .397-.429 x .766-.824)
            if(hudTarget<0) foldBlocked=false;
            // 0.8.91: the minimap magnifier (bottom-right corner button of the minimap module) zooms the minimap, after which its white view frame
            // no longer follows the VR view: no click there (MinimapZoomButton=1 restores it).
            if(hudTarget==HudMinimap&&!minimapZoomButton) {const HudPanel& q=L.panel[HudMinimap];
                const float u=(fx*float(lastHudW)-q.x0)/std::max(1.f,q.x1-q.x0),v=(fy*float(lastHudH)-q.y0)/std::max(1.f,q.y1-q.y0);
                if(u>=.72f&&v>=.70f) {foldBlocked=true;if(trig&&!hudTrigBefore) {++statZoomBlocked;if(statZoomBlocked<=5) log("Minimap magnifier click ignored (MinimapZoomButton=0)");}}}
            if(hudTarget>=0) {fx=std::clamp(fx,.02f,.98f);fy=std::clamp(fy,.02f,.98f);}
            if(hudTarget>=0&&backW>0&&backH>0) {
                // Game render target fraction -> back buffer (client) fraction, letterboxed like NativeMouse.
                const float k=std::min(float(backW)/lastHudW,float(backH)/lastHudH);
                fx=((backW-lastHudW*k)*.5f+fx*lastHudW*k)/backW;fy=((backH-lastHudH*k)*.5f+fy*lastHudH*k)/backH;
            }
            if(hudTarget>=0) {r.pointerX=fx;r.pointerY=fy;r.pointerFlags|=PointerActive;++statHudHits;lastPointerX=fx;lastPointerY=fy;}
        }
        // 0.6.6: laser on the table -> game cursor. Ray (LOCAL) meets the table surface (the walking floor the
        // dungeon is fitted to), the point is taken back into the native camera and projected with the game's
        // own lens: exactly the inverse of the vertex reconstruction, so the cursor lands on what is drawn there.
        bool tableHit=false;
        if(buttonHover&&!(raw&PointerActive)) {} // pointing at the pedestal button: no game cursor move
        else if(tablePointer&&hudTarget<0&&!(raw&PointerActive)&&(raw&PointerAimValid)&&lastPickValid&&!possessedFrame&&!native.frontend()) {
            float fx=std::clamp(lastPickFx,.02f,.98f),fy=std::clamp(lastPickFy,.02f,.98f);
            if(backW>0&&backH>0) {const float k=std::min(float(backW)/lastHudW,float(backH)/lastHudH);
                fx=((backW-lastHudW*k)*.5f+fx*lastHudW*k)/backW;fy=((backH-lastHudH*k)*.5f+fy*lastHudH*k)/backH;}
            r.pointerX=fx;r.pointerY=fy;r.pointerFlags|=PointerActive;tableHit=true;++statTableHits;++statPickHits;lastTableX=fx;lastTableY=fy;
        }
        else if(tablePointer&&hudTarget<0&&!(raw&PointerActive)&&(raw&PointerAimValid)&&anchored&&!possessedFrame&&table.valid&&pointerLensOk&&lastHudW>=64&&!native.frontend()) {
            const Vec3 n=rotate(tableCamera.orientation,table.up),c=toWorld(tableCamera,table.center);
            const float den=dot(n,r.rayDirection);
            if(std::fabs(den)>1e-4f) {
                const float t=dot(n,sub(c,r.rayOrigin))/den;
                if(t>.02f&&t<6.f) {
                    const Vec3 pc=toLocal(tableCamera,add(r.rayOrigin,scale(r.rayDirection,t))),rel=sub(pc,table.center);
                    if(std::fabs(dot(rel,table.right))<=table.halfWidth&&std::fabs(dot(rel,table.forward))<=table.halfDepth&&pc.z<-1e-4f) {
                        const float depth=-pc.z;
                        float fx=(pointerLens.centerX+pc.x*pointerLens.focalX/depth)/lastHudW,fy=(pointerLens.centerY-pc.y*pointerLens.focalY/depth)/lastHudH;
                        if(std::isfinite(fx)&&std::isfinite(fy)) {
                            fx=std::clamp(fx,.02f,.98f);fy=std::clamp(fy,.02f,.98f);
                            if(backW>0&&backH>0) {const float k=std::min(float(backW)/lastHudW,float(backH)/lastHudH);
                                fx=((backW-lastHudW*k)*.5f+fx*lastHudW*k)/backW;fy=((backH-lastHudH*k)*.5f+fy*lastHudH*k)/backH;}
                            r.pointerX=fx;r.pointerY=fy;r.pointerFlags|=PointerActive;tableHit=true;++statTableHits;++statPlaneHits;lastTableX=fx;lastTableY=fy;
                        }
                    }
                }
            }
        }
        lastPointerOnTable=tableHit;
        const bool on=(hudTarget>=0&&!foldBlocked)||tableHit;
        if(!(raw&PointerActive)) {
            hudLatchL=on&&trig&&(hudLatchL||!hudTrigBefore);hudLatchR=on&&second&&(hudLatchR||!hudSecondBefore);
            if(hudLatchL&&!hudTrigBefore) {if(tableHit) ++statTableClicks; else ++statHudClicks;}
            if(hudLatchL) r.pointerFlags|=PointerLeft;
            if(hudLatchR) r.pointerFlags|=PointerRight;
        } else hudLatchL=hudLatchR=false;
        hudTrigBefore=trig;hudSecondBefore=second;
        return r;
    }
    // 0.6.7: the laser meets the dungeon geometry itself (not a flat plane): the nearest reconstructed triangle
    // hit inside the table gives the point the game must see under its cursor (rock tops, walls, creatures).
    void pickTri(const Vec3& a,const Vec3& b,const Vec3& c) {
        const Vec3 e1=sub(b,a),e2=sub(c,a);
        const Vec3 pv{pickD.y*e2.z-pickD.z*e2.y,pickD.z*e2.x-pickD.x*e2.z,pickD.x*e2.y-pickD.y*e2.x};
        const float det=dot(e1,pv);if(std::fabs(det)<1e-9f) return;
        const float inv=1/det;const Vec3 tv=sub(pickO,a);
        const float u=dot(tv,pv)*inv;if(u<0||u>1) return;
        const Vec3 qv{tv.y*e1.z-tv.z*e1.y,tv.z*e1.x-tv.x*e1.z,tv.x*e1.y-tv.y*e1.x};
        const float v=dot(pickD,qv)*inv;if(v<0||u+v>1) return;
        const float t=dot(e2,qv)*inv;if(t<=.02f||t>=pickT) return;
        const Vec3 pc=toLocal(tableCamera,add(pickO,scale(pickD,t))),rel=sub(pc,table.center);
        if(!table.valid||std::fabs(dot(rel,table.right))>table.halfWidth||std::fabs(dot(rel,table.forward))>table.halfDepth) return;
        pickT=t;pickFound=true;
    }
    void pickDraw(D3DPRIMITIVETYPE type,UINT primitives,UINT count,const void* indices,D3DFORMAT indexFormat) {
        auto idx=[&](UINT k)->UINT{if(!indices) return k;return indexFormat==D3DFMT_INDEX32?static_cast<const uint32_t*>(indices)[k]:static_cast<const uint16_t*>(indices)[k];};
        auto tri=[&](UINT i0,UINT i1,UINT i2){i0=idx(i0);i1=idx(i1);i2=idx(i2);if(i0<count&&i1<count&&i2<count) pickTri(positions[i0],positions[i1],positions[i2]);};
        if(type==D3DPT_TRIANGLELIST) for(UINT i=0;i<primitives;++i) tri(i*3,i*3+1,i*3+2);
        else if(type==D3DPT_TRIANGLESTRIP) for(UINT i=0;i<primitives;++i) tri(i,i+1,i+2);
        else if(type==D3DPT_TRIANGLEFAN) for(UINT i=0;i<primitives;++i) tri(0,i+1,i+2);
    }
    // End of the game frame: the hit becomes a cursor position (game lens, exact inverse of unproject).
    void finishPick() {
        lastPickValid=false;lastPickDist=0;
        if(!pickActive||!pickFound||!anchored||!pointerLensOk||lastHudW<64) return;
        const Vec3 pc=toLocal(tableCamera,add(pickO,scale(pickD,pickT)));
        if(pc.z>-1e-4f) return;
        const float depth=-pc.z;
        lastPickFx=(pointerLens.centerX+pc.x*pointerLens.focalX/depth)/lastHudW;lastPickFy=(pointerLens.centerY-pc.y*pointerLens.focalY/depth)/lastHudH;
        lastPickValid=std::isfinite(lastPickFx)&&std::isfinite(lastPickFy);
        lastPickDist=lastPickValid?pickT:0;
    }
    // 0.8.97 DIAGNOSTIC (read by Claude): every 2D piece (rhw 1) the game draws during a possession. Today none of them reaches the headset (the game
    // interface stays on the PC screen). The attack effect the user does not see may be one of them: pieces in the middle of the screen (away from the
    // interface bars) are counted apart and the first ones are described (size, place, texture, blending).
    uint64_t presentSeq=0,p2dFrames=0,p2dDraws=0,p2dCentral=0,p2dCentralFrames=0;uint64_t p2dLastFrame=~0ull,p2dLastCentralFrame=~0ull;int p2dLines=0;float p2dBox[4]={1e9f,1e9f,-1e9f,-1e9f};double p2dLog=0;
    // 0.9.47.36 PROBE (read by Claude): the possession interface (ability icons of the creature) is made of 2D pieces. Every DISTINCT piece (place + texture)
    // is described once in the log ("Possession UI piece") with its texture coordinates, and its texture is saved once as Captures\DK2VR_possui_NN.bmp, no key needed.
    std::set<uint64_t> possUiSeen;int possUiLogs=0,possUiDumps=0;
    void possessionUiProbe(IDirect3DDevice9* d,const unsigned char* input,UINT stride,UINT count,float x0,float y0,float x1,float y1,DWORD vx,DWORD vy,float W,float H,uint64_t frame) {
        if(possUiLogs>=160||stride<28||count==0) return;
        ComPtr<IDirect3DBaseTexture9> t;d->GetTexture(0,&t);
        const uint64_t key=(uint64_t(uintptr_t(t.Get()))*1315423911ull)^(uint64_t(int((x0-vx)/W*100))<<40)^(uint64_t(int((y0-vy)/H*100))<<28)^(uint64_t(int((x1-vx)/W*100))<<16)^(uint64_t(int((y1-vy)/H*100))<<4)^count;
        if(!possUiSeen.insert(key).second) return;
        float u0=1e9f,v0=1e9f,u1=-1e9f,v1=-1e9f;
        for(UINT i=0;i<count;++i) {float uv[2];memcpy(uv,input+size_t(i)*stride+20,8);u0=std::min(u0,uv[0]);v0=std::min(v0,uv[1]);u1=std::max(u1,uv[0]);v1=std::max(v1,uv[1]);}
        UINT tw=0,th=0;int dumped=-1;
        if(t&&t->GetType()==D3DRTYPE_TEXTURE) {
            auto* tex=static_cast<IDirect3DTexture9*>(t.Get());D3DSURFACE_DESC td{};
            if(SUCCEEDED(tex->GetLevelDesc(0,&td))) {tw=td.Width;th=td.Height;
                if(!kPublicBuild&&possUiDumps<14&&td.Width<=512&&td.Height<=512&&(td.Format==D3DFMT_A8R8G8B8||td.Format==D3DFMT_X8R8G8B8)) {
                    D3DLOCKED_RECT lr{};if(SUCCEEDED(tex->LockRect(0,&lr,nullptr,D3DLOCK_READONLY))) {
                        std::error_code ec;auto folder=configurationPath().parent_path()/L"Captures";std::filesystem::create_directories(folder,ec);
                        wchar_t n[64];swprintf(n,64,L"DK2VR_possui_%02d_%p.bmp",possUiDumps,(void*)tex);
                        writeBmp32(folder/n,td.Width,td.Height,static_cast<const uint32_t*>(lr.pBits),lr.Pitch/4);tex->UnlockRect(0);dumped=possUiDumps++;}}}
        }
        ++possUiLogs;
        log("Possession UI piece (0.9.47.36): frame %llu, %u vertices, box %.3f,%.3f-%.3f,%.3f of the screen, uv %.3f,%.3f-%.3f,%.3f, texture %p %ux%u, saved as possui_%d",
            (unsigned long long)frame,count,(x0-vx)/W,(y0-vy)/H,(x1-vx)/W,(y1-vy)/H,u0,v0,u1,v1,(void*)t.Get(),tw,th,dumped);
    }
    // 0.9.47.42: the skills the creature really HAS. Report 20261009_162218 (screen + user's annotation): the top bar of the possession screen holds the
    // creature portrait, its health bar, the gold icon and amount, THEN the skill icons: slots of 0.0525 of the game area starting at x = 0.551 (the first
    // skill at .551, the second at .604 ...; up to 6). Everything left of x = .54 is not a skill. The game draws one piece per available skill in its slot
    // (a frame / highlight piece of the chosen one lies in the same slot). A slot counts as available while it was seen in the last 150 game frames.
    constexpr static float SkillSlotX0=.551f,SkillSlotStride=.0525f,SkillSlotW=.0505f;constexpr static int SkillSlots=6;
    ComPtr<IDirect3DSurface9> skRt,skSys;
    uint64_t skillSeen[SkillSlots]{};int skillLatch=0;   // 0.9.47.43: skillLatch = number of slots known for this possession (highest slot seen + 1)
    void noteSkillIcon(const unsigned char* input,UINT stride,UINT count,float x0,float y0,float x1,float y1,DWORD vx,DWORD vy,float W,float H,uint64_t frame) {
        (void)input;
        if(count<4||count>16||stride<28) return;
        const float bx0=(x0-vx)/W,bx1=(x1-vx)/W,by0=(y0-vy)/H,by1=(y1-vy)/H;
        if(by0>.02f||by1-by0<.05f||by1-by0>.085f||bx1-bx0<.04f||bx1-bx0>.075f||bx0<.535f||bx0>.86f) return;
        const int slot=std::clamp(int(std::lround((bx0-SkillSlotX0)/SkillSlotStride)),0,SkillSlots-1);
        skillSeen[slot]=frame+1;   // 0.9.47.45: only recorded; the count comes from the image analysis   // 0.9.47.43: report 164930: the game redraws these pieces only when a skill changes, so a slot, once seen, stays for the whole possession
    }
    void possession2D(IDirect3DDevice9* d,const unsigned char* input,UINT stride,UINT count) {
        D3DVIEWPORT9 vp{};d->GetViewport(&vp);if(vp.Width<64||vp.Height<64) return;
        const uint64_t f=presentSeq;if(f!=p2dLastFrame) {p2dLastFrame=f;++p2dFrames;}
        ++p2dDraws;
        float x0=1e9f,y0=1e9f,x1=-1e9f,y1=-1e9f;
        for(UINT i=0;i<count;++i) {float v[2];memcpy(v,input+size_t(i)*stride,8);x0=std::min(x0,v[0]);y0=std::min(y0,v[1]);x1=std::max(x1,v[0]);y1=std::max(y1,v[1]);}
        const float W=float(vp.Width),H=float(vp.Height),cx=((x0+x1)*.5f-vp.X)/W,cy=((y0+y1)*.5f-vp.Y)/H;
        noteSkillIcon(input,stride,count,x0,y0,x1,y1,vp.X,vp.Y,W,H,f);   // 0.9.47.41
        possessionUiProbe(d,input,stride,count,x0,y0,x1,y1,vp.X,vp.Y,W,H,f);   // 0.9.47.36 probe: ability icons of the possession interface
        if(cx<.12f||cx>.88f||cy<.10f||cy>.66f) return;   // bars, minimap, panels of the possession interface
        ++p2dCentral;if(f!=p2dLastCentralFrame) {p2dLastCentralFrame=f;++p2dCentralFrames;}
        p2dBox[0]=std::min(p2dBox[0],(x0-vp.X)/W);p2dBox[1]=std::min(p2dBox[1],(y0-vp.Y)/H);p2dBox[2]=std::max(p2dBox[2],(x1-vp.X)/W);p2dBox[3]=std::max(p2dBox[3],(y1-vp.Y)/H);
        if(p2dLines<60) {++p2dLines;ComPtr<IDirect3DBaseTexture9> t;d->GetTexture(0,&t);UINT tw=0,th=0;
            if(t&&t->GetType()==D3DRTYPE_TEXTURE) {D3DSURFACE_DESC td{};if(SUCCEEDED(static_cast<IDirect3DTexture9*>(t.Get())->GetLevelDesc(0,&td))) {tw=td.Width;th=td.Height;}}
            DWORD ab=0,sb=0,db=0,zen=0;d->GetRenderState(D3DRS_ALPHABLENDENABLE,&ab);d->GetRenderState(D3DRS_SRCBLEND,&sb);d->GetRenderState(D3DRS_DESTBLEND,&db);d->GetRenderState(D3DRS_ZENABLE,&zen);
            log("Possession 2D piece (0.8.97 diagnostic): frame %llu, %u vertices, box %.3f,%.3f-%.3f,%.3f of the screen, texture %ux%u, blend %lu (%lu/%lu), z %lu",(unsigned long long)f,count,(x0-vp.X)/W,(y0-vp.Y)/H,(x1-vp.X)/W,(y1-vp.Y)/H,tw,th,(unsigned long)ab,(unsigned long)sb,(unsigned long)db,(unsigned long)zen);}
        (void)stride;
    }
    // 0.8.99: ATTACK EFFECTS of the possessed creature. Diagnostic of 0.8.97/0.8.98 (report 20261006_094957): when the creature attacks, the game draws
    // ADDITIVE 2D pieces (blend ONE/ONE, rhw 1, 128x128 textures) over the middle of the screen (x .30-.70, full height); the constant pieces of the
    // possession interface are alpha blended (SRCALPHA/INVSRCALPHA). Up to 0.8.98 every 2D piece stayed on the PC screen: the effects never reached the
    // headset. They are now recorded like the world, on a plane PossessionEffectsDistance metres in front of the game camera (same angular size as on the
    // screen), drawn over everything (the game draws them over its 3D too) and without depth writes.
    bool possessionEffects=true;float possessionEffectsDistance=1.f;uint64_t statEffects=0,statEffectFrames=0,effectLastFrame=~0ull,effectsTotal=0;
    static bool additiveBlend(IDirect3DDevice9* d) {
        DWORD ab=0,sb=0,db=0;d->GetRenderState(D3DRS_ALPHABLENDENABLE,&ab);if(!ab) return false;
        d->GetRenderState(D3DRS_SRCBLEND,&sb);d->GetRenderState(D3DRS_DESTBLEND,&db);
        return db==D3DBLEND_ONE&&(sb==D3DBLEND_ONE||sb==D3DBLEND_SRCALPHA);
    }
    void possession2DLog() {
        const double now=milliseconds();if(now-p2dLog<5000) return;
        if(p2dLog>0&&p2dFrames) log("Possession 2D (0.8.97 diagnostic, last 5 s): %llu frames with 2D, %.1f 2D pieces per frame, %llu pieces in the middle of the screen over %llu frames, their box %.3f,%.3f-%.3f,%.3f",
            (unsigned long long)p2dFrames,double(p2dDraws)/double(p2dFrames),(unsigned long long)p2dCentral,(unsigned long long)p2dCentralFrames,p2dBox[0],p2dBox[1],p2dBox[2],p2dBox[3]);
        if(p2dLog>0&&(statEffects||p2dFrames)) log("Possession attack effects (0.8.99, last 5 s): %s, %llu additive 2D pieces sent to the headset over %llu game frames (distance %.2f m, %llu since the start)",
            possessionEffects?"ON":"off (PossessionEffects=0)",(unsigned long long)statEffects,(unsigned long long)statEffectFrames,double(possessionEffectsDistance),(unsigned long long)effectsTotal);
        statEffects=statEffectFrames=0;
        p2dLog=now;p2dFrames=p2dDraws=p2dCentral=p2dCentralFrames=0;p2dBox[0]=p2dBox[1]=1e9f;p2dBox[2]=p2dBox[3]=-1e9f;
    }
    // 0.8.99.1 ADAPTIVE 360 (report 20261006_101213: the 6 cube faces cost the game 10-25 ms per frame, i.e. about half of every possession frame).
    // A face is run only when the headset view (both eyes, the head pose of the newest request, plus a margin for the head turning and for the creature
    // turning) reaches a direction of that face that the game's own normal view does not cover (it is wide: about 142 x 130 degrees, 45 degrees down).
    // A face stays asked for cubeHoldMs after it was last needed. The down face (a few draws) is always run: it keeps the cube data of every frame.
    // In the headset a face that was not run shows the normal view inside its pyramid (VR thread). Possession360Adaptive=0: the 6 faces as in 0.8.84.
    bool cubeAdaptive=true;float cubeMarginDeg=15.f,cubeHoldMs=300.f;double cubeNeedMs[6]{};double cubeYawMs=0;float cubeYawPrev=0,cubeYawRate=0;bool cubeYawHave=false;
    uint64_t statAdaptFrames=0,statAdaptAll=0;double statAdaptMargin=0;
    unsigned cubeWanted() {
        if(!cubeAdaptive) return 63u;
        const double now=milliseconds();
        if(possYawNowOk) {
            if(cubeYawHave&&now>cubeYawMs+1&&now-cubeYawMs<300) {const float rate=std::fabs(wrapRadians(possYawNow-cubeYawPrev))/float((now-cubeYawMs)*.001);if(std::isfinite(rate)) cubeYawRate=.6f*cubeYawRate+.4f*rate;}
            cubeYawPrev=possYawNow;cubeYawMs=now;cubeYawHave=true;
        }
        ++statAdaptFrames;
        if(!LensHack::cubeLastOk||!possessionAnchorValid||!request.valid||!pointerLensOk||!(pointerLens.focalX>1)||!(pointerLens.focalY>1)) {++statAdaptAll;return 63u;}
        const Quat cam=normalized(multiply(possessionAnchor.orientation,possessionLevel));
        const float tx=pointerLens.centerX/pointerLens.focalX,ty=pointerLens.centerY/pointerLens.focalY;
        const float marginDeg=cubeMarginDeg+std::min(30.f,cubeYawRate*57.29578f*.07f);statAdaptMargin+=marginDeg;
        const float margin=marginDeg*float(Pi)/180.f;
        unsigned need=0;
        for(int e=0;e<2;++e) {
            const Quat q=normalized(multiply(conjugate(cam),request.eyes[e].pose.orientation));const Fov& f=request.eyes[e].fov;
            need|=cubeFacesReached(LensHack::cubeLast.N,f.left,f.right,f.up,f.down,margin,
                [&](const float* in,float* o){const Vec3 v=rotate(q,Vec3{in[0],in[1],in[2]});o[0]=v.x;o[1]=v.y;o[2]=v.z;},
                [&](const float* d){return frontViewCovers(d,tx,ty,.92f);});
        }
        need|=32u;   // down face: cheap, the cube data of every frame
        unsigned want=0;
        for(int k=0;k<6;++k) {if(need&(1u<<k)) cubeNeedMs[k]=now;if(now-cubeNeedMs[k]<double(cubeHoldMs)) want|=1u<<k;}
        return want;
    }
    bool clipping() const {return table.valid&&!possessedFrame&&tableEnabled&&clipSupported;}
    void draw(IDirect3DDevice9* d,D3DPRIMITIVETYPE type,UINT primitives,const void* verts,UINT stride,const void* indices,D3DFORMAT indexFormat,UINT minIndex,UINT count) {
        // 0.6.9.16: this hook runs just BEFORE the game's own draw (WrapperBridge). The dungeon draws recorded for the
        // headset, and the things held by the Keeper, are made invisible in the game's image (colour writes off): the
        // image then holds only the interface over black, exactly composed by the game -> source of the VR bars.
        // COLORWRITEENABLE is a D3D9 state the DX7 game never sets; it is written for EVERY draw (on or off).
        // 0.7.2: a hidden dungeon draw writes no depth either (probe: its depth hid the '0' button and small panel pieces
        // drawn later in the game image). The game's ZWRITEENABLE is put back at the start of the next draw, unless the
        // game has set it itself in between.
        // 0.9.47.10 NativeWorldSkip: a HIDDEN draw (colour off, depth off) changes nothing in the game's image, so when the device-level draw hooks are
        // verified in place the wrapper's own call is dropped (TimedCalls::gate) and the colour / depth states are not touched at all. Anything
        // unexpected (hooks replaced, arguments not matching, draw never sent) falls back to the exact previous behaviour.
        // nativeMaskOpen: the colour mask is known to be 15 on the game's device (only this function ever writes 0), so the "back to 15" write that
        // used to precede and follow EVERY draw is skipped when it would change nothing.
        const nativeskip::Call nativeCall=[&]{nativeskip::Call c;c.kind=indices?1:0;c.type=uint32_t(type);c.primitives=primitives;c.stride=stride;c.verts=verts;
            if(indices) {c.indices=indices;c.indexFormat=uint32_t(indexFormat);c.minIndex=minIndex;c.numVertices=count;}return c;}();
        TimedCalls::gate.clear();
        struct NativeWrite {IDirect3DDevice9* d;const bool& hide;Renderer* r;const nativeskip::Call& call;
            ~NativeWrite(){if(!d) return;
                if(hide&&TimedCalls::skipReady(d)) {   // the wrapper's call is dropped by the device hook
                    TimedCalls::gate.arm(call);
                    if(TimedCalls::gate.lean()) return;   // verified long enough: nothing to hide, no state written
                }   // otherwise the draw is hidden the usual way as well (belt and braces while the drop is being verified)
                const bool open=!hide;
                if(open!=r->nativeMaskOpen) {d->SetRenderState(D3DRS_COLORWRITEENABLE,open?15u:0u);r->nativeMaskOpen=open;}
                if(hide) {DWORD z=TRUE;d->GetRenderState(D3DRS_ZWRITEENABLE,&z);if(z) {r->gameZWrite=z;r->zOverridden=true;d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);}}}};
        hideNative=false;uiWentToHeld=false;
        if(zOverridden) {zOverridden=false;DWORD z=TRUE;d->GetRenderState(D3DRS_ZWRITEENABLE,&z);if(!z) d->SetRenderState(D3DRS_ZWRITEENABLE,gameZWrite);}
        NativeWrite nativeWrite{hideNativeWorld&&!bypass?d:nullptr,hideNative,this,nativeCall};
        // 0.6.9.17: colour writes back ON before anything reads the device state: the draw is recorded for the headset
        // with the current states (0.6.9.16 left 0 from the previous dungeon draw -> the headset replayed an invisible
        // dungeon: empty table). Only the game's own draw, after this hook, sees 0.
        if(hideNativeWorld&&!bypass&&!nativeMaskOpen) {d->SetRenderState(D3DRS_COLORWRITEENABLE,15);nativeMaskOpen=true;}
        if(bypass||!verts||primitives==0||!start(d)) return;
        // Menus must remain readable even while F7 cycles through manual
        // tabletop/possession modes. Their animated background is a separate
        // native 3D scene, not the dungeon; cloning it hides the menu overlay.
        if(request.mode==3||native.frontend()) { if(menuDiag&&native.frontend()) menuCensus(d,type,primitives,verts,stride,indices,indexFormat,count); if(menuRelief&&native.frontend()&&decoupledFrame&&decoupledKind==3) recordMenuDraw(d,type,primitives,verts,stride,indices,indexFormat,count); ++ui;return; }
        if(!indices) {
            switch(type) {
            case D3DPT_TRIANGLELIST: count=primitives*3;break;
            case D3DPT_TRIANGLESTRIP:case D3DPT_TRIANGLEFAN:count=primitives+2;break;
            case D3DPT_LINELIST:count=primitives*2;break;
            case D3DPT_LINESTRIP:count=primitives+1;break;
            case D3DPT_POINTLIST:count=primitives;break;
            default:++rejected;return;
            }
        }
        DWORD fvf=0;d->GetFVF(&fvf);
        if((fvf&D3DFVF_POSITION_MASK)!=D3DFVF_XYZRHW||stride<16||stride>128||count>100000||minIndex!=0) {++rejected;return;}
        const auto* input=static_cast<const unsigned char*>(verts);
        // 0.7.1: probe of the 'o'/'i' button area (interface x .42-.53, y .70-.85): every draw touching it is logged
        // with the path it takes, one frame every ~5 s, 240 lines at most -> exact cause of the missing buttons.
        float pb[6]={1e9f,1e9f,-1e9f,-1e9f,1e9f,-1e9f};bool probeHit=false;
        if(probeLines<240&&probeFrame&&uiW>0&&uiH>0) {
            for(UINT i=0;i<count;++i) {float v[4];memcpy(v,input+i*stride,16);pb[0]=std::min(pb[0],v[0]);pb[1]=std::min(pb[1],v[1]);pb[2]=std::max(pb[2],v[0]);pb[3]=std::max(pb[3],v[1]);pb[4]=std::min(pb[4],v[3]);pb[5]=std::max(pb[5],v[3]);}
            const float fx0=pb[0]/uiW,fy0=pb[1]/uiH,fx1=pb[2]/uiW,fy1=pb[3]/uiH;
            probeHit=fx1>=.42f&&fx0<=.53f&&fy1>=.70f&&fy0<=.85f;
            if(probeHit) {pb[0]=fx0;pb[1]=fy0;pb[2]=fx1;pb[3]=fy1;}
        }
        auto probe=[&](const char* path){if(!probeHit) return;++probeLines;ComPtr<IDirect3DBaseTexture9> t;d->GetTexture(0,&t);
            log("Buttons probe: %s n=%u box %.3f,%.3f-%.3f,%.3f rhw %.3f-%.3f typical %.3f fits %d tex %p",path,count,pb[0],pb[1],pb[2],pb[3],pb[4],pb[5],worldRhw,int(pickFits),(void*)t.Get());};
        bool allFlat=true;
        for(UINT i=0;i<count;++i) {float v[4];memcpy(v,input+i*stride,16);if(std::fabs(v[3]-1.f)>1e-6f) {allFlat=false;break;}}
        if(allFlat&&SurroundHack::drawView!=0) {hideNative=hideNativeWorld;++statViewSkipped;return;}   // 0.8.84: a cube face never feeds the interface
        bool effectDraw=false;   // 0.8.99: attack effect of the possessed creature (additive 2D piece) recorded for the headset
        if(allFlat) {
            if(uiTexFilter) {ComPtr<IDirect3DBaseTexture9> t;d->GetTexture(0,&t);if(t&&uiTextures.size()<8192) uiTextures.insert(t.Get());}
            // (0.7.0 cleared the native depth here: 'i' vanished in that version -> removed in 0.7.1.)
            probe("2D interface");
            if(possessionWorld(request.mode,mode)) {
                possession2D(d,input,stride,count);   // 0.8.97 diagnostic: 2D drawn by the game in possession
                // 0.8.99: an additive piece = attack effect: recorded for the headset below (world path) instead of staying on the PC screen only
                if(possessionEffects&&additiveBlend(d)&&decoupledFrame&&decoupledKind==2) {effectDraw=true;++statEffects;++effectsTotal;if(presentSeq!=effectLastFrame) {effectLastFrame=presentSeq;++statEffectFrames;}}
            }
            if(!effectDraw) {++ui;if(hudWanted()) uiReplay(d,type,primitives,verts,stride,indices,indexFormat,count,fvf);hideNative=uiWentToHeld;return;} // Native 2D path writes RHW=1
        }   // 0.8.99: an attack effect goes on (in possession originDone is set: none of the interface filters below applies to it)
        // 0.8.0: the engine says where every 3D vertex comes from (EngineOrigin). Interface triangles go to the HUD and
        // stay in the game image; dungeon triangles go to the table and are hidden in the game image. The screen rules
        // of 0.6.9.3-0.7.10 below are no longer used (they sent dungeon pieces into the bars and the hand window).
        // 0.9.33: heat-haze / refraction quads: alpha-blended 3D draws textured with a RENDER TARGET (a copy of the game screen). Replayed for the headset they paste the
        // flat game-camera picture over the scene (rectangles with a wall copy over the forge, torch flame cut by a straight edge, report 215302).
        if(!allFlat&&hazeSkip) {
            DWORD hab=0;d->GetRenderState(D3DRS_ALPHABLENDENABLE,&hab);
            if(hab) {
                ComPtr<IDirect3DBaseTexture9> ht;d->GetTexture(0,&ht);bool hrt=false;D3DSURFACE_DESC hsd{};
                if(ht&&ht->GetType()==D3DRTYPE_TEXTURE&&SUCCEEDED(static_cast<IDirect3DTexture9*>(ht.Get())->GetLevelDesc(0,&hsd))) hrt=(hsd.Usage&D3DUSAGE_RENDERTARGET)!=0;
                if(hrt) {++statHaze;if(hazeLogs<60) {++hazeLogs;log("Render-target textured draw (0.9.33): %u vertices, texture %ux%u, tex %p, skipped for the headset=%d",unsigned(count),hsd.Width,hsd.Height,(void*)ht.Get(),int(hazeSkip));}
                    if(hazeSkip) {hideNative=hideNativeWorld;return;}}
            }
        }
        bool originDone=possessionWorld(request.mode,mode);
        if(EngineOrigin::enabled&&!originDone) {
            originDone=true;
            originFlags.resize(count);UINT marked=0,uiVerts=0;int firstId=0;float bx0=1e9f,by0=1e9f,bx1=-1e9f,by1=-1e9f;
            for(UINT i=0;i<count;++i) {
                const unsigned char* vx=input+size_t(i)*stride;const int id=EngineOrigin::idOf(vx);
                if(id) {++marked;if(!firstId) firstId=id;}
                // 0.8.17b: the game batches 2D interface quads (rhw EXACTLY 1.0: panel icons, '0' button, counter icons) into the same draw as dungeon
                // geometry when they share a texture-cache block (0.8.17 report: 'Buttons probe 3D DUNGEON n=52 rhw .188-1.000' = mixed draw hidden with the
                // dungeon). Vertices at rhw 1.0 are interface; the triangle rule (2 of 3 vertices) then splits the draw.
                float rq;memcpy(&rq,vx+12,4);
                const uint8_t f=(EngineOrigin::uiId(id)||std::fabs(rq-1.f)<1e-6f)?1:0;originFlags[i]=f;uiVerts+=f;
                float q[2];memcpy(q,vx,8);bx0=std::min(bx0,q[0]);by0=std::min(by0,q[1]);bx1=std::max(bx1,q[0]);by1=std::max(by1,q[1]);
            }
            if(!marked) ++statOriginUntagged;
            if(probeFrame&&smallLogs<700&&smallLogsFrame<90&&uiW>0&&uiH>0) {
                const float ihe=std::min(float(uiH),uiW*.75f),iwe=std::min(float(uiW),uiH*4.f/3.f),oye=(uiH-ihe)*.5f,oxe=(uiW-iwe)*.5f;
                const float x0=(bx0-oxe)/iwe,x1=(bx1-oxe)/iwe,y0=(by0-oye)/ihe,y1=(by1-oye)/ihe;
                if(x1-x0<.16f&&y1-y0<.18f&&(y0>=.60f||y1<=.13f)) {
                    ++smallLogs;++smallLogsFrame;float rmin=1e9f,rmax=-1e9f;
                    for(UINT i=0;i<count;++i) {float r;memcpy(&r,input+size_t(i)*stride+12,4);rmin=std::min(rmin,r);rmax=std::max(rmax,r);}
                    const uint32_t r0=(firstId>0&&firstId<256)?EngineOrigin::origins[firstId].ret0-uint32_t(EngineOrigin::k):0;
                    log("Small draw near the bars: n=%u prim=%u type=%d box %.3f,%.3f-%.3f,%.3f rhw %.3f-%.3f setup %06X tagged=%d",count,primitives,int(type),x0,y0,x1,y1,rmin,rmax,r0,int(marked>0));
                }
            }
            // 0.8.12: the 3D buttons / portraits / cards of the bars sit on a FIXED plane in front of the camera (rhw .38-.50 in every report:
            // n=195 .379-.441, n=104 .381-.427, n=192 .379-.418, n=37 .400-.452 ...), while the dungeon's rhw follows the zoom (.08-.31 in the
            // 0.8.9 / 0.8.10 reports). That is why the buttons came and went with the zoom. A draw that is small, lies on the lower half of the
            // screen and has ALL its vertices at rhw >= .36 (and at least 1.6 x the running dungeon rhw) is interface; never terrain code.
            if(!uiVerts&&count<=1500&&uiW>0&&uiH>0) {
                float rmin=1e9f,rmax=-1e9f,rsum=0;
                for(UINT i=0;i<count;++i) {float r;memcpy(&r,input+size_t(i)*stride+12,4);rmin=std::min(rmin,r);rmax=std::max(rmax,r);rsum+=r;}
                bool terrain=false;
                if(firstId>0&&firstId<256) {const uint32_t r=EngineOrigin::origins[firstId].ret0-uint32_t(EngineOrigin::k);terrain=(r>=0x587600&&r<0x587700)||(r>=0x588500&&r<0x588600);}
                const float ihe=std::min(float(uiH),uiW*.75f),iwe=std::min(float(uiW),uiH*4.f/3.f),oye=(uiH-ihe)*.5f;
                const float y0=(by0-oye)/ihe,y1=(by1-oye)/ihe,cyy=(y0+y1)*.5f,bwf=(bx1-bx0)/iwe,bhf=y1-y0;
                // 0.8.14: measured (0.8.12 report, 'Small draw' lines): buttons n=195 .360-.412, n=322 .360-.417, n=104 .362-.403, n=331 .360-.396, n=187 .360-.421 ->
                // the plane starts at exactly .360: the 0.8.12 threshold (.36 and 1.6 x the dungeon rhw) missed them, worse when zoomed in (dungeon rhw up).
                const bool plane=rmin>=.345f&&rmax<=.72f&&nearDungeonHold==0;
                // 0.8.16: when the dungeon itself reaches the interface plane (zoomed in / options screen: 0.8.15 report, dungeon draws at rhw .35-.72
                // over the whole screen -> thousands of dungeon pieces taken for buttons), the plane rule is off. Measured on draws in the MIDDLE of
                // the screen (y .12-.55), where no 3D interface piece ever lies.
                if(rmax>=.345f&&cyy>=.12f&&cyy<=.55f&&count>=6) ++nearDungeonDraws;
                // 0.8.17: small panel icons (filters right of the counters, '0' button) were missing: 3D pieces off the rhw plane. While the whole table lies
                // between the bars (pickFits), the dungeon never goes below its own lowest edge of the PREVIOUS frame (engine rectangle, .839 in the
                // 0.8.15 report): a small non-terrain piece ENTIRELY below it, in the bottom panel, is interface.
                const bool underDungeon=pickFits&&nearDungeonHold==0&&!terrain&&y0>=std::max(.70f,dungeonBottom+.004f)&&y1<=1.01f&&bwf<.35f&&bhf<.25f;
                if(y0<.60f&&count>=20&&y1<=1.2f) dungeonBottomNow=std::max(dungeonBottomNow,y1);
                if(probeFrame&&panelProbeLines<60&&y0>=.66f&&y0<=1.f) {++panelProbeLines;log("Panel 3D piece: n=%u box %.3f,%.3f-%.3f,%.3f rhw %.3f-%.3f terrain %d dungeon bottom %.3f fits %d near %d -> %s",count,(bx0-(uiW-iwe)*.5f)/iwe,y0,(bx1-(uiW-iwe)*.5f)/iwe,y1,rmin,rmax,int(terrain),dungeonBottom,int(pickFits),nearDungeonHold,(plane&&!terrain&&cyy>=.60f)?"plane":underDungeon?"below dungeon":"dungeon");}
                if(plane&&!terrain&&cyy>=.60f&&((bwf<.35f&&bhf<.25f)||(bhf<.25f&&cyy>=.80f))) {std::fill(originFlags.begin(),originFlags.end(),uint8_t(1));uiVerts=count;++statButtonRect;}
                else if(underDungeon) {std::fill(originFlags.begin(),originFlags.end(),uint8_t(1));uiVerts=count;++statUnderDungeon;}
                else if(rmax<.34f&&count>=40) rhwWorldEma=rhwWorldEma>0?rhwWorldEma+.02f*(rsum/count-rhwWorldEma):rsum/count;
            }
            auto at=[&](UINT n)->uint32_t{return !indices?uint32_t(n):indexFormat==D3DFMT_INDEX32?static_cast<const uint32_t*>(indices)[n]:uint32_t(static_cast<const uint16_t*>(indices)[n]);};
            const bool triangles=type==D3DPT_TRIANGLELIST||type==D3DPT_TRIANGLESTRIP||type==D3DPT_TRIANGLEFAN;
            UINT tris=primitives,uiTris=0;originUi.clear();originWorld.clear();
            if(uiVerts&&triangles) {
                tris=0;
                for(UINT t=0;t<primitives;++t) {
                    uint32_t a,b,c;
                    if(type==D3DPT_TRIANGLELIST) {a=at(3*t);b=at(3*t+1);c=at(3*t+2);}
                    else if(type==D3DPT_TRIANGLESTRIP) {a=at(t);b=at(t+1);c=at(t+2);if(t&1) std::swap(a,b);}
                    else {a=at(0);b=at(t+1);c=at(t+2);}
                    if(a>=count||b>=count||c>=count) continue;
                    ++tris;
                    // A triangle is interface when at least 2 of its 3 vertices come from an interface origin (a vertex
                    // created by the engine's clipping carries no origin).
                    if(originFlags[a]+originFlags[b]+originFlags[c]>=2) {++uiTris;originUi.insert(originUi.end(),{a,b,c});}
                    else originWorld.insert(originWorld.end(),{a,b,c});
                }
            } else if(uiVerts&&uiVerts*2>=count) uiTris=tris; // points / lines: whole draw by majority
            if(firstId>0&&firstId<256&&uiW>0&&uiH>0) {
                auto& o=EngineOrigin::origins[firstId];++o.draws;o.tris+=tris;o.uiTris+=uiTris;
                {const float ihe=std::min(float(uiH),uiW*.75f),iwe=std::min(float(uiW),uiH*4.f/3.f),oye=(uiH-ihe)*.5f;
                 const float ty=(by0-oye)/ihe,by=(by1-oye)/ihe,bw=(bx1-bx0)/iwe,bh=(by1-by0)/ihe;
                 const float Re=heldRadius*uiW/1024.f*.6f;const bool holdingNow=keeperW>=heldMin(100)&&keeperH>=heldMin(100); // grabbing-hand cursor image (118x112 / 134x137) = something is held
                 // Entirely inside the bottom bars / the top bar, or a small piece right at the cursor (held creature, tooltip).
                 const bool inBars=ty>=.66f&&by<=1.02f; // 0.8.10: no top-bar clause (dungeon torches / rubble at the top edge were taken for interface)
                 const bool atCursor=false&&holdingNow&&cursorKnown&&bw<.25f&&bh<.30f&&(bx0+bx1)*.5f>cursorPx-Re&&(bx0+bx1)*.5f<cursorPx+Re&&(by0+by1)*.5f>cursorPy-Re*1.4f&&(by0+by1)*.5f<cursorPy+Re*1.4f;
                 ++o.winDraws;if(inBars||atCursor) ++o.winEdge;}
                o.box[0]=std::min(o.box[0],bx0/uiW);o.box[1]=std::min(o.box[1],by0/uiH);o.box[2]=std::max(o.box[2],bx1/uiW);o.box[3]=std::max(o.box[3],by1/uiH);
            }
            if(uiTris&&uiTris>=tris) {
                ++statOriginUi;statOriginUiTris+=uiTris;++statUi3d;probe("3D INTERFACE (engine origin)");
                if(hudWanted()) uiReplay(d,type,primitives,verts,stride,indices,indexFormat,count,fvf);
                return;
            }
            if(uiTris) {
                ++statOriginMixed;statOriginUiTris+=uiTris;statOriginWorldTris+=tris-uiTris;probe("3D MIXED (engine origin: interface part to the HUD)");
                // Interface part: its own compact vertex list (the interface target measures boxes on the vertices it gets).
                originRemap.assign(count,UINT32_MAX);originUiVerts.clear();uint32_t nv=0;
                for(uint32_t& v:originUi) {
                    if(originRemap[v]==UINT32_MAX) {originRemap[v]=nv++;originUiVerts.insert(originUiVerts.end(),input+size_t(v)*stride,input+size_t(v+1)*stride);}
                    v=originRemap[v];
                }
                auto pack=[](const std::vector<uint32_t>& src,std::vector<unsigned char>& out,bool wide) {
                    out.resize(src.size()*(wide?4:2));
                    for(size_t i=0;i<src.size();++i) {if(wide) memcpy(out.data()+4*i,&src[i],4); else {const uint16_t s=uint16_t(src[i]);memcpy(out.data()+2*i,&s,2);}}
                };
                const bool uiWide=nv>65535,worldWide=count>65535;
                pack(originUi,originUiIdx,uiWide);pack(originWorld,originWorldIdx,worldWide);
                const D3DFORMAT uiFmt=uiWide?D3DFMT_INDEX32:D3DFMT_INDEX16;
                if(hudWanted()) uiReplay(d,D3DPT_TRIANGLELIST,uiTris,originUiVerts.data(),stride,originUiIdx.data(),uiFmt,nv,fvf);
                // The game draws this batch right after this hook with colour writes off (dungeon): its interface
                // triangles are drawn here, visible, with the game's own states.
                if(hideNativeWorld&&anchored&&topDown.active&&!possessedFrame&&!native.frontend())
                    d->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,nv,uiTris,originUiIdx.data(),uiFmt,originUiVerts.data(),stride);
                type=D3DPT_TRIANGLELIST;primitives=tris-uiTris;indices=originWorldIdx.data();indexFormat=worldWide?D3DFMT_INDEX32:D3DFMT_INDEX16;
                if(primitives==0) return;
            } else ++statOriginWorld;
        }
        // 0.6.5.3: interface cards (portraits, room/spell tabs) are animated by the game in 3D (RHW != 1) when
        // hovered or picked up. They use the same textures as the 2D interface: keep them out of the dungeon.
        if(!originDone&&uiTexFilter&&!uiTextures.empty()&&request.mode!=2) {
            ComPtr<IDirect3DBaseTexture9> t;d->GetTexture(0,&t);
            if(t&&uiTextures.count(t.Get())) {++statUiIn3d;if(hudWanted()) uiReplay(d,type,primitives,verts,stride,indices,indexFormat,count,fvf);return;}
        }
        // 0.6.9.3: interface pieces the game animates in 3D (RHW != 1: panel cards, stone strip with a portrait)
        // appeared on the table when zoomed out. Geometric test only (no texture identity): every vertex much
        // nearer the camera than the dungeon (rhw > 3x the typical dungeon rhw) AND drawn over the bottom bars.
        // 0.8.0: only when the engine origin is unavailable (InterfaceFromEngine=0 or unsupported exe).
        if(!originDone) {float r0=1e9f,rs=0,y0=1e9f,y1=-1e9f;
         for(UINT i=0;i<count;++i) {float v[4];memcpy(v,input+i*stride,16);r0=std::min(r0,v[3]);rs+=v[3];y0=std::min(y0,v[1]);y1=std::max(y1,v[1]);}
         const float mean=rs/std::max<UINT>(count,1);
         // 0.7.1: rhw rules restored (removed in 0.7.0 -> the 'i' button, visible in 0.6.9.17, vanished).
         const bool near3=ui3dFilter&&worldRhw>0&&r0>worldRhw*3.f;
         if(!near3&&mean>0) worldRhw=worldRhw>0?worldRhw+.002f*(mean-worldRhw):mean;
         // 0.6.9.4: 0.6.9.3.1 showed the stone strip still on the table (not nearer than 3x): while the whole table
         // lies between the bars (pickFits), a 3D draw lying ENTIRELY over the bottom bars (or the top bar) cannot be
         // table content (tiles out of the table are clipped anyway) -> interface.
         if(ui3dFilter&&uiH>0&&request.mode!=2) {
             const float ih=std::min(float(uiH),uiW*.75f),oy=(uiH-ih)*.5f,ty=(y0-oy)/ih,by=(y1-oy)/ih,cy=((y0+y1)*.5f-oy)/ih;
             float x0b=1e9f,x1b=-1e9f;for(UINT i=0;i<count;++i) {float v[2];memcpy(v,input+i*stride,8);x0b=std::min(x0b,v[0]);x1b=std::max(x1b,v[0]);}
             const float iwb=std::min(float(uiW),uiH*4.f/3.f),oxb=(uiW-iwb)*.5f,lx=(x0b-oxb)/iwb,rx=(x1b-oxb)/iwb;
             // 0.6.9.11: a small 3D piece lying inside one bar (stats bar with its 'o'/'i' buttons, minimap, panel, top bar)
             // is an interface element the game animates when hovered ('0' button, imp portrait vanished under the laser).
             bool inBar=count<=64&&rx-lx<=.30f&&by-ty<=.14f; // 0.6.9.13: every corner in SOME bar; 0.7.2: SMALL pieces only (probe: dungeon draws spanning .05-.78 had corners in the top AND bottom bars)
             {const float px[2]={lx,rx},py[2]={ty,by};
              for(int c=0;c<4&&inBar;++c) {bool any=false;const float qx=px[c&1],qy=py[c>>1];
                  for(int b=0;b<4&&!any;++b) {const HudRegion& g=HudRegions[b];any=qx>=g.u0-.01f&&qx<=g.u1+.01f&&qy>=g.v0-.01f&&qy<=g.v1+.01f;}
                  inBar=any;}}
             const bool overBars=(pickFits&&(ty>=std::max(pickBottom,.69f)||cy>=.74f||by<=std::min(pickTop,.07f)))||ty>=.76f||inBar; // 0.6.9.10: entirely on the bottom panel = interface (hovered portrait animated in 3D)
             if(!overBars&&by>.69f&&uiDiagLogs<30&&y1-y0>.02f*ih) {++uiDiagLogs;log("3D draw reaching the bars kept in the dungeon: y %.2f-%.2f count %u fits %d",ty,by,count,int(pickFits));}
             // 0.6.9.7: interface card the game animates in 3D next to the cursor (creature portrait picked from the
             // panel, 0.6.9.6 capture): a few vertices, every one much nearer the camera than the dungeon (rhw > 2.5x).
             const bool card=worldRhw>0&&count<=8&&r0>worldRhw*2.5f;
             // 0.7.3: ONLY small pieces can be interface (buttons, portraits, cards): a large 3D draw is always dungeon
             // (dungeon pieces classified as interface stayed visible in the game image -> seen in the transparent bars).
             // 0.7.5: the interface pieces animated in 3D (buttons row, portraits, counters) lie in the lower half of the
             // screen, away from its very edges: small dungeon triangles at the bottom edge / top bar (orange specks in the
             // 0.7.4 capture) are no longer taken for interface.
             // 0.7.7: SHAPE too: buttons / portraits / cards are square or tall (height >= 0.6 x width, width <= .10, height <= .20);
             // the orange rock strips seen under the brown bar are flat and wide -> dungeon.
             const float bw=rx-lx,bh=by-ty;
             const bool wideStrip=bw>=.5f&&bh<=.22f&&ty>=.80f&&by<=1.005f; // 0.7.10: the stone strip of the bottom panel (0.7.7 hid it)
             const bool small=(bw<=.10f&&bh<=.20f&&bh>=.6f*bw&&ty>=.68f&&by<=.975f)||wideStrip;
             if(small&&(overBars||card||(near3&&count<=256&&cy>.69f))) {++statUi3d;probe(overBars?"3D->HUD (bars)":card?"3D->HUD (near card)":"3D->HUD (near)");if(hudWanted()) uiReplay(d,type,primitives,verts,stride,indices,indexFormat,count,fvf);return;}
         }}
        // 0.6.9.16: from here the draw is dungeon for the headset: hidden in the game's image while the table is shown.
        hideNative=(anchored&&topDown.active&&!possessedFrame&&!native.frontend()&&request.mode!=2)||(SurroundHack::drawView!=0&&hideNativeWorld);
        probe(hideNative&&hideNativeWorld?"3D DUNGEON (hidden in the game image)":"3D dungeon (visible)");
        Projection lens{};
        if(!native.lens(lens)) {++rejected;return;}
        if(!loggedDepthState) {
            DWORD zEnable=0,zFunc=0,zWrite=0;
            d->GetRenderState(D3DRS_ZENABLE,&zEnable);d->GetRenderState(D3DRS_ZFUNC,&zFunc);d->GetRenderState(D3DRS_ZWRITEENABLE,&zWrite);
            float sample[4]{};memcpy(sample,input,16);
            DWORD cull=0;d->GetRenderState(D3DRS_CULLMODE,&cull);
            log("Native cull mode for world draws: %lu (1=none, 2=CW, 3=CCW; 1 means back faces are removed by the game itself)",(unsigned long)cull);
            log("Native world state: zEnable=%lu zFunc=%lu zWrite=%lu firstVertex=(%.3f,%.3f,%.6f,rhw=%.6f)",(unsigned long)zEnable,(unsigned long)zFunc,(unsigned long)zWrite,sample[0],sample[1],sample[2],sample[3]);
            loggedDepthState=true;
        }
        const bool possessed=request.mode==2||(request.mode==0&&(mode==1||mode==2));
        const double drawStarted=milliseconds();
        struct Timer {double start;double& total;~Timer(){total+=milliseconds()-start;}} timer{drawStarted,drawMilliseconds};
        long long rpMark=Clock::now();   // 0.9.26 PERF sections
        const int drawViewNow=SurroundHack::drawView;
        const bool face=drawViewNow>=1&&drawViewNow<=6;
        if(drawViewNow!=0) {
            // 0.8.84: a cube face (SurroundHack): only recorded for the VR thread, never shown in the game's own image
            hideNative=hideNativeWorld;
            if(!face||!possessed||!decoupledFrame||decoupledKind!=2||!possession360||!LensHack::cube.valid||LensHack::faceApplied!=drawViewNow) {++statViewSkipped;faceDropped=true;return;}
            // the anchor / reference level are only (re)made by the normal view (prepareFrame must never read the globals of a face)
            if(!possessionAnchorValid||possessionRecenter!=request.recenterSerial||!possessionLevelFinal) {++statViewSkipped;faceDropped=true;return;}
        } else {
            if(!tablePrepared||(possessed&&!possessionFrameReady)) prepareFrame(lens,possessed);
            if(possessed&&!possessionFrameReady) {++rejected;return;} // do not record leftover tabletop geometry as first-person world
            if(possessed) {selectView(0,lens);if(!viewReady) {++statViewSkipped;return;}}
        }
        const float units=face?request.possessionUnitsPerMeter:activeUnits;
        Pose camera=tableCamera;
        if(face) {
            if(!facePoseOk) {facePose=possessionAnchor;facePose.orientation=normalized(multiply(facePose.orientation,possessionLevel));facePoseOk=true;}
            camera=facePose;   // one pose for all faces of the frame; the clip planes of the package are built with the same pose
        }
        const float* Q=face?LensHack::cube.Q[drawViewNow-1]:nullptr;
        // 0.9.37: SCENE DEPTH MAP + SCREEN OVERLAYS. Report 011201 (BLEND F11): the dark 'star' over the forge walls is ONE transparent mesh (227 vertices,
        // 175 triangles, 222x262 px) drawn by the game at a single depth (rhw .3091 for every vertex): a light / shadow fan made for the flat screen. In the
        // headset it became a flat plate standing in front of the walls. The opaque geometry already drawn this frame is rasterised into a small rhw map;
        // each vertex of such an overlay is put back on the surface under it (just in front of it), so the fan lies on the walls and floor again.
        // 0.9.39: SMOKE / SOOT FANS of the forge. Reports 011201 and 082805 (BLEND F11): over the forge walls the game draws ONE alpha-blended mesh made like
        // a fan (227 v / 175 triangles, 109 v / 73 t, 113 v / 90 t), every vertex at the SAME depth: a darkening painted for the flat screen. In the headset it is a
        // flat dark 'star' standing in front of the walls. Only such fans are left out of the headset; the full-screen darkness / fog layers that 0.9.37 broke
        // (392 v / 193 t over the whole width, or 4-6 quads) do not match: blending SRCALPHA/INVSRCALPHA only, >= 90 vertices, >= 50 triangles,
        // triangles/vertices >= 0.6 (fan), one depth for all vertices, box <= 60 % x 70 % of the screen.
        if(forgeFanSkip&&drawViewNow==0&&!possessed&&!effectDraw&&count>=90&&primitives>=50&&primitives*10>=count*6&&uiW>0&&uiH>0) {
            DWORD fab=0,fsb=0,fdb=0;d->GetRenderState(D3DRS_ALPHABLENDENABLE,&fab);
            if(fab) {d->GetRenderState(D3DRS_SRCBLEND,&fsb);d->GetRenderState(D3DRS_DESTBLEND,&fdb);}
            if(fab&&fsb==D3DBLEND_SRCALPHA&&fdb==D3DBLEND_INVSRCALPHA) {
                float r0=1e9f,r1=-1e9f,x0=1e9f,x1=-1e9f,y0=1e9f,y1=-1e9f;
                for(UINT i=0;i<count;++i){float v[4];memcpy(v,input+i*stride,16);r0=std::min(r0,v[3]);r1=std::max(r1,v[3]);x0=std::min(x0,v[0]);x1=std::max(x1,v[0]);y0=std::min(y0,v[1]);y1=std::max(y1,v[1]);}
                if(r0>0&&r1<.9f&&(r1-r0)<=.002f*r1&&x1-x0<=.6f*float(uiW)&&y1-y0<=.7f*float(uiH)) {
                    // 0.9.47.19 WATER: the report of the water test (TEST_EAU_4 / 5 good, 3 bad) showed that the big draws of the WATER SURFACE (flat, one depth for
                    // the near top-down game camera, 150-300 vertices, 150-256 triangles, SRCALPHA/INVSRCALPHA, pale turquoise vertex colours 0xB27EFFFF) fit this fan rule
                    // and were hidden in the headset; only the small pieces that escape the size test stayed, so the surface showed a few patches that changed every frame.
                    // The forge soot is DARK: a fan whose vertex colours are pale (mean of R, G, B >= ForgeFanPaleMin, default 96 of 255) is not hidden any more.
                    static const bool keepPale=iniInt(L"ForgeFanKeepPale",1)!=0;
                    static const double paleMin=double(iniInt(L"ForgeFanPaleMin",96));
                    bool pale=false;double meanLight=0;
                    if(keepPale&&(fvf&D3DFVF_DIFFUSE)!=0) {meanLight=fanrule::meanLight(static_cast<const unsigned char*>(input),stride,count);pale=fanrule::paleMesh(meanLight,paleMin);}
                    if(pale) {
                        ++statForgeFanKept;
                        if(forgeFanKeptLogs<12) {++forgeFanKeptLogs;log("Forge fan rule (0.9.47.19): pale draw kept (water surface): %u vertices, %u triangles, box %.0f,%.0f-%.0f,%.0f, mean colour %.0f of 255",unsigned(count),unsigned(primitives),x0,y0,x1,y1,meanLight);}
                    } else {
                        ++statForgeFans;
                        if(forgeFanLogs<20) {++forgeFanLogs;log("Forge smoke fan (0.9.39): %u vertices, %u triangles, box %.0f,%.0f-%.0f,%.0f, one depth rhw %.4f, mean colour %.0f -> not shown in the headset",unsigned(count),unsigned(primitives),x0,y0,x1,y1,r0,meanLight);}
                        return;
                    }
                }
            }
        }
        overlayActive=false;
        decalActive=false;
        if((overlayReproject||decalSnap)&&drawViewNow==0&&!possessed&&!effectDraw&&uiW>0&&uiH>0) {
            if(sceneMapSeq!=presentSeq) {sceneMapSeq=presentSeq;sceneMap.assign(size_t(SceneGW)*SceneGH,0.f);}
            DWORD oab=0;d->GetRenderState(D3DRS_ALPHABLENDENABLE,&oab);
            auto idxOf=[&](UINT j)->UINT{if(!indices) return j;return indexFormat==D3DFMT_INDEX32?UINT(((const uint32_t*)indices)[j]):UINT(((const uint16_t*)indices)[j]);};
            auto tri=[&](UINT k,UINT& a,UINT& b,UINT& c)->bool{
                if(type==D3DPT_TRIANGLELIST){a=idxOf(3*k);b=idxOf(3*k+1);c=idxOf(3*k+2);}
                else if(type==D3DPT_TRIANGLESTRIP){a=idxOf(k);b=idxOf(k+1);c=idxOf(k+2);}
                else if(type==D3DPT_TRIANGLEFAN){a=idxOf(0);b=idxOf(k+1);c=idxOf(k+2);}
                else return false;
                return a<count&&b<count&&c<count;};
            const float gx=float(SceneGW)/float(uiW),gy=float(SceneGH)/float(uiH);
            if(!oab) {   // opaque: write the nearest rhw of every covered cell
                const double t0=milliseconds();
                for(UINT k=0;k<primitives;++k) {UINT a,b,c;if(!tri(k,a,b,c)) break;
                    float A[4],B[4],C[4];memcpy(A,input+a*stride,16);memcpy(B,input+b*stride,16);memcpy(C,input+c*stride,16);
                    if(!(A[3]>0&&B[3]>0&&C[3]>0&&A[3]<.9f&&B[3]<.9f&&C[3]<.9f)) continue;
                    const float ax=A[0]*gx,ay=A[1]*gy,bx=B[0]*gx,by=B[1]*gy,cx=C[0]*gx,cy=C[1]*gy;
                    const float den=(by-cy)*(ax-cx)+(cx-bx)*(ay-cy);if(std::fabs(den)<1e-6f) continue;
                    const int x0=std::max(0,int(std::floor(std::min({ax,bx,cx})))),x1=std::min(SceneGW-1,int(std::ceil(std::max({ax,bx,cx})))),
                              y0=std::max(0,int(std::floor(std::min({ay,by,cy})))),y1=std::min(SceneGH-1,int(std::ceil(std::max({ay,by,cy}))));
                    for(int y=y0;y<=y1;++y)for(int x=x0;x<=x1;++x){const float px=x+.5f,py=y+.5f;
                        const float l1=((by-cy)*(px-cx)+(cx-bx)*(py-cy))/den,l2=((cy-ay)*(px-cx)+(ax-cx)*(py-cy))/den,l3=1.f-l1-l2;
                        if(l1<-.02f||l2<-.02f||l3<-.02f) continue;
                        const float w=l1*A[3]+l2*B[3]+l3*C[3];float& m=sceneMap[size_t(y)*SceneGW+x];if(w>m) m=w;}
                }
                sceneMapMs+=milliseconds()-t0;
            } else {
              if(overlayReproject&&count>=6&&primitives>=4) {   // blended: a screen overlay = one single depth over a large area
                float r0=1e9f,r1=-1e9f,x0=1e9f,x1=-1e9f,y0=1e9f,y1=-1e9f;
                for(UINT i=0;i<count;++i){float v[4];memcpy(v,input+i*stride,16);r0=std::min(r0,v[3]);r1=std::max(r1,v[3]);x0=std::min(x0,v[0]);x1=std::max(x1,v[0]);y0=std::min(y0,v[1]);y1=std::max(y1,v[1]);}
                const bool overlay=r0>0&&r1<.9f&&(r1-r0)<=.002f*r1&&(x1-x0>=60.f||y1-y0>=60.f);
                if(overlay) {
                    overlayRhw.assign(count,0.f);unsigned moved=0;
                    for(UINT i=0;i<count;++i){float v[4];memcpy(v,input+i*stride,16);
                        const int cxi=int(v[0]*gx),cyi=int(v[1]*gy);float best=0;
                        for(int dy=-1;dy<=1;++dy)for(int dx=-1;dx<=1;++dx){const int xx=cxi+dx,yy=cyi+dy;if(xx<0||yy<0||xx>=SceneGW||yy>=SceneGH) continue;best=std::max(best,sceneMap[size_t(yy)*SceneGW+xx]);}
                        if(best>0) {overlayRhw[i]=best*1.004f;++moved;}}
                    ++statOverlayDraws;statOverlayVerts+=moved;statOverlayMissed+=count-moved;
                    if(overlayLogs<12) {++overlayLogs;log("Screen overlay (0.9.37): %u vertices, %u triangles, box %.0f,%.0f-%.0f,%.0f, single depth rhw %.4f -> %u vertices put on the surface under them, %u without surface (kept)",
                        unsigned(count),unsigned(primitives),x0,y0,x1,y1,r0,moved,unsigned(count)-moved);}
                    overlayActive=moved>0;
                }
              }
              // 0.9.42: DECALS PULLED TOWARD THE GAME CAMERA. Report 090932 (F11 geometry + textures, rebuilt offline): the arched door of the forge wall is
              // drawn as transparent decal polygons over the wall (texture = the door), the game textures and their headset copies are IDENTICAL, but part of
              // the decal vertices sit 14-38 mm in front of the wall: the game pulls decals toward its camera so they never fight with the wall. On the flat
              // screen this is invisible; in the headset these vertices stand out of the wall: the 'star' made of door / wall pieces in front of the forge.
              // A vertex of a perspective (non camera-facing) triangle of a SRCALPHA/INVSRCALPHA draw that is at most DecalSnapMax in front of the opaque
              // surface under it (depth map of this frame) is put back on that surface along the game camera ray (same screen position). Camera-facing
              // pieces (smoke, sprites, icons), anything behind a surface or far in front of it are never moved.
              if(decalSnap&&!overlayActive&&count>=3) {
                  DWORD dsb=0,ddb=0;d->GetRenderState(D3DRS_SRCBLEND,&dsb);d->GetRenderState(D3DRS_DESTBLEND,&ddb);
                  if(dsb==D3DBLEND_SRCALPHA&&ddb==D3DBLEND_INVSRCALPHA) {
                      decalFlag.assign(count,0);
                      for(UINT k=0;k<primitives;++k){UINT a,b,c;if(!tri(k,a,b,c)) break;float A[4],B[4],C[4];memcpy(A,input+a*stride,16);memcpy(B,input+b*stride,16);memcpy(C,input+c*stride,16);
                          const float mx=std::max({A[3],B[3],C[3]}),mn=std::min({A[3],B[3],C[3]});const uint8_t bit=(mx-mn)<=.004f*mx?2:1;decalFlag[a]|=bit;decalFlag[b]|=bit;decalFlag[c]|=bit;}
                      overlayRhw.assign(count,0.f);unsigned moved=0,candidates=0;
                      for(UINT i=0;i<count;++i){if(decalFlag[i]!=1) continue;++candidates;float v[4];memcpy(v,input+i*stride,16);
                          const int cxi=int(v[0]*gx),cyi=int(v[1]*gy);if(cxi<0||cyi<0||cxi>=SceneGW||cyi>=SceneGH) continue;
                          const float sr=sceneMap[size_t(cyi)*SceneGW+cxi];
                          if(sr>0&&v[3]>sr*1.0005f&&v[3]<=sr*decalSnapMax) {overlayRhw[i]=sr*1.0015f;++moved;}}
                      if(moved) {decalActive=true;++statDecalSnapDraws;statDecalSnapVerts+=moved;
                          if(decalSnapLogs<16) {++decalSnapLogs;log("Decal on surface (0.9.42): %u vertices (%u candidates), %u put back on the surface behind them",unsigned(count),candidates,moved);}}
                  }
              }
            }
        }
        positions.resize(count);sprHv.assign(count,0.f);
        for(UINT i=0;i<count;++i) {
            float v[4];memcpy(v,input+i*stride,16);Vec3 p{};
            if((overlayActive||decalActive)&&i<overlayRhw.size()&&overlayRhw[i]>0) v[3]=overlayRhw[i];
            if(!unproject(v[0],v[1],effectDraw?1.f/(possessionEffectsDistance*units):v[3],lens,units,p)) {++rejected;return;}
            if(Q) p=Vec3{Q[0]*p.x+Q[1]*p.y+Q[2]*p.z,Q[3]*p.x+Q[4]*p.y+Q[5]*p.z,Q[6]*p.x+Q[7]*p.y+Q[8]*p.z};   // face -> front camera space (exact)
            positions[i]=toWorld(camera,p);
            if(effectDraw) continue;   // 0.8.99: an attack effect is no geometry of the world (levels, nearest geometry)
            if(anchored&&measuredLevels&&(i&1)==0&&heights.size()<40000) heights.push_back(dot(anchorUnits.up,p)*units);
            sprHv[i]=anchored?dot(anchorUnits.up,p)*units:0.f;
            if(anchored) frameNearGeom=std::min(frameNearGeom,dot(anchorUnits.forward,p)*units); // 0.5.12: nearest geometry the game produced
        }
        RecProf::lap(1,rpMark);
        // 0.8.22: icons / numbers / sprites (salary '$', damage, mood, torch flames, particles) are camera-facing quads of the game: with the top-down
        // game camera they lie FLAT on the table. Detection generalised (0.8.21 only saw draws of <= 8 vertices): the triangles of ANY draw
        // (<= 1500 vertices) are grouped in pieces (shared indices or identical screen position); a piece is a sprite if it is small, camera facing
        // (constant rhw), not terrain code, and above the walking floor. It is rebuilt standing upright and facing the eye (eye position of this frame).
        // 0.9.9 (report 230438): independent switches for icons and additive world sprites.
        // UV orientation + geometric pivot; table vertical locked; additive bottom anchored; never applied in possession.
        std::vector<ReplaySprite> drawSprites;
        bool spriteAdditive=false,uprightEffectDraw=false,floorDecalDraw=false,digitDraw=false;unsigned decalPieces=0,otherPieces=0,softPieces=0;std::vector<digitnum::Piece> pendingDigits;   // 0.9.47.21
        DWORD spAb=0,spSb=0,spDb=0,spAt=0,spZw=0;
        {d->GetRenderState(D3DRS_ALPHABLENDENABLE,&spAb);d->GetRenderState(D3DRS_SRCBLEND,&spSb);d->GetRenderState(D3DRS_DESTBLEND,&spDb);spriteAdditive=spAb&&spDb==D3DBLEND_ONE;}
        // 0.9.27: icons, numbers, flames and smoke are drawn with transparency (blend or alpha test). An OPAQUE draw is model / floor geometry (heart shards,
        // broken floor seen from straight above had a constant depth and stood up facing the player) - it is never turned upright, and costs no sprite search.
        if(spriteAlphaOnly&&!spAb) {d->GetRenderState(D3DRS_ALPHATESTENABLE,&spAt);}
        if(spriteCensus>0) d->GetRenderState(D3DRS_ZWRITEENABLE,&spZw);
        const bool spriteMaterialOk=!spriteAlphaOnly||spAb||spAt;
        if(!spriteMaterialOk&&!possessed&&anchored) ++statSpriteOpaqueSkipped;
        if(spriteMaterialOk&&!overlayActive&&((spriteAdditive&&uprightEffects)||(!spriteAdditive&&billboards))&&!possessed&&!effectDraw&&anchored&&haveLevels&&count>=3&&count<=1500&&table.valid&&(type==D3DPT_TRIANGLELIST||type==D3DPT_TRIANGLESTRIP||type==D3DPT_TRIANGLEFAN)) {
            const float iwe=std::min(float(uiW),uiH*4.f/3.f);
            const float floorH=floorForHeights+(haveWalk?walkOffset:0.f);
            bool terrain=false;{const int id=EngineOrigin::enabled?EngineOrigin::idOf(input):0;if(id>0&&id<256) {const uint32_t r=EngineOrigin::origins[id].ret0-uint32_t(EngineOrigin::k);terrain=(r>=0x587600&&r<0x587700)||(r>=0x588500&&r<0x588600);}}
            // 0.9.47.5 (report 121026): the dark disc + number of a creature badge is a 10-vertex fan drawn by code at 588517 (inside the 'terrain' range): it was never turned upright and
            // lay flat inside the upright ring. A SMALL alpha-blended draw (<= 16 vertices) from that range is a badge disc, not terrain (terrain draws have hundreds of vertices).
            if(terrain&&badgeDiscFix&&spAb&&!spriteAdditive&&count<=16) {terrain=false;++statBadgeDisc;}
            if(!terrain&&iwe>0) {
                auto idxAt=[&](UINT j)->UINT{if(!indices) return j;return indexFormat==D3DFMT_INDEX32?UINT(((const uint32_t*)indices)[j]):UINT(((const uint16_t*)indices)[j]);};
                bbParent.resize(count);for(UINT i=0;i<count;++i) bbParent[i]=i;
                auto findRoot=[&](UINT a){while(bbParent[a]!=a) {bbParent[a]=bbParent[bbParent[a]];a=bbParent[a];}return a;};
                auto unite=[&](UINT a,UINT b){a=findRoot(a);b=findRoot(b);if(a!=b) bbParent[b]=a;};
                bbUsed.assign(count,0);
                bool okTri=true;
                for(UINT k=0;k<primitives&&okTri;++k) {
                    UINT a,b,c2;
                    if(type==D3DPT_TRIANGLELIST) {a=idxAt(3*k);b=idxAt(3*k+1);c2=idxAt(3*k+2);}
                    else if(type==D3DPT_TRIANGLESTRIP) {a=idxAt(k);b=idxAt(k+1);c2=idxAt(k+2);}
                    else {a=idxAt(0);b=idxAt(k+1);c2=idxAt(k+2);}
                    if(a>=count||b>=count||c2>=count) {okTri=false;break;}
                    bbUsed[a]=bbUsed[b]=bbUsed[c2]=1;unite(a,b);unite(a,c2);
                }
                if(okTri) {
                    // identical screen position = same corner shared by two triangles of a quad that are not indexed
                    bbOrder.clear();for(UINT i=0;i<count;++i) if(bbUsed[i]) bbOrder.push_back(i);
                    std::sort(bbOrder.begin(),bbOrder.end(),[&](UINT p,UINT q){float a[2],b[2];memcpy(a,input+p*stride,8);memcpy(b,input+q*stride,8);return a[0]<b[0]||(a[0]==b[0]&&a[1]<b[1]);});
                    for(size_t s=1;s<bbOrder.size();++s) {float a[2],b[2];memcpy(a,input+bbOrder[s-1]*stride,8);memcpy(b,input+bbOrder[s]*stride,8);if(a[0]==b[0]&&a[1]==b[1]) unite(bbOrder[s-1],bbOrder[s]);}
                    // group vertices by piece
                    bbOrder.clear();for(UINT i=0;i<count;++i) if(bbUsed[i]) bbOrder.push_back(i);
                    std::sort(bbOrder.begin(),bbOrder.end(),[&](UINT p,UINT q){const UINT rp=findRoot(p),rq=findRoot(q);return rp<rq||(rp==rq&&p<q);});
                    size_t s0=0;int logsHere=0;
                    struct DigitPc {size_t a,b;float sx,sy;Vec3 right;};static thread_local std::vector<DigitPc> digitPcs;digitPcs.clear();   // 0.9.47.3: pieces of damage digits, to mirror the whole number
                    while(s0<bbOrder.size()) {
                        const UINT root=findRoot(bbOrder[s0]);size_t s1=s0;while(s1<bbOrder.size()&&findRoot(bbOrder[s1])==root) ++s1;
                        const size_t n=s1-s0;
                        if(!(n>=3&&n<=24)) ++otherPieces;
                        if(n>=3&&n<=24) {
                            float rmin=1e9f,rmax=-1e9f,sx0=1e9f,sx1=-1e9f,sy0=1e9f,sy1=-1e9f,hmin=1e9f,hmax=-1e9f;
                            for(size_t s=s0;s<s1;++s) {const UINT i=bbOrder[s];float v[4];memcpy(v,input+i*stride,16);rmin=std::min(rmin,v[3]);rmax=std::max(rmax,v[3]);sx0=std::min(sx0,v[0]);sx1=std::max(sx1,v[0]);sy0=std::min(sy0,v[1]);sy1=std::max(sy1,v[1]);hmin=std::min(hmin,sprHv[i]);hmax=std::max(hmax,sprHv[i]);}
                            const bool flat=rmin>0&&(rmax-rmin)<=.004f*rmax&&rmax<.9f; // rhw 1.0 = 2D interface
                            const bool small=sx1-sx0>2&&sy1-sy0>2&&sx1-sx0<=spriteMaxScreen*iwe&&sy1-sy0<=spriteMaxScreen*iwe;   // 0.9.32: .12 -> .28 (the big flame / glow plates of the forge were never turned to face the headset)
                            const bool above=spriteAdditive?(effectMinHeight<0.f?hmin>=floorH-.04f*units:hmin>floorH+effectMinHeight):(iconTopRule?hmax:hmin)>floorH+billboardMinHeight;   // 0.9.35: an icon is judged by its TOP (a ring split in pieces stood up only partly: half flat, half standing, report 000353)   // 0.9.29: floor-level glows (portal, hatchery) stay flat
                            if(spriteCensus>0&&flat&&spriteCensusLogs<1500&&(spriteAdditive||sx1-sx0>=30||sy1-sy0>=30)) {   // 0.9.27 F11 (0.9.32: additive or large pieces only, so the cap is not eaten by tiny ones): every flat piece of the captured frames, with its material and origin
                                ++spriteCensusLogs;const int oid=EngineOrigin::enabled?EngineOrigin::idOf(input):0;const uint32_t oret=(oid>0&&oid<256)?EngineOrigin::origins[oid].ret0-uint32_t(EngineOrigin::k):0;
                                log("SPRITE F11: draw %u v, piece %u v, screen %.0fx%.0f at %.0f,%.0f, rhw %.4f, height %.3f, blend %lu src %lu dst %lu, alphatest %lu, zwrite %lu, origin %u setup %06X -> %s",count,unsigned(n),sx1-sx0,sy1-sy0,(sx0+sx1)*.5f,(sy0+sy1)*.5f,rmin,hmin-floorH,
                                    (unsigned long)spAb,(unsigned long)spSb,(unsigned long)spDb,(unsigned long)spAt,(unsigned long)spZw,unsigned(oid),unsigned(oret),!small?"too big":!above?"too low":spriteAdditive?"upright effect":"upright icon");
                            }
                            // 0.9.30: floor decals (the soft shadows of the creatures and objects: 3 stacked quads each, all at the floor height, depth-writing).
                            // Coplanar quads that all write depth fight with each other when the headset moves (report 202644, hatchery: several flickering shadows).
                            if(!flat&&rmin>0&&rmax<.9f) ++softPieces;   // 0.9.31: perspective piece (smoke / fume plate of the forge, brazier, portal)
                            if(flat&&small&&!spriteAdditive&&hmin-floorH>=-.03f&&hmin-floorH<=.03f) ++decalPieces; else ++otherPieces;
                            if(spriteCensus>0&&!flat&&small&&spriteCensusLogs<500&&rmin>0) {   // 0.9.30 F11: small transparent pieces that are NOT constant-depth (they are never turned upright): could be icons drawn in perspective
                                ++spriteCensusLogs;log("SPRITE F11 NF: draw %u v, piece %u v, screen %.0fx%.0f at %.0f,%.0f, rhw %.4f..%.4f (spread %.2f %%), height %.3f, blend %lu src %lu dst %lu, alphatest %lu",count,unsigned(n),sx1-sx0,sy1-sy0,(sx0+sx1)*.5f,(sy0+sy1)*.5f,rmin,rmax,100.f*(rmax-rmin)/std::max(rmax,1e-6f),hmin-floorH,
                                    (unsigned long)spAb,(unsigned long)spSb,(unsigned long)spDb,(unsigned long)spAt);
                            }
                            // 0.9.45: damage numbers (user, report 102002: present and the right way round, but turned where they cannot be seen). The game draws them as small
                            // glyph quads tilted toward ITS camera (rhw not constant, so 'flat' is false and they were never turned to the headset like the creature icons).
                            // Narrow rule: alpha-blended (not additive), exactly one or two quads, a perspective spread under 12 %, texture cells of a glyph atlas (0.2..0.3 wide).
                            bool glyphCand=false;
                            if(damageFacing&&!flat&&!spriteAdditive&&small&&rmin>0&&rmax<.9f&&(rmax-rmin)<=.12f*rmax&&(n==4||n==8)&&spAb&&spSb==5&&spDb==6) {
                                const UINT uvAt=16u+((fvf&D3DFVF_DIFFUSE)?4u:0u)+((fvf&D3DFVF_SPECULAR)?4u:0u);
                                if((fvf&D3DFVF_TEXCOUNT_MASK)>=D3DFVF_TEX1&&uvAt+8<=stride) {
                                    float uu0=1e9f,uu1=-1e9f,vv0=1e9f,vv1=-1e9f;
                                    for(size_t j=s0;j<s1;++j) {float uv[2];memcpy(uv,input+bbOrder[j]*stride+uvAt,8);uu0=std::min(uu0,uv[0]);uu1=std::max(uu1,uv[0]);vv0=std::min(vv0,uv[1]);vv1=std::max(vv1,uv[1]);}
                                    const float cu=uu1-uu0,cv=vv1-vv0;
                                    if(cu>=.2f&&cu<=.3f&&cv>=.2f&&cv<=.3f) {glyphCand=true;++statGlyphSprites;}
                                }
                            }
                            // 0.9.47: damage numbers (report 105954): additive, constant-depth, tiny (about 11x17 px) digit quads from a 4x4 glyph atlas, drawn at floor height
                            // (census: 'too low'), so they stayed lying flat = seen edge-on. Narrow rule: flat + additive + small + 6..30 px + UV cell .2..0.3. 0.9.47.2: the digit shape is also tested above the floor (the same digits rise: report 113329 showed them still mirrored there) - only the mirror applies then.
                            bool digitCand=false;
                            bool digitShapeOk=false;
                            if(damageDigits&&flat&&spriteAdditive&&small&&(n==4||n==6)) {   // 0.9.47.23: size of the glyph cell measured along its own axes (the game also draws digits tilted or turned: report 010044, some stayed flat = seen edge-on)
                                const UINT uvAt3=16u+((fvf&D3DFVF_DIFFUSE)?4u:0u)+((fvf&D3DFVF_SPECULAR)?4u:0u);
                                if((fvf&D3DFVF_TEXCOUNT_MASK)>=D3DFVF_TEX1&&uvAt3+8<=stride) {
                                    float ax[24],ay[24],au[24],av[24];size_t k=0;
                                    for(size_t j=s0;j<s1&&k<24;++j,++k) {const UINT i=bbOrder[j];float sp[2],uv[2];memcpy(sp,input+i*stride,8);memcpy(uv,input+i*stride+uvAt3,8);ax[k]=sp[0];ay[k]=sp[1];au[k]=uv[0];av[k]=uv[1];}
                                    float gw=0,gh=0;digitShapeOk=digitnum::digitShape(ax,ay,au,av,k,gw,gh)&&gw>=6&&gw<=30&&gh>=8&&gh<=34&&gh>=1.25f*gw;
                                }
                            }
                            if(digitShapeOk) {   // 0.9.47.4: a digit is taller than wide (7x10 .. 12x18 px measured); square sparks of the same atlas are left alone
                                const UINT uvAt2=16u+((fvf&D3DFVF_DIFFUSE)?4u:0u)+((fvf&D3DFVF_SPECULAR)?4u:0u);
                                if((fvf&D3DFVF_TEXCOUNT_MASK)>=D3DFVF_TEX1&&uvAt2+8<=stride) {
                                    float uu0=1e9f,uu1=-1e9f,vv0=1e9f,vv1=-1e9f;
                                    for(size_t j=s0;j<s1;++j) {float uv[2];memcpy(uv,input+bbOrder[j]*stride+uvAt2,8);uu0=std::min(uu0,uv[0]);uu1=std::max(uu1,uv[0]);vv0=std::min(vv0,uv[1]);vv1=std::max(vv1,uv[1]);}
                                    const float cu=uu1-uu0,cv=vv1-vv0;
                                    if(cu>=.2f&&cu<=.3f&&cv>=.2f&&cv<=.3f) {digitCand=true;digitDraw=true;++statDigitSprites;}
                                }
                            }
                            if(spriteAdditive&&(n==4||n==6)&&(fvf&D3DFVF_TEXCOUNT_MASK)>=D3DFVF_TEX1) {   // 0.9.47.29: counted for ever (the log used to stop after 160 lines = the first load); small quads only are logged   // 0.9.47.24: diagnostic - every additive small quad that looks like a glyph, whatever the rules decide (digits correct in only 1 launch out of 3, report 081721)
                                const UINT uvP=16u+((fvf&D3DFVF_DIFFUSE)?4u:0u)+((fvf&D3DFVF_SPECULAR)?4u:0u);
                                if(uvP+8<=stride) {
                                    float pax[24],pay[24],pau[24],pav[24];size_t pk=0;float pu0=1e9f,pu1=-1e9f,pv0=1e9f,pv1=-1e9f;
                                    for(size_t j=s0;j<s1&&pk<24;++j,++pk) {const UINT i=bbOrder[j];float sp[2],uv[2];memcpy(sp,input+i*stride,8);memcpy(uv,input+i*stride+uvP,8);pax[pk]=sp[0];pay[pk]=sp[1];pau[pk]=uv[0];pav[pk]=uv[1];pu0=std::min(pu0,uv[0]);pu1=std::max(pu1,uv[0]);pv0=std::min(pv0,uv[1]);pv1=std::max(pv1,uv[1]);}
                                    if(pu1-pu0>=.2f&&pu1-pu0<=.3f&&pv1-pv0>=.2f&&pv1-pv0<=.3f) {
                                        float pgw=0,pgh=0;const bool pshape=digitnum::digitShape(pax,pay,pau,pav,pk,pgw,pgh);++dgGlyph;if(!small)++dgBig;else if(!flat)++dgNotFlat;else if(!pshape)++dgShapeNo;
                                        static DWORD probeWin=0;static int probeWinN=0;if(DWORD(GetTickCount()-probeWin)>5000){probeWin=GetTickCount();probeWinN=0;}
                                        if(!kPublicBuild&&small&&(digitProbeLogs<160||probeWinN<3)) {if(digitProbeLogs>=160)++probeWinN;++digitProbeLogs;
                                        log("Digit probe (0.9.47.24) #%d: %u v, flat %d (rhw %.4f..%.4f, spread %.2f %%), screen %.0fx%.0f at %.0f,%.0f, cell %.2fx%.2f, shape %s %.1fx%.1f, above %d, height %.2f, rules damageDigits=%d screenCopy=%d",
                                            digitProbeLogs,unsigned(n),flat?1:0,rmin,rmax,100.f*(rmax-rmin)/std::max(rmax,1e-6f),sx1-sx0,sy1-sy0,(sx0+sx1)*.5f,(sy0+sy1)*.5f,pu1-pu0,pv1-pv0,pshape?"ok":"no",pgw,pgh,above?1:0,hmin-floorH,damageDigits?1:0,digitScreen?1:0);}
                                    }
                                }
                            }
                            if(flat||glyphCand) {
                                ++statBillboardSeen;
                                if(probeFrame&&billboardLogs<400&&logsHere<14&&(sx1-sx0>=8||sy1-sy0>=8)) {++billboardLogs;++logsHere;log("Sprite candidate: draw %u verts, piece %u verts, screen %.0fx%.0f rhw %.4f height above floor %.2f -> %s",count,unsigned(n),sx1-sx0,sy1-sy0,rmin,hmin-floorH,!small?"too big, kept flat":!above?"too low, kept flat":"upright, UV-centred, yaw faces headset");}
                                if(small&&(above||digitCand)) {
                                    // 0.9.9: texture-space axes remove the native camera's roll.
                                    // A bbox UV pivot is independent of repeated triangle corners.
                                    const UINT texOffset=16u+((fvf&D3DFVF_DIFFUSE)?4u:0u)+((fvf&D3DFVF_SPECULAR)?4u:0u);
                                    if((fvf&D3DFVF_TEXCOUNT_MASK)>=D3DFVF_TEX1&&((fvf>>16)&0xffffu)==0&&texOffset+8<=stride) {
                                        static thread_local std::vector<SpriteSample> samples;samples.clear();samples.reserve(n);   // 0.9.26: reused (no allocation per sprite)
                                        for(size_t j=s0;j<s1;++j) {const UINT i=bbOrder[j];float uv[2];memcpy(uv,input+i*stride+texOffset,8);samples.push_back({positions[i],uv[0],uv[1]});}
                                        const Vec3 eye=scale(add(request.eyes[0].pose.position,request.eyes[1].pose.position),.5f);
                                        // 0.9.47.23: damage digits = copy of the game-screen picture on a plane facing the headset (see src/common/DigitNumber.hpp)
                                        bool digitHandled=false;
                                        if(digitCand&&digitScreen) {
                                            float bestS=0,bestW=0;
                                            for(size_t a=s0;a<s1;++a)for(size_t b=a+1;b<s1;++b){float A[2],B[2];memcpy(A,input+bbOrder[a]*stride,8);memcpy(B,input+bbOrder[b]*stride,8);
                                                const float ds=std::hypot(A[0]-B[0],A[1]-B[1]);if(ds>bestS){const Vec3 dw=sub(positions[bbOrder[a]],positions[bbOrder[b]]);bestS=ds;bestW=std::sqrt(dw.x*dw.x+dw.y*dw.y+dw.z*dw.z);}}
                                            if(bestS>1.f&&bestW>0&&std::isfinite(bestW)) {
                                                digitnum::Piece dp;dp.sx=(sx0+sx1)*.5f;dp.sy=(sy0+sy1)*.5f;dp.pw=bestW/bestS;dp.up=spriteWorldVertical(tableCamera,table.up);dp.eye=eye;dp.right=rotate(camera.orientation,{1,0,0});
                                                dp.lift=!above;dp.pull=effectPull>0?effectPull*dp.pw*std::max(sx1-sx0,sy1-sy0):0.f;dp.ext=std::max(sx1-sx0,sy1-sy0);{float cu0=1e9f,cv0=1e9f;for(size_t j=s0;j<s1;++j){float uv[2];memcpy(uv,input+bbOrder[j]*stride+texOffset,8);cu0=std::min(cu0,uv[0]);cv0=std::min(cv0,uv[1]);}dp.cu=cu0;dp.cv=cv0;}
                                                Vec3 dc{0,0,0};
                                                for(size_t j=s0;j<s1;++j) {const UINT i=bbOrder[j];float sp[2];memcpy(sp,input+i*stride,8);dp.ids.push_back(uint32_t(i));dp.vsx.push_back(sp[0]);dp.vsy.push_back(sp[1]);{float tuv[2];memcpy(tuv,input+i*stride+texOffset,8);dp.vu.push_back(tuv[0]);dp.vv.push_back(tuv[1]);}dc=add(dc,positions[i]);}
                                                dp.centre=scale(dc,1.f/float(s1-s0));pendingDigits.push_back(std::move(dp));
                                                ++statBillboards;uprightEffectDraw=true;digitHandled=true;++dgHandledQ;++dgHandledQ;
                                                if(digitHandledLogs<80) {++digitHandledLogs;log("Digit handled (0.9.47.24) #%d: screen %.0f,%.0f pw %.5f up %.2f %.2f %.2f right %.2f %.2f %.2f eye %.2f %.2f %.2f centre %.2f %.2f %.2f lift %d",digitHandledLogs,pendingDigits.back().sx,pendingDigits.back().sy,pendingDigits.back().pw,pendingDigits.back().up.x,pendingDigits.back().up.y,pendingDigits.back().up.z,pendingDigits.back().right.x,pendingDigits.back().right.y,pendingDigits.back().right.z,eye.x,eye.y,eye.z,pendingDigits.back().centre.x,pendingDigits.back().centre.y,pendingDigits.back().centre.z,pendingDigits.back().lift?1:0);}
                                            }
                                        }
                                        if(!digitHandled) {
                                        UprightSprite frame;
                                        const bool fitted=frame.fit(samples,spriteWorldVertical(tableCamera,table.up),eye,rotate(camera.orientation,{1,0,0}));
                                        if(spriteAdditive&&fitted) {
                                            // 0.9.34: a flame stood upright next to its torch / brazier crossed the wall behind it and was cut by a straight edge (report 232249):
                                            // the plane is moved toward the eye by half its width (same picture from the eye, no longer inside the wall)
                                            Vec3 pull{};
                                            if(effectPull>0) {float u0=1e9f,u1=-1e9f;for(const auto& q:samples){u0=std::min(u0,q.u);u1=std::max(u1,q.u);}
                                                Vec3 tw=sub(eye,frame.bottom);const float tl=std::sqrt(tw.x*tw.x+tw.y*tw.y+tw.z*tw.z);const float amount=effectPull*frame.scaleU*(u1-u0);
                                                if(tl>amount*2.f&&std::isfinite(amount)) pull=scale(tw,amount/tl);}
                                            // 0.9.40: the damage numbers were MIRRORED (report 083854: '200' read as a mirrored '002'): their texture runs right-to-left
                                            // on the game screen. With EffectsScreenAxes the piece is laid out as on the monitor around its centre (same centre as 0.9.38).
                                            bool laid=false;
                                            if(effectScreenAxes&&effectAnchorCenter&&(s1-s0)>=3) {
                                                float mx=0,my=0;Vec3 c0{};for(size_t j=s0;j<s1;++j){const UINT i=bbOrder[j];float sv[2];memcpy(sv,input+i*stride,8);mx+=sv[0];my+=sv[1];c0=add(c0,positions[i]);}
                                                const float nn=float(s1-s0);mx/=nn;my/=nn;c0=scale(c0,1.f/nn);float bestS=0,bestW=0;
                                                for(size_t a=s0;a<s1;++a)for(size_t b=a+1;b<s1;++b){float A[2],B[2];memcpy(A,input+bbOrder[a]*stride,8);memcpy(B,input+bbOrder[b]*stride,8);
                                                    const float ds=std::hypot(A[0]-B[0],A[1]-B[1]);if(ds>bestS){const Vec3 dw=sub(positions[bbOrder[a]],positions[bbOrder[b]]);bestS=ds;bestW=std::sqrt(dw.x*dw.x+dw.y*dw.y+dw.z*dw.z);}}
                                                if(bestS>1.f&&bestW>0&&std::isfinite(bestW)) {const float pw=bestW/bestS;
                                                    static thread_local std::vector<Vec3> laidPos;laidPos.resize(s1-s0);
                                                    for(size_t j=s0;j<s1;++j){const UINT i=bbOrder[j];float sv[2];memcpy(sv,input+i*stride,8);laidPos[j-s0]=add(add(c0,add(scale(frame.right,(sv[0]-mx)*pw),scale(frame.up,-(sv[1]-my)*pw))),pull);}
                                                    for(size_t j=s0;j<s1;++j) positions[bbOrder[j]]=laidPos[j-s0];
                                                    laid=true;}
                                            }
                                            // 0.9.41: damage numbers 'upside down' (report 083854, '200' seen as an upside-down '002'): on the game screen their texture
                                            // runs right-to-left AND bottom-to-top (turned 180 degrees). Such a piece (u against screen x, v against screen y) is
                                            // rebuilt with u and v reversed about their centre; every other piece is unchanged (0.9.38 path, flames validated).
                                            bool flip=false;
                                            if(!laid&&effectUpsideFix&&(s1-s0)>=3) {double su=0,sv=0,sx=0,sy=0,suu=0,svv=0,sxx=0,syy=0,sux=0,svy=0;const double nn=double(s1-s0);
                                                for(size_t j=s0;j<s1;++j){const UINT i=bbOrder[j];float uv[2],sp[2];memcpy(uv,input+i*stride+texOffset,8);memcpy(sp,input+i*stride,8);
                                                    su+=uv[0];sv+=uv[1];sx+=sp[0];sy+=sp[1];suu+=double(uv[0])*uv[0];svv+=double(uv[1])*uv[1];sxx+=double(sp[0])*sp[0];syy+=double(sp[1])*sp[1];sux+=double(uv[0])*sp[0];svy+=double(uv[1])*sp[1];}
                                                const double cux=sux-su*sx/nn,cvy=svy-sv*sy/nn,vu=suu-su*su/nn,vv=svv-sv*sv/nn,vx=sxx-sx*sx/nn,vy=syy-sy*sy/nn;
                                                if(vu>0&&vv>0&&vx>0&&vy>0) {const double ru=cux/std::sqrt(vu*vx),rv=cvy/std::sqrt(vv*vy);flip=ru<-.9&&rv<-.9;}
                                                if(flip) ++statUpsideFixed;}
                                            if(!laid) for(size_t j=s0;j<s1;++j){const UINT i=bbOrder[j];float uv[2];memcpy(uv,input+i*stride+texOffset,8);
                                                if(flip){uv[0]=2.f*frame.midU-uv[0];uv[1]=2.f*frame.midV-uv[1];}
                                                if(digitCand&&digitMirror) uv[0]=2.f*frame.midU-uv[0];   // 0.9.47: report 112157: digits seen mirrored
                                                positions[i]=add(frame.vertex(uv[0],uv[1],(digitCand&&!above)||!effectAnchorCenter),pull);}
                                            if(digitCand&&digitMirror&&!laid) {
                                                digitPcs.push_back({s0,s1,(sx0+sx1)*.5f,(sy0+sy1)*.5f,frame.right});
                                                // 0.9.47.21: the piece is kept for the whole frame (digitFrame), a number can come as several draws (report 235536: '20' read '02')
                                                digitnum::Piece dp;dp.sx=(sx0+sx1)*.5f;dp.sy=(sy0+sy1)*.5f;dp.right=frame.right;Vec3 dc{0,0,0};
                                                for(size_t j=s0;j<s1;++j) {dp.ids.push_back(uint32_t(bbOrder[j]));dc=add(dc,positions[bbOrder[j]]);}
                                                dp.centre=scale(dc,1.f/float(s1-s0));pendingDigits.push_back(std::move(dp));
                                            }   // 0.9.37: centre anchored (the game's particle point), see effectAnchorCenter
                                            ++statBillboards;uprightEffectDraw=true;
                                        } else if(!spriteAdditive) {
                                            ReplaySprite replay;replay.up=spriteWorldVertical(tableCamera,table.up);replay.hasAxes=fitted;
                                            replay.cx=(sx0+sx1)*.5f;replay.cy=(sy0+sy1)*.5f;replay.width=sx1-sx0;replay.height=sy1-sy0;replay.rhw=rmin;
                                            if(fitted){replay.originalCenter=frame.center;replay.sourceRight=frame.sourceRight;replay.sourceDown=frame.sourceDown;}
                                            for(size_t j=s0;j<s1;++j){const UINT i=bbOrder[j];float uv[2];memcpy(uv,input+i*stride+texOffset,8);replay.corners.push_back({i,uv[0],uv[1],positions[i]});}
                                            replay.noJoin=glyphCellRow(replay.corners);if(replay.noJoin) ++statNumberSprites;   // 0.9.47.33
                                            if(iconScreenAxes&&replay.corners.size()>=3) {   // 0.9.36: screen layout of the piece + world units per pixel
                                                float mx=0,my=0;for(auto& c:replay.corners){float sv[2];memcpy(sv,input+c.index*stride,8);c.sx=sv[0];c.sy=sv[1];mx+=sv[0];my+=sv[1];}
                                                mx/=float(replay.corners.size());my/=float(replay.corners.size());float bestS=0,bestW=0;
                                                for(auto& c:replay.corners){c.sx-=mx;c.sy-=my;}
                                                for(size_t a=0;a<replay.corners.size();++a)for(size_t b=a+1;b<replay.corners.size();++b){const auto& A=replay.corners[a];const auto& B=replay.corners[b];
                                                    const float ds=std::hypot(A.sx-B.sx,A.sy-B.sy);if(ds>bestS){const Vec3 dw=sub(A.original,B.original);bestS=ds;bestW=std::sqrt(dw.x*dw.x+dw.y*dw.y+dw.z*dw.z);}}
                                                if(bestS>1.f&&bestW>0&&std::isfinite(bestW)){replay.screenAxes=true;replay.pixelWorld=bestW/bestS;}
                                            }
                                            drawSprites.push_back(std::move(replay));++statBillboards;
                                        }
                                        }   // !digitHandled

                                    }
                                }
                            }
                        }
                        s0=s1;
                    }
                    // 0.9.47.21: the reflection of a whole number is done per FRAME (digitFrame.addDraw after the record is pushed), not per draw: see src/common/DigitNumber.hpp
                }
            }
        }
        // 0.9.35 F11 DIAGNOSTIC (forge): every blended 3D draw of the captured frame with its depth states and screen box. The forge plates are not sprites
        // (F9 test of 0.9.34: effects left flat, plates still there): this tells which draws they are and how their depth differs from the wall.
        if(spriteCensus>0&&blendCensusLogs<900&&count>=3) {
            DWORD bAb=0;d->GetRenderState(D3DRS_ALPHABLENDENABLE,&bAb);DWORD bAt=0;d->GetRenderState(D3DRS_ALPHATESTENABLE,&bAt);
            if(bAb||bAt) {
                DWORD bSb=0,bDb=0,bZf=0,bZw=0,bZe=0,bBias=0,bSlope=0,bCull=0;d->GetRenderState(D3DRS_SRCBLEND,&bSb);d->GetRenderState(D3DRS_DESTBLEND,&bDb);d->GetRenderState(D3DRS_ZFUNC,&bZf);
                d->GetRenderState(D3DRS_ZWRITEENABLE,&bZw);d->GetRenderState(D3DRS_ZENABLE,&bZe);d->GetRenderState(D3DRS_DEPTHBIAS,&bBias);d->GetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,&bSlope);d->GetRenderState(D3DRS_CULLMODE,&bCull);
                float x0=1e9f,x1=-1e9f,y0=1e9f,y1=-1e9f,z0=1e9f,z1=-1e9f,w0=1e9f,w1=-1e9f;
                for(UINT i=0;i<count;++i){float v[4];memcpy(v,input+i*stride,16);x0=std::min(x0,v[0]);x1=std::max(x1,v[0]);y0=std::min(y0,v[1]);y1=std::max(y1,v[1]);z0=std::min(z0,v[2]);z1=std::max(z1,v[2]);w0=std::min(w0,v[3]);w1=std::max(w1,v[3]);}
                ComPtr<IDirect3DBaseTexture9> bt;d->GetTexture(0,&bt);D3DSURFACE_DESC bsd{};if(bt&&bt->GetType()==D3DRTYPE_TEXTURE) static_cast<IDirect3DTexture9*>(bt.Get())->GetLevelDesc(0,&bsd);
                float zq=0;{const float zz=(z0+z1)*.5f,ww=(w0+w1)*.5f;zq=ww>0?zz/ww:0;}
                float biasF=0,slopeF=0;memcpy(&biasF,&bBias,4);memcpy(&slopeF,&bSlope,4);
                const int oid=EngineOrigin::enabled?EngineOrigin::idOf(input):0;const uint32_t oret=(oid>0&&oid<256)?EngineOrigin::origins[oid].ret0-uint32_t(EngineOrigin::k):0;
                ++blendCensusLogs;
                log("BLEND F11: %u v %u prim fvf %lx stride %u | box %.0f,%.0f-%.0f,%.0f | z %.5f..%.5f rhw %.4f..%.4f (z*w %.4f) | blend %lu src %lu dst %lu atest %lu zen %lu zfunc %lu zwrite %lu bias %g slope %g cull %lu | tex %ux%u fmt %d usage %lx | origin %u %06X | sprites %u upright %d",
                    unsigned(count),unsigned(primitives),(unsigned long)fvf,unsigned(stride),x0,y0,x1,y1,z0,z1,w0,w1,zq,(unsigned long)bAb,(unsigned long)bSb,(unsigned long)bDb,(unsigned long)bAt,(unsigned long)bZe,(unsigned long)bZf,(unsigned long)bZw,biasF,slopeF,(unsigned long)bCull,
                    bsd.Width,bsd.Height,int(bsd.Format),(unsigned long)bsd.Usage,unsigned(oid),unsigned(oret),unsigned(drawSprites.size()),int(uprightEffectDraw));
            }
        }
        RecProf::lap(2,rpMark);
        if(pickActive&&anchored&&!effectDraw) pickDraw(type,primitives,count,indices,indexFormat);
        UINT targetStride=stride-4;
        if(batched||decoupledFrame) {
            DrawRecord rec{type,primitives,count,targetStride,(fvf&~D3DFVF_POSITION_MASK)|D3DFVF_XYZ,arena.size(),indexArena.size(),indexFormat,indices!=nullptr,0};
            rec.sprites=std::move(drawSprites);
            rec.view=uint8_t(drawViewNow);
            arena.resize(arena.size()+size_t(count)*targetStride);
            unsigned char* out=arena.data()+rec.vertices;
            for(UINT i=0;i<count;++i) {memcpy(out+i*targetStride,&positions[i],12);memcpy(out+i*targetStride+12,input+i*stride+16,stride-16);}
            if(indices) {
                size_t bytes=size_t(indexCountFor(type,primitives))*(indexFormat==D3DFMT_INDEX32?4:2);
                indexArena.resize(indexArena.size()+bytes);memcpy(indexArena.data()+rec.indices,indices,bytes);
            }
            RecProf::lap(3,rpMark);
            Snapshot current{};
            if(!current.capture(d)) {current.release();++rejected;return;}
            RecProf::lap(4,rpMark);
            static_assert(Snapshot::renderStates[4]==D3DRS_ALPHABLENDENABLE,"render state 4 of a snapshot is ALPHABLENDENABLE");
            static_assert(Snapshot::renderStates[0]==D3DRS_ZWRITEENABLE,"render state 0 of a snapshot is ZWRITEENABLE");
            if(effectDraw) {current.rs[0]=FALSE;rec.overlay=true;}
            if(digitsLast&&digitDraw&&!effectDraw&&rec.sprites.empty()) {rec.top=true;++statDigitsLast;}   // 0.9.47.6
            if(floorDecalNoDepth&&decalPieces>0&&otherPieces==0&&spAb&&!spriteAdditive&&!effectDraw&&current.rs[0]) {current.rs[0]=FALSE;++statDecalNoDepth;}   // 0.9.30: drawn in order, tested against the floor, no longer fighting each other
            // 0.9.31: transparent smoke / fume plates seen in perspective (forge, brazier) wrote depth and cut the meshes behind them (report 210937): drawn in order, depth-tested, not written
            if(softNoDepth&&softPieces>0&&rec.sprites.empty()&&spAb&&!effectDraw&&current.rs[0]) {current.rs[0]=FALSE;++statSoftNoDepth;}
            // 0.9.36: every transparent draw (smoke, glows, fumes, sparks: not icons) is replayed without writing depth, and after all opaque geometry (Decoupled
            // OpaqueFirst). Report 004324: the depth of the blended draws is exact (z linear in 1/w for all 498 of them), but a smoke quad drawn BEFORE the forge
            // wall wrote depth and the wall seen from the headset was rejected inside the quad: rectangles of 'missing decor' around the brazier.
            if(translucentNoDepth&&current.rs[4]&&rec.sprites.empty()&&!effectDraw&&current.rs[0]) {current.rs[0]=FALSE;++statTranslucentNoDepth;}
            // 0.9.44: report 094406 (forge with smoke): the smoke puffs are small alpha quads recognised as sprites (rec.sprites non-empty), so the rule above skipped them;
            // they wrote depth (zwrite 1, src 5 dst 6, 28 puffs) and the wall / arch seen from the headset was rejected inside each quad. Replayed without depth write too.
            if(spriteNoDepth&&current.rs[4]&&!rec.sprites.empty()&&!effectDraw&&current.rs[0]) {
                // 0.9.44.1: report 095947 - turning depth write off for the creature icons / numbers made them lie flat again. Smoke puffs use a 2x2 atlas
                // (texture cells ~0.5 wide), the icons a 4x4 atlas (cells ~0.25 wide): only the large-cell textures lose their depth write.
                const UINT uvOff=16u+((fvf&D3DFVF_DIFFUSE)?4u:0u)+((fvf&D3DFVF_SPECULAR)?4u:0u);
                float cell=0;
                if((fvf&D3DFVF_TEXCOUNT_MASK)>=D3DFVF_TEX1&&uvOff+8<=stride) {
                    float u0=1e9f,u1=-1e9f;for(UINT i=0;i<count;++i) {float uv[2];memcpy(uv,input+i*stride+uvOff,8);u0=std::min(u0,uv[0]);u1=std::max(u1,uv[0]);}
                    cell=u1-u0;
                }
                if(cell>=.4f) {current.rs[0]=FALSE;++statSpriteNoDepth;}
            }
            // 0.9.27: a glow / smoke sprite stood upright must not write depth: lying flat it hid nothing, standing it hid the walls behind it (report 144911).
            if(uprightEffectDraw&&uprightEffectNoDepth&&rec.sprites.empty()) {if(current.rs[0]) ++statUprightNoDepth;current.rs[0]=FALSE;}   // 0.9.30: never for a draw that also carries icons / digits   // 0.8.99: Snapshot::renderStates[0] = ZWRITEENABLE: an effect never hides what is drawn after it
            // 0.9.40 F11 DIAGNOSTIC: full geometry of the captured frame (every 3D draw: screen + world vertices, UV, colour, triangles, blend and depth states)
            // in Captures\DK2VR_geom_NNN.csv, so the forge can be rebuilt offline from the headset's point of view (what the plates really are).
            if(spriteCensus==2&&geomDumps<6) {
                if(!geomFile) {std::error_code gec;auto folder=configurationPath().parent_path()/L"Captures";std::filesystem::create_directories(folder,gec);
                    wchar_t gn[64];swprintf(gn,64,L"DK2VR_geom_%03u.csv",geomDumps+1);geomFile=_wfopen((folder/gn).c_str(),L"w");geomDraw=0;
                    if(geomFile){const Vec3 ge=scale(add(request.eyes[0].pose.position,request.eyes[1].pose.position),.5f);
                        fprintf(geomFile,"E,%.5f,%.5f,%.5f,%u,%u,%.5f\n",ge.x,ge.y,ge.z,uiW,uiH,units);
                        const Vec3 tu=spriteWorldVertical(tableCamera,table.up);fprintf(geomFile,"U,%.5f,%.5f,%.5f\n",tu.x,tu.y,tu.z);}}
                if(geomFile) {
                    const UINT texOff=16u+((fvf&D3DFVF_DIFFUSE)?4u:0u)+((fvf&D3DFVF_SPECULAR)?4u:0u);const bool hasUv=(fvf&D3DFVF_TEXCOUNT_MASK)>=D3DFVF_TEX1&&texOff+8<=stride;
                    const int gid=EngineOrigin::enabled?EngineOrigin::idOf(input):0;const uint32_t gret=(gid>0&&gid<256)?EngineOrigin::origins[gid].ret0-uint32_t(EngineOrigin::k):0;
                    fprintf(geomFile,"D,%u,%u,%u,%d,%lu,%lu,%lu,%lu,%lu,%06X,%u,%d\n",geomDraw,unsigned(count),unsigned(primitives),int(type),(unsigned long)current.rs[4],(unsigned long)current.rs[5],(unsigned long)current.rs[6],(unsigned long)current.rs[0],(unsigned long)current.rs[1],unsigned(gret),unsigned(rec.sprites.size()),int(uprightEffectDraw));
                    for(UINT i=0;i<count;++i){float v[4];memcpy(v,input+i*stride,16);float uv[2]{};if(hasUv)memcpy(uv,input+i*stride+texOff,8);uint32_t col=0;if(fvf&D3DFVF_DIFFUSE)memcpy(&col,input+i*stride+16,4);
                        fprintf(geomFile,"V,%.1f,%.1f,%.5f,%.5f,%.5f,%.5f,%.4f,%.4f,%08X\n",v[0],v[1],v[3],positions[i].x,positions[i].y,positions[i].z,uv[0],uv[1],col);}
                    auto gidx=[&](UINT j)->UINT{if(!indices) return j;return indexFormat==D3DFMT_INDEX32?UINT(((const uint32_t*)indices)[j]):UINT(((const uint16_t*)indices)[j]);};
                    for(UINT k=0;k<primitives;++k){UINT a,b,c;
                        if(type==D3DPT_TRIANGLELIST){a=gidx(3*k);b=gidx(3*k+1);c=gidx(3*k+2);} else if(type==D3DPT_TRIANGLESTRIP){a=gidx(k);b=gidx(k+1);c=gidx(k+2);} else if(type==D3DPT_TRIANGLEFAN){a=gidx(0);b=gidx(k+1);c=gidx(k+2);} else break;
                        fprintf(geomFile,"T,%u,%u,%u\n",a,b,c);}
                    ++geomDraw;
                }
            }
            // Measure native opaque surface coverage, not texture count or GPU readback.
            // Particles/torches alone must not turn an incomplete dungeon into a valid frame.
            if(possessed&&!current.rs[4]&&drawViewNow==0&&!effectDraw) {
                auto at=[&](UINT n)->UINT {return !indices?n:indexFormat==D3DFMT_INDEX32?static_cast<const uint32_t*>(indices)[n]:static_cast<const uint16_t*>(indices)[n];};
                const float denom=std::max(1.f,4.f*lens.centerX*lens.centerY);
                for(UINT t=0;t<primitives;++t) {
                    UINT a,b,c;
                    if(type==D3DPT_TRIANGLELIST){a=at(t*3);b=at(t*3+1);c=at(t*3+2);}
                    else if(type==D3DPT_TRIANGLESTRIP){a=at(t);b=at(t+1);c=at(t+2);}
                    else if(type==D3DPT_TRIANGLEFAN){a=at(0);b=at(t+1);c=at(t+2);}
                    else break;
                    if(a>=count||b>=count||c>=count) continue;
                    float av[2],bv[2],cv[2];memcpy(av,input+a*stride,8);memcpy(bv,input+b*stride,8);memcpy(cv,input+c*stride,8);
                    possessionCoverage+=std::min(1.f,projectedTriangleArea(av[0],av[1],bv[0],bv[1],cv[0],cv[1])/denom);
                }
            }
            std::array<uint32_t,4> ids{};
            if(decoupledFrame) {
                // Up-to-date shared copies of the bound textures (copied now if new or changed by the game).
                CopyCache::Guard guard;guard.d=d;guard.cache=&copies;
                for(int i=0;i<Snapshot::Samplers;++i) ids[i]=copies.copyFor(d,guard,current.texture[i]);
                ids[3]=shaders.idFor(current.pixel);
                // 0.9.41 F11: which texture each draw of the frame uses (game texture, its watch version, the copy replayed in the headset); the textures of
                // the transparent draws are saved twice, as the GAME has them and as the HEADSET COPY has them (DK2VR_tex_NNN_<id>_jeu/_copie.bmp).
                if(spriteCensus==2&&geomFile&&geomDraw>0) {
                    IDirect3DBaseTexture9* t0=current.texture[0];
                    fprintf(geomFile,"X,%u,%p,%u,%u\n",geomDraw-1,(void*)t0,ids[0],t0?TextureWatch::watch(t0):0u);
                    if(t0&&current.rs[4]&&t0->GetType()==D3DRTYPE_TEXTURE&&texDumped.size()<48&&texDumped.insert(t0).second) dumpTexturePair(d,static_cast<IDirect3DTexture9*>(t0),ids[0]);
                }
            }
            RecProf::lap(5,rpMark);
            if(!snapshots.empty()&&snapshots.back().same(current)&&snapIds.back()==ids) {current.release();rec.snapshot=uint32_t(snapshots.size()-1);}
            else {snapshots.push_back(current);snapIds.push_back(ids);rec.snapshot=uint32_t(snapshots.size()-1);}
            if(drawViewNow==0&&!effectDraw) classifyHand(d,rec,input,stride,count);
            RecProf::lap(6,rpMark);
            if(rec.sprites.empty()&&!records.empty()&&records.back().sprites.empty()&&records.back().view==rec.view&&records.back().overlay==rec.overlay&&mergeDraw(records.back(),rec,indexArena.data(),D3DPT_TRIANGLELIST,D3DFMT_INDEX16)) ++mergedDraws;
            else records.push_back(rec);
            if(!pendingDigits.empty()) {digitFrame.addDraw(arena,rec.vertices,targetStride,pendingDigits);{static DWORD dgW=0;static int dgN=0;if(DWORD(GetTickCount()-dgW)>5000){dgW=GetTickCount();dgN=0;}if(!digitFrame.lastDiag.empty()&&digitFrame.lastDiagM>=2&&dgN<4){++dgN;log("Digit layout (0.9.47.29): %s",digitFrame.lastDiag.c_str());}}   // 0.9.47.21
                static DWORD digitGroupLast=0;if(digitGroupLogs<300&&digitFrame.lastGroups<int(digitFrame.pieces.size())&&DWORD(GetTickCount()-digitGroupLast)>=400) {digitGroupLast=GetTickCount();++digitGroupLogs;   // 0.9.47.27: one line per 400 ms (the 120 first lines of 0.9.47.26 were all used by sparks in the first 10 s)   // 0.9.47.26: log numbers made of several digits: pieces in the order they were drawn, with their glyph cell and screen place
                    std::string line;for(size_t i=0;i<digitFrame.pieces.size();++i){char b[96];snprintf(b,sizeof b," [g%d %.0f,%.0f %.0fpx cell %.2f,%.2f]",digitFrame.lastGroup[i],digitFrame.pieces[i].sx,digitFrame.pieces[i].sy,digitFrame.pieces[i].ext,digitFrame.pieces[i].cu,digitFrame.pieces[i].cv);if(line.size()<900)line+=b;}
                    log("Digit numbers (0.9.47.26) #%d: %u pieces in %d groups (reach %.1f x size, world reach %.1f):%s",digitGroupLogs,unsigned(digitFrame.pieces.size()),digitFrame.lastGroups,digitFrame.reach,digitFrame.worldReach,line.c_str());}}
            ++world;statsVertices+=count;
            if(decoupledFrame&&pendingPackage&&(records.size()&63)==0) pollPackage(false);
            RecProf::lap(7,rpMark);
            return;
        }
        converted.resize(size_t(count)*targetStride);
        for(UINT i=0;i<count;++i) memcpy(converted.data()+i*targetStride+12,input+i*stride+16,stride-16);
        ComPtr<IDirect3DSurface9> rt,ds;D3DVIEWPORT9 vp{};
        if(FAILED(d->GetRenderTarget(0,&rt))||FAILED(d->GetViewport(&vp))) {++rejected;return;}
        if(fastStates?!saved.capture(d):FAILED(state->Capture())) {++rejected;return;}
        d->GetDepthStencilSurface(&ds);
        auto& im=images[slot];
        D3DMATRIX identity{};identity._11=identity._22=identity._33=identity._44=1;
        d->SetVertexShader(nullptr); d->SetFVF((fvf&~D3DFVF_POSITION_MASK)|D3DFVF_XYZ);
        d->SetTransform(D3DTS_WORLD,&identity);d->SetTransform(D3DTS_VIEW,&identity);
        d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
        d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);
        d->SetRenderState(D3DRS_CLIPPING,TRUE);
        d->SetRenderState(D3DRS_FOGENABLE,FALSE);
        // Reconstructed XYZ now uses our standard near=0 / far=1 projection.
        // Native RHW draws may use reciprocal/reversed depth and a GREATER test.
        d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
        d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
        d->SetRenderState(D3DRS_DEPTHBIAS,0);
        d->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,0);
        const bool clip=clipping();
        std::array<Plane,4> planes{};if(clip) planes=table.planes();
        bool success=true;
        for(int e=0;e<2;++e) {
            // Each eye has its own position, orientation, and asymmetric projection.
            for(UINT i=0;i<count;++i) {auto p=toLocal(request.eyes[e].pose,positions[i]);memcpy(converted.data()+i*targetStride,&p,12);}
            D3DMATRIX proj{};projectionMatrix(request.eyes[e].fov,.015f,200.f,&proj._11);
            d->SetTransform(D3DTS_PROJECTION,&proj);
            d->SetDepthStencilSurface(nullptr);d->SetRenderTarget(0,im.target[e].Get());d->SetDepthStencilSurface(im.depth[e].Get());
            D3DVIEWPORT9 eyeVp{0,0,im.w,im.h,0,1};d->SetViewport(&eyeVp);
            if(clip) {
                for(DWORD i=0;i<4;++i) {
                    auto plane=transformPlane(planes[i],tableCamera,request.eyes[e].pose);
                    float coefficients[4]={plane.normal.x,plane.normal.y,plane.normal.z,plane.offset};
                    d->SetClipPlane(i,coefficients);
                }
                d->SetRenderState(D3DRS_CLIPPLANEENABLE,15);
            }
            HRESULT hr=indices?d->DrawIndexedPrimitiveUP(type,0,count,primitives,indices,indexFormat,converted.data(),targetStride):
                d->DrawPrimitiveUP(type,primitives,converted.data(),targetStride);
            success=success&&SUCCEEDED(hr);
        }
        d->SetDepthStencilSurface(nullptr);d->SetRenderTarget(0,rt.Get());d->SetDepthStencilSurface(ds.Get());
        if(fastStates) saved.apply(d); else state->Apply();
        d->SetViewport(&vp);
        if(success) ++world; else ++rejected;
        statsVertices+=count;
    }
    void replay(IDirect3DDevice9* d) {
        if(records.empty()) return;
        double started=milliseconds();
        ComPtr<IDirect3DSurface9> rt,ds;D3DVIEWPORT9 vp{};
        if(FAILED(state->Capture())||FAILED(d->GetRenderTarget(0,&rt))||FAILED(d->GetViewport(&vp))) {clearBatch();return;}
        d->GetDepthStencilSurface(&ds);
        D3DMATRIX identity{};identity._11=identity._22=identity._33=identity._44=1;
        d->SetVertexShader(nullptr);
        d->SetTransform(D3DTS_WORLD,&identity);d->SetTransform(D3DTS_VIEW,&identity);
        d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
        d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);d->SetRenderState(D3DRS_CLIPPING,TRUE);d->SetRenderState(D3DRS_FOGENABLE,FALSE);
        d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
        d->SetRenderState(D3DRS_DEPTHBIAS,0);d->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,0);
        const bool clip=clipping();std::array<Plane,4> planes{};if(clip) planes=table.planes();
        auto& im=images[slot];
        eyeArena.resize(arena.size());
        for(int e=0;e<2;++e) {
            // One vertex transform pass per eye over the whole frame.
            memcpy(eyeArena.data(),arena.data(),arena.size());
            for(auto& rec:records) {
                unsigned char* v=eyeArena.data()+rec.vertices;
                for(UINT i=0;i<rec.count;++i) {Vec3 p;memcpy(&p,v+i*rec.stride,12);p=toLocal(request.eyes[e].pose,p);memcpy(v+i*rec.stride,&p,12);}
            }
            D3DMATRIX proj{};projectionMatrix(request.eyes[e].fov,.015f,200.f,&proj._11);d->SetTransform(D3DTS_PROJECTION,&proj);
            d->SetDepthStencilSurface(nullptr);d->SetRenderTarget(0,im.target[e].Get());d->SetDepthStencilSurface(im.depth[e].Get());
            D3DVIEWPORT9 eyeVp{0,0,im.w,im.h,0,1};d->SetViewport(&eyeVp);
            if(clip) {
                for(DWORD i=0;i<4;++i) {
                    auto plane=transformPlane(planes[i],tableCamera,request.eyes[e].pose);
                    float coefficients[4]={plane.normal.x,plane.normal.y,plane.normal.z,plane.offset};
                    d->SetClipPlane(i,coefficients);
                }
                d->SetRenderState(D3DRS_CLIPPLANEENABLE,15);
            } else d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);
            if(FAILED(d->BeginScene())) {++rejected;continue;}
            drawBaseFirst(d,e);
            d->SetRenderState(D3DRS_CLIPPLANEENABLE,clip?15:0);
            uint32_t applied=UINT32_MAX;DWORD fvf=0;
            for(auto& rec:records) {
                if(rec.snapshot!=applied) {
                    const Snapshot* previous=applied==UINT32_MAX?nullptr:&snapshots[applied];
                    avoidedReplayStates+=snapshots[rec.snapshot].applyDelta(d,previous);applied=rec.snapshot;
                }
                if(rec.fvf!=fvf) {d->SetFVF(rec.fvf);fvf=rec.fvf;}
                HRESULT hr=rec.indexed?d->DrawIndexedPrimitiveUP(rec.type,0,rec.count,rec.primitives,indexArena.data()+rec.indices,rec.indexFormat,eyeArena.data()+rec.vertices,rec.stride):
                    d->DrawPrimitiveUP(rec.type,rec.primitives,eyeArena.data()+rec.vertices,rec.stride);
                if(FAILED(hr)) ++rejected;
            }
            d->EndScene();
        }
        d->SetDepthStencilSurface(nullptr);d->SetRenderTarget(0,rt.Get());d->SetDepthStencilSurface(ds.Get());
        state->Apply();d->SetViewport(&vp);
        clearBatch();HeldGeometry::depths.clear();
        replayMilliseconds=milliseconds()-started;
    }
    // Table base, rim and controller markers, drawn once per frame at Present.
    // 0.8.92: the four table edges cut the dungeon open (rooms and corridors seen from the side, empty). A brown face, the colour of the table's wood,
    // closes each cut from below the deepest floor up to just under the earth surface: the dungeon looks like a solid block sawn at the table edges.
    void buildSkirt(std::vector<OverlayVertex>& mesh) {
        if(!clipping()||!haveLevels||activeUnits<=0||!table.valid) return;
        const float walk=haveWalk?walkOffset:0.f;
        const float top=(highOffset-walk-skirtTopGap)/activeUnits,bottom=(std::min(lowOffset,0.f)-walk-.25f)/activeUnits;
        if(!(top>bottom)||top-bottom>4.f) return;
        const float w=table.halfWidth,z=table.halfDepth;
        auto p=[&](float x,float depth,float y){return toWorld(tableCamera,table.point(x,depth,y));};
        auto quad=[&](Vec3 a,Vec3 b,Vec3 c,Vec3 f,DWORD color){for(auto v:{a,b,c,a,c,f}) mesh.push_back({v,color});};
        quad(p(-w,-z,bottom),p(w,-z,bottom),p(w,-z,top),p(-w,-z,top),0xff4e321e);
        quad(p(-w,z,bottom),p(w,z,bottom),p(w,z,top),p(-w,z,top),0xff4e321e);
        quad(p(-w,-z,bottom),p(-w,z,bottom),p(-w,z,top),p(-w,-z,top),0xff452c1a);
        quad(p(w,-z,bottom),p(w,z,bottom),p(w,z,top),p(w,-z,top),0xff452c1a);
    }
    void buildBase(std::vector<OverlayVertex>& mesh) {
        if(!clipping()) return;
        float metric=10/activeUnits;
        float rim=.012f*metric,top=-.003f*metric,bottom=top-.025f*metric;
        float w=table.halfWidth,z=table.halfDepth;
        auto quad=[&](Vec3 a,Vec3 b,Vec3 c,Vec3 f,DWORD color) {
            for(auto p:{a,b,c,a,c,f}) mesh.push_back({toWorld(tableCamera,p),color});
        };
        auto p=[&](float x,float depth,float y){return table.point(x,depth,y);};
        // 0.4.0: with the wooden table, only a dark earth tray under the dungeon (the frame is drawn by the VR thread).
        if(environmentOn&&decoupledFrame) {quad(p(-w,-z,top),p(w,-z,top),p(w,z,top),p(-w,z,top),0xff15110d);return;}
        quad(p(-w,-z,top),p(w,-z,top),p(w,z,top),p(-w,z,top),0xff24272b);
        // A narrow rim makes the four limits readable even over water/void.
        for(int side:{-1,1}) {
            float x=side*w,xOuter=side*(w+rim),a=side*z,b=side*(z+rim);
            quad(p(x,-z,0),p(xOuter,-z,0),p(xOuter,z,0),p(x,z,0),0xff746653);
            quad(p(-w-rim,a,0),p(w+rim,a,0),p(w+rim,b,0),p(-w-rim,b,0),0xff746653);
            quad(p(xOuter,-z-rim,0),p(xOuter,z+rim,0),p(xOuter,z+rim,bottom),p(xOuter,-z-rim,bottom),0xff34383f);
            quad(p(-w-rim,b,0),p(w+rim,b,0),p(w+rim,b,bottom),p(-w-rim,b,bottom),0xff34383f);
        }
    }
    // The base is drawn before the dungeon, without depth: any dungeon geometry, even below
    // the floor level (dungeon heart pit, recessed floors, water), is drawn over it.
    void drawBaseFirst(IDirect3DDevice9* d,int eye) {
        std::vector<OverlayVertex> mesh;buildBase(mesh);
        if(mesh.empty()) return;
        for(auto& v:mesh) v.position=toLocal(request.eyes[eye].pose,v.position);
        d->SetPixelShader(nullptr);d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
        d->SetTexture(0,nullptr);d->SetTexture(1,nullptr);
        d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
        d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
        d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
        d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
        d->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
        d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);
        d->SetRenderState(D3DRS_COLORWRITEENABLE,15);
        d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(mesh.size()/3),mesh.data(),sizeof(OverlayVertex));
        d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
    }
    void buildPossessionWheel(std::vector<OverlayVertex>& mesh) const {
        if(!possessedFrame||!(request.possessionFlags&PossessionWheel)) return;
        const Quat yaw=yawOnly(request.eyes[0].pose.orientation);
        const Pose wheel{yaw,add(request.wheelPose.position,rotate(yaw,{0,.12f,-.12f}))};
        auto tri=[&](Vec3 a,Vec3 b,Vec3 c,DWORD color){mesh.push_back({toWorld(wheel,a),color});mesh.push_back({toWorld(wheel,b),color});mesh.push_back({toWorld(wheel,c),color});};
        auto rect=[&](float x,float y,float w,float h,DWORD color){Vec3 a{x-w*.5f,y+h*.5f,0},b{x+w*.5f,y+h*.5f,0},c{x+w*.5f,y-h*.5f,0},dd{x-w*.5f,y-h*.5f,0};tri(a,b,c,color);tri(a,c,dd,color);};
        static const unsigned char digitSegments[7]={0,0x06,0x5b,0x4f,0x66,0x6d,0x7d};
        for(int i=0;i<6;++i) {
            const float a0=-Pi+i*Pi/3.f,a1=a0+Pi/3.f,am=(a0+a1)*.5f;
            const DWORD color=request.possessionSkill==uint32_t(i+1)?0xffffbc38:0xff314558;
            for(int k=0;k<8;++k) {
                float a=a0+(a1-a0)*k/8.f,b=a0+(a1-a0)*(k+1)/8.f;
                Vec3 p{.075f*std::cos(a),.075f*std::sin(a),0},q{.205f*std::cos(a),.205f*std::sin(a),0};
                Vec3 r{.205f*std::cos(b),.205f*std::sin(b),0},s{.075f*std::cos(b),.075f*std::sin(b),0};
                tri(p,q,r,color);tri(p,r,s,color);
            }
            const float x=.14f*std::cos(am),y=.14f*std::sin(am),w=.026f,h=.041f,t=.006f;
            const unsigned char bits=digitSegments[i+1];const DWORD white=0xfff4f7ff;
            if(bits&1) rect(x,y+h*.5f-t*.5f,w,t,white);
            if(bits&2) rect(x+w*.5f-t*.5f,y+h*.25f,t,h*.5f,white);
            if(bits&4) rect(x+w*.5f-t*.5f,y-h*.25f,t,h*.5f,white);
            if(bits&8) rect(x,y-h*.5f+t*.5f,w,t,white);
            if(bits&16) rect(x-w*.5f+t*.5f,y-h*.25f,t,h*.5f,white);
            if(bits&32) rect(x-w*.5f+t*.5f,y+h*.25f,t,h*.5f,white);
            if(bits&64) rect(x,y,w,t,white);
        }
    }
    void drawOverlay(IDirect3DDevice9* d,bool includeBase) {
        if(!world) return;
        using Vertex=OverlayVertex;
        std::vector<Vertex> mesh;mesh.reserve(700);
        if(includeBase) buildBase(mesh);
        if(drawHands&&!possessedFrame) buildHands(request,mesh); // no red/blue controller rods in possession
        const uint32_t vignetteLevel=(request.comfortFlags&ComfortVignetteMask)>>ComfortVignetteShift;
        const bool vignette=false; // 0.8.33: host compositor owns possession vignette, after eye-image copying
        if(mesh.empty()&&!vignette) return;
        ComPtr<IDirect3DSurface9> rt,ds;D3DVIEWPORT9 vp{};
        if(FAILED(state->Capture())||FAILED(d->GetRenderTarget(0,&rt))||FAILED(d->GetViewport(&vp))) return;
        d->GetDepthStencilSurface(&ds);
        D3DMATRIX identity{};identity._11=identity._22=identity._33=identity._44=1;
        d->SetVertexShader(nullptr);d->SetPixelShader(nullptr);d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
        d->SetTransform(D3DTS_WORLD,&identity);d->SetTransform(D3DTS_VIEW,&identity);
        d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_FOGENABLE,FALSE);
        d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
        d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_CLIPPING,TRUE);
        d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);
        d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
        d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
        d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);d->SetRenderState(D3DRS_DEPTHBIAS,0);
        d->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,0);
        d->SetRenderState(D3DRS_COLORWRITEENABLE,15);
        d->SetTexture(0,nullptr);d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
        d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
        d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
        d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
        d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
        auto& im=images[slot];std::vector<Vertex> vertices(mesh.size());
        for(int e=0;e<2;++e) {
            for(size_t i=0;i<mesh.size();++i) vertices[i]={toLocal(request.eyes[e].pose,mesh[i].position),mesh[i].color};
            D3DMATRIX proj{};projectionMatrix(request.eyes[e].fov,.015f,200.f,&proj._11);d->SetTransform(D3DTS_PROJECTION,&proj);
            d->SetDepthStencilSurface(nullptr);d->SetRenderTarget(0,im.target[e].Get());d->SetDepthStencilSurface(im.depth[e].Get());
            D3DVIEWPORT9 eyeVp{0,0,im.w,im.h,0,1};d->SetViewport(&eyeVp);
            // Present is outside the game's BeginScene/EndScene pair.
            if(SUCCEEDED(d->BeginScene())) {
                d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
                d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
                d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
                if(!vertices.empty()) {HRESULT hr=d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(vertices.size()/3),vertices.data(),sizeof(Vertex));if(FAILED(hr)) ++rejected;}
                if(vignette) {
                    struct ScreenVertex {float x,y,z,rhw;DWORD color;};
                    std::vector<ScreenVertex> ring;ring.reserve(144);
                    const float cx=im.w*.5f,cy=im.h*.5f,inner=std::min(im.w,im.h)*(vignetteLevel==1?.38f:vignetteLevel==2?.30f:.22f),outer=std::sqrt(cx*cx+cy*cy)+2.f;
                    const DWORD edge=vignetteLevel==1?0x99000000:vignetteLevel==2?0xbb000000:0xdd000000;
                    auto v=[&](float a,float radius,DWORD color){return ScreenVertex{cx+std::cos(a)*radius,cy+std::sin(a)*radius,0.f,1.f,color};};
                    for(int k=0;k<48;++k) {float a=2*Pi*k/48.f,b=2*Pi*(k+1)/48.f;
                        auto p=v(a,inner,0x00000000),q=v(a,outer,edge),r=v(b,outer,edge),s=v(b,inner,0x00000000);
                        ring.insert(ring.end(),{p,q,r,p,r,s});}
                    d->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
                    d->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
                    d->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
                    if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(ring.size()/3),ring.data(),sizeof(ScreenVertex)))) ++rejected;
                    d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
                }
                d->EndScene();
            } else ++rejected;
        }
        d->SetDepthStencilSurface(nullptr);d->SetRenderTarget(0,rt.Get());d->SetDepthStencilSurface(ds.Get());
        state->Apply();d->SetViewport(&vp);
    }
    // 0.8.67: heading of the native first-person camera (same axis reading as NativeControl::heading, no table frame).
    bool possessionCameraYaw(float& yaw) const {
        float sx=native.read<float>(0x77f4cc),sy=native.read<float>(0x78093c),focal=native.read<float>(0x77f430);
        if(!(focal>1)) return false;
        sx/=focal;sy/=focal;
        Vec3 axis{native.read<float>(0x77f458)*sx,-native.read<float>(0x77f45c)*sy,-native.read<float>(0x77f460)};
        if(!normalize(axis)||axis.x*axis.x+axis.z*axis.z<1e-4f) return false;
        yaw=std::atan2(-axis.z,axis.x);return true;
    }
    void diagnostics() {
        // Native visibility inputs, logged for the culling study (read only).
        if(possessedFrame) {
            Vec3 head=rotate(request.eyes[0].pose.orientation,{0,0,-1});
            log("Possession VR view: head forward=(%.3f,%.3f,%.3f), native camera Z=(%.3f,%.3f,%.3f), focal factor=%.2f, input=%d, move=(%.2f,%.2f), skill=%u, buttons=%u",
                head.x,head.y,head.z,native.read<float>(0x77f470),native.read<float>(0x77f474),native.read<float>(0x77f478),
                possessionFovFactor,int(possession.active),request.possessionMoveX,request.possessionMoveY,request.possessionSkill,request.possessionFlags);
        }
        int32_t minimum=0,maximum=0,current=0;uint32_t viewDistance=0;float cullFrom=0,cullTo=0;
        if(auto* c=native.camera()) {memcpy(&minimum,c+0xd50,4);memcpy(&maximum,c+0xd54,4);memcpy(&current,c+0xd80,4);}
        if(native.supported) {
            // MyResources_instance 0x758340 + video settings 0x1738 + view_distance 0x50 (Flame mapping).
            viewDistance=native.read<uint32_t>(0x759ac8);
            cullFrom=native.read<float>(0x760aa8);cullTo=native.read<float>(0x760b4c);
        }
        int32_t pos[3]{};if(auto* c=native.camera()) memcpy(pos,c+0xd68,12);
        float camZ=native.supported?native.read<float>(0x760ac0):0;
        {Projection lp{};native.lens(lp);ScreenRect ra=native.renderArea(lp),fs=fullScreen(lp);
         log("Render area: engine %.1f,%.1f - %.1f,%.1f (screen %.0fx%.0f)",ra.left,ra.top,ra.right,ra.bottom,fs.right,fs.bottom);}
        log("Table levels: camera floor=%.2f (camState z=%.2f) raw low=%.2f high=%.2f -> offsets low=%+.2f walk=%+.2f high=%+.2f (%u samples, %u small frames ignored); PC screen images shown=%ld skipped=%ld",
            frameFloor,camZ,levelLow,levelHigh,lowOffset,walkOffset,highOffset,levelSamples,rejectedLevels,NonBlockingPresent::shown,NonBlockingPresent::skipped);rejectedLevels=0;
        InterlockedExchange(&NonBlockingPresent::shown,0);InterlockedExchange(&NonBlockingPresent::skipped,0);
        log("Native view: zoom cur=%d min=%d max=%d viewDistance=%u cullFrom=%.3f cullTo=%.3f anchored=%u visible=%.1fx%.1f units fallbacks=%llu target=(%.2f,%.2f) pans=%llu zooms=%llu screen=%ux%u",
            current,minimum,maximum,viewDistance,cullFrom,cullTo,anchored,anchorUnits.halfWidth*2,anchorUnits.halfDepth*2,(unsigned long long)anchorFallbacks,
            pos[0]/4096.,pos[1]/4096.,(unsigned long long)control.pans,(unsigned long long)control.zooms,images[0].dw?images[0].dw:images[1].dw,images[0].dh?images[0].dh:images[1].dh);
    }
    // Native timing counters (MyGameSession::tick, verified at 0x526AFB..0x526D3C):
    // world = game logic ticks, engine = camera + scene rendering.
    void gameCounters() {
        if(!native.supported) return;
        uint32_t wt=native.read<uint32_t>(0x70d43c),wc=native.read<uint32_t>(0x70d424),et=native.read<uint32_t>(0x70d45c),ec=native.read<uint32_t>(0x70d41c),eh=native.read<uint32_t>(0x70d460);
        if(haveCounters) {
            uint32_t dwt=wt-lastWorldTime,dwc=wc-lastWorldCount,det=et-lastEngineTime,dec=ec-lastEngineCount;
            log("Game (native counters, last 5 s): logic ticks=%u (%.1f/s) %.2f ms/tick | scene frames=%u (%.1f/s) %.2f ms/frame, worst=%u ms",
                dwc,dwc/5.,dwc?double(dwt)/dwc:0.,dec,dec/5.,dec?double(det)/dec:0.,eh);
        }
        lastWorldTime=wt;lastWorldCount=wc;lastEngineTime=et;lastEngineCount=ec;lastEngineHighest=eh;haveCounters=true;
        simWatch();
    }
    // 0.8.84.2 diagnostic: does the DUNGEON SIMULATION advance? The log counters above count calls of the session tick, which run even while the game is paused.
    // The game turn (+4 of the session object, incremented once per simulated turn unless paused: tick 0x526B9E) and the pause field (+0x284) are read directly;
    // the session object is the one the pause function 0x527270 was called on (known after the first pause) and, as a second candidate, the game object 0x795700.
    uint32_t simLastTurn[2]{0,0};bool simHave[2]{false,false};
    void simWatch() {
        if(!native.supported) return;
        void* cand[2]={const_cast<void*>(static_cast<volatile void*>(PauseWatch::session)),nullptr};
        const uint32_t g=native.read<uint32_t>(0x795700);cand[1]=reinterpret_cast<void*>(uintptr_t(g));
        const char* nm[2]={"session (pause call)","game object"};
        for(int i=0;i<2;++i) {
            auto* p=static_cast<unsigned char*>(cand[i]);
            if(!p||!readable(p+4,4)||!readable(p+0x280,8)) {simHave[i]=false;continue;}
            uint32_t turn=0,saved=0,pause=0;memcpy(&turn,p+4,4);memcpy(&saved,p+0x280,4);memcpy(&pause,p+0x284,4);
            log("Simulation watch (0.8.90, %s): turn %u (%+d in 5 s) | pause field %u (saved turn %u) | pause watch flag %ld | possession %d",nm[i],turn,simHave[i]?int(turn-simLastTurn[i]):0,pause,saved,long(PauseWatch::paused),int(LensHack::possessionNow()));
            simLastTurn[i]=turn;simHave[i]=true;
        }
    }
    // 0.8.25: a frame without any dungeon draw (anchored=false) used to switch the headset to the normal game path for a few frames (tilt restored,
    // shared images...) = the freeze when a creature is released. While the VR thread is running and the game is not in possession / menu, up to
    // 40 such frames are tolerated: the VR thread keeps showing the last package (frames without dungeon are dropped by presentDecoupled).
    void noteAnchored() {
        if(anchored) {lastAnchored=true;anchorLost=0;return;}
        const bool keep=vr&&vr->active&&lastAnchored&&!possessedFrame&&!native.frontend()&&!(mode==1||mode==2)&&anchorLost<40;
        if(keep) {if(++anchorLost==1&&hitchLogs<40) {++hitchLogs;log("Frame without dungeon draws while the table view is running: kept in headset mode (max 40 frames)");}return;}
        if(lastAnchored&&hitchLogs<40) {++hitchLogs;log("Table view left: anchored=0 for %d frames, possessedFrame=%d mode=%u frontend=%d",anchorLost,int(possessedFrame),unsigned(mode),int(native.frontend()));}
        lastAnchored=false;
    }
    // 0.9.25 PERF probe + automatic A/B of the GPU scheduling priorities (PerfTest=1). Game thread, once per game frame.
    perfab::GameTotals perfTotals;double perfLastMs=0,perfCycleStart=0,perfSettleUntil=0,perfPeriod=10000;
    int perfLastMode=-1,perfLastConfig=-1,perfAppliedGame=-100,perfPrioLogs=0;bool perfInit=false,perfAB=true;uint64_t perfLastBilan=0;
    void perfFrame(IDirect3DDevice9* d) {
        auto& S=perfab::shared();const double now=milliseconds();
        if(!perfInit) {
            perfInit=true;perfAB=iniInt(L"PerfTest",0)!=0;perfPeriod=1000.*iniFloat(L"PerfTestSeconds",10.f,4.f,120.f);
            S.fixedVr=std::clamp(iniInt(L"GpuPriorityVR",7),-7,7);S.fixedGame=std::clamp(iniInt(L"GpuPriorityGame",0),-7,7);
            perfCycleStart=now;S.enabled=perfAB;
            if(perfAB) log("PERF (0.9.25): automatic A/B test ON (PerfTest=1): every %.0f s the GPU priorities change between %s / %s / %s; results per scene in the BILAN PERF lines",perfPeriod/1000.,perfab::configs[0].name,perfab::configs[1].name,perfab::configs[2].name);
            else log("PERF (0.9.25): fixed GPU priorities: VR thread %+d, game %+d (PerfTest=0)",S.fixedVr.load(),S.fixedGame.load());
        }
        const int kind=decoupledFrame?decoupledKind:0;
        const int m=kind==3?0:kind==1?1:kind==2?2:3;
        const int c=perfAB?perfab::configAt(now-perfCycleStart,perfPeriod):0;
        const int wantGame=perfAB?perfab::configs[c].game:S.fixedGame.load();
        if(wantGame!=perfAppliedGame) {
            ComPtr<IDirect3DDevice9Ex> ex;HRESULT h=d->QueryInterface(IID_PPV_ARGS(&ex));
            if(SUCCEEDED(h)) h=ex->SetGPUThreadPriority(wantGame);
            perfAppliedGame=wantGame;
            if(perfPrioLogs<8) {++perfPrioLogs;log("PERF (0.9.25): game device GPU priority %+d (%s, %08lX)",wantGame,SUCCEEDED(h)?"OK":"refused",(unsigned long)h);}
        }
        if(m!=perfLastMode||c!=perfLastConfig) {perfSettleUntil=now+1500;perfLastMode=m;perfLastConfig=c;}
        const bool settling=now<perfSettleUntil;
        S.mode=m;S.config=c;S.settling=settling;
        if(perfLastMs>0) {
            const double dt=now-perfLastMs;
            if(dt<2000) {perfTotals.window.add(dt);++perfTotals.windowModeFrames[m];if(!settling) perfTotals.hist[m][c].add(dt);}
        }
        perfLastMs=now;
    }
    // 0.9.47.7 MEASURE: per-scene totals over the whole session, written to DK2VR-perf.txt every 30 s (and a first time after 12 s).
    struct SceneStat {
        double ms[4]{},fr[4]{};   // index = sampler running (1) + 2 x FastBlt active (0.9.47.13)
        double draw=0,present=0,lock=0,evict=0,rec[8]{},draws=0,vrFps=0,vrN=0,vrWait=0,vrGpu=0;
    };
    double bypassMs[4]{},bypassFr[4]{};   // 0.9.47.10: frames of the game alone (F12), per scene
    SceneStat sceneStat[4];bool perfSamplerOn=true,perfAlternate=true,perfMesureInit=false;uint64_t perfLastFile=0;
    int dominantScene() const {int dom=3;uint64_t best=0;for(int m=0;m<perfab::Modes;++m) if(perfTotals.windowModeFrames[m]>best) {best=perfTotals.windowModeFrames[m];dom=m;}return dom;}
    void writePerfFile() {
        FILE* f=fopen("DK2VR-perf.txt","w");if(!f) return;
        auto out=[&](const char* fmt,auto... a){fprintf(f,fmt,a...);fputc('\n',f);};
        out("DK2VR RAPPORT DE PERFORMANCE DU JEU - version %s - made by LoVeMaKeRz",Version);
        out("Ce fichier resume, par scene, ou part le temps du JEU (pas du casque). Les chiffres sont cumules sur la session.");
        if(ThreadStateProbe::running) out("%s",ThreadStateProbe::systemLine(GetCurrentThreadId()).c_str());
        out("Duree de la session : %.0f s | echantillonneur CPU alterne : %s (fenetres de 5 s avec / sans sonde pour mesurer son cout)",double(GetTickCount64()-perfStartTick)/1000.,perfAlternate?"oui":"non");
        for(int m=0;m<3;++m) {
            const SceneStat& t=sceneStat[m];perfab::Hist h;for(int c=0;c<perfab::Configs;++c) h.merge(perfTotals.hist[m][c]);
            const double fr=t.fr[0]+t.fr[1]+t.fr[2]+t.fr[3];
            const double msAll=t.ms[0]+t.ms[1]+t.ms[2]+t.ms[3];
            out("");out("=== SCENE : %s ===",perfab::modeNames[m]);
            if(fr<20) {out("  pas assez de donnees (%.0f images). Restez au moins 2 minutes dans cette scene.",fr);continue;}
            const double f1=std::max(1.,fr);
            {const double offMs=t.ms[0]+t.ms[2],offFr=t.fr[0]+t.fr[2],onMs=t.ms[1]+t.ms[3],onFr=t.fr[1]+t.fr[3];
            out("  Images du jeu : %.0f en %.0f s -> %.1f i/s (sonde coupee : %.1f i/s sur %.0f s | sonde active : %.1f i/s sur %.0f s)",fr,msAll/1000.,1000.*fr/std::max(1.,msAll),
                offMs>0?1000.*offFr/offMs:0.,offMs/1000.,onMs>0?1000.*onFr/onMs:0.,onMs/1000.);}
            if(FastBlt::available&&FastBlt::ab) {   // 0.9.47.13: same scene, the wrapper blit loop replaced / original
                const double a0=t.ms[0]+t.ms[1],a1=t.ms[2]+t.ms[3],f0=t.fr[0]+t.fr[1],f1b=t.fr[2]+t.fr[3];
                char cmp[96]="pas assez de donnees pour comparer";
                if(a0>5000&&a1>5000) snprintf(cmp,sizeof(cmp),"difference mesuree %+.1f i/s (actif - coupe)",1000.*f1b/a1-1000.*f0/a0);
                out("  FastBlt (copie rapide du blit du wrapper) : coupe %.1f i/s sur %.0f s | actif %.1f i/s sur %.0f s -> %s",a0>0?1000.*f0/a0:0.,a0/1000.,a1>0?1000.*f1b/a1:0.,a1/1000.,cmp);
            }
            if(h.n>20) out("  Duree d'une image : moyenne %.1f ms, mediane %.1f ms, 95%% %.1f ms, 99%% %.1f ms, pire %.0f ms | >34 ms : %.1f %% | >50 ms : %.1f %%",h.mean(),h.pct(.5),h.pct(.95),h.pct(.99),h.worst,100.*h.over34/double(h.n),100.*h.over50/double(h.n));
            out("  Repartition par image du jeu (ms) : verrous GPU %.2f | evictions de textures %.2f | mod : enregistrement %.2f + Present %.2f | %.0f draws/image",t.lock/f1,t.evict/f1,t.draw/f1,t.present/f1,t.draws/f1);
            out("  Mod, detail du crochet de dessin (ms/image) : total %.2f | reprojection %.2f, sprites/icones %.2f, copie sommets %.2f, etats %.2f, copies textures %.2f, main %.2f, fusion %.2f",t.rec[0]/f1,t.rec[1]/f1,t.rec[2]/f1,t.rec[3]/f1,t.rec[4]/f1,t.rec[5]/f1,t.rec[6]/f1,t.rec[7]/f1);
            if(t.vrN>0) out("  Casque : %.1f i/s | attente GPU VR %.2f ms | GPU VR %.2f ms",t.vrFps/t.vrN,t.vrWait/t.vrN,t.vrGpu/t.vrN);
            if(bypassFr[m]>=20) {
                const double withMod=msAll/std::max(1.,fr),alone=bypassMs[m]/std::max(1.,bypassFr[m]);
                out("  JEU SEUL (F12, mod en pause, meme scene) : %.1f i/s sur %.0f s (image de %.1f ms) contre %.1f ms avec le mod -> le mod coute %.1f ms par image dans cette scene",
                    1000./std::max(1e-3,alone),bypassMs[m]/1000.,alone,withMod,withMod-alone);
            }
            else out("  Jeu seul (F12) : pas de mesure dans cette scene (appuyez sur F12 pendant 25 s dans cette scene (les 5 premieres secondes ne comptent pas), puis F12 pour reprendre).");
            WaitProbe::sceneDump(m,out);
            ThreadStateProbe::dump(m,fr,out);
        }
        fclose(f);
    }
    uint64_t perfStartTick=GetTickCount64();
    void perfReport(double frames) {
        auto& S=perfab::shared();
        const auto vs=perfab::take(S.vrWindow);
        const auto& h=perfTotals.window;
        int dom=3;uint64_t best=0;for(int m=0;m<perfab::Modes;++m) if(perfTotals.windowModeFrames[m]>best) {best=perfTotals.windowModeFrames[m];dom=m;}
        log("PERF jeu (0.9.25, 5 s) scene %s, reglage %s : jeu %.1f i/s (median %.1f ms, 95%% %.1f ms, max %.0f ms, >34 ms %llu, >50 ms %llu) | fil du jeu : attentes de verrous %.2f ms/image, evictions de textures %.2f ms/image, mod (enregistrement %.2f, Present %.2f ms/image) | casque (fil VR) %.1f i/s, attente GPU VR %.2f ms, GPU VR %.2f ms",
            perfab::modeNames[dom],perfAB?perfab::configs[std::clamp(S.config.load(),0,perfab::Configs-1)].name:"fixe",h.sum>0?1000.*h.n/h.sum:0.,h.pct(.5),h.pct(.95),h.worst,(unsigned long long)h.over34,(unsigned long long)h.over50,
            LockDetail::lastMsPerFrame,PaintProbe::lastEvictMs/std::max(1.,frames),statsDraw/std::max(1.,frames),statsPresent/std::max(1.,frames),vs.fps(),vs.waitMs(),vs.gpuMs());
        {   // 0.9.26 PERF: the mod's draw hook by section, and what the texture cache evictions spend their time on
            const double f=std::max(1.,frames);auto ms=[&](long long t){return Clock::micros(t)/1000./f;};
            log("PERF mod (0.9.26, ms par image du jeu, %.0f draws/image) : crochet draw total %.2f | reprojection %.2f, sprites/icones %.2f, copie des sommets %.2f, etats %.2f, copies de textures %.2f, main %.2f, fusion %.2f",
                double(RecProf::draws)/f,ms(RecProf::t[0]),ms(RecProf::t[1]),ms(RecProf::t[2]),ms(RecProf::t[3]),ms(RecProf::t[4]),ms(RecProf::t[5]),ms(RecProf::t[6]),ms(RecProf::t[7]));
            if(!measureTaint) {   // 0.9.47.7: per-scene accumulation (0.9.47.11: not for the window next to an F12 press)
                SceneStat& t=sceneStat[dom];const int si=(perfSamplerOn?1:0)+(FastBlt::patched?2:0);
                t.ms[si]+=h.sum;t.fr[si]+=h.n;t.lock+=LockDetail::lastMsPerFrame*f;t.evict+=PaintProbe::lastEvictMs;t.draw+=statsDraw;t.present+=statsPresent;t.draws+=double(RecProf::draws);
                for(int i=0;i<8;++i) t.rec[i]+=Clock::micros(RecProf::t[i])/1000.;
                if(vs.wallUs) {t.vrFps+=vs.fps();t.vrWait+=vs.waitMs();t.vrGpu+=vs.gpuMs();t.vrN+=1;}
            }
            RecProf::clear();
            using E=CallTiming::Evict;
            if(PaintProbe::lastEvictN>0) {
                const double n=double(PaintProbe::lastEvictN);auto us=[&](long long t){return Clock::micros(t)/n;};
                long long lockAll=0;for(auto& a:E::lockT) for(long long v:a) lockAll+=v;long long callAll=0;for(long long v:E::callT) callAll+=v;
                char calls[400];int k=0;calls[0]=0;
                for(int i=0;i<15;++i) if(E::callN[i]) k+=snprintf(calls+k,sizeof(calls)-k," %s %.1f x %.0f us;",CallTiming::names[i],E::callN[i]/n,Clock::micros(E::callT[i])/E::callN[i]);
                log("PERF evictions (0.9.26) : %ld evictions, %.0f us chacune = %.2f ms/image | verrous %.0f us (texture natif/jumeau/miroir %.0f/%.0f/%.0f, surface natif/ombre/miroir %.0f/%.0f/%.0f ; %.1f verrous/eviction) | deverrouillages %.0f us (%.1f) | appels Direct3D %.0f us :%s | reste (copie CPU, wrapper) %.0f us",
                    long(PaintProbe::lastEvictN),1000.*PaintProbe::lastEvictMs/n,PaintProbe::lastEvictMs/f,us(lockAll),us(E::lockT[0][2]+E::lockT[0][0]),us(E::lockT[0][1]),us(E::lockT[0][3]),us(E::lockT[1][2]+E::lockT[1][0]),us(E::lockT[1][1]),us(E::lockT[1][3]),
                    [&]{long c=0;for(auto& a:E::lockN) for(long v:a) c+=v;return c/n;}(),us(E::unlockT),E::unlockN/n,us(callAll),calls[0]?calls:" aucun",
                    std::max(0.,1000.*PaintProbe::lastEvictMs/n-us(lockAll)-us(E::unlockT)-us(callAll)));
            }
            E::clear();PaintProbe::lastEvictN=0;
        }
        ThreadStateProbe::scene=measureTaint?3:dom;
        measureTaint=false;   // 0.9.47.11: the next window is clean again
        perfTotals.window.clear();for(auto& n:perfTotals.windowModeFrames) n=0;
        perfTotals.collectVr();
        if(!perfMesureInit) {perfMesureInit=true;perfAlternate=iniInt(L"PerfMesureAlterne",1)!=0;}
        if(FastBlt::available&&FastBlt::ab) {static unsigned fbWin=0;++fbWin;FastBlt::set(((fbWin>>1)&1)==0);}   // 0.9.47.13: 2 windows with the fast blit, 2 with the original (every sampler setting seen once in each)
        if(perfAlternate&&WaitProbe::running) {perfSamplerOn=!perfSamplerOn;WaitProbe::samplerPaused=perfSamplerOn?0:1;}   // 0.9.47.7: alternate windows with / without the CPU sampler
        if(GetTickCount64()-perfLastFile>15000) {perfLastFile=GetTickCount64();writePerfFile();}
        if(GetTickCount64()-perfLastBilan>30000) {perfLastBilan=GetTickCount64();perfTotals.bilan([](const char* f,auto... a){log(f,a...);},perfAB);}
    }
    void present(IDirect3DDevice9* d) {
        const double cadenceNow=milliseconds();
        const uint32_t cadenceMode=native.mode();
        if(native.frontend()) {tableCadence.pause();possessionCadence.pause();}
        else if(possessionWorld(request.mode,cadenceMode)) {possessionCadence.sample(cadenceNow);tableCadence.pause();}
        else if(cadenceMode==3) {tableCadence.sample(cadenceNow);possessionCadence.pause();}
        else {tableCadence.pause();possessionCadence.pause();}
        if(!gameWindow) {D3DDEVICE_CREATION_PARAMETERS cp{};if(SUCCEEDED(d->GetCreationParameters(&cp))) gameWindow=cp.hFocusWindow;}
        {const double nowMs=milliseconds();if(lastPresentMs>0&&nowMs-lastPresentMs>600&&hitchLogs<40) {++hitchLogs;log("HITCH: %.0f ms between two game frames (decoupled=%d anchored=%d lastAnchored=%d cameraMode=%u render memory blocks=%u expansion skipped=%ld)",nowMs-lastPresentMs,int(decoupledFrame),int(anchored),int(lastAnchored),unsigned(native.mode()),unsigned(native.read<uint32_t>(0x765B08)),long(ViewExpand::skipped));}lastPresentMs=nowMs;}
        if(spriteCensus==2&&geomFile) {fclose(geomFile);geomFile=nullptr;++geomDumps;log("Geometry of the F11 frame saved in Captures (DK2VR_geom_%03u.csv, %u draws)",geomDumps,geomDraw);}   // 0.9.40
        if(spriteCensus>0) --spriteCensus;   // 0.9.27
        {static uint32_t pf=0;probeFrame=(++pf%150)==0;if(probeFrame) smallLogsFrame=0;if(nearDungeonDraws>=8) nearDungeonHold=15; else if(nearDungeonHold>0) --nearDungeonHold;if(nearDungeonHold) ++statPlaneOff;nearDungeonDraws=0;dungeonBottom=dungeonBottomNow>0?dungeonBottomNow:1.f;dungeonBottomNow=0;if(probeFrame) panelProbeLines=0;if(pf%10==0) EngineOrigin::decideAll();}
        profiler.start(GetCurrentThreadId());
        {   // 0.9.47.7: PerfMesure=1 (measure build) switches the wait probe and the CPU sampler on; settings read once (not every frame)
            static const bool mesure=iniInt(L"PerfMesure",0)!=0;static const bool waitOn=iniInt(L"WaitProbe",0)!=0;static const bool cpuOn=iniInt(L"CpuSampler",0)!=0;static const bool engOn=iniInt(L"EngineProfile",0)!=0;
            ProcessAffinity::apply(GetCurrentThreadId(),[](const char* f,auto... a){log(f,a...);});   // 0.9.47.12 (once)
            FastBlt::install([](const char* f,auto... a){log(f,a...);});   // 0.9.47.13 (once)
            {static bool snapOnce=false;if(!snapOnce) {snapOnce=true;if(measureMode()) log("MEASURE MODE (0.9.47.20, DK2VR_MEASURE=1): PerfMesure ThreadStateProbe MotionProbe PossessionTrace MenuDiagnostic LockDetail are ON for this run");if(testProfile()) log("TEST PROFILE %d (0.9.47.18, DK2VR_PROFILE): %s, whatever DK2VR.ini says",testProfile(),testProfile()==1?"NativeWorldSkip FastSnapshot FastBlt WorkerAffinity InputAsync MenuFastExp and the probes forced to 0":testProfile()==2?"the 0.9.47.7+ optimisations and probes AND TextureStaging FastCrc FastStretch SyscallSavings forced to 0":testProfile()==3?"profile 2 + ENGINE settings off (DrawBackFaces ForceHighDetail TextureDetailFix ForceTextureQuality = 0, ViewExpand ViewFarScale = 1)":testProfile()==4?"profile 2 + REPLAY rules off (FloorDecalNoDepth TranslucentNoDepth SpriteNoDepth OpaqueFirst UprightEffects(NoDepth) ForgeSmokeFanHide SpriteAlphaOnly CreatureIconsFacePlayer DamageDigitsUpright DigitsDrawnLast BadgeDiscUpright = 0)":"profiles 3 + 4 together (engine settings and replay rules off)");Snapshot::policy.fast=iniInt(L"FastSnapshot",1)!=0;log("Snapshot reads (0.9.47.16): FastSnapshot=%d (%s)",Snapshot::policy.fast?1:0,Snapshot::policy.fast?"states of disabled texture stages and of samplers without texture are not re-read from the device":"every state read for every draw, as before");}}
            if(mesure||waitOn) {WaitProbe::cpuSampler=mesure||cpuOn;WaitProbe::install(GetCurrentThreadId(),[](const char* f,auto... a){log(f,a...);});}
            if(mesure&&iniInt(L"ThreadStateProbe",0)!=0) {   // 0.9.47.10: how Windows itself sees the game thread (running / ready / waiting), the other threads and the cores
                static bool probeTried=false;   // 0.9.47.12: ONCE (0.9.47.10 / 11 called install and the log line on every frame)
                if(!probeTried) {
                    probeTried=true;
                    ThreadStateProbe::install(GetCurrentThreadId(),[](const char* f,auto... a){log(f,a...);});
                    log("%s",ThreadStateProbe::systemLine(GetCurrentThreadId()).c_str());
                }
            }
            else if(engOn) WaitProbe::installSamplerOnly(GetCurrentThreadId(),[](const char* f,auto... a){log(f,a...);});
        }
        profiler.report();profiler.lastPresentTick=GetTickCount64();profiler.stallLogged=false;
        NonBlockingPresent::install(d);
        {   // 0.8.60: game-thread identity + timing sample (1 frame in 4) for the Direct3D call/lock diagnostics.
            static bool timingInit=false;static uint32_t timingFrame=0;
            if(!timingInit) {timingInit=true;CallTiming::enabled=iniInt(L"CallTiming",1)!=0;LockDetail::enabled=iniInt(L"LockDetail",0)!=0;CallTiming::gameThread=GetCurrentThreadId();CallerScan::init();LockDetail::init();GpuUse::init();}
            const bool sample=CallTiming::enabled&&(timingFrame++&3)==0;CallTiming::sampling.store(sample,std::memory_order_relaxed);if(sample) ++sampledFrames;
            TextureWatch::stagingEndFrame();
            TextureWatch::healDevice(d); // 0.8.62: puts the device hooks back if the runtime rewrote its vtable
            TimedCalls::gate.clear();    // 0.9.47.10: a hidden draw armed but never sent is an anomaly (counted), never carried into the next frame
        }
        textureQuality.enforce();
        // 0.9.47.11: F12 = the "game alone" measurement and NOTHING else. It is read only by the KeyPoll thread (rising edges). The old polling of F12 on
        // the game thread (GetAsyncKeyState every 4th frame, 0.8.55) is removed: if the thread cannot run, F12 stays inactive (logged once) instead of
        // two readers racing for the same key. F11 (interface dump) keeps its old polling as a fallback.
        static uint32_t keyTick=0;static bool f12Warned=false;
        bool f12Edge=false;
        if(KeyPoll::ready()) {   // 0.9.47.10: the thread hands over the rising edges
            if(KeyPoll::take11()) {uiDumpPending=true;spriteCensus=2;spriteCensusLogs=0;blendCensusLogs=0;texDumped.clear();}
            f12Edge=KeyPoll::take12();
        } else if(!kPublicBuild) {   // 0.9.47.60: public build: F11 / F12 are not read at all
            if(!NativeMouse::savings||(keyTick++&3)==0) {const bool f11=(GetAsyncKeyState(VK_F11)&0x8000)!=0;if(f11&&!f11Down) {uiDumpPending=true;spriteCensus=2;spriteCensusLogs=0;blendCensusLogs=0;texDumped.clear();}f11Down=f11;}
            if(!f12Warned) {f12Warned=true;log("F12 (game alone measurement): inactive, the key poll thread is off (KeyPollThread=0) or could not be created");}
        }
        if(f12Edge) {
            bypass=!bypass;log("VR pause (F12): %s",bypass?"ON - the mod does nothing, measuring the game alone (the first 5 s are a settling period and are not counted)":"OFF - the first 5 s window after the pause is not counted");
            if(bypass&&slot>=0&&ipc.data) {InterlockedExchange(&ipc.data->slots[slot].state,0);slot=-1;}
            if(bypass) {dropPending();if(vr) vr->active=false;pendingPackage.reset();}
            // 0.6.9.5: the pause also gives the game back its normal camera focal (0.6.9.4: focal left at x0.33 while paused -> crash on resume).
            if(bypass) {LensHack::want=1.f;pickWant=1.f;pickFits=false;ViewExpand::rectValid=0;}
            if(bypass&&perfAppliedGame!=0) {ComPtr<IDirect3DDevice9Ex> ex;if(SUCCEEDED(d->QueryInterface(IID_PPV_ARGS(&ex)))) ex->SetGPUThreadPriority(0);perfAppliedGame=0;}   // 0.9.25: the game alone = original GPU priority
            clearBatch();frameOpen=false;
            // 0.9.47.11 clean measurement: whatever the probes gathered around the press (waits, CPU, thread states, windows that mix paused and
            // normal frames) goes to the "other" bucket (3), never to the menu / dungeon / possession totals, and the 5 s window restarts now.
            measureTaint=true;bypassSettle=bypass;
            WaitProbe::scene=3;ThreadStateProbe::scene=3;
            statsFrames=0;statsStarted=milliseconds();lastStatus=GetTickCount64();
        }
        double presentStarted=milliseconds();
        struct PresentTimer {double start;double& total;~PresentTimer(){total+=milliseconds()-start;}} presentTimer{presentStarted,presentMilliseconds};
        if(ipc.data) mouse.pointer(pointerRequest());
        mouse.tick(d);
        if(bypass) {
            possession.release();
            frameOpen=false;
            ++statsFrames;
            if(GetTickCount64()-lastStatus>5000) {
                double elapsed=milliseconds()-statsStarted;
                log("Timing (VR PAUSED): game=%.1f fps (%.1f ms/frame)",1000*statsFrames/std::max(1.,elapsed),elapsed/std::max<uint64_t>(1,statsFrames));
                {const int bs=native.frontend()?0:(possessionWorld(request.mode,native.mode())?2:1);if(bypassSettle) bypassSettle=false;else if(elapsed<8000) {bypassMs[bs]+=elapsed;bypassFr[bs]+=double(statsFrames);}}   // 0.9.47.10 / 0.9.47.11: first window = settling
                if(!native.frontend()) gameCounters();
                statsFrames=0;statsStarted=milliseconds();lastStatus=GetTickCount64();
            }
            return;
        }
        perfFrame(d);   // 0.9.25
        if((wheel.zoomRel||wheel.background)&&!wheel.window) {D3DDEVICE_CREATION_PARAMETERS cp{};if(SUCCEEDED(d->GetCreationParameters(&cp))) wheel.attach(cp.hFocusWindow);}
        zoom.setExtra(zoomExtraPercent(request)*.01f);   // 0.8.94.15: DEZOOM TABLE slider of the OPTIONS wall panel
        zoom.tick();
        possessionTransition("at Present");   // 0.8.94.19 (also without the 360 hook)
        topDown.apply(native,lastAnchored&&!possessedFrame&&!native.frontend()&&!(scriptedOnWall&&native.mode()==18));   // 0.8.91: never fight the scripted camera
        if(!start(d)) {possession.release();publishPending(d,false);return;}
        {   // 0.8.67: what turn does this frame already contain? (measured on the native camera heading; see PossessionInput::fit)
            const bool possNow=request.mode==2||(request.mode==0&&(mode==1||mode==2));
            float yawM=0;const bool yawOk=possNow&&possessionCameraYaw(yawM);
            {   // the game's heading readout belongs to the FRONT view (it is the last world render of the frame)
                possession.observe(yawOk,yawM);
                possYawNow=yawOk?yawM:0.f;possYawNowOk=yawOk&&std::isfinite(yawM);
            }
            possCamNowOk=false;
            if(possNow&&native.supported) {if(auto* cam=native.camera()) {memcpy(possCamNow,cam+0xd68,12);possCamZNow=native.read<float>(0x760ac0);possCamNowOk=std::isfinite(possCamZNow);}}
            contentTurnNow=possession.content();
            if(ipc.data) ipc.data->possessionGain=(possNow&&possession.active&&possession.fitted)?possession.gain:0.f;
            if(possNow&&possession.active&&request.valid) {
                const double dd=std::fabs(double(request.possessionTurn)-contentTurnNow)*180./Pi;possession.predSum+=dd;possession.predMax=std::max(possession.predMax,dd);++possession.predFrames;
            }
        }
        if(request.mode==2||(request.mode==0&&(mode==1||mode==2))) uiWatch.inPossession();   // 0.9.47.39
        possession.tick(request,gameWindow,request.mode==2||(request.mode==0&&(mode==1||mode==2)),false);
        keyButtons.tick(request,gameWindow);   // 0.9.1
        keyboard.tick(request,gameWindow);     // 0.9.47.34
        struct CloseFrame {bool& open;~CloseFrame(){open=false;}} closeFrame{frameOpen};++presentSeq;   // 0.8.97
        // 0.8.68: the next frame may be drawn at the headset rate only if THIS frame was a complete first-person world.
        {   // a few empty frames in a row do not send the view back to the frame-by-frame path (it would pause and restart the VR thread)
            const bool good=possessionWorld(request.mode,mode)&&possessedFrame&&possessionFrameReady&&world>0;
            if(good) {possessionBadFrames=0;lastPossessionReady=true;}
            else if(possessionWorld(request.mode,mode)&&vr&&vr->active&&activeKind==2&&possessionBadFrames<20) ++possessionBadFrames;
            else {possessionBadFrames=0;lastPossessionReady=false;}
        }
        zeroWorldRun=(world==0&&!possessedFrame)?std::min(zeroWorldRun+1,100000):0; // 0.8.88
        if(!(decoupledFrame&&decoupledKind==3)) {MenuTap::armed.store(false,std::memory_order_relaxed);MenuTap::fresh=false;MenuTap::clearRing();MenuTap::finalArmed.store(false,std::memory_order_relaxed);MenuTap::clearFinal();menuUiLast.reset();}   // 0.8.90.7: the read-back tap only listens in the menu
        if(decoupledFrame) {if(decoupledKind==2) presentDecoupledPossession(d); else if(decoupledKind==3) presentDecoupledMenu(d); else presentDecoupled(d);return;}
        if(possessionWorld(request.mode,mode)&&(world==0||!possessionFrameReady)) {
            if(slot>=0) InterlockedExchange(&ipc.data->slots[slot].state,0);
            slot=-1;clearBatch();++dropped;
            publishPending(d,false); // keep a previously completed, non-empty frame
            if(GetTickCount64()-lastStatus>5000) statusLog();
            return; // do not publish two cleared possession eyes
        }
        if(batched&&possessionWorld(request.mode,mode)&&!possessionCoverageGuard.accept(possessionCoverage,GetTickCount64())) {
            if(slot>=0) InterlockedExchange(&ipc.data->slots[slot].state,0);
            slot=-1;clearBatch();++dropped;publishPending(d,false);
            if(GetTickCount64()-lastStatus>5000) statusLog();
            return;
        }
        if(anchored) control.measure(nativeCameraInTable(table),anchored);
        control.tick(request,tableCamera,activeUnits,anchored,mode);
        noteAnchored();
        replayMilliseconds=0;replay(d);
        double baseStarted=milliseconds();drawOverlay(d,!batched);drawMilliseconds+=milliseconds()-baseStarted;
        auto& im=images[slot];auto& out=ipc.data->slots[slot];
        ComPtr<IDirect3DSurface9> back;
        HRESULT hr=d->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&back);
        if(SUCCEEDED(hr)) hr=d->StretchRect(back.Get(),nullptr,im.target[2].Get(),nullptr,D3DTEXF_NONE);
        double syncStarted=milliseconds();
        if(pipelined&&!fences[slot]&&FAILED(d->CreateQuery(D3DQUERYTYPE_EVENT,&fences[slot]))) {pipelined=false;log("Pipelined sync unavailable; direct sync used");}
        if(pipelined) {
            if(SUCCEEDED(hr)) hr=fences[slot]->Issue(D3DISSUE_END);
            if(SUCCEEDED(hr)) fences[slot]->GetData(nullptr,0,D3DGETDATA_FLUSH); // submit now, do not wait
            publishPending(d,true); // previous frame: normally already finished on the GPU
            syncMilliseconds=milliseconds()-syncStarted;
            if(FAILED(hr)) {log("Frame GPU fence failed %08lX",(unsigned long)hr);InterlockedExchange(&out.state,0);slot=-1;return;}
            pending.slot=slot;memcpy(pending.eyes,request.eyes,sizeof(pending.eyes));pending.anchor=request.anchor;
            pending.world=world;pending.ui=ui;pending.rejected=rejected;pending.mode=mode;pending.turn=contentTurnNow;
            slot=-1;
        } else {
        if(SUCCEEDED(hr)) hr=fence->Issue(D3DISSUE_END);
        uint64_t deadline=GetTickCount64()+250;
        if(SUCCEEDED(hr)) do {hr=fence->GetData(nullptr,0,D3DGETDATA_FLUSH);if(hr==S_FALSE) Sleep(0);} while(hr==S_FALSE&&GetTickCount64()<deadline);
        if(hr!=S_OK) {log("Frame GPU synchronization failed %08lX",(unsigned long)hr);InterlockedExchange(&out.state,0);slot=-1;return;}
        syncMilliseconds=milliseconds()-syncStarted;
        }
        statsDraw+=drawMilliseconds;statsSync+=syncMilliseconds;statsReplay+=replayMilliseconds;statsPresent+=presentMilliseconds;presentMilliseconds=0;++statsFrames;statsWorld+=world;
        if(pipelined) {
            if(GetTickCount64()-lastStatus>5000) statusLog();
            return;
        }
        out.generation=im.generation;out.frame=frame=++sharedFrames;
        for(int i=0;i<3;++i) out.handles[i]=reinterpret_cast<uintptr_t>(im.handles[i]);
        out.width=im.w;out.height=im.h;out.desktopWidth=im.dw;out.desktopHeight=im.dh;
        memcpy(out.eyes,request.eyes,sizeof(out.eyes));out.anchor=request.anchor;
        out.worldDraws=world;out.uiDraws=ui;out.rejectedDraws=rejected;out.cameraMode=mode;out.possessionTurn=contentTurnNow;
        MemoryBarrier();InterlockedExchange(&out.state,2);SetEvent(ipc.ready);slot=-1;
        if(GetTickCount64()-lastStatus>5000) statusLog();
    }
    // 0.8.68: the first-person world of a game frame becomes a package for the VR thread (PossessionDecoupled=1): the headset images are then
    // drawn at the headset rate with the newest head pose, and the turn / walk the game has not drawn yet is carried on from the measured motion.
    // The game thread only records, exactly as in the table view; nothing about the table package path is shared or changed.
    // 0.9.47.41: the possession interface (top bar with the skill icons) is cut from the game's final image. Called by BOTH present paths (0.9.47.40 only had it
    // in the table path: report 20261009_155611 shows it never ran in a possession).
    void captureSkillBar(IDirect3DDevice9* d,Package* p,ComPtr<IDirect3DSurface9>& back) {
        static bool wasPossessed=false;static int skFramesG=0;static int skPending=-1;
        if(!possessedFrame) {memset(skillSeen,0,sizeof(skillSeen));skillLatch=0;skPending=-1;}
        if(possessedFrame&&!wasPossessed) {memset(skillSeen,0,sizeof(skillSeen));skillLatch=0;skPending=-1;skFramesG=0;}   // 0.9.47.45: every possession starts from zero and analyses at once
        wasPossessed=possessedFrame;   // 0.9.47.42: a new possession starts with no skill slot
        p->skillBar=0;p->skillIconCount=0;
        if(possessedFrame&&iniInt(L"PossessionSkillBar",1)!=0&&back.Get()) {   // 0.9.47.40: the game's own skill icons (top bar of the possession screen), cut from its final image
            D3DSURFACE_DESC bd{};back->GetDesc(&bd);
            const LONG W=LONG(bd.Width),H=LONG(bd.Height);
            LONG gw=W,gx=0;if(W>H*1.34f) {gw=LONG(H*4.f/3.f+.5f);gx=(W-gw)/2;}
            static const float fx0=iniFloat(L"PossessionSkillBarX0",.545f,0.f,.9f),fx1=iniFloat(L"PossessionSkillBarX1",.875f,.1f,1.f),fy1=iniFloat(L"PossessionSkillBarY1",.08f,.02f,.3f);
            RECT src{gx+LONG(gw*fx0),0,gx+LONG(gw*fx1),LONG(H*fy1)};
            const UINT cw=UINT(src.right-src.left),ch=UINT(src.bottom-src.top);
            if(cw>=32&&ch>=8) {
                {   // 0.9.47.43: which slots really hold a skill icon? (the game redraws its icon pieces only when something changes, so they cannot be counted live)
                    // The top bar is shrunk to 330 x 20 pixels every 15 game frames and each slot is compared with the plain wood of the bar (report 164930: icon slots
                    // differ by 45..59 grey levels from the wood, empty slots by 1..2).
                    if((skFramesG++%15)==0) {
                        if(!skRt) d->CreateRenderTarget(330,20,D3DFMT_A8R8G8B8,D3DMULTISAMPLE_NONE,0,FALSE,&skRt,nullptr);
                        if(!skSys) d->CreateOffscreenPlainSurface(330,20,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&skSys,nullptr);
                        if(skRt&&skSys&&SUCCEEDED(d->StretchRect(back.Get(),&src,skRt.Get(),nullptr,D3DTEXF_LINEAR))&&SUCCEEDED(d->GetRenderTargetData(skRt.Get(),skSys.Get()))) {
                            D3DLOCKED_RECT lr{};
                            if(SUCCEEDED(skSys->LockRect(&lr,nullptr,D3DLOCK_READONLY))) {
                                auto px=[&](int x,int y,int c)->float{const unsigned char* b=static_cast<const unsigned char*>(lr.pBits)+size_t(y)*lr.Pitch+size_t(x)*4;return float(b[c]);};
                                float ref[11][3];
                                for(int y=3;y<14;++y) for(int c=0;c<3;++c) {float a=0;for(int x=300;x<322;++x) a+=px(x,y,c);ref[y-3][c]=a/22.f;}
                                int found=0;float diffs[SkillSlots]{};
                                for(int sl=0;sl<SkillSlots;++sl) {
                                    const float f0=(SkillSlotX0+SkillSlotStride*float(sl)-fx0)/(fx1-fx0);
                                    const int x0=int(f0*330.f)+8,x1=std::min(330,x0+34);
                                    if(x0<0||x0>=330) continue;
                                    float dsum=0;int cnt=0;
                                    for(int x=x0;x<x1;++x) for(int y=3;y<14;++y) {float dd=0;for(int c=0;c<3;++c) dd+=std::fabs(px(x,y,c)-ref[y-3][c]);dsum+=dd/3.f;++cnt;}
                                    diffs[sl]=cnt?dsum/float(cnt):0.f;
                                    if(diffs[sl]>12.f) found=sl+1;
                                }
                                skSys->UnlockRect();
                                // 0.9.47.45: the image is the only authority (report 192210: a 2-skill creature showed 4 slots after a 4-skill one). The count can go down:
                                // a lower value must be seen twice in a row (15 frames apart) before it is taken.
                                if(found>=skillLatch) {skillLatch=found;skPending=-1;}
                                else if(skPending==found) {skillLatch=found;skPending=-1;}
                                else skPending=found;
                                static int alog=0,lastFound=-1;if(found!=lastFound&&alog++<20) {lastFound=found;log("Possession skills (0.9.47.43): image analysis, %d skill slot(s) hold an icon (differences %.0f %.0f %.0f %.0f %.0f %.0f)",found,diffs[0],diffs[1],diffs[2],diffs[3],diffs[4],diffs[5]);}
                            }
                        }
                    }
                }
                p->skillBar=copies.gameImage(d,back.Get(),src,cw,ch);
                if(p->skillBar) {
                    if(auto* c=copies.find(p->skillBar)) {
                        bool listed=false;for(auto& ref:p->copies) if(ref.id==p->skillBar) listed=true;
                        if(!listed) p->copies.push_back({p->skillBar,c->handle,c->w,c->h,c->levels});
                        p->skillAspect=float(ch)/float(cw);
                        {int n=0;
                         for(int sl=0;sl<skillLatch;++sl) {   // slots are contiguous from the first one
                             const float sx=SkillSlotX0+SkillSlotStride*float(sl);
                             p->skillIconX0[n]=(sx-fx0)/(fx1-fx0);p->skillIconX1[n]=(sx+SkillSlotW-fx0)/(fx1-fx0);++n;}
                         p->skillIconCount=n;PossessionInput::detected=n;
                         static int ilog=0,last=-1;if(n!=last&&ilog++<20) {last=n;log("Possession skills (0.9.47.42): %d skill icon(s) in the top bar slots (mask %d%d%d%d%d%d)",n,skillSeen[0]?1:0,skillSeen[1]?1:0,skillSeen[2]?1:0,skillSeen[3]?1:0,skillSeen[4]?1:0,skillSeen[5]?1:0);}}
                    } else p->skillBar=0;
                }
                {   // diagnostic: the cut-out and the whole screen are saved in Captures (3 times: first frame, 2 s, 6 s of possession) so that the area can be checked / corrected
                    static uint64_t dumpFrames=0;static int dumps=0;++dumpFrames;
                    if(!kPublicBuild&&dumps<3&&(dumpFrames==1||dumpFrames==100||dumpFrames==300)&&(bd.Format==D3DFMT_X8R8G8B8||bd.Format==D3DFMT_A8R8G8B8)) {
                        ComPtr<IDirect3DSurface9> sys;
                        if(SUCCEEDED(d->CreateOffscreenPlainSurface(bd.Width,bd.Height,bd.Format,D3DPOOL_SYSTEMMEM,&sys,nullptr))&&SUCCEEDED(d->GetRenderTargetData(back.Get(),sys.Get()))) {
                            D3DLOCKED_RECT lr{};
                            if(SUCCEEDED(sys->LockRect(&lr,nullptr,D3DLOCK_READONLY))) {
                                std::error_code ec;auto folder=configurationPath().parent_path()/L"Captures";std::filesystem::create_directories(folder,ec);
                                wchar_t n[80];
                                swprintf(n,80,L"DK2VR_possession_ecran_%d.bmp",dumps);writeBmp32(folder/n,bd.Width,bd.Height,static_cast<const uint32_t*>(lr.pBits),lr.Pitch/4);
                                swprintf(n,80,L"DK2VR_possession_barre_%d.bmp",dumps);writeBmp32(folder/n,cw,ch,reinterpret_cast<const uint32_t*>(static_cast<const unsigned char*>(lr.pBits)+size_t(src.top)*lr.Pitch+size_t(src.left)*4),lr.Pitch/4);
                                sys->UnlockRect();++dumps;
                            }
                        }
                    }
                }
                static int logs=0;if(logs++<3) log("Possession skill bar (0.9.47.40): game screen %ldx%ld, area %ld,%ld-%ld,%ld copied (%u): %s",long(W),long(H),long(src.left),long(src.top),long(src.right),long(src.bottom),p->skillBar,p->skillBar?"ok":"copy failed");
            }
        }
    }
    void presentDecoupledPossession(IDirect3DDevice9* d) {
        publishPending(d,false); // a 0.2.9 image possibly still waiting from the previous (legacy) frame
        control.tick(request,tableCamera,activeUnits,anchored,mode);
        noteAnchored();
        statsDraw+=drawMilliseconds;statsPresent+=presentMilliseconds;presentMilliseconds=0;++statsFrames;statsWorld+=world;
        // Same acceptance rules as the legacy possession path: no empty frame, no partially drawn dungeon.
        // 0.8.81: the coverage guard is fed by the FRONT view only (a side / rear view may legitimately show little: a wall, an empty room behind)
        if(world==0||records.empty()||!possessionFrameReady||!possessionCoverageGuard.accept(possessionCoverage,GetTickCount64())) {
            clearBatch();++dropped;++statPossDropped;
            if(GetTickCount64()-lastStatus>5000) statusLog();
            return;
        }
        const double syncStarted=milliseconds();
        double partMark=syncStarted;
        auto p=std::make_shared<Package>();
        p->id=packageId;p->epoch=vr->epoch;p->recordedMs=milliseconds();p->contentMs=frameStartMs;p->possession=true;
        {   // 0.8.84: the cube is shown only when ALL 6 faces of this game frame were drawn by the game (otherwise the reference view alone, as 0.8.74)
            uint32_t nv[7]{};for(const auto& rc:records) if(rc.view<7) ++nv[rc.view];
            bool all=SurroundHack::frameCube&&LensHack::cube.valid&&nv[0]>0&&!faceDropped&&facePoseOk;
            const unsigned want=SurroundHack::frameWant;   // 0.8.99.1: only the faces asked for this frame must be there
            for(int v=1;v<=6;++v) {statViewRecs[v]+=nv[v];if((want&(1u<<(v-1)))&&!SurroundHack::frameOk[v-1]) all=false;}
            for(int v=1;v<=4;++v) if((want&(1u<<(v-1)))&&nv[v]==0) all=false;   // a level face always sees walls / floor; up / down may legitimately be empty
            p->cube=all;p->faceMask=all?want:63u;if(all) ++statCubeFrames; else if(SurroundHack::sFrames) ++statViewMissing;
            statViewRecs[0]+=nv[0];++statViewFrames;
        }
        p->vertices.swap(arena);p->indices.swap(indexArena);joinRecordedBadges();p->records.swap(records);p->digitGroups.swap(digitGroupsNow);
        p->snapshots.resize(snapshots.size());
        std::vector<uint32_t> refs,shaderRefs;refs.reserve(snapshots.size()*2);
        for(size_t i=0;i<snapshots.size();++i) {
            auto& in=snapshots[i];auto& out=p->snapshots[i];
            memcpy(out.rs,in.rs,sizeof(out.rs));memcpy(out.ts,in.ts,sizeof(out.ts));memcpy(out.ss,in.ss,sizeof(out.ss));
            for(int t=0;t<Snapshot::Samplers;++t) {out.copy[t]=snapIds[i][t];if(out.copy[t]) refs.push_back(out.copy[t]);}
            out.shader=snapIds[i][3];if(out.shader) shaderRefs.push_back(out.shader);
        }
        std::sort(refs.begin(),refs.end());refs.erase(std::unique(refs.begin(),refs.end()),refs.end());
        for(uint32_t id:refs) if(auto* c=copies.find(id)) p->copies.push_back({id,c->handle,c->w,c->h,c->levels});
        std::sort(shaderRefs.begin(),shaderRefs.end());shaderRefs.erase(std::unique(shaderRefs.begin(),shaderRefs.end()),shaderRefs.end());
        for(uint32_t id:shaderRefs) {auto it=shaders.code.find(id);if(it!=shaders.code.end()) p->shaders.push_back({id,it->second});}
        p->clip=false;p->ceiling=false;
        p->table=Pose{{0,0,0,1},{0,0,0}};p->tableWidth=1;p->tableDepth=.75f;
        p->possPivot={possessionAnchor.position.x,0,possessionAnchor.position.z};
        if(p->cube) {   // 0.8.84: the 4 planes of every face pyramid, in record space, through the camera position, with the pose the face vertices were placed with
            const Pose c=facePose;
            p->faceO=c.position;
            for(int k=0;k<6;++k) for(int q=0;q<4;++q) {const float* n=LensHack::cube.N[k][q];p->faceN[k][q]=rotate(c.orientation,Vec3{n[0],n[1],n[2]});}
        }
        p->possYaw=possYawNow;p->possYawOk=possYawNowOk;memcpy(p->possUp,possUpNow,sizeof(p->possUp));p->possUpOk=possUpOk;
        memcpy(p->possCam,possCamNow,sizeof(p->possCam));p->possCamZ=possCamZNow;p->possCamOk=possCamNowOk;
        p->world=world;p->ui=ui;p->rejected=rejected;p->mode=mode;
        ComPtr<IDirect3DSurface9> back;
        if(SUCCEEDED(d->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&back))) {
            D3DSURFACE_DESC desc{};back->GetDesc(&desc);
            p->desktopWidth=desc.Width;p->desktopHeight=desc.Height; // the game screen is not shown in the first-person world: no copy
        }
        captureSkillBar(d,p.get(),back);   // 0.9.47.41
        presParts[0]+=milliseconds()-partMark;partMark=milliseconds();
        clearBatch();
        presParts[2]+=milliseconds()-partMark;partMark=milliseconds();
        pollPackage(true); // previous game frame: normally already finished on the GPU
        presParts[3]+=milliseconds()-partMark;partMark=milliseconds();
        auto& q=packageFences[p->id&1];
        HRESULT hr=q?S_OK:d->CreateQuery(D3DQUERYTYPE_EVENT,&q);
        if(SUCCEEDED(hr)) hr=q->Issue(D3DISSUE_END);
        if(SUCCEEDED(hr)) {q->GetData(nullptr,0,D3DGETDATA_FLUSH);pendingPackage=std::move(p);++statPackages;++statPossPackages;}
        else {log("Package fence failed %08lX",(unsigned long)hr);++dropped;}
        presParts[4]+=milliseconds()-partMark;
        statsSync+=milliseconds()-syncStarted;
        const double since=std::max(vr->lastPublishMs.load(),decoupledSince);
        if(vr->active&&handed>0&&milliseconds()-since>2000) {
            ++watchdogTrips;vr->active=false;activeKind=0;
            if(watchdogTrips>=4) vr->failed=true; else retryAt=milliseconds()+5000;
            log("Decoupled VR rendering: no headset image from the VR thread for %.0f ms (first-person world); 0.2.9 path %s",milliseconds()-since,vr->failed?"from now on":"for 5 s, then retry");
        }
        if(GetTickCount64()-lastStatus>5000) statusLog();
    }
    // 0.3.0: the game frame becomes a package for the VR thread; no eye image is drawn here.
    // 0.8.89 DIAGNOSTIC (MenuDiagnostic=1): census of what the game draws in the frontend (main menu): 2D interface (rhw exactly 1) against pre-transformed
    // 3D (rhw varying), vertex counts, depth / rhw ranges, textures and blend states. One detailed frame every 3 s, totals every 3 s. Read by Claude, not by the player.
    // 0.8.95: 2D / decor separation of the main menu on the game thread (MenuSplit), see MenuTap in Decoupled.inl
    bool menuUiCpu=true;MenuSplit menuSplit;std::shared_ptr<const std::vector<uint32_t>> menuUiLast;UINT menuUiLastW=0,menuUiLastH=0;uint64_t menuUiGen=0;int menuUiAge=0;
    uint64_t msFrames=0,msNoDecor=0,msNoFinal=0,msFinalEarly=0,msSize=0;double msMs=0,msUi=0,msBins[6]{},msDiff[3]{};double msLast=0;bool msLogged=false;
    bool menuUiLastCursor=false;bool menuHandSolid=true;bool menuHandHalo=true;float menuHandScale=0;uint64_t menuCursorMatches=0;
    std::vector<HandCand> handCands;uint32_t handCandSerial=~0u;float handCandK=0;HandLock handLock;HandStats handStats;uint64_t menuCursorPos=0,menuCursorNoPos=0;
    void menuSplitFrame(Package* p) {
        const bool decor=MenuTap::fresh&&MenuTap::pixels.size()==size_t(MenuTap::w)*MenuTap::h&&MenuTap::w>0;
        // 0.9.7: the menu replay check of the VR thread (0.9.6) needs the decor of the SAME frame: one copy every ~40 game frames travels with the package
        // (0.9.6 put it only on the non-CPU path: the check never ran, report 221111)
        static const bool replayCheck=iniInt(L"MenuReplayCheck",0)!=0;static unsigned rcFrames=0;

        if(replayCheck&&decor&&(++rcFrames%40)==0) {p->checkDecor=std::make_shared<const std::vector<uint32_t>>(MenuTap::pixels);p->checkDecorW=MenuTap::w;p->checkDecorH=MenuTap::h;}
        const bool fin=MenuTap::finalFresh&&MenuTap::finalPx.size()==size_t(MenuTap::finalW)*MenuTap::finalH&&MenuTap::finalW>0;
        if(!decor) ++msNoDecor; else if(!fin) ++msNoFinal; else if(!MenuTap::finalAfterDecor) ++msFinalEarly;
        else if(MenuTap::finalW!=MenuTap::w||MenuTap::finalH!=MenuTap::h) ++msSize;
        else {
            const double t0=milliseconds();
            auto v=std::make_shared<std::vector<uint32_t>>();
            if(menuSplit.run(MenuTap::pixels.data(),MenuTap::finalPx.data(),MenuTap::w,MenuTap::h,*v)) {
                p->menuCursor=false;
                float mx=0,my=0,mk=1.f;
                if(p->keeperHandle&&mouse.cursorPixel(float(MenuTap::w),float(MenuTap::h),mx,my,&mk)) {
                    std::lock_guard<std::mutex> cursorLock(keeper.m);
                    {   // 0.9.33: always the wide search (report 215302: image scale 1.0, the 1:1 test at the reported position found 0 of ~200 frames: the game hand lags behind the VR position)
                        mk=std::min(mk,1.f);
                        // 0.9.31: every recent hand image x several reductions, searched around the reported position
                        if(!keeper.pixels.empty()) {   // 0.9.47.26: the cursor shown while the menu is on screen is the menu hand: keep it for good
                            bool have=false;for(auto& e:keeper.pinned)if(e.w==keeper.w&&e.h==keeper.h&&e.hotX==keeper.hotX&&e.hotY==keeper.hotY&&e.pixels==keeper.pixels){have=true;break;}
                            if(!have) {if(keeper.pinned.size()>=4)keeper.pinned.erase(keeper.pinned.begin());keeper.pinned.push_back(KeeperCursor::HistoryImage{keeper.pixels,keeper.w,keeper.h,keeper.hotX,keeper.hotY});++keeper.historySerial;}
                        }
                        if(handCandSerial!=keeper.historySerial||std::fabs(handCandK-mk)>.002f) {
                            handCands.clear();handLock=HandLock{};
                            const float allScales[]={mk,.5f,.55f,.6f,.7f,.8f,.9f,1.f};std::vector<float> scales(std::begin(allScales),std::end(allScales));
                            if(menuHandScale>0)scales.assign(1,menuHandScale);   // 0.9.35: once found, the reduction of the menu image is known
                            std::vector<const KeeperCursor::HistoryImage*> sourcesOfHands;for(auto& e:keeper.history)sourcesOfHands.push_back(&e);
                            for(auto& e:keeper.pinned){bool dup=false;for(auto* o:sourcesOfHands)if(o->w==e.w&&o->h==e.h&&o->hotX==e.hotX&&o->hotY==e.hotY&&o->pixels==e.pixels){dup=true;break;}if(!dup)sourcesOfHands.push_back(&e);}
                            for(size_t i=0;i<sourcesOfHands.size();++i)for(float s:scales){
                                const auto& e=*sourcesOfHands[i];HandCand c;
                                const auto keyed=keyedHand(e.pixels,e.w,e.h);if(!c.hand.build(keyed,e.w,e.h,s))continue;
                                c.keyed=std::any_of(keyed.begin(),keyed.end(),[](uint32_t q){return (q>>24)==0;});
                                c.hotX=e.hotX*s;c.hotY=e.hotY*s;c.image=int(i);handCands.push_back(std::move(c));}
                            handCandSerial=keeper.historySerial;handCandK=mk;
                        }
                        if(matchMenuCursor(*v,MenuTap::finalPx,int(MenuTap::w),int(MenuTap::h),handCands,mx,my,handLock,handStats,menuHandSolid,&MenuTap::pixels,menuHandHalo)){++menuCursorMatches;if(menuHandScale<=0&&handLock.cand>=0&&handLock.cand<int(handCands.size()))menuHandScale=handCands[handLock.cand].hand.scale;}
                        p->menuCursor=false;   // 0.9.34: the game's own hand stays (made opaque), no second hand drawn at the VR ray
                    }
                    ++menuCursorPos;
                } else ++menuCursorNoPos;
                menuUiLastCursor=p->menuCursor;
                menuUiLast=std::move(v);menuUiLastW=MenuTap::w;menuUiLastH=MenuTap::h;++menuUiGen;menuUiAge=0;
                ++msFrames;msMs+=milliseconds()-t0;msUi+=menuSplit.last.uiFraction;
                for(int k=0;k<6;++k) {msBins[k]+=menuSplit.last.errBins[k];}
                for(int k=0;k<3;++k) {msDiff[k]+=menuSplit.last.meanDiff[k];}
                if(!msLogged) {msLogged=true;log("Menu 2D layer (0.8.95): first separation, decor %ux%u read back from %p, final image from %p (written after the read-back), interface %.1f %% of the image",
                    MenuTap::w,MenuTap::h,(void*)MenuTap::rtDest,(void*)MenuTap::finalSurf,100.*menuSplit.last.uiFraction);}
            }
        }
        // no new pair this frame: the last layer stays for a few frames (same screen, one frame older), then none (flat screen, never an old interface)
        if(menuUiLast&&++menuUiAge>8) menuUiLast.reset();
        if(menuUiLast) {p->menuUiLayer=menuUiLast;p->menuUiW=menuUiLastW;p->menuUiH=menuUiLastH;p->menuUiGen=menuUiGen;p->menuCursor=menuUiLastCursor;}
        const double now=milliseconds();
        if(now-msLast>5000) {
            const uint64_t total=msFrames+msNoDecor+msNoFinal+msFinalEarly+msSize;
            if(msLast>0&&total) {
                const double f=msFrames?double(msFrames):1.;
                log("Menu 2D layer (0.8.95, last 5 s): %llu frames separated (%.2f ms each), missing: decor %llu, final image %llu, final before the decor %llu, size %llu | interface %.1f %% | colour difference final-decor (largest channel, decor and 2D): 0 %.1f %%, <=4 %.1f %%, <=8 %.1f %%, <=12 %.1f %%, <=24 %.1f %%, >24 %.1f %% | mean R %+.2f G %+.2f B %+.2f | tolerance %d, mapping %s (deviation %.2f, max %d) | final writes after/before the read-back %u/%u",
                    (unsigned long long)msFrames,msMs/f,(unsigned long long)msNoDecor,(unsigned long long)msNoFinal,(unsigned long long)msFinalEarly,(unsigned long long)msSize,100.*msUi/f,
                    100.*msBins[0]/f,100.*msBins[1]/f,100.*msBins[2]/f,100.*msBins[3]/f,100.*msBins[4]/f,100.*msBins[5]/f,msDiff[0]/f,msDiff[1]/f,msDiff[2]/f,
                    menuSplit.tolerance,menuSplit.learn?"learned":"identity",menuSplit.last.lutDeviation,menuSplit.last.lutMaxDeviation,MenuTap::finalWrites,MenuTap::finalWritesBeforeDecor);
            }
            if(msLast>0)log("Menu live cursor (0.9.36): %llu of %llu separated frames: native hand found and made opaque (position known %llu, unknown %llu); images %zu, candidates %zu, best score %zu/%zu cand %d at %+d,%+d, lock %d (image %d, scale %.3f, offset %+d,%+d; locked %u, searched %u, skipped %u); matched hand follows current VR ray, unmatched interface preserved",
                (unsigned long long)menuCursorMatches,(unsigned long long)msFrames,(unsigned long long)menuCursorPos,(unsigned long long)menuCursorNoPos,keeper.history.size(),handCands.size(),handStats.bestGood,handStats.bestVisible,handStats.bestCand,handStats.bestDx,handStats.bestDy,handLock.cand,handLock.cand>=0&&handLock.cand<int(handCands.size())?handCands[handLock.cand].image:-1,handLock.cand>=0&&handLock.cand<int(handCands.size())?handCands[handLock.cand].hand.scale:0.f,handLock.dx,handLock.dy,handStats.lockedFrames,handStats.searchedFrames,handStats.skippedFrames);
            menuCursorMatches=0;menuCursorPos=menuCursorNoPos=0;handStats=HandStats{};
            msLast=now;msFrames=msNoDecor=msNoFinal=msFinalEarly=msSize=0;msMs=msUi=0;for(auto& b:msBins) b=0;for(auto& d:msDiff) d=0;MenuTap::finalWrites=MenuTap::finalWritesBeforeDecor=0;
        }
    }
    bool floorDecalNoDepth=true;uint64_t statDecalNoDepth=0;bool hazeSkip=false;uint64_t statHaze=0;unsigned hazeLogs=0;static constexpr int SceneGW=192,SceneGH=144;std::vector<uint8_t> decalFlag;bool decalSnap=false,decalActive=false;float decalSnapMax=1.15f;uint64_t statDecalSnapDraws=0,statDecalSnapVerts=0;unsigned decalSnapLogs=0;std::vector<float> sceneMap,overlayRhw;uint64_t sceneMapSeq=~0ull;double sceneMapMs=0;uint64_t statOverlayDraws=0,statOverlayVerts=0,statOverlayMissed=0;unsigned overlayLogs=0;bool overlayReproject=false,overlayActive=false,effectAnchorCenter=true;
    FILE* geomFile=nullptr;unsigned geomDumps=0,geomDraw=0;bool effectScreenAxes=false,effectUpsideFix=false;uint64_t statUpsideFixed=0;
    bool forgeFanSkip=true;uint64_t statForgeFans=0,statForgeFanKept=0;unsigned forgeFanLogs=0,forgeFanKeptLogs=0;
    bool iconTopRule=true,iconScreenAxes=true,translucentNoDepth=true;bool spriteNoDepth=true;uint64_t statSpriteNoDepth=0;bool heldOffTable=true;uint64_t statHeldOffTable=0;bool damageFacing=true;uint64_t statGlyphSprites=0;uint64_t statNumberSprites=0,statNumberSpritesLast=0;uint64_t dgGlyph=0,dgBig=0,dgNotFlat=0,dgShapeNo=0,dgHandledQ=0,dgLast[5]={0,0,0,0,0};int digitProbeLogs=0,digitHandledLogs=0,digitGroupLogs=0;bool damageDigits=true,digitMirror=true,digitScreen=true,badgeDiscFix=true,digitsLast=true;uint64_t statBadgeDisc=0,statDigitsLast=0;uint64_t statDigitSprites=0;uint64_t statTranslucentNoDepth=0;unsigned blendCensusLogs=0;float spriteMaxScreen=.12f;bool softNoDepth=false;float effectPull=.5f;bool f9Down=false;uint64_t statSoftNoDepth=0;
    bool menuPillarLight=true;float menuPillarMin=176.f;uint64_t menuPillarLifted=0;
    // soft rectangle (normalised menu image coordinates) around the dark left pillar
    static float menuPillarWeight(float nx,float ny) {
        const float x0=.17f,x1=.47f,y0=.25f,y1=.81f,f=.05f;
        auto edge=[&](float v,float a,float b){const float d=std::min(v-a,b-v);return d<=0.f?0.f:std::min(1.f,d/f);};
        return edge(nx,x0,x1)*edge(ny,y0,y1);
    }
    bool menuRelief=true;float menuDepthK=.45f,menuFarK=2.f,menuNearK=.15f,menuEdge=.28f,menuEdgeMin=.35f,menuTessPx=16.f,menuTessTol=.0012f/1.36f,menuTessTolRel=.04f,menuOrderK=.06f;uint64_t menuBiasAll=0,menuBiasDraws=0;float menuBiasMin=1e9f,menuBiasMax=-1e9f;bool menuTessLogged=false;double menuTessMs=0;uint64_t menuTessHits=0,menuTessMisses=0,menuTessCuts=0;float menuLens[3]={0,0,0};uint64_t statMenuRecords=0;bool menuLensLogged=false;
    bool menuDiag=true;uint32_t mcFrames=0,mcDraws=0,mcFlat=0,mc3d=0,mcOther=0,mcLines=0;uint64_t mcVerts=0,mcTris3d=0,mcTrisFlat=0;float mcRhw[2]={1e9f,-1e9f},mcZ[2]={1e9f,-1e9f};double mcLast=0;bool mcCapture=false;
    void menuCensus(IDirect3DDevice9* d,D3DPRIMITIVETYPE type,UINT primitives,const void* verts,UINT stride,const void* indices,D3DFORMAT indexFormat,UINT count) {
        UINT n=count;
        if(!indices) switch(type) {case D3DPT_TRIANGLELIST:n=primitives*3;break;case D3DPT_TRIANGLESTRIP:case D3DPT_TRIANGLEFAN:n=primitives+2;break;case D3DPT_LINELIST:n=primitives*2;break;case D3DPT_LINESTRIP:n=primitives+1;break;default:n=primitives;}
        DWORD fvf=0;d->GetFVF(&fvf);
        ++mcDraws;mcVerts+=n;
        if((fvf&D3DFVF_POSITION_MASK)!=D3DFVF_XYZRHW||stride<16||stride>128||!verts||n==0||n>100000) {++mcOther;return;}
        const auto* in=static_cast<const unsigned char*>(verts);
        float bx0=1e9f,by0=1e9f,bx1=-1e9f,by1=-1e9f,r0=1e9f,r1=-1e9f,z0=1e9f,z1=-1e9f;bool flat=true;
        const UINT lim=std::min<UINT>(n,2048);
        for(UINT i=0;i<lim;++i) {float v[4];memcpy(v,in+size_t(i)*stride,16);bx0=std::min(bx0,v[0]);by0=std::min(by0,v[1]);bx1=std::max(bx1,v[0]);by1=std::max(by1,v[1]);z0=std::min(z0,v[2]);z1=std::max(z1,v[2]);r0=std::min(r0,v[3]);r1=std::max(r1,v[3]);if(std::fabs(v[3]-1.f)>1e-6f) flat=false;}
        if(flat) {++mcFlat;mcTrisFlat+=primitives;} else {++mc3d;mcTris3d+=primitives;mcRhw[0]=std::min(mcRhw[0],r0);mcRhw[1]=std::max(mcRhw[1],r1);mcZ[0]=std::min(mcZ[0],z0);mcZ[1]=std::max(mcZ[1],z1);}
        if(mcCapture&&mcLines<90) {
            ++mcLines;DWORD zen=0,zw=0,ab=0,sb=0,db=0,cull=0,zf=0;
            d->GetRenderState(D3DRS_ZENABLE,&zen);d->GetRenderState(D3DRS_ZWRITEENABLE,&zw);d->GetRenderState(D3DRS_ALPHABLENDENABLE,&ab);d->GetRenderState(D3DRS_SRCBLEND,&sb);d->GetRenderState(D3DRS_DESTBLEND,&db);d->GetRenderState(D3DRS_CULLMODE,&cull);d->GetRenderState(D3DRS_ZFUNC,&zf);
            ComPtr<IDirect3DBaseTexture9> t;d->GetTexture(0,&t);UINT tw=0,th=0;DWORD tf=0;
            if(t&&t->GetType()==D3DRTYPE_TEXTURE) {D3DSURFACE_DESC sd{};if(SUCCEEDED(static_cast<IDirect3DTexture9*>(t.Get())->GetLevelDesc(0,&sd))) {tw=sd.Width;th=sd.Height;tf=sd.Format;}}
            log("Menu draw #%u: %s type=%d prim=%u verts=%u stride=%u indexed=%d fvf=%08lX box %.0f,%.0f-%.0f,%.0f z %.4f-%.4f rhw %.4f-%.4f tex %ux%u fmt %lu | zen=%lu zwrite=%lu zfunc=%lu blend=%lu src=%lu dst=%lu cull=%lu",
                mcDraws,flat?"2D":"3D",int(type),primitives,n,stride,int(indices!=nullptr),(unsigned long)fvf,bx0,by0,bx1,by1,z0,z1,r0,r1,tw,th,(unsigned long)tf,zen,zw,zf,ab,sb,db,cull);
        }
    }
    void menuCensusEnd() {
        if(!menuDiag||!native.frontend()) return;
        ++mcFrames;const double now=milliseconds();
        if(mcCapture) {mcCapture=false;mcLines=0;}
        if(now-mcLast>3000) {
            if(mcLast>0&&mcFrames) log("Menu census (0.8.89, frontend, last %.1f s): frames %u | per frame: draws %.1f (2D interface %.1f, 3D %.1f, other formats %.1f), vertices %.0f, triangles 2D %.0f / 3D %.0f | 3D rhw %.4f..%.4f, z %.4f..%.4f | desktop %ux%u",
                (now-mcLast)/1000.,mcFrames,double(mcDraws)/mcFrames,double(mcFlat)/mcFrames,double(mc3d)/mcFrames,double(mcOther)/mcFrames,double(mcVerts)/mcFrames,double(mcTrisFlat)/mcFrames,double(mcTris3d)/mcFrames,mcRhw[0],mcRhw[1],mcZ[0],mcZ[1],request.width,request.height);
            if(mcFrames) log("Menu relief cost (0.8.99.12, game thread): edge-band cut %.2f ms/frame | draws cut per frame %.1f",menuTessMs/mcFrames,double(menuTessCuts)/mcFrames);
            if(menuBiasAll) {log("Menu depth bias (0.9.8): %llu of %llu menu draws carry a game depth bias (min %.3g, max %.3g): replayed in the headset",(unsigned long long)menuBiasDraws,(unsigned long long)menuBiasAll,menuBiasDraws?double(menuBiasMin):0.,menuBiasDraws?double(menuBiasMax):0.);menuBiasAll=menuBiasDraws=0;menuBiasMin=1e9f;menuBiasMax=-1e9f;}
            menuTessMs=0;menuTessHits=menuTessMisses=menuTessCuts=0;
            mcLast=now;mcFrames=0;mcDraws=mcFlat=mc3d=mcOther=0;mcVerts=mcTris3d=mcTrisFlat=0;mcRhw[0]=mcZ[0]=1e9f;mcRhw[1]=mcZ[1]=-1e9f;mcCapture=true;mcLines=0;
        }
    }
    // 0.8.90: menu relief. The menu's pre-transformed vertices (x, y in the 640x480 game area, rhw = 1/depth) are rebuilt in 'menu space' with the game's own
    // focal / centre (fallback 554 / 320,240 when the engine values look wrong); the VR thread replays them inside a window of the front wall, in stereo.
    // Interface draws (rhw >= 1) are flattened onto a plane just in front of the wall.
    // 0.8.99.12: menu relief transform of one pre-transformed vertex (x, y in the game image, rhw) into menu space.
    //  - depth curve (0.8.96): slope MenuReliefDepth at the wall, far content tends to Ref + MenuReliefFar, near content to Ref - MenuReliefNear;
    //    0.8.99.12: + a small linear part (menuOrderK) so that two surfaces at different true depths NEVER land at (almost) the same depth: the nearest
    //    content (rim of the floor right under the game camera) was squeezed by the saturation, its surfaces fought (black patches, report 141824).
    //  - edge fade (0.8.99.5/0.8.99.11): the relief tends to menuEdgeMin of itself towards the edge of the game image.
    Vec3 menuPos(float x,float y,float rhw,const Projection& lens) const {
        const float dTrue=1.f/std::max(rhw,1e-4f);
        static const bool fastExp=iniInt(L"MenuFastExp",1)!=0;   // 0.9.47.8: libm expf (x87 double) was ~20 % of the menu CPU time; same result to ~1e-7
        auto ex=[&](float v){return fastExp?fastmath::expf_fast(v):std::exp(v);};
        float depth=dTrue>=MenuRefDepth?MenuRefDepth+menuFarK*(1.f-ex(-menuDepthK*(dTrue-MenuRefDepth)/menuFarK))
                                       :MenuRefDepth-menuNearK*(1.f-ex(-menuDepthK*(MenuRefDepth-dTrue)/menuNearK));
        if(dTrue<MenuRefDepth) depth-=menuOrderK*(MenuRefDepth-dTrue);
        float fade=1.f;
        if(menuEdge>0.f) {
            const float w2=lens.centerX*2.f,h2=lens.centerY*2.f,m=menuEdge*w2;
            const float e=std::min(std::min(x,w2-x),std::min(y,h2-y));
            const float t=std::min(1.f,std::max(0.f,e/m));fade=menuEdgeMin+(1.f-menuEdgeMin)*(t*t*(3.f-2.f*t));   // never 0: at 0 every surface of the band lands on one plane
        }
        const float depthF=MenuRefDepth+(depth-MenuRefDepth)*fade;
        return {(x-lens.centerX)*depthF/lens.focalX,-(y-lens.centerY)*depthF/lens.focalY,-depthF};
    }
    void recordMenuDraw(IDirect3DDevice9* d,D3DPRIMITIVETYPE type,UINT primitives,const void* verts,UINT stride,const void* indices,D3DFORMAT indexFormat,UINT count) {
        if(type!=D3DPT_TRIANGLELIST&&type!=D3DPT_TRIANGLESTRIP&&type!=D3DPT_TRIANGLEFAN) return;
        UINT n=count;if(!indices) n=(type==D3DPT_TRIANGLELIST)?primitives*3:primitives+2;
        DWORD fvf=0;d->GetFVF(&fvf);
        if((fvf&D3DFVF_POSITION_MASK)!=D3DFVF_XYZRHW||stride<20||stride>128||!verts||n==0||n>100000||primitives==0) return;
        // 0.9.24: resolve native menu faces before any subdivision. Dungeon / sprites never enter this function.
        static thread_local std::vector<unsigned char> menuFaceVertices;
        static thread_local std::vector<uint32_t> menuFaceIndices;
        bool pairedFlags=false;
        DWORD faceBlend=TRUE,faceWrite=FALSE;
        if(type==D3DPT_TRIANGLELIST&&fvf==0x144&&indices&&
           SUCCEEDED(d->GetRenderState(D3DRS_ALPHABLENDENABLE,&faceBlend))&&!faceBlend&&
           SUCCEEDED(d->GetRenderState(D3DRS_ZWRITEENABLE,&faceWrite))&&faceWrite) {
            const auto fixed=MenuFaces::correct(verts,stride,n,indices,indexFormat==D3DFMT_INDEX32,primitives,menuFaceVertices,menuFaceIndices);
            pairedFlags=fixed.flags!=0;
            if(fixed.flags||fixed.caps) {
                if(!menuFaceVertices.empty())verts=menuFaceVertices.data();
                indices=menuFaceIndices.data();indexFormat=D3DFMT_INDEX32;primitives=UINT(menuFaceIndices.size()/3);
                static unsigned faceLogs=0;
                if(faceLogs<12){++faceLogs;log("Menu faces (0.9.24): %u duplicate flag backs removed, %u black hanger cap triangles lit",fixed.flags,fixed.caps);}
            }
        }
        // 0.9.24: material-specific light, including the separate alpha-blended chains.
        static thread_local std::vector<unsigned char> menuMaterialVertices;
        DWORD matBlend=TRUE,matWrite=FALSE,matSrc=0,matDst=0;
        if(type==D3DPT_TRIANGLELIST&&fvf==0x144&&indices&&
           SUCCEEDED(d->GetRenderState(D3DRS_ALPHABLENDENABLE,&matBlend))&&
           SUCCEEDED(d->GetRenderState(D3DRS_ZWRITEENABLE,&matWrite))) {
            bool chains=false;
            if(matBlend&&!matWrite&&SUCCEEDED(d->GetRenderState(D3DRS_SRCBLEND,&matSrc))&&SUCCEEDED(d->GetRenderState(D3DRS_DESTBLEND,&matDst)))
                chains=matSrc==D3DBLEND_SRCALPHA&&matDst==D3DBLEND_INVSRCALPHA;
            const auto lit=MenuMaterials::correct(verts,stride,n,indices,indexFormat==D3DFMT_INDEX32,primitives,pairedFlags,!matBlend&&matWrite,chains,menuMaterialVertices);
            if(!menuMaterialVertices.empty())verts=menuMaterialVertices.data();
            static unsigned matLogs=0;
            if((lit.flags||lit.lamps||lit.chains)&&matLogs<16){++matLogs;log("Menu materials (0.9.24): %u flag, %u lantern body, %u alpha chain triangles lit",lit.flags,lit.lamps,lit.chains);}
        }
        // 0.9.24: remove only exact opposite alpha-chain faces; a visible subset is sufficient.
        static thread_local std::vector<unsigned char> menuChainVertices;
        static thread_local std::vector<uint32_t> menuChainIndices;
        if(type==D3DPT_TRIANGLELIST&&fvf==0x144&&indices&&matBlend&&!matWrite&&matSrc==D3DBLEND_SRCALPHA&&matDst==D3DBLEND_INVSRCALPHA) {
            const auto chains=MenuChains::correct(verts,stride,n,indices,indexFormat==D3DFMT_INDEX32,primitives,menuChainVertices,menuChainIndices);
            if(chains.removed){
                if(!menuChainVertices.empty())verts=menuChainVertices.data();
                indices=menuChainIndices.data();indexFormat=D3DFMT_INDEX32;primitives=UINT(menuChainIndices.size()/3);
                static unsigned chainLogs=0;if(chainLogs<16){++chainLogs;log("Menu chains (0.9.24): %u exact opposite faces removed, %u vertices lit; alpha/depth/UV preserved",chains.removed,chains.lit);}
            }
        }
        // 0.9.30: the left stone pillar of the menu room is almost black in the game's own lighting (the right one is lit). Opaque faces whose vertices fall inside
        // that screen area get a minimum vertex light (soft edges, never darker than the original): same textures, same geometry, only the light.
        static thread_local std::vector<unsigned char> menuPillarVertices;
        if(menuPillarLight&&fvf==0x144&&stride==28&&indices&&type==D3DPT_TRIANGLELIST) {
            DWORD pb=TRUE,pw=FALSE;
            if(SUCCEEDED(d->GetRenderState(D3DRS_ALPHABLENDENABLE,&pb))&&!pb&&SUCCEEDED(d->GetRenderState(D3DRS_ZWRITEENABLE,&pw))&&pw) {
                const auto* pv=static_cast<const unsigned char*>(verts);unsigned lifted=0;menuPillarVertices.clear();
                for(UINT i=0;i<n;++i) {
                    float sx,sy;memcpy(&sx,pv+size_t(i)*stride,4);memcpy(&sy,pv+size_t(i)*stride+4,4);
                    if(!std::isfinite(sx)||!std::isfinite(sy)) continue;
                    const float w=menuPillarWeight(sx/640.f,sy/480.f);if(w<=0.f) continue;
                    uint32_t c;memcpy(&c,pv+size_t(i)*stride+16,4);uint32_t out=c&0xff000000u;
                    for(unsigned sh:{0u,8u,16u}) {const unsigned ch=(c>>sh)&255u;const unsigned want=unsigned(menuPillarMin*w);out|=std::max(ch,want)<<sh;}
                    if(out==c) continue;
                    if(menuPillarVertices.empty()) menuPillarVertices.assign(pv,pv+size_t(n)*stride);
                    memcpy(menuPillarVertices.data()+size_t(i)*stride+16,&out,4);++lifted;
                }
                if(lifted) {verts=menuPillarVertices.data();menuPillarLifted+=lifted;
                    static unsigned pl=0;if(pl<4){++pl;log("Menu pillar light (0.9.30): %u vertices of the left pillar lifted to a minimum light of %.0f",lifted,menuPillarMin);}}
            }
        }
        Projection lens{};
        const bool lensOk=native.lens(lens)&&lens.focalX>100&&lens.focalX<3000&&lens.focalY>100&&lens.focalY<3000&&lens.centerX>100&&lens.centerX<700&&lens.centerY>100&&lens.centerY<500;
        if(!lensOk) lens={554.f,554.f,320.f,240.f};
        menuLens[0]=lens.focalX;menuLens[1]=lens.centerX;menuLens[2]=lens.centerY;
        if(!menuLensLogged) {menuLensLogged=true;Projection raw{};const bool got=native.lens(raw);log("Menu relief (0.8.90): engine lens %s (focal %.1f/%.1f, centre %.1f,%.1f) -> used focal %.1f, centre %.1f,%.1f",got?(lensOk?"valid":"REJECTED"):"unreadable",raw.focalX,raw.focalY,raw.centerX,raw.centerY,lens.focalX,lens.centerX,lens.centerY);}
        // 0.8.99.6/0.8.99.10: tessellation of the edge band. The relief moves every vertex along its viewing ray by a NON-linear depth (curve + edge fade);
        // inside a large triangle the interpolated surface then no longer matches the flat image. Triangles of the edge band are cut until their edges are
        // below menuTessPx. 0.8.99.10: the cut is decided PER EDGE from its two end points only (length and band), with one shared midpoint per edge:
        // two neighbours always cut their common edge the same way -> no crack (0.8.99.9 cut per triangle: T-junctions = black holes in the band).
        // Output stays indexed with shared vertices (0.8.99.9 expanded to 3 vertices per triangle: 3x the vertex data in every eye replay).
        static thread_local std::vector<unsigned char> tessVerts,tessNew;static thread_local std::vector<uint32_t> tessTri,tessOut;static thread_local std::vector<uint16_t> tessOut16;
        static thread_local std::unordered_map<uint64_t,uint32_t> tessMid;
        const double tessT0=milliseconds();bool tessApplied=false;   // 0.8.99.12: no cache (report 141824: 85 % of the draws change every frame, the camera of the menu moves)
        if(menuTessPx>=4.f&&menuEdge>0.f) {
            const bool hasD=(fvf&D3DFVF_DIFFUSE)!=0,hasS=(fvf&D3DFVF_SPECULAR)!=0;const UINT texN=(fvf&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT;
            const bool texSimple=((fvf>>16)&0xFFFF)==0;
            if(texSimple&&texN<=4&&stride<=128&&stride==16u+(hasD?4u:0u)+(hasS?4u:0u)+8u*texN&&(fvf&(D3DFVF_POSITION_MASK|D3DFVF_NORMAL|D3DFVF_PSIZE))==D3DFVF_XYZRHW) {
                const auto* src=static_cast<const unsigned char*>(verts);
                const UINT idxCount=indices?UINT(indexCountFor(type,primitives)):0;
                std::vector<uint32_t>& tri=tessTri;tri.clear();bool bad=false;
                auto idxAt=[&](UINT k)->uint32_t {if(!indices) return k;return indexFormat==D3DFMT_INDEX32?static_cast<const uint32_t*>(indices)[k]:uint32_t(static_cast<const uint16_t*>(indices)[k]);};
                const UINT lim=indices?idxCount:n;
                for(UINT p=0;p<primitives&&!bad;++p) {
                    UINT k0,k1,k2;
                    if(type==D3DPT_TRIANGLELIST) {k0=p*3;k1=p*3+1;k2=p*3+2;}
                    else if(type==D3DPT_TRIANGLESTRIP) {if(p&1) {k0=p+1;k1=p;k2=p+2;} else {k0=p;k1=p+1;k2=p+2;}}
                    else {k0=0;k1=p+1;k2=p+2;}
                    if(k0>=lim||k1>=lim||k2>=lim) {bad=true;break;}
                    const uint32_t a0=idxAt(k0),a1=idxAt(k1),a2=idxAt(k2);
                    if(a0>=n||a1>=n||a2>=n) {bad=true;break;}
                    tri.push_back(a0);tri.push_back(a1);tri.push_back(a2);
                }
                for(size_t q=0;q<tri.size()&&!bad;++q) {float t4[4];memcpy(t4,src+size_t(tri[q])*stride,16);if(!std::isfinite(t4[0])||!std::isfinite(t4[1])||!std::isfinite(t4[3])||std::fabs(t4[0])>1e5f||std::fabs(t4[1])>1e5f) bad=true;}
                const float bw2=lens.centerX*2.f,bh2=lens.centerY*2.f,band=menuEdge*bw2,lim2=menuTessPx*menuTessPx;
                std::vector<unsigned char>& pool=tessNew;pool.clear();   // midpoints only (numbers n, n+1, ...); the originals are read in place
                auto P=[&](uint32_t k)->const unsigned char* {return k<n?src+size_t(k)*stride:pool.data()+size_t(k-n)*stride;};
                auto inBandPt=[&](uint32_t k) {float q[2];memcpy(q,P(k),8);return std::min(std::min(q[0],bw2-q[0]),std::min(q[1],bh2-q[1]))<band;};
                // 0.8.99.12: an edge is cut iff one of its ends lies in the band AND it is longer than the minimum piece AND its straight middle is more than
                // menuTessTol away from where the relief really puts that point (exact test, end points only: neighbours still decide alike, no crack).
                // Before: every long edge of the band was cut (100 draws cut per frame, 1.2-2.9 ms per game frame, more triangles in every eye).
                const float tol2=menuTessTol*menuTessTol;
                // 0.9.28 PERF (exact): the relief position of a vertex and the cut decision of an edge are computed once per draw instead of at every
                // visit (an edge is tested by both its triangles, by the first scan and again in the subdivision). Same function, same inputs: same output.
                static thread_local std::vector<Vec3> posMemo;static thread_local std::vector<uint8_t> posOk;posOk.assign(n,0);posMemo.resize(n);
                static thread_local std::unordered_map<uint64_t,uint8_t> cutMemo;cutMemo.clear();
                auto posAt=[&](uint32_t k,const float* f)->Vec3 {
                    if(!menuTessMemo) return menuPos(f[0],f[1],f[3],lens);
                    if(k>=posOk.size()) {posOk.resize(k+1,0);posMemo.resize(k+1);}
                    if(!posOk[k]) {posMemo[k]=menuPos(f[0],f[1],f[3],lens);posOk[k]=1;}
                    return posMemo[k];};
                auto cutEdgeRaw=[&](uint32_t a,uint32_t b) {
                    float fa[4],fb[4];memcpy(fa,P(a),16);memcpy(fb,P(b),16);const float dx=fa[0]-fb[0],dy=fa[1]-fb[1];
                    if(dx*dx+dy*dy<=lim2||!(inBandPt(a)||inBandPt(b))) return false;
                    const Vec3 pa=posAt(a,fa),pb=posAt(b,fb);
                    const Vec3 pm=menuPos(.5f*(fa[0]+fb[0]),.5f*(fa[1]+fb[1]),.5f*(fa[3]+fb[3]),lens);   // screen middle, rhw linear in screen = the true middle point
                    const float ex=pm.x-.5f*(pa.x+pb.x),ey=pm.y-.5f*(pa.y+pb.y),ez=pm.z-.5f*(pa.z+pb.z);
                    const float rel=menuTessTolRel*std::max(0.f,-pm.z-MenuRefDepth);   // BEHIND the wall plane (far content) a few mm are invisible: the tolerance grows with the depth there; in front (near rim, floor under the camera) the strict tolerance stays
                    return ex*ex+ey*ey+ez*ez>tol2+rel*rel;};
                auto cutEdge=[&](uint32_t a,uint32_t b)->bool {
                    if(!menuTessMemo) return cutEdgeRaw(a,b);
                    const uint64_t key=a<b?(uint64_t(a)<<32|b):(uint64_t(b)<<32|a);
                    auto it=cutMemo.find(key);if(it!=cutMemo.end()) return it->second!=0;
                    const bool r=cutEdgeRaw(a,b);cutMemo.emplace(key,uint8_t(r));return r;};
                bool need=false;
                if(!bad&&!tri.empty()) {
                    for(size_t q=0;q+2<tri.size()&&!need;q+=3) need=cutEdge(tri[q],tri[q+1])||cutEdge(tri[q+1],tri[q+2])||cutEdge(tri[q+2],tri[q]);
                }
                if(need) {
                    tessMid.clear();std::vector<uint32_t>& out=tessOut;out.clear();
                    // midpoint of the SAME edge = the same vertex (key: sorted end numbers); its value is symmetric in (a,b) anyway (identical for duplicated vertices)
                    auto mid=[&](uint32_t ka,uint32_t kb)->uint32_t {
                        const uint64_t key=ka<kb?(uint64_t(ka)<<32|kb):(uint64_t(kb)<<32|ka);
                        auto it=tessMid.find(key);if(it!=tessMid.end()) return it->second;
                        unsigned char A[128],B[128],o[128];memcpy(A,P(std::min(ka,kb)),stride);memcpy(B,P(std::max(ka,kb)),stride);
                        float fa[4],fb[4],fo[4];memcpy(fa,A,16);memcpy(fb,B,16);
                        for(int j=0;j<4;++j) fo[j]=.5f*(fa[j]+fb[j]);memcpy(o,fo,16);
                        UINT off=16;
                        for(UINT j=0;j<(hasD?1u:0u)+(hasS?1u:0u);++j,off+=4) for(int c=0;c<4;++c) o[off+c]=(unsigned char)((unsigned(A[off+c])+unsigned(B[off+c])+1u)/2u);
                        const float wsum=fa[3]+fb[3];
                        for(UINT j=0;j<texN;++j,off+=8) for(int c=0;c<2;++c) {
                            float ua,ub;memcpy(&ua,A+off+c*4,4);memcpy(&ub,B+off+c*4,4);
                            const float u=wsum>1e-12f?(ua*fa[3]+ub*fb[3])/wsum:.5f*(ua+ub);memcpy(o+off+c*4,&u,4);   // perspective-correct
                        }
                        const uint32_t id=n+uint32_t(pool.size()/stride);pool.insert(pool.end(),o,o+stride);tessMid.emplace(key,id);return id;
                    };
                    struct T3 {uint32_t a,b,c;int depth;};
                    static thread_local std::vector<T3> stack;stack.clear();
                    for(size_t q=0;q+2<tri.size();q+=3) stack.push_back({tri[q],tri[q+1],tri[q+2],0});
                    const size_t maxTris=60000;
                    while(!stack.empty()) {
                        const T3 t=stack.back();stack.pop_back();
                        const bool big=t.depth<8&&out.size()/3+stack.size()<maxTris;
                        const bool cab=big&&cutEdge(t.a,t.b),cbc=big&&cutEdge(t.b,t.c),cca=big&&cutEdge(t.c,t.a);
                        const int nc=int(cab)+int(cbc)+int(cca);const int dd=t.depth+1;
                        if(nc==0) {out.push_back(t.a);out.push_back(t.b);out.push_back(t.c);continue;}
                        if(nc==3) {const uint32_t m1=mid(t.a,t.b),m2=mid(t.b,t.c),m3=mid(t.c,t.a);
                            stack.push_back({t.a,m1,m3,dd});stack.push_back({m1,t.b,m2,dd});stack.push_back({m3,m2,t.c,dd});stack.push_back({m1,m2,m3,dd});continue;}
                        // rotate so that the cut edges come first: (x,y,z) with x-y cut (and y-z cut when two)
                        uint32_t x=t.a,y=t.b,z=t.c;
                        if(nc==1) {if(cbc) {x=t.b;y=t.c;z=t.a;} else if(cca) {x=t.c;y=t.a;z=t.b;}
                            const uint32_t m=mid(x,y);stack.push_back({x,m,z,dd});stack.push_back({m,y,z,dd});continue;}
                        if(!cca) {x=t.a;y=t.b;z=t.c;} else if(!cab) {x=t.b;y=t.c;z=t.a;} else {x=t.c;y=t.a;z=t.b;}   // cut: x-y and y-z
                        const uint32_t m1=mid(x,y),m2=mid(y,z);
                        stack.push_back({m1,y,m2,dd});stack.push_back({x,m1,m2,dd});stack.push_back({x,m2,z,dd});
                    }
                    const uint32_t nv=n+uint32_t(pool.size()/stride);
                    if(out.size()>=3&&nv>0) {
                        tessApplied=true;tessVerts.assign(src,src+size_t(n)*stride);tessVerts.insert(tessVerts.end(),pool.begin(),pool.end());verts=tessVerts.data();n=nv;primitives=UINT(out.size()/3);type=D3DPT_TRIANGLELIST;
                        if(nv<=65535) {tessOut16.assign(out.begin(),out.end());indices=tessOut16.data();indexFormat=D3DFMT_INDEX16;}
                        else {indices=out.data();indexFormat=D3DFMT_INDEX32;}
                        if(!menuTessLogged) {menuTessLogged=true;log("Menu relief (0.8.99.12): adaptive crack-free tessellation on (pieces >= %.0f px, indexed), first draw %u -> %u triangles, %u -> %u vertices",menuTessPx,unsigned(tri.size()/3),unsigned(out.size()/3),unsigned(count?count:tri.size()),unsigned(nv));}
                    }
                }
            }
        }
        if(tessApplied) ++menuTessCuts;
        menuTessMs+=milliseconds()-tessT0;
        const auto* input=static_cast<const unsigned char*>(verts);
        const UINT targetStride=stride-4;
        DrawRecord rec{type,primitives,n,targetStride,(fvf&~D3DFVF_POSITION_MASK)|D3DFVF_XYZ,arena.size(),indexArena.size(),indexFormat,indices!=nullptr,0};
        rec.menuState.capture(d);
        static unsigned nativeStateLogs=0;
        if(nativeStateLogs<8) {++nativeStateLogs;log("Menu native states (0.9.10): depth enable %lu func %lu, fog enable %lu color %08lX table %lu vertex %lu",(unsigned long)rec.menuState.value[0],(unsigned long)rec.menuState.value[1],(unsigned long)rec.menuState.value[2],(unsigned long)rec.menuState.value[3],(unsigned long)rec.menuState.value[4],(unsigned long)rec.menuState.value[5]);}
        {DWORD bb=0;if(SUCCEEDED(d->GetRenderState(D3DRS_DEPTHBIAS,&bb))) {float fb;memcpy(&fb,&bb,4);if(std::isfinite(fb)) rec.depthBias=fb;}
         ++menuBiasAll;if(rec.depthBias!=0.f) {++menuBiasDraws;menuBiasMin=std::min(menuBiasMin,rec.depthBias);menuBiasMax=std::max(menuBiasMax,rec.depthBias);}}
        arena.resize(arena.size()+size_t(n)*targetStride);
        unsigned char* out=arena.data()+rec.vertices;
        rec.screenVertices=screenArena.size();screenArena.resize(screenArena.size()+size_t(n)*stride);memcpy(screenArena.data()+rec.screenVertices,input,size_t(n)*stride);
        for(UINT i=0;i<n;++i) {
            float v[4];memcpy(v,input+size_t(i)*stride,16);
            if(!std::isfinite(v[0])||!std::isfinite(v[1])||!std::isfinite(v[3])) {arena.resize(rec.vertices);screenArena.resize(rec.screenVertices);return;}
            // 0.8.96: every vertex goes through ONE smooth, monotonic depth curve (report 20261006_015307: torn map, near floor flat on the wall).
            // Before: vertices at rhw >= 1 were flattened onto the wall plane (no 2D draw exists in the menu: those were the NEAREST 3D vertices,
            // e.g. the map border, rhw up to 16) and far vertices were cut at Ref+2.5: a triangle with one vertex flattened or cut was folded.
            // Now: slope MenuReliefDepth at the wall, far content tends to Ref + MenuReliefFar, near content to Ref - MenuReliefNear (it can never
            // come out of the wall by more than that, nor reach the virtual camera).
            const Vec3 pos=menuPos(v[0],v[1],v[3],lens);   // 0.8.99.12: one transform shared with the tessellation test
            memcpy(out+size_t(i)*targetStride,&pos,12);memcpy(out+size_t(i)*targetStride+12,input+size_t(i)*stride+16,stride-16);
        }
        if(indices) {
            const size_t bytes=size_t(indexCountFor(type,primitives))*(indexFormat==D3DFMT_INDEX32?4:2);
            indexArena.resize(indexArena.size()+bytes);memcpy(indexArena.data()+rec.indices,indices,bytes);
        }
        Snapshot current{};
        if(!current.capture(d)) {current.release();arena.resize(rec.vertices);screenArena.resize(rec.screenVertices);indexArena.resize(rec.indices);return;}
        std::array<uint32_t,4> ids{};
        {CopyCache::Guard guard;guard.d=d;guard.cache=&copies;
         for(int i=0;i<Snapshot::Samplers;++i) ids[i]=copies.copyFor(d,guard,current.texture[i]);
         ids[3]=shaders.idFor(current.pixel);}
        if(!snapshots.empty()&&snapshots.back().same(current)&&snapIds.back()==ids) {current.release();rec.snapshot=uint32_t(snapshots.size()-1);}
        else {snapshots.push_back(current);snapIds.push_back(ids);rec.snapshot=uint32_t(snapshots.size()-1);}
        records.push_back(rec);++statMenuRecords;
    }
    // 0.8.92: message / page / options-screen states belong to one game session. Kept across the menu (e.g. a level left through its options menu),
    // they sustained themselves in the next game (stale box -> every piece 'in the box' -> message state on) and the HUD stayed in its 'panel open'
    // layout (bars spread apart, 'UI too big') until the options menu was opened and closed again (journal 161000: message state in 100 % of frames).
    void resetHudStates() {
        PageKeys::reset();   // 0.8.94.7: a new table session never starts with the objectives page open
        InterlockedExchange(&PauseWatch::paused,0);   // 0.8.94.4: a new table session is never paused (report 230831: the flag of the pause before 'Quit' stayed 1 -> options layout at new game)
        msgHold=objHold=pageHold=screenHold=0;msgShown=false;pageSeen=false;msgFrame=false;screenActive=false;screenWhole=false;panelWhole=false;scan.open=false;scan.onRun=scan.offRun=0;
        heldPrev=heldModelSeen=false;msgRegion={0,0,0,0};allPieces.clear();objLogs=0;
        prevBox[0]=prevBox[1]=1e9f;prevBox[2]=prevBox[3]=-1e9f;
        boxTailY1=-1e9f;prevTail=-1e9f;boxX0=boxY0=1e9f;boxX1=boxY1=-1e9f;msgX0=msgY0=1e9f;msgX1=msgY1=-1e9f;allX0=allY0=1e9f;allX1=allY1=-1e9f;midX0=midY0=1e9f;midX1=midY1=-1e9f;
    }
    // 0.8.90.9 diagnostic: F11 in the menu saves, in Captures (DK2VR_calque_NNN_menu-*.bmp, 640x480): the decor read back by the game before its
    // interface, every 640x480 surface the game writes during that frame (content at unlock), and the presented screen of that frame and of the next
    // one (centre of each block of the x3 stretch). These exact images decide how the interface is separated from the decor.
    std::set<IDirect3DBaseTexture9*> texDumped;
    // 0.9.41 diagnostic: game texture (level 0) and its headset copy, side by side for the transparent draws of the F11 frame
    void dumpTexturePair(IDirect3DDevice9* d,IDirect3DTexture9* src,uint32_t copyId) {
        std::error_code ec;auto folder=configurationPath().parent_path()/L"Captures";std::filesystem::create_directories(folder,ec);
        D3DSURFACE_DESC desc{};if(FAILED(src->GetLevelDesc(0,&desc))||desc.Width>512||desc.Height>512) return;
        auto save=[&](IDirect3DSurface9* sys,const wchar_t* tag){D3DLOCKED_RECT lr{};if(FAILED(sys->LockRect(&lr,nullptr,D3DLOCK_READONLY))) return false;
            wchar_t n[96];swprintf(n,96,L"DK2VR_tex_%03u_%04u_%ls.bmp",geomDumps+1,unsigned(texDumped.size()),tag);
            if(desc.Format==D3DFMT_A8R8G8B8||desc.Format==D3DFMT_X8R8G8B8) writeBmp32(folder/n,desc.Width,desc.Height,static_cast<const uint32_t*>(lr.pBits),lr.Pitch/4);
            sys->UnlockRect();return true;};
        bool a=false,b=false;
        {D3DLOCKED_RECT lr{};if(SUCCEEDED(src->LockRect(0,&lr,nullptr,D3DLOCK_READONLY))) {
            wchar_t n[96];swprintf(n,96,L"DK2VR_tex_%03u_%04u_jeu.bmp",geomDumps+1,unsigned(texDumped.size()));
            if(desc.Format==D3DFMT_A8R8G8B8||desc.Format==D3DFMT_X8R8G8B8) writeBmp32(folder/n,desc.Width,desc.Height,static_cast<const uint32_t*>(lr.pBits),lr.Pitch/4);
            src->UnlockRect(0);a=true;}}
        if(TextureCopy* c=copies.find(copyId)) {
            ComPtr<IDirect3DSurface9> cs,sys;D3DSURFACE_DESC cd{};
            if(SUCCEEDED(c->texture->GetSurfaceLevel(0,&cs))&&SUCCEEDED(cs->GetDesc(&cd))&&SUCCEEDED(d->CreateOffscreenPlainSurface(cd.Width,cd.Height,cd.Format,D3DPOOL_SYSTEMMEM,&sys,nullptr))&&SUCCEEDED(d->GetRenderTargetData(cs.Get(),sys.Get()))) {desc.Width=cd.Width;desc.Height=cd.Height;desc.Format=cd.Format;b=save(sys.Get(),L"copie");}
        }
        if(geomFile) fprintf(geomFile,"B,%04u,%p,%u,%d,%d,%d\n",unsigned(texDumped.size()),(void*)src,copyId,int(desc.Format),int(a),int(b));
    }
    static void writeBmp32(const std::filesystem::path& path,UINT w,UINT h,const uint32_t* px,size_t stride) {
        FILE* f=_wfopen(path.c_str(),L"wb");if(!f) return;
        const uint32_t bytes=w*h*4;unsigned char hd[54]{};hd[0]='B';hd[1]='M';uint32_t sz=54+bytes,off=54,dib=40;int32_t hh=-int32_t(h);uint16_t pl=1,bp=32;
        memcpy(hd+2,&sz,4);memcpy(hd+10,&off,4);memcpy(hd+14,&dib,4);memcpy(hd+18,&w,4);memcpy(hd+22,&hh,4);memcpy(hd+26,&pl,2);memcpy(hd+28,&bp,2);memcpy(hd+34,&bytes,4);
        fwrite(hd,1,54,f);std::vector<uint32_t> row(w);
        for(UINT y=0;y<h;++y) {for(UINT x=0;x<w;++x) row[x]=px[size_t(y)*stride+x]|0xff000000u;fwrite(row.data(),4,w,f);}
        fclose(f);
    }
    void saveScreen640(IDirect3DDevice9* d,const std::filesystem::path& path,UINT dw,UINT dh) {
        ComPtr<IDirect3DSurface9> back,sys;if(FAILED(d->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&back))) return;
        D3DSURFACE_DESC bd{};back->GetDesc(&bd);
        if(FAILED(d->CreateOffscreenPlainSurface(bd.Width,bd.Height,bd.Format,D3DPOOL_SYSTEMMEM,&sys,nullptr))||FAILED(d->GetRenderTargetData(back.Get(),sys.Get()))) {log("Menu diagnostic: screen read-back failed");return;}
        D3DLOCKED_RECT lr{};if(FAILED(sys->LockRect(&lr,nullptr,D3DLOCK_READONLY))) return;
        const float s=float(bd.Height)/float(dh),cw=float(dw)*s,cx0=(float(bd.Width)-cw)*.5f;
        std::vector<uint32_t> px(size_t(dw)*dh);
        for(UINT y=0;y<dh;++y) {const UINT sy=std::min<UINT>(bd.Height-1,UINT((y+.5f)*s));const uint32_t* row=reinterpret_cast<const uint32_t*>(static_cast<const unsigned char*>(lr.pBits)+size_t(sy)*lr.Pitch);
            for(UINT x=0;x<dw;++x) {const UINT sx=std::min<UINT>(bd.Width-1,UINT(cx0+(x+.5f)*s));px[size_t(y)*dw+x]=row[sx];}}
        sys->UnlockRect();writeBmp32(path,dw,dh,px.data(),dw);
        log("Menu diagnostic: presented screen %ux%u format %u saved as %ux%u (block centres, scale %.3f, left %.1f)",bd.Width,bd.Height,unsigned(bd.Format),dw,dh,s,cx0);
    }
    int menuDumpIndex=0;
    void menuDiagnosticDump(IDirect3DDevice9* d) {
        if(MenuTap::dumpStage==0) {
            if(!uiDumpPending||!native.frontend()) return;
            uiDumpPending=false;menuDumpIndex=++uiDumpCount;MenuTap::resetWrites();MenuTap::dumpStage=1;
            log("Menu diagnostic: F11 -> capturing the next game frame (calque %d)",menuDumpIndex);return;
        }
        std::error_code ec;auto folder=configurationPath().parent_path()/L"Captures";std::filesystem::create_directories(folder,ec);
        auto name=[&](const wchar_t* tag){wchar_t n[96];swprintf(n,96,L"DK2VR_calque_%03d_menu-%ls.bmp",menuDumpIndex,tag);return folder/n;};
        const UINT dw=MenuTap::w?MenuTap::w:640,dh=MenuTap::h?MenuTap::h:480;
        if(MenuTap::dumpStage==1) {
            MenuTap::dumpStage=0;   // stop listening before writing (the writes below lock nothing of the game)
            if(MenuTap::fresh&&MenuTap::pixels.size()==size_t(dw)*dh) {writeBmp32(name(L"1-decor"),dw,dh,MenuTap::pixels.data(),dw);log("Menu diagnostic: decor read-back %ux%u saved",dw,dh);}
            else log("Menu diagnostic: NO decor read-back in this frame (fresh=%d, %zu pixels)",int(MenuTap::fresh),MenuTap::pixels.size());
            int k=0;for(auto& c:MenuTap::wcap) {if(c.done&&c.px.size()==size_t(c.w)*c.h) {wchar_t tag[24];swprintf(tag,24,L"3-ecrit%d",k);writeBmp32(name(tag),c.w,c.h,c.px.data(),c.w);
                log("Menu diagnostic: written surface %d (%p, %ux%u, %u locks) saved",k,(void*)c.s,c.w,c.h,c.locks);}
                else if(c.s) log("Menu diagnostic: written surface %d (%p) locked %u times, never unlocked in the frame",k,(void*)c.s,c.locks);++k;}
            MenuTap::resetWrites();
            saveScreen640(d,name(L"2-ecran-a"),dw,dh);
            MenuTap::dumpStage=2;return;
        }
        if(MenuTap::dumpStage==2) {saveScreen640(d,name(L"4-ecran-b"),dw,dh);MenuTap::dumpStage=0;log("Menu diagnostic: calque %d complete",menuDumpIndex);}
    }
    // 0.8.88: menu package. No dungeon, no interface: only the game screen (copied from the back buffer) travels to the VR thread, which draws the empty
    // room around it; the host places the screen on the front wall and points the Keeper's mouse on it.
    void presentDecoupledMenu(IDirect3DDevice9* d) {
        publishPending(d,false);
        resetHudStates();   // 0.8.92
        menuDiagnosticDump(d);
        menuCensusEnd();
        noteAnchored();
        statsDraw+=drawMilliseconds;statsPresent+=presentMilliseconds;presentMilliseconds=0;++statsFrames;
        auto p=std::make_shared<Package>();
        p->id=packageId;p->epoch=vr->epoch;p->recordedMs=milliseconds();p->menu=true;
        uploadKeeper(d,*p);p->hudSizes=hudSizes;
        p->table=request.table;p->tableWidth=request.tableWidthMeters>0.f?request.tableWidthMeters:1.f;p->tableDepth=p->tableWidth*.75f;
        p->world=0;p->ui=ui;p->rejected=0;p->mode=mode;
        bool haveScreen=false;
        ComPtr<IDirect3DSurface9> back;
        if(SUCCEEDED(d->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&back))) {
            D3DSURFACE_DESC desc{};back->GetDesc(&desc);
            p->desktopWidth=desc.Width;p->desktopHeight=desc.Height;
            p->desktop=copies.desktop(d,back.Get(),desc.Width,desc.Height);
            if(auto* c=copies.find(p->desktop)) {p->desktopHandle=c->handle;haveScreen=true;}
        }
        if(menuRelief&&!records.empty()&&menuLens[0]>0) {   // 0.8.90: the menu's 3D draws travel with the package
            p->menuFocal=menuLens[0];p->menuCx=menuLens[1];p->menuCy=menuLens[2];
            if(menuUiCpu) menuSplitFrame(p.get());   // 0.8.95: 2D layer separated here, exact (same frame, same grid)
            else {   // 0.8.90.8: the presented image may lag the read-back by a frame or two: the last three read-backs travel, the VR thread measures which one matches
                std::shared_ptr<const std::vector<uint32_t>> cur;
                if(MenuTap::fresh) {auto v=std::make_shared<std::vector<uint32_t>>();v->swap(MenuTap::pixels);cur=std::move(v);MenuTap::fresh=false;}
                MenuTap::ring[2]=std::move(MenuTap::ring[1]);MenuTap::ring[1]=std::move(MenuTap::ring[0]);MenuTap::ring[0]=std::move(cur);
                for(int k=0;k<3;++k) p->menuDecorLag[k]=MenuTap::ring[k];
                p->menuDecorW=MenuTap::w;p->menuDecorH=MenuTap::h;
            }
            p->vertices.swap(arena);p->screenVerts.swap(screenArena);p->indices.swap(indexArena);joinRecordedBadges();p->records.swap(records);p->digitGroups.swap(digitGroupsNow);
            p->snapshots.resize(snapshots.size());
            std::vector<uint32_t> refs,shaderRefs;refs.reserve(snapshots.size()*2);
            for(size_t i=0;i<snapshots.size();++i) {
                auto& in=snapshots[i];auto& out=p->snapshots[i];
                memcpy(out.rs,in.rs,sizeof(out.rs));memcpy(out.ts,in.ts,sizeof(out.ts));memcpy(out.ss,in.ss,sizeof(out.ss));
                for(int t=0;t<Snapshot::Samplers;++t) {out.copy[t]=snapIds[i][t];if(out.copy[t]) refs.push_back(out.copy[t]);}
                out.shader=snapIds[i][3];if(out.shader) shaderRefs.push_back(out.shader);
            }
            std::sort(refs.begin(),refs.end());refs.erase(std::unique(refs.begin(),refs.end()),refs.end());
            for(uint32_t id:refs) if(auto* c=copies.find(id)) p->copies.push_back({id,c->handle,c->w,c->h,c->levels});
            std::sort(shaderRefs.begin(),shaderRefs.end());shaderRefs.erase(std::unique(shaderRefs.begin(),shaderRefs.end()),shaderRefs.end());
            for(uint32_t id:shaderRefs) {auto it=shaders.code.find(id);if(it!=shaders.code.end()) p->shaders.push_back({id,it->second});}
        }
        MenuTap::fresh=false;MenuTap::armed.store((menuRelief||MenuTap::dumpStage==1)&&native.frontend(),std::memory_order_relaxed);   // 0.8.90.7: listen for the next frame's decor read-back
        MenuTap::clearFinal();MenuTap::finalArmed.store(menuRelief&&menuUiCpu&&native.frontend(),std::memory_order_relaxed);   // 0.8.95: and for its final image
        clearBatch();finishPick();pickActive=false;screenCleared=false;uiCleared=false;cursorFound=false;cursorBacked=false;heldCleared=false;holdingThings=false;heldModel=false;
        if(!haveScreen) {++dropped;if(GetTickCount64()-lastStatus>5000) statusLog();return;}
        pollPackage(true);
        auto& q=packageFences[p->id&1];
        HRESULT hr=q?S_OK:d->CreateQuery(D3DQUERYTYPE_EVENT,&q);
        if(SUCCEEDED(hr)) hr=q->Issue(D3DISSUE_END);
        if(SUCCEEDED(hr)) {q->GetData(nullptr,0,D3DGETDATA_FLUSH);pendingPackage=std::move(p);++statPackages;}
        else {log("Package fence failed %08lX",(unsigned long)hr);++dropped;}
        double since=std::max(vr->lastPublishMs.load(),decoupledSince);
        if(vr->active&&handed>0&&milliseconds()-since>2000) {
            ++watchdogTrips;vr->active=false;
            if(watchdogTrips>=4) vr->failed=true; else retryAt=milliseconds()+5000;
            log("Decoupled VR rendering: no headset image from the VR thread for %.0f ms (menu room); 0.2.9 path %s",milliseconds()-since,vr->failed?"from now on":"for 5 s, then retry");
        }
        if(GetTickCount64()-lastStatus>5000) statusLog();
    }
    void presentDecoupled(IDirect3DDevice9* d) {
        publishPending(d,false); // a 0.2.9 image possibly still waiting from the previous frame
        // Pan state that produced THIS frame's native camera (the tick below moves the next frame's camera).
        const bool panning=control.panning;const int panHand=control.hand;const Vec3 panFrom=control.lastHand;
        const bool turning=control.rotating;const float turnFrom=control.lastAngle;
        const bool stickOn=control.stickDriven;const Vec3 stickFrom=control.appliedHand;const float stickTurnFrom=control.appliedAngle; // 0.8.66
        Vec3 pivot{0,0,0};
        if(anchored) {
            Pose camInTable=nativeCameraInTable(table);
            control.measure(camInTable,anchored);
            // Native rotation pivot: ground point at the centre of the game view (table frame).
            float along=dot(table.up,Vec3{0,0,-1});
            if(std::fabs(along)>1e-3f) {
                Vec3 ground=scale(Vec3{0,0,-1},dot(table.up,table.center)/along);
                Vec3 q=toWorld(camInTable,ground);
                if(std::isfinite(q.x)&&std::isfinite(q.z)&&std::fabs(q.x)<5&&std::fabs(q.z)<5) pivot={q.x,0,q.z};
            }
        }
        control.tick(request,tableCamera,activeUnits,anchored,mode);
        noteAnchored();
        statsDraw+=drawMilliseconds;statsPresent+=presentMilliseconds;presentMilliseconds=0;++statsFrames;statsWorld+=world;
        if(!anchored||records.empty()) {
            clearBatch();++dropped;
            if(GetTickCount64()-lastStatus>5000) statusLog();
            return;
        }
        double syncStarted=milliseconds();
        double partMark=syncStarted;
        auto p=std::make_shared<Package>();
        p->id=packageId;p->epoch=vr->epoch;p->recordedMs=milliseconds();
        if(heldKeep[2]>heldKeep[0]) p->heldModelRight=heldKeep[2]-cursorPx+12.f;
        p->vertices.swap(arena);p->indices.swap(indexArena);joinRecordedBadges();p->records.swap(records);p->digitGroups.swap(digitGroupsNow);
        bool hasHeld3d=false;for(const auto& rec:p->records)hasHeld3d|=rec.held3d;
        if(HeldGeometry::enabled&&keeperW>=heldMin(100)&&keeperH>=heldMin(100)&&lastPointerOnTable&&!hasHeld3d) {
            ++held3dMissingFrames;
            if(held3dMissingFrames<=8)log("Held 3D fallback frame: native depth vertices=%u, cursorKnown=%d, pointer=%.0f,%.0f, page=%d/%d, screen=%d, keeper=%dx%d",unsigned(HeldGeometry::depths.size()),int(cursorKnown),cursorPx,cursorPy,pageHold,int(pageSeen),int(screenWhole),keeperW,keeperH);
        }
        // Bring all native sub-meshes into one depth frame. The game's held
        // animation can be horizontal; only that clear case is turned upright.
        float hx0=1e9f,hy0=1e9f,hz0=1e9f,hx1=-1e9f,hy1=-1e9f,hz1=-1e9f;
        for(const auto& rec:p->records) if(rec.held3d) for(UINT i=0;i<rec.count;++i) {
            Vec3 q;memcpy(&q,p->vertices.data()+rec.vertices+size_t(i)*rec.stride,sizeof(q));
            hx0=std::min(hx0,q.x);hy0=std::min(hy0,q.y);hz0=std::min(hz0,q.z);
            hx1=std::max(hx1,q.x);hy1=std::max(hy1,q.y);hz1=std::max(hz1,q.z);
        }
        if(hx1>=hx0) {
            const float cx=(hx0+hx1)*.5f,cy=(hy0+hy1)*.5f,cz=(hz0+hz1)*.5f;
            // 0.9.47.22: NO automatic turning upright by default. The creature keeps exactly the orientation (and swing) the game gives it, like the flat image did.
            static const bool uprightRule=iniInt(L"HeldUprightRule",0)!=0;
            const bool upright=uprightRule&&heldNeedsUpright(hx1-hx0,hy1-hy0);
            static int heldBoxLogs=0;if(heldBoxLogs<12) {++heldBoxLogs;log("Held 3D model box (0.9.47.23): %.0f x %.0f px, depth %.0f px, depth sign %d, upright rule %s",hx1-hx0,hy1-hy0,hz1-hz0,heldDecider.sign,uprightRule?"on":"off");}
            if(upright)++heldUprightFrames;
            for(const auto& rec:p->records) if(rec.held3d) for(UINT i=0;i<rec.count;++i) {
                auto* at=p->vertices.data()+rec.vertices+size_t(i)*rec.stride;
                Vec3 q;memcpy(&q,at,sizeof(q));q.z-=cz;
                if(upright)q=heldUprightPoint(q,cx,cy);
                memcpy(at,&q,sizeof(q));
            }
        }
        p->snapshots.resize(snapshots.size());
        std::vector<uint32_t> refs,shaderRefs;refs.reserve(snapshots.size()*2);
        for(size_t i=0;i<snapshots.size();++i) {
            auto& in=snapshots[i];auto& out=p->snapshots[i];
            memcpy(out.rs,in.rs,sizeof(out.rs));memcpy(out.ts,in.ts,sizeof(out.ts));memcpy(out.ss,in.ss,sizeof(out.ss));
            for(int t=0;t<Snapshot::Samplers;++t) {out.copy[t]=snapIds[i][t];if(out.copy[t]) refs.push_back(out.copy[t]);}
            out.shader=snapIds[i][3];if(out.shader) shaderRefs.push_back(out.shader);
        }
        std::sort(refs.begin(),refs.end());refs.erase(std::unique(refs.begin(),refs.end()),refs.end());
        for(uint32_t id:refs) if(auto* c=copies.find(id)) p->copies.push_back({id,c->handle,c->w,c->h,c->levels});
        std::sort(shaderRefs.begin(),shaderRefs.end());shaderRefs.erase(std::unique(shaderRefs.begin(),shaderRefs.end()),shaderRefs.end());
        for(uint32_t id:shaderRefs) {auto it=shaders.code.find(id);if(it!=shaders.code.end()) p->shaders.push_back({id,it->second});}
        buildBase(p->base); // record-LOCAL
        if(tableEdgeFill) buildSkirt(p->skirt);   // 0.8.92
        p->clip=clipping();
        if(p->clip) {
            auto planes=table.planes();const Pose identity{{0,0,0,1},{0,0,0}};
            for(int i=0;i<4;++i) {auto q=transformPlane(planes[i],tableCamera,identity);float c[4]={q.normal.x,q.normal.y,q.normal.z,q.offset};memcpy(p->planes[i],c,sizeof(c));}
            // Ceiling above the dungeon walls (+0.4 game unit): interface pieces the game draws with a perspective
            // depth (info panel, icons) were reprojected high above the table (validated in the 0.4.9.2 test).
            p->ceiling=false;
            if(hideFloatingUi&&haveLevels&&activeUnits>0) {
                float h=dot(table.up,table.center)+(highOffset-(haveWalk?walkOffset:0.f)+ceilingMargin)/activeUnits;   // 0.8.91: +1.0 unit above the walls (was +0.4: wall tops / tall meshes were cut)
                Plane top{scale(table.up,-1),h};auto q=transformPlane(top,tableCamera,identity);
                float c[4]={q.normal.x,q.normal.y,q.normal.z,q.offset};memcpy(p->planes[4],c,sizeof(c));p->ceiling=true;
            }
        }
        // 0.8.57 diagnostic: how much of what the engine transforms is thrown away by the table clip planes in VR?
        if(p->clip&&probeFrame&&!possessedFrame) {
            uint64_t tris=0,culled[2]={0,0},inside=0,verts=0,vertsCulledRecords=0;
            for(const auto& rec:p->records) {
                if(rec.hand||rec.held3d||rec.type!=D3DPT_TRIANGLELIST||rec.stride<12) continue;
                const unsigned char* vb=p->vertices.data()+rec.vertices;verts+=rec.count;
                auto pos=[&](UINT i){Vec3 v;memcpy(&v,vb+size_t(i)*rec.stride,12);return v;};
                auto idx=[&](UINT n)->UINT {if(!rec.indexed) return n;const unsigned char* ib=p->indices.data()+rec.indices;
                    return rec.indexFormat==D3DFMT_INDEX32?reinterpret_cast<const uint32_t*>(ib)[n]:reinterpret_cast<const uint16_t*>(ib)[n];};
                uint64_t recTris=0,recCulled=0;
                for(UINT tr=0;tr<rec.primitives;++tr) {
                    UINT i0=idx(tr*3),i1=idx(tr*3+1),i2=idx(tr*3+2);if(i0>=rec.count||i1>=rec.count||i2>=rec.count) continue;
                    Vec3 v[3]={pos(i0),pos(i1),pos(i2)};++tris;++recTris;
                    bool allIn=true;bool cut[2]={false,false};
                    for(int k=0;k<4;++k) {
                        const float* pl=p->planes[k];int out0=0,out1=0;
                        for(int j=0;j<3;++j) {float s=pl[0]*v[j].x+pl[1]*v[j].y+pl[2]*v[j].z+pl[3];if(s<0) ++out0;if(s>0) ++out1;}
                        if(out0==3) cut[0]=true;if(out1==3) cut[1]=true;if(out0) allIn=false;
                    }
                    if(cut[0]) {++culled[0];++recCulled;}if(cut[1]) ++culled[1];if(allIn) ++inside;
                }
                if(recTris&&recCulled==recTris) vertsCulledRecords+=rec.count;
            }
            log("Engine waste (planes sign +/-): of %llu triangles (%llu vertices in %zu draws), fully outside the table: %.1f%% (sign '<0 outside') / %.1f%% (opposite sign); fully inside: %.1f%%; vertices of draws entirely outside: %.1f%%",
                (unsigned long long)tris,(unsigned long long)verts,p->records.size(),100.*culled[0]/std::max<uint64_t>(1,tris),100.*culled[1]/std::max<uint64_t>(1,tris),100.*inside/std::max<uint64_t>(1,tris),100.*vertsCulledRecords/std::max<uint64_t>(1,verts));
        }
        presParts[0]+=milliseconds()-partMark;partMark=milliseconds(); // package content (CPU)
        p->table=request.table;p->tableWidth=request.tableWidthMeters;p->tableDepth=table.halfDepth*2;
        p->panning=panning;p->panHand=panHand;p->panFrom=panFrom;
        p->turning=turning;p->turnFrom=turnFrom;p->pivot=pivot;
        p->stickOn=stickOn;p->stickFrom=stickFrom;p->stickTurnFrom=stickTurnFrom;
        p->world=world;p->ui=ui;p->rejected=rejected;p->mode=mode;
        ComPtr<IDirect3DSurface9> back;
        if(SUCCEEDED(d->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&back))) {
            D3DSURFACE_DESC desc{};back->GetDesc(&desc);
            p->desktopWidth=desc.Width;p->desktopHeight=desc.Height;
            // 0.5.16: the game screen is copied only when the headset shows it (panel F3, menus, no dungeon drawn).
            const bool needScreen=(request.handFlags&RequestPanelShown)||world==0||request.mode==3||native.frontend();
            if(needScreen) {
                p->desktop=copies.desktop(d,back.Get(),desc.Width,desc.Height);
                if(auto* c=copies.find(p->desktop)) p->desktopHandle=c->handle;
            } else {p->desktop=0;p->desktopHandle=nullptr;}
        }
        uiWatch.frame(possessedFrame,tablePrepared&&!native.frontend()&&!possessedFrame,uiCleared,gameWindow,request.valid);   // 0.9.47.39
        captureSkillBar(d,p.get(),back);   // 0.9.47.41: also called by the possession path
        if(uiCleared&&uiSurf) {
            // The cursor is shown on the controller: remove it from the panels (what lay under it is put back).
            if(cursorFound&&cursorBacked&&hudErase) {
                RECT src{0,0,cursorSaved.right-cursorSaved.left,cursorSaved.bottom-cursorSaved.top};
                d->StretchRect(cursorBackup.Get(),&src,uiSurf.Get(),&cursorSaved,D3DTEXF_NONE);
            }
            scanStep(d);   // 0.8.94.22
            if(GetTickCount64()-lastUiProbe>5000) {
                lastUiProbe=GetTickCount64();
                ComPtr<IDirect3DSurface9> sys;
                if(SUCCEEDED(d->CreateOffscreenPlainSurface(uiW,uiH,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&sys,nullptr))&&SUCCEEDED(d->GetRenderTargetData(uiSurf.Get(),sys.Get()))) {
                    D3DLOCKED_RECT lr{};
                    if(SUCCEEDED(sys->LockRect(&lr,nullptr,D3DLOCK_READONLY))) {
                        uint64_t alpha=0,color=0,n=0;
                        for(UINT y=0;y<uiH;y+=8) for(UINT x=0;x<uiW;x+=8) {DWORD px=reinterpret_cast<const DWORD*>(static_cast<const unsigned char*>(lr.pBits)+y*lr.Pitch)[x];++n;if(px>>24) ++alpha;if(px&0xffffff) ++color;}
                        sys->UnlockRect();
                        log("HUD texture: %.1f%% covered (alpha>0), %.1f%% coloured; cursor-covering draw: count=%lu indexed=%lu fvf=%08lX size=%lux%lu textured=%lu",
                            100.*alpha/std::max<uint64_t>(n,1),100.*color/std::max<uint64_t>(n,1),probeCursorInfo[0]&0xffff,probeCursorInfo[0]>>16,probeCursorInfo[1],probeCursorInfo[2]&0xffff,probeCursorInfo[2]>>16,probeCursorInfo[3]);
                    }
                }
            }
            // 0.7.6: no hand window while the objectives page is shown (a message page without a box): its text lies in the
            // interface texture and appeared on the table next to the hand (0.7.5 captures).
            const bool objectivesShown=!panelUi&&pageHold>0&&!screenWhole; // 0.8.26: real page signal (background quad), not the text unions
            // 0.9.45: report 102002 (capture 007): a held creature stayed pinned on the interface wall while the laser was on the interface (pointer off the table), because the hand window only ran with the pointer on the table
            const bool heldOffTableNow=heldOffTable&&!lastPointerOnTable&&keeperW>=heldMin(100)&&keeperH>=heldMin(60);if(heldOffTableNow) ++statHeldOffTable;
            if(heldWindow&&keeperHeld&&cursorKnown&&(lastPointerOnTable||heldOffTableNow)&&!(screenActive&&screenWhole)&&!objectivesShown) { // 0.6.9.10: held things also while a message is shown
                bool onPanel=false;float r[4][4];
                // Real bar outlines (measured 0.6.0/0.6.3), tighter than the HUD regions kept for tooltips.
                static constexpr HudRegion bars[4]={{0.f,0.f,1.f,.09f},{0.f,.693f,.73f,.760f},{0.f,.760f,.182f,1.f},{.182f,.760f,1.f,1.f}}; // 0.8.10: left bar up to .182 (the 4-pixel gap left the minimap's edge line in the hand window)
                const float iw=std::min(float(uiW),uiH*4.f/3.f),ih=std::min(float(uiH),uiW*3.f/4.f),ox=(uiW-iw)*.5f,oy=(uiH-ih)*.5f;
                for(int i=0;i<4;++i) {r[i][0]=ox+bars[i].u0*iw;r[i][1]=oy+bars[i].v0*ih;r[i][2]=ox+bars[i].u1*iw;r[i][3]=oy+bars[i].v1*ih;if(cursorPx>=r[i][0]&&cursorPx<=r[i][2]&&cursorPy>=r[i][1]&&cursorPy<=r[i][3]) onPanel=true;}
                if(!onPanel||heldOffTableNow) {   // 0.9.47.2: report 113329: the pointer lies ON the statistics bar when the laser is on the interface -> no hand window at all
                    if(!heldSurf&&(FAILED(d->CreateTexture(uiW,uiH,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&heldTex,nullptr))||FAILED(heldTex->GetSurfaceLevel(0,&heldSurf)))) {heldTex.Reset();heldSurf.Reset();}
                    if(heldSurf&&SUCCEEDED(d->StretchRect(uiSurf.Get(),nullptr,heldSurf.Get(),nullptr,D3DTEXF_NONE))) {
                        for(int i=0;i<4;++i) {RECT e{LONG(std::floor(r[i][0])),LONG(std::floor(r[i][1])),LONG(std::ceil(r[i][2])),LONG(std::ceil(r[i][3]))};
                            e.left=std::max<LONG>(0,e.left);e.top=std::max<LONG>(0,e.top);e.right=std::min<LONG>(LONG(uiW),e.right);e.bottom=std::min<LONG>(LONG(uiH),e.bottom);
                            // 0.7.0: the bars are erased EXCEPT under the held model (0.6.9.15: a creature over a bar was cut).
                            const bool keepOff=heldOffTableNow&&creatureKeep[2]>creatureKeep[0];   // 0.9.47.2: off the table the creature (only) is kept over the bar it hangs on
                            RECT k=keepOff?RECT{LONG(creatureKeep[0])-12,LONG(creatureKeep[1])-12,LONG(creatureKeep[2])+12,LONG(creatureKeep[3])+12}:RECT{LONG(heldKeep[0])-12,LONG(heldKeep[1])-12,LONG(heldKeep[2])+12,LONG(heldKeep[3])+12};
                            RECT parts[4]={e,e,e,e};int np=1;
                            if(keepOff&&k.left<e.right&&k.right>e.left&&k.top<e.bottom&&k.bottom>e.top) {
                                np=0;
                                if(k.top>e.top) parts[np++]=RECT{e.left,e.top,e.right,k.top};
                                if(k.bottom<e.bottom) parts[np++]=RECT{e.left,k.bottom,e.right,e.bottom};
                                const LONG t0=std::max(e.top,k.top),t1=std::min(e.bottom,k.bottom);
                                if(k.left>e.left) parts[np++]=RECT{e.left,t0,k.left,t1};
                                if(k.right<e.right) parts[np++]=RECT{k.right,t0,e.right,t1};
                            }
                            for(int q=0;q<np;++q) if(parts[q].right>parts[q].left&&parts[q].bottom>parts[q].top) d->ColorFill(heldSurf.Get(),&parts[q],0x00000000);}
                        // 0.8.15: a message / objectives page lies in the interface layer between the bars. While one is shown OR opening (the text
                        // of the current frame counts: 0.8.13 only knew last frame's region and text near the cursor leaked into the hand window
                        // for the first frames) the whole band between the bars is erased from the hand window.
                        if((pageHold>0||pageSeen)&&!heldOffTableNow) { // 0.8.26: only while the objectives page is open (before: any message state -> the held creature was erased)
                            RECT e{0,LONG(std::floor(oy+.085f*ih)),LONG(uiW),LONG(std::ceil(oy+.72f*ih))};
                            e.top=std::max<LONG>(0,e.top);e.bottom=std::min<LONG>(LONG(uiH),e.bottom);
                            if(e.bottom>e.top) d->ColorFill(heldSurf.Get(),&e,0x00000000);
                        }
                        if(panelUi&&scan.open) {   // 0.8.94.22: the panel's rectangle (box / page) never appears in the hand window
                            RECT e{LONG(scan.rect[0])-4,LONG(scan.rect[1])-4,LONG(scan.rect[2])+4,LONG(scan.rect[3])+4};
                            e.left=std::max<LONG>(0,e.left);e.top=std::max<LONG>(0,e.top);e.right=std::min<LONG>(LONG(uiW),e.right);e.bottom=std::min<LONG>(LONG(uiH),e.bottom);
                            if(e.right>e.left&&e.bottom>e.top) d->ColorFill(heldSurf.Get(),&e,0x00000000);
                        }
                        if(handBox[2]>handBox[0]&&handBox[3]>handBox[1]&&!heldOffTableNow) {   // 0.9.47.3: not while a creature is held with the laser on the interface (report 114545: the false 'box' erased the creature) // 0.8.94.34: notification box background(s) of this frame: never in the hand window
                            RECT e{LONG(std::floor(handBox[0]))-6,LONG(std::floor(handBox[1]))-6,LONG(std::ceil(handBox[2]))+6,LONG(std::ceil(handBox[3]))+6};
                            e.left=std::max<LONG>(0,e.left);e.top=std::max<LONG>(0,e.top);e.right=std::min<LONG>(LONG(uiW),e.right);e.bottom=std::min<LONG>(LONG(uiH),e.bottom);
                            // the held creature (its own box + 12 px) is never erased: the box rectangle is cut around it
                            RECT parts[4]={e,e,e,e};int np=1;
                            if(creatureKeep[2]>creatureKeep[0]) {RECT k{LONG(creatureKeep[0])-12,LONG(creatureKeep[1])-12,LONG(creatureKeep[2])+12,LONG(creatureKeep[3])+12};
                                if(k.left<e.right&&k.right>e.left&&k.top<e.bottom&&k.bottom>e.top) {np=0;
                                    if(k.top>e.top) parts[np++]=RECT{e.left,e.top,e.right,k.top};
                                    if(k.bottom<e.bottom) parts[np++]=RECT{e.left,k.bottom,e.right,e.bottom};
                                    const LONG t0=std::max(e.top,k.top),t1=std::min(e.bottom,k.bottom);
                                    if(k.left>e.left) parts[np++]=RECT{e.left,t0,k.left,t1};
                                    if(k.right<e.right) parts[np++]=RECT{k.right,t0,e.right,t1};}}
                            for(int q=0;q<np;++q) if(parts[q].right>parts[q].left&&parts[q].bottom>parts[q].top) d->ColorFill(heldSurf.Get(),&parts[q],0x00000000);
                            ++statHandBoxFrames;
                            if(handBoxLogs<20) {++handBoxLogs;log("Hand window (0.8.94.34): notification box %.0f,%.0f-%.0f,%.0f erased from the hand window (cursor %.0f,%.0f, creature box %s, %d part(s))",
                                handBox[0],handBox[1],handBox[2],handBox[3],cursorPx,cursorPy,creatureKeep[2]>creatureKeep[0]?"kept":"none",np);}
                        }
                        // (0.6.9.9-0.6.9.10 erased the message box here; 0.6.9.11: the box is no longer in the interface texture.)
                        heldCleared=true;heldPx=cursorPx;heldPy=cursorPy;
                    }
                }
            }
            if(uiDumpPending) { // 0.8.8: F11 also saves the interface layer and the hand window (BMP, grey behind the transparency) in the Captures folder (0.8.16: a trailing backslash here commented out the next line -> saved EVERY frame)
                uiDumpPending=false;++uiDumpCount;
                std::error_code ec;auto folder=configurationPath().parent_path()/L"Captures";std::filesystem::create_directories(folder,ec);
                auto save=[&](IDirect3DSurface9* surf,const wchar_t* tag) {
                    if(!surf) return;D3DSURFACE_DESC dd{};surf->GetDesc(&dd);
                    ComPtr<IDirect3DSurface9> sys;
                    if(FAILED(d->CreateOffscreenPlainSurface(dd.Width,dd.Height,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&sys,nullptr))||FAILED(d->GetRenderTargetData(surf,sys.Get()))) return;
                    D3DLOCKED_RECT lr{};if(FAILED(sys->LockRect(&lr,nullptr,D3DLOCK_READONLY))) return;
                    wchar_t name[96];swprintf(name,96,L"DK2VR_calque_%03d_%ls.bmp",uiDumpCount,tag);
                    FILE* f=_wfopen((folder/name).c_str(),L"wb");
                    if(f) {
                        const uint32_t bytes=dd.Width*dd.Height*4;const uint32_t hdr[]={0x4d42|(uint32_t(54+bytes)<<16),0,0,54};
                        unsigned char h[54]{};h[0]='B';h[1]='M';uint32_t sz=54+bytes,off=54,dib=40,w=dd.Width;int32_t hh=-int32_t(dd.Height);uint16_t pl=1,bp=32;
                        memcpy(h+2,&sz,4);memcpy(h+10,&off,4);memcpy(h+14,&dib,4);memcpy(h+18,&w,4);memcpy(h+22,&hh,4);memcpy(h+26,&pl,2);memcpy(h+28,&bp,2);memcpy(h+34,&bytes,4);(void)hdr;
                        fwrite(h,1,54,f);std::vector<uint32_t> row(dd.Width);
                        for(UINT y=0;y<dd.Height;++y) {
                            const uint32_t* src=reinterpret_cast<const uint32_t*>(static_cast<const unsigned char*>(lr.pBits)+size_t(y)*lr.Pitch);
                            for(UINT x=0;x<dd.Width;++x) {
                                const uint32_t px=src[x];const uint32_t a=px>>24;
                                auto mix=[&](uint32_t c,uint32_t g){return std::min<uint32_t>(255,c+g*(255-a)/255);};
                                row[x]=0xff000000u|(mix((px>>16)&255,96)<<16)|(mix((px>>8)&255,102)<<8)|mix(px&255,110);
                            }
                            fwrite(row.data(),4,dd.Width,f);
                        }
                        fclose(f);
                    }
                    sys->UnlockRect();
                };
                save(uiSurf.Get(),L"interface");if(heldCleared&&heldSurf) save(heldSurf.Get(),L"main");if(screenActive&&screenCleared&&screenSurf) save(screenSurf.Get(),L"mur");
                log("Interface layer / hand window / wall panel saved in Captures (calque %d)",uiDumpCount);
            }
            p->pickDistance=lastPickDist;
            {const float iw=std::min(float(uiW),uiH*4.f/3.f),ih=std::min(float(uiH),uiW*.75f);
             {const float kw=screenActive?.30f:.40f,kh=screenActive?.20f:.30f;const bool big=midX1-midX0>kw*iw&&midY1-midY0>kh*ih;
              // 0.6.9.7: with the pause watch, the panel is shown exactly while the game is paused (options screen).
              if(PauseWatch::paused>=0) screenHold=PauseWatch::paused?30:0;
              else if(big) screenHold=30; else if(screenHold>0) --screenHold;
              // ---------------------------------------------------------------------------------------------------------------------------
              // 0.8.94.7: ONE rule decides the middle panel. It can be opened by THREE named things only, and by nothing else:
              //   A. the game's own menu / pause        -> PauseWatch (the hooked pause function 0x527270), whole band
              //   B. the objectives page ('0')          -> PageKeys (the key read in the window procedure), whole band
              //   C. a real notification box seen THIS frame -> bigBox (boxX0..boxY1), the box rectangle only
              // Every pixel-union heuristic (0.8.21 ... 0.8.94.6) is now DIAGNOSTIC ONLY: each one eventually stayed true for ever and the panel
              // never closed again ('UI de la barre de vie ouverte de base'). With no menu, no page and no box, the panel is CLOSED. Always.
              const bool gameMenu=screenHold>0;
              // The page is open when those big strips cover nearly the WHOLE width of the band. A notification box is <= 70 % wide (bigBoxRaw) and the
              // held-creature union of report 20261004_160911 reached 55 %: neither can reach 85 %.
              const bool pageDrawn=(pgX1-pgX0>=.85f*uiW&&pgY1-pgY0>=.45f*ih&&pgPieces>=3)||(pgTopLeft&&pgY1-pgY0>=.20f*ih&&PauseWatch::paused!=1);   // 0.8.94.19: short page of a new game
              // confirmed over 5 frames in a row before opening, so a single odd frame can never open it; the 240-frame failsafe still bounds a mistake
              if(pageDrawn) ++pageRun; else pageRun=0;
              if(pageRun>=5||PageKeys::objectivesOpen) pageWideHold=20; else if(pageWideHold>0) --pageWideHold;
              const bool pageNow=pageWideHold>0&&!native.frontend();
              if(pageNow!=pageWasOpen&&pageLogs<10) {++pageLogs;pageWasOpen=pageNow;
                  log("Objectives page ('0' / 'i'): %s (background strips %.0f x %.0f of %ux%u)",pageNow?"OPEN":"closed",pgX1-pgX0,pgY1-pgY0,uiW,uiH);}
              pgX0=pgY0=1e9f;pgX1=pgY1=-1e9f;pgTopLeft=false;pgPieces=0;
              if(panelCool>0) --panelCool;   // after a failsafe cut, a box cannot re-open the panel for a while
              if(TextBoxWatch::installed) {   // 0.8.94.21: the box IS the game's text box rectangle of this frame (nothing else opens a notification panel)
                  TextBoxWatch::R u{};if(TextBoxWatch::any(float(uiW),float(uiH),u)) {boxX0=float(u.l);boxY0=float(u.t);boxX1=float(u.r);boxY1=float(u.b);++statGameBoxFrames;panelCool=0;}
                  else {boxX0=boxY0=1e9f;boxX1=boxY1=-1e9f;}}
              const bool boxNow=boxX1>boxX0&&boxY1>boxY0&&panelCool<=0;
              objHold=pageNow?25:0;
              msgFrame=boxNow;
              pageShown=pageNow;
              if(msgFrame) msgHold=20; else if(msgHold>0) --msgHold;
              // Diagnostic only: the old text union, logged so a future report still shows what the pixels said. It opens nothing.
              if(allX1-allX0>=.45f*uiW&&allY1-allY0>=.12f*uiH&&objLogs<12&&PauseWatch::paused!=1) {++objLogs;std::string t;char b[64];
                  for(auto& q:allPieces) {snprintf(b,64," [%.0f,%.0f-%.0f,%.0f]",q[0],q[1],q[2],q[3]);t+=b;}
                  log("Objectives-text union (0.8.94.7 diagnostic only, opens nothing): %.0f,%.0f-%.0f,%.0f of %ux%u from pieces%s",allX0,allY0,allX1,allY1,uiW,uiH,t.c_str());}
              allPieces.clear();
              if(pageSeen) ++statPageFrames;pageHold=pageNow?25:0;pageSeen=false;
              if(objHold>0) ++statObjFrames;if(msgHold>0) ++statMsgFrames;
              // Failsafe: without the game's menu and without the page, the panel may never stay up more than MiddlePanelMaxSeconds of game frames.
              // 0.8.94.19: the failsafe cut REAL tutorial boxes (they stay until the player clicks: 4 cuts in report 20261005_100309, capture 1: box gone,
              // X / check left alone). It was made for the false 256 px boxes (25 % wide), filtered at the source since 0.8.94.8. A real notification box
              // is ~48 % wide: only boxes narrower than 35 % are still bounded.
              const float claimedW=std::max(boxX1-boxX0,prevBox[2]-prevBox[0]);
              if(!gameMenu&&!pageNow&&claimedW<.35f*uiW&&!TextBoxWatch::installed) {if(msgHold>0&&++panelFrames>panelMaxFrames) {msgHold=0;panelFrames=0;panelCool=panelMaxFrames;
                      if(panelCuts<8) {++panelCuts;log("Middle panel: closed by the failsafe (no game menu, no objectives page, box still claimed after %d frames)",panelMaxFrames);}}}
              else panelFrames=0;
              {   // panel rectangle: whole band for the menu and the objectives page, the box itself for a notification
                  if(gameMenu||pageNow) msgRegion={0.f,.09f,1.f,.7135f};
                  else if(boxNow) {const float iw2=std::min(float(uiW),uiH*4.f/3.f),ih2=std::min(float(uiH),uiW*.75f),ox=(uiW-iw2)*.5f,oy=(uiH-ih2)*.5f;
                      msgRegion={std::clamp((boxX0-ox)/iw2-.01f,0.f,1.f),std::clamp((boxY0-oy)/ih2-.01f,.09f,.715f),
                                 std::clamp((boxX1-ox)/iw2+.01f,0.f,1.f),std::clamp((std::max(boxY1,boxTailY1)-oy)/ih2+.01f,.09f,(boxTailY1>boxY1||TextBoxWatch::installed)?.785f:.715f)};
                      if(boxTailY1>boxY1&&boxTailLogs<6) {++boxTailLogs;log("Notification box lower than usual (new game, no statistics bar): box bottom %.0f -> %.0f px, X / check kept in the box",boxY1,boxTailY1);}}
              }
              tipCluster();lastTip[0]=tipX0;lastTip[1]=tipY0;lastTip[2]=tipX1;lastTip[3]=tipY1;tipX0=tipY0=1e9f;tipX1=tipY1=-1e9f;tipCands.clear();
              msgFrame=false;allX0=allY0=1e9f;allX1=allY1=-1e9f;msgShown=msgHold>0;msgX0=msgY0=1e9f;msgX1=msgY1=-1e9f;heldPrev=heldModelSeen;heldModelSeen=false;lastKeep[0]=creatureKeep[0];lastKeep[1]=creatureKeep[1];lastKeep[2]=creatureKeep[2];lastKeep[3]=creatureKeep[3];creatureKeep[0]=creatureKeep[1]=1e9f;creatureKeep[2]=creatureKeep[3]=-1e9f;handBox[0]=handBox[1]=1e9f;handBox[2]=handBox[3]=-1e9f;heldKeep[0]=heldKeep[1]=1e9f;heldKeep[2]=heldKeep[3]=-1e9f;TextBoxWatch::endFrame();prevBox[0]=boxX0;prevBox[1]=boxY0;prevBox[2]=boxX1;prevBox[3]=boxY1;prevTail=(boxX1>boxX0&&boxTailY1>boxY1)?boxTailY1:-1e9f;boxX0=boxY0=1e9f;boxX1=boxY1=-1e9f;boxTailY1=-1e9f;
              screenWhole=screenHold>0;   // the game's own menu (interface + screen layers, game paused)
              // 0.8.94.20: a notification box or the objectives page takes the SAME whole band as the game menu (same place, between the health bar and
              // the bottom panel, statistics bar included), drawn from the screen layer only. Every flat piece the game draws in that band goes to the screen
              // layer while it is open (except the things at the cursor while the laser is on the table): nothing is cut out of the box any more.
              panelWhole=false;   // 0.8.94.21: the whole band for boxes / the page (0.8.94.20) is withdrawn: it hid the OPTIONS wall and stayed open at load
              // 0.8.94.22: the panel = the visible interface between the bars (scan), unless the game's menu is open (unchanged path)
              panelUi=panelFromPixels&&scan.ready&&!scan.failed&&!screenWhole;
              if(panelUi) {
                  objHold=0;pageShown=false;
                  if(scan.open) {const float iw2=std::min(float(uiW),uiH*4.f/3.f),ih2=std::min(float(uiH),uiW*.75f),ox=(uiW-iw2)*.5f,oy=(uiH-ih2)*.5f;
                      msgHold=20;msgRegion={std::clamp((scan.rect[0]-ox)/iw2-.005f,0.f,1.f),std::clamp((scan.rect[1]-oy)/ih2-.005f,.09f,1.f),
                                            std::clamp((scan.rect[2]-ox)/iw2+.005f,0.f,1.f),scanOnStats()?.762f:std::clamp((scan.rect[3]-oy)/ih2+.005f,.09f,.78f)};}   // 0.8.94.26: a panel over the timer bar shows it whole, as the game does (its separate slot is hidden)
                  else msgHold=0;
                  msgShown=msgHold>0;
              }
              }
             screenActive=screenHold>0||objHold>0||msgHold>0;if(screenActive) ++statScreenFrames;
             midX0=midY0=1e9f;midX1=midY1=-1e9f;}
            p->screenShown=screenActive;p->screenWhole=screenWhole;p->panelWhole=panelWhole;p->panelUi=panelUi;p->statsShown=statsShownNow();p->msgRegion=msgRegion;p->tipValid=false;p->keepValid=false;
            if(screenActive&&screenCleared&&screenSurf) {p->screen=copies.desktop(d,screenSurf.Get(),uiW,uiH);} else p->screen=0;p->pedestalSide=pedestalSide;p->buttonHover=buttonHover;
            if(heldCleared&&heldSurf) {
                p->held=copies.desktop(d,heldSurf.Get(),uiW,uiH);
                if(auto* c=copies.find(p->held)) {p->heldHandle=c->handle;p->heldPx=heldPx;p->heldPy=heldPy;p->tipValid=lastTip[2]>lastTip[0]&&lastTip[3]>lastTip[1];if(p->tipValid) {p->tipRect[0]=lastTip[0]-heldPx;p->tipRect[1]=lastTip[1]-heldPy;p->tipRect[2]=lastTip[2]-heldPx;p->tipRect[3]=lastTip[3]-heldPy;}p->keepValid=lastKeep[2]>lastKeep[0]&&lastKeep[3]>lastKeep[1];if(p->keepValid) {p->keepRect[0]=lastKeep[0]-heldPx;p->keepRect[1]=lastKeep[1]-heldPy;p->keepRect[2]=lastKeep[2]-heldPx;p->keepRect[3]=lastKeep[3]-heldPy;}p->heldRadius=heldRadius*uiW/1024.f;++statHeldFrames;} else p->held=0;
            }
            p->hud=copies.desktop(d,uiSurf.Get(),uiW,uiH);
            if(auto* c=copies.find(p->hud)) p->hudHandle=c->handle; else p->hud=0;
            // 0.6.9.15: the game's final image, game area of the back buffer (letterboxed like the pointer), at the
            // interface size: the VR bars are cut from it with the same rectangles.
            if(hudFromImage) {
                ComPtr<IDirect3DSurface9> bb;
                if(SUCCEEDED(d->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&bb))) {
                    D3DSURFACE_DESC bd{};bb->GetDesc(&bd);
                    const float k=std::min(float(bd.Width)/uiW,float(bd.Height)/uiH);
                    const LONG gw=LONG(uiW*k+.5f),gh=LONG(uiH*k+.5f),gx=(LONG(bd.Width)-gw)/2,gy=(LONG(bd.Height)-gh)/2;
                    RECT src{gx,gy,gx+gw,gy+gh};
                    p->frame=copies.gameImage(d,bb.Get(),src,uiW,uiH);
                }
            }
            p->hudWidth=uiW;p->hudHeight=uiH;p->hudSizes=hudSizes;
            p->hudTop=(haveLevels&&activeUnits>0)?std::clamp((highOffset-(haveWalk?walkOffset:0.f))/activeUnits,0.f,.8f):.05f;
            lastHudW=uiW;lastHudH=uiH;lastHudTop=p->hudTop;++statHudFrames;
            if(cursorFound) {
                ++statCursorFrames;
                if(auto* c=copies.find(cursorCopy)) {
                    bool listed=false;for(auto& ref:p->copies) if(ref.id==cursorCopy) listed=true;
                    if(!listed) p->copies.push_back({cursorCopy,c->handle,c->w,c->h,c->levels});
                    p->cursor=cursorCopy;memcpy(p->cursorUv,cursorUv,sizeof(cursorUv));
                    p->cursorW=cursorW;p->cursorH=cursorH;p->cursorHotX=cursorHotX;p->cursorHotY=cursorHotY;
                }
            }
        }
        if(handDraws) {
            p->handCount=handDraws;p->handCenter=scale(add(handMin,handMax),.5f);
            Vec3 e=sub(handMax,handMin);p->handExtent=std::max({e.x,e.y,e.z});++statHandFrames;statHandDraws+=handDraws;
        }
        if(!handProbe.empty()&&GetTickCount64()-lastHandProbeLog>5000) {
            lastHandProbeLog=GetTickCount64();
            std::string line;char b[160];
            for(auto& h:handProbe) {snprintf(b,sizeof(b)," [#%u/%u n=%u rhw %.3f-%.3f above %.3f m %.0fx%.0f%s]",h.index,frameWorldIndex,h.count,h.rmin,h.rmax,h.above,h.w,h.h,h.tagged?" HAND":"");line+=b;}
            log("Draws under the cursor (%.0f,%.0f), typical rhw %.3f, ceiling %s:%s",cursorPx,cursorPy,rhwTypical,handCeiling<1e8f?"known":"unknown",line.c_str());
        }
        handProbe.clear();frameWorldIndex=0;
        handDraws=0;handVerts=0;handFrameState=0;
        presParts[1]+=milliseconds()-partMark;partMark=milliseconds(); // game image copy, interface / hand window (GPU copies)
        uploadKeeper(d,*p);
        finishPick();pickActive=false;screenCleared=false;
        uiCleared=false;cursorFound=false;cursorBacked=false;heldCleared=false;holdingThings=false;heldModel=false;
        clearBatch();
        presParts[2]+=milliseconds()-partMark;partMark=milliseconds(); // keeper cursor texture, pick, clear
        pollPackage(true); // previous game frame: normally already finished on the GPU
        presParts[3]+=milliseconds()-partMark;partMark=milliseconds(); // wait for the previous package's GPU fence
        auto& q=packageFences[p->id&1];
        HRESULT hr=q?S_OK:d->CreateQuery(D3DQUERYTYPE_EVENT,&q);
        if(SUCCEEDED(hr)) hr=q->Issue(D3DISSUE_END);
        if(SUCCEEDED(hr)) {q->GetData(nullptr,0,D3DGETDATA_FLUSH);pendingPackage=std::move(p);++statPackages;}
        else {log("Package fence failed %08lX",(unsigned long)hr);++dropped;}
        presParts[4]+=milliseconds()-partMark; // fence issue + command flush
        statsSync+=milliseconds()-syncStarted;
        // Watchdog: the VR thread must publish images; otherwise the 0.2.9 path takes over again.
        double since=std::max(vr->lastPublishMs.load(),decoupledSince);
        if(vr->active&&handed>0&&milliseconds()-since>2000) {
            // 0.4.2: retried after 5 s (0.4.1 gave up for the whole session after one long pause).
            ++watchdogTrips;vr->active=false;
            if(watchdogTrips>=4) vr->failed=true; else retryAt=milliseconds()+5000;
            log("Decoupled VR rendering: no headset image from the VR thread for %.0f ms; 0.2.9 path %s",milliseconds()-since,vr->failed?"from now on":"for 5 s, then retry");
        }
        if(GetTickCount64()-lastStatus>5000) statusLog();
    }
    void statusLog() {
        possession2DLog();   // 0.8.97
        auto cadenceLog=[](const char* name,Cadence& c){
            if(c.seen) log("Source cadence %s: %.1f game frames/s | intervals p50=%.2f p95=%.2f p99=%.2f worst=%.2f ms | >22ms=%u >50ms=%u / %u (NOT headset refresh)",
                name,c.total>0?1000.*c.seen/c.total:0.,c.percentile(.50f),c.percentile(.95f),c.percentile(.99f),c.worst,c.over22,c.over50,c.seen);
            c.clearWindow();
        };
        cadenceLog("table",tableCadence);cadenceLog("possession",possessionCadence);
        log("Draw submission savings: consecutive draws merged=%llu unchanged replay states avoided=%llu (same vertices/material/order)",
            (unsigned long long)mergedDraws,(unsigned long long)avoidedReplayStates);
        mergedDraws=avoidedReplayStates=0;
        ProcessAffinity::statusLine();   // 0.9.47.12
        FastBlt::statusLine();   // 0.9.47.13
        if(Snapshot::policy.fast) {static unsigned long long lastDone=0,lastSkip=0;const auto dn=Snapshot::policy.done-lastDone,sk=Snapshot::policy.skipped-lastSkip;lastDone=Snapshot::policy.done;lastSkip=Snapshot::policy.skipped;
            log("Snapshot reads (0.9.47.16, last 5 s): %llu device state reads done, %llu avoided (%.0f %% of the %llu that were needed before), %u snapshots taken in total",(unsigned long long)dn,(unsigned long long)sk,dn+sk?100.*double(sk)/double(dn+sk):0.,(unsigned long long)(dn+sk),Snapshot::policy.captures);}
        {auto& g=TimedCalls::gate;const uint64_t delta=g.dropped-skipDrops0;skipDrops0=g.dropped;
         log("Native world skip (0.9.47.10, NativeWorldSkip=%d): %s | hidden dungeon draws not sent to the game's device: %llu in the last 5 s, %llu in total | anomalies: armed but never sent %llu, arguments differing %llu",
             TimedCalls::skipWanted?1:0,TimedCalls::skipWanted?(g.active()?(g.lean()?"ON (verified, lean)":"ON (verifying: states still written)"):"SWITCHED OFF by anomalies (previous behaviour restored)"):"off",(unsigned long long)delta,(unsigned long long)g.dropped,(unsigned long long)g.stale,(unsigned long long)g.mismatched);}
        log("Possession coverage guard: score=%.3f partial frames held=%u (60 ms maximum; no GPU texture copies)",possessionCoverage,possessionCoverageGuard.rejected);
        if(tiltN) {log("Possession tilt (0.8.80, last 5 s): game camera tilt against the entry reference over %llu frames | roll rms %.2f deg (mean %+.2f, max %.2f) | pitch rms %.2f deg (max %.2f) | live level %s, applied on %llu frames",(unsigned long long)tiltN,std::sqrt(tiltRollSq/double(tiltN)),tiltRollMean/double(tiltN),tiltRollMax,std::sqrt(tiltPitchSq/double(tiltN)),tiltPitchMax,possessionLiveLevel?"ON":"off",(unsigned long long)tiltLiveUsed);tiltN=tiltLiveUsed=0;tiltRollSq=tiltPitchSq=tiltRollMax=tiltPitchMax=tiltRollMean=0;}
        if(possession.followTicks) {log("Possession heading follow (0.8.80, last 5 s): game ticks %llu, correcting on %llu | heading error view - game avg %.1f deg, max %.1f deg | extra mouse pixels %llu | %s",(unsigned long long)possession.followTicks,(unsigned long long)possession.followActive,possession.followAbs/double(possession.followTicks),possession.followMax,(unsigned long long)possession.followPixels,possession.follow.enabled?"ON (PossessionHeadingFollow)":"off");possession.followTicks=possession.followActive=possession.followPixels=0;possession.followAbs=possession.followMax=0;}
        if(possession360&&(SurroundHack::sFrames||SurroundHack::sSkipped||statViewFrames)) {
            const double fr=double(std::max<uint64_t>(1,SurroundHack::sFrames));
            log("Possession 360 view (0.8.84 cube map, game thread, last 5 s): game frames %llu, with the 6 faces %llu, without (reference only) %llu | queue not empty first %llu, face cameras accepted %ld refused %ld (face runs not applied %llu, item list not empty at start %llu) | draws front %llu, faces F/R/B/L/U/D %llu/%llu/%llu/%llu/%llu/%llu (queued items %llu/%llu/%llu/%llu/%llu/%llu) | game render of the 6 faces %.2f ms/frame (draw flush %.2f ms), normal view %.2f ms | render memory blocks: peak during the faces %u, now %u (limit 127) | %s",
                (unsigned long long)statViewFrames,(unsigned long long)statCubeFrames,(unsigned long long)statViewMissing,(unsigned long long)SurroundHack::sSkipped,long(InterlockedExchange(&LensHack::auxCalls,0)),long(InterlockedExchange(&LensHack::auxRefused,0)),(unsigned long long)SurroundHack::sRefused,(unsigned long long)SurroundHack::sNodesKept,
                (unsigned long long)statViewRecs[0],(unsigned long long)statViewRecs[1],(unsigned long long)statViewRecs[2],(unsigned long long)statViewRecs[3],(unsigned long long)statViewRecs[4],(unsigned long long)statViewRecs[5],(unsigned long long)statViewRecs[6],
                (unsigned long long)SurroundHack::sQueued[1],(unsigned long long)SurroundHack::sQueued[2],(unsigned long long)SurroundHack::sQueued[3],(unsigned long long)SurroundHack::sQueued[4],(unsigned long long)SurroundHack::sQueued[5],(unsigned long long)SurroundHack::sQueued[6],
                SurroundHack::msFaces/fr,SurroundHack::msFlush/fr,SurroundHack::msMain/fr,unsigned(SurroundHack::memPeak),unsigned(native.read<uint32_t>(0x765B08)),SurroundHack::disabled?SurroundHack::disabledReason:"active");
            if(statAdaptFrames) {const double af=double(statAdaptFrames);
                log("Possession 360 adaptive (0.8.99.1, last 5 s): %s | faces run per game frame %.2f of 6 (F/R/B/L/U/D on %.0f/%.0f/%.0f/%.0f/%.0f/%.0f %% of the frames), all 6 (no head pose / cube yet) %llu of %llu frames, margin avg %.1f deg (base %.0f, hold %.0f ms)",
                    cubeAdaptive?"ON":"off (Possession360Adaptive=0)",double(SurroundHack::sFacesRun)/af,100.*SurroundHack::sFaceRun[0]/af,100.*SurroundHack::sFaceRun[1]/af,100.*SurroundHack::sFaceRun[2]/af,100.*SurroundHack::sFaceRun[3]/af,100.*SurroundHack::sFaceRun[4]/af,100.*SurroundHack::sFaceRun[5]/af,
                    (unsigned long long)statAdaptAll,(unsigned long long)statAdaptFrames,(statAdaptFrames>statAdaptAll)?statAdaptMargin/double(statAdaptFrames-statAdaptAll):0.,double(cubeMarginDeg),double(cubeHoldMs));}
            statAdaptFrames=statAdaptAll=0;statAdaptMargin=0;SurroundHack::sFacesRun=0;for(auto& n:SurroundHack::sFaceRun) n=0;
            if(LensHack::cube.valid) log("Possession 360 view: cube of the last frame: world down %+.0f (Z), face focal %.1f, square rectangle %.1f..%.1f x %.1f..%.1f",LensHack::cube.down[2],LensHack::cubeFocal,LensHack::cubeRect[0],LensHack::cubeRect[1],LensHack::cubeRect[2],LensHack::cubeRect[3]);
            SurroundHack::sFrames=SurroundHack::sSkipped=SurroundHack::sRefused=SurroundHack::sNodesKept=0;SurroundHack::memPeak=0;SurroundHack::msFaces=SurroundHack::msFlush=SurroundHack::msMain=0;
            for(int i=0;i<7;++i) {SurroundHack::sQueued[i]=0;statViewRecs[i]=0;}
            statViewMissing=statViewFrames=statCubeFrames=0;
        }
        if(possessionDecoupledOn&&(statPossPackages||statPossDropped||statPossEntered)) {log("Possession drawn by the VR thread (0.8.80, last 5 s): game frames packaged %llu, dropped (empty / partial) %llu, entered %llu time(s)",(unsigned long long)statPossPackages,(unsigned long long)statPossDropped,(unsigned long long)statPossEntered);statPossPackages=statPossDropped=statPossEntered=0;}
        possessionCoverageGuard.rejected=0;
        if(possession.predFrames) {
            log("Possession turn (0.8.67, last 5 s): game frames %llu | turn not yet in the image: avg %.2f deg, max %.2f deg | game response: %s, delay %d frame(s), gain %.2f, fit quality %.0f%% | fits %llu (rejected %llu) | mouse carry %s",
                (unsigned long long)possession.predFrames,possession.predSum/possession.predFrames,possession.predMax,possession.fitted?"measured":"not measured yet",possession.lag,possession.gain,100.*possession.quality,
                (unsigned long long)possession.fits,(unsigned long long)possession.fitFailures,possession.carry?"on":"off");
            possession.predSum=possession.predMax=0;possession.predFrames=0;
        }
        if(EngineOrigin::possessionDepthProtected) {
            log("Possession native depth protected: %llu vertices without UI RHW marker (native world draw classification preserved)",(unsigned long long)EngineOrigin::possessionDepthProtected);
            EngineOrigin::possessionDepthProtected=0;
        }
        log("Held 3D: native models %llu (%llu with no Z scale), projected vertices %llu (%llu of them with z outside the game clamp), captured draws %llu / triangles %llu (%llu cut by clipping, %llu vertices given a neighbour depth), upright frames %llu, fallback frames %llu, refused draws: no depth %llu, off the hand %llu, depth direction undecided %llu, texture %llu, no triangle %llu, depth direction %d%s (enabled=%d)",(unsigned long long)HeldGeometry::calls,(unsigned long long)HeldGeometry::zeroScaleCalls,(unsigned long long)HeldGeometry::vertices,(unsigned long long)HeldGeometry::clampedVertices,(unsigned long long)held3dDraws,(unsigned long long)held3dTriangles,(unsigned long long)heldFilledTriangles,(unsigned long long)heldFilledVertices,(unsigned long long)heldUprightFrames,(unsigned long long)held3dMissingFrames,(unsigned long long)heldRej[0],(unsigned long long)heldRej[1],(unsigned long long)heldRej[2],(unsigned long long)heldRej[3],(unsigned long long)heldRej[4],heldDecider.sign,heldDepthFlip?" (inverted by launcher)":"",int(HeldGeometry::enabled));
        log("Held 3D gates (0.9.47.53): held image present, draws turned away: cursor unknown %llu, laser off the table %llu, paused %llu, not triangles %llu, tiny %llu | examined %llu, with depth range %llu (flat %llu), near the hand %llu | pause state %d, hook depth entries %u",(unsigned long long)heldGate[0],(unsigned long long)heldGate[1],(unsigned long long)heldGate[2],(unsigned long long)heldGate[3],(unsigned long long)heldGate[4],(unsigned long long)heldGate[5],(unsigned long long)heldGate[6],(unsigned long long)heldFlatDraws,(unsigned long long)heldNearHand,int(PauseWatch::paused),unsigned(HeldGeometry::depths.size()));
        if(statHudFrames||statUiDraws) {
            log("HUD: frames=%llu interface draws=%llu cursor found in %llu frames (last %.0fx%.0f px, hotspot %.2f,%.2f at %.0f,%.0f) laser hits=%llu clicks=%llu last point=%.3f,%.3f top=%.2f m",
                (unsigned long long)statHudFrames,(unsigned long long)statUiDraws,(unsigned long long)statCursorFrames,cursorW,cursorH,cursorHotX,cursorHotY,cursorPx,cursorPy,
                (unsigned long long)statHudHits,(unsigned long long)statHudClicks,lastPointerX,lastPointerY,lastHudTop);
            log("Keeper hand (software cursor): %llu images set, %llu captured, %llu failures, last %dx%d hotspot %d,%d, %u bpp, key %08X, %u opaque pixels",
                (unsigned long long)keeper.calls,(unsigned long long)keeper.captures,(unsigned long long)keeper.failures,keeper.w,keeper.h,keeper.hotX,keeper.hotY,keeper.lastBits,keeper.lastKey,keeper.lastOpaque);
            keeper.calls=keeper.captures=0;
            log("Wall screen (options/menus) shown in %llu frames (game paused flag %ld, changes %ld)",(unsigned long long)statScreenFrames,long(PauseWatch::paused),long(PauseWatch::changes));
            if(CinematicView::installed) {const LONG u=InterlockedExchange(&CinematicView::used,0),c=InterlockedExchange(&CinematicView::centred,0),kk=InterlockedExchange(&CinematicView::kept,0),sh=InterlockedExchange(&CinematicView::shifted,0),ff=InterlockedExchange(&CinematicView::flagsFixed,0);
                if(u) log("Game cinematics (0.8.94, last 5 s): %ld world renders with the player's camera; place followed in %ld (+%ld with the last place), view moved %ld times, roof flags set to the player's %ld times, last shift %.2f,%.2f tiles",u,c,kk,sh,ff,CinematicView::dx,CinematicView::dy);}
            statScreenFrames=0;   // 0.8.94.3: reset lost in 0.8.94 (the 'Wall screen' count was cumulative)
            log("Table picking: game camera focal x%.2f (wanted x%.2f), table between the bars %d, projection setups %ld",float(LensHack::applied),float(LensHack::want),int(pickFits),long(InterlockedExchange(&LensHack::calls,0)));
            log("Table pointer: %llu frames on the table (%llu on dungeon geometry, %llu on the flat table), %llu clicks, last point %.3f,%.3f",(unsigned long long)statTableHits,(unsigned long long)statPickHits,(unsigned long long)statPlaneHits,(unsigned long long)statTableClicks,lastTableX,lastTableY);statTableHits=statTableClicks=statPickHits=statPlaneHits=0;
            log("Interface in 3D over the bars (Ui3DFilter=%d): %llu draws moved to the HUD; minimap frame corners corrected %llu times; small buttons kept on the HUD %llu, held pieces %llu, plane rule off (dungeon near) %llu frames, panel pieces below the dungeon %llu, sprites facing the headset %llu of %llu, opaque draws never turned upright %llu (SpriteAlphaOnly), upright glow/smoke draws without depth write %llu",int(ui3dFilter),(unsigned long long)statUi3d,(unsigned long long)MinimapFrame::fixedCalls,(unsigned long long)statButtonRect,(unsigned long long)statHeldPieces,(unsigned long long)statPlaneOff,(unsigned long long)statUnderDungeon,(unsigned long long)statBillboards,(unsigned long long)statBillboardSeen,(unsigned long long)statSpriteOpaqueSkipped,(unsigned long long)statUprightNoDepth);statSpriteOpaqueSkipped=0;statUprightNoDepth=0;statPlaneOff=0;statUnderDungeon=0;statBillboards=statBillboardSeen=0;statHeldPieces=statButtonRect=0;statUi3d=0;MinimapFrame::fixedCalls=0;
            log("Forge fan rule (0.9.47.19, ForgeFanKeepPale=%d): %llu fan-like draws hidden (dark), %llu pale ones (water) kept",iniInt(L"ForgeFanKeepPale",1),(unsigned long long)statForgeFans,(unsigned long long)statForgeFanKept);
            log("Numbers born as ONE sprite (0.9.47.33): %llu laid out from their screen picture, no shared plane",(unsigned long long)(statNumberSprites-statNumberSpritesLast));statNumberSpritesLast=statNumberSprites;
            log("Damage digits per period (0.9.47.29, every load): glyph-like additive quads %llu (too big %llu, not flat %llu, bad shape %llu), handled by the digit rules %llu | reading frame: %llu groups laid out, %llu fell back to the screen copy",(unsigned long long)(dgGlyph-dgLast[0]),(unsigned long long)(dgBig-dgLast[1]),(unsigned long long)(dgNotFlat-dgLast[2]),(unsigned long long)(dgShapeNo-dgLast[3]),(unsigned long long)(dgHandledQ-dgLast[4]),(unsigned long long)digitFrame.readingOk,(unsigned long long)digitFrame.readingFallback);
            dgLast[0]=dgGlyph;dgLast[1]=dgBig;dgLast[2]=dgNotFlat;dgLast[3]=dgShapeNo;dgLast[4]=dgHandledQ;digitFrame.readingOk=digitFrame.readingFallback=0;
            log("Floor decals (0.9.30, FloorDecalNoDepth=%d): %llu shadow/decal draws replayed without depth write, menu pillar light %llu vertices lifted | smoke plates (0.9.31, SmokeNoDepth=%d): %llu draws without depth write | render-target (haze) draws %llu, HazeSkip=%d | translucent without depth (0.9.36) %llu | forge smoke fans hidden (0.9.39) %llu | upside-down effects turned (0.9.41) %llu | decals put back on surfaces (0.9.42) %llu draws %llu vertices | alpha sprites (smoke puffs) without depth write (0.9.44, SpriteNoDepth=%d) %llu | held-creature window with the pointer off the table (0.9.45, HeldOffTable=%d) %llu frames | damage-number glyph pieces turned to the headset (0.9.45, DamageNumbersFacing=%d) %llu | damage digits stood up (0.9.47, DamageDigitsUpright=%d) %llu | badge discs no longer taken for terrain (0.9.47.5, BadgeDiscUpright=%d) %llu | damage-digit draws replayed last (0.9.47.6, DigitsDrawnLast=%d) %llu | screen overlays put on surfaces (0.9.37) %llu draws, %llu vertices, %llu without surface, depth map %.2f ms",int(floorDecalNoDepth),(unsigned long long)statDecalNoDepth,(unsigned long long)menuPillarLifted,int(softNoDepth),(unsigned long long)statSoftNoDepth,(unsigned long long)statHaze,int(hazeSkip),(unsigned long long)statTranslucentNoDepth,(unsigned long long)statForgeFans,(unsigned long long)statUpsideFixed,(unsigned long long)statDecalSnapDraws,(unsigned long long)statDecalSnapVerts,int(spriteNoDepth),(unsigned long long)statSpriteNoDepth,int(heldOffTable),(unsigned long long)statHeldOffTable,int(damageFacing),(unsigned long long)statGlyphSprites,int(damageDigits),(unsigned long long)statDigitSprites,int(badgeDiscFix),(unsigned long long)statBadgeDisc,int(digitsLast),(unsigned long long)statDigitsLast,(unsigned long long)statOverlayDraws,(unsigned long long)statOverlayVerts,(unsigned long long)statOverlayMissed,sceneMapMs);statHeldOffTable=statBadgeDisc=statDigitsLast=statDigitSprites=statGlyphSprites=statSpriteNoDepth=statOverlayDraws=statOverlayVerts=statOverlayMissed=statForgeFans=statUpsideFixed=statDecalSnapDraws=statDecalSnapVerts=0;sceneMapMs=0;statDecalNoDepth=menuPillarLifted=statSoftNoDepth=statHaze=statTranslucentNoDepth=0;
            log("3D draws by engine origin: interface=%llu mixed=%llu (interface triangles %llu, dungeon triangles %llu) dungeon=%llu (of which without any interface mark %llu)",
                (unsigned long long)statOriginUi,(unsigned long long)statOriginMixed,(unsigned long long)statOriginUiTris,(unsigned long long)statOriginWorldTris,(unsigned long long)statOriginWorld,(unsigned long long)statOriginUntagged);
            statOriginUi=statOriginMixed=statOriginUiTris=statOriginWorldTris=statOriginWorld=statOriginUntagged=0;
            EngineOrigin::report();
            log("Interface cards kept out of the dungeon: %llu draws (%zu interface textures)",(unsigned long long)statUiIn3d,uiTextures.size());statUiIn3d=0;uiTextures.clear();
            log("Middle panel (0.8.94.22, last 5 s): read-backs %llu, with a panel %llu, opened %llu times, open now %d, last %llu px / %.1f %% of the game area",(unsigned long long)scan.scans,(unsigned long long)scan.positives,(unsigned long long)scan.opens,int(scan.open),(unsigned long long)scan.lastCount,100.*scan.lastArea);scan.scans=scan.positives=0;
            log("Middle panel (0.8.94.21, last 5 s): game text box on %llu frames (%llu pieces kept in it, %llu text box calls in total); page shown %d; game menu %d",(unsigned long long)statGameBoxFrames,(unsigned long long)statGameBoxPieces,(unsigned long long)TextBoxWatch::calls,int(pageShown),int(screenWhole));statGameBoxFrames=statGameBoxPieces=0;
            log("Held things (window=%d): %llu/%llu frames, %llu pieces moved to the hand, rejected %llu far from cursor / %llu in a panel, game hand count>0 in %llu frames | page open %llu, objectives-text rule %llu, message state %llu frames, tooltip pieces kept for the hand %llu",int(heldWindow),(unsigned long long)statHeldFrames,(unsigned long long)statUiFrames,(unsigned long long)statHeldDraws,(unsigned long long)statHeldFar,(unsigned long long)statHeldPanel,(unsigned long long)statHoldCount,(unsigned long long)statPageFrames,(unsigned long long)statObjFrames,(unsigned long long)statMsgFrames,(unsigned long long)statTipPieces);statPageFrames=statObjFrames=statMsgFrames=statTipPieces=0;statHeldFrames=statHeldDraws=statHeldFar=statHeldPanel=statHoldCount=statUiFrames=0;
            log("Keeper hand (3D cursor): found in %llu frames, %.1f draws/frame, last nearest rhw %.4f vs typical %.4f, cursor %.0f,%.0f in %ux%u",
                (unsigned long long)statHandFrames,statHandFrames?double(statHandDraws)/statHandFrames:0.,lastHandRhw,lastHandTypical,cursorPx,cursorPy,gameW,gameH);
            statHandFrames=statHandDraws=0;
            statHudFrames=statUiDraws=statCursorFrames=statHudHits=statHudClicks=0;
        }
        {
            Projection p{};native.lens(p);
            log("frame=%llu world=%u ui=%u rejected=%u cameraMode=%u lens=%.2f center=%.2f,%.2f",(unsigned long long)frame,world,ui,rejected,mode,p.focalX,p.centerX,p.centerY);
            double elapsed=milliseconds()-statsStarted;
            double frames=double(std::max<uint64_t>(1,statsFrames));
            log("Texture cache: %d blocks in use; render memory %lu x 2 MB",TextureCache::actual(native),(unsigned long)PoolGrow::inUse(native.base));
            PaintProbe::report();
            LockProbe::report(5);
            TextureWatch::reportShadow();
            {   const double sampled=double(std::max<uint64_t>(1,sampledFrames));
                CallTiming::report(sampled,elapsed*sampled/frames,[](const char* f,auto... a){log(f,a...);});sampledFrames=0;
                LockDetail::report(frames,[](const char* f,auto... a){log(f,a...);});
                TextureWatch::reportStaging();WrapperUpload::report();ReadbackLag::report();TimedCalls::integrityReport(false);reportAddressSpace();}
            FastCrc::report();
            log("Timing: game=%.1f fps (%.1f ms/frame), VR record+overlay CPU=%.2f ms, stereo replay=%.2f ms, GPU wait=%.2f ms, draws=%.0f/frame, vertices=%.0f/frame, rectangular=%u, anchored=%u, batched=%u, units/m=%.2f, detailmode=%d, probe=%d",
                1000*statsFrames/std::max(1.,elapsed),elapsed/frames,statsDraw/frames,statsReplay/frames,statsSync/frames,double(statsWorld)/frames,double(statsVertices)/frames,table.valid,anchored,batched,activeUnits,LensHack::detailMode,probeMode.load());
            WaitProbe::scene=measureTaint?3:dominantScene();   // 0.9.47.7 / 0.9.47.11 (window next to an F12 press = bucket "other")
            WaitProbe::report(frames,elapsed,[](const char* f,auto... a){log(f,a...);});
            WaitProbe::reportCpu(frames,[](const char* f,auto... a){log(f,a...);},table.valid&&anchored&&statsVertices/std::max(1.,double(frames))>20000);
            log("Timing detail: mod Present total=%.2f ms/frame (includes replay, overlay, copy, GPU wait)",statsPresent/frames);
            log("Present hand-over breakdown (ms/frame, 0.8.64): package content %.2f | game image copy + interface/hand window %.2f | keeper cursor + pick %.2f | wait for previous package fence %.2f (blocked %llu of %llu frames, %.2f ms each) | fence issue + flush %.2f",
                presParts[0]/frames,presParts[1]/frames,presParts[2]/frames,presParts[3]/frames,(unsigned long long)pollBlockedCount,(unsigned long long)pollCalls,pollBlockedCount?pollBlockedMs/pollBlockedCount:0.,presParts[4]/frames);
            for(double& v:presParts) v=0;pollBlockedMs=0;pollBlockedCount=pollCalls=0;
            InputPump::report([](const char* f,auto... a){log(f,a...);});   // 0.9.47.8
            perfReport(frames);   // 0.9.25
            if(vr) {
                char line[400];copies.statsLine(line,sizeof(line));
                log("Controllers: pans=%llu zooms=%llu native turns=%llu (direction changed by the game %llu), rotation sign %d%s",
                    (unsigned long long)control.pans,(unsigned long long)control.zooms,(unsigned long long)control.turns,(unsigned long long)control.overridden,
                    control.sign,control.calibrated?" measured":" (default, not yet measured)");
                log("Decoupled: %s, game frames packaged=%llu handed=%llu dropped=%llu, VR thread images=%llu; texture %s; hooks=%s",
                    vr->failed?"FAILED (0.2.9 path)":vr->active?"active":"paused",(unsigned long long)statPackages,(unsigned long long)handed,(unsigned long long)dropped,
                    (unsigned long long)vr->published.load(),line,TextureWatch::hooksOk?"ok":"partial");
                statPackages=handed=dropped=0;
            }
            if(!native.frontend()) {diagnostics();gameCounters();}
            statsDraw=statsSync=statsReplay=statsPresent=0;statsFrames=statsVertices=statsWorld=0;statsStarted=milliseconds();
            lastStatus=GetTickCount64();
            // 0.8.54 diagnostic sequence: one probe mode per 3 windows while the table is really shown, then back to normal.
            if(probeAuto&&vr&&vr->active&&anchored) {
                static const int sequence[]={0,1,2,3,4,5,0};
                if(++probeWindows>=3) {
                    probeWindows=0;++probeStep;
                    if(probeStep>=int(sizeof(sequence)/sizeof(sequence[0]))) {probeAuto=false;probeMode.store(0);log("Probe sequence finished: normal mode restored. Close the game and collect the report.");}
                    else {probeMode.store(sequence[probeStep]);log("Probe sequence: step %d/%d -> mode %d (0 normal, 1 no antialiasing, 2 no room, 3 both off, 4 headset 45/s, 5 headset 30/s)",probeStep+1,int(sizeof(sequence)/sizeof(sequence[0])),sequence[probeStep]);}
                }
            }
        }
    }
};
Renderer& renderer() {static Renderer* r=new Renderer;return *r;}
void SurroundHack::install(uintptr_t base) {
    if(installed) return;installed=true;
    if(iniInt(L"Possession360",0)==0) {log("Possession 360 view: off by default since 0.8.94.7 (Possession360=0): the reference view only - the 6 extra world renders per game frame were the entry/exit lag and filled the game's render memory (exit -1)");return;}
    const uintptr_t delta=base-0x400000;
    if(!LensHack::gOk||!LensHack::original) {log("Possession 360 view: not installed (the view setup hook is not active)");return;}
    uint32_t* slot=reinterpret_cast<uint32_t*>(delta+0x6705a0);
    const uint32_t expected=uint32_t(delta+0x598880);
    if(!readable(slot,4)||*slot!=expected) {log("Possession 360 view: not installed - the renderer table does not hold the expected world render (found %08X, expected %08X): unknown game version",readable(slot,4)?unsigned(*slot):0u,unsigned(expected));return;}
    // the functions the faces use must be the ones that were disassembled / emulated
    struct Sig {uint32_t va;unsigned char b[5];};
    static const Sig sigs[]={{0x58f640,{0x83,0xec,0x2c,0x53,0x55}},{0x589c90,{0xa0,0xe0,0xc2,0x76,0x00}},{0x5898f0,{0xf6,0x05,0xe0,0xc2,0x76}},{0x598880,{0x6a,0xff,0x68,0x86,0x00}},
                             {0x575a00,{0xe9,0,0,0,0}},{0x574820,{0x6a,0xff,0x68,0xeb,0xf8}},{0x58a6f0,{0x83,0xec,0x60,0x8b,0x44}},{0x578e80,{0xa1,0x24,0x52,0x76,0x00}}};
    // (0.8.84.1: 0x58ADC0 is NOT checked: the mod's own EngineOrigin hook patches it before this point - that check refused the install in 0.8.84)
    for(const auto& g:sigs) {
        const unsigned char* q=reinterpret_cast<const unsigned char*>(delta+g.va);
        const size_t n=g.b[0]==0xe9?1:5;   // 0x575A00 carries our own jump (LensHack): only its first byte is checked
        if(!readable(q,5)||memcmp(q,g.b,n)) {log("Possession 360 view: not installed - function %06X is not the expected one (unknown game version)",unsigned(g.va));return;}
    }
    LensHack::cubeGuard=iniFloat(L"Possession360Guard",1.04f,1.f,1.3f);
    original=reinterpret_cast<WFn>(expected);slotPtr=slot;
    DWORD old=0;
    if(!VirtualProtect(slot,4,PAGE_READWRITE,&old)) {log("Possession 360 view: not installed (the renderer table cannot be written, error %lu)",GetLastError());return;}
    *slot=uint32_t(reinterpret_cast<uintptr_t>(&hook));
    DWORD ig=0;VirtualProtect(slot,4,old,&ig);
    log("Possession 360 view (0.8.84): installed - CUBE MAP of 6 faces of 90 degrees per game frame in first-person (front, right, back, left, up, down; world render 0x598880 run 6 more times, the normal view last and unchanged), guard band x%.3f",LensHack::cubeGuard);
}
int __fastcall SurroundHack::hook(void* self,void* edx,void* arg) {
    renderer().preWorld();   // 0.8.94.17: possession entry / exit -> the top-down tilt is set BEFORE this world render, not one frame late
    for(bool& b:frameOk) b=false;frameCube=false;frameWant=63;
    drawView=0;LensHack::cube.valid=false;LensHack::faceApplied=0;LensHack::cubeBuildNext=false;
    auto& R=renderer();R.faceDropped=false;R.facePoseOk=false;
    const uintptr_t D=LensHack::gDelta;
    if(disabled||!LensHack::gOk||!R.surroundWanted()||!LensHack::possessionNow()) return original(self,edx,arg);
    auto rd32=[&](uint32_t va)->uint32_t {uint32_t v=0;memcpy(&v,reinterpret_cast<void*>(D+va),4);return v;};
    auto wr32=[&](uint32_t va,uint32_t v) {memcpy(reinterpret_cast<void*>(D+va),&v,4);};
    if(rd32(0x7820c4)!=0) {++sSkipped;return original(self,edx,arg);}   // something is already queued: the faces could not be told apart from it
    unsigned char* me=static_cast<unsigned char*>(self);
    if(!readable(me+0xc9,4)) return original(self,edx,arg);
    ++sFrames;
    LensHack::saveGlobals();
    uint32_t counter=0;memcpy(&counter,me+0xc9,4);
    const uint32_t t0=rd32(0x795710),t1=rd32(0x79570c);
    const uint32_t stamp=rd32(0x760b10);   // frame stamp of the map block cache / per-object 'seen this frame' marks (compared for equality only): every run gets its own
    // the render item list (0x765224) is empty at the start of the world render (frame begin 0x578E50 sets it to 0): each face then ends exactly like a frame
    // (0x589C90, 0x58F640, 0x5898F0, 0x578E80) so that 7 world renders never hold their items at the same time
    const bool nodesClean=rd32(0x765224)==0;if(!nodesClean) ++sNodesKept;
    (void)0;
    int ok=0;
    // 0.8.94.6: the 6 extra world renders are what fills the game's render memory pool the fastest (report 20261004_160911: 1 -> 19 blocks within
    // one possession, then exit(-1)). While the pool is hot this frame shows the reference view only; nothing else changes.
    if(sCubeOff||!PoolGrow::safe()) {
        ++sPoolHeld;
        if(sPoolHeldLogs<8) {++sPoolHeldLogs;log("Possession 360 view: cube map held back, render memory %lu blocks of 2 MB (trip level %u, reference %u): the reference view only",(unsigned long)rd32(0x765b08),unsigned(PoolGrow::guard.tripLevel()),unsigned(PoolGrow::guard.reference));}
        LensHack::faceNext=0;drawView=0;
        return original(self,edx,arg);
    }
    // 0.8.99.1 ADAPTIVE 360: only the faces the headset can reach (with a margin) outside the wide normal view are run (Renderer::cubeWanted)
    const unsigned want=R.cubeWanted();frameWant=want;LensHack::cubeBuildNext=true;
    int wanted=0;for(int v=1;v<=6;++v) if(want&(1u<<(v-1))) ++wanted;
    for(int v=1;v<=6;++v) {
        if(!(want&(1u<<(v-1)))) {frameOk[v-1]=false;continue;}
        // 0.8.94.8: the pool is checked before EVERY face, not once per frame. One game frame can allocate far more than the whole headroom
        // (report 20261004_202042: Possession360=1 was still set in the user's INI and the game reached exit(-1) again), so the faces are given
        // up one by one as soon as the pool climbs, and after two trips the cube map stops for the rest of the session. Independent of the INI.
        {const uint32_t blocks=rd32(0x765b08);
         if(blocks>=PoolGrow::guard.tripLevel()||blocks>=PoolGrow::guard.offLevel()) {
             if(++sCubeTrips>=2||blocks>=PoolGrow::guard.offLevel()) {sCubeOff=1;log("Possession 360 view: STOPPED for this session at face %d, render memory %lu blocks of 2 MB - the 6 extra world renders are what fills the game's render memory (exit -1)",v,(unsigned long)blocks);}
             else log("Possession 360 view: faces %d..6 given up this frame, render memory %lu blocks of 2 MB",v,(unsigned long)blocks);
             break;
         }}
        const double a=milliseconds();
        LensHack::faceApplied=0;LensHack::faceNext=v;drawView=v;
        wr32(0x760b10,stamp+uint32_t(v));   // 0.8.84.2: strictly increasing stamps stamp+1..stamp+6 (then stamp+7 for the normal run): the per-tile 'processed' mark is compared with >= (0x5723E3), the 0.8.84.1 huge face stamps froze every tile seen by a face
        original(self,edx,arg);
        LensHack::faceNext=0;
        const bool applied=LensHack::faceApplied==v;
        if(!applied) ++sRefused;
        sQueued[v]+=rd32(0x7820c4);
        const double b=milliseconds();
        {   // draw what this face queued (the same three calls as the frame end 0x575780), every draw tagged with the face and hidden in the game's image
            using F4=void(__cdecl*)(int,int,int,int);using F0=void(__cdecl*)();
            reinterpret_cast<F4>(D+0x589c90)(0,0,int(rd32(0x760b54)),int(rd32(0x760b88)));
            reinterpret_cast<F0>(D+0x58f640)();
            reinterpret_cast<F0>(D+0x5898f0)();
            memPeak=std::max(memPeak,rd32(0x765b08));
            if(nodesClean) {reinterpret_cast<F0>(D+0x578e80)();wr32(0x765234,1);}   // frees the items of this face (sets the 'open' flag back like the frame begin)
        }
        const double c=milliseconds();
        msFaces+=c-a;msFlush+=c-b;
        drawView=0;
        // the game must not see this run: camera globals, frame counter and frame times are put back
        LensHack::restoreAux();
        memcpy(me+0xc9,&counter,4);wr32(0x795710,t0);wr32(0x79570c,t1);
        frameOk[v-1]=applied;if(applied) {++ok;++sFacesRun;++sFaceRun[v-1];}
    }
    const double m0=milliseconds();
    LensHack::faceNext=0;drawView=0;
    wr32(0x760b10,stamp+7u);   // the normal view runs last with the highest stamp; the next frame begin increments from here (always above every mark left by the faces)
    const int result=original(self,edx,arg);   // the normal view, last, unchanged
    msMain+=milliseconds()-m0;
    frameCube=wanted>0&&ok==wanted;   // otherwise this frame shows the reference view (e.g. a game camera looking straight up has no heading for the cube)
    return result;
}
}
extern "C" void __cdecl dk2vr_beginNote(uint32_t* sp) {engineOriginNote(sp);}
extern "C" __declspec(dllexport) void __cdecl DK2VR_Draw(IDirect3DDevice9* d,D3DPRIMITIVETYPE t,UINT n,const void* v,UINT stride,const void* i,D3DFORMAT f,UINT min,UINT num) {
    const long long rp0=Clock::now();++RecProf::draws;
    try {renderer().draw(d,t,n,v,stride,i,f,min,num);} catch(...) {log("Draw exception; frame skipped");}
    RecProf::t[0]+=Clock::now()-rp0;
}
extern "C" __declspec(dllexport) void __cdecl DK2VR_Present(IDirect3DDevice9* d) {
    try {renderer().present(d);} catch(...) {log("Present exception; frame skipped");}
}
extern "C" __declspec(dllexport) void __cdecl DK2VR_Reset() {renderer().reset();}
extern "C" __declspec(dllexport) UINT __cdecl DK2VR_Adapter(IDirect3D9* d,UINT fallback) {
    auto& r=renderer();if(!r.connect()||!r.ipc.lock(100)) return fallback;
    Request req=r.ipc.data->request;r.ipc.unlock();
    ComPtr<IDirect3D9Ex> ex;if(FAILED(d->QueryInterface(IID_PPV_ARGS(&ex)))) return fallback;
    for(UINT a=0;a<ex->GetAdapterCount();++a) {LUID id{};if(SUCCEEDED(ex->GetAdapterLUID(a,&id))&&id.LowPart==req.adapterLow&&id.HighPart==req.adapterHigh) return a;}
    log("OpenXR GPU not found among D3D9 adapters; sharing may fail");return fallback;
}
