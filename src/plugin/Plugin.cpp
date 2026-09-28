// DK2VR 0.7.2 - hidden dungeon draws write no depth (hid the '0' button); bar test limited to small pieces (large dungeon draws went to the HUD). 0.7.1 - transparent bars again; rhw rules restored and native depth clear removed ('i' back); probe of the 'o'/'i' button area. 0.7.0.1 - hand window erases the bars entirely again (0.7.0: the pickaxe model box kept whole bars on the table). 0.7.0 - rebuilt from the 0.6.9.2 reference with only the validated/expected changes: sharp textures (LOD), render memory pool, crash log, pause-driven options panel, messages above the bars, fold button blocked, bars from the game image over stone (dungeon hidden in the game image, depth cleared before the interface), hand window keeps the held model, auto recenter, JPG captures, minimap/pedestal; draw-classification experiments removed. 0.6.9.17 - fix 0.6.9.16 regression (empty table: colour writes recorded off); no stone behind the message panel. 0.6.9.16 - dungeon hidden in the game image (colour writes off) so the bars come clean from it; hand window from the held model on; stone backing behind the panels. 0.6.9.15 - VR bars and minimap cut from the game final image (no draw classification); engine view kept between the bars. 0.6.9.14 - interface replayed with its own depth buffer (dungeon depth hid interface pieces); fold button rect measured; larger hand window. 0.6.9.13 - bar elements straddling two bars kept in the HUD; box text stays in the box (only pieces after the held model go to the hand); taller hand window. 0.6.9.12 - message box test limited to real boxes (0.6.9.11 regression: blank bars). 0.6.9.11 - hovered bar elements animated in 3D kept in the HUD; message panel = box only, box never in the interface texture (held creature back); panel fold button disabled. 0.6.9.10 - depth test restored for interface draws (0.6.9.9 regression: empty bars); message boxes detected under the cursor and erased from the hand; bottom-panel 3D pieces to the HUD. 0.6.9.9 - messages above all bars and steady when pointed, kept off the hand; interface draws replayed without depth test. 0.6.9.8 - render memory pool 32 -> 127 blocks (exit(-1) crash); game messages/objectives above the bars; auto recenter 5 s after launch. 0.6.9.7 - options panel follows the game pause (0x527270); game exit logged; 3D interface cards out of the table; minimap crop; auto recenter. 0.6.9.6 - texture detail fix with exact addresses; options screen closes again; UI-over-bars test tolerant; pedestal column under the plate. 0.6.9.5 - texture detail follows the focal factor (21 LOD compares redirected); F12 removed; pedestal plate restored. 0.6.9.4 - crash log (exception handler); interface over the bars kept out of the table while the table fits between them; F12 toggles whole-table picking. 0.6.9.3.1 - host linked to openxr_loader.dll (was libopenxr_loader.dll: start failure). 0.6.9.3 - options screen whole (stats bar included) and steady under the laser; pedestal plate removed; adaptive focal factor (sharper textures); 3D interface pieces over the bottom bars kept out of the table; JPG captures; system OpenXR runtime. 0.6.9.2 - whole-table picking fixed (view setup 0x575A00 hooked: tile selection uses the same focal); options screen stacked on the timer bar under the health bar; VDXR runtime preferred. 0.6.9.1 - picking off, options screen stacked. 0.6.9 - whole table pickable (game camera focal widened while the table is shown); wall screen from middle draws only; original room textures; glow hands; smaller pedestal button. 0.6.8 - wall screen detection ignores held model/cursor area; smaller held creature; rusted pedestal with side button, drawn before the tray; earthen room. 0.6.7.1 - wall screen only when a menu/options screen is open; held window only with the laser on the table, scaled like the hand. 0.6.7 - laser picks the dungeon triangles; hand size fixed; held window wider; wall = whole game area, bars stacked over the panel. 0.6.6.1 - interface-texture filter off (game texture cache blocks are shared by UI and world). 0.6.6 - laser on the table drives the game cursor (inverse projection), trigger/A = clicks. 0.6.5.3 - held things = HUD window around the cursor (panels erased); 3D interface cards out of the dungeon. 0.6.5.2 - held things only near the cursor and outside panels the cursor is not on. 0.6.5.1 - top-down: engine rectangle = projected table (exact); game screen panel off (F3); texture upload callers measured. 0.5.15 - far distance x3 with the top-down camera (cross-shaped table when zoomed out). 0.5.14 - no near-edge shift with the top-down camera, wider side margin. 0.5.13 - steep (top-down) game camera while the table is shown. 0.5.12 - table near edge follows the nearest geometry the game draws. 0.5.11 - texture cache patch address fixed, table near edge kept in the engine's reach. 0.5.10 - real texture cache (SurfHashList2 512 -> 1536), pan prediction limited. 0.5.9 - larger player-side / side margins (holes while dragging). 0.5.8 - table depth shared between far and player sides (player-side strip), far from the screen far edge. 0.5.7 - vertical enlargement on both screen edges again (player side filled in 0.5.2), far limited to the corner ratio. 0.5.6 - texture cache byte+hook+real count, far distance for the far corners. 0.5.5 - texture cache 256 blocks (constructor call), far edge fixed / sides+player side only. 0.5.4.1 - fix: lockBuf/unlockBuf wrappers used wrong stack sizes (crash at start). 0.5.4 - near corners (+30 % player side), surface-lock measurement. 0.5.3 - game texture shadow (no GPU wait on texture locks), no fresh-image wait, wider corner margin. 0.5.2 - table view exactly as validated in 0.4.7 (TileCover removed) + VR state cache, full resolution, ceiling clip. Was 0.5.0 (base 0.4.7) - VR state cache, full headset resolution, engine tiles = table (corners), ceiling clip. 0.4.7 - table shows the whole PC screen view (TableView), anisotropic x16 VR textures, table 0.50 m below the eyes.
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
#include "../common/Ipc.hpp"
#include "../common/Math.hpp"
#include "../common/Tabletop.hpp"
#include "../common/Decouple.hpp"
#include "../common/TableModel.hpp"
#include "../common/HudLayout.hpp"
#include <d3d9.h>
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
using Microsoft::WRL::ComPtr;
using namespace dk2vr;

namespace {
constexpr const char* Version="0.7.2";
std::filesystem::path configurationPath() {
    wchar_t module[MAX_PATH]{};GetModuleFileNameW(nullptr,module,MAX_PATH);
    return std::filesystem::path(module).parent_path().parent_path()/L"DK2VR.ini";
}
float iniFloat(const wchar_t* name,float fallback,float min,float max) {
    wchar_t value[64]{};GetPrivateProfileStringW(L"VR",name,L"",value,64,configurationPath().c_str());
    if(!value[0]) return fallback;
    wchar_t* end=nullptr;float parsed=wcstof(value,&end);
    return end!=value&&std::isfinite(parsed)?std::clamp(parsed,min,max):fallback;
}
int iniInt(const wchar_t* name,int fallback) {return int(GetPrivateProfileIntW(L"VR",name,fallback,configurationPath().c_str()));}
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
    void tick() {
        if(extra<=0||failed||native.frontend()) return;
        auto* c=native.camera(); if(!c) return;
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
    double lastStickMs=0;float stickRestX=0,stickRestY=0,stickSpeed=.6f;uint64_t stickPans=0;
    NativeControl(const NativeCamera& n):native(n) {
        grabMode=iniInt(L"GrabMode",2);stickSpeed=iniFloat(L"StickScrollSpeed",.6f,.05f,5.f);
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
        if(!usable) {lastStickMs=0;return;}
        // 0.6.2: thumbstick scrolling (all directions, relative to where the player looks), StickScrollSpeed table metres/s.
        {
            const double now=milliseconds();const float dt=lastStickMs>0?float(std::min(now-lastStickMs,50.)*.001):0.f;lastStickMs=now;
            if(hands==0&&dt>0&&(r.stickX!=0||r.stickY!=0)&&std::isfinite(r.stickX)&&std::isfinite(r.stickY)) {
                const Quat yaw=yawOnly(r.eyes[0].pose.orientation);
                Vec3 side=rotate(yaw,{1,0,0}),fwd=rotate(yaw,{0,0,-1});
                Vec3 delta=add(scale(side,r.stickX*stickSpeed*dt),scale(fwd,r.stickY*stickSpeed*dt));
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
                        if(step[0]||step[1]) {moveRel(c,step);++stickPans;}
                    }
                }
            }
        }
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
struct WheelZoom {
    using ZoomRel=void(__thiscall*)(void*,int32_t);
    inline static WheelZoom* active=nullptr;
    const NativeCamera& native;ZoomRel zoomRel=nullptr;WNDPROC previous=nullptr;HWND window=nullptr;int step=50;
    WheelZoom(const NativeCamera& n):native(n) {
        if(!native.supported||iniInt(L"WheelZoom",1)==0) return;
        const unsigned char sig[]={0xf6,0x81,0x92,0x0d,0x00,0x00,0x08};
        auto* f=reinterpret_cast<unsigned char*>(native.base+0x49ca0);
        if(memcmp(f,sig,sizeof(sig))) {log("Wheel zoom signature unsupported");return;}
        zoomRel=reinterpret_cast<ZoomRel>(f);step=int(iniFloat(L"WheelZoomStep",50,5,500));active=this;
    }
    static LRESULT CALLBACK proc(HWND h,UINT m,WPARAM w,LPARAM l) {
        if(m==WM_MOUSEWHEEL&&active&&active->zoomRel&&!active->native.frontend()) {
            if(auto* c=active->native.camera()) active->zoomRel(c,-int32_t(GET_WHEEL_DELTA_WPARAM(w))*active->step);
        }
        return CallWindowProcW(active&&active->previous?active->previous:DefWindowProcW,h,m,w,l);
    }
    void attach(HWND w) {
        if(!zoomRel||!w||window==w) return;
        if(window) return; // keep the first game window only
        previous=reinterpret_cast<WNDPROC>(SetWindowLongPtrW(w,GWLP_WNDPROC,reinterpret_cast<LONG_PTR>(&proc)));
        if(previous) {window=w;log("Mouse wheel zoom enabled (step %d)",step);} else log("Mouse wheel zoom unavailable (%lu)",(unsigned long)GetLastError());
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
struct LensHack {
    // 0.6.9.2: hooked one level higher, on the engine's view setup 0x575A00 (cdecl: view struct, float; verified:
    // copies +0x30 -> 0x760AE8 = focal used by the tile selection 0x574820 / screen picking 0x572920, then calls
    // the projection setup 0x58A6F0 with those globals). 0.6.9 scaled only 0x58A6F0: the tile selection kept the
    // unscaled focal with the shrunk rectangle -> tiles culled 3x too narrow = holes around the table.
    using Setup=int(__cdecl*)(void*,float);
    inline static Setup original=nullptr;inline static volatile float want=1.f,applied=1.f;inline static volatile LONG calls=0;
    static int __cdecl hook(unsigned char* p,float extra) {
        InterlockedIncrement(&calls);
        const float k=want;
        if(k<.999f&&p&&readable(p,0x54)) {
            unsigned char copy[0x60];memcpy(copy,p,0x54);memset(copy+0x54,0,0xc);
            float f;memcpy(&f,copy+0x30,4);
            if(std::isfinite(f)&&f>1) {f*=k;memcpy(copy+0x30,&f,4);applied=k;lodThresholds(k);return original(copy,extra);}
        }
        applied=1.f;lodThresholds(1.f);return original(p,extra);
    }
    // 0.6.9.5: texture / model detail. The engine picks one of 4 detail levels per mesh from
    // ratio = focal(0x760AE8) * size / depth / textureSize, compared with 1.0 / 0.5 / 0.25 (doubles 0x66FC00/08/10)
    // at 7 places (0x57F050, 0x57F92F, 0x580017, 0x5815DA, 0x585024, 0x58630B, 0x5871D6; verified in the 1.70 exe).
    // With the focal scaled by k every ratio is k times smaller -> lower-resolution textures (0.6.9.2-0.6.9.4 blur).
    // These 21 compares are redirected to our own thresholds k, k/2, k/4: same choice as the unscaled camera.
    inline static double thresholds[3]={1.0,.5,.25};
    static void lodThresholds(float k) {thresholds[0]=k;thresholds[1]=k*.5;thresholds[2]=k*.25;}
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
        if(iniInt(L"TextureDetailFix",1)!=0) installLod(base);
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
};
// 0.6.9.7: game pause state. 0x527270 (thiscall, 1 arg: 1 = pause, 0 = resume; verified in the 1.70 exe) sets
// +0x284 (paused) of the game object; called by the interface pause command (options screen) and on resume.
// The options panel follows this flag instead of guessing from the draws (0.6.9.5/0.6.9.6: stayed open).
struct PauseWatch {
    using Fn=void(__fastcall*)(void*,void*,int);
    inline static Fn original=nullptr;inline static volatile LONG paused=-1;inline static volatile LONG changes=0;
    static void __fastcall hook(void* self,void* edx,int on) {original(self,edx,on);paused=on?1:0;InterlockedIncrement(&changes);}
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
struct TopDownCamera {
    using Rebuild=void(__thiscall*)(void*);
    inline static Rebuild rebuild=nullptr;
    bool enabled=true,active=false,flipped=false,checked=false;int16_t original=0,target=0x1cc;float baseline=0;int frames=0;uint32_t rewrites=0;
    void init(const NativeCamera& native) {
        enabled=iniInt(L"TopDownCamera",1)!=0&&native.supported;
        const unsigned char sig[]={0x8b,0x44,0x24,0x04,0x66,0x8b,0x10,0x66,0x89,0x91,0x84,0x0d,0x00,0x00,0xe8};
        if(enabled&&memcmp(reinterpret_cast<void*>(native.base+0x49f50),sig,sizeof(sig))) {enabled=false;log("Top-down camera: signature unsupported");}
        if(enabled) {rebuild=reinterpret_cast<Rebuild>(native.base+0x4d6d0);log("Top-down camera: on (TopDownCamera=1)");}
    }
    static float down(const NativeCamera& native) {return native.read<float>(0x760ac4+8*4);}
    void apply(const NativeCamera& native,bool want) {
        if(!enabled) return;
        unsigned char* c=native.camera();if(!c) return;
        int16_t& tilt=*reinterpret_cast<int16_t*>(c+0xd86);
        if(want) {
            if(!active) {active=true;original=tilt;baseline=down(native);frames=0;checked=false;log("Top-down camera: tilt %d -> %d (downward %.3f)",int(original),int(target),baseline);}
            if(tilt!=target) {if(frames>2) ++rewrites;tilt=target;rebuild(c);}
            if(++frames==4&&!checked) {
                checked=true;float now=down(native);
                if(now<baseline-.01f&&!flipped) {flipped=true;target=int16_t(target==0?0x1cc:0);tilt=target;rebuild(c);log("Top-down camera: the other end looks down (downward %.3f < %.3f): tilt %d",now,baseline,int(target));}
                else log("Top-down camera: downward component %.3f (was %.3f)",now,baseline);
            }
        } else if(active) {
            active=false;tilt=original;rebuild(c);log("Top-down camera: player tilt %d restored (rewrites while active: %u)",int(original),rewrites);rewrites=0;
        }
    }
};
// 0.5.16 diagnostic: who uploads textures. paintSurf (0x590450, thiscall(src,x,y), ret 0xC, verified) copies a
// texture into the game's texture cache; 0.5.15 measured ~2200 uploads per second even with a still view.
// Its 7 direct callers (verified: call rel32 to 0x590450) are counted separately. Log: "Texture uploads".
struct PaintProbe {
    using Paint=void(__fastcall*)(void*,void*,void*,int,int);
    inline static Paint original=nullptr;inline static volatile LONG counts[7]{};
    inline static const uint32_t sites[7]={0x590d3f,0x5912b4,0x591bc9,0x5939d2,0x593ddf,0x5940b8,0x594771};
    inline static const char* names[7]={"paint","loadPrescaled","copyScaledWithType","SurfHashList2 fill A","SurfHashList2 fill B","_probablySort (eviction)","expandPut (new texture)"};
    template<int N> static void __fastcall thunk(void* self,void*,void* src,int x,int y) {InterlockedIncrement(&counts[N]);original(self,nullptr,src,x,y);}
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
    static void report() {
        if(!original) return;
        char line[700];int n=snprintf(line,sizeof(line),"Texture uploads (last 5 s):");
        for(int i=0;i<7;++i) {LONG c=InterlockedExchange(&counts[i],0);if(c) n+=snprintf(line+n,sizeof(line)-n," | %s %ld",names[i],(long)c);}
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
    static LONG CALLBACK handler(EXCEPTION_POINTERS* e) {
        const DWORD code=e->ExceptionRecord->ExceptionCode;
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
        if(AddVectoredExceptionHandler(1,&handler)) log("Crash log: exception handler installed");
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
    bool enabled=false;uint64_t lastReport=0;
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
        running=1;enabled=true;lastReport=GetTickCount64();
        worker=CreateThread(nullptr,0,&loop,this,0,nullptr);
        log("Profiler started on render thread %lu",(unsigned long)threadId);
    }
    static DWORD WINAPI loop(void* self) {
        auto* p=static_cast<Profiler*>(self);
        timeBeginPeriod(1); // finer Sleep for ~500 samples/s; released at process exit
        while(p->running) {
            Sleep(2);
            if(p->lastPresentTick&&GetTickCount64()-p->lastPresentTick>3000) {if(!p->stallLogged) {p->stallLogged=true;p->stallDump();}continue;}
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
        if(!enabled||GetTickCount64()-lastReport<10000) return;
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
    static void* __cdecl generate() {
        if(rectValid) {
            // 0.5.16: the engine's rectangle becomes (original rectangle) U (table rectangle on the screen).
            float saved[5]={at(0x760aec),at(0x760af0),at(0x760af4),at(0x760af8),at(0x760b04)};
            at(0x760aec)=std::min(saved[0],float(rectL));at(0x760af0)=std::max(saved[1],float(rectR));
            at(0x760af4)=std::min(saved[2],float(rectT));at(0x760af8)=std::max(saved[3],float(rectB));
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
        base=native.base;
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
struct NativeMouse {
    using Update = void (__thiscall*)(void*);
    inline static NativeMouse* active=nullptr;
    const NativeCamera& camera;
    HWND window=nullptr;
    Update original=nullptr;
    bool enabled=true;
    NativeMouse(const NativeCamera& c):camera(c) {
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
    void correct(void* object) {
        if(!object||!window||GetForegroundWindow()!=window||IsIconic(window)||!readable(object,0x64)) return;
        auto mode=camera.mode();
        if(mode==1||mode==2||(GetAsyncKeyState(VK_MBUTTON)&0x8000)||(GetAsyncKeyState(VK_MENU)&0x8000)) return;
        auto* bytes=static_cast<unsigned char*>(object);
        int bounds[4]{};memcpy(bounds,bytes+0x34,sizeof(bounds));
        float w=float(bounds[2]-bounds[0]),h=float(bounds[3]-bounds[1]);
        RECT client{};POINT cursor{};
        if(w<=1||h<=1||!GetClientRect(window,&client)) return;
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
    bool cursorPixel(float W,float H,float& px,float& py) const {
        if(!camera.supported) return false;
        auto* o=reinterpret_cast<unsigned char*>(uintptr_t(camera.read<uint32_t>(0x79cff4)));
        if(!o||!readable(o,0x64)) return false;
        int b[4]{};memcpy(b,o+0x34,sizeof(b));float w=float(b[2]-b[0]),h=float(b[3]-b[1]);
        if(w<=1||h<=1) return false;
        float x=0,y=0;memcpy(&x,o+0x20,4);memcpy(&y,o+0x24,4);
        float k=std::min(W/w,H/h);px=(x-b[0])*k+(W-w*k)*.5f;py=(y-b[1])*k+(H-h*k)*.5f;
        return std::isfinite(px)&&std::isfinite(py);
    }
    static void __fastcall hook(void* self,void*) {
        if(active) {active->correct(self);active->original(self);}
    }
    // 0.6.0: VR pointer (host ray on a HUD panel). The Windows cursor follows it so that the
    // synthetic clicks land in the game window; buttons are sent with SendInput on edges only.
    bool pointerOn=false,leftDown=false,rightDown=false;float pointerX=0,pointerY=0;
    uint32_t pointerLogged=0;
    void pointer(const Request& r) {
        bool on=(r.pointerFlags&PointerActive)&&std::isfinite(r.pointerX)&&std::isfinite(r.pointerY);
        bool wantLeft=on&&(r.pointerFlags&PointerLeft),wantRight=on&&(r.pointerFlags&PointerRight);
        if(on) {
            pointerX=std::clamp(r.pointerX,0.f,1.f);pointerY=std::clamp(r.pointerY,0.f,1.f);
            RECT client{};
            if(window&&GetClientRect(window,&client)) {
                POINT p{LONG(pointerX*client.right),LONG(pointerY*client.bottom)};
                if(ClientToScreen(window,&p)) SetCursorPos(p.x,p.y);
            }
        }
        if(on!=pointerOn&&pointerLogged<20) {++pointerLogged;log("VR pointer %s",on?"on a HUD panel":"off");}
        if(!on&&pointerOn&&window) {
            // 0.6.2: leaving a panel parks the cursor in the middle of the game view (a cursor left on an
            // edge made the game scroll the map continuously).
            RECT client{};if(GetClientRect(window,&client)) {POINT p{client.right/2,client.bottom/2};if(ClientToScreen(window,&p)) SetCursorPos(p.x,p.y);}
        }
        pointerOn=on;
        auto send=[](DWORD flag){INPUT in{};in.type=INPUT_MOUSE;in.mi.dwFlags=flag;SendInput(1,&in,sizeof(in));};
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
        pixels.swap(out);w=cw;h=ch;hotX=-ox;hotY=-oy;++serial;++captures;lastKey=key;lastBits=bpp;lastOpaque=opaque;
    }
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
struct Snapshot {
    static constexpr D3DRENDERSTATETYPE renderStates[]={D3DRS_ZWRITEENABLE,D3DRS_ALPHATESTENABLE,D3DRS_ALPHAREF,D3DRS_ALPHAFUNC,
        D3DRS_ALPHABLENDENABLE,D3DRS_SRCBLEND,D3DRS_DESTBLEND,D3DRS_BLENDOP,D3DRS_SEPARATEALPHABLENDENABLE,D3DRS_SRCBLENDALPHA,
        D3DRS_DESTBLENDALPHA,D3DRS_BLENDOPALPHA,D3DRS_TEXTUREFACTOR,D3DRS_SHADEMODE,D3DRS_SPECULARENABLE,D3DRS_DITHERENABLE,
        D3DRS_COLORWRITEENABLE,D3DRS_STENCILENABLE,D3DRS_FILLMODE,D3DRS_WRAP0,D3DRS_SRGBWRITEENABLE,D3DRS_POINTSIZE};
    static constexpr D3DTEXTURESTAGESTATETYPE stageStates[]={D3DTSS_COLOROP,D3DTSS_COLORARG1,D3DTSS_COLORARG2,D3DTSS_ALPHAOP,
        D3DTSS_ALPHAARG1,D3DTSS_ALPHAARG2,D3DTSS_TEXCOORDINDEX,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTSS_COLORARG0,D3DTSS_ALPHAARG0,
        D3DTSS_RESULTARG,D3DTSS_BUMPENVMAT00,D3DTSS_BUMPENVMAT01,D3DTSS_BUMPENVMAT10,D3DTSS_BUMPENVMAT11};
    static constexpr D3DSAMPLERSTATETYPE samplerStates[]={D3DSAMP_ADDRESSU,D3DSAMP_ADDRESSV,D3DSAMP_MAGFILTER,D3DSAMP_MINFILTER,
        D3DSAMP_MIPFILTER,D3DSAMP_MIPMAPLODBIAS,D3DSAMP_MAXANISOTROPY,D3DSAMP_BORDERCOLOR};
    static constexpr int Stages=3,Samplers=3;
    static constexpr size_t RS=sizeof(renderStates)/sizeof(renderStates[0]),TS=sizeof(stageStates)/sizeof(stageStates[0]),SS=sizeof(samplerStates)/sizeof(samplerStates[0]);
    IDirect3DBaseTexture9* texture[Samplers]{};IDirect3DPixelShader9* pixel=nullptr;
    DWORD rs[RS]{},ts[Stages][TS]{},ss[Samplers][SS]{};
    void release() {for(auto& t:texture) if(t){t->Release();t=nullptr;} if(pixel){pixel->Release();pixel=nullptr;}}
    bool capture(IDirect3DDevice9* d) {
        for(int i=0;i<Samplers;++i) if(FAILED(d->GetTexture(i,&texture[i]))) return false;
        if(FAILED(d->GetPixelShader(&pixel))) return false;
        for(size_t i=0;i<RS;++i) d->GetRenderState(renderStates[i],&rs[i]);
        for(int t=0;t<Stages;++t) for(size_t i=0;i<TS;++i) d->GetTextureStageState(t,stageStates[i],&ts[t][i]);
        for(int t=0;t<Samplers;++t) for(size_t i=0;i<SS;++i) d->GetSamplerState(t,samplerStates[i],&ss[t][i]);
        return true;
    }
    bool same(const Snapshot& o) const {
        return !memcmp(texture,o.texture,sizeof(texture))&&pixel==o.pixel&&!memcmp(rs,o.rs,sizeof(rs))&&!memcmp(ts,o.ts,sizeof(ts))&&!memcmp(ss,o.ss,sizeof(ss));
    }
    void apply(IDirect3DDevice9* d) const {
        for(int i=0;i<Samplers;++i) d->SetTexture(i,texture[i]);
        d->SetPixelShader(pixel);
        for(size_t i=0;i<RS;++i) d->SetRenderState(renderStates[i],rs[i]);
        for(int t=0;t<Stages;++t) for(size_t i=0;i<TS;++i) d->SetTextureStageState(t,stageStates[i],ts[t][i]);
        for(int t=0;t<Samplers;++t) for(size_t i=0;i<SS;++i) d->SetSamplerState(t,samplerStates[i],ss[t][i]);
    }
};
struct DrawRecord {
    D3DPRIMITIVETYPE type;UINT primitives,count,stride;DWORD fvf;
    size_t vertices,indices;D3DFORMAT indexFormat;bool indexed;uint32_t snapshot;
    bool hand=false; // 0.6.3: part of the Keeper's hand (3D cursor) -> drawn on the controller
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
struct Renderer {
    TextureQuality textureQuality;
    Ipc ipc; NativeCamera native; NativeMouse mouse{native};KeeperCursor keeper{native}; NativeZoom zoom{native}; NativeControl control{native}; WheelZoom wheel{native};
    bool batched=true;std::vector<unsigned char> arena,eyeArena,indexArena;std::vector<DrawRecord> records;std::vector<Snapshot> snapshots;
    double recordMilliseconds=0,replayMilliseconds=0,statsReplay=0;
    // F12: VR pause (the mod stops recording/replaying) to measure the game alone in the same scene.
    bool bypass=false,f12Down=false;double presentMilliseconds=0,statsPresent=0;
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
    struct Pending {int slot=-1;Eye eyes[2]{};Pose anchor{};uint32_t world=0,ui=0,rejected=0,mode=0;} pending;
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
        out.worldDraws=pending.world;out.uiDraws=pending.ui;out.rejectedDraws=pending.rejected;out.cameraMode=pending.mode;
        MemoryBarrier();InterlockedExchange(&out.state,2);SetEvent(ipc.ready);pending.slot=-1;
        (void)d;return true;
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
    HudRegion msgRegion{0,0,0,0};float heldKeep[4]={1e9f,1e9f,-1e9f,-1e9f};bool nativeZDirty=false,probeFrame=false,zOverridden=false;DWORD gameZWrite=TRUE;int probeLines=0;bool hudFromImage=true,hideNativeWorld=true,hideNative=false,uiWentToHeld=false;bool heldModelSeen=false,foldBlocked=false,ui3dFilter=true,pickFits=false,screenWhole=true,msgFrame=false,msgShown=false;int msgHold=0;float msgX0=1e9f,msgY0=1e9f,msgX1=-1e9f,msgY1=-1e9f,boxX0=1e9f,boxY0=1e9f,boxX1=-1e9f,boxY1=-1e9f,prevBox[4]={1e9f,1e9f,-1e9f,-1e9f};int uiDiagLogs=0;uint64_t statCards=0;float worldRhw=0;uint64_t statUi3d=0;float pickMin=.30f,pickTop=.07f,pickBottom=.68f,pickWant=1.f;uint64_t pickLogs=0;
    float pickScale=.34f;ComPtr<IDirect3DTexture9> screenTex;ComPtr<IDirect3DSurface9> screenSurf;bool screenCleared=false,screenFailed=false;int pedestalSide=0;bool buttonHover=false;uint64_t statButtonClicks=0;bool screenActive=false,lastPointerOnTable=false;int screenHold=0;float midX0=1e9f,midY0=1e9f,midX1=-1e9f,midY1=-1e9f;uint64_t statScreenFrames=0;bool pickActive=false,pickFound=false,lastPickValid=false;float pickT=0,lastPickDist=0,lastPickFx=0,lastPickFy=0;uint64_t statPickHits=0,statPlaneHits=0;Vec3 pickO{},pickD{};Projection pointerLens{};bool pointerLensOk=false,tablePointer=true;uint64_t statTableHits=0,statTableClicks=0;float lastTableX=0,lastTableY=0;UINT lastHudW=0,lastHudH=0;float lastHudTop=0;bool hudLatchL=false,hudLatchR=false,hudTrigBefore=false,hudSecondBefore=false;int hudTarget=-1;
    uint64_t statUiDraws=0,statCursorFrames=0,statHudFrames=0,statHudHits=0,statHudClicks=0;float lastPointerX=-1,lastPointerY=-1;
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
    float panMarginMeters=.05f;uint64_t rectLogs=0;
    bool projectTable(const Projection& lens,ScreenRect area,const Tabletop& box) {
        const float margin=1.5f+panMarginMeters*std::max(activeUnits,1.f)+.08f*std::max(box.halfWidth,box.halfDepth);
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
        ViewExpand::autoFar=6.f;
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
    void updateExpansion(const Projection& lens,ScreenRect area,const Tabletop& box) {
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
         ViewExpand::autoFar=dm>1e-3f?std::clamp(dc/dm*1.10f,1.f,4.f):1.f;
         // 0.5.15: with the top-down camera the engine's distance limit is a sphere around a HIGH camera: the table's
         // corners were cut (0.5.14: only a cross of tiles when zoomed out). The tiles stay limited by the view
         // rectangle, so a larger distance costs little.
         if(topDown.active) ViewExpand::autoFar=std::max(float(ViewExpand::autoFar),3.f);}
        ViewExpand::autoX=std::min(ex,5.f);ViewExpand::autoBottom=std::min(eb,6.f);
        if(expandLogs<8&&(std::fabs(ex-loggedEx)>.15f||std::fabs(eb-loggedEy)>.15f)) {
            ++expandLogs;loggedEx=ex;loggedEy=eb;
            log("TableView: table %.1f x %.1f, engine view x%.2f on the sides, x%.2f towards the player, far distance x%.2f (top unchanged)",box.halfWidth*2,box.halfDepth*2,ex,eb,float(ViewExpand::autoFar));
        }
    }
    Tabletop table{};Pose tableCamera{};float activeUnits=10;bool anchored=false,possessedFrame=false;
    // Last valid table anchor, reused while the native camera transiently gives no floor.
    bool haveAnchor=false;Tabletop anchorUnits{};bool loggedAnchor=false;uint64_t anchorFallbacks=0;
    double drawMilliseconds=0,syncMilliseconds=0,statsDraw=0,statsSync=0,statsStarted=0;
    uint64_t statsFrames=0,statsVertices=0,statsWorld=0;
    std::vector<unsigned char> converted;
    std::vector<Vec3> positions;
    // 0.3.0 decoupled rendering (DecoupledRendering=1). 0 = the 0.2.9 path, which also stays in use for
    // menus, possession and the flat-screen mode.
    bool decoupledEnabled=true,decoupledFrame=false,frameOpen=false,lastAnchored=false;
    VrThread* vr=nullptr;CopyCache copies;ShaderCache shaders;std::vector<std::array<uint32_t,4>> snapIds;
    uint64_t packageId=0,handed=0,dropped=0,statPackages=0;std::shared_ptr<Package> pendingPackage;ComPtr<IDirect3DQuery9> packageFences[2];
    double decoupledSince=0,retryAt=0;int watchdogTrips=0;
    bool ensureVr(IDirect3DDevice9* d) {
        if(!decoupledEnabled) return false;
        if(vr) return !vr->failed;
        vr=new VrThread();
        TextureWatch::installDevice(d);
        if(!vr->create(d,&ipc)) return false;
        copies.mips=vr->mips;
        return true;
    }
    // Hands the previous game frame to the VR thread as soon as its GPU work is complete.
    void pollPackage(bool block) {
        if(!pendingPackage) return;
        auto& q=packageFences[pendingPackage->id&1];
        HRESULT hr=q?q->GetData(nullptr,0,0):E_FAIL;
        if(hr==S_FALSE&&block) {
            uint64_t deadline=GetTickCount64()+250;
            do {hr=q->GetData(nullptr,0,D3DGETDATA_FLUSH);if(hr==S_FALSE) Sleep(0);} while(hr==S_FALSE&&GetTickCount64()<deadline);
        }
        if(hr==S_FALSE) return;
        if(hr==S_OK&&vr&&!vr->failed) {vr->handOff(std::move(pendingPackage));++handed;}
        else {++dropped;if(hr!=S_OK) log("Package GPU synchronization failed %08lX",(unsigned long)hr);}
        pendingPackage.reset();
    }
    Renderer() {
        tableEnabled=iniInt(L"TableBoundary",1)!=0;
        measuredLevels=iniInt(L"MeasuredLevels",1)!=0;heights.reserve(40000);
        tableCrop=iniFloat(L"TableCrop",.92f,.5f,1.f);
        tableAspect=iniFloat(L"TableAspect",.75f,0.f,3.f);
        tableView=iniInt(L"TableView",1);
        environmentOn=iniInt(L"Environment",1)!=0;
        // 0.4.3: the room is prepared from the game start (menus), cached in the mod's Cache folder.
        if(environmentOn&&iniInt(L"DecoupledRendering",1)!=0) {
            std::error_code ec;auto dir=configurationPath().parent_path()/L"Cache";std::filesystem::create_directories(dir,ec);
            env::globalScene().startBackground((dir/L"DK2VR_room.cache").wstring());
        }
        fastStates=iniInt(L"FastStates",1)!=0;
        drawHands=iniInt(L"ShowControllers",1)!=0;
        batched=iniInt(L"BatchedRendering",1)!=0;
        ViewExpand::install(native);
        topDown.init(native);
        PauseWatch::install(native.base);
        PoolGrow::install(native.base);
        LensHack::install(native.base);pickScale=iniFloat(L"TablePickScale",.30f,.15f,1.f);
        // 0.6.9.3: the focal factor is now the LARGEST one that still puts the whole table between the bars
        // (TablePickScale = lower limit): x0.30 fixed made the game choose low-resolution textures.
        pickMin=pickScale;pickTop=iniFloat(L"TablePickTop",.07f,0.f,.4f);pickBottom=iniFloat(L"TablePickBottom",.68f,.4f,1.f);
        ui3dFilter=iniInt(L"Ui3DFilter",1)!=0;hudFromImage=iniInt(L"HudFromGameImage",1)!=0;hideNativeWorld=hudFromImage&&iniInt(L"HideNativeWorld",1)!=0;
        LockProbe::install(native);
        PaintProbe::install(native);
        hudSizes.wall=iniFloat(L"HudWallWidth",1.8f,.3f,5.f);hudSizes.bottom=iniFloat(L"HudBottomWidth",.80f,.15f,3.f);
        hudSizes.minimap=iniFloat(L"HudMinimapWidth",.30f,.05f,1.f);hudSizes.cursor=iniFloat(L"CursorSize",.10f,.02f,.5f);
        hudErase=iniInt(L"HudHideCursor",1)!=0;handToController=iniInt(L"HandFrom3D",0)!=0;keeperHeld=iniInt(L"HeldThingsOnHand",1)!=0;tablePointer=iniInt(L"TablePointer",1)!=0;pedestalSide=std::clamp(userInt(L"MinimapPedestalSide",0),0,2);heldWindow=iniInt(L"HeldWindow",1)!=0;uiTexFilter=iniInt(L"InterfaceCardsOutOfDungeon",0)!=0;heldRadius=iniFloat(L"HeldRadius",200.f,40.f,400.f);cursorFromUi=iniInt(L"CursorFromInterface",0)!=0;
        hideFloatingUi=iniInt(L"HideFloatingUi",1)!=0;panMarginMeters=iniFloat(L"PanPredictionLimit",.05f,0.f,.5f);
        BackFaces::install(native);
        TextureCache::install(native);
        textureQuality.install(native);
        NoVBlankWait::install();
        pipelined=iniInt(L"PipelinedSync",1)!=0;
        decoupledEnabled=iniInt(L"DecoupledRendering",1)!=0;
        copies.refreshFrames=int(iniFloat(L"TextureRefreshFrames",180,0,100000));
        log("Decoupled VR rendering: %s (TextureRefreshFrames=%d)",decoupledEnabled?"enabled":"disabled (0.2.9 path)",copies.refreshFrames);
        statsStarted=milliseconds();
        log("Settings: TableBoundary=%d TableCrop=%.2f FastStates=%d BatchedRendering=%d ShowControllers=%d ZoomOutExtra=%.2f ViewExpand=%.2f ViewFarScale=%.2f",
            tableEnabled,tableCrop,fastStates,batched,drawHands,zoom.extra,ViewExpand::expand,ViewExpand::farScale);
    }
    ~Renderer() { if(hostProcess) CloseHandle(hostProcess); }
    void clearBatch() {for(auto& sn:snapshots) sn.release();snapshots.clear();snapIds.clear();records.clear();arena.clear();indexArena.clear();}
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
        if(slot>=0&&ipc.data) InterlockedExchange(&ipc.data->slots[slot].state,0);
        clearBatch();dropPending();for(auto& f:fences) f.Reset();
        if(vr) {vr->active=false;vr->dropAll();}
        pendingPackage.reset();for(auto& f:packageFences) f.Reset();copies.releaseAll();shaders.clear();frameOpen=false;decoupledFrame=false;
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
        decoupledFrame=lastAnchored&&request.tableAnchor&&request.mode!=3&&!possessedMode&&!native.frontend()&&milliseconds()>=retryAt&&ensureVr(d);
        HRESULT hr=S_OK;
        auto fail=[&](const char* s,HRESULT hr) { log("%s failed: %08lX",s,(unsigned long)hr); if(slot>=0) InterlockedExchange(&ipc.data->slots[slot].state,0);slot=-1; return false; };
        if(decoupledFrame) {
            if(!vr->active) {vr->dropMailbox();pendingPackage.reset();vr->active=true;decoupledSince=milliseconds();log("Decoupled VR rendering: ON (headset images drawn by the VR thread)");}
            pollPackage(false);
            uint64_t safe=std::min(vr->safeBelow(),pendingPackage?pendingPackage->id:NoPackage);
            ++packageId;
            copies.beginFrame(packageId,std::min(safe,packageId));
        } else {
        if(vr&&vr->active) {vr->active=false;pendingPackage.reset();log("Decoupled VR rendering: paused (menu, possession, flat mode or no table)");}
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
        world=ui=rejected=0; mode=native.mode();table={};tablePrepared=false;anchored=false;possessedFrame=false;
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
        frameOpen=true;
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
    // Chooses, once per game frame, where the native camera sits in VR space.
    void prepareFrame(const Projection& lens,bool possessed) {
        tablePrepared=true;possessedFrame=possessed;TileCover::tableValid=0;ViewExpand::rectValid=0;
        pointerLens=lens;pointerLensOk=true;
        pickActive=tablePointer&&!possessed&&(request.pointerFlags&PointerAimValid)&&!(request.pointerFlags&PointerActive)&&!native.frontend();
        pickFound=false;pickT=6.f;pickO=request.rayOrigin;pickD=request.rayDirection;
        activeUnits=possessed?request.possessionUnitsPerMeter:request.unitsPerMeter;
        Pose camera=request.anchor;
        camera.orientation=multiply(camera.orientation,pitch(possessed?0:-request.tablePitchDegrees*Pi/180));
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
            if(std::fabs(target-pickWant)>.03f) pickWant+=(target-pickWant)*.15f;
            pickWant=std::clamp(pickWant,pickMin,1.f);
            if(pickLogs<12&&std::fabs(pickWant-target)<.01f&&std::fabs(target-float(LensHack::applied))>.05f) {++pickLogs;log("Table picking: focal factor -> x%.2f (min x%.2f, bars %.2f-%.2f)",target,pickMin,pickTop,pickBottom);}
            LensHack::want=pickWant;
        } else {LensHack::want=1.f;pickFits=false;}
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
        return !uiFailed&&decoupledFrame&&(request.handFlags&RequestHudShown)&&request.mode!=3&&!native.frontend();
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
            middleDraw=cy>.10f&&cy<.69f&&z1-z0<=1e-4f;
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
            const bool bigBox=middleDraw&&x1-x0>=.08f*uiW&&y1-y0>=.08f*uiH&&x1-x0<=.70f*uiW&&y1-y0<=.55f*ihb&&tyb>=.085f&&byb<=.72f;
            if(bigBox) {msgFrame=true;boxX0=std::min(boxX0,x0);boxY0=std::min(boxY0,y0);boxX1=std::max(boxX1,x1);boxY1=std::max(boxY1,y1);}
            if(middleDraw&&(!nearCursor||(msgShown&&!lastPointerOnTable))) {
                msgX0=std::min(msgX0,x0);msgY0=std::min(msgY0,y0);msgX1=std::max(msgX1,x1);msgY1=std::max(msgY1,y1);
            }
            // Text / buttons of last frame's box (not the held things at the cursor while the laser is on the table).
            // 0.6.9.13: the held creature (pieces with varying depth near the cursor) is drawn after the interface: what is
            // drawn inside the box after it (count of things held) belongs to the hand, the rest (text, X/check) to the box.
            if(nearCursor&&z1-z0>1e-4f) {heldModelSeen=true; // 0.7.0: bounding box of the held model (kept when the bars are erased)
                heldKeep[0]=std::min(heldKeep[0],x0);heldKeep[1]=std::min(heldKeep[1],y0);heldKeep[2]=std::max(heldKeep[2],x1);heldKeep[3]=std::max(heldKeep[3],y1);}
            const bool inBox=cx>=prevBox[0]&&cx<=prevBox[2]&&cyy>=prevBox[1]&&cyy<=prevBox[3]&&!(heldModelSeen&&nearCursor);
            // 0.6.9.11: message box pieces only go to the message panel, never to the interface texture (the hand's
            // window is cut from it: erasing the box there also erased the held creature drawn over it).
            boxOnly=PauseWatch::paused!=1&&(bigBox||inBox);
            middleToScreen=middleDraw&&(bigBox||inBox||!(nearCursor&&lastPointerOnTable&&PauseWatch::paused!=1));
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
            HudLayout L=layoutHud(r,hudSizes,depthRatio,lastHudTop,float(lastHudW),float(lastHudH),screenActive,pedestalSide,screenWhole,&msgRegion);
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
    bool clipping() const {return table.valid&&!possessedFrame&&tableEnabled&&clipSupported;}
    void draw(IDirect3DDevice9* d,D3DPRIMITIVETYPE type,UINT primitives,const void* verts,UINT stride,const void* indices,D3DFORMAT indexFormat,UINT minIndex,UINT count) {
        // 0.6.9.16: this hook runs just BEFORE the game's own draw (WrapperBridge). The dungeon draws recorded for the
        // headset, and the things held by the Keeper, are made invisible in the game's image (colour writes off): the
        // image then holds only the interface over black, exactly composed by the game -> source of the VR bars.
        // COLORWRITEENABLE is a D3D9 state the DX7 game never sets; it is written for EVERY draw (on or off).
        // 0.7.2: a hidden dungeon draw writes no depth either (probe: its depth hid the '0' button and small panel pieces
        // drawn later in the game image). The game's ZWRITEENABLE is put back at the start of the next draw, unless the
        // game has set it itself in between.
        struct NativeWrite {IDirect3DDevice9* d;const bool& hide;Renderer* r;
            ~NativeWrite(){if(!d) return;d->SetRenderState(D3DRS_COLORWRITEENABLE,hide?0u:15u);
                if(hide) {DWORD z=TRUE;d->GetRenderState(D3DRS_ZWRITEENABLE,&z);if(z) {r->gameZWrite=z;r->zOverridden=true;d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);}}}};
        hideNative=false;uiWentToHeld=false;
        if(zOverridden) {zOverridden=false;DWORD z=TRUE;d->GetRenderState(D3DRS_ZWRITEENABLE,&z);if(!z) d->SetRenderState(D3DRS_ZWRITEENABLE,gameZWrite);}
        NativeWrite nativeWrite{hideNativeWorld&&!bypass?d:nullptr,hideNative,this};
        // 0.6.9.17: colour writes back ON before anything reads the device state: the draw is recorded for the headset
        // with the current states (0.6.9.16 left 0 from the previous dungeon draw -> the headset replayed an invisible
        // dungeon: empty table). Only the game's own draw, after this hook, sees 0.
        if(hideNativeWorld&&!bypass) d->SetRenderState(D3DRS_COLORWRITEENABLE,15);
        if(bypass||!verts||primitives==0||!start(d)) return;
        // Menus must remain readable even while F7 cycles through manual
        // tabletop/possession modes. Their animated background is a separate
        // native 3D scene, not the dungeon; cloning it hides the menu overlay.
        if(request.mode==3||native.frontend()) { ++ui;return; }
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
        if(allFlat) {
            if(uiTexFilter) {ComPtr<IDirect3DBaseTexture9> t;d->GetTexture(0,&t);if(t&&uiTextures.size()<8192) uiTextures.insert(t.Get());}
            // (0.7.0 cleared the native depth here: 'i' vanished in that version -> removed in 0.7.1.)
            probe("2D interface");
            ++ui;if(hudWanted()) uiReplay(d,type,primitives,verts,stride,indices,indexFormat,count,fvf);hideNative=uiWentToHeld;return;} // Native 2D path writes RHW=1
        // 0.6.5.3: interface cards (portraits, room/spell tabs) are animated by the game in 3D (RHW != 1) when
        // hovered or picked up. They use the same textures as the 2D interface: keep them out of the dungeon.
        if(uiTexFilter&&!uiTextures.empty()&&request.mode!=2) {
            ComPtr<IDirect3DBaseTexture9> t;d->GetTexture(0,&t);
            if(t&&uiTextures.count(t.Get())) {++statUiIn3d;if(hudWanted()) uiReplay(d,type,primitives,verts,stride,indices,indexFormat,count,fvf);return;}
        }
        // 0.6.9.3: interface pieces the game animates in 3D (RHW != 1: panel cards, stone strip with a portrait)
        // appeared on the table when zoomed out. Geometric test only (no texture identity): every vertex much
        // nearer the camera than the dungeon (rhw > 3x the typical dungeon rhw) AND drawn over the bottom bars.
        {float r0=1e9f,rs=0,y0=1e9f,y1=-1e9f;
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
             if(overBars||card||(near3&&count<=256&&cy>.69f)) {++statUi3d;probe(overBars?"3D->HUD (bars)":card?"3D->HUD (near card)":"3D->HUD (near)");if(hudWanted()) uiReplay(d,type,primitives,verts,stride,indices,indexFormat,count,fvf);return;}
         }}
        // 0.6.9.16: from here the draw is dungeon for the headset: hidden in the game's image while the table is shown.
        hideNative=anchored&&topDown.active&&!possessedFrame&&!native.frontend()&&request.mode!=2;
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
        if(!tablePrepared) prepareFrame(lens,possessed);
        const float units=activeUnits;
        const Pose camera=tableCamera;
        positions.resize(count);
        for(UINT i=0;i<count;++i) {
            float v[4];memcpy(v,input+i*stride,16);Vec3 p{};
            if(!unproject(v[0],v[1],v[3],lens,units,p)) {++rejected;return;}
            positions[i]=toWorld(camera,p);
            if(anchored&&measuredLevels&&(i&1)==0&&heights.size()<40000) heights.push_back(dot(anchorUnits.up,p)*units);
            if(anchored) frameNearGeom=std::min(frameNearGeom,dot(anchorUnits.forward,p)*units); // 0.5.12: nearest geometry the game produced
        }
        if(pickActive&&anchored) pickDraw(type,primitives,count,indices,indexFormat);
        UINT targetStride=stride-4;
        if(batched||decoupledFrame) {
            DrawRecord rec{type,primitives,count,targetStride,(fvf&~D3DFVF_POSITION_MASK)|D3DFVF_XYZ,arena.size(),indexArena.size(),indexFormat,indices!=nullptr,0};
            arena.resize(arena.size()+size_t(count)*targetStride);
            unsigned char* out=arena.data()+rec.vertices;
            for(UINT i=0;i<count;++i) {memcpy(out+i*targetStride,&positions[i],12);memcpy(out+i*targetStride+12,input+i*stride+16,stride-16);}
            if(indices) {
                size_t bytes=size_t(indexCountFor(type,primitives))*(indexFormat==D3DFMT_INDEX32?4:2);
                indexArena.resize(indexArena.size()+bytes);memcpy(indexArena.data()+rec.indices,indices,bytes);
            }
            Snapshot current{};
            if(!current.capture(d)) {current.release();++rejected;return;}
            std::array<uint32_t,4> ids{};
            if(decoupledFrame) {
                // Up-to-date shared copies of the bound textures (copied now if new or changed by the game).
                CopyCache::Guard guard;guard.d=d;guard.cache=&copies;
                for(int i=0;i<Snapshot::Samplers;++i) ids[i]=copies.copyFor(d,guard,current.texture[i]);
                ids[3]=shaders.idFor(current.pixel);
            }
            if(!snapshots.empty()&&snapshots.back().same(current)&&snapIds.back()==ids) {current.release();rec.snapshot=uint32_t(snapshots.size()-1);}
            else {snapshots.push_back(current);snapIds.push_back(ids);rec.snapshot=uint32_t(snapshots.size()-1);}
            classifyHand(d,rec,input,stride,count);
            records.push_back(rec);++world;statsVertices+=count;
            if(decoupledFrame&&pendingPackage&&(records.size()&63)==0) pollPackage(false);
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
                if(rec.snapshot!=applied) {snapshots[rec.snapshot].apply(d);applied=rec.snapshot;}
                if(rec.fvf!=fvf) {d->SetFVF(rec.fvf);fvf=rec.fvf;}
                HRESULT hr=rec.indexed?d->DrawIndexedPrimitiveUP(rec.type,0,rec.count,rec.primitives,indexArena.data()+rec.indices,rec.indexFormat,eyeArena.data()+rec.vertices,rec.stride):
                    d->DrawPrimitiveUP(rec.type,rec.primitives,eyeArena.data()+rec.vertices,rec.stride);
                if(FAILED(hr)) ++rejected;
            }
            d->EndScene();
        }
        d->SetDepthStencilSurface(nullptr);d->SetRenderTarget(0,rt.Get());d->SetDepthStencilSurface(ds.Get());
        state->Apply();d->SetViewport(&vp);
        clearBatch();
        replayMilliseconds=milliseconds()-started;
    }
    // Table base, rim and controller markers, drawn once per frame at Present.
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
    void drawOverlay(IDirect3DDevice9* d,bool includeBase) {
        if(!world) return;
        using Vertex=OverlayVertex;
        std::vector<Vertex> mesh;mesh.reserve(700);
        if(includeBase) buildBase(mesh);
        if(drawHands) buildHands(request,mesh); // controller bodies, brighter while gripping
        if(mesh.empty()) return;
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
                HRESULT hr=d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(vertices.size()/3),vertices.data(),sizeof(Vertex));
                if(FAILED(hr)) ++rejected;
                d->EndScene();
            } else ++rejected;
        }
        d->SetDepthStencilSurface(nullptr);d->SetRenderTarget(0,rt.Get());d->SetDepthStencilSurface(ds.Get());
        state->Apply();d->SetViewport(&vp);
    }
    void diagnostics() {
        // Native visibility inputs, logged for the culling study (read only).
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
    }
    void present(IDirect3DDevice9* d) {
        {static uint32_t pf=0;probeFrame=(++pf%150)==0;}
        profiler.start(GetCurrentThreadId());profiler.report();profiler.lastPresentTick=GetTickCount64();profiler.stallLogged=false;
        NonBlockingPresent::install(d);
        textureQuality.enforce();
        bool f12=(GetAsyncKeyState(VK_F12)&0x8000)!=0;
        if(f12&&!f12Down) {
            bypass=!bypass;log("VR pause (F12): %s",bypass?"ON - the mod does nothing, measuring the game alone":"OFF");
            if(bypass&&slot>=0&&ipc.data) {InterlockedExchange(&ipc.data->slots[slot].state,0);slot=-1;}
            if(bypass) {dropPending();if(vr) vr->active=false;pendingPackage.reset();}
            // 0.6.9.5: the pause also gives the game back its normal camera focal (0.6.9.4: focal left at x0.33 while paused -> crash on resume).
            if(bypass) {LensHack::want=1.f;pickWant=1.f;pickFits=false;ViewExpand::rectValid=0;}
            clearBatch();frameOpen=false;
        }
        f12Down=f12;
        double presentStarted=milliseconds();
        struct PresentTimer {double start;double& total;~PresentTimer(){total+=milliseconds()-start;}} presentTimer{presentStarted,presentMilliseconds};
        if(ipc.data) mouse.pointer(pointerRequest());
        mouse.tick(d);
        if(bypass) {
            frameOpen=false;
            ++statsFrames;
            if(GetTickCount64()-lastStatus>5000) {
                double elapsed=milliseconds()-statsStarted;
                log("Timing (VR PAUSED): game=%.1f fps (%.1f ms/frame)",1000*statsFrames/std::max(1.,elapsed),elapsed/std::max<uint64_t>(1,statsFrames));
                if(!native.frontend()) gameCounters();
                statsFrames=0;statsStarted=milliseconds();lastStatus=GetTickCount64();
            }
            return;
        }
        if(wheel.zoomRel&&!wheel.window) {D3DDEVICE_CREATION_PARAMETERS cp{};if(SUCCEEDED(d->GetCreationParameters(&cp))) wheel.attach(cp.hFocusWindow);}
        zoom.tick();
        topDown.apply(native,lastAnchored&&!possessedFrame&&!native.frontend());
        if(!start(d)) {publishPending(d,false);return;}
        struct CloseFrame {bool& open;~CloseFrame(){open=false;}} closeFrame{frameOpen};
        if(decoupledFrame) {presentDecoupled(d);return;}
        if(anchored) control.measure(nativeCameraInTable(table),anchored);
        control.tick(request,tableCamera,activeUnits,anchored,mode);
        lastAnchored=anchored;
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
            pending.world=world;pending.ui=ui;pending.rejected=rejected;pending.mode=mode;
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
        out.worldDraws=world;out.uiDraws=ui;out.rejectedDraws=rejected;out.cameraMode=mode;
        MemoryBarrier();InterlockedExchange(&out.state,2);SetEvent(ipc.ready);slot=-1;
        if(GetTickCount64()-lastStatus>5000) statusLog();
    }
    // 0.3.0: the game frame becomes a package for the VR thread; no eye image is drawn here.
    void presentDecoupled(IDirect3DDevice9* d) {
        publishPending(d,false); // a 0.2.9 image possibly still waiting from the previous frame
        // Pan state that produced THIS frame's native camera (the tick below moves the next frame's camera).
        const bool panning=control.panning;const int panHand=control.hand;const Vec3 panFrom=control.lastHand;
        const bool turning=control.rotating;const float turnFrom=control.lastAngle;
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
        lastAnchored=anchored;
        statsDraw+=drawMilliseconds;statsPresent+=presentMilliseconds;presentMilliseconds=0;++statsFrames;statsWorld+=world;
        if(!anchored||records.empty()) {
            clearBatch();++dropped;
            if(GetTickCount64()-lastStatus>5000) statusLog();
            return;
        }
        double syncStarted=milliseconds();
        auto p=std::make_shared<Package>();
        p->id=packageId;p->epoch=vr->epoch;p->recordedMs=milliseconds();
        p->vertices.swap(arena);p->indices.swap(indexArena);p->records.swap(records);
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
        p->clip=clipping();
        if(p->clip) {
            auto planes=table.planes();const Pose identity{{0,0,0,1},{0,0,0}};
            for(int i=0;i<4;++i) {auto q=transformPlane(planes[i],tableCamera,identity);float c[4]={q.normal.x,q.normal.y,q.normal.z,q.offset};memcpy(p->planes[i],c,sizeof(c));}
            // Ceiling above the dungeon walls (+0.4 game unit): interface pieces the game draws with a perspective
            // depth (info panel, icons) were reprojected high above the table (validated in the 0.4.9.2 test).
            p->ceiling=false;
            if(hideFloatingUi&&haveLevels&&activeUnits>0) {
                float h=dot(table.up,table.center)+(highOffset-(haveWalk?walkOffset:0.f)+.4f)/activeUnits;
                Plane top{scale(table.up,-1),h};auto q=transformPlane(top,tableCamera,identity);
                float c[4]={q.normal.x,q.normal.y,q.normal.z,q.offset};memcpy(p->planes[4],c,sizeof(c));p->ceiling=true;
            }
        }
        p->table=request.table;p->tableWidth=request.tableWidthMeters;p->tableDepth=table.halfDepth*2;
        p->panning=panning;p->panHand=panHand;p->panFrom=panFrom;
        p->turning=turning;p->turnFrom=turnFrom;p->pivot=pivot;
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
        if(uiCleared&&uiSurf) {
            // The cursor is shown on the controller: remove it from the panels (what lay under it is put back).
            if(cursorFound&&cursorBacked&&hudErase) {
                RECT src{0,0,cursorSaved.right-cursorSaved.left,cursorSaved.bottom-cursorSaved.top};
                d->StretchRect(cursorBackup.Get(),&src,uiSurf.Get(),&cursorSaved,D3DTEXF_NONE);
            }
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
            if(heldWindow&&keeperHeld&&cursorKnown&&lastPointerOnTable&&!(screenActive&&screenWhole)) { // 0.6.9.10: held things also while a message is shown
                bool onPanel=false;float r[4][4];
                // Real bar outlines (measured 0.6.0/0.6.3), tighter than the HUD regions kept for tooltips.
                static constexpr HudRegion bars[4]={{0.f,0.f,1.f,.09f},{0.f,.693f,.73f,.760f},{0.f,.760f,.178f,1.f},{.182f,.760f,1.f,1.f}};
                const float iw=std::min(float(uiW),uiH*4.f/3.f),ih=std::min(float(uiH),uiW*3.f/4.f),ox=(uiW-iw)*.5f,oy=(uiH-ih)*.5f;
                for(int i=0;i<4;++i) {r[i][0]=ox+bars[i].u0*iw;r[i][1]=oy+bars[i].v0*ih;r[i][2]=ox+bars[i].u1*iw;r[i][3]=oy+bars[i].v1*ih;if(cursorPx>=r[i][0]&&cursorPx<=r[i][2]&&cursorPy>=r[i][1]&&cursorPy<=r[i][3]) onPanel=true;}
                if(!onPanel) {
                    if(!heldSurf&&(FAILED(d->CreateTexture(uiW,uiH,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&heldTex,nullptr))||FAILED(heldTex->GetSurfaceLevel(0,&heldSurf)))) {heldTex.Reset();heldSurf.Reset();}
                    if(heldSurf&&SUCCEEDED(d->StretchRect(uiSurf.Get(),nullptr,heldSurf.Get(),nullptr,D3DTEXF_NONE))) {
                        for(int i=0;i<4;++i) {RECT e{LONG(std::floor(r[i][0])),LONG(std::floor(r[i][1])),LONG(std::ceil(r[i][2])),LONG(std::ceil(r[i][3]))};
                            e.left=std::max<LONG>(0,e.left);e.top=std::max<LONG>(0,e.top);e.right=std::min<LONG>(LONG(uiW),e.right);e.bottom=std::min<LONG>(LONG(uiH),e.bottom);
                            // 0.7.0: the bars are erased EXCEPT under the held model (0.6.9.15: a creature over a bar was cut).
                            RECT k{LONG(heldKeep[0])-12,LONG(heldKeep[1])-12,LONG(heldKeep[2])+12,LONG(heldKeep[3])+12};
                            RECT parts[4]={e,e,e,e};int np=1;
                            if(false&&heldKeep[2]>heldKeep[0]&&k.left<e.right&&k.right>e.left&&k.top<e.bottom&&k.bottom>e.top) {
                                np=0;
                                if(k.top>e.top) parts[np++]=RECT{e.left,e.top,e.right,k.top};
                                if(k.bottom<e.bottom) parts[np++]=RECT{e.left,k.bottom,e.right,e.bottom};
                                const LONG t0=std::max(e.top,k.top),t1=std::min(e.bottom,k.bottom);
                                if(k.left>e.left) parts[np++]=RECT{e.left,t0,k.left,t1};
                                if(k.right<e.right) parts[np++]=RECT{k.right,t0,e.right,t1};
                            }
                            for(int q=0;q<np;++q) if(parts[q].right>parts[q].left&&parts[q].bottom>parts[q].top) d->ColorFill(heldSurf.Get(),&parts[q],0x00000000);}
                        // (0.6.9.9-0.6.9.10 erased the message box here; 0.6.9.11: the box is no longer in the interface texture.)
                        heldCleared=true;heldPx=cursorPx;heldPy=cursorPy;
                    }
                }
            }
            p->pickDistance=lastPickDist;
            {const float iw=std::min(float(uiW),uiH*4.f/3.f),ih=std::min(float(uiH),uiW*.75f);
             {const float kw=screenActive?.30f:.40f,kh=screenActive?.20f:.30f;const bool big=midX1-midX0>kw*iw&&midY1-midY0>kh*ih;
              // 0.6.9.7: with the pause watch, the panel is shown exactly while the game is paused (options screen).
              if(PauseWatch::paused>=0) screenHold=PauseWatch::paused?30:0;
              else if(big) screenHold=30; else if(screenHold>0) --screenHold;
              // 0.6.9.8: game messages (notifications to validate, objectives page) shown above the bars while not paused.
              if(msgX1-msgX0>=.30f*uiW&&msgY1-msgY0>=.10f*uiH) msgFrame=true;
              if(msgFrame) msgHold=20; else if(msgHold>0) --msgHold;
              {// 0.6.9.11: panel rectangle = message box (or objectives page union) of this frame, with a small margin.
               float a0=std::min(boxX0,msgX0),b0=std::min(boxY0,msgY0),a1=std::max(boxX1,msgX1),b1=std::max(boxY1,msgY1);
               if(msgFrame&&a1>a0&&b1>b0) {const float iw=std::min(float(uiW),uiH*4.f/3.f),ih=std::min(float(uiH),uiW*.75f),ox=(uiW-iw)*.5f,oy=(uiH-ih)*.5f;
                   msgRegion={std::clamp((a0-ox)/iw-.01f,0.f,1.f),std::clamp((b0-oy)/ih-.01f,.09f,.715f),std::clamp((a1-ox)/iw+.01f,0.f,1.f),std::clamp((b1-oy)/ih+.01f,.09f,.715f)};}}
              msgFrame=false;msgShown=msgHold>0;msgX0=msgY0=1e9f;msgX1=msgY1=-1e9f;heldModelSeen=false;heldKeep[0]=heldKeep[1]=1e9f;heldKeep[2]=heldKeep[3]=-1e9f;prevBox[0]=boxX0;prevBox[1]=boxY0;prevBox[2]=boxX1;prevBox[3]=boxY1;boxX0=boxY0=1e9f;boxX1=boxY1=-1e9f;
              screenWhole=screenHold>0;}
             screenActive=screenHold>0||msgHold>0;if(screenActive) ++statScreenFrames;
             midX0=midY0=1e9f;midX1=midY1=-1e9f;}
            p->screenShown=screenActive;p->screenWhole=screenWhole;p->msgRegion=msgRegion;
            if(screenActive&&screenCleared&&screenSurf) {p->screen=copies.desktop(d,screenSurf.Get(),uiW,uiH);} else p->screen=0;p->pedestalSide=pedestalSide;p->buttonHover=buttonHover;
            if(heldCleared&&heldSurf) {
                p->held=copies.desktop(d,heldSurf.Get(),uiW,uiH);
                if(auto* c=copies.find(p->held)) {p->heldHandle=c->handle;p->heldPx=heldPx;p->heldPy=heldPy;p->heldRadius=heldRadius*uiW/1024.f;++statHeldFrames;} else p->held=0;
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
        uploadKeeper(d,*p);
        finishPick();pickActive=false;screenCleared=false;
        uiCleared=false;cursorFound=false;cursorBacked=false;heldCleared=false;holdingThings=false;heldModel=false;
        clearBatch();
        pollPackage(true); // previous game frame: normally already finished on the GPU
        auto& q=packageFences[p->id&1];
        HRESULT hr=q?S_OK:d->CreateQuery(D3DQUERYTYPE_EVENT,&q);
        if(SUCCEEDED(hr)) hr=q->Issue(D3DISSUE_END);
        if(SUCCEEDED(hr)) {q->GetData(nullptr,0,D3DGETDATA_FLUSH);pendingPackage=std::move(p);++statPackages;}
        else {log("Package fence failed %08lX",(unsigned long)hr);++dropped;}
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
        if(statHudFrames||statUiDraws) {
            log("HUD: frames=%llu interface draws=%llu cursor found in %llu frames (last %.0fx%.0f px, hotspot %.2f,%.2f at %.0f,%.0f) laser hits=%llu clicks=%llu last point=%.3f,%.3f top=%.2f m",
                (unsigned long long)statHudFrames,(unsigned long long)statUiDraws,(unsigned long long)statCursorFrames,cursorW,cursorH,cursorHotX,cursorHotY,cursorPx,cursorPy,
                (unsigned long long)statHudHits,(unsigned long long)statHudClicks,lastPointerX,lastPointerY,lastHudTop);
            log("Keeper hand (software cursor): %llu images set, %llu captured, %llu failures, last %dx%d hotspot %d,%d, %u bpp, key %08X, %u opaque pixels",
                (unsigned long long)keeper.calls,(unsigned long long)keeper.captures,(unsigned long long)keeper.failures,keeper.w,keeper.h,keeper.hotX,keeper.hotY,keeper.lastBits,keeper.lastKey,keeper.lastOpaque);
            keeper.calls=keeper.captures=0;
            log("Wall screen (options/menus) shown in %llu frames (game paused flag %ld, changes %ld)",(unsigned long long)statScreenFrames,long(PauseWatch::paused),long(PauseWatch::changes));statScreenFrames=0;
            log("Table picking: game camera focal x%.2f (wanted x%.2f), projection setups %ld",float(LensHack::applied),float(LensHack::want),long(InterlockedExchange(&LensHack::calls,0)));
            log("Table pointer: %llu frames on the table (%llu on dungeon geometry, %llu on the flat table), %llu clicks, last point %.3f,%.3f",(unsigned long long)statTableHits,(unsigned long long)statPickHits,(unsigned long long)statPlaneHits,(unsigned long long)statTableClicks,lastTableX,lastTableY);statTableHits=statTableClicks=statPickHits=statPlaneHits=0;
            log("Interface in 3D over the bars (Ui3DFilter=%d): %llu draws moved to the HUD",int(ui3dFilter),(unsigned long long)statUi3d);statUi3d=0;
            log("Interface cards kept out of the dungeon: %llu draws (%zu interface textures)",(unsigned long long)statUiIn3d,uiTextures.size());statUiIn3d=0;uiTextures.clear();
            log("Held things (window=%d): %llu/%llu frames, %llu pieces moved to the hand, rejected %llu far from cursor / %llu in a panel, game hand count>0 in %llu frames",int(heldWindow),(unsigned long long)statHeldFrames,(unsigned long long)statUiFrames,(unsigned long long)statHeldDraws,(unsigned long long)statHeldFar,(unsigned long long)statHeldPanel,(unsigned long long)statHoldCount);statHeldFrames=statHeldDraws=statHeldFar=statHeldPanel=statHoldCount=statUiFrames=0;
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
            log("Timing: game=%.1f fps (%.1f ms/frame), VR record+overlay CPU=%.2f ms, stereo replay=%.2f ms, GPU wait=%.2f ms, draws=%.0f/frame, vertices=%.0f/frame, rectangular=%u, anchored=%u, batched=%u, units/m=%.2f",
                1000*statsFrames/std::max(1.,elapsed),elapsed/frames,statsDraw/frames,statsReplay/frames,statsSync/frames,double(statsWorld)/frames,double(statsVertices)/frames,table.valid,anchored,batched,activeUnits);
            log("Timing detail: mod Present total=%.2f ms/frame (includes replay, overlay, copy, GPU wait)",statsPresent/frames);
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
        }
    }
};
Renderer& renderer() {static Renderer* r=new Renderer;return *r;}
}
extern "C" __declspec(dllexport) void __cdecl DK2VR_Draw(IDirect3DDevice9* d,D3DPRIMITIVETYPE t,UINT n,const void* v,UINT stride,const void* i,D3DFORMAT f,UINT min,UINT num) {
    try {renderer().draw(d,t,n,v,stride,i,f,min,num);} catch(...) {log("Draw exception; frame skipped");}
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
