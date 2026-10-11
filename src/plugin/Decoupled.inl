// DK2VR 0.3.0 - decoupled VR rendering (included by Plugin.cpp inside its anonymous namespace).
//
// The game thread (device A, the game's own D3D9Ex device) records each game frame as before, but no
// longer draws the two eyes. It hands a "package" (vertices in record-LOCAL space, draw states,
// texture copies) to a VR thread that owns a second D3D9Ex device (device B) on the same GPU.
// The VR thread redraws the newest package for every headset frame with the newest head pose, table
// pose and hands. Game textures are copied on device A into shared render-target textures that device
// B opens. A copy is refreshed when the game changes its texture (vtable hooks on the unlock/update
// paths) and a copy still possibly read by the VR thread is never overwritten (a new one is used).

// 0.8.54 DIAGNOSTIC ONLY (ProbeAuto=1 in LANCER_TEST_ISOLATION.cmd; normal launch = mode 0 = identical to 0.8.41).
// Probe modes, advanced automatically by the game thread every 3 timing windows, to isolate what slows the game:
// 0 normal, 1 headset antialiasing off, 2 room/table decor off, 3 both off, 4 headset images capped at 45/s, 5 capped at 30/s.
// None of them touches the game's own rendering quality.
std::atomic<int> probeMode{0};
// True GPU duration of each headset frame (D3D9 timestamp queries, read back without waiting, ring of 6).
struct GpuTimer {
    struct Slot {ComPtr<IDirect3DQuery9> dis,start,end,freq;bool pending=false;};
    Slot s[6];int next=0;bool ok=false;
    double total=0,maximum=0;uint64_t count=0,disjoint=0;
    bool init(IDirect3DDevice9* d) {
        if(kPublicBuild) return ok=false;   // 0.9.47.60: no GPU timestamp queries in the public build
        for(auto& x:s) {
            if(FAILED(d->CreateQuery(D3DQUERYTYPE_TIMESTAMPDISJOINT,&x.dis))||FAILED(d->CreateQuery(D3DQUERYTYPE_TIMESTAMP,&x.start))||
               FAILED(d->CreateQuery(D3DQUERYTYPE_TIMESTAMP,&x.end))||FAILED(d->CreateQuery(D3DQUERYTYPE_TIMESTAMPFREQ,&x.freq))) return ok=false;
        }
        return ok=true;
    }
    void begin() {if(!ok) return;Slot& x=s[next];if(x.pending) return;x.dis->Issue(D3DISSUE_BEGIN);x.start->Issue(D3DISSUE_END);}
    void end() {
        if(!ok) return;Slot& x=s[next];if(x.pending) return;
        x.end->Issue(D3DISSUE_END);x.dis->Issue(D3DISSUE_END);x.freq->Issue(D3DISSUE_END);x.pending=true;next=(next+1)%6;
    }
    void collect() {
        if(!ok) return;
        for(auto& x:s) if(x.pending) {
            BOOL dj=FALSE;UINT64 a=0,b=0,f=0;
            if(x.dis->GetData(&dj,sizeof(dj),0)!=S_OK||x.start->GetData(&a,sizeof(a),0)!=S_OK||x.end->GetData(&b,sizeof(b),0)!=S_OK||x.freq->GetData(&f,sizeof(f),0)!=S_OK) continue;
            x.pending=false;
            if(dj||!f||b<a) {++disjoint;continue;}
            double ms=1000.*double(b-a)/double(f);total+=ms;maximum=std::max(maximum,ms);++count;
            {auto& S=perfab::shared();const uint64_t us=uint64_t(ms*1000.);S.vrWindow.gpu(us);if(!S.settling.load()) S.vr[std::clamp(S.mode.load(),0,perfab::Modes-1)][std::clamp(S.config.load(),0,perfab::Configs-1)].gpu(us);}   // 0.9.25 PERF
        }
    }
};
std::atomic<uint64_t> sharedFrames{0};      // slot frame numbers: game-thread and VR-thread images share one sequence
std::atomic<uint32_t> sharedGenerations{0}; // slot texture generations (the host reopens handles when it changes)
constexpr uint64_t NoPackage=~0ull;

// 0.8.90.7: menu interface separation, exact. In the frontend the wrapper reads the game's 640x480 render target back into a
// system-memory surface right after the 3D draws (GetRenderTargetData + READ-ONLY LockRect, measured: 1 per frame, wrapper+0x121317,
// "EndScene ... GetRenderTargetData ON THIS SURFACE"), and only then does the game draw its 2D interface in software. That read-back
// is therefore the decor ALONE, pixel exact. The final image (decor + interface) is the presented screen, a software x3 stretch of the
// same 640x480 image: comparing both at the centre of each 3x3 block gives the interface with no guess at all.
namespace MenuTap {
    inline std::atomic<bool> armed{false};
    inline bool fresh=false;inline UINT w=0,h=0;inline std::vector<uint32_t> pixels;
    inline uint64_t captures=0;inline bool logged=false;
    inline std::shared_ptr<const std::vector<uint32_t>> ring[3]; // 0.8.90.8: decor of this frame, of the previous one, of the one before
    inline void clearRing() {for(auto& r:ring) r.reset();}
    // 0.8.90.9 diagnostic (F11 in the menu): the 640x480 system-memory surfaces the game WRITES during one frame are copied at their unlock,
    // with the decor read-back and the presented screen, into Captures\DK2VR_calque_*.bmp (collected by the report).
    inline int dumpStage=0;
    struct WriteCap {IDirect3DSurface9* s=nullptr;const unsigned char* base=nullptr;INT pitch=0;UINT w=0,h=0,locks=0;std::vector<uint32_t> px;bool done=false;};
    inline WriteCap wcap[3];
    inline void resetWrites() {for(auto& c:wcap) {c.s=nullptr;c.base=nullptr;c.px.clear();c.done=false;c.locks=0;}}
    inline void noteWriteLock(IDirect3DSurface9* s,const D3DLOCKED_RECT* lr,const RECT* rect,DWORD flags) {
        if((flags&D3DLOCK_READONLY)||!lr||!lr->pBits||lr->Pitch<=0) return;
        D3DSURFACE_DESC desc{};if(FAILED(s->GetDesc(&desc))) return;
        if(desc.Pool!=D3DPOOL_SYSTEMMEM||(desc.Format!=D3DFMT_X8R8G8B8&&desc.Format!=D3DFMT_A8R8G8B8)) return;
        if(desc.Width<320||desc.Width>2048||desc.Width*3!=desc.Height*4) return;
        const unsigned char* base=static_cast<const unsigned char*>(lr->pBits);
        if(rect) base-=size_t(rect->top)*size_t(lr->Pitch)+size_t(rect->left)*4;
        for(auto& c:wcap) if(c.s==s||!c.s) {c.s=s;c.base=base;c.pitch=lr->Pitch;c.w=desc.Width;c.h=desc.Height;++c.locks;return;}
    }
    inline void noteUnlock(IDirect3DSurface9* s) {
        for(auto& c:wcap) if(c.s==s&&c.base) {
            c.px.resize(size_t(c.w)*c.h);
            for(UINT y=0;y<c.h;++y) memcpy(c.px.data()+size_t(y)*c.w,c.base+size_t(y)*size_t(c.pitch),size_t(c.w)*4);
            c.done=true;c.base=nullptr;return;
        }
    }
    // 0.8.95: the decor read-back is the destination of the game thread's last GetRenderTargetData (set by TimedCalls::hRtData). The wrapper
    // also locks other 640x480 system surfaces read-only (presentation copies, log 20261005_212232 'Lock detail'): those are not the decor.
    inline IDirect3DSurface9* rtDest=nullptr;
    // 0.8.95: FINAL image of the frame (decor + the game's 2D interface) = content, at its unlock, of the last 640x480 system surface written WHOLE
    // (write lock of the full surface) after the decor read-back. 0.8.91 (F11 diagnostic): the presented screen equals such a written surface
    // (100 % identical). Same frame, same 640x480 grid as the decor: the 2D / decor separation (MenuSplit) needs no stretch, no frame lag.
    inline std::atomic<bool> finalArmed{false};
    struct FinalLock {IDirect3DSurface9* s=nullptr;const unsigned char* base=nullptr;INT pitch=0;UINT w=0,h=0;};
    inline FinalLock finalLocks[4];
    inline std::vector<uint32_t> finalPx;inline bool finalFresh=false,finalAfterDecor=false;inline IDirect3DSurface9* finalSurf=nullptr;inline UINT finalW=0,finalH=0;
    inline uint32_t finalWrites=0,finalWritesBeforeDecor=0;inline uint64_t finalCaptures=0;
    inline bool menuSurface(IDirect3DSurface9* s,D3DSURFACE_DESC& desc) {
        if(FAILED(s->GetDesc(&desc))) return false;
        if(desc.Pool!=D3DPOOL_SYSTEMMEM||(desc.Format!=D3DFMT_X8R8G8B8&&desc.Format!=D3DFMT_A8R8G8B8)) return false;
        return desc.Width>=320&&desc.Width<=2048&&desc.Width*3==desc.Height*4;
    }
    inline void noteFinalLock(IDirect3DSurface9* s,const D3DLOCKED_RECT* lr,const RECT* rect,DWORD flags) {
        if((flags&D3DLOCK_READONLY)||!lr||!lr->pBits||lr->Pitch<=0||s==rtDest) return;
        D3DSURFACE_DESC desc{};if(!menuSurface(s,desc)) return;
        if(rect&&(rect->left!=0||rect->top!=0||UINT(rect->right)!=desc.Width||UINT(rect->bottom)!=desc.Height)) return;   // whole surface only
        FinalLock* slot=nullptr;for(auto& c:finalLocks) if(c.s==s) {slot=&c;break;}
        if(!slot) for(auto& c:finalLocks) if(!c.s) {slot=&c;break;}
        if(!slot) return;
        slot->s=s;slot->base=static_cast<const unsigned char*>(lr->pBits);slot->pitch=lr->Pitch;slot->w=desc.Width;slot->h=desc.Height;
    }
    // Several whole 640x480 surfaces are written in one frame (log 212232: two by the wrapper, plus the presented one). The final image is
    // the most complete composite: of the surfaces written after the decor read-back, the one that differs from the decor in the most
    // pixels (sparse grid, 1 pixel in 4 x 4; a copy of the decor alone differs in none; equal scores: the later one).
    inline uint32_t finalScore=0;
    inline uint32_t sparseDiff(const unsigned char* base,INT pitch,UINT cw,UINT ch) {
        if(cw!=w||ch!=h||pixels.size()!=size_t(w)*h) return 0;
        uint32_t n=0;
        for(UINT y=1;y<ch;y+=4) {const uint32_t* row=reinterpret_cast<const uint32_t*>(base+size_t(y)*size_t(pitch));const uint32_t* dec=pixels.data()+size_t(y)*w;
            for(UINT x=1;x<cw;x+=4) {const uint32_t a=row[x],b=dec[x];
                const int dr=std::abs(int((a>>16)&255)-int((b>>16)&255)),dg=std::abs(int((a>>8)&255)-int((b>>8)&255)),db=std::abs(int(a&255)-int(b&255));
                if(std::max(dr,std::max(dg,db))>12) ++n;}}
        return n;
    }
    inline void noteFinalUnlock(IDirect3DSurface9* s,bool decorTaken) {
        for(auto& c:finalLocks) if(c.s==s) {
            if(c.base) {
                if(decorTaken) ++finalWrites; else ++finalWritesBeforeDecor;
                if(w&&(c.w!=w||c.h!=h)) {c=FinalLock{};return;}   // review 0.8.95: only the decor's own grid (a stretched copy of another size never wins)
                const uint32_t score=decorTaken?sparseDiff(c.base,c.pitch,c.w,c.h):0;
                // before the read-back: kept only while nothing better exists (diagnostic counters say how often); after it: the most complete composite wins
                if(!finalFresh||(decorTaken&&(!finalAfterDecor||score>=finalScore))) {
                    finalPx.resize(size_t(c.w)*c.h);
                    for(UINT y=0;y<c.h;++y) memcpy(finalPx.data()+size_t(y)*c.w,c.base+size_t(y)*size_t(c.pitch),size_t(c.w)*4);
                    finalFresh=true;finalAfterDecor=decorTaken;finalSurf=s;finalW=c.w;finalH=c.h;finalScore=score;++finalCaptures;
                }
            }
            c=FinalLock{};return;
        }
    }
    inline void clearFinal() {for(auto& c:finalLocks) c=FinalLock{};finalFresh=false;finalAfterDecor=false;finalScore=0;}
    inline void take(IDirect3DSurface9* s,const D3DLOCKED_RECT* lr,const RECT* rect,DWORD flags) {
        if(!(flags&D3DLOCK_READONLY)||!lr||!lr->pBits||lr->Pitch<=0) return;
        if(rtDest&&s!=rtDest) return;   // 0.8.95
        D3DSURFACE_DESC desc{};if(FAILED(s->GetDesc(&desc))) return;
        if(desc.Pool!=D3DPOOL_SYSTEMMEM||(desc.Format!=D3DFMT_X8R8G8B8&&desc.Format!=D3DFMT_A8R8G8B8)) return;
        if(desc.Width<320||desc.Width>2048||desc.Width*3!=desc.Height*4) return;
        if(rect&&(rect->left!=0||rect->top!=0||UINT(rect->right)!=desc.Width||UINT(rect->bottom)!=desc.Height)) return;
        w=desc.Width;h=desc.Height;pixels.resize(size_t(w)*h);
        const unsigned char* src=static_cast<const unsigned char*>(lr->pBits);
        for(UINT y=0;y<h;++y) memcpy(pixels.data()+size_t(y)*w,src+size_t(y)*size_t(lr->Pitch),size_t(w)*4);
        fresh=true;armed.store(false,std::memory_order_relaxed);++captures;
        if(!logged) {logged=true;log("Menu interface separation (0.8.90.7): decor captured from the game's own read-back %ux%u (format %u) before the 2D interface",w,h,unsigned(desc.Format));}
    }
}

// ---------------------------------------------------------------------------------------------
// Texture modification tracking. Game textures are D3DPOOL_DEFAULT|DYNAMIC (dxwrapper D3d9to9Ex)
// and are filled through IDirect3DTexture9::LockRect/UnlockRect (profiled at wrapper RVA 0x1213A9),
// possibly through surface locks, UpdateSurface/UpdateTexture, StretchRect, ColorFill or rendering.
// Every one of these paths increments the version of the texture if it is watched.
// Vtable slot patches (one entry per distinct vtable, original kept for the hook).
#include "PatchSet.hpp"
// 0.5.3: CPU shadow of the game's textures. dxwrapper (D3d9to9Ex) turns the game's MANAGED textures into
// DEFAULT|DYNAMIC ones; every LockRect without DISCARD then waits until the GPU has finished with the texture
// (lockBuf 0x590AC0 -> wrapper LockRect RVA 0x1213A9: 55-70 % of the game thread in the 0.5.2 profile,
// game ~40 fps even with the VR paused). Each texture gets a SYSTEMMEM twin: the game locks the twin (no wait)
// and on UnlockRect the changes are sent with UpdateTexture (queued on the GPU, no wait). The twin is filled
// once from the texture (one wait) and dropped when the texture is written by another path.
static const GUID ShadowTag={0x6b1d2a31,0x5c1e,0x4b8e,{0x9a,0x21,0x3f,0x44,0x10,0x7d,0xe2,0x55}};
struct TextureShadow {
    ComPtr<IDirect3DTexture9> sys;UINT w=0,h=0;D3DFORMAT format=D3DFMT_UNKNOWN;bool locked=false,readOnly=false,valid=false;DWORD lockedAt=0;
};
#include "SurfaceShadow.hpp"
// 0.8.60 (measurement only): duration of the main Direct3D 9 device calls of the game thread. Each hook calls the original
// unchanged; the vtable is shared with the VR thread's device, so the timing only counts the game thread (CallTiming::Scope).
struct TimedCalls {
    static inline PatchSet beginScene,endScene,clearCall,setTexture,drawPrim,drawIdx,drawPrimUP,drawIdxUP,rtData;
    static inline bool installed=false;
    // 0.8.61: which device vtable slots we patched, to check that nobody replaced them and to see which module the vtable belongs to.
    struct Registered {const char* name;int index;void* hook;PatchSet* set;};
    static inline std::vector<Registered> registered;static inline void** deviceVtable=nullptr;
    static inline unsigned heals=0,healEvents=0,healLogs=0;static inline bool deviceHooksOn=true;
    static void note(const char* name,int index,void* hook,PatchSet* set) {registered.push_back({name,index,hook,set});}
    static std::string moduleOf(void* address) {
        HMODULE m=nullptr;char path[MAX_PATH]{};
        if(GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(address),&m)&&m) GetModuleFileNameA(m,path,MAX_PATH);
        const char* slash=strrchr(path,'\\');return slash?slash+1:path;
    }
    // 0.8.62: every game frame, 14 pointer comparisons. The D3D9 runtime refilled the shared device vtable when the VR thread's device was created
    // (0.8.61 report: all 14 slots back to d3d9.dll, so every device hook was silent from then on). Repaired slots chain to what they now contain.
    static void heal(IDirect3DDevice9* d) {
        if(!deviceHooksOn||registered.empty()||!d) return;
        void** vt=*reinterpret_cast<void***>(d);
        if(vt!=deviceVtable) {
            // The device object now uses another table (device recreated): hook that one too (PatchSet keeps one entry per table).
            bool ok=true;for(auto& r:registered) ok=r.set->install(d,r.index,r.hook)&&ok;
            ++healEvents;if(healLogs<8) {++healLogs;log("Device hooks: the runtime device now uses vtable %p (was %p): hooks installed there (%s)",static_cast<void*>(vt),static_cast<void*>(deviceVtable),ok?"ok":"some failed");}
            deviceVtable=vt;return;
        }
        std::string names;unsigned n=0;
        for(auto& r:registered) {
            void* previous=nullptr;
            if(r.set->refresh(d,r.index,r.hook,&previous)) {++n;if(names.size()<300) {char b[96];snprintf(b,sizeof(b)," %s(was %s)",r.name,moduleOf(previous).c_str());names+=b;}}
        }
        if(n) {heals+=n;++healEvents;if(healLogs<8) {++healLogs;log("Device hooks: %u slot(s) rewritten by the runtime, hooks put back:%s",n,names.c_str());}}
    }
    static void integrityReport(bool always) {
        if(!deviceVtable) return;
        std::string bad;char b[200];int n=0;
        for(auto& r:registered) {
            void* now=deviceVtable[r.index];++n;
            if(now!=r.hook) {snprintf(b,sizeof(b)," | slot %d %s now %p (%s)",r.index,r.name,now,moduleOf(now).c_str());bad+=b;}
        }
        if(!bad.empty()) log("Device hook integrity: %zu slots watched, REPLACED:%s",registered.size(),bad.c_str());
        else if(always) log("Device hook integrity: %zu slots patched at vtable %p (%s), all still ours",registered.size(),static_cast<void*>(deviceVtable),moduleOf(deviceVtable).c_str());
        if(healEvents) log("Device hooks repaired %u time(s), %u slot(s) in total since the start",healEvents,heals);
    }
    static HRESULT __stdcall hBegin(IDirect3DDevice9* d) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*)>(beginScene.original(d));CallTiming::Scope t(CallTiming::BeginScene,nullptr,nullptr,reinterpret_cast<uintptr_t>(__builtin_return_address(0)));return f?f(d):E_FAIL;
    }
    static HRESULT __stdcall hEnd(IDirect3DDevice9* d) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*)>(endScene.original(d));CallTiming::Scope t(CallTiming::EndScene,nullptr,nullptr,reinterpret_cast<uintptr_t>(__builtin_return_address(0)));return f?f(d):E_FAIL;
    }
    static HRESULT __stdcall hClear(IDirect3DDevice9* d,DWORD n,const D3DRECT* r,DWORD fl,D3DCOLOR c,float z,DWORD st) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,DWORD,const D3DRECT*,DWORD,D3DCOLOR,float,DWORD)>(clearCall.original(d));CallTiming::Scope t(CallTiming::Clear,nullptr,nullptr,reinterpret_cast<uintptr_t>(__builtin_return_address(0)));return f?f(d,n,r,fl,c,z,st):E_FAIL;
    }
    static HRESULT __stdcall hSetTexture(IDirect3DDevice9* d,DWORD s,IDirect3DBaseTexture9* tex) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,DWORD,IDirect3DBaseTexture9*)>(setTexture.original(d));CallTiming::Scope t(CallTiming::SetTexture);return f?f(d,s,tex):E_FAIL;
    }
    static HRESULT __stdcall hDraw(IDirect3DDevice9* d,D3DPRIMITIVETYPE pt,UINT s,UINT n) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,D3DPRIMITIVETYPE,UINT,UINT)>(drawPrim.original(d));CallTiming::Scope t(CallTiming::DrawPrimitive);return f?f(d,pt,s,n):E_FAIL;
    }
    static HRESULT __stdcall hDrawIdx(IDirect3DDevice9* d,D3DPRIMITIVETYPE pt,INT b,UINT mn,UINT nv,UINT si,UINT pc) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,D3DPRIMITIVETYPE,INT,UINT,UINT,UINT,UINT)>(drawIdx.original(d));CallTiming::Scope t(CallTiming::DrawIndexed);return f?f(d,pt,b,mn,nv,si,pc):E_FAIL;
    }
    // 0.9.47.10 NativeWorldSkip: the mod's draw hook (Renderer::draw) arms this gate for a dungeon draw that is HIDDEN in the game's image; the wrapper
    // then calls the device's DrawPrimitiveUP / DrawIndexedPrimitiveUP with the very same arguments and the hooks below drop that one call.
    static inline nativeskip::Gate gate;static inline bool skipWanted=false;static inline unsigned skipLogs=0;
    static bool skipReady(IDirect3DDevice9* d) {
        return skipWanted&&gate.active()&&deviceHooksOn&&deviceVtable&&d&&GetCurrentThreadId()==CallTiming::gameThread.load(std::memory_order_relaxed)&&*reinterpret_cast<void***>(d)==deviceVtable&&
            deviceVtable[83]==reinterpret_cast<void*>(&hDrawUP)&&deviceVtable[84]==reinterpret_cast<void*>(&hDrawIdxUP);
    }
    static HRESULT __stdcall hDrawUP(IDirect3DDevice9* d,D3DPRIMITIVETYPE pt,UINT n,const void* v,UINT st) {
        if(gate.isArmed&&GetCurrentThreadId()==CallTiming::gameThread.load(std::memory_order_relaxed)) {
            nativeskip::Call c;c.kind=0;c.type=uint32_t(pt);c.primitives=n;c.stride=st;c.verts=v;
            if(gate.take(c)) return D3D_OK;
        }
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,D3DPRIMITIVETYPE,UINT,const void*,UINT)>(drawPrimUP.original(d));CallTiming::Scope t(CallTiming::DrawPrimitiveUP);return f?f(d,pt,n,v,st):E_FAIL;
    }
    static HRESULT __stdcall hDrawIdxUP(IDirect3DDevice9* d,D3DPRIMITIVETYPE pt,UINT mn,UINT nv,UINT pc,const void* ix,D3DFORMAT fmt,const void* v,UINT st) {
        if(gate.isArmed&&GetCurrentThreadId()==CallTiming::gameThread.load(std::memory_order_relaxed)) {
            nativeskip::Call c;c.kind=1;c.type=uint32_t(pt);c.minIndex=mn;c.numVertices=nv;c.primitives=pc;c.indices=ix;c.indexFormat=uint32_t(fmt);c.verts=v;c.stride=st;
            if(gate.take(c)) return D3D_OK;
        }
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,D3DPRIMITIVETYPE,UINT,UINT,UINT,const void*,D3DFORMAT,const void*,UINT)>(drawIdxUP.original(d));CallTiming::Scope t(CallTiming::DrawIndexedUP);return f?f(d,pt,mn,nv,pc,ix,fmt,v,st):E_FAIL;
    }
    // 0.8.63: ReadbackLag (defined later) may answer the game thread's per-frame read-back itself; 0x7fffffff = not handled, the original runs.
    using ReadbackFn=HRESULT(*)(IDirect3DDevice9* device,IDirect3DSurface9* rt,IDirect3DSurface9* dest,void* original);
    static inline ReadbackFn readbackLag=nullptr;
    static HRESULT __stdcall hRtData(IDirect3DDevice9* d,IDirect3DSurface9* rt,IDirect3DSurface9* dest) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,IDirect3DSurface9*,IDirect3DSurface9*)>(rtData.original(d));
        CallTiming::Scope t(CallTiming::GetRenderTargetData,rt,dest,reinterpret_cast<uintptr_t>(__builtin_return_address(0)));
        const bool game=GetCurrentThreadId()==CallTiming::gameThread.load(std::memory_order_relaxed);
        if(LockDetail::enabled&&game) {GpuUse::note(rt);GpuUse::note(dest);}
        if(game) MenuTap::rtDest=dest;   // 0.8.95: the decor read-back surface
        if(f&&game&&readbackLag) {const HRESULT h=readbackLag(d,rt,dest,reinterpret_cast<void*>(f));if(h!=HRESULT(0x7fffffff)) return h;}
        return f?f(d,rt,dest):E_FAIL;
    }
    static void install(IDirect3DDevice9* d) {
        if(installed) return;installed=true;
        if(iniInt(L"CallTiming",1)==0) {log("Direct3D call timing: off (CallTiming=0)");return;}
        // IDirect3DDevice9 vtable: 32 GetRenderTargetData, 41 BeginScene, 42 EndScene, 43 Clear, 65 SetTexture, 81 DrawPrimitive,
        // 82 DrawIndexedPrimitive, 83 DrawPrimitiveUP, 84 DrawIndexedPrimitiveUP.
        const bool ok=rtData.install(d,32,reinterpret_cast<void*>(&hRtData))&&beginScene.install(d,41,reinterpret_cast<void*>(&hBegin))&&endScene.install(d,42,reinterpret_cast<void*>(&hEnd))&&
            clearCall.install(d,43,reinterpret_cast<void*>(&hClear))&&setTexture.install(d,65,reinterpret_cast<void*>(&hSetTexture))&&drawPrim.install(d,81,reinterpret_cast<void*>(&hDraw))&&
            drawIdx.install(d,82,reinterpret_cast<void*>(&hDrawIdx))&&drawPrimUP.install(d,83,reinterpret_cast<void*>(&hDrawUP))&&drawIdxUP.install(d,84,reinterpret_cast<void*>(&hDrawIdxUP));
        deviceVtable=*reinterpret_cast<void***>(d);
        note("GetRenderTargetData",32,reinterpret_cast<void*>(&hRtData),&rtData);note("BeginScene",41,reinterpret_cast<void*>(&hBegin),&beginScene);note("EndScene",42,reinterpret_cast<void*>(&hEnd),&endScene);
        note("Clear",43,reinterpret_cast<void*>(&hClear),&clearCall);note("SetTexture",65,reinterpret_cast<void*>(&hSetTexture),&setTexture);note("DrawPrimitive",81,reinterpret_cast<void*>(&hDraw),&drawPrim);
        note("DrawIndexedPrimitive",82,reinterpret_cast<void*>(&hDrawIdx),&drawIdx);note("DrawPrimitiveUP",83,reinterpret_cast<void*>(&hDrawUP),&drawPrimUP);note("DrawIndexedPrimitiveUP",84,reinterpret_cast<void*>(&hDrawIdxUP),&drawIdxUP);
        log("Direct3D call timing: %s (game thread, 1 frame in 4)",ok?"hooks installed":"some hooks unavailable");
    }
};
struct TextureWatch {
    static inline PatchSet texUnlock,texLock,surfLock,surfUnlock,updSurface,updTexture,stretch,fill,setTarget;
    static inline SurfaceShadows* surfaceShadows=nullptr;static inline bool surfaceShadowOn=true;
    static inline unsigned surfaceDescLogs=0,surfaceDescAttempts=0;
    static inline std::unordered_map<void*,TextureShadow>* shadows=nullptr;static inline bool shadowOn=true,shadowLogged=false;
    static inline volatile LONG shadowLocks=0,shadowFills=0,shadowFailures=0,directLocks=0;
    // 0.8.59: what each lock really costs (game thread). Path 1 = served from the system-memory twin, 2 = native lock.
    static inline volatile LONG64 lockTicks[2][4]{},lockCsTicks[2]{};static inline volatile LONG lockCount[2][4]{}; // path 3 (0.8.60) = served from the memory mirror
    static inline thread_local int lockPath=0,lockDepth=0;static inline thread_local long long lockCs=0; // depth: the shadow's own twin locks call the hooked vtable again (not counted)
    static inline thread_local void* lockGap=nullptr;  // 0.8.60: resource whose last GPU use explains a wait (the twin when served from it)
    static bool onGameThread() {return GetCurrentThreadId()==CallTiming::gameThread.load(std::memory_order_relaxed);}
    // 0.8.62: the device vtable is shared with the VR thread's device; its calls concern its own resources, never the game's textures.
    static bool notVrThread() {return GetCurrentThreadId()!=CallTiming::vrThread.load(std::memory_order_relaxed);}
    // 0.8.62: set by WrapperUpload (defined after this struct): copies a large system-memory source to a pooled surface before UpdateSurface.
    using SourceStageFn=HRESULT(*)(void* device,IDirect3DSurface9* src,const RECT* r,IDirect3DSurface9* dst,const POINT* p,void* original);
    static inline SourceStageFn sourceStage=nullptr;
    // 0.8.63: set by ReadbackLag (defined after this struct). restoreHook: the wrapper's UpdateSurface(shadow -> render target) at the start of a frame; with the
    // lag the shadow holds the PREVIOUS frame's image, so ReadbackLag sends only what the game changed in it (0x7fffffff = not handled).
    // lagWatch/lagDirty/lagWrites: the read-back surface the lag watches, whether the game locked it for writing since the last fill, and how often.
    using RestoreFn=HRESULT(*)(void* device,IDirect3DSurface9* src,const RECT* r,IDirect3DSurface9* dst,const POINT* p,void* original);
    static inline RestoreFn restoreHook=nullptr;
    static inline IDirect3DSurface9* volatile lagWatch=nullptr;static inline volatile LONG lagDirty=0,lagWrites=0;
    static long long qpc() {LARGE_INTEGER v;QueryPerformanceCounter(&v);return v.QuadPart;}
    static void lockDone(int kind,long long t0) {
        const long long dt=qpc()-t0;int p=lockPath>=1&&lockPath<=3?lockPath:0;
        if(CallTiming::paintActive) ++CallTiming::paintLocks;
        if(CallTiming::Evict::active()) {CallTiming::Evict::lockT[kind&1][p]+=dt;++CallTiming::Evict::lockN[kind&1][p];}   // 0.9.26 PERF
        InterlockedExchangeAdd64(&lockTicks[kind][p],dt);InterlockedIncrement(&lockCount[kind][p]);
        if(lockCs) InterlockedExchangeAdd64(&lockCsTicks[kind],lockCs);
        lockPath=0;lockCs=0;
    }
    static inline CRITICAL_SECTION lock;static inline bool ready=false,hooksOk=true;
    static inline std::unordered_map<void*,uint32_t>* versions=nullptr;
    static inline volatile LONG changes=0;
    static void init() {if(ready) return;InitializeCriticalSection(&lock);InitializeCriticalSection(&stagingLock);mirrors=new std::unordered_map<void*,TextureMirror>();mirrors->reserve(64);stagingOn=iniInt(L"TextureStaging",1)!=0;versions=new std::unordered_map<void*,uint32_t>();versions->reserve(4096);shadows=new std::unordered_map<void*,TextureShadow>();shadows->reserve(1024);surfaceShadows=new SurfaceShadows;shadowOn=iniInt(L"GameTextureShadow",1)!=0;surfaceShadowOn=iniInt(L"GameSurfaceShadow",1)!=0;ready=true;}
    static void touch(void* texture) {
        if(!ready||!texture) return;
        EnterCriticalSection(&lock);
        auto it=versions->find(texture);if(it!=versions->end()) {++it->second;InterlockedIncrement(&changes);}
        LeaveCriticalSection(&lock);
    }
    static void dropShadow(void* texture) {
        if(!ready||!texture) return;
        EnterCriticalSection(&lock);auto it=shadows->find(texture);if(it!=shadows->end()&&!it->second.locked) it->second.valid=false;
        if(mirrors) {auto mi=mirrors->find(texture);if(mi!=mirrors->end()&&!mi->second.locked) {if(mi->second.valid) InterlockedIncrement(&stDrops);mi->second.valid=false;}}
        LeaveCriticalSection(&lock);
    }
    static void invalidateTextureSurfaces(void* texture) {
        if(!ready||!texture)return;
        EnterCriticalSection(&lock);surfaceShadows->invalidateTexture(texture);LeaveCriticalSection(&lock);
    }
    static void touchSurface(IDirect3DSurface9* s,bool preserveSurfaceMirror=false) {
        if(!ready||!s) return;
        if(!preserveSurfaceMirror) {EnterCriticalSection(&lock);surfaceShadows->invalidate(s);LeaveCriticalSection(&lock);}
        void* container=nullptr;
        if(SUCCEEDED(s->GetContainer(__uuidof(IDirect3DTexture9),&container))&&container) {dropShadow(container);touch(container);static_cast<IUnknown*>(container)->Release();}
    }
    // Registers a texture and returns its current version.
    static uint32_t watch(void* texture) {
        EnterCriticalSection(&lock);
        uint32_t v=(*versions)[texture];
        LeaveCriticalSection(&lock);
        return v;
    }
    static void unwatch(void* texture) {EnterCriticalSection(&lock);versions->erase(texture);LeaveCriticalSection(&lock);}
    static void clear() {if(!ready) return;EnterCriticalSection(&lock);versions->clear();surfaceShadows->clear();clearMirrors();LeaveCriticalSection(&lock);}
    // Hooks (vtable slots: texture UnlockRect 20; surface UnlockRect 14; device UpdateSurface 30,
    // UpdateTexture 31, StretchRect 34, ColorFill 35, SetRenderTarget 37).
    using LockFn=HRESULT(__stdcall*)(IDirect3DTexture9*,UINT,D3DLOCKED_RECT*,const RECT*,DWORD);
    using UnlockFn=HRESULT(__stdcall*)(IDirect3DTexture9*,UINT);
    // Returns the shadow of a texture that can use one (level 0, single level, DEFAULT|DYNAMIC), filling it
    // from the texture the first time. Called under 'lock'.
    // 0.9.47.26: a texture whose twin cannot be made or filled is left to the native lock for 2 s (instead of retrying - and failing - at every
    // lock, thousands of times per second after a game load), and the first failures are described in the log with their HRESULT.
    // 0.9.47.27: stale 'locked' states (see SurfaceShadows::isLocked): counted per layer and the first ones described in the log.
    static inline volatile LONG staleMirror=0,staleTwin=0;static inline unsigned staleLogs=0;
    static void noteStale(const char* what,void* obj,bool sameObject,DWORD age) {
        if(staleLogs<16) {++staleLogs;log("Stale lock state discarded (0.9.47.27) #%u: %s %p (%s, lock open %lu ms)",staleLogs,what,obj,sameObject?"same object, open too long":"another object now lives at this address",(unsigned long)age);}
    }
    static bool tagIs(IDirect3DTexture9* t,const GUID& id,DWORD value) {DWORD tag=0,sz=sizeof(tag);return SUCCEEDED(t->GetPrivateData(id,&tag,&sz))&&sz==sizeof(tag)&&tag==value;}
    static inline std::unordered_map<void*,DWORD>* shadowBackoff=nullptr;static inline unsigned shadowFailLogs=0;
    static void shadowFail(IDirect3DTexture9* self,int site,HRESULT hr,const D3DSURFACE_DESC& d) {
        ++shadowFailures;shadows->erase(self);
        if(!shadowBackoff) shadowBackoff=new std::unordered_map<void*,DWORD>();
        if(shadowBackoff->size()>4096) shadowBackoff->clear();
        (*shadowBackoff)[self]=GetTickCount();
        if(shadowFailLogs<24) {++shadowFailLogs;log("Texture twin failure (0.9.47.26) #%u: site %d hr 0x%08lX texture %p %ux%u format %u pool %u usage 0x%lx (native lock used for 2 s)",shadowFailLogs,site,(unsigned long)hr,(void*)self,d.Width,d.Height,unsigned(d.Format),unsigned(d.Pool),(unsigned long)d.Usage);}
    }
    static TextureShadow* shadowFor(IDirect3DTexture9* self) {
        if(shadowBackoff&&!shadowBackoff->empty()) {auto bo=shadowBackoff->find(self);if(bo!=shadowBackoff->end()) {if(GetTickCount()-bo->second<2000) return nullptr;shadowBackoff->erase(bo);}}
        auto it=shadows->find(self);
        D3DSURFACE_DESC desc{};if(FAILED(self->GetLevelDesc(0,&desc))) return nullptr;
        DWORD tag=0,tagSize=sizeof(tag);
        const bool same=SUCCEEDED(self->GetPrivateData(ShadowTag,&tag,&tagSize))&&tag==0x5348444f; // same object as when the twin was made
        if(it!=shadows->end()&&same&&it->second.valid&&it->second.w==desc.Width&&it->second.h==desc.Height&&it->second.format==desc.Format) return &it->second;
        if(it!=shadows->end()&&it->second.locked&&(!same||DWORD(GetTickCount()-it->second.lockedAt)>8000)) {   // 0.9.47.27
            InterlockedIncrement(&staleTwin);noteStale("texture twin",self,same,GetTickCount()-it->second.lockedAt);
            if(it->second.sys) it->second.sys->UnlockRect(0);
            shadows->erase(it);it=shadows->end();
        }
        if(it!=shadows->end()&&!same) it->second.valid=false;
        if(self->GetLevelCount()!=1||desc.Pool!=D3DPOOL_DEFAULT||!(desc.Usage&D3DUSAGE_DYNAMIC)) {
            // 0.8.59 diagnostic: why a texture gets no twin (reasons counted, the first few described in the log).
            if(desc.Pool!=D3DPOOL_DEFAULT) InterlockedIncrement(&refused[0]);else if(self->GetLevelCount()!=1) InterlockedIncrement(&refused[1]);else InterlockedIncrement(&refused[2]);
            if(refusedLogged<6&&desc.Pool==D3DPOOL_DEFAULT) {++refusedLogged;log("Texture twin refused: %ux%u format %u levels %lu pool %u usage 0x%lx",desc.Width,desc.Height,unsigned(desc.Format),(unsigned long)self->GetLevelCount(),unsigned(desc.Pool),(unsigned long)desc.Usage);}
            return nullptr;
        }
        TextureShadow& sh=(*shadows)[self];
        if(!sh.sys||sh.w!=desc.Width||sh.h!=desc.Height||sh.format!=desc.Format) {
            sh=TextureShadow{};ComPtr<IDirect3DDevice9> dev;if(FAILED(self->GetDevice(&dev))) return nullptr;
            const HRESULT hc=dev->CreateTexture(desc.Width,desc.Height,1,0,desc.Format,D3DPOOL_SYSTEMMEM,&sh.sys,nullptr);
            if(FAILED(hc)) {shadowFail(self,1,hc,desc);return nullptr;}
            sh.w=desc.Width;sh.h=desc.Height;sh.format=desc.Format;
        }
        // Fill from the texture (waits once for the GPU).
        auto lk=reinterpret_cast<LockFn>(texLock.original(self));auto ul=reinterpret_cast<UnlockFn>(texUnlock.original(self));
        D3DLOCKED_RECT src{},dst{};
        const HRESULT hl=(lk&&ul)?lk(self,0,&src,nullptr,D3DLOCK_READONLY):E_FAIL;
        if(FAILED(hl)) {shadowFail(self,2,hl,desc);return nullptr;}
        const HRESULT hs=sh.sys->LockRect(0,&dst,nullptr,0);bool ok=SUCCEEDED(hs);
        if(ok) {
            UINT rowBytes=UINT(std::min(src.Pitch,dst.Pitch));UINT rows=desc.Height;
            if(desc.Format==D3DFMT_DXT1||desc.Format==D3DFMT_DXT3||desc.Format==D3DFMT_DXT5) rows=(desc.Height+3)/4;
            for(UINT y=0;y<rows;++y) memcpy(static_cast<unsigned char*>(dst.pBits)+size_t(y)*dst.Pitch,static_cast<unsigned char*>(src.pBits)+size_t(y)*src.Pitch,rowBytes);
            sh.sys->UnlockRect(0);
        }
        ul(self,0);
        if(!ok) {shadowFail(self,3,hs,desc);return nullptr;}
        tag=0x5348444f;self->SetPrivateData(ShadowTag,&tag,sizeof(tag),0);
        sh.valid=true;++shadowFills;
        return &sh;
    }
    // ---------------------------------------------------------------------------------------------
    // 0.8.60 TextureStaging (INI TextureStaging=1, default). The 0.8.59 measurements showed that the two texture twins locked
    // ~2000 times per second each cost ~70 us per lock: the twin is a SYSTEMMEM texture whose previous UpdateTexture is still
    // in flight, and the runtime makes every new LockRect wait for that copy. Here the game's lock is served from a plain
    // CPU buffer (the "mirror", same bytes as the texture, no D3D object, never busy). UnlockRect then copies the locked
    // rectangle into a SYSTEMMEM staging surface taken from a pool (a surface is reused only after its own copy has finished,
    // tracked by an event query) and queues UpdateSurface into the texture. The game thread never waits for the GPU.
    // Safety: the mirror is filled from the real texture (one wait) and dropped whenever another path writes the texture
    // (same invalidation as the twin); about every 400th upload (and the first 6) is read back from the GPU and compared with the
    // mirror (X channel ignored); a difference repairs the texture and 3 differences switch the whole feature off (back to the twin).
    static inline const GUID MirrorTag={0x2f6c8d11,0x7a3e,0x4c0b,{0x8e,0x57,0x1d,0x93,0xc4,0x6a,0x20,0xb8}};
    struct TextureMirror {std::vector<unsigned char> data;UINT w=0,h=0,pitch=0,bpp=0;D3DFORMAT format=D3DFMT_UNKNOWN;bool valid=false,locked=false,readOnly=false;RECT rect{};uint64_t used=0;DWORD lockedAt=0;};
    struct StagingEntry {ComPtr<IDirect3DSurface9> surface;ComPtr<IDirect3DQuery9> fence;IDirect3DDevice9* device=nullptr;UINT w=0,h=0;D3DFORMAT format=D3DFMT_UNKNOWN;size_t bytes=0;bool busy=false;uint64_t issued=0;};
    static inline std::unordered_map<void*,TextureMirror>* mirrors=nullptr;
    static inline size_t mirrorBytes=0;static inline uint64_t mirrorSequence=0;
    static inline bool stagingOn=true;
    static constexpr size_t MirrorCap=96u*1024*1024,StagingCap=48u*1024*1024;static constexpr size_t StagingMaxEntries=160;
    static inline CRITICAL_SECTION stagingLock;
    static inline std::vector<std::unique_ptr<StagingEntry>> stagingAll;
    static inline std::deque<StagingEntry*> stagingBusy;
    static inline std::unordered_map<uint64_t,std::vector<StagingEntry*>> stagingFree;
    static inline size_t stagingBytes=0;static inline uint64_t stagingIssue=0;
    static inline volatile LONG stLocks=0,stUploads=0,stCreated=0,stReused=0,stBlocked=0,stFallbacks=0,stVerified=0,stMismatch=0,stVerifySkipped=0,stFills=0,stRefused=0,stFailures=0,stDrops=0;
    static inline volatile LONG64 stUploadTicks=0,stStageLockTicks=0,stUploadBytes=0;
    static inline unsigned stMismatchLogs=0,stFailStreak=0,stSkipLogs=0,stSurfaceCheckLogs=0;static inline uint32_t stUploadSeq=0;static inline bool stDisabledLogged=false;
    static UINT mirrorBpp(D3DFORMAT f) {
        switch(f) {
        case D3DFMT_A8R8G8B8:case D3DFMT_X8R8G8B8:case D3DFMT_A8B8G8R8:case D3DFMT_X8B8G8R8: return 4;
        case D3DFMT_R5G6B5:case D3DFMT_X1R5G5B5:case D3DFMT_A1R5G5B5:case D3DFMT_A4R4G4B4:case D3DFMT_A8L8: return 2;
        case D3DFMT_L8:case D3DFMT_A8: return 1;
        default: return 0;
        }
    }
    static uint64_t stagingKey(UINT w,UINT h,D3DFORMAT f) {return (uint64_t(w)<<40)|(uint64_t(h)<<20)|uint64_t(unsigned(f)&0xfffff);}
    // Under stagingLock. Moves finished copies back to the free lists (event queries complete in issue order).
    static void stagingReap(bool flush) {
        while(!stagingBusy.empty()) {
            StagingEntry* e=stagingBusy.front();
            if(e->fence->GetData(nullptr,0,flush?D3DGETDATA_FLUSH:0)!=S_OK) break;
            stagingBusy.pop_front();e->busy=false;stagingFree[stagingKey(e->w,e->h,e->format)].push_back(e);
        }
    }
    static StagingEntry* stagingTake(IDirect3DDevice9* dev,uint64_t key) {
        auto it=stagingFree.find(key);
        if(it==stagingFree.end()) return nullptr;
        while(!it->second.empty()) {StagingEntry* e=it->second.back();it->second.pop_back();if(e->device==dev) {InterlockedIncrement(&stReused);return e;}}
        return nullptr;
    }
    static StagingEntry* stagingCreate(IDirect3DDevice9* dev,UINT w,UINT h,D3DFORMAT f,UINT bpp) {
        const size_t n=size_t(w)*h*bpp;
        if(stagingAll.size()>=StagingMaxEntries||stagingBytes+n>StagingCap) return nullptr;
        auto e=std::make_unique<StagingEntry>();
        if(FAILED(dev->CreateOffscreenPlainSurface(w,h,f,D3DPOOL_SYSTEMMEM,&e->surface,nullptr))||FAILED(dev->CreateQuery(D3DQUERYTYPE_EVENT,&e->fence))) {InterlockedIncrement(&stFailures);return nullptr;}
        e->device=dev;e->w=w;e->h=h;e->format=f;e->bytes=n;stagingBytes+=n;InterlockedIncrement(&stCreated);
        StagingEntry* raw=e.get();stagingAll.push_back(std::move(e));return raw;
    }
    // Under stagingLock. Frees every idle surface (all sizes) to make room.
    static void stagingTrim() {
        std::unordered_set<StagingEntry*> idle;
        for(auto& kv:stagingFree) for(auto* e:kv.second) idle.insert(e);
        stagingFree.clear();
        stagingAll.erase(std::remove_if(stagingAll.begin(),stagingAll.end(),[&](const std::unique_ptr<StagingEntry>& e){
            if(!idle.count(e.get())) return false;stagingBytes-=e->bytes;return true;}),stagingAll.end());
    }
    static StagingEntry* stagingAcquire(IDirect3DDevice9* dev,UINT w,UINT h,D3DFORMAT f,UINT bpp,bool& blocked) {
        blocked=false;
        stagingReap(false);
        const uint64_t key=stagingKey(w,h,f);
        if(StagingEntry* e=stagingTake(dev,key)) return e;
        if(StagingEntry* e=stagingCreate(dev,w,h,f,bpp)) return e;
        stagingReap(true);
        if(StagingEntry* e=stagingTake(dev,key)) return e;
        // Pool full and everything of this size still in flight: the oldest of them is reused (its lock waits for its copy).
        for(auto it=stagingBusy.begin();it!=stagingBusy.end();++it) if((*it)->device==dev&&stagingKey((*it)->w,(*it)->h,(*it)->format)==key) {
            StagingEntry* e=*it;stagingBusy.erase(it);e->busy=false;blocked=true;InterlockedIncrement(&stBlocked);return e;
        }
        stagingTrim();
        return stagingCreate(dev,w,h,f,bpp);
    }
    static void stagingReset() {
        EnterCriticalSection(&stagingLock);
        stagingBusy.clear();stagingFree.clear();stagingAll.clear();stagingBytes=0;
        LeaveCriticalSection(&stagingLock);
    }
    // Once per game frame (game thread): collect the copies that finished with the previous frame's presentation.
    static void stagingEndFrame() {
        if(!ready||!stagingOn) return;
        EnterCriticalSection(&stagingLock);stagingReap(false);LeaveCriticalSection(&stagingLock);
    }
    static void clearMirrors() {if(mirrors) {mirrors->clear();mirrorBytes=0;}stagingReset();}
    static void disableStaging(const char* why) {
        stagingOn=false;
        if(!stDisabledLogged) {stDisabledLogged=true;log("Texture staging: switched OFF (%s); back to the 0.8.59 texture twins",why);}
        EnterCriticalSection(&lock);for(auto& kv:*mirrors) if(!kv.second.locked) kv.second.valid=false;LeaveCriticalSection(&lock);
    }
    // Under 'lock'. The mirror of a texture (created and filled from the texture on first use), or null when the texture cannot have one.
    static TextureMirror* mirrorFor(IDirect3DTexture9* self) {
        D3DSURFACE_DESC desc{};if(FAILED(self->GetLevelDesc(0,&desc))) return nullptr;
        const UINT bpp=mirrorBpp(desc.Format);
        if(!bpp||desc.Pool!=D3DPOOL_DEFAULT||!(desc.Usage&D3DUSAGE_DYNAMIC)||(desc.Usage&(D3DUSAGE_RENDERTARGET|D3DUSAGE_DEPTHSTENCIL))||desc.Width>4096||desc.Height>4096||self->GetLevelCount()!=1) {InterlockedIncrement(&stRefused);return nullptr;}
        DWORD tag=0,tagSize=sizeof(tag);
        const bool same=SUCCEEDED(self->GetPrivateData(MirrorTag,&tag,&tagSize))&&tagSize==sizeof(tag)&&tag==0x4d495252; // same object as when the mirror was made
        auto it=mirrors->find(self);
        if(it!=mirrors->end()) {
            TextureMirror& m=it->second;
            if(m.locked&&(!same||DWORD(GetTickCount()-m.lockedAt)>8000)) {   // 0.9.47.27: lock state of a texture that no longer exists (or never unlocked)
                InterlockedIncrement(&staleMirror);noteStale("texture mirror",self,same,GetTickCount()-m.lockedAt);
                m.locked=false;m.valid=false;
            }
            if(same&&m.valid&&m.w==desc.Width&&m.h==desc.Height&&m.format==desc.Format) {m.used=++mirrorSequence;return &m;}
            if(m.locked) return nullptr;
            mirrorBytes-=m.data.size();mirrors->erase(it);
        }
        const UINT pitch=(desc.Width*bpp+15u)&~15u;const size_t n=size_t(pitch)*desc.Height;
        while(mirrorBytes+n>MirrorCap) {
            auto oldest=mirrors->end();
            for(auto i=mirrors->begin();i!=mirrors->end();++i) if(!i->second.locked&&(oldest==mirrors->end()||i->second.used<oldest->second.used)) oldest=i;
            if(oldest==mirrors->end()) return nullptr;
            mirrorBytes-=oldest->second.data.size();mirrors->erase(oldest);
        }
        auto lk=reinterpret_cast<LockFn>(texLock.original(self));auto ul=reinterpret_cast<UnlockFn>(texUnlock.original(self));
        D3DLOCKED_RECT src{};
        if(!lk||!ul||FAILED(lk(self,0,&src,nullptr,D3DLOCK_READONLY))) return nullptr; // waits once for the GPU
        TextureMirror m;m.w=desc.Width;m.h=desc.Height;m.pitch=pitch;m.bpp=bpp;m.format=desc.Format;
        const bool ok=src.pBits&&src.Pitch>=int(desc.Width*bpp);
        if(ok) {
            m.data.resize(n);
            for(UINT y=0;y<desc.Height;++y) memcpy(m.data.data()+size_t(y)*pitch,static_cast<unsigned char*>(src.pBits)+size_t(y)*src.Pitch,size_t(desc.Width)*bpp);
        }
        ul(self,0);
        if(!ok) return nullptr;
        tag=0x4d495252;self->SetPrivateData(MirrorTag,&tag,sizeof(tag),0);
        m.valid=true;m.used=++mirrorSequence;
        mirrorBytes+=n;InterlockedIncrement(&stFills);
        return &((*mirrors)[self]=std::move(m));
    }
    // Writes a rectangle of the mirror into the texture with a native lock (waits for the GPU): repair and fallback path.
    static bool writeNative(IDirect3DTexture9* self,const TextureMirror& m,const RECT& r) {
        auto lk=reinterpret_cast<LockFn>(texLock.original(self));auto ul=reinterpret_cast<UnlockFn>(texUnlock.original(self));
        if(!lk||!ul) return false;
        D3DLOCKED_RECT dst{};
        if(FAILED(lk(self,0,&dst,nullptr,0))||!dst.pBits||dst.Pitch<int(m.w*m.bpp)) {if(dst.pBits) ul(self,0);return false;}
        const size_t rowBytes=size_t(r.right-r.left)*m.bpp;
        for(LONG y=r.top;y<r.bottom;++y) memcpy(static_cast<unsigned char*>(dst.pBits)+size_t(y)*dst.Pitch+size_t(r.left)*m.bpp,m.data.data()+size_t(y)*m.pitch+size_t(r.left)*m.bpp,rowBytes);
        ul(self,0);
        return true;
    }
    // Reads the whole texture back from the GPU (waits) and compares it with the mirror; true when equal. X channels are ignored.
    static bool verifyAgainstTexture(IDirect3DTexture9* self,const TextureMirror& m,int& firstX,int& firstY) {
        auto lk=reinterpret_cast<LockFn>(texLock.original(self));auto ul=reinterpret_cast<UnlockFn>(texUnlock.original(self));
        D3DLOCKED_RECT src{};firstX=firstY=-1;
        HRESULT lr=lk?lk(self,0,&src,nullptr,D3DLOCK_READONLY):E_FAIL;
        // 0.8.62: the texture-level read-back was refused (0x8876086C) in 29 of 31 checks of the 0.8.61 report; try the same level through its surface.
        ComPtr<IDirect3DSurface9> level;bool viaSurface=false;const HRESULT textureResult=lr;
        if((FAILED(lr)||!src.pBits)&&SUCCEEDED(self->GetSurfaceLevel(0,&level))&&level) {
            auto sl=reinterpret_cast<SurfaceShadows::LockFn>(surfLock.original(level.Get()));
            D3DLOCKED_RECT alt{};
            const HRESULT hr2=sl?sl(level.Get(),&alt,nullptr,D3DLOCK_READONLY):level->LockRect(&alt,nullptr,D3DLOCK_READONLY);
            if(SUCCEEDED(hr2)&&alt.pBits) {src=alt;lr=hr2;viaSurface=true;if(stSurfaceCheckLogs<2) {++stSurfaceCheckLogs;log("Texture staging check: read-back through the surface (the texture lock returned 0x%08lX)",(unsigned long)textureResult);}}
        }
        auto release=[&]() {
            if(viaSurface) {auto su=reinterpret_cast<SurfaceShadows::UnlockFn>(surfUnlock.original(level.Get()));if(su) su(level.Get());else level->UnlockRect();}
            else if(ul) ul(self,0);
        };
        if(!lk||!ul||FAILED(lr)||!src.pBits||src.Pitch<int(m.w*m.bpp)) {
            InterlockedIncrement(&stVerifySkipped);
            if(stSkipLogs<4) {++stSkipLogs;log("Texture staging check skipped: read-back lock returned 0x%08lX (pitch %d, bits %p) for %ux%u format %u",(unsigned long)lr,int(src.Pitch),src.pBits,m.w,m.h,unsigned(m.format));}
            if(SUCCEEDED(lr)&&src.pBits) release();
            return true;
        }
        bool same=true;
        for(UINT y=0;y<m.h&&same;++y) {
            const unsigned char* a=m.data.data()+size_t(y)*m.pitch;const unsigned char* b=static_cast<const unsigned char*>(src.pBits)+size_t(y)*src.Pitch;
            if(memcmp(a,b,size_t(m.w)*m.bpp)==0) continue;
            for(UINT x=0;x<m.w;++x) {
                bool eq;
                if(m.format==D3DFMT_X8R8G8B8||m.format==D3DFMT_X8B8G8R8) eq=memcmp(a+x*4,b+x*4,3)==0;
                else if(m.format==D3DFMT_X1R5G5B5) {uint16_t p,q;memcpy(&p,a+x*2,2);memcpy(&q,b+x*2,2);eq=((p^q)&0x7fff)==0;}
                else eq=memcmp(a+size_t(x)*m.bpp,b+size_t(x)*m.bpp,m.bpp)==0;
                if(!eq) {same=false;firstX=int(x);firstY=int(y);break;}
            }
        }
        release();
        return same;
    }
    // Sends a rectangle of the mirror to the texture through a staging surface. Called with 'lock' held. False: nothing was queued.
    static bool stagingUpload(IDirect3DTexture9* self,const TextureMirror& m,const RECT& r) {
        const long long t0=qpc();
        ComPtr<IDirect3DDevice9> dev;ComPtr<IDirect3DSurface9> dst;
        if(FAILED(self->GetDevice(&dev))||FAILED(self->GetSurfaceLevel(0,&dst))) return false;
        auto up=reinterpret_cast<SurfaceShadows::UploadFn>(updSurface.original(dev.Get()));
        const UINT rw=UINT(r.right-r.left),rh=UINT(r.bottom-r.top);
        bool ok=false;
        EnterCriticalSection(&stagingLock);
        bool blocked=false;StagingEntry* e=stagingAcquire(dev.Get(),rw,rh,m.format,m.bpp,blocked);
        if(e) {
            IDirect3DSurface9* ss=e->surface.Get();
            auto sl=reinterpret_cast<SurfaceShadows::LockFn>(surfLock.original(ss));auto su=reinterpret_cast<SurfaceShadows::UnlockFn>(surfUnlock.original(ss)); // original calls: not our own hooks
            D3DLOCKED_RECT lr{};const long long l0=qpc();
            HRESULT hr=sl?sl(ss,&lr,nullptr,0):ss->LockRect(&lr,nullptr,0);
            InterlockedExchangeAdd64(&stStageLockTicks,qpc()-l0);
            const bool locked=SUCCEEDED(hr);ok=locked&&lr.pBits&&lr.Pitch>=int(rw*m.bpp);
            if(ok) for(UINT y=0;y<rh;++y) memcpy(static_cast<unsigned char*>(lr.pBits)+size_t(y)*lr.Pitch,m.data.data()+size_t(r.top+LONG(y))*m.pitch+size_t(r.left)*m.bpp,size_t(rw)*m.bpp);
            if(locked) {if(su) su(ss);else ss->UnlockRect();}
            if(ok) {
                POINT pt{r.left,r.top};
                hr=up?up(dev.Get(),ss,nullptr,dst.Get(),&pt):dev->UpdateSurface(ss,nullptr,dst.Get(),&pt);
                ok=SUCCEEDED(hr);
            }
            if(ok) {e->fence->Issue(D3DISSUE_END);e->busy=true;e->issued=++stagingIssue;stagingBusy.push_back(e);}
            else {e->busy=false;stagingFree[stagingKey(e->w,e->h,e->format)].push_back(e);InterlockedIncrement(&stFailures);}
        }
        LeaveCriticalSection(&stagingLock);
        if(ok) {
            InterlockedIncrement(&stUploads);InterlockedExchangeAdd64(&stUploadBytes,LONG64(rw)*rh*m.bpp);InterlockedExchangeAdd64(&stUploadTicks,qpc()-t0);
            if(LockDetail::enabled) GpuUse::note(self);
        }
        return ok;
    }
    // UnlockRect of a mirrored texture. Called with 'lock' held; returns with it still held. 'verify' = a read-back check is due
    // (done by the caller after releasing 'lock': it waits for the GPU).
    static HRESULT mirrorUnlock(IDirect3DTexture9* self,TextureMirror& m,bool& verify) {
        m.locked=false;verify=false;
        if(m.readOnly) return D3D_OK;
        const RECT r=m.rect;
        const bool sent=stagingOn&&stagingUpload(self,m,r);
        if(!sent) {
            InterlockedIncrement(&stFallbacks);if(!writeNative(self,m,r)) {m.valid=false;InterlockedIncrement(&stFailures);}
            if(stagingOn&&++stFailStreak>=20) {LeaveCriticalSection(&lock);disableStaging("uploads keep failing");EnterCriticalSection(&lock);} // 20 in a row: this GPU/driver refuses the staging path
        }
        else {stFailStreak=0;++stUploadSeq;verify=stUploadSeq<=6||(stUploadSeq%400)==0;}
        return D3D_OK;
    }
    static void mirrorVerify(IDirect3DTexture9* self) {
        EnterCriticalSection(&lock);
        auto it=mirrors->find(self);
        if(it==mirrors->end()||!it->second.valid||it->second.locked) {LeaveCriticalSection(&lock);return;}
        const TextureMirror* m=&it->second;
        LeaveCriticalSection(&lock);
        int fx=-1,fy=-1;const bool same=verifyAgainstTexture(self,*m,fx,fy); // only the game thread changes a mirror
        InterlockedIncrement(&stVerified);
        if(same) return;
        InterlockedIncrement(&stMismatch);
        if(stMismatchLogs<4) {++stMismatchLogs;log("Texture staging CHECK FAILED: %ux%u format %u differs from the mirror at %d,%d; texture repaired from the mirror",m->w,m->h,unsigned(m->format),fx,fy);}
        writeNative(self,*m,RECT{0,0,LONG(m->w),LONG(m->h)});
        if(stMismatch>=3) disableStaging("3 failed checks");
    }
    // 0.8.60: one record per outermost lock of the game thread (what, how big, which flags, who called, time since the GPU used it).
    static void recordDetail(int kind,void* resource,const D3DSURFACE_DESC& desc,const RECT* rect,DWORD flags,long long t0,long long dt,uintptr_t ret,void** frame) {
        LockDetail::Key k;memset(&k,0,sizeof(k));
        const int p=lockPath;
        k.kind=uint8_t(kind);k.pool=uint8_t(desc.Pool);k.flags=LockDetail::flagBits(flags);k.w=uint16_t(desc.Width);k.h=uint16_t(desc.Height);
        k.path=uint8_t(p==2?0:(kind==0&&p==1)?1:(p==1||p==3)?2:3);
        if(rect) {k.rw=uint16_t(rect->right-rect->left);k.rh=uint16_t(rect->bottom-rect->top);}
        CallerScan::scan(frame,ret,k.wrapperRva,k.engineRva);
        k.object=uint32_t(reinterpret_cast<uintptr_t>(resource));
        const int group=(kind==1&&desc.Pool==D3DPOOL_SYSTEMMEM)?0:(kind==0&&p==1)?1:(p==1||p==3)?2:3;
        LockDetail::record(k,dt,group,GpuUse::bucketAt(GpuUse::tickOf(lockGap),t0));
        // 0.8.62: a lock longer than 1 ms is explained by what the game thread did just before it (3 dumps per 5 s at most).
        if(dt>=static_cast<long long>(Clock::freq()/1000.)&&CallTiming::slowBudget>0) {
            --CallTiming::slowBudget;
            log("Slow lock %.0f us: %s %ux%u pool %u %s%s obj %08X wrapper+%06X engine+%06X; last game-thread device calls before it, newest first:%s",
                Clock::micros(dt),kind?"surface":"texture",unsigned(desc.Width),unsigned(desc.Height),unsigned(desc.Pool),(flags&D3DLOCK_READONLY)?"R":"W",(flags&D3DLOCK_DISCARD)?"+discard":"",
                unsigned(k.object),unsigned(k.wrapperRva),unsigned(k.engineRva),CallTiming::dumpTrace(t0,resource,14).c_str());
        }
    }
    static HRESULT __stdcall hTexLock(IDirect3DTexture9* self,UINT level,D3DLOCKED_RECT* out,const RECT* rect,DWORD flags) {
        const bool top=lockDepth++==0;const long long t0=top?qpc():0;if(top){lockPath=0;lockCs=0;lockGap=self;}
        const uintptr_t ret=reinterpret_cast<uintptr_t>(__builtin_return_address(0));void** frame=reinterpret_cast<void**>(__builtin_frame_address(0));
        HRESULT hr=texLockImpl(self,level,out,rect,flags);
        --lockDepth;
        if(top) {
            if(LockDetail::enabled&&onGameThread()) {D3DSURFACE_DESC desc{};if(SUCCEEDED(self->GetLevelDesc(0,&desc))) recordDetail(0,self,desc,rect,flags,t0,qpc()-t0,ret,frame);}
            lockDone(0,t0);
        }
        return hr;
    }
    static HRESULT texLockImpl(IDirect3DTexture9* self,UINT level,D3DLOCKED_RECT* out,const RECT* rect,DWORD flags) {
        auto f=reinterpret_cast<LockFn>(texLock.original(self));
        if(!f) return E_FAIL;
        if(stagingOn&&level==0&&out&&onGameThread()) {
            // 0.8.60: served from the memory mirror (see TextureStaging above): no D3D lock, no wait.
            const bool top=lockDepth==1;const long long c0=top?qpc():0;EnterCriticalSection(&lock);if(top) lockCs=qpc()-c0;
            TextureMirror* m=mirrorFor(self);
            if(m&&m->locked) {LeaveCriticalSection(&lock);return D3DERR_INVALIDCALL;}
            if(m) {
                const RECT area=rect?*rect:RECT{0,0,LONG(m->w),LONG(m->h)};
                if(area.left>=0&&area.top>=0&&area.right>area.left&&area.bottom>area.top&&area.right<=LONG(m->w)&&area.bottom<=LONG(m->h)) {
                    out->Pitch=INT(m->pitch);out->pBits=m->data.data()+size_t(area.top)*m->pitch+size_t(area.left)*m->bpp;
                    m->locked=true;m->lockedAt=GetTickCount();m->readOnly=(flags&D3DLOCK_READONLY)!=0;m->rect=area;
                    auto tw=shadows->find(self);if(tw!=shadows->end()&&!tw->second.locked) tw->second.valid=false; // the twin no longer matches the texture
                    LeaveCriticalSection(&lock);InterlockedIncrement(&stLocks);if(top) lockPath=3;
                    return D3D_OK;
                }
            }
            LeaveCriticalSection(&lock);
        }
        if(shadowOn&&level==0&&out) {
            const bool top=lockDepth==1;const long long c0=top?qpc():0;EnterCriticalSection(&lock);const long long cs=top?qpc()-c0:0;
            TextureShadow* sh=shadowFor(self);
            if(sh&&!sh->locked) {
                DWORD sf=flags&~(D3DLOCK_DISCARD|D3DLOCK_NOOVERWRITE|D3DLOCK_DONOTWAIT);
                if(SUCCEEDED(sh->sys->LockRect(0,out,rect,sf))) {
                    sh->locked=true;sh->lockedAt=GetTickCount();sh->readOnly=(flags&D3DLOCK_READONLY)!=0;
                    if(mirrors) {auto mi=mirrors->find(self);if(mi!=mirrors->end()&&!mi->second.locked) mi->second.valid=false;} // the twin is about to be written
                    LeaveCriticalSection(&lock);InterlockedIncrement(&shadowLocks);if(top){lockPath=1;lockCs=cs;lockGap=sh->sys.Get();}
                    if(!shadowLogged) {shadowLogged=true;log("Game texture shadow: active (locks served from system memory, uploads queued)");}
                    return D3D_OK;
                }
                ++shadowFailures;sh->valid=false;
                if(shadowFailLogs<24) {++shadowFailLogs;log("Texture twin failure (0.9.47.26) #%u: site 4 (lock of the twin) texture %p",shadowFailLogs,(void*)self);}
                if(!shadowBackoff) shadowBackoff=new std::unordered_map<void*,DWORD>();
                (*shadowBackoff)[self]=GetTickCount();
            }
            LeaveCriticalSection(&lock);
        }
        InterlockedIncrement(&directLocks);if(lockDepth==1) lockPath=2;
        return f(self,level,out,rect,flags);
    }
    static HRESULT __stdcall hTexUnlock(IDirect3DTexture9* self,UINT level) {
        CallTiming::EvictUnlock evictTimer;   // 0.9.26 PERF
        auto f=reinterpret_cast<UnlockFn>(texUnlock.original(self));
        if(level==0&&ready) {
            EnterCriticalSection(&lock);
            auto mi=mirrors->find(self);
            if(mi!=mirrors->end()&&mi->second.locked&&!tagIs(self,MirrorTag,0x4d495252)) {   // 0.9.47.27: the locked mirror belongs to a texture that is gone: this unlock is a native one
                InterlockedIncrement(&staleMirror);noteStale("texture mirror at unlock",self,false,GetTickCount()-mi->second.lockedAt);
                mi->second.locked=false;mi->second.valid=false;
            }
            if(mi!=mirrors->end()&&mi->second.locked) {
                bool verify=false;const bool readOnly=mi->second.readOnly;
                HRESULT hr=mirrorUnlock(self,mi->second,verify);
                LeaveCriticalSection(&lock);
                if(!readOnly) {invalidateTextureSurfaces(self);touch(self);}
                if(verify) mirrorVerify(self);
                return hr;
            }
            auto it=shadows->find(self);
            if(it!=shadows->end()&&it->second.locked&&!tagIs(self,ShadowTag,0x5348444f)) {   // 0.9.47.27: same as above for the twin
                InterlockedIncrement(&staleTwin);noteStale("texture twin at unlock",self,false,GetTickCount()-it->second.lockedAt);
                if(it->second.sys) it->second.sys->UnlockRect(0);
                shadows->erase(it);it=shadows->end();
            }
            if(it!=shadows->end()&&it->second.locked) {
                TextureShadow& sh=it->second;sh.locked=false;
                HRESULT hr=sh.sys->UnlockRect(0);bool readOnly=sh.readOnly;ComPtr<IDirect3DTexture9> sys=sh.sys;
                LeaveCriticalSection(&lock);
                if(!readOnly) {
                    invalidateTextureSurfaces(self);
                    ComPtr<IDirect3DDevice9> dev;
                    // Queued copy of the changed area (dirty region of the system-memory twin).
                    if(SUCCEEDED(self->GetDevice(&dev))) {
                        auto u=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,IDirect3DBaseTexture9*,IDirect3DBaseTexture9*)>(updTexture.original(dev.Get()));
                        HRESULT r2=u?u(dev.Get(),sys.Get(),self):dev->UpdateTexture(sys.Get(),self);
                        if(LockDetail::enabled&&onGameThread()) {GpuUse::note(sys.Get());GpuUse::note(self);}
                        if(FAILED(r2)) {InterlockedIncrement(&shadowFailures);dropShadow(self);}
                    }
                    touch(self);
                }
                return hr;
            }
            LeaveCriticalSection(&lock);
        }
        HRESULT hr=f?f(self,level):E_FAIL;dropShadow(self);invalidateTextureSurfaces(self);touch(self);return hr;
    }
    static void reportShadow() {
        if(!ready) return;
        {const LONG sm=staleMirror,st=staleTwin;const unsigned long long ss=surfaceShadows?surfaceShadows->stats.stale:0;
         if(sm||st||ss) log("Stale lock states discarded since the start (0.9.47.27): mirrors %ld, twins %ld, surfaces %llu",(long)sm,(long)st,ss);}
        log("Game texture shadow: %s; locks from shadow %ld, direct %ld, fills %ld, failures %ld, textures %zu",shadowOn?"on":"off (GameTextureShadow=0)",
            (long)InterlockedExchange(&shadowLocks,0),(long)InterlockedExchange(&directLocks,0),(long)InterlockedExchange(&shadowFills,0),(long)shadowFailures,shadows->size());
        {
            static const double freq=[]{LARGE_INTEGER f;QueryPerformanceFrequency(&f);return double(f.QuadPart);}();
            char line[900];int n=snprintf(line,sizeof(line),"Lock cost (last 5 s, game thread; n x average us; CS wait = waiting for the shadow lock):");
            static const char* kinds[2]={"texture","surface"};static const char* paths[4]={"other","shadow","native","memory mirror"};
            for(int k=0;k<2;++k) {
                long long csTicks=InterlockedExchange64(&lockCsTicks[k],0);long long total=0;LONG totalN=0;
                for(int p=0;p<4;++p) {
                    long long t=InterlockedExchange64(&lockTicks[k][p],0);LONG c=InterlockedExchange(&lockCount[k][p],0);
                    total+=t;totalN+=c;
                    if(c) n+=snprintf(line+n,sizeof(line)-n," | %s %s %ld x %.1f",kinds[k],paths[p],(long)c,t*1e6/freq/c);
                }
                if(totalN) n+=snprintf(line+n,sizeof(line)-n," | %s total %.0f ms, CS wait %.0f ms",kinds[k],total*1e3/freq,csTicks*1e3/freq);
            }
            n+=snprintf(line+n,sizeof(line)-n," | texture locks without twin: pool %ld, mip levels %ld, usage %ld",
                (long)InterlockedExchange(&refused[0],0),(long)InterlockedExchange(&refused[1],0),(long)InterlockedExchange(&refused[2],0));
            n+=snprintf(line+n,sizeof(line)-n," | native surface locks: DEFAULT small %ld, DEFAULT large %ld, other pools %ld",
                (long)InterlockedExchange(&directKinds[0],0),(long)InterlockedExchange(&directKinds[1],0),(long)InterlockedExchange(&directKinds[2],0));
            log("%s",line);
        }
        EnterCriticalSection(&lock);
        const auto st=surfaceShadows->stats;
        log("Game surface shadow: %s; locks %llu, direct %llu, fills %llu, uploads %llu, invalidations %llu, failures %llu, surfaces %zu",
            surfaceShadowOn&&hooksOk?"on":"off",(unsigned long long)st.locks,(unsigned long long)st.direct,(unsigned long long)st.fills,
            (unsigned long long)st.uploads,(unsigned long long)st.invalidations,(unsigned long long)st.failures,surfaceShadows->size());
        surfaceShadows->stats={};LeaveCriticalSection(&lock);
    }
    static void reportStaging() {
        if(!ready) return;
        static const double freq=[]{LARGE_INTEGER f;QueryPerformanceFrequency(&f);return double(f.QuadPart);}();
        const LONG locks=InterlockedExchange(&stLocks,0),ups=InterlockedExchange(&stUploads,0);
        const long long upT=InterlockedExchange64(&stUploadTicks,0),lkT=InterlockedExchange64(&stStageLockTicks,0),upB=InterlockedExchange64(&stUploadBytes,0);
        EnterCriticalSection(&stagingLock);const size_t pool=stagingAll.size(),busy=stagingBusy.size(),bytes=stagingBytes;LeaveCriticalSection(&stagingLock);
        EnterCriticalSection(&lock);const size_t mir=mirrors->size(),mirBytes=mirrorBytes;LeaveCriticalSection(&lock);
        log("Texture staging (TextureStaging=%d): %s; mirror locks %ld, uploads %ld (%.1f KB each, %.1f us each in the game thread, staging lock %.1f us each), pool %zu surfaces %.1f MB (%zu in flight), created %ld reused %ld waited %ld, mirrors %zu %.1f MB (fills %ld, invalidated by other writes %ld, refused %ld), fallbacks %ld, failures %ld, read-back checks %ld (skipped %ld, failed %ld)",
            iniInt(L"TextureStaging",1),stagingOn?"on":"OFF",(long)locks,(long)ups,ups?upB/1024./ups:0.,ups?upT*1e6/freq/ups:0.,ups?lkT*1e6/freq/ups:0.,pool,bytes/1048576.,busy,
            (long)InterlockedExchange(&stCreated,0),(long)InterlockedExchange(&stReused,0),(long)InterlockedExchange(&stBlocked,0),mir,mirBytes/1048576.,(long)InterlockedExchange(&stFills,0),(long)InterlockedExchange(&stDrops,0),(long)InterlockedExchange(&stRefused,0),
            (long)InterlockedExchange(&stFallbacks,0),(long)InterlockedExchange(&stFailures,0),(long)stVerified,(long)stVerifySkipped,(long)stMismatch);
    }
    static inline volatile LONG refused[3]{};static inline unsigned refusedLogged=0;
    static inline volatile LONG directKinds[3]{}; // 0.8.59: native surface locks by kind: 0 DEFAULT small (texture sized), 1 DEFAULT large, 2 other pools
    static HRESULT __stdcall hSurfLock(IDirect3DSurface9* self,D3DLOCKED_RECT* out,const RECT* rect,DWORD flags) {
        if(self==lagWatch&&!(flags&D3DLOCK_READONLY)) {
            static unsigned logged=0;
            if(!lagDirty&&logged<6) {++logged;log("Readback lag: the game locks the read-back surface for writing (flags 0x%lx) from %s",(unsigned long)flags,CallTiming::moduleSpot(reinterpret_cast<uintptr_t>(__builtin_return_address(0))).c_str());}
            lagDirty=1;InterlockedIncrement(&lagWrites);
        }
        const bool top=lockDepth++==0;const long long t0=top?qpc():0;if(top){lockPath=0;lockCs=0;lockGap=self;}
        const uintptr_t ret=reinterpret_cast<uintptr_t>(__builtin_return_address(0));void** frame=reinterpret_cast<void**>(__builtin_frame_address(0));
        HRESULT hr=surfLockImpl(self,out,rect,flags);
        --lockDepth;
        if(top&&SUCCEEDED(hr)&&MenuTap::armed.load(std::memory_order_relaxed)&&onGameThread()) MenuTap::take(self,out,rect,flags);   // 0.8.90.7
        if(top&&SUCCEEDED(hr)&&MenuTap::dumpStage==1&&onGameThread()) MenuTap::noteWriteLock(self,out,rect,flags);   // 0.8.90.9 diagnostic
        if(top&&SUCCEEDED(hr)&&MenuTap::finalArmed.load(std::memory_order_relaxed)&&onGameThread()) MenuTap::noteFinalLock(self,out,rect,flags);   // 0.8.95
        if(top) {
            if(LockDetail::enabled&&onGameThread()) {D3DSURFACE_DESC desc{};if(SUCCEEDED(self->GetDesc(&desc))) recordDetail(1,self,desc,rect,flags,t0,qpc()-t0,ret,frame);}
            lockDone(1,t0);
        }
        return hr;
    }
    static HRESULT surfLockImpl(IDirect3DSurface9* self,D3DLOCKED_RECT* out,const RECT* rect,DWORD flags) {
        auto f=reinterpret_cast<SurfaceShadows::LockFn>(surfLock.original(self));
        auto u=reinterpret_cast<SurfaceShadows::UnlockFn>(surfUnlock.original(self));
        if(!f)return E_FAIL;
        if(ready&&surfaceShadowOn&&hooksOk&&u&&out) {
            const bool top=lockDepth==1;const long long c0=top?qpc():0;EnterCriticalSection(&lock);if(top) lockCs=qpc()-c0;
            {const uint64_t st0=surfaceShadows->stats.stale;const bool lk0=surfaceShadows->isLocked(self);if(surfaceShadows->stats.stale!=st0) noteStale("surface shadow",self,surfaceShadows->lastStaleSame,0);
             if(lk0){LeaveCriticalSection(&lock);return D3DERR_INVALIDCALL;}}
            bool redirected=surfaceShadows->lock(self,out,rect,flags,f,u);
            if(redirected) {if(top) lockPath=1;}
            else {
                ++surfaceShadows->stats.direct;if(top) lockPath=2;
                D3DSURFACE_DESC kd{};
                if(top&&SUCCEEDED(self->GetDesc(&kd))) InterlockedIncrement(&directKinds[kd.Pool==D3DPOOL_DEFAULT?(kd.Width*kd.Height<=16384u?0:1):2]);
            }
            if(surfaceDescLogs<8&&surfaceDescAttempts<256) {
                ++surfaceDescAttempts;
                D3DSURFACE_DESC desc{};
                uintptr_t caller=reinterpret_cast<uintptr_t>(__builtin_return_address(0));
                HMODULE callerModule=nullptr,wrapperModule=GetModuleHandleW(L"ddraw.dll");
                GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(caller),&callerModule);
                if(wrapperModule&&callerModule==wrapperModule&&SUCCEEDED(self->GetDesc(&desc))) {
                    uintptr_t wrapper=reinterpret_cast<uintptr_t>(wrapperModule);
                    ++surfaceDescLogs;log("Surface LockRect: %ux%u format %u pool %u usage 0x%lx flags 0x%lx CPU-mirror %s caller %p wrapper-RVA 0x%lx",
                        desc.Width,desc.Height,unsigned(desc.Format),unsigned(desc.Pool),(unsigned long)desc.Usage,(unsigned long)flags,redirected?"yes":"no",
                        reinterpret_cast<void*>(caller),(unsigned long)(wrapper?caller-wrapper:0));
                }
            }
            LeaveCriticalSection(&lock);if(redirected)return D3D_OK;
        }
        if(lockDepth==1) lockPath=2;
        return f(self,out,rect,flags);
    }
    static HRESULT __stdcall hSurfUnlock(IDirect3DSurface9* self) {
        CallTiming::EvictUnlock evictTimer;   // 0.9.26 PERF
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DSurface9*)>(surfUnlock.original(self));
        if(MenuTap::dumpStage==1&&onGameThread()) MenuTap::noteUnlock(self);   // 0.8.90.9 diagnostic: content before the unlock
        if(MenuTap::finalArmed.load(std::memory_order_relaxed)&&onGameThread()) MenuTap::noteFinalUnlock(self,MenuTap::fresh);   // 0.8.95: final image (before the unlock)
        if(ready) {
            EnterCriticalSection(&lock);
            HRESULT hr=D3D_OK;bool written=false;
            bool redirected=false;
            const uint64_t st1=surfaceShadows->stats.stale;const bool lk1=surfaceShadows->isLocked(self);if(surfaceShadows->stats.stale!=st1) noteStale("surface shadow at unlock",self,surfaceShadows->lastStaleSame,0);
            if(lk1) {
                ComPtr<IDirect3DDevice9> device;self->GetDevice(&device);
                auto upload=device?reinterpret_cast<SurfaceShadows::UploadFn>(updSurface.original(device.Get())):nullptr;
                redirected=surfaceShadows->unlock(self,reinterpret_cast<SurfaceShadows::LockFn>(surfLock.original(self)),f,upload,hr,written);
            }
            LeaveCriticalSection(&lock);
            if(redirected) {if(written)touchSurface(self,true);return hr;}
        }
        HRESULT hr=f?f(self):E_FAIL;touchSurface(self);return hr;
    }
    static HRESULT __stdcall hUpdSurface(IDirect3DDevice9* d,IDirect3DSurface9* src,const RECT* r,IDirect3DSurface9* dst,const POINT* p) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,IDirect3DSurface9*,const RECT*,IDirect3DSurface9*,const POINT*)>(updSurface.original(d));
        CallTiming::Scope timed(CallTiming::UpdateSurface,src,dst,reinterpret_cast<uintptr_t>(__builtin_return_address(0)));
        const bool mine=notVrThread();
        HRESULT hr=E_FAIL;bool staged=false;
        if(f&&restoreHook&&onGameThread()) {const HRESULT h=restoreHook(d,src,r,dst,p,reinterpret_cast<void*>(f));if(h!=HRESULT(0x7fffffff)) return h;} // 0.8.63: see ReadbackLag::restore
        if(f&&sourceStage&&onGameThread()) {const HRESULT h=sourceStage(d,src,r,dst,p,reinterpret_cast<void*>(f));if(h!=HRESULT(0x7fffffff)) {hr=h;staged=true;}}
        if(!staged) hr=f?f(d,src,r,dst,p):E_FAIL;
        if(LockDetail::enabled&&onGameThread()) {GpuUse::note(src);GpuUse::note(dst);}
        if(mine) touchSurface(dst);
        return hr;
    }
    static HRESULT __stdcall hUpdTexture(IDirect3DDevice9* d,IDirect3DBaseTexture9* src,IDirect3DBaseTexture9* dst) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,IDirect3DBaseTexture9*,IDirect3DBaseTexture9*)>(updTexture.original(d));
        CallTiming::Scope timed(CallTiming::UpdateTexture,src,dst,reinterpret_cast<uintptr_t>(__builtin_return_address(0)));
        HRESULT hr=f?f(d,src,dst):E_FAIL;if(LockDetail::enabled&&onGameThread()) {GpuUse::note(src);GpuUse::note(dst);}
        if(notVrThread()) {dropShadow(dst);invalidateTextureSurfaces(dst);touch(dst);} // the shadow's own uploads call the original directly
        return hr;
    }
    static HRESULT __stdcall hStretch(IDirect3DDevice9* d,IDirect3DSurface9* src,const RECT* a,IDirect3DSurface9* dst,const RECT* b,D3DTEXTUREFILTERTYPE t) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,IDirect3DSurface9*,const RECT*,IDirect3DSurface9*,const RECT*,D3DTEXTUREFILTERTYPE)>(stretch.original(d));
        CallTiming::Scope timed(CallTiming::StretchRect,src,dst,reinterpret_cast<uintptr_t>(__builtin_return_address(0)));
        HRESULT hr=f?f(d,src,a,dst,b,t):E_FAIL;if(LockDetail::enabled&&onGameThread()) {GpuUse::note(src);GpuUse::note(dst);}
        if(notVrThread()) touchSurface(dst);
        return hr;
    }
    static HRESULT __stdcall hFill(IDirect3DDevice9* d,IDirect3DSurface9* s,const RECT* r,D3DCOLOR c) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,IDirect3DSurface9*,const RECT*,D3DCOLOR)>(fill.original(d));
        CallTiming::Scope timed(CallTiming::ColorFill,s,nullptr,reinterpret_cast<uintptr_t>(__builtin_return_address(0)));
        HRESULT hr=f?f(d,s,r,c):E_FAIL;if(LockDetail::enabled&&onGameThread()) GpuUse::note(s);
        if(notVrThread()) touchSurface(s);
        return hr;
    }
    static HRESULT __stdcall hSetTarget(IDirect3DDevice9* d,DWORD i,IDirect3DSurface9* s) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,DWORD,IDirect3DSurface9*)>(setTarget.original(d));
        CallTiming::Scope timed(CallTiming::SetRenderTarget,s,nullptr,reinterpret_cast<uintptr_t>(__builtin_return_address(0)));
        if(LockDetail::enabled&&onGameThread()) GpuUse::note(s);
        if(notVrThread()) touchSurface(s);
        return f?f(d,i,s):E_FAIL;
    }
    static void installDevice(IDirect3DDevice9* d) {
        init();
        // Catch standalone offscreen surfaces even before a texture is captured.
        ComPtr<IDirect3DSurface9> surface;
        if(SUCCEEDED(d->CreateOffscreenPlainSurface(4,4,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&surface,nullptr)))installSurface(surface.Get());
    }
    // 0.8.62: the device-level hooks (UpdateSurface, UpdateTexture, StretchRect, ColorFill, SetRenderTarget and the timing hooks) go in AFTER the VR
    // thread's device exists: creating it made the runtime refill the vtable the two devices share, which silenced every 0.8.61 device hook.
    // INI DeviceHooks=0 skips them all (what the game thread effectively had in 0.8.60 and 0.8.61).
    static void installDeviceSlots(IDirect3DDevice9* d) {
        static bool done=false;if(done||!d) return;done=true;init();
        TimedCalls::deviceHooksOn=iniInt(L"DeviceHooks",1)!=0;
        if(!TimedCalls::deviceHooksOn) {log("Device hooks: off (DeviceHooks=0)");return;}
        bool ok=updSurface.install(d,30,reinterpret_cast<void*>(&hUpdSurface))&&updTexture.install(d,31,reinterpret_cast<void*>(&hUpdTexture))&&
            stretch.install(d,34,reinterpret_cast<void*>(&hStretch))&&fill.install(d,35,reinterpret_cast<void*>(&hFill))&&
            setTarget.install(d,37,reinterpret_cast<void*>(&hSetTarget));
        if(!ok&&hooksOk) {hooksOk=false;log("Texture tracking: device hooks unavailable (textures refreshed periodically only)");}
        TimedCalls::install(d);
        TimedCalls::deviceVtable=*reinterpret_cast<void***>(d);
        TimedCalls::note("UpdateSurface",30,reinterpret_cast<void*>(&hUpdSurface),&updSurface);TimedCalls::note("UpdateTexture",31,reinterpret_cast<void*>(&hUpdTexture),&updTexture);
        TimedCalls::note("StretchRect",34,reinterpret_cast<void*>(&hStretch),&stretch);TimedCalls::note("ColorFill",35,reinterpret_cast<void*>(&hFill),&fill);
        TimedCalls::note("SetRenderTarget",37,reinterpret_cast<void*>(&hSetTarget),&setTarget);
        TimedCalls::integrityReport(true);
    }
    static void healDevice(IDirect3DDevice9* d) {TimedCalls::heal(d);}
    static void installSurface(IDirect3DSurface9* s) {
        bool ok=surfUnlock.install(s,14,reinterpret_cast<void*>(&hSurfUnlock))&&surfLock.install(s,13,reinterpret_cast<void*>(&hSurfLock));
        if(!ok&&hooksOk){hooksOk=false;log("Surface shadow: hook unavailable; native locking retained");}
    }
    static void installTexture(IDirect3DTexture9* t) {
        void** vt=*reinterpret_cast<void***>(t);
        if(!texUnlock.has(vt)) {
            if(!texLock.install(t,19,reinterpret_cast<void*>(&hTexLock))) {if(hooksOk) log("Texture tracking: texture lock hook unavailable");}
            if(!texUnlock.install(t,20,reinterpret_cast<void*>(&hTexUnlock))) {if(hooksOk) log("Texture tracking: texture hook unavailable (periodic refresh only)");hooksOk=false;return;}
            log("Texture tracking: texture class hooked (%u)",unsigned(texUnlock.count));
            ComPtr<IDirect3DSurface9> s;
            if(SUCCEEDED(t->GetSurfaceLevel(0,&s))&&s) installSurface(s.Get());
        }
    }
};

// ---------------------------------------------------------------------------------------------
// SourceStaging (INI SourceStaging=1). The wrapper keeps every DirectDraw system-memory surface as a D3D9 SYSTEMMEM surface and uploads it with
// IDirect3DDevice9::UpdateSurface after the engine unlocked it. The GPU reads that surface later and the engine's NEXT lock of the same surface
// waits until the copy is done: in the 0.8.60/0.8.61 reports one read lock per frame of the 1024x768 interface image cost 3 to 4.7 ms (about 30 %
// of a frame in possession mode). 0.8.61 redirected three wrapper call sites for this and they were never executed (0 calls); 0.8.62 does it in
// the UpdateSurface slot of the device itself (TextureWatch::hUpdSurface -> process), where every caller goes: a large system-memory source is copied
// on the CPU into a pooled staging surface (the pool of TextureStaging, reused only after its copy is finished) and UpdateSurface reads THAT surface.
// The engine's surface is never referenced by the GPU, so its locks do not wait. Same bytes, same destination, same rectangle.
struct WrapperUpload {
    using UpdateFn=HRESULT(__stdcall*)(void*,IDirect3DSurface9*,const RECT*,IDirect3DSurface9*,const POINT*);
    static inline bool on=false;static inline bool installed=false;
    static constexpr size_t MinBytes=64*1024;
    static constexpr HRESULT NotHandled=HRESULT(0x7fffffff);
    static inline volatile LONG calls=0,done=0,sSurface=0,sPool=0,sFormat=0,sRect=0,sSmall=0,sPoolFull=0,failed=0,blocked=0;
    static inline volatile LONG64 bytes=0,ticks=0;static inline unsigned guardCount=0;static inline long long guardTicks=0;
    // NotHandled: the caller makes the original call itself.
    static HRESULT process(void* self,IDirect3DSurface9* src,const RECT* r,IDirect3DSurface9* dst,const POINT* p,void* originalFn) {
        if(!on||!src||!dst||!originalFn) return NotHandled;
        auto orig=reinterpret_cast<UpdateFn>(originalFn);
        InterlockedIncrement(&calls);
        if(!TextureWatch::surfLock.has(*reinterpret_cast<void***>(src))) {InterlockedIncrement(&sSurface);return NotHandled;}
        D3DSURFACE_DESC sd{};
        if(FAILED(src->GetDesc(&sd))) {InterlockedIncrement(&sSurface);return NotHandled;}
        if(sd.Pool!=D3DPOOL_SYSTEMMEM) {InterlockedIncrement(&sPool);return NotHandled;}
        const UINT bpp=TextureWatch::mirrorBpp(sd.Format);
        if(!bpp) {InterlockedIncrement(&sFormat);return NotHandled;}
        const RECT area=r?*r:RECT{0,0,LONG(sd.Width),LONG(sd.Height)};
        if(area.left<0||area.top<0||area.right<=area.left||area.bottom<=area.top||area.right>LONG(sd.Width)||area.bottom>LONG(sd.Height)) {InterlockedIncrement(&sRect);return NotHandled;}
        const UINT w=UINT(area.right-area.left),h=UINT(area.bottom-area.top);
        if(size_t(w)*h*bpp<MinBytes) {InterlockedIncrement(&sSmall);return NotHandled;}
        const long long t0=TextureWatch::qpc();
        ComPtr<IDirect3DDevice9> dev;
        if(FAILED(src->GetDevice(&dev))) {InterlockedIncrement(&sSurface);return NotHandled;}
        EnterCriticalSection(&TextureWatch::stagingLock);
        bool wait=false;TextureWatch::StagingEntry* e=TextureWatch::stagingAcquire(dev.Get(),w,h,sd.Format,bpp,wait);
        LeaveCriticalSection(&TextureWatch::stagingLock);
        if(!e) {InterlockedIncrement(&sPoolFull);return NotHandled;}
        if(wait) InterlockedIncrement(&blocked);
        // The entry is ours until it is handed back below (not in any list).
        IDirect3DSurface9* ss=e->surface.Get();
        auto sl=reinterpret_cast<SurfaceShadows::LockFn>(TextureWatch::surfLock.original(src));auto su=reinterpret_cast<SurfaceShadows::UnlockFn>(TextureWatch::surfUnlock.original(src));
        auto tl=reinterpret_cast<SurfaceShadows::LockFn>(TextureWatch::surfLock.original(ss));auto tu=reinterpret_cast<SurfaceShadows::UnlockFn>(TextureWatch::surfUnlock.original(ss));
        D3DLOCKED_RECT a{},b{};bool copied=false;
        HRESULT h1=sl?sl(src,&a,&area,D3DLOCK_READONLY):src->LockRect(&a,&area,D3DLOCK_READONLY);
        if(SUCCEEDED(h1)) {
            HRESULT h2=tl?tl(ss,&b,nullptr,0):ss->LockRect(&b,nullptr,0);
            if(SUCCEEDED(h2)) {
                if(a.pBits&&b.pBits&&a.Pitch>=int(w*bpp)&&b.Pitch>=int(w*bpp)) {
                    for(UINT y=0;y<h;++y) memcpy(static_cast<unsigned char*>(b.pBits)+size_t(y)*b.Pitch,static_cast<unsigned char*>(a.pBits)+size_t(y)*a.Pitch,size_t(w)*bpp);
                    copied=true;
                }
                if(tu) tu(ss);else ss->UnlockRect();
            }
            if(su) su(src);else src->UnlockRect();
        }
        HRESULT hr=E_FAIL;bool queued=false;
        if(copied) {hr=orig(self,ss,nullptr,dst,p);queued=SUCCEEDED(hr);}
        EnterCriticalSection(&TextureWatch::stagingLock);
        if(queued) {e->fence->Issue(D3DISSUE_END);e->busy=true;e->issued=++TextureWatch::stagingIssue;TextureWatch::stagingBusy.push_back(e);}
        else {e->busy=false;TextureWatch::stagingFree[TextureWatch::stagingKey(e->w,e->h,e->format)].push_back(e);}
        LeaveCriticalSection(&TextureWatch::stagingLock);
        if(!queued) {InterlockedIncrement(&failed);return NotHandled;} // anything unexpected: the caller's own call, unchanged
        const long long dt=TextureWatch::qpc()-t0;
        InterlockedIncrement(&done);InterlockedExchangeAdd64(&bytes,LONG64(w)*h*bpp);InterlockedExchangeAdd64(&ticks,dt);
        // Safety: a copy must stay far cheaper than the wait it replaces. 64 copies averaging over 2.5 ms = switch the redirection off.
        guardTicks+=dt;
        if(++guardCount>=64) {
            const double avgUs=Clock::micros(guardTicks)/guardCount;
            if(avgUs>2500.) {on=false;log("Source staging: SWITCHED OFF, the copies cost %.0f us each on average (more than the wait they replace)",avgUs);}
            guardTicks=0;guardCount=0;
        }
        return hr;
    }
    static void install() {
        if(installed) return;installed=true;
        if(iniInt(L"SourceStaging",0)==0) {log("Source staging: off (SourceStaging=0)");return;}
        if(!TimedCalls::deviceHooksOn) {log("Source staging: needs the device hooks (DeviceHooks=1): off");return;}
        TextureWatch::sourceStage=&process;on=true;
        log("Source staging: on, in the device's UpdateSurface (system-memory sources of 64 KB or more are copied to a pooled surface first)");
    }
    static void report() {
        if(!installed||!on) return;
        static const double freq=[]{LARGE_INTEGER f;QueryPerformanceFrequency(&f);return double(f.QuadPart);}();
        const LONG c=InterlockedExchange(&calls,0),d=InterlockedExchange(&done,0);const long long t=InterlockedExchange64(&ticks,0),by=InterlockedExchange64(&bytes,0);
        log("Source staging (SourceStaging=1): UpdateSurface calls of the game thread %ld, copied through staging %ld (%.0f KB, %.1f us each, %.0f ms in total), unchanged: surface %ld, pool %ld, format %ld, rect %ld, small %ld, staging pool full %ld; failed %ld, waited for a surface %ld",
            (long)c,(long)d,d?by/1024./d:0.,d?t*1e6/freq/d:0.,t*1e3/freq,(long)InterlockedExchange(&sSurface,0),(long)InterlockedExchange(&sPool,0),
            (long)InterlockedExchange(&sFormat,0),(long)InterlockedExchange(&sRect,0),(long)InterlockedExchange(&sSmall,0),(long)InterlockedExchange(&sPoolFull,0),(long)InterlockedExchange(&failed,0),(long)InterlockedExchange(&blocked,0));
    }
};

// ---------------------------------------------------------------------------------------------
// ReadbackLag (INI ReadbackLag=1, 0.8.63). The 0.8.62 report showed where the biggest wait of the game thread comes from: after EVERY frame the
// wrapper (dxwrapper, m_IDirectDrawSurfaceX::SetRenderTargetShadow, called from its Blt/CopySurface of the game's 1024x768 render target to the
// primary surface) calls GetRenderTargetData into a system-memory surface and locks it at once to stretch the image (PC window and the HUD bars cut
// from the game image). That lock waits until the GPU has finished the whole frame: 4.3 to 6 ms on a 29 ms frame (0.8.62: 5.8 ms per frame in
// "Lock detail", always right after "GetRenderTargetData 3 us ago ON THIS SURFACE from DDRAW.dll+226C2").
// Here the game thread no longer waits: the read-back of frame N goes to a private system-memory surface of ours (asynchronous, a fence follows it)
// and the surface the wrapper locks receives the image of frame N-1 (already finished: a plain 3 MB copy of ~0.2 ms). The image the wrapper stretches
// (PC window, HUD bars, minimap) is therefore one game frame late; the 3D world, the table, the hands, the laser and every texture are untouched.
// The wrapper also pushes that system-memory surface back into the render target at the start of the next frame (UpdateSurface shadow -> render
// target, 3 MB), so that whatever the game blitted into it (cursor, 2D pieces) reaches the render target. That copy is an identity where nothing was
// written, and with the lag it would put the previous frame's image into the render target. 0.8.63.2: the surface is compared with the image we
// filled it with and only the bands the game really changed are sent (nothing when it wrote nothing). The 0.8.63/0.8.63.1 report showed the game
// DOES lock this surface for writing once per frame, which stopped the lag from ever starting; writes are now handled, not forbidden.
// Safety: engaged only after 40 consecutive read-backs of the same (render target, system-memory surface) pair of 1 MB or more, at most one per frame
// (gap >= 4 ms; a short gap restarts the count). Any failure (fence late, copy or comparison failed, two read-backs closer than 4 ms) switches it off
// for that pair with a line in the log (synchronous as in 0.8.62). The first read-back after engaging stays synchronous.
struct ReadbackLag {
    using ReadFn=HRESULT(__stdcall*)(IDirect3DDevice9*,IDirect3DSurface9*,IDirect3DSurface9*);
    static constexpr HRESULT NotHandled=HRESULT(0x7fffffff);
    static constexpr unsigned StreakNeeded=40;
    static constexpr double MinGapMs=4.0;
    static inline bool on=false,installed=false,engaged=false;
    static inline IDirect3DSurface9 *candRt=nullptr,*candDest=nullptr,*curRt=nullptr,*curDest=nullptr,*banRt=nullptr,*banDest=nullptr;
    static inline unsigned candCalls=0,engages=0,stops=0,dirtyRestarts=0;static inline long long candLast=0,lastEngaged=0;
    static inline ComPtr<IDirect3DSurface9> ring[2];static inline ComPtr<IDirect3DQuery9> fence[2];
    static inline bool filled[2]{false,false};static inline int head=0;
    static inline UINT w=0,h=0,bpp=0;static inline D3DFORMAT fmt=D3DFMT_UNKNOWN;
    static inline volatile LONG lagged=0,syncFirst=0,fenceWaits=0,rClean=0,rSame=0,rBanded=0,rUntracked=0,rFail=0;
    static inline volatile LONG64 copyTicks=0,waitTicks=0,rTicks=0,rBytes=0;
    static inline int lastFill=-1;static inline unsigned restoreFrames=0,untrackedLogs=0;
    static inline char lastReason[160]="";
    static bool shapeOk(IDirect3DSurface9* rt,IDirect3DSurface9* dest,UINT& ow,UINT& oh,D3DFORMAT& of,UINT& obpp) {
        D3DSURFACE_DESC a{},b{};
        if(FAILED(rt->GetDesc(&a))||FAILED(dest->GetDesc(&b))) return false;
        if(a.Pool!=D3DPOOL_DEFAULT||!(a.Usage&D3DUSAGE_RENDERTARGET)||b.Pool!=D3DPOOL_SYSTEMMEM) return false;
        if(a.Width!=b.Width||a.Height!=b.Height||a.Format!=b.Format||a.MultiSampleType!=D3DMULTISAMPLE_NONE) return false;
        const UINT bp=TextureWatch::mirrorBpp(a.Format);
        if(!bp||size_t(a.Width)*a.Height*bp<1024u*1024u) return false;
        ow=a.Width;oh=a.Height;of=a.Format;obpp=bp;return true;
    }
    static bool build(IDirect3DDevice9* d) {
        for(int i=0;i<2;++i) {
            ring[i].Reset();fence[i].Reset();filled[i]=false;
            if(FAILED(d->CreateOffscreenPlainSurface(w,h,fmt,D3DPOOL_SYSTEMMEM,&ring[i],nullptr))||FAILED(d->CreateQuery(D3DQUERYTYPE_EVENT,&fence[i]))) return false;
        }
        head=0;return true;
    }
    static void stop(const char* why,bool ban) {
        snprintf(lastReason,sizeof(lastReason),"%s",why);
        if(engaged) {++stops;log("Readback lag: OFF (%s); the read-back is synchronous again%s",why,ban?" for this surface":"");}
        else log("Readback lag: not used (%s)",why);
        if(ban) {banRt=engaged?curRt:candRt;banDest=engaged?curDest:candDest;}
        engaged=false;lastFill=-1;candRt=candDest=nullptr;candCalls=0;TextureWatch::lagWatch=nullptr;
        for(int i=0;i<2;++i) {ring[i].Reset();fence[i].Reset();filled[i]=false;}
    }
    static bool waitFence(IDirect3DQuery9* q,long long& waited) {
        const long long t0=TextureWatch::qpc();
        if(q->GetData(nullptr,0,0)==S_OK) {waited=0;return true;}
        InterlockedIncrement(&fenceWaits);
        const long long limit=t0+static_cast<long long>(0.25*Clock::freq());
        for(;;) {
            const HRESULT r=q->GetData(nullptr,0,D3DGETDATA_FLUSH);
            if(r==S_OK) {waited=TextureWatch::qpc()-t0;return true;}
            if(FAILED(r)||TextureWatch::qpc()>limit) return false;
            SwitchToThread();
        }
    }
    static bool copyTo(IDirect3DSurface9* src,IDirect3DSurface9* dst) {
        auto sl=reinterpret_cast<SurfaceShadows::LockFn>(TextureWatch::surfLock.original(src));auto su=reinterpret_cast<SurfaceShadows::UnlockFn>(TextureWatch::surfUnlock.original(src));
        auto dl=reinterpret_cast<SurfaceShadows::LockFn>(TextureWatch::surfLock.original(dst));auto du=reinterpret_cast<SurfaceShadows::UnlockFn>(TextureWatch::surfUnlock.original(dst));
        D3DLOCKED_RECT a{},b{};
        if(FAILED(sl?sl(src,&a,nullptr,D3DLOCK_READONLY):src->LockRect(&a,nullptr,D3DLOCK_READONLY))) return false;
        bool ok=false;
        if(SUCCEEDED(dl?dl(dst,&b,nullptr,0):dst->LockRect(&b,nullptr,0))) {
            const size_t row=size_t(w)*bpp;
            if(a.pBits&&b.pBits&&size_t(a.Pitch)>=row&&size_t(b.Pitch)>=row) {
                if(a.Pitch==b.Pitch) memcpy(b.pBits,a.pBits,size_t(a.Pitch)*h);
                else for(UINT y=0;y<h;++y) memcpy(static_cast<unsigned char*>(b.pBits)+size_t(y)*b.Pitch,static_cast<unsigned char*>(a.pBits)+size_t(y)*a.Pitch,row);
                ok=true;
            }
            if(du) du(dst);else dst->UnlockRect();
        }
        if(su) su(src);else src->UnlockRect();
        return ok;
    }
    static HRESULT engagedCall(IDirect3DDevice9* d,IDirect3DSurface9* rt,IDirect3DSurface9* dest,ReadFn orig,long long now,bool checkGap) {
        const double gapMs=Clock::micros(now-lastEngaged)/1000.;lastEngaged=now;
        if(checkGap&&gapMs<MinGapMs) {stop("two read-backs of the pair less than 4 ms apart",stops>=2);return NotHandled;}
        lastFill=-1;
        const int cur=head,prev=cur^1;
        if(FAILED(orig(d,rt,ring[cur].Get()))) {stop("the read-back into the private surface failed",true);return NotHandled;}
        fence[cur]->Issue(D3DISSUE_END);filled[cur]=true;head=prev;
        if(!filled[prev]) {InterlockedIncrement(&syncFirst);return NotHandled;} // nothing older yet: this one stays the exact synchronous read-back
        long long waited=0;
        if(!waitFence(fence[prev].Get(),waited)) {stop("the previous read-back did not finish in 250 ms",true);return NotHandled;}
        const long long t0=TextureWatch::qpc();
        if(!copyTo(ring[prev].Get(),dest)) {stop("the copy into the wrapper's surface failed",true);return NotHandled;}
        InterlockedIncrement(&lagged);InterlockedExchangeAdd64(&copyTicks,TextureWatch::qpc()-t0);InterlockedExchangeAdd64(&waitTicks,waited);
        lastFill=prev;TextureWatch::lagDirty=0;
        return D3D_OK;
    }
    static HRESULT handle(IDirect3DDevice9* d,IDirect3DSurface9* rt,IDirect3DSurface9* dest,void* original) {
        if(!on||!rt||!dest||!original) return NotHandled;
        const ReadFn orig=reinterpret_cast<ReadFn>(original);
        const long long now=TextureWatch::qpc();
        if(engaged) {
            if(rt==curRt&&dest==curDest) return engagedCall(d,rt,dest,orig,now,true);
            if(Clock::micros(now-lastEngaged)<3e6) return NotHandled; // another read-back (checks, other surfaces): untouched
            stop("no read-back of the pair for 3 s (surfaces replaced)",false);
        }
        if(rt==banRt&&dest==banDest) return NotHandled;
        if(rt!=candRt||dest!=candDest) {
            UINT bw=0,bh=0,bb=0;D3DFORMAT bf=D3DFMT_UNKNOWN;
            if(!shapeOk(rt,dest,bw,bh,bf,bb)) return NotHandled; // small read-backs (texture checks...) never disturb the candidate
            candRt=rt;candDest=dest;candCalls=1;candLast=now;w=bw;h=bh;fmt=bf;bpp=bb;
            TextureWatch::lagDirty=0;TextureWatch::lagWatch=dest;
            return NotHandled;
        }
        const double gapMs=Clock::micros(now-candLast)/1000.;candLast=now;
        if(gapMs<MinGapMs) {candCalls=1;++dirtyRestarts;return NotHandled;} // two per frame: not the pattern, count again
        if(++candCalls<StreakNeeded) return NotHandled;
        if(!build(d)) {stop("the private surfaces could not be created",true);return NotHandled;}
        engaged=true;curRt=rt;curDest=dest;lastEngaged=now;++engages;
        log("Readback lag: ON for the %ux%u read-back after each game frame (format %u): the wrapper reads the previous frame's image, the GPU wait leaves the game thread",w,h,unsigned(fmt));
        return engagedCall(d,rt,dest,orig,now,false);
    }
    struct Band {LONG x0,y0,x1,y1;};
    // Compares the wrapper's surface with the image we put in it. 'bands' = rectangles (at most 6) holding every difference.
    static bool diff(IDirect3DSurface9* a,IDirect3DSurface9* b,Band* bands,int& nb) {
        nb=0;
        auto al=reinterpret_cast<SurfaceShadows::LockFn>(TextureWatch::surfLock.original(a));auto au=reinterpret_cast<SurfaceShadows::UnlockFn>(TextureWatch::surfUnlock.original(a));
        auto bl=reinterpret_cast<SurfaceShadows::LockFn>(TextureWatch::surfLock.original(b));auto bu=reinterpret_cast<SurfaceShadows::UnlockFn>(TextureWatch::surfUnlock.original(b));
        D3DLOCKED_RECT la{},lb{};
        if(FAILED(al?al(a,&la,nullptr,D3DLOCK_READONLY):a->LockRect(&la,nullptr,D3DLOCK_READONLY))) return false;
        bool ok=false;
        if(SUCCEEDED(bl?bl(b,&lb,nullptr,D3DLOCK_READONLY):b->LockRect(&lb,nullptr,D3DLOCK_READONLY))) {
            const size_t row=size_t(w)*bpp;
            if(la.pBits&&lb.pBits&&size_t(la.Pitch)>=row&&size_t(lb.Pitch)>=row&&bpp==4) {
                ok=true;
                for(UINT y=0;y<h;++y) {
                    const unsigned char* ra=static_cast<const unsigned char*>(la.pBits)+size_t(y)*la.Pitch;const unsigned char* rb=static_cast<const unsigned char*>(lb.pBits)+size_t(y)*lb.Pitch;
                    if(memcmp(ra,rb,row)==0) continue;
                    const uint32_t* pa=reinterpret_cast<const uint32_t*>(ra);const uint32_t* pb=reinterpret_cast<const uint32_t*>(rb);
                    UINT x0=0,x1=w;while(x0<w&&pa[x0]==pb[x0]) ++x0;while(x1>x0&&pa[x1-1]==pb[x1-1]) --x1;
                    if(nb>0&&LONG(y)-bands[nb-1].y1<=8) {Band& c=bands[nb-1];c.y1=LONG(y)+1;c.x0=std::min(c.x0,LONG(x0));c.x1=std::max(c.x1,LONG(x1));}
                    else if(nb<6) bands[nb++]={LONG(x0),LONG(y),LONG(x1),LONG(y)+1};
                    else {Band& c=bands[nb-1];c.y1=LONG(y)+1;c.x0=std::min(c.x0,LONG(x0));c.x1=std::max(c.x1,LONG(x1));}
                }
            }
            if(bu) bu(b);else b->UnlockRect();
        }
        if(au) au(a);else a->UnlockRect();
        return ok;
    }
    // The wrapper's UpdateSurface(shadow -> render target), whole surface, at the start of a frame. The render target already holds the true image of the
    // last frame; the shadow holds that image one frame older plus whatever the game blitted into it. Only the changed bands go to the render target.
    static HRESULT restore(void* device,IDirect3DSurface9* src,const RECT* r,IDirect3DSurface9* dst,const POINT* p,void* original) {
        if(!engaged||src!=curDest||dst!=curRt||r||p||lastFill<0||!original) return NotHandled;
        const int f=lastFill;lastFill=-1;
        const long long t0=TextureWatch::qpc();
        const bool verify=(++restoreFrames%64)==0;
        const bool dirty=TextureWatch::lagDirty!=0;
        if(!dirty&&!verify) {InterlockedIncrement(&rClean);return D3D_OK;} // nothing locked it for writing since the fill
        Band bands[6];int nb=0;
        if(!diff(ring[f].Get(),curDest,bands,nb)) {InterlockedIncrement(&rFail);stop("the comparison of the surface failed",true);return NotHandled;}
        if(!dirty&&nb>0) {InterlockedIncrement(&rUntracked);if(untrackedLogs<4) {++untrackedLogs;log("Readback lag: the read-back surface was changed without a write lock (%d band(s)); it is compared every frame from now on",nb);}restoreFrames=63;}
        InterlockedExchangeAdd64(&rTicks,TextureWatch::qpc()-t0);
        if(nb==0) {InterlockedIncrement(&rSame);return D3D_OK;}
        auto orig=reinterpret_cast<WrapperUpload::UpdateFn>(original);
        for(int i=0;i<nb;++i) {
            const RECT rc{bands[i].x0,bands[i].y0,bands[i].x1,bands[i].y1};const POINT pt{rc.left,rc.top};
            const HRESULT hr=orig(device,src,&rc,dst,&pt);
            if(FAILED(hr)) {InterlockedIncrement(&rFail);return hr;}
            InterlockedExchangeAdd64(&rBytes,LONG64(rc.right-rc.left)*(rc.bottom-rc.top)*bpp);
        }
        InterlockedIncrement(&rBanded);
        return D3D_OK;
    }
    static void install() {
        if(installed) return;installed=true;
        if(iniInt(L"ReadbackLag",0)==0) {log("Readback lag: off (ReadbackLag=0)");return;}
        if(!TimedCalls::deviceHooksOn||iniInt(L"CallTiming",1)==0||!TimedCalls::installed) {log("Readback lag: needs DeviceHooks=1 and CallTiming=1: off");return;}
        TimedCalls::readbackLag=&handle;TextureWatch::restoreHook=&restore;on=true;
        log("Readback lag: armed (engages after %u consecutive per-frame read-backs of one 1 MB+ surface, never if the game writes into it)",StreakNeeded);
    }
    static void report() {
        if(!installed||!on) return;
        static const double freq=[]{LARGE_INTEGER f;QueryPerformanceFrequency(&f);return double(f.QuadPart);}();
        const LONG n=InterlockedExchange(&lagged,0);const long long c=InterlockedExchange64(&copyTicks,0),wt=InterlockedExchange64(&waitTicks,0);const LONG fw=InterlockedExchange(&fenceWaits,0);
        const LONG cl=InterlockedExchange(&rClean,0),sm=InterlockedExchange(&rSame,0),bd=InterlockedExchange(&rBanded,0),un=InterlockedExchange(&rUntracked,0),fl=InterlockedExchange(&rFail,0),wr=InterlockedExchange(&TextureWatch::lagWrites,0);
        const long long rt=InterlockedExchange64(&rTicks,0),rb=InterlockedExchange64(&rBytes,0);
        log("Readback lag (ReadbackLag=1): %s; frames served from the previous read-back %ld (copy %.0f us each), fence waited %ld times (%.0f us in total), synchronous first frames %ld; game write locks on the surface %ld; frame-start restore: nothing written %ld, compared and identical %ld, changed bands sent %ld (%.0f KB, compare %.0f us in total), changed without a write lock %ld, failed %ld; engaged %u time(s), stopped %u time(s), short-gap restarts %u%s%s",
            engaged?"ENGAGED":"not engaged",(long)n,n?c*1e6/freq/n:0.,(long)fw,wt*1e6/freq,(long)InterlockedExchange(&syncFirst,0),(long)wr,(long)cl,(long)sm,(long)bd,rb/1024.,rt*1e6/freq,(long)un,(long)fl,engages,stops,dirtyRestarts,lastReason[0]?", last stop: ":"",lastReason);
    }
};

// ---------------------------------------------------------------------------------------------
// Frame package handed from the game thread to the VR thread (immutable once handed).
struct PackageSnapshot {
    uint32_t copy[Snapshot::Samplers]{};uint32_t shader=0;
    DWORD rs[Snapshot::RS]{},ts[Snapshot::Stages][Snapshot::TS]{},ss[Snapshot::Samplers][Snapshot::SS]{};
};
struct CopyRef {uint32_t id;HANDLE handle;UINT w,h,levels;};
struct ShaderRef {uint32_t id;std::vector<DWORD> code;};
// 0.9.47.33: what the VR thread really does with the damage digits (report 113401: digits still seen mirrored / edge-on from some positions although the
// record side links every digit vertex). Counted for ever, logged once per second while digits are on screen.
struct DigitReplayStats {uint64_t recs=0,verts=0,badView=0,badIndex=0,bare=0,skippedHand=0;double logMs=0;uint32_t samples=0;};
inline DigitReplayStats gDigitStats;

struct Package {
    uint64_t id=0;uint32_t epoch=0;
    std::vector<unsigned char> vertices,indices,screenVerts;std::vector<DrawRecord> records;std::vector<digitnum::DigitGroup> digitGroups;std::vector<PackageSnapshot> snapshots;
    std::vector<CopyRef> copies;std::vector<ShaderRef> shaders;
    std::vector<OverlayVertex> base;              // record-LOCAL
    std::vector<OverlayVertex> skirt;             // 0.8.92: brown faces closing the dungeon cut at the four table edges (table space, like base)
    bool clip=false,ceiling=false;float planes[5][4]{};         // record-LOCAL (fixed-function clip planes are in world space)
    int pedestalSide=0;bool buttonHover=false;         // 0.6.8: minimap pedestal side (0 auto, 1 left, 2 right), laser on its button
    uint32_t screen=0;                              // 0.6.9: wall screen texture (only what lies between the bars)
    int skillIconCount=0;float skillIconX0[8]{},skillIconX1[8]{};   // 0.9.47.41: icons the game draws (fractions of the cut-out bar, left to right)
    uint32_t skillBar=0;float skillAspect=.125f;     // 0.9.47.40: top bar of the possession interface (skill icons) cut from the game's final image
    uint32_t frame=0;                               // 0.6.9.15: game final image (game area, interface size) for the bars
    bool screenShown=false;                         // 0.6.7.1: wall screen (options / menus) visible
    bool screenWhole=true;                          // 0.6.9.8: options (whole area, HUD texture) vs messages (screen texture)
    bool panelWhole=false;                          // 0.8.94.20: notification box / objectives page: same whole band as the game menu, screen layer only
    bool panelUi=false;                             // 0.8.94.22: the middle panel shows the INTERFACE layer itself (rectangle found from its visible pixels)
    bool statsShown=true;                           // 0.8.94.23: the game shows its statistics bar (no empty slot while it is absent)
    HudRegion msgRegion{0,0,0,0};                   // 0.6.9.11: message box rectangle (fractions of the game area)
    float pickDistance=0;                           // 0.6.7: laser distance to the dungeon geometry (0: none)
    Pose table{};float tableWidth=1;              // table pose/width of the request used by this game frame
    bool panning=false;int panHand=0;Vec3 panFrom{}; // hand position whose native pan produced this frame
    bool turning=false;float turnFrom=0;Vec3 pivot{};
    bool stickOn=false;Vec3 stickFrom{};float stickTurnFrom=0; // 0.8.66: virtual stick 'hand' and angle that produced this frame's native camera
    float tableDepth=0;                                // 0.3.2: depth of the table (metres of the recorded table)  // 0.3.1: hands angle whose native rotation produced this frame
    uint32_t desktop=0;UINT desktopWidth=0,desktopHeight=0;HANDLE desktopHandle=nullptr;
    // 0.6.1: transparent interface texture (HUD modules) and the game cursor sprite.
    uint32_t hud=0;HANDLE hudHandle=nullptr;UINT hudWidth=0,hudHeight=0;float hudTop=0;HudSizes hudSizes{};
    uint32_t held=0;HANDLE heldHandle=nullptr;float heldPx=0,heldPy=0,heldRadius=0; // 0.6.5 things in hand (interface pieces)
    bool tipValid=false;float tipRect[4]{};   // 0.8.94.3: hover tooltip rectangle, game pixels relative to the cursor (hand window)
    bool keepValid=false;float keepRect[4]{};  // 0.8.94.31: held creature box (3D pieces at the cursor), game pixels relative to the cursor: never moved
    float heldModelRight=0; // 0.8.29: protect the complete creature from the icon-block shift in 2D fallback.
    HANDLE keeperHandle=nullptr;int keeperW=0,keeperH=0,keeperHotX=0,keeperHotY=0; // 0.6.4 software cursor image
    uint32_t handCount=0;Vec3 handCenter{};float handExtent=0; // 0.6.3: native 3D hand records
    uint32_t cursor=0;float cursorUv[4]{};float cursorW=0,cursorH=0,cursorHotX=0,cursorHotY=0;
    uint32_t world=0,ui=0,rejected=0,mode=0;
    double recordedMs=0,handedMs=0;
    float possUp[3]{};bool possUpOk=false; // 0.8.75: floor normal in the game camera frame when this frame was drawn (normalised), for the trace
    int32_t possCam[3]{};float possCamZ=0;bool possCamOk=false; // 0.8.74: position of the game's camera when this frame was drawn (ints /4096 = tiles)
    float possYaw=0;bool possYawOk=false;           // 0.8.72: heading of the game's own camera when this frame was drawn (read from the game)
    bool cube=false;Vec3 faceO{};Vec3 faceN[6][4]{};unsigned faceMask=63;   // 0.8.99.1: faceMask = faces run this frame (adaptive 360), the others show the normal view in their pyramid   // 0.8.84: 360 view = cube map. cube = the 6 faces (DrawRecord::view 1..6) of this game frame are complete; their vertices are already in the
                                                         // record space of the normal view; face k keeps only its pyramid: dot(faceN[k][i], X - faceO) >= 0 for the 4 planes
    float menuFocal=0,menuCx=0,menuCy=0; // 0.8.90: lens of the menu records (records present = relief)
    std::shared_ptr<const std::vector<uint32_t>> menuDecorLag[3];UINT menuDecorW=0,menuDecorH=0; // 0.9.7: decor of THIS frame for the VR replay check only (CPU 2D path)
    std::shared_ptr<const std::vector<uint32_t>> checkDecor;UINT checkDecorW=0,checkDecorH=0;
    // 0.8.90.8: the decor alone (read back before the 2D interface) of this frame and the 2 previous ones
    std::shared_ptr<const std::vector<uint32_t>> menuUiLayer;UINT menuUiW=0,menuUiH=0;uint64_t menuUiGen=0; // 0.8.95: the menu's 2D interface alone (premultiplied ARGB, game grid), separated on the CPU
    bool menu=false,menuCursor=false; // 0.8.88: menu package (no table, no dungeon): the room is drawn alone, the game screen goes to the front wall
    bool possession=false;Vec3 possPivot{};double contentMs=0; // 0.8.68: first-person world package (turn pivot = possession anchor, time of the game camera it shows)
};

// ---------------------------------------------------------------------------------------------
// Shared copies of the game textures (device A side).
struct TextureCopy {
    uint32_t id=0;ComPtr<IDirect3DTexture9> texture;HANDLE handle=nullptr;UINT w=0,h=0,levels=0;
    uint64_t lastPackage=0;enum State {Live,Retired,Pooled} state=Live;
};
struct CopyCache {
    struct Source {uint32_t copy=0;UINT w=0,h=0,levels=0;D3DFORMAT format=D3DFMT_UNKNOWN;uint32_t version=~0u;uint64_t copiedFrame=0,usedFrame=0;};
    std::unordered_map<uint32_t,std::unique_ptr<TextureCopy>> objects;
    std::unordered_map<IDirect3DBaseTexture9*,Source> sources;
    std::vector<uint32_t> retired,pool;
    uint32_t nextId=0;uint64_t frame=0,package=0,safeBelow=0;bool mips=true;int refreshBudget=0,refreshFrames=180;
    ComPtr<IDirect3DStateBlock9> block;bool blockFailed=false;
    uint64_t statCopies=0,statLevels=0,statCreated=0,statReused=0,statFailures=0,statUnsupported=0,statRefreshes=0,statDirty=0;
    double statMs=0;
    struct Guard {
        IDirect3DDevice9* d=nullptr;CopyCache* cache=nullptr;bool open=false,failed=false;
        ComPtr<IDirect3DSurface9> rt,ds;D3DVIEWPORT9 vp{};RECT scissor{};double started=0;
        bool begin() {
            if(open) return true;
            if(failed) return false;
            started=milliseconds();
            if(!cache->block&&!cache->blockFailed) cache->makeBlock(d);
            if(!cache->block||FAILED(cache->block->Capture())) {failed=true;return false;}
            d->GetRenderTarget(0,&rt);d->GetDepthStencilSurface(&ds);d->GetViewport(&vp);d->GetScissorRect(&scissor);
            d->SetDepthStencilSurface(nullptr);
            d->SetVertexShader(nullptr);d->SetPixelShader(nullptr);d->SetFVF(D3DFVF_XYZRHW|D3DFVF_TEX1);
            const std::pair<D3DRENDERSTATETYPE,DWORD> rs[]={{D3DRS_ZENABLE,D3DZB_FALSE},{D3DRS_ZWRITEENABLE,FALSE},{D3DRS_ALPHABLENDENABLE,FALSE},
                {D3DRS_ALPHATESTENABLE,FALSE},{D3DRS_CULLMODE,D3DCULL_NONE},{D3DRS_STENCILENABLE,FALSE},{D3DRS_SCISSORTESTENABLE,FALSE},
                {D3DRS_COLORWRITEENABLE,15},{D3DRS_FOGENABLE,FALSE},{D3DRS_SRGBWRITEENABLE,FALSE},{D3DRS_CLIPPLANEENABLE,0},
                {D3DRS_FILLMODE,D3DFILL_SOLID},{D3DRS_LIGHTING,FALSE},{D3DRS_DITHERENABLE,FALSE},{D3DRS_WRAP0,0},{D3DRS_SPECULARENABLE,FALSE}};
            for(auto& s:rs) d->SetRenderState(s.first,s.second);
            d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
            d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
            d->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);d->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
            d->SetTextureStageState(0,D3DTSS_RESULTARG,D3DTA_CURRENT);
            d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);d->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
            d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
            d->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_POINT);d->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_POINT);
            d->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_POINT);d->SetSamplerState(0,D3DSAMP_MIPMAPLODBIAS,0);
            d->SetSamplerState(0,D3DSAMP_SRGBTEXTURE,0);
            open=true;return true;
        }
        ~Guard() {
            if(!open) return;
            d->SetRenderTarget(0,rt.Get());d->SetDepthStencilSurface(ds.Get());
            cache->block->Apply();d->SetViewport(&vp);d->SetScissorRect(&scissor);
            cache->statMs+=milliseconds()-started;
        }
    };
    void makeBlock(IDirect3DDevice9* d) {
        // Records exactly the states the copy changes; Capture() then saves only those.
        if(FAILED(d->BeginStateBlock())) {blockFailed=true;log("Texture copies: BeginStateBlock failed");return;}
        d->SetVertexShader(nullptr);d->SetPixelShader(nullptr);d->SetFVF(0);d->SetTexture(0,nullptr);d->SetStreamSource(0,nullptr,0,0);
        for(auto s:{D3DRS_ZENABLE,D3DRS_ZWRITEENABLE,D3DRS_ALPHABLENDENABLE,D3DRS_ALPHATESTENABLE,D3DRS_CULLMODE,D3DRS_STENCILENABLE,
                    D3DRS_SCISSORTESTENABLE,D3DRS_COLORWRITEENABLE,D3DRS_FOGENABLE,D3DRS_SRGBWRITEENABLE,D3DRS_CLIPPLANEENABLE,
                    D3DRS_FILLMODE,D3DRS_LIGHTING,D3DRS_DITHERENABLE,D3DRS_WRAP0,D3DRS_SPECULARENABLE}) d->SetRenderState(s,0);
        for(auto s:{D3DTSS_COLOROP,D3DTSS_COLORARG1,D3DTSS_ALPHAOP,D3DTSS_ALPHAARG1,D3DTSS_TEXCOORDINDEX,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTSS_RESULTARG})
            d->SetTextureStageState(0,s,0);
        d->SetTextureStageState(1,D3DTSS_COLOROP,0);d->SetTextureStageState(1,D3DTSS_ALPHAOP,0);
        for(auto s:{D3DSAMP_ADDRESSU,D3DSAMP_ADDRESSV,D3DSAMP_MAGFILTER,D3DSAMP_MINFILTER,D3DSAMP_MIPFILTER,D3DSAMP_MAXMIPLEVEL,D3DSAMP_MIPMAPLODBIAS,D3DSAMP_SRGBTEXTURE})
            d->SetSamplerState(0,s,0);
        if(FAILED(d->EndStateBlock(&block))||!block) {blockFailed=true;block.Reset();log("Texture copies: EndStateBlock failed");}
    }
    TextureCopy* find(uint32_t id) {auto it=objects.find(id);return it==objects.end()?nullptr:it->second.get();}
    void retire(uint32_t id) {if(auto* c=find(id)) if(c->state==TextureCopy::Live) {c->state=TextureCopy::Retired;retired.push_back(id);}}
    TextureCopy* acquire(IDirect3DDevice9* d,UINT w,UINT h,UINT levels) {
        for(size_t i=0;i<pool.size();++i) {
            auto* c=find(pool[i]);
            if(c&&c->w==w&&c->h==h&&c->levels==levels) {pool[i]=pool.back();pool.pop_back();c->state=TextureCopy::Live;++statReused;return c;}
        }
        auto c=std::make_unique<TextureCopy>();
        HRESULT hr=d->CreateTexture(w,h,levels,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&c->texture,&c->handle);
        if(FAILED(hr)||!c->handle) {
            if(statFailures++<5) log("Texture copy %ux%u levels=%u: CreateTexture(shared) failed %08lX",w,h,levels,(unsigned long)hr);
            return nullptr;
        }
        c->id=++nextId;c->w=w;c->h=h;c->levels=levels;++statCreated;
        auto* raw=c.get();objects[raw->id]=std::move(c);return raw;
    }
    // Called once per game frame (decoupled frames): recycles copies that no package in use can reference.
    void beginFrame(uint64_t currentPackage,uint64_t safe) {
        ++frame;package=currentPackage;safeBelow=safe;refreshBudget=4;
        for(size_t i=0;i<retired.size();) {
            auto* c=find(retired[i]);
            if(!c) {retired[i]=retired.back();retired.pop_back();continue;}
            if(c->lastPackage<safeBelow) {
                c->state=TextureCopy::Pooled;
                if(pool.size()<192) pool.push_back(c->id); else objects.erase(c->id);
                retired[i]=retired.back();retired.pop_back();
            } else ++i;
        }
        if(frame%300==0) {
            for(auto it=sources.begin();it!=sources.end();) {
                if(frame-it->second.usedFrame>600) {retire(it->second.copy);TextureWatch::unwatch(it->first);it=sources.erase(it);}
                else ++it;
            }
        }
    }
    bool copyLevels(IDirect3DDevice9* d,IDirect3DTexture9* src,TextureCopy* dst) {
        struct V {float x,y,z,w,u,v;};
        d->SetTexture(0,src);
        bool ok=true;
        for(UINT level=0;level<dst->levels;++level) {
            ComPtr<IDirect3DSurface9> s;
            if(FAILED(dst->texture->GetSurfaceLevel(level,&s))) {ok=false;break;}
            D3DSURFACE_DESC desc{};s->GetDesc(&desc);
            d->SetRenderTarget(0,s.Get());
            d->SetSamplerState(0,D3DSAMP_MAXMIPLEVEL,level);
            float w=float(desc.Width)-.5f,h=float(desc.Height)-.5f;
            V q[4]={{-.5f,-.5f,0,1,0,0},{w,-.5f,0,1,1,0},{-.5f,h,0,1,0,1},{w,h,0,1,1,1}};
            if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,q,sizeof(V)))) {ok=false;break;}
            ++statLevels;
        }
        d->SetTexture(0,nullptr);
        return ok;
    }
    // Returns the id of an up-to-date copy of 'base' for the current package (0: none).
    uint32_t copyFor(IDirect3DDevice9* d,Guard& guard,IDirect3DBaseTexture9* base) {
        if(!base) return 0;
        auto it=sources.find(base);
        if(it==sources.end()) {
            if(base->GetType()!=D3DRTYPE_TEXTURE) {if(statUnsupported++<3) log("Texture copies: non-2D texture type %d ignored",int(base->GetType()));return 0;}
            TextureWatch::installTexture(static_cast<IDirect3DTexture9*>(base));
            it=sources.emplace(base,Source{}).first;
        }
        Source& s=it->second;auto* tex=static_cast<IDirect3DTexture9*>(base);
        bool firstUse=s.usedFrame!=frame;s.usedFrame=frame;
        bool need=s.copy==0;
        if(firstUse||need) {
            D3DSURFACE_DESC desc{};if(FAILED(tex->GetLevelDesc(0,&desc))) return 0;
            UINT levels=mips?std::min<UINT>(tex->GetLevelCount(),12):1;
            if(desc.Width!=s.w||desc.Height!=s.h||levels!=s.levels||desc.Format!=s.format) {s.w=desc.Width;s.h=desc.Height;s.levels=levels;s.format=desc.Format;need=true;}
        }
        uint32_t version=TextureWatch::watch(base);
        if(!need&&version!=s.version) {need=true;++statDirty;}
        if(!need&&firstUse&&refreshFrames>0&&frame-s.copiedFrame>uint64_t(refreshFrames)&&refreshBudget>0) {--refreshBudget;need=true;++statRefreshes;}
        TextureCopy* current=s.copy?find(s.copy):nullptr;
        if(!need&&current) {current->lastPackage=package;return s.copy;}
        if(!guard.begin()) return current?s.copy:0;
        TextureCopy* target=nullptr;
        if(current&&current->w==s.w&&current->h==s.h&&current->levels==s.levels&&current->lastPackage<safeBelow) target=current;
        else {
            if(current) retire(current->id);
            target=acquire(d,s.w,s.h,s.levels);
            if(!target&&s.levels>1) {
                target=acquire(d,s.w,s.h,1);
                if(target) {mips=false;s.levels=1;log("Texture copies: shared mip chains unavailable, single level used");}
            }
            if(!target) {s.copy=0;return 0;}
        }
        if(!copyLevels(d,tex,target)) {if(statFailures++<5) log("Texture copy draw failed (%ux%u)",s.w,s.h);}
        ++statCopies;
        s.copy=target->id;s.version=version;s.copiedFrame=frame;target->lastPackage=package;
        return s.copy;
    }
    // Back buffer copy for the wall panel (one per package, recycled like the texture copies).
    uint32_t desktop(IDirect3DDevice9* d,IDirect3DSurface9* back,UINT w,UINT h) {
        auto* c=acquire(d,w,h,1);if(!c) return 0;
        ComPtr<IDirect3DSurface9> s;
        if(FAILED(c->texture->GetSurfaceLevel(0,&s))||FAILED(d->StretchRect(back,nullptr,s.Get(),nullptr,D3DTEXF_NONE))) {retire(c->id);return 0;}
        c->lastPackage=package;retire(c->id);return c->id;
    }
    // 0.6.9.15: the game's FINAL image (what the PC screen shows), game area only, at the interface resolution:
    // the VR bars are cut from it (same coordinates as the interface texture) instead of classifying draws.
    uint32_t gameImage(IDirect3DDevice9* d,IDirect3DSurface9* back,const RECT& src,UINT w,UINT h) {
        auto* c=acquire(d,w,h,1);if(!c) return 0;
        ComPtr<IDirect3DSurface9> s;
        if(FAILED(c->texture->GetSurfaceLevel(0,&s))||FAILED(d->StretchRect(back,&src,s.Get(),nullptr,D3DTEXF_LINEAR))) {retire(c->id);return 0;}
        c->lastPackage=package;retire(c->id);return c->id;
    }
    void releaseAll() {objects.clear();sources.clear();retired.clear();pool.clear();block.Reset();blockFailed=false;TextureWatch::clear();}
    void statsLine(char* out,size_t n) {
        snprintf(out,n,"copies=%llu (%llu levels, %.2f ms) new=%llu reused=%llu changed=%llu refreshed=%llu live=%zu pool=%zu retired=%zu sources=%zu failures=%llu mips=%u",
            (unsigned long long)statCopies,(unsigned long long)statLevels,statMs,(unsigned long long)statCreated,(unsigned long long)statReused,
            (unsigned long long)statDirty,(unsigned long long)statRefreshes,objects.size()-pool.size()-retired.size(),pool.size(),retired.size(),sources.size(),
            (unsigned long long)statFailures,mips);
        statCopies=statLevels=statCreated=statReused=statDirty=statRefreshes=0;statMs=0;
    }
};
// Pixel shaders used by world draws (if any): bytecode handed to the VR thread.
struct ShaderCache {
    std::unordered_map<IDirect3DPixelShader9*,uint32_t> ids;std::unordered_map<uint32_t,std::vector<DWORD>> code;uint32_t next=0;
    uint32_t idFor(IDirect3DPixelShader9* p) {
        if(!p) return 0;
        UINT size=0;if(FAILED(p->GetFunction(nullptr,&size))||size<4) return 0;
        auto it=ids.find(p);
        if(it!=ids.end()&&code[it->second].size()*4==size) return it->second;
        std::vector<DWORD> c(size/4);if(FAILED(p->GetFunction(c.data(),&size))) return 0;
        uint32_t id=++next;ids[p]=id;code[id]=std::move(c);return id;
    }
    void clear() {ids.clear();code.clear();}
};

// 0.6.1: stone pedestal under the minimap (column from the floor, slanted plate behind the map).
void coloredBox(std::vector<OverlayVertex>& mesh,const Pose& pose,Vec3 lo,Vec3 hi,DWORD top,DWORD side) {
    Vec3 c[8]={{lo.x,lo.y,lo.z},{hi.x,lo.y,lo.z},{hi.x,hi.y,lo.z},{lo.x,hi.y,lo.z},{lo.x,lo.y,hi.z},{hi.x,lo.y,hi.z},{hi.x,hi.y,hi.z},{lo.x,hi.y,hi.z}};
    for(auto& v:c) v=toWorld(pose,v);
    const int faces[6][4]={{0,1,2,3},{5,4,7,6},{4,0,3,7},{1,5,6,2},{3,2,6,7},{4,5,1,0}};
    for(int f=0;f<6;++f) {DWORD color=(f==1||f==4)?top:side;for(int k:{0,1,2,0,2,3}) mesh.push_back({c[faces[f][k]],color});}
}
// 0.8.93.1: same box with the user's slab texture (UV in metres / 0.5 m per tile); colours are tints (modulated).
// 0.9.6: + outward normal and tangent (U direction, sign of the V direction) for the lit pedestal (room shader with the user's normal map);
// the fixed-function path reads only position / colour / UV (FVF prefix, stride = sizeof).
struct PedVertex {Vec3 p;DWORD c;float u,v;Vec3 n;float t[4];};
inline Vec3 pedUnit(Vec3 a) {const float l=std::sqrt(dot(a,a));return l>1e-9f?scale(a,1.f/l):a;}
void texturedBox(std::vector<PedVertex>& mesh,const Pose& pose,Vec3 lo,Vec3 hi,DWORD top,DWORD side) {
    Vec3 c[8]={{lo.x,lo.y,lo.z},{hi.x,lo.y,lo.z},{hi.x,hi.y,lo.z},{lo.x,hi.y,lo.z},{lo.x,lo.y,hi.z},{hi.x,lo.y,hi.z},{hi.x,hi.y,hi.z},{lo.x,hi.y,hi.z}};
    for(auto& v:c) v=toWorld(pose,v);
    const float dx=hi.x-lo.x,dy=hi.y-lo.y,dz=hi.z-lo.z,T=.5f;
    const float fa[6]={dx,dx,dz,dz,dx,dx},fb[6]={dy,dy,dy,dy,dz,dz};
    const int faces[6][4]={{0,1,2,3},{5,4,7,6},{4,0,3,7},{1,5,6,2},{3,2,6,7},{4,5,1,0}};
    const Vec3 outward[6]={{0,0,-1},{0,0,1},{-1,0,0},{1,0,0},{0,1,0},{0,-1,0}};
    const float cu[4]={0,1,1,0},cv[4]={0,0,1,1};
    for(int f=0;f<6;++f) {
        const DWORD color=(f==1||f==4)?top:side;
        const Vec3 n=pedUnit(rotate(pose.orientation,outward[f]));
        const Vec3 tu=pedUnit(sub(c[faces[f][1]],c[faces[f][0]])),tv=sub(c[faces[f][3]],c[faces[f][0]]);
        const float w=dot(cross(n,tu),tv)>=0?1.f:-1.f;
        // Subdivide only this object's faces: actual edges receive a narrow darker worn band.
        const float eu=std::min(.22f,.008f/std::max(fa[f],.001f)),ev=std::min(.22f,.008f/std::max(fb[f],.001f));
        const float gridU[4]={0,eu,1-eu,1},gridV[4]={0,ev,1-ev,1};
        Vec3 origin=c[faces[f][0]],du=sub(c[faces[f][1]],origin),dv=sub(c[faces[f][3]],origin);
        for(int y=0;y<3;++y)for(int x=0;x<3;++x)for(int k:{0,1,2,0,2,3}) {
            float u=gridU[x+int(cu[k])],v=gridV[y+int(cv[k])];
            float edge=(u==0||u==1||v==0||v==1)?.30f:1.f;
            DWORD tint=0xff000000;for(int shift:{0,8,16})tint|=DWORD(float((color>>shift)&255)*edge)<<shift;
            Vec3 pos=add(origin,add(scale(du,u),scale(dv,v)));
            mesh.push_back({pos,tint,u*fa[f]/T,v*fb[f]/T,n,{tu.x,tu.y,tu.z,w}});
        }
    }
}
// 0.6.8: rusted iron pedestal (colour variations per part) + side button with an arrow on the column.
void buildPedestal(const HudLayout& hud,std::vector<OverlayVertex>& mesh,bool hover=false,std::vector<PedVertex>* tex=nullptr) {
    const Pose& t=hud.pedestalTop;
    // Plate: the minimap lies on its front face (+Z), 3 cm thick. (0.6.9.5: restored on request, removed in 0.6.9.3.)
    auto stone=[&](const Pose& ps,Vec3 lo,Vec3 hi,DWORD ft,DWORD fs,DWORD tt,DWORD ts) {if(tex) texturedBox(*tex,ps,lo,hi,tt,ts);else coloredBox(mesh,ps,lo,hi,ft,fs);};
    stone(t,{-hud.plateW*.5f,-hud.plateH*.5f,-.035f},{hud.plateW*.5f,hud.plateH*.5f,-.004f},0xff5a3522,0xff3b2216,0xffffffff,0xffb8b8b8);
    // Rim of darker iron around the plate.
    stone(t,{-hud.plateW*.5f-.012f,-hud.plateH*.5f-.012f,-.040f},{hud.plateW*.5f+.012f,hud.plateH*.5f+.012f,-.030f},0xff3e2518,0xff2a170e,0xffa0a0a0,0xff808080);
    // Column: vertical, from the floor to under the plate.
    // 0.6.9.6: column top under the back of the plate (it showed through the plate's lower edge).
    Vec3 under=toWorld(t,{0,-hud.plateH*.40f,-.06f});under.y-=.02f;
    float bottom=hud.pedestalFoot.y,topY=under.y;
    if(topY-bottom>.05f) {
        Pose col{yawOnly(t.orientation),{under.x,0,under.z}};
        stone(col,{-.05f,bottom,-.05f},{.05f,topY,.05f},0xff64391f,0xff4a2a17,0xffffffff,0xffc8c8c8);
        stone(col,{-.12f,bottom,-.12f},{.12f,bottom+.035f,.12f},0xff4d2c18,0xff2f1a0f,0xffb0b0b0,0xff909090);
        stone(col,{-.09f,bottom+.035f,-.09f},{.09f,bottom+.06f,.09f},0xff5a3320,0xff3a2114,0xffd0d0d0,0xffa8a8a8);
        // Rust streaks: thin lighter bands on the column.
        for(float y: {bottom+.25f,bottom+.52f,bottom+.71f}) if(y<topY-.06f&&!tex) coloredBox(mesh,col,{-.052f,y,-.052f},{.052f,y+.012f,.052f},0xff7a4424,0xff7a4424);
    }
    if(hud.button) {
        // Button: copper-rust plate with a raised frame and a light arrow towards the side it moves the pedestal to.
        const Pose& b=hud.buttonPose;const float w=hud.buttonW*.5f,h=hud.buttonH*.5f;
        const DWORD face=hover?0xffb0602c:0xff8a4a26,frame=hover?0xff6e3a1c:0xff55301a,arrow=hover?0xfffff0c0:0xffe0a868;
        coloredBox(mesh,b,{-w-.006f,-h-.006f,-.016f},{w+.006f,h+.006f,-.004f},frame,frame);
        coloredBox(mesh,b,{-w,-h,-.008f},{w,h,0.f},face,frame);
        const float sgn=hud.arrowRight?1.f:-1.f,k=w/.05f; // arrow scaled to the (smaller) button
        auto P=[&](float x,float y){return toWorld(b,Vec3{x*sgn*k,y*k,.002f});};
        Vec3 tip=P(.036f,0),a=P(-.004f,.03f),c=P(-.004f,-.03f);
        mesh.push_back({tip,arrow});mesh.push_back({a,arrow});mesh.push_back({c,arrow});
        Vec3 s0=P(-.034f,.011f),s1=P(-.004f,.011f),s2=P(-.004f,-.011f),s3=P(-.034f,-.011f);
        for(Vec3 v:{s0,s1,s2,s0,s2,s3}) mesh.push_back({v,arrow});
    }
}
// 0.6.9: controllers shown as a discreet magic glow (soft additive billboard: halo + core), only while the
// hand is below the top of the dungeon walls (reaching for the table handles).
void buildGlow(const Request& r,float dungeonTop,std::vector<OverlayVertex>& mesh) {
    const Vec3 head=scale(add(r.eyes[0].pose.position,r.eyes[1].pose.position),.5f);
    const float limit=levelTable(r).position.y+std::max(dungeonTop,0.f)+.03f;
    for(int h=0;h<2;++h) {
        if(!(r.handFlags&(h?HandRightValid:HandLeftValid))) continue;
        const Vec3 c=toWorld(r.hands[h],Vec3{0,0,-.03f});
        // 0.9.47.41: the glow of the FREE hand (the one without the keeper hand) stays visible everywhere unless the LEFT HAND ORB option is on HIDE
        const bool freeHand=h==((r.pointerFlags&HudLeftHanded)?1:0);
        if(c.y>limit&&!(freeHand&&!(r.comfortFlags&ComfortHideOrb))) continue;
        const bool grip=r.handFlags&(h?HandRightGrip:HandLeftGrip);
        Vec3 to=sub(head,c);float l=std::sqrt(dot(to,to));if(l<1e-4f) continue;to=scale(to,1/l);
        Vec3 right=cross(Vec3{0,1,0},to);float rl=std::sqrt(dot(right,right));if(rl<1e-4f) continue;right=scale(right,1/rl);
        const Vec3 up=cross(to,right);
        auto disc=[&](float radius,DWORD centre,DWORD edge){
            const int N=20;
            for(int i=0;i<N;++i) {
                const float a0=2*Pi*i/N,a1=2*Pi*(i+1)/N;
                Vec3 p0=add(c,add(scale(right,std::cos(a0)*radius),scale(up,std::sin(a0)*radius)));
                Vec3 p1=add(c,add(scale(right,std::cos(a1)*radius),scale(up,std::sin(a1)*radius)));
                mesh.push_back({c,centre});mesh.push_back({p0,edge});mesh.push_back({p1,edge});
            }};
        // Colours: ARGB, alpha = additive strength. Pale violet-blue, brighter while gripping.
        disc(.045f,grip?0x70b8a8ff:0x48a898ff,0x00402060);
        disc(.014f,grip?0xe0f0e8ff:0xb0e0d8ff,0x207060c0);
    }
}

// 0.8.23: "magic" laser: thin red additive ribbon (halo + core) facing the head, discreet, born a little AFTER the hand (margin), fading in,
// soft pulse and a few slow sparks travelling along it, small glow where it lands. Additive (alpha = strength) like the hand glow.
void buildLaser(const Request& r,std::vector<OverlayVertex>& mesh) {
    const float len=r.rayLength;
    if(!(len>0.01f&&len<20.f&&std::isfinite(len))) return;
    Vec3 d=r.rayDirection;float l=std::sqrt(dot(d,d));if(l<.5f) return;d=scale(d,1/l);
    const bool on=r.pointerFlags&PointerActive;
    const Vec3 head=scale(add(r.eyes[0].pose.position,r.eyes[1].pose.position),.5f);
    const double ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now().time_since_epoch()).count();
    const float pulse=.88f+.12f*std::sin(float(std::fmod(ms,100000.0))*.006f);
    const float margin=.10f,fade=.10f; // metres: nothing before 10 cm from the hand, full strength 20 cm from it
    const float t0=margin,t1=std::max(t0+.02f,len-(on?.004f:0.f));
    if(t1<=t0+.01f) return;
    const float base=(on?.42f:.20f)*pulse;
    // 0.8.85: the laser colour is chosen on the options panel (Request::comfortFlags). 'red' / 'other' keep the old call sites: red = the strength of the
    // colour itself (halo: 255), 'other' = how much it is whitened (30 halo ... 170 bright centre) -> white mix = other/255 * 0.55.
    float tint[3];laserColourRgb((r.comfortFlags&ComfortLaserMask)>>ComfortLaserShift,tint);
    auto colour=[&](float a,float red,float other)->DWORD{ // additive colour: rgb scaled by alpha in the blend
        const int A=std::clamp(int(a*255.f),0,255);const float w=std::clamp(other/255.f*.55f,0.f,.6f),k=red/255.f;
        auto ch=[&](int i){return DWORD(std::clamp((tint[i]*(1.f-w)+255.f*w)*k,0.f,255.f));};
        return (DWORD(A)<<24)|(ch(0)<<16)|(ch(1)<<8)|ch(2);};
    auto ribbon=[&](float halfW,float strength,float red,float other){
        const int N=14;
        auto side=[&](float t)->Vec3{Vec3 pt=add(r.rayOrigin,scale(d,t));Vec3 tw=sub(head,pt);Vec3 s=cross(d,tw);float sl=std::sqrt(dot(s,s));return sl<1e-5f?Vec3{0,0,0}:scale(s,1/sl);};
        for(int i=0;i<N;++i) {
            const float ta=t0+(t1-t0)*float(i)/N,tb=t0+(t1-t0)*float(i+1)/N;
            auto env=[&](float t){float f=std::clamp((t-t0)/fade,0.f,1.f);f=f*f*(3.f-2.f*f);return f*strength;};
            const Vec3 pa=add(r.rayOrigin,scale(d,ta)),pb=add(r.rayOrigin,scale(d,tb)),sa=scale(side(ta),halfW),sb=scale(side(tb),halfW);
            const DWORD ca=colour(env(ta),red,other),cb=colour(env(tb),red,other);
            const DWORD edge=0x00000000u|(colour(0.f,red,other)&0x00ffffffu);
            // three-row strip: transparent edge / bright centre / transparent edge (soft sides)
            const Vec3 a0=sub(pa,sa),a1=add(pa,sa),b0=sub(pb,sb),b1=add(pb,sb);
            mesh.push_back({a0,edge});mesh.push_back({pa,ca});mesh.push_back({pb,cb});
            mesh.push_back({a0,edge});mesh.push_back({pb,cb});mesh.push_back({b0,edge});
            mesh.push_back({pa,ca});mesh.push_back({a1,edge});mesh.push_back({b1,edge});
            mesh.push_back({pa,ca});mesh.push_back({b1,edge});mesh.push_back({pb,cb});
        }
    };
    ribbon(.0065f,base*.55f,255.f,30.f); // soft red halo
    ribbon(.0016f,base*1.15f,255.f,120.f); // thin bright core (slightly warm)
    auto disc=[&](Vec3 c,float radius,DWORD centre,DWORD edge){
        Vec3 to=sub(head,c);float tl=std::sqrt(dot(to,to));if(tl<1e-4f) return;to=scale(to,1/tl);
        Vec3 right=cross(Vec3{0,1,0},to);float rl=std::sqrt(dot(right,right));if(rl<1e-4f) return;right=scale(right,1/rl);
        const Vec3 up=cross(to,right);const int N=12;
        for(int i=0;i<N;++i) {
            const float a0=2*Pi*i/N,a1=2*Pi*(i+1)/N;
            const Vec3 p0=add(c,add(scale(right,std::cos(a0)*radius),scale(up,std::sin(a0)*radius))),p1=add(c,add(scale(right,std::cos(a1)*radius),scale(up,std::sin(a1)*radius)));
            mesh.push_back({c,centre});mesh.push_back({p0,edge});mesh.push_back({p1,edge});
        }};
    // slow sparks travelling from the hand to the target
    for(int i=0;i<4;++i) {
        const float f=float(std::fmod(ms*.00035+float(i)*.25,1.0));const float t=t0+(t1-t0)*f;
        const float fadeIn=std::clamp((t-t0)/fade,0.f,1.f),fadeOut=std::clamp((t1-t)/.08f,0.f,1.f);
        const float a=base*.9f*fadeIn*fadeOut;if(a<.02f) continue;
        disc(add(r.rayOrigin,scale(d,t)),.0035f,colour(a,255.f,150.f),colour(0.f,255.f,60.f));
    }
    if(on) { // landing point: small red glow
        const Vec3 end=add(r.rayOrigin,scale(d,len));
        disc(end,.016f,colour(.34f*pulse,255.f,60.f),colour(0.f,255.f,20.f));
        disc(end,.005f,colour(.85f*pulse,255.f,170.f),colour(.1f,255.f,60.f));
    }
}
// Hand markers (grip pose, -Z along the controller), shared by both render paths.
void buildHands(const Request& request,std::vector<OverlayVertex>& mesh,bool bodies=true) {
    for(int h=0;h<2&&bodies;++h) {
        if(!(request.handFlags&(h?HandRightValid:HandLeftValid))) continue;
        bool grip=request.handFlags&(h?HandRightGrip:HandLeftGrip);
        bool trigger=request.handFlags&(h?HandRightTrigger:HandLeftTrigger);
        DWORD base=grip?0xffffd040:(h?0xffc05040:0xff4070c0);
        DWORD tip=trigger?0xffffffff:(h?0xffff9070:0xff80b0ff);
        const Pose& pose=request.hands[h];
        const float x=.016f,y=.012f,z0=.035f,z1=-.075f;
        Vec3 c[8]={{-x,-y,z0},{x,-y,z0},{x,y,z0},{-x,y,z0},{-x,-y,z1},{x,-y,z1},{x,y,z1},{-x,y,z1}};
        for(auto& v:c) v=toWorld(pose,v);
        const int faces[6][4]={{0,1,2,3},{5,4,7,6},{4,0,3,7},{1,5,6,2},{3,2,6,7},{4,5,1,0}};
        for(int f=0;f<6;++f) {
            DWORD color=f==1?tip:base;const int* q=faces[f];
            for(int k:{0,1,2,0,2,3}) mesh.push_back({c[q[k]],color});
        }
    }
    // 0.6.0: laser pointer (host ray). Bright green with an end marker on a HUD panel, dim otherwise.
    const float len=request.rayLength;
    if(false&&len>0.01f&&len<20.f&&std::isfinite(len)) { // 0.8.23: old green box laser replaced by buildLaser (additive red ribbon)
        Vec3 d=request.rayDirection;float l=std::sqrt(dot(d,d));
        if(l>.5f) {
            d=scale(d,1/l);
            const bool on=request.pointerFlags&PointerActive;
            Vec3 up=std::fabs(d.y)<.9f?Vec3{0,1,0}:Vec3{1,0,0};
            Vec3 a=cross(d,up);a=scale(a,1/std::sqrt(dot(a,a)));Vec3 b=cross(a,d);
            auto box=[&](Vec3 o,float z0,float z1,float r,DWORD base,DWORD tip){
                Vec3 c[8];
                const float sx[4]={-1,1,1,-1},sy[4]={-1,-1,1,1};
                for(int i=0;i<4;++i) {
                    Vec3 s=add(scale(a,sx[i]*r),scale(b,sy[i]*r));
                    c[i]=add(o,add(s,scale(d,z0)));c[i+4]=add(o,add(s,scale(d,z1)));
                }
                const int faces[6][4]={{3,2,1,0},{4,5,6,7},{0,1,5,4},{1,2,6,5},{2,3,7,6},{3,0,4,7}};
                for(int f=0;f<6;++f) {DWORD color=f==1?tip:base;for(int k:{0,1,2,0,2,3}) mesh.push_back({c[faces[f][k]],color});}
            };
            DWORD color=on?0xff40ff60:0xff2a7a34;
            box(request.rayOrigin,0,len,on?.0018f:.0012f,color,color);
            if(on) box(request.rayOrigin,len-.006f,len,.006f,0xffb0ffb0,0xffffffff);
        }
    }
}

#include "Environment.inl"
// Environment=0: simple markers at the four handle grab points (same points as the host, TableModel.hpp).
void buildHandles(const Request& r,float depthRatio,std::vector<OverlayVertex>& mesh) {
    if(!r.tableAnchor) return;
    TableModel m=tableModel(r.tableWidthMeters,depthRatio);
    for(int i=0;i<4;++i) {
        Vec3 c=m.handle(i);bool hover=false;
        for(int h=0;h<2;++h) if(r.handFlags&(h?HandRightValid:HandLeftValid)) {
            Vec3 d=sub(r.hands[h].position,toWorld(r.table,c));if(dot(d,d)<TableModel::GrabRadius*TableModel::GrabRadius) hover=true;
        }
        DWORD color=hover?0xffd8dce4:0xff6a7078;
        Vec3 half=i<2?Vec3{.006f,.006f,.045f}:Vec3{.045f,.006f,.006f};
        Vec3 lo=sub(c,half),hi=add(c,half);
        Vec3 v[8]={{lo.x,lo.y,lo.z},{hi.x,lo.y,lo.z},{hi.x,hi.y,lo.z},{lo.x,hi.y,lo.z},{lo.x,lo.y,hi.z},{hi.x,lo.y,hi.z},{hi.x,hi.y,hi.z},{lo.x,hi.y,hi.z}};
        for(auto& p:v) p=toWorld(r.table,p);
        const int faces[6][4]={{0,1,2,3},{5,4,7,6},{4,0,3,7},{1,5,6,2},{3,2,6,7},{4,5,1,0}};
        for(auto& f:faces) for(int k:{0,1,2,0,2,3}) mesh.push_back({v[f[k]],color});
    }
}
// ---------------------------------------------------------------------------------------------
// VR thread (device B).
// ---------------------------------------------------------------------------------------------
// 0.8.55: motion interpolation of the recorded geometry (VR thread only, option MotionInterpolation).
// The game produces ~20-30 images/s while the headset needs 72-90. Between two game frames every
// draw whose vertices moved is displayed at a position blended from the previous game frame to the
// new one, so moving content (creatures, projectiles, water...) advances at the headset rate.
// Nothing is changed in what is drawn (same meshes, textures, states): only vertex positions.
// Disabled automatically for a package when the camera moved (pan/turn/scroll), when the layout
// changed too much, or when a record cannot be matched to the previous game frame.
struct MotionInterp {
    bool enabled=false;float snapDist=.15f,cameraDist=.001f,shrink=.9f;
    std::vector<unsigned char> display;                 // full copy of the displayed vertices of 'cur'
    struct Mov {uint32_t voff,count,stride,startIdx;};
    std::vector<Mov> moving;std::vector<Vec3> startPos;
    bool active=false;double adoptMs=0,delta=50;uint64_t curId=0;
    std::shared_ptr<const Package> prev;                // layout the 'display' buffer had before adoption
    // statistics
    uint64_t nAdopt=0,nActive=0,nCamera=0,nReset=0,nMovRec=0,nMovVert=0,nSnap=0,nUnmatched=0,nStaticRec=0,nUpdates=0;double adoptCostMs=0,updateCostMs=0;

    static uint64_t fnv(uint64_t h,const void* d,size_t n) {const unsigned char* b=static_cast<const unsigned char*>(d);for(size_t i=0;i<n;++i) {h^=b[i];h*=1099511628211ull;}return h;}
    static size_t indexCount(const DrawRecord& r) {
        switch(r.type) {
            case D3DPT_TRIANGLELIST:return size_t(r.primitives)*3;case D3DPT_TRIANGLESTRIP:case D3DPT_TRIANGLEFAN:return size_t(r.primitives)+2;
            case D3DPT_LINELIST:return size_t(r.primitives)*2;case D3DPT_LINESTRIP:return size_t(r.primitives)+1;default:return r.primitives;
        }
    }
    static uint64_t signature(const Package& p,const DrawRecord& r) {
        uint64_t h=1469598103934665603ull;
        const uint32_t k[8]={uint32_t(r.type),r.primitives,r.count,r.stride,uint32_t(r.fvf),uint32_t(r.indexed),uint32_t(r.indexFormat),0};
        h=fnv(h,k,sizeof(k));
        if(r.indexed) {
            const size_t bytes=indexCount(r)*(r.indexFormat==D3DFMT_INDEX32?4:2);
            if(r.indices+bytes<=p.indices.size()) h=fnv(h,p.indices.data()+r.indices,bytes);
        }
        return h;
    }
    static bool usable(const Package& p,const DrawRecord& r) {
        return r.view==0&&!r.hand&&!r.held3d&&!r.overlay&&r.stride>=12&&r.count>0&&r.vertices+size_t(r.count-1)*r.stride+12<=p.vertices.size();
    }
    static Vec3 posAt(const unsigned char* base,const DrawRecord& r,uint32_t i) {Vec3 v;memcpy(&v,base+r.vertices+size_t(i)*r.stride,12);return v;}
    static Vec3 centroid(const unsigned char* base,const DrawRecord& r) {
        double x=0,y=0,z=0;const uint32_t step=std::max<uint32_t>(1,r.count/64);uint32_t n=0;
        for(uint32_t i=0;i<r.count;i+=step) {Vec3 v=posAt(base,r,i);x+=v.x;y+=v.y;z+=v.z;++n;}
        return {float(x/n),float(y/n),float(z/n)};
    }
    void reset(const Package& cur) {
        active=false;moving.clear();startPos.clear();
        display.assign(cur.vertices.begin(),cur.vertices.end());
    }
    // Called when 'cur' replaces the previous package (display holds what was last shown, laid out as 'prev').
    void adopt(const std::shared_ptr<const Package>& curp) {
        if(!enabled) return;
        const double t0=milliseconds();const Package& cur=*curp;++nAdopt;
        if(adoptMs>0) {const double dt=t0-adoptMs;if(dt>8&&dt<250) delta=delta*.8+dt*.2;}
        const std::shared_ptr<const Package> pp=prev;
        const bool layoutOk=pp&&display.size()==pp->vertices.size()&&pp->epoch==cur.epoch&&cur.id>pp->id&&cur.id-pp->id<=3&&
            !cur.panning&&!cur.turning&&!pp->panning&&!pp->turning&&cur.mode==pp->mode&&std::fabs(cur.tableWidth-pp->tableWidth)<1e-4f;
        adoptMs=t0;curId=cur.id;
        if(!layoutOk) {++nReset;reset(cur);prev=curp;return;}
        const Package& pv=*pp;
        std::unordered_map<uint64_t,std::vector<uint32_t>> buckets;
        buckets.reserve(pv.records.size()*2);
        std::vector<Vec3> pc(pv.records.size());
        for(uint32_t j=0;j<pv.records.size();++j) if(usable(pv,pv.records[j])) {buckets[signature(pv,pv.records[j])].push_back(j);pc[j]=centroid(display.data(),pv.records[j]);}
        std::vector<char> used(pv.records.size(),0);
        struct Match {uint32_t ci,pj;float maxD,avgD;};std::vector<Match> matches;matches.reserve(cur.records.size());
        for(uint32_t ci=0;ci<cur.records.size();++ci) {
            const DrawRecord& r=cur.records[ci];if(!usable(cur,r)) continue;
            auto it=buckets.find(signature(cur,r));if(it==buckets.end()) {++nUnmatched;continue;}
            const Vec3 c=centroid(cur.vertices.data(),r);int best=-1;float bd=1e30f;int scanned=0;
            for(uint32_t j:it->second) {
                if(used[j]) continue;if(++scanned>256) break;
                const float dx=pc[j].x-c.x,dy=pc[j].y-c.y,dz=pc[j].z-c.z,d=dx*dx+dy*dy+dz*dz;
                if(d<bd) {bd=d;best=int(j);}
            }
            if(best<0) {++nUnmatched;continue;}
            used[best]=1;
            const DrawRecord& q=pv.records[best];float mx=0;double sum=0;
            for(uint32_t i=0;i<r.count;++i) {
                const Vec3 a=posAt(display.data(),q,i),b=posAt(cur.vertices.data(),r,i);
                const float d=std::sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)+(a.z-b.z)*(a.z-b.z));
                mx=std::max(mx,d);sum+=d;
            }
            matches.push_back({ci,uint32_t(best),mx,float(sum/r.count)});
        }
        // Camera moved? median of the mean displacement of the big records.
        std::vector<float> big;for(auto& m:matches) if(cur.records[m.ci].count>=24) big.push_back(m.avgD);
        bool cameraMoved=false;
        if(big.size()>=4) {std::nth_element(big.begin(),big.begin()+big.size()/2,big.end());cameraMoved=big[big.size()/2]>cameraDist;}
        if(cameraMoved) {++nCamera;reset(cur);prev=curp;return;}
        // Start positions = what was displayed; destination = the new game frame.
        std::vector<unsigned char> next(cur.vertices);
        moving.clear();startPos.clear();
        for(auto& m:matches) {
            const DrawRecord& r=cur.records[m.ci];const DrawRecord& q=pv.records[m.pj];
            if(m.maxD<1e-4f) {++nStaticRec;continue;}
            if(m.maxD>snapDist) {++nSnap;continue;}
            Mov mv{uint32_t(r.vertices),r.count,r.stride,uint32_t(startPos.size())};
            for(uint32_t i=0;i<r.count;++i) startPos.push_back(posAt(display.data(),q,i));
            moving.push_back(mv);++nMovRec;nMovVert+=r.count;
            // start from the old position at alpha=0 (continuity with what was displayed)
            for(uint32_t i=0;i<r.count;++i) memcpy(next.data()+r.vertices+size_t(i)*r.stride,&startPos[mv.startIdx+i],12);
        }
        display.swap(next);prev=curp;active=!moving.empty();if(active) ++nActive;
        adoptCostMs+=milliseconds()-t0;
    }
    // Per headset frame: positions blended between the previous and the new game frame.
    void update(const Package& cur,double now) {
        if(!enabled||!active||cur.id!=curId) return;
        const double t0=milliseconds();
        const float a=float(std::clamp((now-adoptMs)/(std::max(15.,delta)*shrink),0.,1.));
        unsigned char* d=display.data();const unsigned char* c=cur.vertices.data();
        for(const Mov& mv:moving) {
            for(uint32_t i=0;i<mv.count;++i) {
                const size_t o=mv.voff+size_t(i)*mv.stride;
                Vec3 e;memcpy(&e,c+o,12);const Vec3& s=startPos[mv.startIdx+i];
                const Vec3 v{s.x+(e.x-s.x)*a,s.y+(e.y-s.y)*a,s.z+(e.z-s.z)*a};
                memcpy(d+o,&v,12);
            }
        }
        ++nUpdates;updateCostMs+=milliseconds()-t0;
    }
    const unsigned char* vertices(const Package& cur) const {return (enabled&&curId==cur.id&&display.size()==cur.vertices.size())?display.data():cur.vertices.data();}
};

// 0.8.65.1 DIAGNOSTIC (INI MotionProbe=1): what part of a game frame is the same as the previous one, and does the whole dungeon move as ONE rigid
// translation when the map is dragged? Answers "could the static terrain be computed once and moved at the headset rate?" (see the log line 'Motion probe').
struct MotionProbe {
    bool enabled=true;
    uint64_t switches=0,compared=0,staticFrames=0,movingFrames=0,panFrames=0;
    uint64_t sRec=0,sRecSame=0,sVert=0,sVertSame=0;                 // camera static: draws / vertices identical to the previous frame
    uint64_t mBig=0,mBigCoherent=0,mVert=0,mVertMatched=0;           // camera moving: big draws matched, moving by the same vector as the median
    double sumDisp=0,sumHand=0,costMs=0,sumMovingShare=0;
    void compare(const Package& pv,const Package& cur,const Pose& tableNow,float ratio) {
        ++switches;
        if(!enabled||pv.epoch!=cur.epoch||cur.id<=pv.id||cur.id-pv.id>3||pv.mode!=cur.mode||cur.records.empty()||pv.records.empty()) return;
        const double t0=milliseconds();
        std::unordered_map<uint64_t,std::vector<uint32_t>> buckets;buckets.reserve(pv.records.size()*2);
        std::vector<Vec3> pc(pv.records.size());
        for(uint32_t j=0;j<pv.records.size();++j) if(MotionInterp::usable(pv,pv.records[j])) {buckets[MotionInterp::signature(pv,pv.records[j])].push_back(j);pc[j]=MotionInterp::centroid(pv.vertices.data(),pv.records[j]);}
        std::vector<char> used(pv.records.size(),0);
        struct M {uint32_t ci;Vec3 d;bool same;};std::vector<M> matches;matches.reserve(cur.records.size());
        uint64_t recs=0,verts=0;
        for(uint32_t ci=0;ci<cur.records.size();++ci) {
            const DrawRecord& r=cur.records[ci];if(!MotionInterp::usable(cur,r)) continue;
            ++recs;verts+=r.count;
            auto it=buckets.find(MotionInterp::signature(cur,r));if(it==buckets.end()) continue;
            const Vec3 c=MotionInterp::centroid(cur.vertices.data(),r);int best=-1;float bd=1e30f;int scanned=0;
            for(uint32_t j:it->second) {
                if(used[j]) continue;if(++scanned>128) break;
                const float dx=pc[j].x-c.x,dy=pc[j].y-c.y,dz=pc[j].z-c.z,d=dx*dx+dy*dy+dz*dz;
                if(d<bd) {bd=d;best=int(j);}
            }
            if(best<0) continue;
            used[best]=1;const DrawRecord& q=pv.records[best];
            const bool same=q.count==r.count&&!memcmp(pv.vertices.data()+q.vertices,cur.vertices.data()+r.vertices,size_t(r.count)*r.stride);
            matches.push_back({ci,Vec3{c.x-pc[best].x,c.y-pc[best].y,c.z-pc[best].z},same});
        }
        // Camera moved? median displacement of the big matched draws.
        std::vector<float> mx,my,mz,mag;
        for(auto& m:matches) if(cur.records[m.ci].count>=24) {mx.push_back(m.d.x);my.push_back(m.d.y);mz.push_back(m.d.z);mag.push_back(std::sqrt(m.d.x*m.d.x+m.d.y*m.d.y+m.d.z*m.d.z));}
        ++compared;
        if(mag.size()<4) {costMs+=milliseconds()-t0;return;}
        auto med=[](std::vector<float>& v){std::nth_element(v.begin(),v.begin()+v.size()/2,v.end());return v[v.size()/2];};
        const Vec3 md{med(mx),med(my),med(mz)};const float mm=std::sqrt(md.x*md.x+md.y*md.y+md.z*md.z);
        const bool moving=med(mag)>0.001f;
        if(!moving) {
            ++staticFrames;sRec+=recs;verts=std::max<uint64_t>(verts,1);sVert+=verts;
            for(auto& m:matches) if(m.same) {++sRecSame;sVertSame+=cur.records[m.ci].count;}
        } else {
            ++movingFrames;uint64_t matchedVerts=0;
            const float tol=std::max(.002f,.05f*mm);
            for(auto& m:matches) {
                const DrawRecord& r=cur.records[m.ci];matchedVerts+=r.count;
                if(r.count<24) continue;
                ++mBig;
                const float ex=m.d.x-md.x,ey=m.d.y-md.y,ez=m.d.z-md.z;
                if(std::sqrt(ex*ex+ey*ey+ez*ez)<=tol) ++mBigCoherent;
            }
            mVert+=std::max<uint64_t>(verts,1);mVertMatched+=matchedVerts;
            if(pv.panning&&cur.panning&&pv.panHand==cur.panHand) {
                const Vec3 shift=panInTable(sub(cur.panFrom,pv.panFrom),tableNow,ratio,1e9f);
                sumDisp+=mm;sumHand+=std::sqrt(double(shift.x)*shift.x+double(shift.z)*shift.z);++panFrames;
            }
        }
        costMs+=milliseconds()-t0;
    }
    void report() {
        if(!enabled||!compared) {*this=MotionProbe{enabled};return;}
        log("Motion probe (0.8.65.1, last 5 s): game frames compared %llu (cost %.2f ms each) | camera static %llu frames: draws identical to the previous frame %.1f%% (vertices %.1f%% of %.0f per frame) | camera moving %llu frames: vertices of draws found again %.1f%%, big draws moving as ONE rigid translation %.1f%% of %llu | while dragging %llu frames: dungeon moved %.3f game units per metre of hand motion",
            (unsigned long long)compared,costMs/std::max<uint64_t>(1,compared),(unsigned long long)staticFrames,sRec?100.*sRecSame/sRec:0.,sVert?100.*sVertSame/sVert:0.,staticFrames?double(sVert)/staticFrames:0.,
            (unsigned long long)movingFrames,mVert?100.*mVertMatched/mVert:0.,mBig?100.*mBigCoherent/mBig:0.,(unsigned long long)mBig,(unsigned long long)panFrames,sumHand>1e-6?sumDisp/sumHand:0.);
        *this=MotionProbe{enabled};
    }
};

// 0.8.69: first-person world seen at the headset rate. The game produces one world package per game frame (58-115/s, irregular) and
// its turn advances in STEPS: the host raises the commanded turn at 90 Hz, the game samples it at its own irregular frames (+-1 degree
// of stair on the image, and +-2 degrees before 0.8.69 because the host integrated with a 15.6 ms clock). 0.8.68 carried the last
// measured rate beyond each frame; a frame that was 'late' by one step then sent the world BACK and the next one forward = the
// constant tremor reported on 000051. Now:
//  * TURN: the VR thread knows the commanded turn C(t) at the headset rate (smooth). The game's yaw per frame is measured on the
//    recorded geometry (same draws found again in both frames, rigid fit about the creature = the possession anchor) and chained;
//    gain g (game yaw per commanded yaw) and a slowly filtered offset o = Phi_k - g*C(t_k) are learned, and the world is shown at
//    S = g*C(now) + o: it follows the stick smoothly, never goes backwards by itself and lags the stick like the game frames do.
//  * WALK / HEIGHT (0.8.70): the game moves the creature in steps of ~25-30 Hz (report 002106: 24 mm of roughness while walking against
//    0.9 mm standing still). A critically damped follower of the chained position, with a lead of one time constant (no extra lag at
//    constant speed, no overshoot at the stop), turns the steps into a continuous walk. The vertical motion (walk bob) is measured on
//    the same matched draws and smoothed the same way (PossessionStabilizeHeight).
//  * PossessionTrace: a CSV of the game frames and headset frames (DK2VR-possession.csv) lets the next analysis see the real stairs.
// Constant errors of the chain cancel (only 'shown pose - content pose' is applied).
struct PossessionMotion {
    bool predictTurn=true,predictMove=true,stabilizeHeight=true,trace=false;float smoothMs=100,moveSmoothMs=45,swaySmoothMs=150,heightSmoothMs=120; // smoothMs: time constant of the turn offset (ms)
    std::shared_ptr<const Package> prev;
    bool valid=false;
    // absolute pose of the world AT THE CURRENT PACKAGE: L = R(yaw)(X - P) (record-local, about the possession anchor)
    double chainYaw=0,chainY=0;Vec3 chainPos{};
    double lsYaw=0,lsY=0;Vec3 lsPos{};   // the pose shown by the last extrapolate() (chain pose when nothing is predicted)
    // turn: learned gain / offset;  walk: smooth position (critically damped follower of the chain) and its velocity
    // 0.8.71: offset = critically damped follower (state offset / velocity offV) of the target offTarget = Phi_k - gain*C(t_k), moved at the
    // HEADSET rate (0.8.70 moved it in one step at every game frame: a small saccade at the game rate); the gain is a ratio of sums over ok
    // fits only (decayed over ~0.8 s) and a change of gain never moves the picture (offset compensated by dGain*C)
    double gain=2.4,offset=0,offV=0,offTarget=0;bool offsetValid=false;double lastObsMs=0;
    double gSumPhi=0,gSumCmd=0;
    // 0.8.72: the game's own camera heading (Package::possYaw) gives the yaw step of every game frame EXACTLY (no stair guess when the picture
    // cannot be matched, e.g. a view of almost nothing: 8-76 draws). yawScale (sign and scale: world rotation per heading step) is learned on the
    // fitted frames and used only if both agree (correlation > 0.95); it is dropped again if later fits disagree.
    bool useYaw=true,yawLearned=false;double yawScale=0,yXY=0,yXX=0,yYY=0,yawBad=0;
    // 0.8.74: the game's own camera POSITION (Package::possCam, tiles) gives the walk of EVERY game frame exactly, also when the picture has too
    // few draws to be matched (report 011103: the view of an almost empty place: the walk animation passed unfiltered = nausea). The link
    // 'translation of the picture = a x (position step rotated by the new heading)' (a: complex constant, 2 candidate handednesses) is learned on
    // the fitted frames (correlation > 0.95), the height the same way (one scalar, 2 candidate sources); both are dropped if later fits disagree.
    int camN=0;bool useCam=true,camLearned=false;int camModel=0;double camAr=0,camAi=0,camBad=0;
    struct CamSum {double sr=0,si=0,suu=0,svv=0;};CamSum cs[2];
    bool zLearned=false;int zModel=0;double zK=0,zBad=0;double zxy[2]{},zxx[2]{},zyy[2]{};
    double stableSwayLead=.5,stableForwardLead=1.,stableHeightLead=0.;   // lead (in time constants) of the walk followers
    // 0.8.80 CLEAN CAMERA v2 (PossessionCleanCamera=1). Report 102307 (0.8.76 = a heavy low-pass): the game's camera sways at ~0.6 Hz while the creature walks
    // (heading +-2.8 deg, roll +-2.8 deg, sideways/forward +-0.2 m) and a low-pass leaves a DELAYED part of that sway (the 'big latency' pitching).
    // So the viewpoint is no longer a filtered copy of the game's camera: it is a KINEMATIC path driven by the walking stick, at the headset rate:
    //  * pFF integrates the learned cruise velocity of the stick direction (8 directions, learned from the game's own walk, gait averaged out), gated by
    //    the stick (no lag, no sway at all);  * the difference to the game's real path is followed slowly (critically damped, cleanPathMs) so that the
    //    view never drifts away from the creature;  * the heading is the stick turn plus a very slow offset (cleanYawMs): the sway of the heading is gone.
    // 0.8.80 (report 104505: 'camera fixe qui suit les sticks et non le jeu'): the heading is ALWAYS viewGain x commanded turn + a very slow offset (gain fixed,
    // never learned in clean mode: the game's own gain varies 1.96..3.19 along the stick) and the GAME is turned towards the view by HeadingFollow (Plugin.cpp);
    // the error view - game is published every headset frame. Directions not learned yet use the walk of the learned ones (the opposite direction negated,
    // a diagonal as the sum of its two neighbours / sqrt 2) until their own block has been measured.
    bool clean=false;float viewGain=2.7f;float cleanFastMs=80,cleanIdleMs=400,cleanPathMs=2000,cleanYawMs=4000,cleanHeightMs=300,cleanGateMs=100;uint64_t fbFrames=0;double headingErrShown=0;
    // 0.8.80 (report 111004: the learned path drifted up to 1.2 m from the creature, then jumped twice; the gait sway of the position was not removed):
    // the position is the game's REAL position through a GAIT NOTCH (band-stop at the walking frequency, tracked on the roll of the game's camera,
    // which is the cleanest gait clock), a lead that gives the notch's delay back, and a small smoother for the 25-30 Hz steps of the game's walk.
    bool cleanNotch=true;float gaitHz=.64f,gaitQ=3.f,gaitLeadMs=50,gaitSmoothMs=40;bool gaitAuto=true;
    Vec3 nLp{},nBp{},nPrev{},nVel{};bool nInit=false;
    double rollMean=0,rollHalfEma=0,rollLastCross=0;int rollSign=0;bool rollInit=false;double f0Now=.64;uint64_t gaitCross=0;
    Vec3 pFF{},qs{},qv{};double ffGate=0,stickGate=0,stickTarget=0;int stickBin=-1;
    struct Bin {double vx=0,vz=0,t=0;bool ok=false;};Bin bins[8];
    double accX=0,accZ=0,accT=0,holdT=0;int learnBin=-1;Vec3 lastChainPos{};bool lastChainOk=false;double vFx=0,vFz=0;double ffGateSum=0;uint64_t ffN=0,ffLearnFrames=0;
    int32_t lastCam[3]{};float lastCamZ=0;bool lastCamOk=false;
    Vec3 sPos{},vPos{};double sY=0,vY=0;double lastMs=0;bool snapNow=false;
    std::string traceBuf;uint64_t traceLines=0;bool traceHeader=false;
    // 0.8.98 INTERPOLATED VIEW (PossessionInterpolate=1). With the steady camera of 0.8.97 the game's own camera path is clean (no bob, no sway, no roll), and
    // its heading / position are read exactly for every game frame. The pose shown at the headset rate is that path INTERPOLATED in time, a little in the past
    // (one game frame interval, ~30 ms): no stick model, no learned gain, no followers. Before, the shown heading was 'gain x stick + offset' and the walk went
    // through followers in the heading frame: corrections of 15 deg rms / 0.15 m rms WHILE TURNING (trace of report 20261005_131654), i.e. the world swinging
    // = the 'tangage' felt when turning. The newest game frame is re-projected to the interpolated pose (rotation about the eye, exact geometry).
    bool interp=true;float interpDelayMs=0;
    // 0.8.99 LOWER LATENCY (report 20261006_094957: delay 45-90 ms = one game frame interval + the arrival + 3 ms, 'decalage dans les controles').
    // The shown time is now 'arrival + interpLead x interval' (0.8.98 = lead 1 + 3 ms): the newest game frame is carried on with its own velocity for up
    // to interpExtrapMax intervals (0.8.98: half an interval, then held). When the next game frame arrives the path it gives can differ a little from the
    // carried-on one: the difference is never shown as a jump, it is put in a correction that fades out in interpCorrectMs (continuous view).
    // 0.8.99.1: DEFAULTS BACK TO 0.8.98 (lead 1, half an interval, no correction, no stick stop). Report 20261006_101213 ('tournez vite puis lachez le stick:
    // sensation etrange, ca bouge toujours'): the game keeps turning about 100 ms after the stick is released (its own response time), so the stick stop
    // cut the carried-on turn too early, and the fading correction then crept to the game's heading for 200-300 ms. Replayed on the real trace of that
    // report: the shown turn reversed direction 2.5 to 5 times more often than 0.8.98 with every low-latency setting tried. Kept as INI options only.
    float interpLead=1.f,interpExtrapMax=.5f,interpCorrectMs=0.f,interpVelocityMs=0.f;bool interpStick=false;
    struct IP {double t,yaw,y;float x,z;};IP ih[32]{};int ihHead=0,ihN=0;double ihDtEma=0;
    uint64_t ipFrames=0,ipExtrap=0,ipHold=0,ipJump=0,ipOld=0;double ipDelaySum=0;
    double icY=0,icH=0;float icX=0,icZ=0;double icLast=0;double icMax=0,icSum=0;uint64_t icN=0;   // 0.8.99: fading correction (radians / metres)
    const IP& ipAt(int k) const {return ih[(ihHead-1-k+64)%32];}   // k = 0: newest game frame
    double ihAgeEma=0;   // time between a game frame's content and its arrival on the VR thread
    void ihPush(double t,double yaw,Vec3 pos,double y,double now) {
        {const double age=std::clamp(now-t,0.,100.);ihAgeEma=ihAgeEma>0?ihAgeEma+.1*(age-ihAgeEma):age;}
        double oY=0,oH=0;Vec3 oP{};bool had=false;
        if(ihN>0) {
            const IP& l=ipAt(0);const double dt=t-l.t;
            if(dt<=.5) return;   // same or older content
            if(dt<200.) ihDtEma=ihDtEma>0?ihDtEma+.1*(dt-ihDtEma):dt;
            const double dx=double(pos.x)-l.x,dz=double(pos.z)-l.z;
            if(std::fabs(yaw-l.yaw)>60.*Pi/180.||std::sqrt(dx*dx+dz*dz)>1.5||dt>=400.) {ihN=0;++ipJump;icY=icH=0;icX=icZ=0;}   // teleport / long gap: never interpolated across
            else if(interpCorrectMs>0&&ihN>=2) had=rawPose(now,oY,oP,oH,false);   // what was shown just before this game frame
        }
        ih[ihHead]={t,yaw,y,pos.x,pos.z};ihHead=(ihHead+1)%32;if(ihN<32) ++ihN;
        if(had) {double nY=0,nH=0;Vec3 nP{};
            if(rawPose(now,nY,nP,nH,false)) {fadeCorrection(now);icY+=oY-nY;icH+=oH-nH;icX+=oP.x-nP.x;icZ+=oP.z-nP.z;
                const double m=std::fabs(oY-nY);icMax=std::max(icMax,m);icSum+=m;++icN;}}
    }
    double shownDelay() const {
        if(interpDelayMs>0) return double(interpDelayMs);
        return std::clamp(ihAgeEma+double(interpLead)*ihDtEma+(interpLead>=1.f?3.:0.),interpLead>=1.f?12.:4.,90.);
    }
    void fadeCorrection(double now) {
        const double dt=icLast>0?std::clamp(now-icLast,0.,200.):0.;icLast=now;
        if(interpCorrectMs<=0) {icY=icH=0;icX=icZ=0;return;}
        const double k=std::exp(-dt/double(interpCorrectMs));icY*=k;icH*=k;icX=float(icX*k);icZ=float(icZ*k);
    }
    // the game camera's path at the time 'now - delay' (interpolated between game frames, or carried on past the newest one)
    bool rawPose(double now,double& Y,Vec3& P,double& H,bool count) {
        if(ihN<2) return false;
        const double tau=now-shownDelay();
        const IP& n0=ipAt(0);
        if(tau>=n0.t) {   // later than the newest game frame: carried on with the path's velocity for at most interpExtrapMax intervals, then held
            // 0.8.99: velocity over the game frames of the last interpVelocityMs (the stick events reach the game frames as a stair: the step of the
            // last two frames alone is noisy when the game runs fast and irregular)
            int k=1;while(k+1<ihN&&n0.t-ipAt(k).t<double(interpVelocityMs)) ++k;
            const IP& n1=ipAt(k);const double span=n0.t-n1.t;
            const double interval=interpLead>=1.f?(n0.t-ipAt(1).t):std::max(1.,ihDtEma);
            double e=tau-n0.t;const double cap=double(interpExtrapMax)*interval;if(e>cap) {e=cap;if(count) ++ipHold;} else if(count) ++ipExtrap;
            const double a=span>0?e/span:0.;
            // 0.8.99: the turn is carried on only as long as the stick still asks for it (ratio of the commanded turn rate after the newest game frame to the
            // one over the velocity window): releasing the stick stops the carried-on turn at once, no overshoot / bounce back when the turn ends
            double turnK=1.;
            if(interpStick&&e>.5&&span>.5) {float c1=0,c0=0,c2=0;
                if(commandAt(n1.t,c1)&&commandAt(n0.t,c0)&&commandAt(n0.t+e,c2)) {const double cg=double(c0-c1)/span,ce=double(c2-c0)/e;
                    if(std::fabs(cg)>1e-4) turnK=std::clamp(ce/cg,0.,1.2);}}
            Y=n0.yaw+(n0.yaw-n1.yaw)*a*turnK;P={float(n0.x+(n0.x-n1.x)*a),0,float(n0.z+(n0.z-n1.z)*a)};H=n0.y;return true;
        }
        for(int k=0;k+1<ihN;++k) {
            const IP& B=ipAt(k);const IP& A=ipAt(k+1);
            if(tau>=A.t&&tau<=B.t) {const double a=B.t>A.t?(tau-A.t)/(B.t-A.t):1.;
                Y=A.yaw+(B.yaw-A.yaw)*a;P={float(A.x+(B.x-A.x)*a),0,float(A.z+(B.z-A.z)*a)};H=A.y+(B.y-A.y)*a;return true;}
        }
        const IP& o=ipAt(ihN-1);Y=o.yaw;P={o.x,0,o.z};H=o.y;if(count) ++ipOld;return true;
    }
    bool interpPose(double now,double& Y,Vec3& P,double& H) {
        if(!rawPose(now,Y,P,H,true)) return false;
        ipDelaySum+=shownDelay();++ipFrames;
        fadeCorrection(now);
        Y+=icY;H+=icH;P.x+=icX;P.z+=icZ;
        return true;
    }
    // live commands (turn stick) for the statistics only
    float lastTurnCmd=0;bool haveTurnCmd=false;
    struct Cmd {double ms;float yaw;};Cmd ring[96]{};int ringHead=0,ringCount=0;
    // statistics (reset every report)
    uint64_t camUsed=0,camDisagree=0,camUnlearned=0,zUsed=0;uint64_t yawUsed=0,yawDisagree=0,knownRot=0,yawUnlearned=0;uint64_t rescued=0,weakOk=0,failFew=0,failOther=0,gainClamped=0;double failPairsSum=0;
    uint64_t frames=0,applied=0,fits=0,fitsOk=0,guessed=0,skippedSnap=0,gaps=0,resets=0,reversalsRaw=0,reversalsShown=0,motionFrames=0,gainUpdates=0;double gainSum=0;
    double pairsSum=0,shareSum=0,rmsSum=0,costMs=0,costMax=0,sumPhi=0,sumCmd=0,exTurnSum=0,exTurnMax=0,exMoveSum=0,exMoveMax=0;uint64_t gainN=0;
    double rawJitter2=0,shownJitter2=0,rawJitterMove2=0,shownJitterMove2=0,rawJitterY2=0,shownJitterY2=0;uint64_t jitterN=0;
    double rawYHist[2]{},shownYHist[2]{};double exYMax=0,yStep2=0;uint64_t yFits=0;
    double rawHist[2]{},shownHist[2]{},lastCmdSeen=0,lastCmdMs=0;Vec3 rawPosHist[2]{},shownPosHist[2]{};int histN=0;
    void clearStats() {camUsed=camDisagree=camUnlearned=zUsed=0;yawUsed=yawDisagree=knownRot=yawUnlearned=0;rescued=weakOk=failFew=failOther=gainClamped=0;failPairsSum=0;frames=applied=fits=fitsOk=guessed=skippedSnap=gaps=resets=reversalsRaw=reversalsShown=motionFrames=gainUpdates=0;gainSum=0;pairsSum=shareSum=rmsSum=costMs=costMax=sumPhi=sumCmd=exTurnSum=exTurnMax=exMoveSum=exMoveMax=0;gainN=0;rawJitter2=shownJitter2=rawJitterMove2=shownJitterMove2=rawJitterY2=shownJitterY2=0;jitterN=0;exYMax=0;yFits=0;}
    void rotateCam(double angle) {   // the chain frame turns by -angle: u -> u e^{+i angle}; conj(u) v -> e^{-i angle}
        const double c=std::cos(-angle),sn=std::sin(-angle);
        for(CamSum& m:cs) {const double r=m.sr*c-m.si*sn,i=m.sr*sn+m.si*c;m.sr=r;m.si=i;}
        const double r=camAr*c-camAi*sn,i=camAr*sn+camAi*c;camAr=r;camAi=i;
    }
    void restart(double now) {
        rotateCam(chainYaw);
        chainYaw=0;chainY=0;chainPos={};offset=0;offsetValid=false;offV=0;offTarget=0;gSumPhi=gSumCmd=0;lastObsMs=0;sPos={};vPos={};sY=0;vY=0;pFF={};qs={};qv={};ffGate=0;vFx=vFz=0;lastChainOk=false;lastMs=now;valid=true;histN=0;nInit=false;ihN=0;icY=icH=0;icX=icZ=0;
    }
    void reset() {ihN=0;icY=icH=0;icX=icZ=0;prev.reset();valid=false;camLearned=false;zLearned=false;for(CamSum& m:cs) m=CamSum();zxy[0]=zxy[1]=zxx[0]=zxx[1]=zyy[0]=zyy[1]=0;camBad=zBad=0;camN=0;chainYaw=0;chainY=0;chainPos={};offset=0;offV=0;offTarget=0;gSumPhi=gSumCmd=0;offsetValid=false;sPos={};vPos={};sY=0;vY=0;histN=0;pFF={};qs={};qv={};ffGate=0;vFx=vFz=0;for(Bin& b:bins) b=Bin();accX=accZ=accT=0;learnBin=-1;lastChainOk=false;nInit=false;rollInit=false;rollHalfEma=0;rollSign=0;f0Now=gaitHz;}   // a new possession: the speed of the creature is learned again
    void noteCommand(const Request& r,double now) {
        lastTurnCmd=r.possessionTurn;haveTurnCmd=true;snapNow=(r.comfortFlags&ComfortSnapTurn)!=0;
        {   // 0.8.80: the walking stick: magnitude (gate) and direction (8 bins)
            const double mag=std::hypot(double(r.possessionMoveX),double(r.possessionMoveY));
            stickTarget=std::clamp(mag*4.,0.,1.);
            stickBin=mag>.25?(int(std::lround(std::atan2(double(r.possessionMoveX),double(r.possessionMoveY))/(Pi/4.)))+8)&7:-1;
        }
        ring[ringHead]={now,r.possessionTurn};ringHead=(ringHead+1)%96;ringCount=std::min(ringCount+1,96);
    }
    bool commandAt(double ms,float& yaw) const {
        // linear interpolation in the ring of the last ~1 s of commanded body turn
        if(ringCount<2) return false;
        Cmd older{},newer{};bool have=false;
        for(int i=0;i<ringCount;++i) {
            const Cmd& c=ring[(ringHead-1-i+96)%96];
            if(c.ms>=ms) {newer=c;continue;}
            older=c;have=newer.ms>0;break;
        }
        if(!have||older.ms<=0) return false;
        const double span=newer.ms-older.ms;const float a=span>1e-6?float((ms-older.ms)/span):0.f;
        yaw=older.yaw+(newer.yaw-older.yaw)*std::clamp(a,0.f,1.f);return true;
    }
    // exact step of a critically damped follower (time constant 1/w) towards a target that is constant during dt
    static void follow(double& s,double& v,double target,double w,double dt) {
        const double e0=s-target,ex=std::exp(-w*dt),c=v+w*e0;
        s=target+(e0+c*dt)*ex;v=(v-w*c*dt)*ex;
    }
    bool fallbackVelocity(int b,double& vx,double& vz) const {   // velocity (heading frame) of a not yet learned stick direction from the learned ones
        const int opp=(b+4)&7;
        if((b&1)==0) {if(bins[opp].ok) {vx=-bins[opp].vx;vz=-bins[opp].vz;return true;}return false;}
        const int a=(b+7)&7,c=(b+1)&7;double ax,az,cx,cz;
        if(bins[a].ok) {ax=bins[a].vx;az=bins[a].vz;} else if(bins[(a+4)&7].ok) {ax=-bins[(a+4)&7].vx;az=-bins[(a+4)&7].vz;} else return false;
        if(bins[c].ok) {cx=bins[c].vx;cz=bins[c].vz;} else if(bins[(c+4)&7].ok) {cx=-bins[(c+4)&7].vx;cz=-bins[(c+4)&7].vz;} else return false;
        vx=(ax+cx)*.70710678;vz=(az+cz)*.70710678;return true;
    }
    void advance(double now) {
        if(!valid) return;
        double dt=(now-lastMs)*.001;lastMs=now;
        if(dt<=0) return;
        dt=std::min(dt,.05);
        if(offsetValid) follow(offset,offV,offTarget,1000./std::max(10.f,clean?cleanYawMs:smoothMs*.5f),dt);
        if(!stabilizeHeight) {sY=chainY;vY=0;} else follow(sY,vY,chainY,1000./std::max(20.f,clean?cleanHeightMs:heightSmoothMs),dt);
        stickGate+=(stickTarget-stickGate)*(1.-std::exp(-dt/.06));
        if(clean&&predictMove&&cleanNotch) {   // 0.8.80: the game's position through the gait notch
            if(!nInit) {nLp=chainPos;nBp={};nPrev=chainPos;nVel={};sPos=chainPos;vPos={};nInit=true;}
            const double w=2.*Pi*std::clamp(gaitAuto?f0Now:double(gaitHz),.3,1.6),Q=std::max(.5f,gaitQ);
            const int n=std::max(1,int(std::ceil(dt*w/.1)));const double h=dt/n;
            double lx=nLp.x,lz=nLp.z,bx=nBp.x,bz=nBp.z;
            for(int i=0;i<n;++i) {bx+=h*w*(chainPos.x-lx-bx/Q);bz+=h*w*(chainPos.z-lz-bz/Q);lx+=h*w*bx;lz+=h*w*bz;}
            nLp={float(lx),0,float(lz)};nBp={float(bx),0,float(bz)};
            const double ox=chainPos.x-bx/Q,oz=chainPos.z-bz/Q;
            const double a=1.-std::exp(-dt/.15);
            nVel.x+=float(a*((ox-nPrev.x)/dt-nVel.x));nVel.z+=float(a*((oz-nPrev.z)/dt-nVel.z));nPrev={float(ox),0,float(oz)};
            const double L=gaitLeadMs*.001;
            const double tx=ox+L*nVel.x,tz=oz+L*nVel.z;
            double sx=sPos.x,sz=sPos.z,vx=vPos.x,vz=vPos.z;const double ws=1000./std::max(10.f,gaitSmoothMs);
            follow(sx,vx,tx,ws,dt);follow(sz,vz,tz,ws,dt);
            sPos={float(sx),0,float(sz)};vPos={float(vx),0,float(vz)};++ffN;
            return;
        }
        if(clean&&predictMove) {   // 0.8.80: kinematic path driven by the stick (see PossessionMotion::clean)
            double fbx=0,fbz=0;const bool learned=stickBin>=0&&bins[stickBin].ok;
            const bool fb=stickBin>=0&&!learned&&fallbackVelocity(stickBin,fbx,fbz);
            const bool ffOn=learned||fb;
            {   // the stick velocity of the path, first-order filtered at the gate time (a direction change or a release is not an instant jump)
                const double tx=learned?bins[stickBin].vx:(fb?fbx:0.),tz=learned?bins[stickBin].vz:(fb?fbz:0.),a=1.-std::exp(-dt*1000./std::max(20.f,cleanGateMs));
                vFx+=(tx-vFx)*a;vFz+=(tz-vFz)*a;ffGate=ffOn?1.:0.;
            }
            const Vec3 vw=planarRotate(float(-chainYaw),float(vFx),float(vFz));pFF.x+=vw.x*float(dt);pFF.z+=vw.z*float(dt);
            if(fb) ++fbFrames;
            const double w=1000./std::max(20.f,learned?cleanPathMs:(fb?cleanPathMs*.4f:(stickBin>=0?cleanFastMs:cleanIdleMs)));   // a stick direction not learned yet follows the game's path closely (as 0.8.75), a learned one is driven by the stick
            double tx=chainPos.x-pFF.x,tz=chainPos.z-pFF.z,sx=qs.x,sz=qs.z,vx=qv.x,vz=qv.z;
            follow(sx,vx,tx,w,dt);follow(sz,vz,tz,w,dt);
            qs={float(sx),0,float(sz)};qv={float(vx),0,float(vz)};
            sPos={pFF.x+qs.x,0,pFF.z+qs.z};vPos={vw.x+qv.x,0,vw.z+qv.z};ffGateSum+=ffGate;++ffN;
            return;
        }
        if(!predictMove) {sPos=chainPos;vPos={};return;}
        // walk in the heading frame of the creature: forward/back (z) follows closely, sideways (x) is smoothed more (walk sway)
        const Vec3 e=planarRotate(float(chainYaw),sPos.x-chainPos.x,sPos.z-chainPos.z),v=planarRotate(float(chainYaw),vPos.x,vPos.z);
        double ex=e.x,vx=v.x,ez=e.z,vz=v.z;
        follow(ex,vx,0.,1000./std::max(4.f,swaySmoothMs),dt);follow(ez,vz,0.,1000./std::max(4.f,moveSmoothMs),dt);
        const Vec3 eb=planarRotate(float(-chainYaw),float(ex),float(ez)),vb=planarRotate(float(-chainYaw),float(vx),float(vz));
        sPos={chainPos.x+eb.x,0,chainPos.z+eb.z};vPos={vb.x,0,vb.z};
    }
    // Called when a new game frame ('curp') replaces the previous one on the VR thread.
    void observe(const std::shared_ptr<const Package>& curp,double now) {observeCore(curp,now);}
    void observeCore(const std::shared_ptr<const Package>& curp,double now) {
        const Package& cur=*curp;
        advance(now);
        if(clean&&cleanNotch&&gaitAuto&&cur.possUpOk) {   // gait clock = the roll of the game's camera (half period between zero crossings of its slow-mean-free part)
            const double roll=std::atan2(double(cur.possUp[0]),double(cur.possUp[1]))*180./Pi,tm=cur.contentMs;
            if(!rollInit) {rollInit=true;rollMean=roll;rollSign=0;rollLastCross=0;}
            const double ahp=1.-std::exp(-.011/2.5);rollMean+=ahp*(roll-rollMean);
            const double hp=roll-rollMean;int sg=rollSign;
            if(hp>.8) sg=1; else if(hp<-.8) sg=-1;
            if(sg!=rollSign&&rollSign!=0) {
                const double iv=(tm-rollLastCross)*.001;
                if(rollLastCross>0&&iv>.25&&iv<1.5) {rollHalfEma=rollHalfEma>0?rollHalfEma*.6+iv*.4:iv;f0Now=std::clamp(.5/rollHalfEma,.3,1.6);++gaitCross;}
                rollLastCross=tm;
            } else if(sg!=0&&rollSign==0) rollLastCross=tm;
            rollSign=sg;
        }
        const std::shared_ptr<const Package> pp=prev;prev=curp;
        if(!pp||!pp->possession||pp->epoch!=cur.epoch||cur.id<=pp->id||cur.id-pp->id>8) {restart(now);return;}
        const double dt=cur.contentMs-pp->contentMs;
        if(dt>150) {++gaps;restart(now);return;}
        const float sec=float(std::max(dt,0.)*.001);
        // what the chain expects (gain x commanded turn, smooth velocity of the walk): used to pair draws and as the fallback when the fit fails
        float cA=0,cB=0;const bool haveC=commandAt(pp->contentMs,cA)&&commandAt(cur.contentMs,cB);
        // measured yaw step of the game's camera (if learned): exact, replaces the guess gain x commanded turn
        const bool haveYaw=useYaw&&yawLearned&&cur.possYawOk&&pp->possYawOk;
        const float rawYawStep=(cur.possYawOk&&pp->possYawOk)?wrapRadians(cur.possYaw-pp->possYaw):0.f;
        const float predPhi=haveYaw?float(yawScale*rawYawStep):(haveC&&!snapNow?float(gain*(cB-cA)):0.f);
        // game camera position step (tiles): the walk of this game frame, exact
        double dcx=0,dcy=0,dcz=0,dcz2=0;bool haveCamPair=useCam&&cur.possCamOk&&pp->possCamOk;
        if(haveCamPair) {
            dcx=(double(cur.possCam[0])-pp->possCam[0])/4096.;dcy=(double(cur.possCam[1])-pp->possCam[1])/4096.;dcz=(double(cur.possCam[2])-pp->possCam[2])/4096.;dcz2=double(cur.possCamZ)-pp->possCamZ;
            if(!(std::fabs(dcx)<3.&&std::fabs(dcy)<3.&&std::fabs(dcz)<3.&&std::fabs(dcz2)<3.)) haveCamPair=false;   // a jump (teleport): not a walk
        }
        const bool haveCam=haveCamPair&&camLearned;
        auto camU=[&](int model,double Phi,double& ur,double& ui) {   // u = w e^{-i Phi}, w = step (x, y) or its mirror image
            const double wr=dcx,wi=model==0?dcy:-dcy,c=std::cos(Phi),sn=std::sin(Phi);ur=wr*c+wi*sn;ui=-wr*sn+wi*c;
        };
        auto camTr=[&](double Phi)->Vec3 {double ur,ui;camU(camModel,Phi,ur,ui);return {float(camAr*ur-camAi*ui),0.f,float(camAr*ui+camAi*ur)};};
        const Vec3 gv{vPos.x*sec,0,vPos.z*sec};
        const Vec3 predT=haveCam?camTr(chainYaw+predPhi):scale(planarRotate(float(chainYaw)+predPhi,gv.x,gv.z),-1.f);
        float phi=predPhi;Vec3 tr=predT;bool ok=false;float ty=0.f;int pairCount=0,hypUsed=-1;bool knownOk=false,tyFit=false,camTrUsed=false,zNow=false;
        if(dt>=2) {
            const double t0=milliseconds();++fits;
            const Vec3 pivot=cur.possPivot;
            std::unordered_map<uint64_t,std::vector<uint32_t>> buckets;buckets.reserve(pp->records.size()*2);
            std::vector<Vec3> pc(pp->records.size()),pcp(pp->records.size());std::vector<uint32_t> usableJ;usableJ.reserve(pp->records.size());
            for(uint32_t j=0;j<pp->records.size();++j) {
                const DrawRecord& r=pp->records[j];
                if(r.count<24||!MotionInterp::usable(*pp,r)) continue;
                buckets[MotionInterp::signature(*pp,r)].push_back(j);
                pc[j]=MotionInterp::centroid(pp->vertices.data(),r);usableJ.push_back(j);
            }
            struct CurDraw {Vec3 c;const std::vector<uint32_t>* list;float w;};
            std::vector<CurDraw> cd;cd.reserve(cur.records.size());
            for(uint32_t ci=0;ci<cur.records.size();++ci) {
                const DrawRecord& r=cur.records[ci];
                if(r.count<24||!MotionInterp::usable(cur,r)) continue;
                auto it=buckets.find(MotionInterp::signature(cur,r));if(it==buckets.end()) continue;
                cd.push_back({MotionInterp::centroid(cur.vertices.data(),r),&it->second,std::sqrt(float(std::min<UINT>(r.count,400)))});
            }
            std::vector<char> used(pp->records.size(),0);
            std::vector<PlanarPair> pairs,bestPairs;std::vector<float> pdy,bestPdy;
            // pairing under one hypothesis of the game's yaw step (hPhi): nearest predicted centroid within 1.5 units, same mesh signature
            auto pairWith=[&](float hPhi)->Vec3 {
                const Vec3 hT=haveCam?camTr(chainYaw+hPhi):scale(planarRotate(float(chainYaw)+hPhi,vPos.x*sec,vPos.z*sec),-1.f);
                for(uint32_t j:usableJ) {
                    const Vec3 q=planarRotate(hPhi,pc[j].x-pivot.x,pc[j].z-pivot.z);
                    pcp[j]={q.x+pivot.x+hT.x,pc[j].y,q.z+pivot.z+hT.z};used[j]=0;
                }
                pairs.clear();pdy.clear();
                for(const CurDraw& d:cd) {
                    int best=-1;float bd=1e30f;int scanned=0;
                    for(uint32_t j:*d.list) {
                        if(used[j]) continue;if(++scanned>128) break;
                        const float dx=pcp[j].x-d.c.x,dy=pcp[j].y-d.c.y,dz=pcp[j].z-d.c.z,dd=dx*dx+dy*dy+dz*dz;
                        if(dd<bd) {bd=dd;best=int(j);}
                    }
                    if(best<0||bd>1.5f*1.5f) continue;
                    used[best]=1;
                    pairs.push_back({pc[best].x-pivot.x,pc[best].z-pivot.z,d.c.x-pivot.x,d.c.z-pivot.z,d.w});
                    pdy.push_back(d.c.y-pc[best].y);
                }
                return hT;
            };
            // 0.8.71: the game's yaw step of one frame is a stair (0 .. 5 degrees against 2 predicted): when the predicted step gives no fit,
            // other steps are tried (no turn, then +-2.3 / 4.6 / 7.4 degrees around the prediction); a fit is accepted only if it is as
            // rigid as before (>= 8 matched draws) and stays close to the prediction.
            float cand[8];int nc=0;cand[nc++]=predPhi;
            auto addc=[&](float h) {for(int i=0;i<nc;++i) if(std::fabs(cand[i]-h)<.012f) return;if(nc<8) cand[nc++]=h;};
            addc(0.f);{const float st[6]={-.04f,.04f,-.08f,.08f,-.13f,.13f};for(float x:st) addc(predPhi+x);}
            int bestN=0;Vec3 bestT=predT;PlanarFit fit;const std::vector<PlanarPair>* fp=&pairs;const std::vector<float>* fy=&pdy;
            if(cd.size()>=5) for(int k=0;k<nc;++k) {
                const Vec3 hT=pairWith(cand[k]);
                if(int(pairs.size())>bestN) {bestN=int(pairs.size());bestPairs=pairs;bestPdy=pdy;bestT=hT;}
                if(pairs.size()<8) continue;
                const PlanarFit f=fitPlanar(pairs,.01f,8,.4f,.5f,.03f);
                if(!(f.ok&&std::isfinite(f.phi)&&std::fabs(f.phi)<.6f&&std::fabs(f.tx)<4.f&&std::fabs(f.tz)<4.f)) continue;
                if(k>0&&(std::fabs(f.phi-predPhi)>.16f||std::hypot(f.tx-hT.x,f.tz-hT.z)>1.2f)) continue;
                fit=f;ok=true;hypUsed=k;break;
            }
            if(!ok&&bestN>=5) {   // few matched draws: a rigid fit of 5..7 well spread draws, as tight as 2 cm, close to the prediction
                const PlanarFit f=fitPlanar(bestPairs,.01f,5,.6f,.8f,.02f);
                if(f.ok&&std::isfinite(f.phi)&&std::fabs(f.phi-predPhi)<.12f&&std::hypot(f.tx-bestT.x,f.tz-bestT.z)<.6f&&std::fabs(f.tx)<4.f&&std::fabs(f.tz)<4.f) {
                    fit=f;ok=true;hypUsed=99;++weakOk;fp=&bestPairs;fy=&bestPdy;
                }
            }
            float knownTx=0,knownTz=0;
            if(!ok&&haveYaw&&!haveCam&&cd.size()>=3) {   // the rotation is known: only the translation is looked for (3 matched draws are enough)
                pairWith(predPhi);
                if(pairs.size()>=3) {
                    std::vector<float> ux,uz;ux.reserve(pairs.size());uz.reserve(pairs.size());
                    for(const PlanarPair& q:pairs) {const Vec3 r=planarRotate(predPhi,q.ax,q.az);ux.push_back(q.bx-r.x);uz.push_back(q.bz-r.z);}
                    std::vector<float> sx=ux,sz=uz;std::nth_element(sx.begin(),sx.begin()+sx.size()/2,sx.end());std::nth_element(sz.begin(),sz.begin()+sz.size()/2,sz.end());
                    const float mx=sx[sx.size()/2],mz=sz[sz.size()/2];
                    double ax=0,az=0;int n=0;for(size_t i=0;i<ux.size();++i) if(std::hypot(ux[i]-mx,uz[i]-mz)<.03f) {ax+=ux[i];az+=uz[i];++n;}
                    if(n>=3&&n*10>=int(pairs.size())*6&&std::fabs(mx)<4.f&&std::fabs(mz)<4.f) {knownOk=true;knownTx=float(ax/n);knownTz=float(az/n);fit.pairs=int(pairs.size());fp=&pairs;fy=&pdy;++knownRot;}
                }
            }
            const double cost=milliseconds()-t0;costMs+=cost;costMax=std::max(costMax,cost);
            if(knownOk) {
                phi=predPhi;tr={knownTx,0,knownTz};pairCount=int(pairs.size());ok=false;   // 'ok' stays false: the rotation is the measured one
                {   std::vector<float> in;for(size_t i=0;i<pairs.size();++i) {const Vec3 q=planarRotate(predPhi,pairs[i].ax,pairs[i].az);const float ex=q.x+knownTx-pairs[i].bx,ez=q.z+knownTz-pairs[i].bz;if(ex*ex+ez*ez<.03f*.03f) in.push_back(pdy[i]);}
                    if(in.size()>=3) {std::nth_element(in.begin(),in.begin()+in.size()/2,in.end());ty=in[in.size()/2];++yFits;}
                    if(!std::isfinite(ty)||std::fabs(ty)>.5f) ty=0.f;
                }
            } else if(ok) {
                phi=fit.phi;tr={fit.tx,0,fit.tz};++fitsOk;pairCount=fit.pairs;if(hypUsed>0&&hypUsed<99) ++rescued;
                if(cur.possYawOk&&pp->possYawOk&&hypUsed!=99&&std::fabs(rawYawStep)>.003f) {   // learn / check the game's own heading against the picture
                    if(haveYaw) {
                        const bool agree=std::fabs(fit.phi-predPhi)<.03f;
                        if(!agree) ++yawDisagree;
                        yawBad+=.05*((agree?0.:1.)-yawBad);
                        if(yawBad>.4) {yawLearned=false;yXY=yXX=yYY=0;yawBad=0;++yawUnlearned;}
                    }
                    yXY=yXY*.98+double(fit.phi)*rawYawStep;yXX=yXX*.98+double(rawYawStep)*rawYawStep;yYY=yYY*.98+double(fit.phi)*fit.phi;
                    if(yXX>5e-4) {
                        const double sc=yXY/yXX,r2=yYY>1e-12?yXY*yXY/(yXX*yYY):0.;
                        if(r2>.95&&std::fabs(sc)>.8&&std::fabs(sc)<1.25) {if(!yawLearned) {yawLearned=true;yawScale=sc>0?1.:-1.;yawBad=0;log("Possession: the game's camera heading follows the picture (scale %.3f, correlation %.3f): its yaw steps are used",sc,r2);} /* 0.8.75: the scale is exactly +-1 (the log says 'ratio 1.00'); report 013045 learned 1.2 from noisy fits */}
                    }
                }
                if(haveYaw&&std::fabs(fit.phi-predPhi)<.03f) phi=predPhi;
                {   // vertical motion of the world: median over the pairs that follow the planar fit (walk bob / steps)
                    std::vector<float> in;in.reserve(fp->size());
                    for(size_t i=0;i<fp->size();++i) {const Vec3 q=planarRotate(fit.phi,(*fp)[i].ax,(*fp)[i].az);const float ex=q.x+fit.tx-(*fp)[i].bx,ez=q.z+fit.tz-(*fp)[i].bz;if(ex*ex+ez*ez<.03f*.03f) in.push_back((*fy)[i]);}
                    if(in.size()>=8) {std::nth_element(in.begin(),in.begin()+in.size()/2,in.end());ty=in[in.size()/2];++yFits;tyFit=true;}
                    if(!std::isfinite(ty)||std::fabs(ty)>.5f) {ty=0.f;tyFit=false;}
                }
                pairsSum+=fit.pairs;shareSum+=fit.share;rmsSum+=fit.rms;
            } else {pairCount=bestN;failPairsSum+=bestN;if(bestN<8) ++failFew;else ++failOther;}
        }
        if(cur.possCamOk) {lastCam[0]=cur.possCam[0];lastCam[1]=cur.possCam[1];lastCam[2]=cur.possCam[2];lastCamZ=cur.possCamZ;lastCamOk=true;}
        if(haveYaw) ++yawUsed;
        // 0.8.74: translation and height of this game frame from the game camera position
        if(haveCamPair) {
            const double PhiNew=chainYaw+phi;
            const Vec3 trFit=tr;
            if(haveCam) {
                const Vec3 trm=camTr(PhiNew);
                if(ok) {
                    const double err=std::hypot(double(trFit.x)-trm.x,double(trFit.z)-trm.z);
                    const bool agree=err<.05;
                    if(agree) {tr=trm;camTrUsed=true;} else ++camDisagree;
                    camBad+=.05*((agree?0.:1.)-camBad);
                    if(camBad>.4) {camLearned=false;for(CamSum& m:cs) m=CamSum();camN=0;camBad=0;++camUnlearned;}
                } else {tr=trm;camTrUsed=true;}
                if(camTrUsed) ++camUsed;
            }
            if(ok&&hypUsed>=0&&hypUsed!=99&&dcx*dcx+dcy*dcy>4e-8) {   // learn the link between the camera steps and the fitted translations
                const double vr=trFit.x,vi=trFit.z;
                for(int m=0;m<2;++m) {
                    double ur,ui;camU(m,PhiNew,ur,ui);CamSum& q=cs[m];
                    q.sr=q.sr*.98+(ur*vr+ui*vi);q.si=q.si*.98+(ur*vi-ui*vr);q.suu=q.suu*.98+ur*ur+ui*ui;q.svv=q.svv*.98+vr*vr+vi*vi;
                }
                if(++camN>=8) {
                    int bm=0;double bestR2=-1,ar=0,ai=0;
                    for(int m=0;m<2;++m) {
                        const CamSum& q=cs[m];if(q.suu<2e-4||q.svv<1e-12) continue;
                        const double r2=(q.sr*q.sr+q.si*q.si)/(q.suu*q.svv);
                        if(r2>bestR2) {bestR2=r2;bm=m;ar=q.sr/q.suu;ai=q.si/q.suu;}
                    }
                    const double mag=std::hypot(ar,ai);
                    if(bestR2>.95&&mag>.01&&mag<1000.) {
                        if(!camLearned) {camLearned=true;camModel=bm;camAr=ar;camAi=ai;camBad=0;log("Possession: the game's camera position follows the picture (%s, scale %.3f, correlation %.3f): its walk steps are used",bm?"mirrored":"direct",mag,std::sqrt(bestR2));}
                        else if(bm==camModel) {camAr+=.03*(ar-camAr);camAi+=.03*(ai-camAi);}
                    }
                }
            }
            // height: the vertical step of the picture against the camera height step (integer position or float height, one scalar)
            {
                const double xz[2]={dcz,dcz2};
                if(ok&&tyFit&&hypUsed>=0&&hypUsed!=99) {
                    for(int m=0;m<2;++m) {zxy[m]=zxy[m]*.98+xz[m]*ty;zxx[m]=zxx[m]*.98+xz[m]*xz[m];zyy[m]=zyy[m]*.98+double(ty)*ty;}
                    if(zLearned) {const double e=std::fabs(double(ty)-zK*xz[zModel]);const bool agree=e<.03;zBad+=.05*((agree?0.:1.)-zBad);if(zBad>.4) {zLearned=false;zxy[0]=zxy[1]=zxx[0]=zxx[1]=zyy[0]=zyy[1]=0;zBad=0;}}
                    else if(camN>=8) {
                        int bm=-1;double bestR2=.9;
                        for(int m=0;m<2;++m) if(zxx[m]>1e-7&&zyy[m]>1e-10) {const double r2=zxy[m]*zxy[m]/(zxx[m]*zyy[m]);if(r2>bestR2) {bestR2=r2;bm=m;}}
                        if(bm>=0&&std::fabs(zxy[bm]/zxx[bm])>.01&&std::fabs(zxy[bm]/zxx[bm])<1000.) {zLearned=true;zModel=bm;zK=zxy[bm]/zxx[bm];zBad=0;log("Possession: the game's camera height follows the picture (%s, scale %.3f, correlation %.3f): its height steps are used",bm?"float height":"integer height",zK,std::sqrt(bestR2));}
                    }
                }
                if(zLearned) {
                    const double tyM=zK*xz[zModel];
                    if(!tyFit&&std::isfinite(tyM)&&std::fabs(tyM)<.5) {ty=float(tyM);zNow=true;++zUsed;}
                    else if(tyFit&&std::fabs(double(ty)-tyM)<.03) {ty=float(tyM);zNow=true;++zUsed;}
                }                else if(haveCamPair&&std::fabs(dcz)<1e-5&&std::fabs(dcz2)<1e-5) {   // 0.8.74: the game's camera did not move vertically (report 013045: constant height for 25 s while the picture fits drifted by 1.2 m): no vertical step
                    ty=0.f;zNow=true;++zUsed;
                }
            }
        }
        if(!ok&&!knownOk&&!camTrUsed) ++guessed;
        // chain: L' = R(phi) L + t  =>  Phi' = Phi + phi,  P' = P - R(-Phi') t
        chainYaw+=phi;
        const Vec3 dP=planarRotate(float(-chainYaw),tr.x,tr.z);
        chainPos={chainPos.x-dP.x,0,chainPos.z-dP.z};
        chainY+=ty;
        if(clean) {   // 0.8.80: learn the cruise velocity of the stick direction from the game's own walk. Heading-frame displacement summed over blocks of ~1.6 s (one gait period): the 0.6 Hz sway averages out
            const double dts=std::max(dt,1.)*.001;
            if(lastChainOk&&dts<.15&&stickBin>=0&&stickGate>.9&&stickBin==learnBin) {
                const Vec3 hd=planarRotate(float(chainYaw),float(chainPos.x-lastChainPos.x),float(chainPos.z-lastChainPos.z));
                holdT+=dts;   // the first 0.5 s of a press (the game's latency and acceleration) are not part of the cruise speed
                if(holdT>.5) {accX+=hd.x;accZ+=hd.z;accT+=dts;++ffLearnFrames;}
                const double block=bins[stickBin].ok?1.6:1.5;
                if(accT>=block) {
                    Bin& bn=bins[stickBin];const double vx=accX/accT,vz=accZ/accT;
                    if(!bn.ok) {if(std::hypot(vx,vz)>.05) {   // learned: the path is now driven by the stick; the follower held the velocity of the walk, which the stick now carries (no jump of the shown velocity)
                            const Vec3 vwo=planarRotate(float(-chainYaw),float(vFx),float(vFz));   // what the path already moved with (fallback or 0)
                            bn.vx=vx;bn.vz=vz;bn.ok=true;bn.t=accT;vFx=vx;vFz=vz;
                            const Vec3 vwl=planarRotate(float(-chainYaw),float(vx),float(vz));qv.x-=vwl.x-vwo.x;qv.z-=vwl.z-vwo.z;
                        }}
                    else if(std::hypot(vx,vz)>.5*std::hypot(bn.vx,bn.vz)) {bn.vx+=.5*(vx-bn.vx);bn.vz+=.5*(vz-bn.vz);}   // blocked by a wall: keep the speed
                    accX=accZ=accT=0;
                }
            } else {accX=accZ=accT=0;holdT=0;learnBin=(stickBin>=0&&stickGate>.9)?stickBin:-1;}
            lastChainPos=chainPos;lastChainOk=true;
        }
        ihPush(cur.contentMs,chainYaw,chainPos,chainY,now);   // 0.8.98: the game's camera path, one point per game frame
        if(trace&&traceLines<60000) {
            float cc=0;const bool hc=commandAt(cur.contentMs,cc);char b[330];
            snprintf(b,sizeof b,"G,%.2f,%.2f,%.5f,%.4f,%.5f,%.5f,%.5f,%d,%d,%.4f,%.5f,%.5f,%.5f,%d,%.4f,%d,%.5f,%.5f,%.5f\n",now,cur.contentMs,hc?cc:0.f,chainYaw*180./Pi,chainPos.x,chainPos.z,chainY,ok?1:0,pairCount,phi*180./Pi,tr.x,tr.z,ty,hypUsed,double(rawYawStep)*180./Pi,(haveYaw?1:0)|(knownOk?2:0)|(camTrUsed?4:0)|(zNow?8:0),cur.possUpOk?double(cur.possUp[0]):0.,cur.possUpOk?double(cur.possUp[1]):0.,cur.possUpOk?double(cur.possUp[2]):0.);
            traceBuf+=b;++traceLines;
        }
        // learn gain and offset (turn)
        float c=0;
        const double decay=std::exp(-std::clamp(dt,0.,150.)/800.);gSumPhi*=decay;gSumCmd*=decay;
        if(snapNow||!commandAt(cur.contentMs,c)) {offsetValid=false;gSumPhi=gSumCmd=0;return;}
        if((ok||haveYaw)&&haveC) {sumPhi+=phi;sumCmd+=double(cB-cA);++gainN;gSumPhi+=phi;gSumCmd+=double(cB-cA);}
        if(clean) gain=viewGain;   // 0.8.80: the view gain is fixed
        else if(std::fabs(gSumCmd)>.2) {
            // gain = chained yaw over commanded yaw, measured on the fitted game frames of the last ~0.8 s only
            const double gT=gSumPhi/gSumCmd;
            if(gT>.3&&gT<4.) {
                const double ng=gain+.08*(gT-gain),dG=ng-gain;gain=ng;++gainUpdates;gainSum+=gain;
                if(offsetValid&&ringCount>0) offset-=dG*double(ring[(ringHead-1+96)%96].yaw);   // the picture must not move because the gain changed
            } else ++gainClamped;
        }
        offTarget=chainYaw-gain*c;lastObsMs=cur.contentMs;
        if(!offsetValid||std::fabs(offTarget-offset)>70.*Pi/180.) {offset=offTarget;offV=0;offsetValid=true;}
    }
    // Per headset frame: the correction that brings the newest game frame to the pose shown now.
    void extrapolate(const Package& p,const Request& r,double now,float& yaw,Vec3& pan) {
        (void)p;yaw=0;pan={0,0,0};++frames;
        if(!valid) return;
        lsYaw=chainYaw;lsPos=chainPos;lsY=chainY;
        const bool snap=(r.comfortFlags&ComfortSnapTurn)!=0;
        advance(now);
        if(snap) ++skippedSnap;
        {   // 0.8.80: heading error view - game for the closed loop of the game thread (HeadingFollow)
            const bool hf=clean&&predictTurn&&!snap&&offsetValid;
            HeadingFollowShared::publish(hf,hf?wrapRadians(float(offset-offTarget)):0.,gain);headingErrShown=hf?offset-offTarget:0.;
        }
        double shownYaw=chainYaw;Vec3 shownPos=chainPos;double shownY=chainY;
        if(interp) interpPose(now,shownYaw,shownPos,shownY);   // 0.8.98: the game camera's path, interpolated (not ready yet: the newest frame as it is)
        if(!interp) {
        if(!predictTurn&&!predictMove) return;
        if(predictTurn&&!snap&&offsetValid&&std::isfinite(r.possessionTurn)) shownYaw=gain*double(r.possessionTurn)+offset;
        const double leadHeight=stableHeightLead*heightSmoothMs*.001;   // height: no lead by default (a real vertical move is rare; the bob is what must go)
        if(predictMove) {
            const Vec3 vh=planarRotate(float(chainYaw),vPos.x,vPos.z);
            const Vec3 lead=clean?Vec3{0,0,0}:planarRotate(float(-chainYaw),float(vh.x*swaySmoothMs*.001*stableSwayLead),float(vh.z*moveSmoothMs*.001*stableForwardLead));
            shownPos={sPos.x+lead.x,0,sPos.z+lead.z};
        }
        shownY=stabilizeHeight?sY+vY*leadHeight:chainY;
        {   // 0.8.74: the vertical correction is bounded (a walk bob is a few cm; more is drift of the vertical estimate: report 013045 had 20-46 cm and a 0.5 m reset jump)
            const double LY=clean?.03:.06,ey=shownY-chainY;
            if(std::fabs(ey)>LY) shownY=chainY+(ey>0?LY:-LY);
        }
        }
        double dy=shownYaw-chainYaw;
        {   // 0.8.72: a large lead of the shown turn over the game's frame (fast reversal of the stick) is compressed, never cut (0.8.71 reset it:
            // jumps of 13-43 degrees in report 005539). Up to 30 degrees it is exact, beyond that a quarter of the excess.
            const double L=30.*Pi/180.;
            if(std::fabs(dy)>L) {dy=(dy>0?1.:-1.)*(L+(std::fabs(dy)-L)*.25);shownYaw=chainYaw+dy;}
        }
        Vec3 t=planarRotate(float(shownYaw),chainPos.x-shownPos.x,chainPos.z-shownPos.z);
        t.y=float(shownY-chainY);
        double shownYv=shownY;
        if(std::fabs(dy)>150.*Pi/180.||std::sqrt(double(t.x)*t.x+double(t.z)*t.z)>(clean?2.5:1.2)||std::fabs(t.y)>.5) {
            ++resets;offsetValid=false;sPos=chainPos;vPos={};nInit=false;pFF=chainPos;qs={};qv={};ffGate=0;sY=chainY;vY=0;shownYaw=chainYaw;shownPos=chainPos;shownYv=chainY;dy=0;t={0,0,0};ihN=0;icY=icH=0;icX=icZ=0;
        }
        lsYaw=shownYaw;lsPos=shownPos;lsY=shownYv;   // 0.8.80: the pose shown now, for the side / rear views
        yaw=float(dy);pan=t;++applied;exYMax=std::max(exYMax,std::fabs(double(t.y)));
        // statistics: how rough is the world's yaw / position at the headset rate, raw game frames versus what is shown
        const double ty=std::fabs(dy)*180./Pi,tm=std::sqrt(double(t.x)*t.x+double(t.z)*t.z);
        exTurnSum+=ty;exTurnMax=std::max(exTurnMax,ty);exMoveSum+=tm;exMoveMax=std::max(exMoveMax,tm);
        if(histN>=2) {
            const double rawTurn=chainYaw-rawHist[0],shownTurn=shownYaw-shownHist[0];
            const double raw2=chainYaw-2*rawHist[0]+rawHist[1],shown2=shownYaw-2*shownHist[0]+shownHist[1];
            rawJitter2+=raw2*raw2;shownJitter2+=shown2*shown2;++jitterN;
            const Vec3 r0=rawPosHist[0],r1=rawPosHist[1],s0=shownPosHist[0],s1=shownPosHist[1];
            const double rx=chainPos.x-2*r0.x+r1.x,rz=chainPos.z-2*r0.z+r1.z,sx=shownPos.x-2*s0.x+s1.x,sz=shownPos.z-2*s0.z+s1.z;
            rawJitterMove2+=rx*rx+rz*rz;shownJitterMove2+=sx*sx+sz*sz;
            const double ry=chainY-2*rawYHist[0]+rawYHist[1],syy=shownYv-2*shownYHist[0]+shownYHist[1];rawJitterY2+=ry*ry;shownJitterY2+=syy*syy;
            // turning (the stick turns at more than 20 deg/s): this frame goes the other way
            const double cmdRate=histN>=2?(double(r.possessionTurn)-lastCmdSeen)*gain/std::max(1e-3,(now-lastCmdMs)*.001):0.;
            if(std::fabs(cmdRate)>20.*Pi/180.) {++motionFrames;const double dir=cmdRate>0?1:-1;if(rawTurn*dir<-1e-4) ++reversalsRaw;if(shownTurn*dir<-1e-4) ++reversalsShown;}
        }
        lastCmdSeen=r.possessionTurn;lastCmdMs=now;
        rawYHist[1]=rawYHist[0];rawYHist[0]=chainY;shownYHist[1]=shownYHist[0];shownYHist[0]=shownYv;
        if(trace&&traceLines<60000) {
            char b[260];snprintf(b,sizeof b,"H,%.2f,%.5f,%.4f,%.4f,%.4f,%.5f,%.5f,%.5f\n",now,double(r.possessionTurn),shownYaw*180./Pi,chainYaw*180./Pi,dy*180./Pi,t.x,t.y,t.z);
            traceBuf+=b;++traceLines;
        }
        rawHist[1]=rawHist[0];rawHist[0]=chainYaw;shownHist[1]=shownHist[0];shownHist[0]=shownYaw;
        rawPosHist[1]=rawPosHist[0];rawPosHist[0]=chainPos;shownPosHist[1]=shownPosHist[0];shownPosHist[0]=shownPos;
        if(histN<2) ++histN;
    }
    void flushTrace() {
        if(traceBuf.empty()) return;
        FILE* f=fopen("DK2VR-possession.csv",traceHeader?"ab":"wb");
        if(f) {
            if(!traceHeader) {traceHeader=true;fputs("# DK2VR 0.8.80 possession trace. G = game frame (now ms, content ms, commanded turn rad, chain yaw deg, pos x z, height, fit ok, pairs (best pairing when no fit), step yaw deg, step x z y, hypothesis used: 0 predicted step, 1.. other step, 99 weak fit, -1 none, game camera yaw step deg (raw), yaw source bits: 1 camera heading, 2 translation from the picture with the known rotation, 4 translation from the camera position, 8 height from the camera), then the floor normal in the game camera frame x y z (0.8.75: roll / pitch of the game camera). H = headset frame (now ms, commanded turn rad, shown yaw deg, chain yaw deg, correction yaw deg, correction x y z)\n",f);}
            fwrite(traceBuf.data(),1,traceBuf.size(),f);fclose(f);
        }
        traceBuf.clear();
    }
    void report() {
        if(!frames&&!fits) return;
        const double rad=180./Pi;
        if(interp) {log("Possession interpolated view (0.8.99.1, last 5 s): %s | headset images %llu, delay avg %.1f ms (game frame interval %.1f ms, arrival %.1f ms, lead %.2f interval, carried on up to %.2f interval) | carried on %llu, held %llu, older than the history %llu, history restarts (teleport / gap) %llu | faded corrections %llu, avg %.2f deg, max %.2f deg (fade %.0f ms)",
            ipFrames?"ON":"waiting for 2 game frames",(unsigned long long)ipFrames,ipFrames?ipDelaySum/ipFrames:0.,ihDtEma,ihAgeEma,double(interpLead),double(interpExtrapMax),(unsigned long long)ipExtrap,(unsigned long long)ipHold,(unsigned long long)ipOld,(unsigned long long)ipJump,
            (unsigned long long)icN,icN?icSum/icN*rad:0.,icMax*rad,double(interpCorrectMs));
            ipFrames=ipExtrap=ipHold=ipOld=ipJump=0;ipDelaySum=0;icN=0;icSum=0;icMax=0;}
        log("Possession at the headset rate (0.8.80, last 5 s): headset images %llu | smoothing applied %llu (snap turn %llu, resets %llu) | correction avg %.2f deg / %.1f mm, max %.2f deg / %.1f mm | game-frame fits %llu ok %llu (guessed %llu, gaps %llu), pairs avg %.0f, inliers %.0f%%, residual %.1f mm, cost avg %.2f ms max %.2f ms | roughness of the world turn (2nd difference, rms) game frames %.3f deg -> shown %.3f deg; of the walk %.2f mm -> %.2f mm | backward steps while turning %llu -> %llu of %llu | learned game gain %.2f (%llu updates)",
            (unsigned long long)frames,(unsigned long long)applied,(unsigned long long)skippedSnap,(unsigned long long)resets,applied?exTurnSum/applied:0.,applied?1000.*exMoveSum/applied:0.,exTurnMax,1000.*exMoveMax,
            (unsigned long long)fits,(unsigned long long)fitsOk,(unsigned long long)guessed,(unsigned long long)gaps,fitsOk?pairsSum/fitsOk:0.,fitsOk?100.*shareSum/fitsOk:0.,fitsOk?1000.*rmsSum/fitsOk:0.,fits?costMs/fits:0.,costMax,
            jitterN?std::sqrt(rawJitter2/jitterN)*rad:0.,jitterN?std::sqrt(shownJitter2/jitterN)*rad:0.,jitterN?1000.*std::sqrt(rawJitterMove2/jitterN):0.,jitterN?1000.*std::sqrt(shownJitterMove2/jitterN):0.,
            (unsigned long long)reversalsRaw,(unsigned long long)reversalsShown,(unsigned long long)motionFrames,gain,(unsigned long long)gainUpdates);
        log("Possession height / walk (0.8.80, last 5 s): height roughness (2nd difference, rms) game frames %.2f mm -> shown %.2f mm | height correction max %.1f mm | height fits %llu",
            jitterN?1000.*std::sqrt(rawJitterY2/jitterN):0.,jitterN?1000.*std::sqrt(shownJitterY2/jitterN):0.,1000.*exYMax,(unsigned long long)yFits);
        if(clean) {int nb=0;for(const Bin& q:bins) nb+=q.ok?1:0;log("Possession clean camera (0.8.80, last 5 s): stick directions learned %d of 8 (forward %.2f / %.2f, back %.2f / %.2f scene units per s) | feed-forward gate avg %.2f | learned on %llu game frames, walk of a not yet learned direction taken from the others on %llu frames | heading error view - game now %.1f deg | position: %s, gait %.2f Hz (%llu roll half-periods measured)",nb,bins[0].vz,bins[0].vx,bins[4].vz,bins[4].vx,ffN?ffGateSum/ffN:0.,(unsigned long long)ffLearnFrames,(unsigned long long)fbFrames,headingErrShown*180./Pi,cleanNotch?"gait notch (0.8.80)":"learned path (0.8.77)",cleanNotch&&gaitAuto?f0Now:double(gaitHz),(unsigned long long)gaitCross);fbFrames=0;ffGateSum=0;ffN=0;}
        log("Possession camera (0.8.80, last 5 s): game camera position %s (%s, scale %.3f, used on %llu game frames, disagreed with the picture %llu, dropped %llu) | height %s (%s, scale %.3f, used on %llu frames) | last raw position %.3f %.3f %.3f tiles, float height %.3f",
            camLearned?"ON":"not learned",camModel?"mirrored":"direct",std::hypot(camAr,camAi),(unsigned long long)camUsed,(unsigned long long)camDisagree,(unsigned long long)camUnlearned,zLearned?"ON":"not learned",zModel?"float":"integer",zK,(unsigned long long)zUsed,lastCam[0]/4096.,lastCam[1]/4096.,lastCam[2]/4096.,lastCamZ);
        log("Possession fits (0.8.80, last 5 s): game yaw from the camera heading %s (scale %.3f, used on %llu frames, disagreed with the picture %llu, dropped %llu) | translation with the known rotation %llu | game frames %llu | fitted %llu (%.0f%%) of which with another yaw step %llu, weak fits %llu | not fitted %llu (fewer than 8 matched draws %llu, other %llu; best pairing avg %.1f draws) | gain updates %llu (out of range %llu)",
            yawLearned?"ON":"not learned",yawScale,(unsigned long long)yawUsed,(unsigned long long)yawDisagree,(unsigned long long)yawUnlearned,(unsigned long long)knownRot,
            (unsigned long long)fits,(unsigned long long)fitsOk,fits?100.*fitsOk/fits:0.,(unsigned long long)rescued,(unsigned long long)weakOk,(unsigned long long)guessed,(unsigned long long)failFew,(unsigned long long)failOther,guessed?failPairsSum/guessed:0.,(unsigned long long)gainUpdates,(unsigned long long)gainClamped);
        clearStats();
        flushTrace();
    }
};

struct VrThread {
    // Shared with the game thread.
    std::mutex m;std::shared_ptr<const Package> mailbox;uint64_t inUse=NoPackage;
    bool auxEnabled=true;uint64_t sAuxFrames=0,sAuxDrawn=0,sAuxNoPlace=0,sAuxFirstOk=0,sOverlayDraws=0,sFaceFilled=0,sFaceSkipped=0;   // 0.8.81: 360 view statistics (VR thread)
    std::atomic<bool> active{false},failed{false},quit{false};
    std::atomic<uint32_t> epoch{1};
    std::atomic<double> lastPublishMs{0};std::atomic<uint64_t> published{0};
    bool mips=true,panPrediction=true,showHands=true,tablePointerVr=true;float panLimit=.05f; // 0.5.10: 5 cm (0.25 m: the headset showed up to ~9 cm beyond the tiles the game had computed)
    // 0.4.0: dungeon room + wooden table (Environment=1), 4x MSAA on the headset images (Antialiasing=4).
    bool environment=true;float tableAspect=.75f;env::Scene& scene=env::globalScene();
    D3DMULTISAMPLE_TYPE samples=D3DMULTISAMPLE_NONE;ComPtr<IDirect3DSurface9> msaaColor,msaaDepth;UINT msaaW=0,msaaH=0;
    Ipc* ipc=nullptr;
    // VR thread only.
    ComPtr<IDirect3DDevice9Ex> dev;HWND window=nullptr;HANDLE thread=nullptr;
    ComPtr<IDirect3DTexture9> keeperTexture;HANDLE keeperOpened=nullptr;
    ComPtr<IDirect3DSurface9> heldDepth;UINT heldDepthW=0,heldDepthH=0;D3DMULTISAMPLE_TYPE heldDepthSamples=D3DMULTISAMPLE_NONE;
    struct Opened {ComPtr<IDirect3DTexture9> texture;uint64_t lastFrame=0;UINT w=0,levels=0;};
    // 0.4.7: texture sharpness in VR. The table is seen at a grazing angle: trilinear filtering then picks
    // small mip levels (blurry floor). Game samplers using LINEAR minification become ANISOTROPIC (x16);
    // the game's LOD bias is replaced by VrLodBias. Diagnostics: filters and texture sizes seen.
    int anisotropy=16;float lodBias=0;bool keepBias=false;
    std::map<uint32_t,uint64_t> filterSeen;std::map<UINT,uint64_t> sizeSeen;int sharpReports=0;
    std::unordered_map<uint32_t,Opened> textures;std::unordered_map<uint32_t,ComPtr<IDirect3DPixelShader9>> shaders;ComPtr<IDirect3DPixelShader9> barShader,mapShader,menuUiShader;ComPtr<IDirect3DTexture9> menuT,menuU,menuCpuTex;uint64_t menuCpuKey=0;bool menuCpuFailed=false;UINT menuCpuW=0,menuCpuH=0;uint64_t menuCpuUploads=0;ComPtr<IDirect3DSurface9> menuTs,menuUs,menuDs;UINT menuTw=0,menuTh=0;bool menuUiOk=false,menuShaderTried=false,menuUiDebug=false;Pose loadPose{};bool loadPoseSet=false;uint32_t menuUiForEpoch=0;uint64_t menuUiBuilt=0;float menuUiThreshold=.03f;struct MenuLagTex {ComPtr<IDirect3DTexture9> t;const void* key=nullptr;};MenuLagTex menuLagTex[4];ComPtr<IDirect3DTexture9> menuUk[3];ComPtr<IDirect3DSurface9> menuUks[3];ComPtr<IDirect3DQuery9> menuQ[3];bool menuQBusy[3]{},menuQDone[3]{};uint64_t menuQGen[3]{},menuGen=0;DWORD menuQRes[3]{};double menuLagScore[3]={0,.2,0};
    // 0.9.6 diagnostic (menu banners blinking, report 214245): flat replay of the menu package compared with the game's own decor read-back
    ComPtr<IDirect3DTexture9> mcRt;ComPtr<IDirect3DSurface9> mcRts,mcDs,mcSys;UINT mcW=0,mcH=0;uint64_t mcLast=0,mcChecks=0;double mcMs=0,mcLogMs=0,mcSum=0;int mcDumps=0;bool mcFailed=false;float mcCell[16*12]{},mcCellMax[16*12]{};uint64_t mcLag[3]{};
   int menuLagChosen=1,menuLagFixed=-1;uint64_t menuLagSamples=0;double menuLagLog=0,menuUiFrac[3]{};bool hudBacking=true,barsFromImage=false,testWalls=true,hudFront=false;
    uint32_t cacheEpoch=0;uint64_t frames=0;
    MenuDepthTarget menuDepth; // independent full-precision depth, per-eye resolution
    struct Images {ComPtr<IDirect3DTexture9> color[3];ComPtr<IDirect3DSurface9> target[3],depth;HANDLE handles[3]{};UINT w=0,h=0,dw=0,dh=0;uint32_t generation=0;};
    Images images[SlotCount];ComPtr<IDirect3DQuery9> query;
    std::shared_ptr<const Package> current;uint64_t lastPackageId=0;MotionInterp interp;
    MotionProbe motionProbe;std::shared_ptr<const Package> probePrev;PossessionMotion possMotion; // 0.8.68
    Vec3 lastGrip{};double lastGripMs=-1e9;float lastTurn=0;double lastTurnMs=-1e9;double sTurn=0,sEnv=0,sWorst=0;
    // 0.8.65 diagnostic: how well the pan prediction hides the game's latency (the 'double image' when the map moves).
    struct PanDiag {
        uint64_t frames=0,applied=0,clamped=0,stale=0,switches=0,jump5=0,jump10=0,jump20=0;double rawSum=0,rawMax=0,jumpSum=0,jumpMax=0,clampLoss=0;
        bool prevPan=false;Vec3 prevFrom{};int prevHand=0;uint64_t prevId=0;
        uint64_t tFrames=0,tApplied=0,tStale=0,tSwitches=0,tJump05=0,tJump1=0,tJump2=0;double tRawMax=0,tJumpSum=0,tJumpMax=0;bool prevTurn=false;float prevTurnFrom=0;
    } panDiag;
    struct StickDiag {uint64_t frames=0,applied=0;double offSum=0,offMax=0,yawSum=0,yawMax=0;} stickDiag; // 0.8.66
    // Statistics (VR thread).
    uint64_t sFrames=0,sPackages=0,sNoSlot=0,sOpened=0,sOpenFailures=0,sDrawFailures=0,sIdle=0;double sRender=0,sWait=0,sAge=0,sStart=0,sPan=0;uint64_t lastReport=0;
    int vrPriorityApplied=-100,vrPriorityLogs=0,vrSleepSlow=0;double perfLastImageMs=0;bool vrWaitSleep=iniInt(L"VrWaitSleep",0)!=0;   // 0.9.25 (0.9.26: off by default, the Windows timer of the player's PC gives 6.6 ms for Sleep(1))
    uint32_t consecutiveFailures=0;GpuTimer gpuTimer;double lastRenderMs=0;int reportMode=0;uint32_t lastPointerFlags=0;uint64_t sCapped=0,sCurDraw=0,sCurNoTex=0,sCurNoAim=0,sCurNoSize=0;

    static LRESULT CALLBACK windowProc(HWND h,UINT m,WPARAM w,LPARAM l) {return DefWindowProcW(h,m,w,l);}
    // Creates device B on the game's adapter and verifies texture sharing A -> B (level 0 and mip levels).
    bool create(IDirect3DDevice9* a,Ipc* channel) {
        ipc=channel;
        panPrediction=iniInt(L"PanPrediction",1)!=0;panLimit=iniFloat(L"PanPredictionLimit",.18f,0.f,.5f);showHands=iniInt(L"ShowControllers",1)!=0;tablePointerVr=iniInt(L"TablePointer",1)!=0;
        anisotropy=int(iniFloat(L"VrAnisotropy",16,1,16));lodBias=iniFloat(L"VrLodBias",0.f,-3.f,3.f);keepBias=iniInt(L"VrKeepGameLodBias",0)!=0;
        environment=iniInt(L"Environment",1)!=0;tableAspect=iniFloat(L"TableAspect",.75f,0.f,3.f);
        auto fail=[&](const char* what,HRESULT hr) {log("Decoupled VR rendering unavailable (%s, %08lX); 0.2.9 path kept",what,(unsigned long)hr);failed=true;dev.Reset();return false;};
        ComPtr<IDirect3D9> d3d;HRESULT hr=a->GetDirect3D(&d3d);
        if(FAILED(hr)) return fail("GetDirect3D",hr);
        ComPtr<IDirect3D9Ex> ex;if(FAILED(hr=d3d.As(&ex))) return fail("IDirect3D9Ex",hr);
        D3DDEVICE_CREATION_PARAMETERS cp{};if(FAILED(hr=a->GetCreationParameters(&cp))) return fail("GetCreationParameters",hr);
        WNDCLASSW wc{};wc.lpfnWndProc=&windowProc;wc.hInstance=GetModuleHandleW(nullptr);wc.lpszClassName=L"DK2VR_VrThread";
        RegisterClassW(&wc);
        window=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,L"DK2VR_VrThread",L"DK2VR",WS_POPUP,0,0,16,16,nullptr,nullptr,wc.hInstance,nullptr);
        if(!window) return fail("CreateWindow",HRESULT_FROM_WIN32(GetLastError()));
        D3DPRESENT_PARAMETERS pp{};pp.Windowed=TRUE;pp.SwapEffect=D3DSWAPEFFECT_DISCARD;pp.BackBufferFormat=D3DFMT_UNKNOWN;
        pp.BackBufferWidth=pp.BackBufferHeight=16;pp.hDeviceWindow=window;pp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
        hr=ex->CreateDeviceEx(cp.AdapterOrdinal,D3DDEVTYPE_HAL,window,D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_FPU_PRESERVE,&pp,nullptr,&dev);
        if(FAILED(hr)) return fail("CreateDeviceEx",hr);
        // 0.6.9.15: pixel shader for the bars cut from the game image: black (no interface) becomes transparent,
        // colours premultiplied. Compiled at run time with the system d3dcompiler_47 (no link dependency); without it
        // the bars keep the interface texture (previous behaviour).
        hudBacking=iniInt(L"HudStoneBacking",0)!=0; // 0.7.1: transparent again (user request)
        // 0.8.0: the bars come from the interface layer (exact transparency of every piece) now that the dungeon can no
        // longer reach it (engine origin); HudBarsSource=1 goes back to the 0.7.10 cut of the game image.
        barsFromImage=iniInt(L"HudBarsSource",0)==1;testWalls=false; /* 0.8.85: the four numbered test panels are removed */
        log("VR thread: bars from %s; numbered test panels on the side walls %s",barsFromImage?"the game image (HudBarsSource=1)":"the interface layer (HudBarsSource=0)",testWalls?"on (TestWalls=1)":"off");
        if(iniInt(L"HudFromGameImage",1)!=0) {
            using Compile=HRESULT(WINAPI*)(LPCVOID,SIZE_T,LPCSTR,const D3D_SHADER_MACRO*,ID3DInclude*,LPCSTR,LPCSTR,UINT,UINT,ID3DBlob**,ID3DBlob**);
            HMODULE dc=LoadLibraryW(L"d3dcompiler_47.dll");
            auto compile=dc?reinterpret_cast<Compile>(GetProcAddress(dc,"D3DCompile")):nullptr;
            // 0.7.8: fixed cut-out of the game's bars (rectangles measured on a clean capture, fractions of the 1024x768
            // game area): outside them everything is transparent whatever the game image holds (dungeon specks).
            const char* src="sampler f:register(s0);float4 r[8]:register(c0);"
                "float inside(float4 q,float2 uv){float2 a=step(q.xy,uv)*step(uv,q.zw);return a.x*a.y;}"
                "float4 main(float2 uv:TEXCOORD0):COLOR{float4 c=tex2D(f,uv);float m=0;"
                "for(int i=0;i<8;i++) m=max(m,inside(r[i],uv));"
                "float l=max(c.r,max(c.g,c.b));float a=saturate((l-0.008)*60.0)*m;return float4(c.rgb*a,a);}"; // 0.7.10: only pure black is transparent (dark button interiors stay)
            ComPtr<ID3DBlob> code,err;
            // 0.7.10: minimap = its exact frame (dark metal square, measured x 8-186, y 585-760 of 1024x768), opaque:
            // everything outside the rectangle is transparent (the game image may hold dungeon specks there).
            {const char* msrc="sampler f:register(s0);float4 k:register(c0);"
                "float4 main(float2 uv:TEXCOORD0):COLOR{float4 c=tex2D(f,uv);float2 a=step(k.xy,uv)*step(uv,k.zw);float m=a.x*a.y;return float4(c.rgb*m,m);}";
             ComPtr<ID3DBlob> mc,me;
             if(compile&&SUCCEEDED(compile(msrc,strlen(msrc),nullptr,nullptr,nullptr,"main","ps_2_0",0,0,&mc,&me))) dev->CreatePixelShader(static_cast<const DWORD*>(mc->GetBufferPointer()),&mapShader);}
            if(compile&&SUCCEEDED(compile(src,strlen(src),nullptr,nullptr,nullptr,"main","ps_2_a",0,0,&code,&err))&&SUCCEEDED(dev->CreatePixelShader(static_cast<const DWORD*>(code->GetBufferPointer()),&barShader)))
                log("VR thread: bars drawn from the game image (HudFromGameImage=1)");
            else log("VR thread: bar shader unavailable (d3dcompiler_47 %s), bars from the interface texture",dc?"failed":"missing");
        }
        // Self-test: A fills three mip levels with known colours; B must read them back.
        const DWORD colors[3]={0xff12ab34,0xff3456cd,0xffabcdef};
        auto testLevels=[&](UINT levels)->int {
            ComPtr<IDirect3DTexture9> ta,tb;HANDLE handle=nullptr;
            if(FAILED(a->CreateTexture(64,64,levels,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&ta,&handle))||!handle) return -1;
            for(UINT l=0;l<levels;++l) {ComPtr<IDirect3DSurface9> s;ta->GetSurfaceLevel(l,&s);if(!s||FAILED(a->ColorFill(s.Get(),nullptr,colors[l]))) return -2;}
            ComPtr<IDirect3DQuery9> q;if(FAILED(a->CreateQuery(D3DQUERYTYPE_EVENT,&q))) return -3;
            q->Issue(D3DISSUE_END);uint64_t deadline=GetTickCount64()+1000;HRESULT r;
            do {r=q->GetData(nullptr,0,D3DGETDATA_FLUSH);if(r==S_FALSE) Sleep(0);} while(r==S_FALSE&&GetTickCount64()<deadline);
            if(r!=S_OK) return -4;
            if(FAILED(dev->CreateTexture(64,64,levels,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&tb,&handle))) return -5;
            for(UINT l=0;l<levels;++l) {
                ComPtr<IDirect3DSurface9> s,sys;tb->GetSurfaceLevel(l,&s);if(!s) return -6;
                D3DSURFACE_DESC desc{};s->GetDesc(&desc);
                if(FAILED(dev->CreateOffscreenPlainSurface(desc.Width,desc.Height,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&sys,nullptr))) return -7;
                if(FAILED(dev->GetRenderTargetData(s.Get(),sys.Get()))) return -8;
                D3DLOCKED_RECT lr{};if(FAILED(sys->LockRect(&lr,nullptr,D3DLOCK_READONLY))) return -9;
                DWORD px=*reinterpret_cast<const DWORD*>(static_cast<const unsigned char*>(lr.pBits)+lr.Pitch*(desc.Height/2)+4*(desc.Width/2));
                sys->UnlockRect();
                if(px!=colors[l]) {log("Decoupled self-test: level %u read %08lX expected %08lX",l,(unsigned long)px,(unsigned long)colors[l]);return int(l)+1;}
            }
            return 0;
        };
        int single=testLevels(1);
        if(single!=0) {log("Decoupled self-test code %d",single);return fail("shared texture self-test",E_FAIL);}
        int mip=testLevels(3);mips=mip==0;
        if(FAILED(hr=dev->CreateQuery(D3DQUERYTYPE_EVENT,&query))) return fail("CreateQuery",hr);
        log("Probe 0.8.54: VR GPU timestamp queries %s",gpuTimer.init(dev.Get())?"available":"unavailable");
        {
            int want=iniInt(L"Antialiasing",4);
            D3DMULTISAMPLE_TYPE t=want>=8?D3DMULTISAMPLE_8_SAMPLES:want>=4?D3DMULTISAMPLE_4_SAMPLES:want>=2?D3DMULTISAMPLE_2_SAMPLES:D3DMULTISAMPLE_NONE;
            while(t!=D3DMULTISAMPLE_NONE&&(FAILED(ex->CheckDeviceMultiSampleType(cp.AdapterOrdinal,D3DDEVTYPE_HAL,D3DFMT_A8R8G8B8,TRUE,t,nullptr))||
                  FAILED(ex->CheckDeviceMultiSampleType(cp.AdapterOrdinal,D3DDEVTYPE_HAL,D3DFMT_D24S8,TRUE,t,nullptr))))
                t=D3DMULTISAMPLE_TYPE(int(t)/2>=2?int(t)/2:0);
            samples=t;
            log("Headset images: %dx antialiasing, room and wooden table %s",int(samples),environment?"on":"off (Environment=0)");
        }
        D3DCAPS9 caps{};dev->GetDeviceCaps(&caps);
        log("Decoupled VR rendering ready: second D3D9Ex device on adapter %u, sharing self-test OK (mip chains %s, code %d), clip planes %lu, pan prediction %s",
            cp.AdapterOrdinal,mips?"shared":"unavailable",mip,(unsigned long)caps.MaxUserClipPlanes,panPrediction?"on":"off");
        sStart=milliseconds();
        thread=CreateThread(nullptr,0,&entry,this,0,nullptr);
        if(!thread) return fail("CreateThread",HRESULT_FROM_WIN32(GetLastError()));
        return true;
    }
    // Game thread: hands a completed package (its GPU work is finished).
    void handOff(std::shared_ptr<Package> p) {
        p->handedMs=milliseconds();
        std::lock_guard<std::mutex> g(m);
        mailbox=std::move(p);
    }
    // Game thread: copies with lastPackage below this value can no longer be read by the VR thread.
    uint64_t safeBelow() {
        std::lock_guard<std::mutex> g(m);
        uint64_t s=inUse;
        if(mailbox) s=std::min(s,mailbox->id);
        return s;
    }
    void deactivate() {active=false;}
    void dropMailbox() {std::lock_guard<std::mutex> g(m);mailbox.reset();}
    void dropAll() {std::lock_guard<std::mutex> g(m);mailbox.reset();++epoch;}
    static DWORD WINAPI entry(void* self) {
        auto* t=static_cast<VrThread*>(self);
        SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);
        timeBeginPeriod(1);
        CallTiming::vrThread=GetCurrentThreadId();
        log("VR thread started (%lu)",(unsigned long)GetCurrentThreadId());
        try {t->loop();} catch(...) {log("VR thread exception; decoupled rendering stopped");t->failed=true;t->active=false;}
        return 0;
    }
    void releaseCurrent() {
        if(!current) return;
        std::lock_guard<std::mutex> g(m);current.reset();inUse=NoPackage;
        interp.prev.reset();probePrev.reset();interp.display.clear();interp.active=false;interp.adoptMs=0;interp.curId=0;possMotion.reset();
    }
    void loop() {
        LONG lastSerial=-1;bool adoptNew=false;
        auxEnabled=iniInt(L"Possession360",1)!=0;
        interp.enabled=iniInt(L"MotionInterpolation",0)!=0;motionProbe.enabled=iniInt(L"MotionProbe",0)!=0;
        interp.snapDist=iniFloat(L"InterpolationSnap",.15f,.005f,10.f);interp.cameraDist=iniFloat(L"InterpolationCamera",.001f,.00005f,1.f);
        interp.shrink=iniFloat(L"InterpolationSpan",.9f,.3f,1.5f);
        log("Motion interpolation: %s (snap %.2f, camera %.4f, span %.2f)",interp.enabled?"ON":"off",interp.snapDist,interp.cameraDist,interp.shrink);
        possMotion.predictTurn=iniInt(L"PossessionPredictTurn",1)!=0;possMotion.useYaw=iniInt(L"PossessionUseCameraYaw",1)!=0;possMotion.useCam=iniInt(L"PossessionUseCameraPosition",1)!=0;possMotion.stableSwayLead=iniFloat(L"PossessionSwayLead",.5f,0.f,1.5f);possMotion.stableForwardLead=iniFloat(L"PossessionWalkLead",1.f,0.f,1.5f);possMotion.stableHeightLead=iniFloat(L"PossessionHeightLead",0.f,0.f,1.5f);possMotion.predictMove=iniInt(L"PossessionPredictMove",1)!=0;
        possMotion.smoothMs=iniFloat(L"PossessionSmoothMs",100.f,20.f,400.f);possMotion.moveSmoothMs=iniFloat(L"PossessionWalkSmoothMs",45.f,4.f,400.f);possMotion.swaySmoothMs=iniFloat(L"PossessionSwaySmoothMs",150.f,4.f,800.f);possMotion.heightSmoothMs=iniFloat(L"PossessionHeightSmoothMs",120.f,20.f,2000.f);possMotion.stabilizeHeight=iniInt(L"PossessionStabilizeHeight",1)!=0;possMotion.trace=iniInt(L"PossessionTrace",0)!=0;
        possMotion.interp=iniInt(L"PossessionInterpolate",1)!=0;possMotion.interpDelayMs=iniFloat(L"PossessionInterpolateMs",0.f,0.f,150.f);possMotion.interpLead=iniFloat(L"PossessionInterpolateLead",1.f,0.f,1.f);possMotion.interpExtrapMax=iniFloat(L"PossessionExtrapolateMax",.5f,0.f,2.f);possMotion.interpCorrectMs=iniFloat(L"PossessionCorrectionFadeMs",0.f,0.f,500.f);possMotion.interpVelocityMs=iniFloat(L"PossessionVelocityMs",0.f,0.f,200.f);possMotion.interpStick=iniInt(L"PossessionStickStop",0)!=0;possMotion.clean=iniInt(L"PossessionCleanCamera",0)!=0;possMotion.cleanFastMs=iniFloat(L"PossessionCleanFastMs",80.f,20.f,500.f);possMotion.cleanIdleMs=iniFloat(L"PossessionCleanIdleMs",400.f,50.f,3000.f);possMotion.cleanPathMs=iniFloat(L"PossessionCleanPathMs",2000.f,100.f,5000.f);possMotion.cleanYawMs=iniFloat(L"PossessionCleanYawMs",4000.f,20.f,12000.f);possMotion.cleanNotch=iniInt(L"PossessionCleanPosition",0)!=0;possMotion.gaitHz=iniFloat(L"PossessionGaitHz",.64f,.3f,1.6f);possMotion.f0Now=possMotion.gaitHz;possMotion.gaitAuto=iniInt(L"PossessionGaitAuto",1)!=0;possMotion.gaitQ=iniFloat(L"PossessionGaitQ",3.f,.5f,6.f);possMotion.gaitLeadMs=iniFloat(L"PossessionGaitLeadMs",50.f,0.f,600.f);possMotion.gaitSmoothMs=iniFloat(L"PossessionGaitSmoothMs",40.f,10.f,300.f);possMotion.viewGain=iniFloat(L"PossessionViewGain",2.7f,1.f,4.f);if(possMotion.clean) possMotion.gain=possMotion.viewGain;HeadingFollowShared::cfgEnabled=iniInt(L"PossessionHeadingFollow",0)!=0;HeadingFollowShared::cfgDead=iniFloat(L"PossessionFollowDeadDeg",2.f,0.f,10.f);HeadingFollowShared::cfgLowpass=iniFloat(L"PossessionFollowLowpassMs",150.f,50.f,3000.f);HeadingFollowShared::cfgTau=iniFloat(L"PossessionFollowTauMs",300.f,50.f,4000.f);HeadingFollowShared::cfgMax=iniFloat(L"PossessionFollowMaxDegPerS",120.f,10.f,360.f);possMotion.cleanHeightMs=iniFloat(L"PossessionCleanHeightMs",300.f,20.f,2000.f);possMotion.cleanGateMs=iniFloat(L"PossessionCleanGateMs",100.f,20.f,500.f);
        log("Possession clean camera (0.8.80): %s (PossessionCleanCamera) view gain %.2f fixed (PossessionViewGain), heading follow of the game %s (PossessionHeadingFollow), position %s (PossessionCleanPosition, gait %.2f Hz%s, Q %.1f, lead %.0f ms, smoother %.0f ms), path correction %.0f ms, heading offset %.0f ms, height %.0f ms, stick gate %.0f ms",possMotion.clean?"ON: heading AND position are driven by the sticks only; the game camera is not shown":"off: filters of 0.8.75",possMotion.viewGain,HeadingFollowShared::cfgEnabled.load()?"ON":"off",possMotion.cleanNotch?"GAIT NOTCH on the game position":"learned path (0.8.77)",possMotion.gaitHz,possMotion.gaitAuto?" tracked on the roll":" fixed",possMotion.gaitQ,possMotion.gaitLeadMs,possMotion.gaitSmoothMs,possMotion.cleanPathMs,possMotion.cleanYawMs,possMotion.cleanHeightMs,possMotion.cleanGateMs);
        log("Possession at the headset rate (0.8.80): turn smoothed %s (offset filter %.0f ms), walk smoothed %s (forward %.0f ms, sideways %.0f ms), height stabilised %s (%.0f ms), trace %s",possMotion.predictTurn?"ON":"off",possMotion.smoothMs,possMotion.predictMove?"ON":"off",possMotion.moveSmoothMs,possMotion.swaySmoothMs,possMotion.stabilizeHeight?"ON":"off",possMotion.heightSmoothMs,possMotion.trace?"ON":"off");
        while(!quit) {
            if(!active||failed) {releaseCurrent();Sleep(4);continue;}
            if(ipc->requestEvent) WaitForSingleObject(ipc->requestEvent,15); else Sleep(1);
            if(!active) continue;
            LONG serial=ipc->data->requestSerial;
            if(serial==lastSerial) continue;
            lastSerial=serial;
            Request r{};if(!ipc->lock(3)) continue;
            r=ipc->data->request;ipc->unlock();
            {
                std::lock_guard<std::mutex> g(m);
                if(mailbox&&(!current||mailbox->id>current->id)) {current=mailbox;inUse=current->id;adoptNew=true;}
            }
            if(adoptNew) {adoptNew=false;if(interp.enabled) interp.adopt(current);}
            {const int pm=probeMode.load();if(pm>=4&&current&&r.valid&&milliseconds()-lastRenderMs<(pm==4?22.2:33.3)) {++sCapped;continue;}}
            if(!current||!r.valid) {++sIdle;continue;}
            if(current->epoch!=epoch.load()) {++sIdle;continue;}
            lastRenderMs=milliseconds();renderFrame(r);
            report();
        }
    }
    bool ensureImages(Images& im,UINT w,UINT h,UINT dw,UINT dh) {
        if(im.w==w&&im.h==h&&im.dw==dw&&im.dh==dh&&im.color[0]) return true;
        im=Images{};im.w=w;im.h=h;im.dw=dw;im.dh=dh;HRESULT hr;
        for(int j=0;j<3;++j) {
            if(FAILED(hr=dev->CreateTexture(j==2?dw:w,j==2?dh:h,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&im.color[j],&im.handles[j]))||
               FAILED(hr=im.color[j]->GetSurfaceLevel(0,&im.target[j]))) {log("VR thread: shared eye texture failed %08lX",(unsigned long)hr);im=Images{};return false;}
        }
        if(FAILED(hr=dev->CreateDepthStencilSurface(w,h,D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,TRUE,&im.depth,nullptr))) {log("VR thread: depth surface failed %08lX",(unsigned long)hr);im=Images{};return false;}
        im.generation=++sharedGenerations;
        log("VR thread: shared stereo textures %ux%u; desktop %ux%u",w,h,dw,dh);
        if(samples!=D3DMULTISAMPLE_NONE&&(msaaW!=w||msaaH!=h)) {
            msaaColor.Reset();msaaDepth.Reset();msaaW=msaaH=0;
            if(FAILED(hr=dev->CreateRenderTarget(w,h,D3DFMT_A8R8G8B8,samples,0,FALSE,&msaaColor,nullptr))||
               FAILED(hr=dev->CreateDepthStencilSurface(w,h,D3DFMT_D24S8,samples,0,TRUE,&msaaDepth,nullptr))) {
                log("VR thread: antialiased surfaces failed %08lX; antialiasing off",(unsigned long)hr);samples=D3DMULTISAMPLE_NONE;msaaColor.Reset();msaaDepth.Reset();
            } else {msaaW=w;msaaH=h;}
        }
        return true;
    }
    struct HudVertex {Vec3 p;DWORD c;float u,v;};
    // Textured quad (centre pose, size) with the given texture rectangle (u0,v0 top-left).
    static void quad(std::vector<HudVertex>& out,const Pose& pose,float w,float h,float u0,float v0,float u1,float v1,DWORD c=0xffffffff) {
        Vec3 a=toWorld(pose,{-w*.5f,h*.5f,0}),b=toWorld(pose,{w*.5f,h*.5f,0}),cc=toWorld(pose,{w*.5f,-h*.5f,0}),dd=toWorld(pose,{-w*.5f,-h*.5f,0});
        HudVertex v[4]={{a,c,u0,v0},{b,c,u1,v0},{cc,c,u1,v1},{dd,c,u0,v1}};
        for(int k:{0,1,2,0,2,3}) out.push_back(v[k]);
    }
    // 0.8.0: numbered test panels on the two side walls of the room (user request: compare the sources side by side).
    // 1 = the game's whole image (the dungeon is hidden in it: only interface over black must remain),
    // 2 = the interface layer (source of the bars in 0.8.0), 3 = the hand window (held creature),
    // 4 = the game image with only its black made transparent (0.7.x style, no cut-out mask).
    // Big yellow digits (7 segments) above each panel. Left wall: 1 (front) and 2 (back); right wall: 3 (front), 4 (back).
    static void rectQuad(std::vector<HudVertex>& out,const Pose& pose,float x0,float y0,float x1,float y1,float z,DWORD c) {
        Vec3 a=toWorld(pose,{x0,y1,z}),b=toWorld(pose,{x1,y1,z}),cc=toWorld(pose,{x1,y0,z}),dd=toWorld(pose,{x0,y0,z});
        HudVertex v[4]={{a,c,0,0},{b,c,1,0},{cc,c,1,1},{dd,c,0,1}};
        for(int k:{0,1,2,0,2,3}) out.push_back(v[k]);
    }
    static void digit(std::vector<HudVertex>& out,const Pose& pose,float cx,float cy,int n,DWORD c) {
        static const unsigned char segs[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; // bits a b c d e f g
        const float w=.20f,h=.34f,t=.045f,hw=w*.5f,hh=h*.5f;const unsigned char s=segs[n%10];
        if(s&1)  rectQuad(out,pose,cx-hw,cy+hh-t,cx+hw,cy+hh,.004f,c);       // a
        if(s&2)  rectQuad(out,pose,cx+hw-t,cy,cx+hw,cy+hh,.004f,c);          // b
        if(s&4)  rectQuad(out,pose,cx+hw-t,cy-hh,cx+hw,cy,.004f,c);          // c
        if(s&8)  rectQuad(out,pose,cx-hw,cy-hh,cx+hw,cy-hh+t,.004f,c);       // d
        if(s&16) rectQuad(out,pose,cx-hw,cy-hh,cx-hw+t,cy,.004f,c);          // e
        if(s&32) rectQuad(out,pose,cx-hw,cy,cx-hw+t,cy+hh,.004f,c);          // f
        if(s&64) rectQuad(out,pose,cx-hw,cy-t*.5f,cx+hw,cy+t*.5f,.004f,c);   // g
    }
    static void wallText(std::vector<HudVertex>& out,const Pose& pose,float cx,float cy,const char* text,DWORD color,float k=1.f) {
        auto rows=[](char c)->std::array<unsigned char,7>{
            switch(c) {
            case 'A':return {14,17,17,31,17,17,17}; case 'E':return {31,16,16,30,16,16,31};
            case 'F':return {31,16,16,30,16,16,16}; case 'G':return {14,17,16,23,17,17,14};
            case 'I':return {31,4,4,4,4,4,31}; case 'N':return {17,25,21,19,17,17,17};
            case 'O':return {14,17,17,17,17,17,14}; case 'P':return {30,17,17,30,16,16,16};
            case 'R':return {30,17,17,30,20,18,17}; case 'S':return {15,16,16,14,1,1,30};
            case 'T':return {31,4,4,4,4,4,4}; case 'U':return {17,17,17,17,17,17,14};
            case 'V':return {17,17,17,17,17,10,4}; case 'Y':return {17,17,10,4,4,4,4}; case 'X':return {17,17,10,4,10,17,17}; case 'M':return {17,27,21,21,17,17,17};
            case 'L':return {16,16,16,16,16,16,31}; case 'H':return {17,17,17,31,17,17,17};
            case 'D':return {30,17,17,17,17,17,30}; case 'C':return {14,17,16,16,16,17,14};
            case '0':return {14,17,19,21,25,17,14}; case '1':return {4,12,4,4,4,4,14};
            case '2':return {14,17,1,2,4,8,31}; case '3':return {30,1,1,14,1,1,30};
            case '4':return {2,6,10,18,31,2,2}; case '5':return {31,16,16,30,1,1,30};
            case '6':return {14,16,16,30,17,17,14}; case '9':return {14,17,17,15,1,1,14};
            case '7':return {31,1,2,4,8,8,8}; case '8':return {14,17,17,14,17,17,14}; case 'B':return {30,17,17,30,17,17,30}; case 'Z':return {31,1,2,4,8,16,31}; case '-':return {0,0,0,31,0,0,0};
            default:return {0,0,0,0,0,0,0};
            }
        };
        const size_t n=strlen(text);const float pixel=.010f*k,advance=.060f*k;
        const float left=cx-float(n)*advance*.5f+.005f;
        for(size_t i=0;i<n;++i) {auto bits=rows(text[i]);for(int y=0;y<7;++y)for(int x=0;x<5;++x)if(bits[y]&(1u<<(4-x))) {
            float px=left+float(i)*advance+float(x)*pixel,py=cy+(3.f-float(y))*pixel;
            rectQuad(out,pose,px,py,px+pixel*.85f,py+pixel*.85f,.004f,color);
        }}
    }
    static void discFan(std::vector<HudVertex>& out,const Pose& pose,float cx,float cy,float radius,float z,DWORD c) {
        const int N=20;
        for(int k=0;k<N;++k) {
            const float a0=2*Pi*k/N,a1=2*Pi*(k+1)/N;
            const Vec3 o=toWorld(pose,{cx,cy,z}),p0=toWorld(pose,{cx+std::cos(a0)*radius,cy+std::sin(a0)*radius,z}),p1=toWorld(pose,{cx+std::cos(a1)*radius,cy+std::sin(a1)*radius,z});
            out.push_back({o,c,.5f,.5f});out.push_back({p0,c,.5f,.5f});out.push_back({p1,c,.5f,.5f});
        }
    }
    // 0.8.94.5: colour of the PASSTHROUGH VD background, from the three sliders (default pure green 0,255,0).
    static DWORD passthroughClear(const Request& r) {
        float c[3];passthroughRgb(r,c);
        return 0xff000000u|(DWORD(c[0]+.5f)<<16)|(DWORD(c[1]+.5f)<<8)|DWORD(c[2]+.5f);
    }
    // 0.9.47.41: text of the options panel in the style of the game (serif face, gold lettering with a dark outline). The strings are rendered ONCE with GDI into
    // an atlas texture (3 styles: 0 gold, 1 cream, 2 engraved black for the stone OPTIONS button); if anything fails the old pixel font is used.
    struct WallGlyph {float u0=0,v0=0,u1=0,v1=0,aspect=1;};
    ComPtr<IDirect3DTexture9> wallFontTex,wallStoneTex;std::map<std::string,WallGlyph> wallGlyphs[4];bool wallFontTried=false;   // styles: 0 gold, 1 cream, 2 engraved black, 3 dark brown with a pale edge (key under the laser)
    void wallFontBuild(IDirect3DDevice9Ex* d) {
        if(wallFontTried) return;wallFontTried=true;
        wallStoneBuild(d);
        struct E {std::string s;int style;};
        std::vector<E> list={{"SNAP TURN",0},{"LASER COLOR",0},{"INVERT MAP",0},{"PRIMARY HAND",0},{"PASSTHROUGH VD",0},{"TABLE SIZE",0},{"OFF HAND ORB",0},{"PC MIRROR",0},
            {"ON",1},{"OFF",1},{"LEFT",1},{"RIGHT",1},{"SHOW",1},{"HIDE",1},{"15",1},{"30",1},{"45",1},{"90",1},{"X",1},
            {"0",1},{"1",1},{"2",1},{"3",1},{"4",1},{"5",1},{"6",1},{"7",1},{"8",1},{"9",1},{"OPTIONS",2},{"CONTROLS",2}};
        // 0.9.47.42: VR keyboard lettering (one glyph per character the layout can give, plus the special keys), cream and dark
        for(int style:{1,3}) {
            for(int ch=33;ch<127;++ch) {if(ch>='a'&&ch<='z') continue;if(style==1&&((ch>='0'&&ch<='9')||ch=='X')) continue;list.push_back({std::string(1,char(ch)),style});}
            for(const char* k:{"MAJ","EFFACER","ENTREE","ESC","ESPACE","FERMER"}) list.push_back({k,style});
        }
        // 0.9.47.49: CONTROLS panel lettering, composed character by character: gold (style 0) = every printable ASCII + accents, cream (style 1) = lowercase + accents
        {
            static const char* acc[]={"\xC3\xA9","\xC3\xA8","\xC3\xAA","\xC3\xA2","\xC3\xA0","\xC3\xA7","\xC3\xB4","\xC3\xAE","\xC3\xBB","\xC3\x89","\xC3\x8A","\xC3\xAB"};
            for(int style:{0,1}) {
                for(int ch=33;ch<127;++ch) {if(style==1&&!(ch>='a'&&ch<='z')) continue;list.push_back({std::string(1,char(ch)),style});}
                for(const char* a:acc) list.push_back({a,style});
            }
        }
        const int AW=1024,AH=2048,CH=64,PAD=6;
        HDC dc=CreateCompatibleDC(nullptr);if(!dc) {log("Options text (0.9.47.41): no device context, pixel font kept");return;}
        BITMAPINFO bi{};bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=AW;bi.bmiHeader.biHeight=-AH;bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;bi.bmiHeader.biCompression=BI_RGB;
        void* bits=nullptr;HBITMAP bmp=CreateDIBSection(dc,&bi,DIB_RGB_COLORS,&bits,nullptr,0);
        if(!bmp||!bits) {DeleteDC(dc);log("Options text (0.9.47.41): bitmap failed, pixel font kept");return;}
        HGDIOBJ oldBmp=SelectObject(dc,bmp);memset(bits,0,size_t(AW)*AH*4);
        static const wchar_t* faces[]={L"Palatino Linotype",L"Book Antiqua",L"Georgia",L"Cambria",L"Times New Roman"};
        HFONT font=nullptr;wchar_t got[64]{};const wchar_t* usedFace=L"Times New Roman";
        for(const wchar_t* f:faces) {
            HFONT h=CreateFontW(-46,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_ROMAN,f);
            if(!h) continue;HGDIOBJ o=SelectObject(dc,h);got[0]=0;GetTextFaceW(dc,64,got);
            if(_wcsicmp(got,f)==0) {font=h;usedFace=f;break;}
            SelectObject(dc,o);DeleteObject(h);
        }
        if(!font) {font=CreateFontW(-46,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_ROMAN,L"Times New Roman");SelectObject(dc,font);}
        SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGB(255,255,255));
        struct Cell {int x,y,w;int style;std::string s;};std::vector<Cell> cells;
        int cx=0,cy=0;
        for(const E& e:list) {
            wchar_t wbuf[64]{};const int wn=MultiByteToWideChar(CP_UTF8,0,e.s.c_str(),int(e.s.size()),wbuf,63);SIZE sz{};GetTextExtentPoint32W(dc,wbuf,wn,&sz);
            const int w=sz.cx+2*PAD;if(cx+w>AW) {cx=0;cy+=CH;}
            if(cy+CH>AH) break;
            TextOutW(dc,cx+PAD,cy+(CH-sz.cy)/2,wbuf,wn);
            cells.push_back({cx,cy,w,e.style,e.s});cx+=w;
        }
        GdiFlush();
        std::vector<unsigned char> mask(size_t(AW)*AH);
        for(size_t i=0;i<mask.size();++i) mask[i]=static_cast<const unsigned char*>(bits)[i*4+1];
        SelectObject(dc,oldBmp);DeleteObject(font);DeleteObject(bmp);DeleteDC(dc);
        std::vector<uint32_t> out(size_t(AW)*AH,0u);
        auto M=[&](int x,int y)->int{return x<0||y<0||x>=AW||y>=AH?0:mask[size_t(y)*AW+x];};
        for(const Cell& c:cells) for(int y=c.y;y<c.y+CH;++y) for(int x=c.x;x<c.x+c.w;++x) {
            const int m=M(x,y);float ar=0,cr=0,cg=0,cb=0;
            const float t=float(y-c.y)/float(CH);
            auto over=[&](float ca,float r,float g,float b) { // composite (r,g,b,ca) over the running colour
                const float na=ca+ar*(1.f-ca);if(na<=0.f) return;
                cr=(r*ca+cr*ar*(1.f-ca))/na;cg=(g*ca+cg*ar*(1.f-ca))/na;cb=(b*ca+cb*ar*(1.f-ca))/na;ar=na;};
            if(c.style==2) {   // engraved: pale edge below / right of the black letters
                const int hl=std::max(0,M(x-1,y-2)-m);over(float(hl)/255.f*.85f,178,158,128);
                if(m) {const float k=t;over(float(m)/255.f,26.f-14.f*k,20.f-12.f*k,15.f-9.f*k);}
            } else if(c.style==3) {   // dark letter, thin pale edge
                int dil=m;for(int dy=-1;dy<=1;++dy) for(int dx=-1;dx<=1;++dx) dil=std::max(dil,M(x+dx,y+dy));
                over(float(dil)/255.f*.9f,244,226,180);
                if(m) over(float(m)/255.f,48,28,12);
            } else {
                int dil=m;for(int dy=-2;dy<=2;++dy) for(int dx=-2;dx<=2;++dx) if(dx*dx+dy*dy<=5) dil=std::max(dil,M(x+dx,y+dy));
                over(float(dil)/255.f,22,12,5);
                if(m) {
                    // vertical gradient over the letter height (cap height ~ rows 14..50)
                    const float g=std::clamp((float(y-c.y)-12.f)/38.f,0.f,1.f);
                    if(c.style==0) over(float(m)/255.f,255.f-45.f*g,226.f-92.f*g,138.f-100.f*g);   // gold
                    else over(float(m)/255.f,252.f-30.f*g,242.f-42.f*g,218.f-62.f*g);               // cream
                }
            }
            const DWORD A=DWORD(std::clamp(ar*255.f+.5f,0.f,255.f));
            out[size_t(y)*AW+x]=(A<<24)|(DWORD(std::clamp(cr+.5f,0.f,255.f))<<16)|(DWORD(std::clamp(cg+.5f,0.f,255.f))<<8)|DWORD(std::clamp(cb+.5f,0.f,255.f));
        }
        ComPtr<IDirect3DTexture9> sys;
        if(FAILED(d->CreateTexture(AW,AH,1,0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&sys,nullptr))) {log("Options text (0.9.47.41): texture failed, pixel font kept");return;}
        D3DLOCKED_RECT lr{};if(FAILED(sys->LockRect(0,&lr,nullptr,0))) {log("Options text (0.9.47.41): lock failed, pixel font kept");return;}
        for(int y=0;y<AH;++y) memcpy(static_cast<unsigned char*>(lr.pBits)+size_t(y)*lr.Pitch,&out[size_t(y)*AW],size_t(AW)*4);
        sys->UnlockRect(0);
        ComPtr<IDirect3DTexture9> tex;
        if(FAILED(d->CreateTexture(AW,AH,1,0,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&tex,nullptr))||FAILED(d->UpdateTexture(sys.Get(),tex.Get()))) {log("Options text (0.9.47.41): video texture failed, pixel font kept");return;}
        for(const Cell& c:cells) {WallGlyph g;g.u0=float(c.x)/AW;g.u1=float(c.x+c.w)/AW;g.v0=float(c.y)/AH;g.v1=float(c.y+CH)/AH;g.aspect=float(c.w)/float(CH);wallGlyphs[c.style][c.s]=g;}
        wallFontTex=tex;
        log("Options text (0.9.47.41): serif face '%ls', %u strings in a %dx%d atlas",usedFace,unsigned(cells.size()),AW,AH);
    }
    // 0.9.47.42: the stone of the room walls (Textures/mur_color.jpg) for the OPTIONS block and the VR keyboard
    void wallStoneBuild(IDirect3DDevice9Ex* d) {
        const std::wstring file=(configurationPath().parent_path()/L"Textures"/L"mur_color.jpg").wstring();
        int w=0,h=0;std::vector<uint32_t> px;
        if(!env::Scene::loadImageFile(file,1024,w,h,px)||w<8||h<8) {log("Wall stone texture (0.9.47.42): %ls missing or unreadable, flat colours used",file.c_str());return;}
        ComPtr<IDirect3DTexture9> sys,tex;
        if(FAILED(d->CreateTexture(UINT(w),UINT(h),1,0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&sys,nullptr))) return;
        D3DLOCKED_RECT lr{};if(FAILED(sys->LockRect(0,&lr,nullptr,0))) return;
        for(int y=0;y<h;++y) {uint32_t* row=reinterpret_cast<uint32_t*>(static_cast<unsigned char*>(lr.pBits)+size_t(y)*lr.Pitch);for(int x=0;x<w;++x) row[x]=px[size_t(y)*w+x]|0xff000000u;}
        sys->UnlockRect(0);
        if(FAILED(d->CreateTexture(UINT(w),UINT(h),1,0,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&tex,nullptr))||FAILED(d->UpdateTexture(sys.Get(),tex.Get()))) return;
        wallStoneTex=tex;log("Wall stone texture (0.9.47.42): %ls %dx%d loaded",file.c_str(),w,h);
    }
    // rectangle textured with the wall stone: the texture follows the position (unit space * scale), so neighbouring pieces continue the same stone
    static void stoneRect(std::vector<HudVertex>& out,const Pose& pose,float x0,float y0,float x1,float y1,float z,DWORD c,float k) {
        Vec3 a=toWorld(pose,{x0,y1,z}),b=toWorld(pose,{x1,y1,z}),cc=toWorld(pose,{x1,y0,z}),dd=toWorld(pose,{x0,y0,z});
        HudVertex v[4]={{a,c,x0*k,-y1*k},{b,c,x1*k,-y1*k},{cc,c,x1*k,-y0*k},{dd,c,x0*k,-y0*k}};
        for(int q:{0,1,2,0,2,3}) out.push_back(v[q]);
    }
    // 0.9.47.49: a word / sentence composed of single-character cells (UTF-8: only the Latin-1 accents of the atlas), left / centre / right anchored on x
    static std::vector<std::string> utf8Chars(const char* t) {
        std::vector<std::string> out;
        for(size_t i=0;t&&t[i];) {const unsigned char c=static_cast<unsigned char>(t[i]);const size_t n=(c>=0xC0&&t[i+1])?2:1;out.emplace_back(t+i,n);i+=n;}
        return out;
    }
    float wordWidth(int style,const char* t,float H) {
        float w=0;
        for(const std::string& c:utf8Chars(t)) {
            if(c==" ") {w+=.30f*H;continue;}
            auto it=wallGlyphs[style].find(c);w+=(it==wallGlyphs[style].end()?.55f:it->second.aspect-.15f)*H;
        }
        return w;
    }
    void word(std::vector<HudVertex>& out,const Pose& pose,float x,float cy,float z,int style,const char* t,float H,float maxW,int align) {
        float w=wordWidth(style,t,H);if(w>maxW&&w>0) {H*=maxW/w;w=maxW;}
        float px=align<0?x:align==0?x-w*.5f:x-w;
        for(const std::string& c:utf8Chars(t)) {
            if(c==" ") {px+=.30f*H;continue;}
            auto it=wallGlyphs[style].find(c);if(it==wallGlyphs[style].end()) {px+=.55f*H;continue;}
            const float a=(it->second.aspect-.15f)*H;
            textQuad(out,pose,px+a*.5f,cy,z,it->second,H,1e9f);px+=a;
        }
    }
    void textQuad(std::vector<HudVertex>& out,const Pose& pose,float cx,float cy,float z,const WallGlyph& g,float H,float maxW) {
        float W=H*g.aspect;if(W>maxW) {W=maxW;H=W/g.aspect;}
        Pose tp=pose;tp.position=toWorld(pose,{cx,cy,z});quad(out,tp,W,H,g.u0,g.v0,g.u1,g.v1);
    }
    // one textured pass: tex == font atlas -> alpha blended lettering; tex == stone -> opaque, colour = texture x vertex colour
    void texturedPass(IDirect3DDevice9Ex* d,const std::vector<HudVertex>& vv,IDirect3DTexture9* tex,bool lettering,bool zwrite) {
        if(vv.empty()||!tex) return;
        d->SetTexture(0,tex);
        d->SetRenderState(D3DRS_ALPHABLENDENABLE,lettering?TRUE:FALSE);d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
        d->SetRenderState(D3DRS_ZWRITEENABLE,zwrite?TRUE:FALSE);
        if(lettering) {
            d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
            d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
            d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
        } else {
            d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);d->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
            d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
            d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
        }
        d->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);d->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);d->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_NONE);
        if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(vv.size()/3),vv.data(),sizeof(HudVertex)))) ++sDrawFailures;
        d->SetTexture(0,nullptr);d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
        d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
        d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
    }
    // 0.9.47.41: options panel on the LEFT wall, stuck to the stone (brick backing, bronze frame, serif lettering). The layout (Comfort.hpp) is shared with the host hit test.
    void drawComfortWall(IDirect3DDevice9Ex* d,const Request& r,const D3DMATRIX& eyeWorld,const HudLayout* hudMask=nullptr,bool screenWhole=false,bool inMenuRoom=false,int which=0) {   // which: 0 = OPTIONS wall, 1 = CONTROLS wall (0.9.47.49)
        wallFontBuild(d);
        std::vector<HudVertex> v;v.reserve(6000);std::vector<HudVertex> tv;tv.reserve(1200);
        const Pose pose=which?controlsWallPose(r):comfortWallPose(r);
        const bool snapOn=(r.comfortFlags&ComfortSnapTurn)!=0,invert=(r.comfortFlags&ComfortInvertTurn)!=0,leftHand=(r.pointerFlags&HudLeftHanded)!=0,orbHidden=(r.comfortFlags&ComfortHideOrb)!=0;
        const uint32_t colour=std::min<uint32_t>((r.comfortFlags&ComfortLaserMask)>>ComfortLaserShift,ComfortColours-1);
        const uint32_t hover=r.comfortHover;
        const DWORD frame=0xff6e5028,frameDark=0xff2a1f16,gold=0xffd2a03c,mortar=0xff241c15,on=0xff2b5a35,off=0xff6a2a22,hoverC=0xffffd36b,neutral=0xff3b3028,track=0xff160f0a;
        const bool open=which?controlsOpen(r):comfortPanelOpen(r);
        auto glyphOf=[&](int style,const char* s)->const WallGlyph* {if(!wallFontTex) return nullptr;auto it=wallGlyphs[style].find(s);return it==wallGlyphs[style].end()?nullptr:&it->second;};
        // text centred on (cx,cy), cap height ~ 0.62 H, never wider than maxW
        auto text=[&](float cx,float cy,const char* s,int style,float H,float maxW) {
            const WallGlyph* g=glyphOf(style,s);
            if(!g) {wallText(v,pose,cx,cy,s,style==2?0xff101010:style==0?0xffe6c36a:0xffffffff);return;}
            float W=H*g->aspect;if(W>maxW) {W=maxW;H=W/g->aspect;}
            Pose tp=pose;tp.position=toWorld(pose,{cx,cy,.006f});quad(tv,tp,W,H,g->u0,g->v0,g->u1,g->v1);
        };
        auto number=[&](float cx,float cy,unsigned val,float H) {
            char buf[12];snprintf(buf,sizeof(buf),"%u",val);const size_t n=strlen(buf);
            const WallGlyph* g0=glyphOf(1,"0");
            if(!g0) {wallText(v,pose,cx,cy,buf,0xffffffff);return;}
            const float adv=H*g0->aspect*.80f;float x=cx-adv*float(n-1)*.5f;
            for(size_t i=0;i<n;++i,x+=adv) {char one[2]={buf[i],0};text(x,cy,one,1,H,1.f);}
        };
        auto bevel=[&](float x0,float y0,float x1,float y1,float z,DWORD light,DWORD dark,float t) {   // raised block: light top/left edge, dark bottom/right edge
            rectQuad(v,pose,x0,y1-t,x1,y1,z,light);rectQuad(v,pose,x0,y0,x0+t,y1,z,light);
            rectQuad(v,pose,x0,y0,x1,y0+t,z,dark);rectQuad(v,pose,x1-t,y0,x1,y1,z,dark);
        };
        auto hash=[](int a,int b)->float{unsigned h=unsigned(a)*73856093u^unsigned(b)*19349663u;h^=h>>13;h*=1274126177u;h^=h>>16;return float(h&1023u)/1023.f;};
        auto shade=[](float k,int rr,int gg,int bb)->DWORD{return 0xff000000u|(DWORD(std::clamp(rr*k,0.f,255.f))<<16)|(DWORD(std::clamp(gg*k,0.f,255.f))<<8)|DWORD(std::clamp(bb*k,0.f,255.f));};
        // Everything below is built in "unit space" (the 0.8.85 layout) and shrunk about the panel centre by ComfortPanelScale at the end.
        if(open&&which==0) {
        const float HW=ComfortPanelHalfW,HH=ComfortPanelHalfH;
        // 0.9.47.42: NO frame and NO background: the controls and the lettering sit directly on the wall stone.
        static const char* titles[4]={"SNAP TURN","LASER COLOR","INVERT MAP","PRIMARY HAND"};
        const float titleX[4]={ComfortColA,ComfortColB,ComfortColA,ComfortColB};
        for(int k=0;k<4;++k) text(titleX[k],ComfortLabelY[k],titles[k],0,.125f,.80f);
        text(ComfortPassLabelX,ComfortPassLabelY,"PASSTHROUGH VD",0,.125f,.90f);
        const bool passOn=(r.comfortFlags&ComfortPassthrough)!=0;
        float prgb[3];passthroughRgb(r,prgb);
        text(ComfortTableLabelX,ComfortTableY,"TABLE SIZE",0,.125f,.62f);   // 0.9.35
        text(ComfortOrbLabelX,ComfortOrbY,"OFF HAND ORB",0,.125f,.72f);   // 0.9.47.41
        text(ComfortMirrorLabelX,ComfortMirrorY,"PC MIRROR",0,.125f,.72f);   // 0.9.47.61
        const float tablePct=tableSizePercent(r);
        ComfortRect items[ComfortMaxItems];const int n=comfortLayout(items);
        static const char* angles[]={"15","30","45","90"};
        const uint32_t angle=r.possessionSkill;
        for(int k=0;k<n;++k) {
            const ComfortRect& it=items[k];const bool h=hover==it.id;
            const float cx=(it.x0+it.x1)*.5f,cy=(it.y0+it.y1)*.5f;
            if(it.id==ComfortItemSliderTable) {   // 0.9.35: 50..250 % of TableWidth, 100 = the size of DK2VR.ini
                const float f=(std::clamp(tablePct,TableSizeMinPct,TableSizeMaxPct)-TableSizeMinPct)/(TableSizeMaxPct-TableSizeMinPct),sy=ComfortTableY;
                rectQuad(v,pose,ComfortZoomTrackX0-.012f,sy-ComfortTrackHalf-.012f,ComfortZoomTrackX1+.012f,sy+ComfortTrackHalf+.012f,-.006f,h?hoverC:frame);
                rectQuad(v,pose,ComfortZoomTrackX0,sy-ComfortTrackHalf,ComfortZoomTrackX1,sy+ComfortTrackHalf,-.004f,track);
                const float hx=ComfortZoomTrackX0+f*(ComfortZoomTrackX1-ComfortZoomTrackX0);
                if(f>0.f) rectQuad(v,pose,ComfortZoomTrackX0,sy-ComfortTrackHalf,hx,sy+ComfortTrackHalf,-.003f,0xffd2a03c);
                const float mark=ComfortZoomTrackX0+(100.f-TableSizeMinPct)/(TableSizeMaxPct-TableSizeMinPct)*(ComfortZoomTrackX1-ComfortZoomTrackX0);   // tick at 100 %
                rectQuad(v,pose,mark-.004f,sy-.050f,mark+.004f,sy+.050f,-.002f,0xff8a7a60);
                rectQuad(v,pose,hx-.026f,sy-.067f,hx+.026f,sy+.067f,-.001f,frameDark);
                rectQuad(v,pose,hx-.020f,sy-.060f,hx+.020f,sy+.060f,.001f,h?hoverC:0xffc9a35a);
                rectQuad(v,pose,hx-.020f,sy+.040f,hx+.020f,sy+.060f,.0015f,0xffe8cf93);
                number(ComfortValueX+.10f,sy,unsigned(tablePct+.5f),.125f);
                continue;
            }
            if(comfortIsSlider(it.id)) {
                const int ch=int(it.id-ComfortItemSliderR);const float val=std::clamp(prgb[ch],0.f,255.f),f=val/255.f,sy=ComfortSliderY[ch];
                const DWORD chan=ch==0?0xffff0000:ch==1?0xff00ff00:0xff0000ff;   // 0.8.94.5.1: information only, always the full colour (255), never follows the slider
                const DWORD full=ch==0?0xffe03a2e:ch==1?0xff3cd25a:0xff3f78ff;
                rectQuad(v,pose,ComfortSwatchX0-.014f,sy-ComfortSwatchHalf-.014f,ComfortSwatchX1+.014f,sy+ComfortSwatchHalf+.014f,-.006f,h?hoverC:frame);   // square of the channel colour, live
                rectQuad(v,pose,ComfortSwatchX0,sy-ComfortSwatchHalf,ComfortSwatchX1,sy+ComfortSwatchHalf,-.003f,chan);
                rectQuad(v,pose,ComfortTrackX0-.012f,sy-ComfortTrackHalf-.012f,ComfortTrackX1+.012f,sy+ComfortTrackHalf+.012f,-.006f,h?hoverC:frame);
                rectQuad(v,pose,ComfortTrackX0,sy-ComfortTrackHalf,ComfortTrackX1,sy+ComfortTrackHalf,-.004f,track);
                const float hx=ComfortTrackX0+f*(ComfortTrackX1-ComfortTrackX0);
                if(f>0.f) rectQuad(v,pose,ComfortTrackX0,sy-ComfortTrackHalf,hx,sy+ComfortTrackHalf,-.003f,full);
                rectQuad(v,pose,hx-.026f,sy-.067f,hx+.026f,sy+.067f,-.001f,frameDark);                       // handle
                rectQuad(v,pose,hx-.020f,sy-.060f,hx+.020f,sy+.060f,.001f,h?hoverC:0xffc9a35a);
                rectQuad(v,pose,hx-.020f,sy+.040f,hx+.020f,sy+.060f,.0015f,0xffe8cf93);
                number(ComfortValueX,sy,unsigned(val+.5f),.125f);
                continue;
            }
            if(it.id>=ComfortItemColour&&it.id<ComfortItemColour+ComfortColours) {
                float rgb[3];laserColourRgb(it.id-ComfortItemColour,rgb);
                const DWORD c=0xff000000u|(DWORD(rgb[0])<<16)|(DWORD(rgb[1])<<8)|DWORD(rgb[2]);
                const bool chosen=(it.id-ComfortItemColour)==colour;
                discFan(v,pose,cx,cy,chosen||h?.066f:.056f,-.006f,chosen?0xffffe9a8:h?hoverC:frameDark);   // ring: cream = selected, gold = pointed
                discFan(v,pose,cx,cy,.048f,-.005f,frame);
                discFan(v,pose,cx,cy,.042f,-.004f,c);
                continue;
            }
            if(it.id==ComfortItemClose) {
                rectQuad(v,pose,it.x0-.014f,it.y0-.014f,it.x1+.014f,it.y1+.014f,-.006f,h?hoverC:frame);
                rectQuad(v,pose,it.x0,it.y0,it.x1,it.y1,-.003f,0xff7a2a2a);
                bevel(it.x0,it.y0,it.x1,it.y1,-.002f,0xffa5463a,0xff4a1816,.012f);
                text(cx,cy,"X",1,.15f,(it.x1-it.x0)*.8f);
                continue;
            }
            bool active=false;const char* label="";
            switch(it.id) {
                case ComfortItemSnap:active=snapOn;label=!snapOn?"OFF":angles[angle==15?0:angle==30?1:angle==45?2:3];break;
                case ComfortItemInvert:active=invert;label=invert?"ON":"OFF";break;
                case ComfortItemPassthrough:active=passOn;label=active?"ON":"OFF";break;
                case ComfortItemHandLeft:active=leftHand;label="LEFT";break;
                case ComfortItemHandRight:active=!leftHand;label="RIGHT";break;
                case ComfortItemOrb:active=!orbHidden;label=orbHidden?"HIDE":"SHOW";break;
                case ComfortItemMirror:active=(r.comfortFlags&ComfortMirrorOn)!=0;label=active?"ON":"OFF";break;   // 0.9.47.61
            }
            const bool neutralStyle=it.id==ComfortItemHandLeft||it.id==ComfortItemHandRight;
            const DWORD fill=neutralStyle?(active?on:neutral):(active?on:off);
            rectQuad(v,pose,it.x0-.014f,it.y0-.014f,it.x1+.014f,it.y1+.014f,-.006f,h?hoverC:frame);
            rectQuad(v,pose,it.x0,it.y0,it.x1,it.y1,-.003f,fill);
            bevel(it.x0,it.y0,it.x1,it.y1,-.0025f,shade(1.55f,(fill>>16)&255,(fill>>8)&255,fill&255),shade(.55f,(fill>>16)&255,(fill>>8)&255,fill&255),.010f);
            text(cx,cy,label,1,.13f,(it.x1-it.x0)*.80f);
        }
        }
        std::vector<HudVertex> sv;sv.reserve(200);
        {   // OPTIONS button, always visible: a stepped block of the wall stone standing out of the wall (unit space: metres / ComfortPanelScale)
            const float bx0=ComfortButtonX0/ComfortPanelScale,bx1=ComfortButtonX1/ComfortPanelScale,by0=ComfortButtonY0/ComfortPanelScale,by1=ComfortButtonY1/ComfortPanelScale;
            const bool h=hover==(which?ControlsItemOpen:ComfortItemOpen);const float k=.30f;   // texture scale: 1 unit = .30 of the stone tile
            const float e1=.034f,e2=.024f;
            const DWORD lit=h?0xffffffff:0xfff2e6d6,mid=h?0xfffff0d0:0xffb09c86,shadow=0xff3c3228,face=open?0xff7c6c5a:(h?0xffe8d4b4:0xffa89478);
            if(wallStoneTex) {
                // outer slab: lit top / left edge, shadowed bottom / right edge (bevel)
                stoneRect(sv,pose,bx0,by0,bx1,by1,-.011f,mid,k);
                stoneRect(sv,pose,bx0,by1-e1,bx1,by1,-.0105f,lit,k);stoneRect(sv,pose,bx0,by0,bx0+e1,by1,-.0105f,lit,k);
                stoneRect(sv,pose,bx0,by0,bx1,by0+e1,-.0105f,shadow,k);stoneRect(sv,pose,bx1-e1,by0,bx1,by1,-.0105f,shadow,k);
                // raised inner panel
                const float ix0=bx0+.055f,ix1=bx1-.055f,iy0=by0+.055f,iy1=by1-.055f;
                stoneRect(sv,pose,ix0,iy0,ix1,iy1,-.010f,face,k);
                stoneRect(sv,pose,ix0,iy0,ix1,iy0+e2,-.0095f,lit,k);stoneRect(sv,pose,ix1-e2,iy0,ix1,iy1,-.0095f,lit,k);   // inner panel is hollowed: dark top / left, lit bottom / right
                stoneRect(sv,pose,ix0,iy1-e2,ix1,iy1,-.0095f,shadow,k);stoneRect(sv,pose,ix0,iy0,ix0+e2,iy1,-.0095f,shadow,k);
                stoneRect(sv,pose,ix0+e2,iy0+e2,ix1-e2,iy1-e2,-.0094f,face,k);
            } else {rectQuad(v,pose,bx0,by0,bx1,by1,-.011f,0xff5a4c3e);rectQuad(v,pose,bx0+.06f,by0+.06f,bx1-.06f,by1-.06f,-.010f,0xff7c6c5a);}
            text(0,(by0+by1)*.5f,which?"CONTROLS":"OPTIONS",2,.34f,(bx1-bx0)*.80f);
        }
        if(which==1&&open) {   // 0.9.47.49: controller guide: two tables (left hand | right hand), English / Francais tabs, directly on the wall stone
            const bool fr=controlsFrench(r);const float S=ControlsDungeonFit/ComfortPanelScale;   // 0.9.47.58 / .59: same 86 % size in the dungeon room (fits between the stone ribs) AND in the main-menu room (user request)
            struct Row {const char* kEn;const char* kFr;const char* dEn;const char* dFr;};
            static const Row leftDungeon[]={{"Stick","Stick","Move the map","@Deplacer la carte"},{"Stick click","Clic du stick","VR keyboard","Clavier VR"},{"X","X","Space","Espace"},{"Y","Y","Escape","@Echap"},{"Grip","Grip","Grab the table","Saisir la table"}};
            static const Row leftPoss[]={{"Stick","Stick","Move","@Se deplacer"},{"Stick click","Clic du stick","Run","Courir"},{"X","X","Leave possession","Quitter la possession"},{"Y","Y","Disabled","@Desactive"}};
            static const Row rightDungeon[]={{"Trigger","@Gachette","Left click","Clic gauche"},{"A","A","Right click","Clic droit"},{"Stick L/R","Stick G/D","Turn the dungeon","Tourner le donjon"},{"Stick U/D","Stick H/B","Zoom","Zoom"},{"Stick click","Clic du stick","Recenter","Recentrer"},{"Grip","Grip","Grab the table","Saisir la table"}};
            static const Row rightPoss[]={{"Trigger","@Gachette","Attack / use skill","Attaquer / skill"},{"A","A","Next skill","Skill suivant"},{"B","B","Previous skill","@Skill precedent"},{"Stick L/R","Stick G/D","Turn","Tourner"}};
            auto fix=[](const char* t)->std::string {   // '@' marks a string written without accents: the accented spelling is built here (UTF-8)
                std::string o;if(!t) return o;if(t[0]!='@') return t;
                static const struct {const char* plain;const char* acc;} map[]={{"Deplacer la carte","D\xC3\xA9""placer la carte"},{"Echap","\xC3\x89""chap"},{"Se deplacer","Se d\xC3\xA9""placer"},{"Desactive","D\xC3\xA9""sactiv\xC3\xA9"},{"Gachette","G\xC3\xA2""chette"},{"Skill precedent","Skill pr\xC3\xA9""c\xC3\xA9""dent"}};
                for(const auto& m:map) if(std::strcmp(t+1,m.plain)==0) return m.acc;
                return t+1;
            };
            const float xs[2]={-.70f,.01f};
            const char* tableTitle[2]={fr?"MAIN GAUCHE":"LEFT HAND",fr?"MAIN DROITE":"RIGHT HAND"};
            const char* secDungeon=fr?"DONJON":"DUNGEON";const char* secPoss="POSSESSION";
            const Row* dg[2]={leftDungeon,rightDungeon};const int dgN[2]={5,6};const Row* ps[2]={leftPoss,rightPoss};const int psN[2]={4,4};
            const float rowH=.06f,fh=.052f;
            for(int t=0;t<2;++t) {
                const float x0=xs[t],x1=x0+.69f;
                word(tv,pose,(x0+x1)*.5f*S,.30f*S,.006f,0,tableTitle[t],.082f*S,.60f*S,0);
                rectQuad(v,pose,x0*S,.262f*S,x1*S,.268f*S,-.004f,0xffd2a03c);   // gold rule under the table title
                rectQuad(v,pose,x0*S,.259f*S,x1*S,.262f*S,-.0035f,0xff241c15);
                auto section=[&](const char* name,float y) {word(tv,pose,(x0+.02f)*S,y*S,.006f,1,name,.040f*S,.40f*S,-1);rectQuad(v,pose,(x0+.02f)*S,(y-.026f)*S,x1*S,(y-.0235f)*S,-.003f,0xff3a2f24);};
                auto rows=[&](const Row* R,int n,float yTop) {
                    for(int i=0;i<n;++i) {
                        const float y=yTop-rowH*float(i);
                        word(tv,pose,(x0+.02f)*S,y*S,.006f,0,fix(fr?R[i].kFr:R[i].kEn).c_str(),fh*S,.22f*S,-1);
                        word(tv,pose,(x0+.27f)*S,y*S,.006f,1,fix(fr?R[i].dFr:R[i].dEn).c_str(),fh*S,.41f*S,-1);
                    }
                };
                section(secDungeon,.205f);rows(dg[t],dgN[t],.145f);
                section(secPoss,-.235f);rows(ps[t],psN[t],-.295f);
            }
            rectQuad(v,pose,-.004f*S,-.50f*S,.004f*S,.27f*S,-.003f,0xff241c15);   // divider between the two tables
            // tabs: English | Francais
            const uint32_t tabId[2]={ControlsItemTabEn,ControlsItemTabFr};const char* tabName[2]={"English","Fran\xC3\xA7""ais"};
            for(int k=0;k<2;++k) {
                const bool sel=(k==1)==fr,hv=hover==tabId[k];
                const float x0=ControlsTabX0[k]*S,x1=ControlsTabX1[k]*S,y0=ControlsTabY0*S,y1=ControlsTabY1*S,e=.020f;const float kk=.30f;
                const DWORD face=sel?(hv?0xff6a5640:0xff4a3b2c):(hv?0xffe8d4b4:0xffa89478);
                if(wallStoneTex) {
                    stoneRect(sv,pose,x0,y0,x1,y1,-.011f,hv?0xfffff0d0:0xffb09c86,kk);
                    if(sel) {   // pressed in: dark top / left, lit bottom / right
                        stoneRect(sv,pose,x0,y0,x1,y0+e,-.0105f,0xfff2e6d6,kk);stoneRect(sv,pose,x1-e,y0,x1,y1,-.0105f,0xfff2e6d6,kk);
                        stoneRect(sv,pose,x0,y1-e,x1,y1,-.0105f,0xff3c3228,kk);stoneRect(sv,pose,x0,y0,x0+e,y1,-.0105f,0xff3c3228,kk);
                    } else {    // raised: lit top / left, shadowed bottom / right
                        stoneRect(sv,pose,x0,y1-e,x1,y1,-.0105f,0xfff2e6d6,kk);stoneRect(sv,pose,x0,y0,x0+e,y1,-.0105f,0xfff2e6d6,kk);
                        stoneRect(sv,pose,x0,y0,x1,y0+e,-.0105f,0xff3c3228,kk);stoneRect(sv,pose,x1-e,y0,x1,y1,-.0105f,0xff3c3228,kk);
                    }
                    stoneRect(sv,pose,x0+e,y0+e,x1-e,y1-e,-.0100f,face,kk);
                } else {rectQuad(v,pose,x0,y0,x1,y1,-.011f,sel?0xff4a3b2c:0xffa89478);}
                word(tv,pose,(x0+x1)*.5f,(y0+y1)*.5f,.006f,sel?0:1,tabName[k],.074f*S,(x1-x0)*.80f,0);
            }
            {   // close cross
                const float x0=ControlsCloseX0*S,x1=ControlsCloseX1*S,y0=ControlsCloseY0*S,y1=ControlsCloseY1*S;const bool hv=hover==ControlsItemClose;
                rectQuad(v,pose,x0-.014f,y0-.014f,x1+.014f,y1+.014f,-.006f,hv?hoverC:frame);rectQuad(v,pose,x0,y0,x1,y1,-.003f,0xff7a2a2a);
                bevel(x0,y0,x1,y1,-.002f,0xffa5463a,0xff4a1816,.012f);
                text((x0+x1)*.5f,(y0+y1)*.5f,"X",1,.15f,(x1-x0)*.8f);
            }
        }
        for(HudVertex& q:v) q.p=add(pose.position,scale(sub(q.p,pose.position),ComfortPanelScale));
        for(HudVertex& q:tv) q.p=add(pose.position,scale(sub(q.p,pose.position),ComfortPanelScale));
        for(HudVertex& q:sv) q.p=add(pose.position,scale(sub(q.p,pose.position),ComfortPanelScale));
        d->SetTexture(0,nullptr);d->SetPixelShader(nullptr);d->SetVertexShader(nullptr);
        d->SetTransform(D3DTS_WORLD,&eyeWorld);
        d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
        d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
        d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
        // 0.9.47.45: the main-menu relief path leaves stencil / bias / scissor states set: the wall options were clipped away there
        d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);d->SetRenderState(D3DRS_DEPTHBIAS,0);d->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,0);
        d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);d->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
        if(inMenuRoom) {   // 0.9.47.46: main-menu room: side wall only, nothing in front of it; depth test off so the relief depth left by the menu cannot hide it
            d->SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);
            static int mlog=0;if(mlog++<3) log("Menu room options (0.9.47.46): wall block drawn, masked=%d pose %.2f %.2f %.2f",hudMask?1:0,pose.position.x,pose.position.y,pose.position.z);
        }d->SetRenderState(D3DRS_FOGENABLE,FALSE);d->SetRenderState(D3DRS_SRGBWRITEENABLE,FALSE);
        d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
        d->SetRenderState(D3DRS_COLORWRITEENABLE,15);d->SetRenderState(D3DRS_LIGHTING,FALSE);
        d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
        d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
        d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
        // 0.8.86.4: the game's interface covers the panel completely, even where the interface is translucent: the rectangles of the interface
        // panels are first marked in the stencil buffer (no colour, no depth), the panel is drawn only outside them, then the stencil is cleared.
        bool masked=false;
        if(hudMask) {
            std::vector<HudVertex> mk;mk.reserve(64);
            for(int i=0;i<HudCount;++i) {const HudPanel& q=hudMask->panel[i];if(q.shown&&(i!=HudScreen||screenWhole||(q.w<.9f*hudMask->panel[HudBottom].w))) quad(mk,q.pose,q.w,q.h,0,0,1,1);} // 0.8.94.5.1: a message panel narrower than the bars hides the wall panel behind it; a full-width empty one never does
            if(!mk.empty()) {
                d->SetRenderState(D3DRS_COLORWRITEENABLE,0);d->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
                d->SetRenderState(D3DRS_STENCILENABLE,TRUE);d->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_ALWAYS);d->SetRenderState(D3DRS_STENCILREF,1);
                d->SetRenderState(D3DRS_STENCILMASK,0xff);d->SetRenderState(D3DRS_STENCILWRITEMASK,0xff);
                d->SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_REPLACE);d->SetRenderState(D3DRS_STENCILFAIL,D3DSTENCILOP_KEEP);d->SetRenderState(D3DRS_STENCILZFAIL,D3DSTENCILOP_KEEP);
                if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(mk.size()/3),mk.data(),sizeof(HudVertex)))) ++sDrawFailures;
                d->SetRenderState(D3DRS_COLORWRITEENABLE,15);d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
                d->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_NOTEQUAL);d->SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_KEEP);
                masked=true;
            }
        }
        if(!v.empty()&&FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(v.size()/3),v.data(),sizeof(HudVertex)))) ++sDrawFailures;
        texturedPass(d,sv,wallStoneTex.Get(),false,false);
        texturedPass(d,tv,wallFontTex.Get(),true,false);   // lettering: alpha-blended atlas, same depth / stencil state
        if(masked) {d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);d->Clear(0,nullptr,D3DCLEAR_STENCIL,0,1,0);}
        d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
    }
    // 0.9.47.34: complete 5x7 pixel font for the VR keyboard (A-Z, 0-9, punctuation). wallText() keeps its own partial table (validated panel).
    static std::array<unsigned char,7> kbGlyph(char c) {
        switch(c) {
        case 'A':return {14,17,17,31,17,17,17}; case 'B':return {30,17,17,30,17,17,30}; case 'C':return {14,17,16,16,16,17,14}; case 'D':return {30,17,17,17,17,17,30};
        case 'E':return {31,16,16,30,16,16,31}; case 'F':return {31,16,16,30,16,16,16}; case 'G':return {14,17,16,23,17,17,15}; case 'H':return {17,17,17,31,17,17,17};
        case 'I':return {14,4,4,4,4,4,14}; case 'J':return {7,2,2,2,2,18,12}; case 'K':return {17,18,20,24,20,18,17}; case 'L':return {16,16,16,16,16,16,31};
        case 'M':return {17,27,21,21,17,17,17}; case 'N':return {17,17,25,21,19,17,17}; case 'O':return {14,17,17,17,17,17,14}; case 'P':return {30,17,17,30,16,16,16};
        case 'Q':return {14,17,17,17,21,18,13}; case 'R':return {30,17,17,30,20,18,17}; case 'S':return {15,16,16,14,1,1,30}; case 'T':return {31,4,4,4,4,4,4};
        case 'U':return {17,17,17,17,17,17,14}; case 'V':return {17,17,17,17,17,10,4}; case 'W':return {17,17,17,21,21,27,17}; case 'X':return {17,17,10,4,10,17,17};
        case 'Y':return {17,17,10,4,4,4,4}; case 'Z':return {31,1,2,4,8,16,31};
        case '0':return {14,17,19,21,25,17,14}; case '1':return {4,12,4,4,4,4,14}; case '2':return {14,17,1,2,4,8,31}; case '3':return {30,1,1,14,1,1,30};
        case '4':return {2,6,10,18,31,2,2}; case '5':return {31,16,30,1,1,17,14}; case '6':return {6,8,16,30,17,17,14}; case '7':return {31,1,2,4,8,8,8};
        case '8':return {14,17,17,14,17,17,14}; case '9':return {14,17,17,15,1,2,12};
        case '.':return {0,0,0,0,0,12,12}; case ',':return {0,0,0,0,12,4,8}; case ';':return {0,12,12,0,12,4,8}; case ':':return {0,12,12,0,12,12,0};
        case '!':return {4,4,4,4,4,0,4}; case '?':return {14,17,1,2,4,0,4}; case '-':return {0,0,0,31,0,0,0}; case '_':return {0,0,0,0,0,0,31};
        case '(':return {2,4,8,8,8,4,2}; case ')':return {8,4,2,2,2,4,8}; case '/':return {1,1,2,4,8,16,16}; case '\\':return {16,16,8,4,2,1,1};
        case '=':return {0,0,31,0,31,0,0}; case '+':return {0,4,4,31,4,4,0}; case '*':return {0,21,14,31,14,21,0}; case '\'':return {4,4,8,0,0,0,0};
        case '"':return {10,10,0,0,0,0,0}; case '&':return {12,18,20,8,21,18,13}; case '#':return {10,10,31,10,31,10,10}; case '@':return {14,17,23,21,23,16,14};
        case '%':return {24,25,2,4,8,19,3}; case '$':return {4,15,20,14,5,30,4}; case '<':return {2,4,8,16,8,4,2}; case '>':return {8,4,2,1,2,4,8};
        case '[':return {14,8,8,8,8,8,14}; case ']':return {14,2,2,2,2,2,14}; case '^':return {4,10,17,0,0,0,0}; case '|':return {4,4,4,4,4,4,4}; case '~':return {0,0,8,21,2,0,0};
        default:return {0,0,0,0,0,0,0};
        }
    }
    // Text centred on (cx,cy) in the panel plane, 'pixel' metres per font pixel; horizontal runs are merged into one rectangle each.
    static void kbText(std::vector<HudVertex>& out,const Pose& pose,float cx,float cy,const char* text,float pixel,float z,DWORD color) {
        const size_t n=strlen(text);if(!n) return;
        const float advance=6.f*pixel,left=cx-(float(n)*advance-pixel)*.5f;
        for(size_t i=0;i<n;++i) {
            const auto bits=kbGlyph(text[i]);
            for(int y=0;y<7;++y) {
                int x=0;
                while(x<5) {
                    if(!(bits[y]&(1u<<(4-x)))) {++x;continue;}
                    int e=x;while(e<5&&(bits[y]&(1u<<(4-e)))) ++e;
                    const float px=left+float(i)*advance+float(x)*pixel,py=cy+(3.f-float(y))*pixel;
                    rectQuad(out,pose,px,py,left+float(i)*advance+float(e)*pixel,py+pixel,z,color);
                    x=e;
                }
            }
        }
    }
    KbLabels kbLabels;
    // 0.9.47.34: VR keyboard panel (Keyboard.hpp). Floats in front of the player at Request::keyboardPose, over the game's own interface, hit by the host laser.
    void drawKeyboard(IDirect3DDevice9Ex* d,const Request& r,const D3DMATRIX& eyeWorld,bool menu=false) {
        if(!(r.keyboardFlags&KeyboardOpen)) return;
        {const HWND fg=GetForegroundWindow();const HKL cur=GetKeyboardLayout(fg?GetWindowThreadProcessId(fg,nullptr):0);if(cur!=kbLabels.hkl) {kbLabels.build(cur);log("Clavier VR : disposition clavier Windows %08X, etiquettes construites",unsigned(uintptr_t(cur)&0xffffffffu));}}
        const Pose pose=r.keyboardPose;
        if(!std::isfinite(pose.position.x+pose.position.y+pose.position.z+pose.orientation.x+pose.orientation.w)) return;
        std::vector<HudVertex> v;v.reserve(9000);
        const bool armed=(r.keyboardFlags&KeyboardShift)!=0;
        wallFontBuild(d);
        std::vector<HudVertex> sv,tv;sv.reserve(1500);tv.reserve(400);
        // 0.9.47.42: same look as the options block: wall stone, bronze keys with a bevel, serif lettering (no pixel font)
        const DWORD frameC=0xff2a1f16,bronze=0xff6e5028,keyC=0xff4a3a2a,keyLit=0xff8a7358,keyDark=0xff22190f,hoverC=0xffd9a03a,pressC=0xff3f8c52,armC=0xff2b5a35,closeC=0xff7a2a22;
        const float W=kbPanelW(),H=kbPanelH();
        rectQuad(v,pose,-W*.5f-.010f,-H*.5f-.010f,W*.5f+.010f,H*.5f+.010f,0.f,frameC);
        rectQuad(v,pose,-W*.5f-.006f,-H*.5f-.006f,W*.5f+.006f,H*.5f+.006f,.0004f,bronze);
        const size_t vFrame=v.size();   // 0.9.47.57: frame + bronze plate end here (painter order in the main-menu room)
        if(wallStoneTex) stoneRect(sv,pose,-W*.5f,-H*.5f,W*.5f,H*.5f,.0012f,0xff6f665c,3.2f);   // darkened wall stone as the base plate
        else rectQuad(v,pose,-W*.5f,-H*.5f,W*.5f,H*.5f,.0012f,0xff2c241c);
        KbKey keys[KbMaxKeys];const int n=kbLayout(keys);
        for(int i=0;i<n;++i) {
            const KbKey& k=keys[i];float x0,y0,x1,y1;kbKeyRect(k,x0,y0,x1,y1);
            const bool hover=r.keyboardHover==uint32_t(i)+1,pressed=r.keyboardPressed==uint32_t(i)+1;
            DWORD bg=k.kind==KbClose?closeC:(k.kind==KbShift&&armed?armC:keyC);
            if(hover) bg=hoverC;
            if(pressed) bg=pressC;
            const float e=.0030f;
            rectQuad(v,pose,x0-.0012f,y0-.0012f,x1+.0012f,y1+.0012f,.0020f,frameC);   // dark joint around the key
            rectQuad(v,pose,x0,y0,x1,y1,.0024f,bg);
            const DWORD hi=pressed?keyDark:keyLit,lo=pressed?keyLit:keyDark;          // pressed key = bevel reversed
            // 0.9.47.42: the four edge strips no longer overlap with different colours (the corners flickered): left + top = lit, right + bottom = dark;
            // bottom-left corner belongs to the lit left strip, top-right corner to the dark right strip.
            rectQuad(v,pose,x0,y0,x0+e,y1,.0028f,hi);rectQuad(v,pose,x0+e,y1-e,x1-e,y1,.0028f,hi);
            rectQuad(v,pose,x1-e,y0,x1,y1,.0028f,lo);rectQuad(v,pose,x0+e,y0,x1-e,y0+e,.0028f,lo);
            const float cx=(x0+x1)*.5f,cy=(y0+y1)*.5f,kw=x1-x0,kh=y1-y0;
            std::string label;
            switch(k.kind) {
            case KbNormal: {char c1=kbLabels.label[armed?1:0][i];if(!c1) c1='.';label=std::string(1,c1);break;}   // a key without a printable character (dead key) shows a dot
            case KbShift: label="MAJ";break;
            case KbBackspace: label="EFFACER";break;
            case KbEnter: label="ENTREE";break;
            case KbEscape: label="ESC";break;
            case KbSpace: label="ESPACE";break;
            case KbClose: label="FERMER";break;
            }
            auto it=wallGlyphs[hover?3:1].find(label);
            if(wallFontTex&&it!=wallGlyphs[hover?3:1].end()) textQuad(tv,pose,cx,cy,.0036f,it->second,kh*1.05f,kw*.86f);
            else {float px=std::min(.0050f,kw/(6.f*float(label.size())+.5f));kbText(v,pose,cx,cy,label.c_str(),px,.0036f,hover?0xff10171c:0xfff5e7ce);}
        }
        d->SetTexture(0,nullptr);d->SetPixelShader(nullptr);d->SetVertexShader(nullptr);
        d->SetTransform(D3DTS_WORLD,&eyeWorld);
        d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
        d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
        d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
        d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);
        d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
        d->SetRenderState(D3DRS_COLORWRITEENABLE,15);d->SetRenderState(D3DRS_LIGHTING,FALSE);
        d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
        d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
        d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
        auto flatStates=[&]{d->SetTexture(0,nullptr);d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
            d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
            d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);};
        auto drawRange=[&](size_t from,size_t to){if(to>from&&FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT((to-from)/3),v.data()+from,sizeof(HudVertex)))) ++sDrawFailures;};
        if(menu) {
            // 0.9.47.57 (report 235946: flickering hatch lines over the keys in the main-menu room): the isolated menu depth buffer has too little precision for the
            // stacked plates (0.4-3.6 mm apart), they fought each other. In that room the keyboard is drawn in strict painter order, no depth test, no depth write.
            d->SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
            static int kl=0;if(kl++<3) log("Clavier VR (menu 3D) : dessin dans l'ordre, sans test de profondeur");
            drawRange(0,vFrame);
            texturedPass(d,sv,wallStoneTex.Get(),false,false);
            flatStates();drawRange(vFrame,v.size());
            texturedPass(d,tv,wallFontTex.Get(),true,false);
            d->SetTexture(0,nullptr);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
        } else {
            drawRange(0,v.size());
            texturedPass(d,sv,wallStoneTex.Get(),false,true);
            texturedPass(d,tv,wallFontTex.Get(),true,false);
        }
        d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
        ++keyboardDraws;
    }
    // 0.9.47.39: skill choice of a possessed creature, head-locked at the bottom centre of the view: numbered badges 1..N, the chosen one lit
    // (A = next, B = previous, right trigger = attack). Drawn with the same flat coloured quads and pixel font as the keyboard.
    uint64_t skillDraws=0,skillBarDraws=0;
    void drawPossessionSkills(IDirect3DDevice9Ex* d,const Package& p,const Request& r,const D3DMATRIX& eyeWorld) {
        const int count=PossessionInput::shownCount.load(),sel=PossessionInput::shownSel.load();
        if(count<=0||sel<=0||!r.valid) return;
        const Pose& e0=r.eyes[0].pose;const Pose& e1=r.eyes[1].pose;
        const Vec3 head=scale(add(e0.position,e1.position),.5f);
        if(!std::isfinite(head.x+head.y+head.z+e0.orientation.w)) return;
        const Quat yaw=yawOnly(e0.orientation);
        Pose pose{};pose.orientation=normalized(multiply(yaw,pitch(-.32f)));
        pose.position=add(head,add(rotate(yaw,{0,0,-.70f}),Vec3{0,-.27f-iniFloat(L"PossessionSkillDrop",.075f,0.f,.4f),0}));   // 0.9.47.43: one bar lower
        // 0.9.47.41: ONLY the skills the creature has (the icons the game itself draws in its top bar), no frame, no number; the chosen one gets a gold contour.
        if(p.skillBar&&p.skillIconCount>0) if(IDirect3DTexture9* bt=texture(p.skillBar)) {
            static const float isz=iniFloat(L"PossessionSkillIcon",.062f,.03f,.14f);
            const int n=p.skillIconCount,act=std::clamp(sel,1,n)-1;const float igap=isz*.20f,tot=n*isz+(n-1)*igap;
            auto setFlat=[&]{
                d->SetTexture(0,nullptr);d->SetPixelShader(nullptr);d->SetVertexShader(nullptr);d->SetTransform(D3DTS_WORLD,&eyeWorld);
                d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
                d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
                d->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
                d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);
                d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
                d->SetRenderState(D3DRS_COLORWRITEENABLE,15);d->SetRenderState(D3DRS_LIGHTING,FALSE);
                d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);};
            std::vector<HudVertex> bg;bg.reserve(64);
            for(int i=0;i<n;++i) {const float cx=-tot*.5f+isz*.5f+i*(isz+igap);rectQuad(bg,pose,cx-isz*.5f-.003f,-isz*.5f-.003f,cx+isz*.5f+.003f,isz*.5f+.003f,0.f,0xff10171c);}
            setFlat();
            if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(bg.size()/3),bg.data(),sizeof(HudVertex)))) ++sDrawFailures;
            const float vA=.05f,vB=.875f;   // icon rows of the cut-out (game y .004 .. .070 of a bar cut at .08)
            for(int i=0;i<n;++i) {
                const float cx=-tot*.5f+isz*.5f+i*(isz+igap);
                std::vector<HudVertex> iv;iv.reserve(6);
                Pose ip=pose;ip.position=toWorld(pose,{cx,0.f,.0012f});
                quad(iv,ip,isz,isz,p.skillIconX0[i],vA,p.skillIconX1[i],vB);
                d->SetTexture(0,bt);
                d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
                d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
                d->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);d->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
                if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(iv.size()/3),iv.data(),sizeof(HudVertex)))) ++sDrawFailures;
            }
            setFlat();
            {   // contour of the chosen skill only
                const float cx=-tot*.5f+isz*.5f+act*(isz+igap),h=isz*.5f+.004f,t=.0055f;std::vector<HudVertex> ov;
                rectQuad(ov,pose,cx-h-t,h,cx+h+t,h+t,.0030f,0xffffbc38);rectQuad(ov,pose,cx-h-t,-h-t,cx+h+t,-h,.0030f,0xffffbc38);
                rectQuad(ov,pose,cx-h-t,-h,cx-h,h,.0030f,0xffffbc38);rectQuad(ov,pose,cx+h,-h,cx+h+t,h,.0030f,0xffffbc38);
                if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(ov.size()/3),ov.data(),sizeof(HudVertex)))) ++sDrawFailures;
            }
            d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
            ++skillDraws;++skillBarDraws;return;
        }
        static const float side=iniFloat(L"PossessionSkillBadge",.060f,.03f,.12f);
        const float gap=side*.22f,total=count*side+(count-1)*gap;
        std::vector<HudVertex> v;v.reserve(3000);
        rectQuad(v,pose,-total*.5f-.010f,-side*.5f-.026f,total*.5f+.010f,side*.5f+.012f,0.f,0xff10171c);
        char label[32];snprintf(label,sizeof(label),"ATTAQUE %d",sel);
        kbText(v,pose,0.f,side*.5f+.004f-.002f,"",.003f,.002f,0xfff5e7ce);
        for(int i=0;i<count;++i) {
            const bool on=(i+1==sel);
            const float cx=-total*.5f+side*.5f+i*(side+gap),h=on?side*.5f:side*.42f;
            rectQuad(v,pose,cx-h-.003f,-h-.003f,cx+h+.003f,h+.003f,.0012f,on?0xffffbc38:0xff504538);
            rectQuad(v,pose,cx-h,-h,cx+h,h,.0024f,on?0xff3a2a10:0xff2d3a44);
            char one[2]={char('1'+i),0};
            kbText(v,pose,cx,0.f,one,on?.0068f:.0050f,.0036f,on?0xffffe9a8:0xffb9c3cc);
        }
        kbText(v,pose,0.f,-side*.5f-.012f,label,.0030f,.0036f,0xfff5e7ce);
        if(p.skillBar&&texture(p.skillBar)) v.clear();   // 0.9.47.41: the game's own bar is shown, no numbered badges (their number strip was refused by the user)
        d->SetTexture(0,nullptr);d->SetPixelShader(nullptr);d->SetVertexShader(nullptr);
        d->SetTransform(D3DTS_WORLD,&eyeWorld);
        d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
        d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
        d->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
        d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);
        d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
        d->SetRenderState(D3DRS_COLORWRITEENABLE,15);d->SetRenderState(D3DRS_LIGHTING,FALSE);
        d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
        d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
        d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
        if(!v.empty()&&FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(v.size()/3),v.data(),sizeof(HudVertex)))) ++sDrawFailures;
        d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
        // 0.9.47.40: the game's own skill bar (top of the possession screen) copied above the badges, so the icons and the highlighted skill are the game's.
        if(IDirect3DTexture9* bt=p.skillBar?texture(p.skillBar):nullptr) {
            static const float bw=iniFloat(L"PossessionSkillBarWidth",.50f,.15f,1.2f);
            Pose bp=pose;bp.position=toWorld(pose,{0.f,side*.5f+.012f+bw*p.skillAspect*.5f,0.f});
            std::vector<HudVertex> bv;bv.reserve(6);
            rectQuad(bv,bp,-bw*.5f-.006f,-bw*p.skillAspect*.5f-.006f,bw*.5f+.006f,bw*p.skillAspect*.5f+.006f,0.f,0xff10171c);
            d->SetTexture(0,nullptr);
            if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(bv.size()/3),bv.data(),sizeof(HudVertex)))) ++sDrawFailures;
            bv.clear();quad(bv,bp,bw,bw*p.skillAspect,0.f,0.f,1.f,1.f);
            d->SetTexture(0,bt);
            d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
            d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
            d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
            d->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);d->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
            if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(bv.size()/3),bv.data(),sizeof(HudVertex)))) ++sDrawFailures;
            d->SetTexture(0,nullptr);
            d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
            ++skillBarDraws;
        }
        ++skillDraws;
    }
    uint64_t keyboardDraws=0;
    void drawPossessionVignette(IDirect3DDevice9Ex* d,const Request& r,UINT width,UINT height,bool possessed) {
        const uint32_t level=(r.comfortFlags&ComfortVignetteMask)>>ComfortVignetteShift;
        if(!possessed||level==0) return;
        struct ScreenVertex {float x,y,z,rhw;DWORD color;};
        std::vector<ScreenVertex> ring;ring.reserve(48*6);
        const float cx=width*.5f,cy=height*.5f;
        const float inner=std::min(width,height)*(level==1?.38f:level==2?.30f:.22f);
        const float outer=std::sqrt(cx*cx+cy*cy)+2.f;
        const DWORD edge=level==1?0x99000000:level==2?0xbb000000:0xdd000000;
        auto point=[&](float angle,float radius,DWORD color){return ScreenVertex{cx+std::cos(angle)*radius,cy+std::sin(angle)*radius,0.f,1.f,color};};
        for(int k=0;k<48;++k) {
            const float a=2*Pi*k/48.f,b=2*Pi*(k+1)/48.f;
            const auto p=point(a,inner,0),q=point(a,outer,edge),s=point(b,inner,0),t=point(b,outer,edge);
            ring.insert(ring.end(),{p,q,t,p,t,s});
        }
        d->SetVertexShader(nullptr);d->SetPixelShader(nullptr);d->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
        d->SetTexture(0,nullptr);d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
        d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
        d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
        d->SetRenderState(D3DRS_SRGBWRITEENABLE,FALSE);d->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
        d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
        d->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
        d->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
        d->SetRenderState(D3DRS_COLORWRITEENABLE,15);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);
        if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(ring.size()/3),ring.data(),sizeof(ScreenVertex)))) ++sDrawFailures;
    }
    void drawTestWalls(IDirect3DDevice9Ex* d,const Package& p,const Request& r) {
        const bool floorKnown=r.handFlags&RequestFloorKnown;
        const float floorY=floorKnown?r.floorY:levelTable(r).position.y-.78f;
        const Pose room{r.anchor.orientation,{r.anchor.position.x,floorY,r.anchor.position.z}};
        const float W=1.5f,H=1.125f,Y=1.45f;
        struct Spot {float x,z,yaw;};
        const Spot spots[4]={{-2.25f,-1.25f,Pi*.5f},{-2.25f,.75f,Pi*.5f},{2.25f,-1.25f,-Pi*.5f},{2.25f,.75f,-Pi*.5f}};
        Pose pose[4];
        for(int i=0;i<4;++i) pose[i]={normalized(multiply(room.orientation,yawQuat(spots[i].yaw))),toWorld(room,{spots[i].x,Y,spots[i].z})};
        // Untextured: frames, grey backings (specks show on them), digits.
        std::vector<HudVertex> plain;plain.reserve(600);
        for(int i=0;i<4;++i) {
            rectQuad(plain,pose[i],-W*.5f-.04f,-H*.5f-.04f,W*.5f+.04f,H*.5f+.04f,-.010f,0xff141414);
            rectQuad(plain,pose[i],-W*.5f,-H*.5f,W*.5f,H*.5f,-.005f,i==0?0xff000000:0xff5a6068);
            digit(plain,pose[i],0,H*.5f+.26f,i+1,0xffffd21e);
        }
        d->SetTexture(0,nullptr);d->SetPixelShader(nullptr);
        d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
        d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
        d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
        if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(plain.size()/3),plain.data(),sizeof(HudVertex)))) ++sDrawFailures;
        d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
        d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
        auto panel=[&](int i,IDirect3DTexture9* t,bool opaque,IDirect3DPixelShader9* ps) {
            if(!t) return;
            std::vector<HudVertex> q;quad(q,pose[i],W,H,0,0,1,1);
            d->SetRenderState(D3DRS_ALPHABLENDENABLE,opaque?FALSE:TRUE);
            d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
            d->SetTexture(0,t);d->SetPixelShader(ps);
            if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,2,q.data(),sizeof(HudVertex)))) ++sDrawFailures;
            d->SetPixelShader(nullptr);
        };
        IDirect3DTexture9* frame=p.frame?texture(p.frame):nullptr;
        panel(0,frame,true,nullptr);
        panel(1,p.hud?texture(p.hud):nullptr,false,nullptr);
        panel(2,p.held?texture(p.held):nullptr,false,nullptr);
        if(barShader&&frame) {
            static const float none[8][4]={{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1}};
            d->SetPixelShaderConstantF(0,&none[0][0],8);panel(3,frame,false,barShader.Get());
        }
        d->SetTexture(0,nullptr);
    }
    void drawHeld3D(IDirect3DDevice9Ex* d,const Package& p,const Request& r,int eye) {
        bool any=false;for(const auto& rec:p.records) any|=rec.held3d;if(!any)return;
        ComPtr<IDirect3DSurface9> rt,oldDepth;D3DSURFACE_DESC desc{};
        if(FAILED(d->GetRenderTarget(0,&rt))||!rt||FAILED(rt->GetDesc(&desc)))return;
        d->GetDepthStencilSurface(&oldDepth);
        if(!heldDepth||heldDepthW!=desc.Width||heldDepthH!=desc.Height||heldDepthSamples!=desc.MultiSampleType) {
            heldDepth.Reset();
            if(FAILED(d->CreateDepthStencilSurface(desc.Width,desc.Height,D3DFMT_D24S8,desc.MultiSampleType,desc.MultiSampleQuality,FALSE,&heldDepth,nullptr))) {++sDrawFailures;return;}
            heldDepthW=desc.Width;heldDepthH=desc.Height;heldDepthSamples=desc.MultiSampleType;
        }
        ComPtr<IDirect3DStateBlock9> restore;if(FAILED(d->CreateStateBlock(D3DSBT_ALL,&restore)))return;
        if(FAILED(d->SetDepthStencilSurface(heldDepth.Get())))return;
        d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);d->Clear(0,nullptr,D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,0,1,0);
        const Vec3 hot=add(r.rayOrigin,scale(r.rayDirection,.03f));
        // 0.9.47.22: genuine model geometry (screen-pixel units recovered from the native XY/Z scales) placed in EXACTLY the frame of the flat held window:
        // origin = hotspot, right/up = the window's own axes (facing the head, vertical kept), same pixel scale (mpp * .35). x and y are the game's own screen
        // coordinates (so the orientation and the swing are the ones of the flat image); only the depth (toward the head) is added. Both eyes use the same frame:
        // binocular parallax comes from the depth.
        const Vec3 head3=scale(add(r.eyes[0].pose.position,r.eyes[1].pose.position),.5f);
        Vec3 right3{},up3{},to3{};if(!heldWindowAxes(hot,head3,right3,up3,to3)) return;
        Similarity model{quatFromRows({right3.x,up3.x,to3.x},{right3.y,up3.y,to3.y},{right3.z,up3.z,to3.z}),p.hudSizes.cursor/64.f*.35f,hot};
        model=then(model,Similarity{conjugate(r.eyes[eye].pose.orientation),1,rotate(conjugate(r.eyes[eye].pose.orientation),scale(r.eyes[eye].pose.position,-1))});
        D3DMATRIX mw{};matrixOf(model,&mw._11);
         // 0.8.85: primary hand LEFT: the held creature model is mirrored (culling is off)
        d->SetTransform(D3DTS_WORLD,&mw);
        d->SetVertexShader(nullptr);
        for(const auto& rec:p.records) if(rec.held3d) {
            const auto& s=p.snapshots[rec.snapshot];
            for(int t=0;t<Snapshot::Samplers;++t){d->SetTexture(t,texture(s.copy[t]));for(size_t i=0;i<Snapshot::SS;++i)d->SetSamplerState(t,Snapshot::samplerStates[i],s.ss[t][i]);}
            for(size_t i=0;i<Snapshot::RS;++i)d->SetRenderState(Snapshot::renderStates[i],s.rs[i]);
            for(int t=0;t<Snapshot::Stages;++t)for(size_t i=0;i<Snapshot::TS;++i)d->SetTextureStageState(t,Snapshot::stageStates[i],s.ts[t][i]);
            IDirect3DPixelShader9* ps=nullptr;auto it=shaders.find(s.shader);if(it!=shaders.end())ps=it->second.Get();d->SetPixelShader(ps);
            d->SetFVF(rec.fvf);d->SetRenderState(D3DRS_COLORWRITEENABLE,15);d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
            d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);d->SetRenderState(D3DRS_FOGENABLE,FALSE);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);
            if(FAILED(d->DrawPrimitiveUP(rec.type,rec.primitives,p.vertices.data()+rec.vertices,rec.stride)))++sDrawFailures;
        }
        d->SetDepthStencilSurface(oldDepth.Get());restore->Apply();
    }
    // 0.8.89.2: the menu screen is a real object of the room (opaque, depth tested, same eye images as the walls): no compositor layer, so it cannot
    // swim against the wall when the head moves. 4:3 area of the game's back buffer copy.
    // 0.9.47.39: START-UP PICTURE ON THE WALL SCREEN. While the game boots, its screen is black (long black wall before the intro video / menu): the picture
    // Textures\demarrage.jpg takes the place of that black on the wall screen of the room (no floating image). It is shown until the game's screen has
    // some light in it (video, loading image, menu): measured on an 8x8 reduction of the game's screen every 10 headset frames. If that measure is
    // impossible on this machine, the picture stays StartupPictureFallbackSeconds (8 s) from the first image of the room.
    ComPtr<IDirect3DTexture9> splashTex;ComPtr<IDirect3DSurface9> splashRt,splashSys;bool splashTried=false,splashDone=false,splashProbeFailed=false;
    double splashFirstMs=0;uint32_t splashFrames=0;uint64_t splashShownFrames=0;
    void splashLoad(IDirect3DDevice9Ex* d) {
        if(splashTried) return;splashTried=true;
        if(iniInt(L"StartupPictureOnWall",1)==0) {splashDone=true;log("Start-up picture on the wall: disabled (StartupPictureOnWall=0)");return;}
        const std::wstring file=(configurationPath().parent_path()/L"Textures"/L"demarrage.jpg").wstring();
        int w=0,h=0;std::vector<uint32_t> px;
        if(!env::Scene::loadImageFile(file,2048,w,h,px)) {splashDone=true;log("Start-up picture on the wall: %ls missing or unreadable (black wall as before)",file.c_str());return;}
        ComPtr<IDirect3DTexture9> sys;
        if(FAILED(d->CreateTexture(UINT(w),UINT(h),1,0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&sys,nullptr))) {splashDone=true;log("Start-up picture on the wall: texture creation failed (system memory)");return;}
        D3DLOCKED_RECT lr{};
        if(FAILED(sys->LockRect(0,&lr,nullptr,0))) {splashDone=true;log("Start-up picture on the wall: lock failed");return;}
        for(int y=0;y<h;++y) {uint32_t* row=reinterpret_cast<uint32_t*>(static_cast<unsigned char*>(lr.pBits)+size_t(y)*lr.Pitch);for(int x=0;x<w;++x) row[x]=px[size_t(y)*w+x]|0xff000000u;}
        sys->UnlockRect(0);
        if(FAILED(d->CreateTexture(UINT(w),UINT(h),1,0,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&splashTex,nullptr))||FAILED(d->UpdateTexture(sys.Get(),splashTex.Get()))) {splashTex.Reset();splashDone=true;log("Start-up picture on the wall: texture creation failed (video memory)");return;}
        log("Start-up picture on the wall: %ls %dx%d loaded",file.c_str(),w,h);
    }
    bool splashWanted(IDirect3DDevice9Ex* d,const Package& p,IDirect3DTexture9* desk) {
        if(splashDone) return false;
        splashLoad(d);if(!splashTex) return false;
        const double now=milliseconds();
        if(splashFirstMs==0) {splashFirstMs=now;log("Start-up picture on the wall: shown on the wall screen of the room");}
        ++splashFrames;
        static const double maxMs=1000.*double(iniFloat(L"StartupPictureMaxSeconds",90.f,3.f,600.f));
        static const double fallbackMs=1000.*double(iniFloat(L"StartupPictureFallbackSeconds",8.f,1.f,60.f));
        auto finish=[&](const char* why) {splashDone=true;log("Start-up picture on the wall: removed (%s) after %.1f s, %llu headset frames",why,(now-splashFirstMs)/1000.,(unsigned long long)splashShownFrames);splashTex.Reset();splashRt.Reset();splashSys.Reset();};
        if(now-splashFirstMs>maxMs) {finish("time limit");return false;}
        if(splashProbeFailed) {if(now-splashFirstMs>fallbackMs) {finish("fallback delay");return false;}}
        else if(desk&&p.desktopWidth>=64&&p.desktopHeight>=64&&(splashFrames%10)==1) {
            bool ok=true;
            if(!splashRt) ok=SUCCEEDED(d->CreateRenderTarget(8,8,D3DFMT_A8R8G8B8,D3DMULTISAMPLE_NONE,0,FALSE,&splashRt,nullptr))&&SUCCEEDED(d->CreateOffscreenPlainSurface(8,8,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&splashSys,nullptr));
            ComPtr<IDirect3DSurface9> s;
            if(ok) ok=SUCCEEDED(desk->GetSurfaceLevel(0,&s));
            const float W=float(p.desktopWidth),H=float(p.desktopHeight);
            float cw=W,cx=0;if(W>H*1.34f) {cw=std::floor(H*4.f/3.f+.5f);cx=std::floor((W-cw)*.5f);}
            RECT src{LONG(cx),0,LONG(cx+cw),LONG(H)};
            if(ok) ok=SUCCEEDED(d->StretchRect(s.Get(),&src,splashRt.Get(),nullptr,D3DTEXF_LINEAR))&&SUCCEEDED(d->GetRenderTargetData(splashRt.Get(),splashSys.Get()));
            D3DLOCKED_RECT lr{};
            if(ok) ok=SUCCEEDED(splashSys->LockRect(&lr,nullptr,D3DLOCK_READONLY));
            if(!ok) {splashProbeFailed=true;log("Start-up picture on the wall: the game's screen cannot be measured here, picture kept %.0f s instead",double(fallbackMs)/1000.);}
            else {
                double sum=0;for(int y=0;y<8;++y) {const unsigned char* row=static_cast<const unsigned char*>(lr.pBits)+size_t(y)*lr.Pitch;for(int x=0;x<8;++x) sum+=std::max({row[x*4],row[x*4+1],row[x*4+2]});}
                splashSys->UnlockRect();
                const double mean=sum/64.;
                if(splashFrames<=1||(splashFrames%90)==1) log("Start-up picture on the wall: brightness of the game's screen %.1f / 255",mean);
                if(mean>10.) {finish("the game's screen shows its own picture");return false;}
            }
        }
        ++splashShownFrames;
        return true;
    }
    void drawMenuScreen(IDirect3DDevice9Ex* d,const Package& p,const Request& r,const D3DMATRIX& eyeWorld) {
        IDirect3DTexture9* t=texture(p.desktop);const bool desk=t&&p.desktopWidth>=64&&p.desktopHeight>=64;
        const bool splash=splashWanted(d,p,desk?t:nullptr);
        if(!desk&&!splash) return;
        float u0=0.f,u1=1.f,sw=MenuScreenWidth,sh=MenuScreenWidth*.75f;
        if(desk) {
            const float W=float(p.desktopWidth),H=float(p.desktopHeight);
            float cw=W,cx=0;if(W>H*1.34f) {cw=std::floor(H*4.f/3.f+.5f);cx=std::floor((W-cw)*.5f);}
            sh=MenuScreenWidth*H/cw;
            if(!splash) {u0=cx/W;u1=(cx+cw)/W;}
        }
        if(splash) t=splashTex.Get();
        Pose pose=menuScreenPose(r);pose.position=toWorld(pose,{0.f,0.f,MenuScreenPlaneOffset});
        std::vector<HudVertex> v;v.reserve(6);
        quad(v,pose,sw,sh,u0,0.f,u1,1.f);
        d->SetTransform(D3DTS_WORLD,&eyeWorld);d->SetPixelShader(nullptr);d->SetVertexShader(nullptr);
        d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
        for(int i=1;i<Snapshot::Samplers;++i) d->SetTexture(i,nullptr);
        d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
        d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
        d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);
        d->SetRenderState(D3DRS_COLORWRITEENABLE,15);d->SetRenderState(D3DRS_FOGENABLE,FALSE);d->SetRenderState(D3DRS_SRGBWRITEENABLE,FALSE);
        d->SetRenderState(D3DRS_DEPTHBIAS,0);d->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,0);
        d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
        d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
        d->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);d->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
        d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);d->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
        d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
        d->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);d->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);d->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_NONE);
        d->SetTexture(0,t);
        if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(v.size()/3),v.data(),sizeof(HudVertex)))) ++sDrawFailures;
    }
    void drawMenuCursor(IDirect3DDevice9Ex* d,const Package& p,const Request& r,const D3DMATRIX& eyeWorld) {
        if(!p.menuCursor||!p.keeperHandle||p.keeperW<=0||p.keeperH<=0||!(r.pointerFlags&PointerAimValid)) return;
        if(p.keeperHandle!=keeperOpened) {keeperTexture.Reset();HANDLE hh=p.keeperHandle;
            if(SUCCEEDED(dev->CreateTexture(256,256,1,0,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&keeperTexture,&hh)))keeperOpened=p.keeperHandle;else keeperOpened=nullptr;}
        if(!keeperTexture)return;
        Pose pose=menuScreenPose(r);Vec3 o=toLocal(pose,r.rayOrigin),dir=rotate(conjugate(pose.orientation),r.rayDirection);
        if(std::fabs(dir.z)<1e-6f)return;float distance=-o.z/dir.z;if(distance<=0)return;
        Vec3 hit=add(o,scale(dir,distance));float screenH=MenuScreenWidth*std::max(p.menuCy,1.f)/std::max(p.menuCx,1.f);
        if(std::fabs(hit.x)>MenuScreenWidth*.5f||std::fabs(hit.y)>screenH*.5f)return;
        float mpp=MenuScreenWidth/std::max(2.f*p.menuCx,1.f),left=hit.x-p.keeperHotX*mpp,top=hit.y+p.keeperHotY*mpp;
        auto point=[&](float x,float y){return toWorld(pose,{x,y,.002f});};float right=left+p.keeperW*mpp,bottom=top-p.keeperH*mpp;
        float u=p.keeperW/256.f,v=p.keeperH/256.f;
        HudVertex q[6]={{point(left,top),0xffffffff,0,0},{point(right,top),0xffffffff,u,0},{point(right,bottom),0xffffffff,u,v},
                      {point(left,top),0xffffffff,0,0},{point(right,bottom),0xffffffff,u,v},{point(left,bottom),0xffffffff,0,v}};
        d->SetTransform(D3DTS_WORLD,&eyeWorld);d->SetVertexShader(nullptr);d->SetPixelShader(nullptr);d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
        d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_FOGENABLE,FALSE);d->SetRenderState(D3DRS_ZENABLE,FALSE);d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
        d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
        d->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_SRGBWRITEENABLE,FALSE);
        d->SetSamplerState(0,D3DSAMP_SRGBTEXTURE,FALSE);d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
        d->SetTexture(0,keeperTexture.Get());d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
        d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
        d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,2,q,sizeof(HudVertex));d->SetTexture(0,nullptr);
        d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ZENABLE,TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
    }
    void drawHud(IDirect3DDevice9Ex* d,const HudLayout& hud,const Package& p,const Request& r,int eye,const D3DMATRIX& eyeWorld) {
        (void)eye;
        std::vector<HudVertex> v;v.reserve(64);
        const float W=float(p.hudWidth),H=float(p.hudHeight);
        std::vector<HudVertex> sv,fv,mv,hv;
        IDirect3DTexture9* frameTex=(p.frame&&barShader&&barsFromImage)?texture(p.frame):nullptr;
        for(int i=0;i<HudCount;++i) {
            const HudPanel& q=hud.panel[i];if(!q.shown) continue;
            // 0.6.9.15: bars and minimap from the game's final image (exact composition, nothing to classify).
            if(frameTex&&i!=HudScreen) {quad(i==HudMinimap?mv:fv,q.pose,q.w,q.h,q.x0/W,q.y0/H,q.x1/W,q.y1/H);continue;} // minimap opaque (its black = unexplored land)
            // 0.8.94.1: the middle of the game screen (message boxes, objectives page, options) is split by the classification between the interface layer
            // and the screen layer in a way that changes with every element (report 210345: box in one layer, its text or its buttons in the other, with
            // matching holes). The middle panel always covers the whole band and shows BOTH layers at the same place: their union is the complete picture.
            // 0.8.94.3: drawn ONLY for the game's own full menu / page (whole band): screen layer first (it marks the stencil), then the interface layer only
            // where the screen layer is empty -> no double translucent backgrounds (report 223726 capture 5). Messages / notifications: screen layer only
            // (0.8.94.1 drew the interface layer there too: the creature in the hand appeared on the wall, report 223726 capture 3).
            if(i==HudScreen&&p.panelUi&&!p.screenWhole) {quad(hv,q.pose,q.w,q.h,q.x0/W,q.y0/H,q.x1/W,q.y1/H);continue;}   // 0.8.94.22: exactly what the game shows there
            if(i==HudScreen) {if(p.screenWhole) {quad(hv,q.pose,q.w,q.h,q.x0/W,q.y0/H,q.x1/W,q.y1/H);if(p.screen) quad(sv,q.pose,q.w,q.h,q.x0/W,q.y0/H,q.x1/W,q.y1/H);}
                else quad(sv,q.pose,q.w,q.h,q.x0/W,q.y0/H,q.x1/W,q.y1/H);continue;}
            quad(v,q.pose,q.w,q.h,q.x0/W,q.y0/H,q.x1/W,q.y1/H);
        }
        d->SetTransform(D3DTS_WORLD,&eyeWorld);d->SetPixelShader(nullptr);d->SetVertexShader(nullptr);
        d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
        for(int i=1;i<Snapshot::Samplers;++i) d->SetTexture(i,nullptr);
        d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
        d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
        d->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);d->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
        d->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);d->SetRenderState(D3DRS_ALPHAREF,4);d->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);
        d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_COLORWRITEENABLE,15);d->SetRenderState(D3DRS_FOGENABLE,FALSE);
        d->SetRenderState(D3DRS_SRGBWRITEENABLE,FALSE);
        d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
        d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
        d->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);d->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
        d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);d->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
        d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
        d->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);d->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);d->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_NONE);
        d->SetSamplerState(0,D3DSAMP_MAXANISOTROPY,1);d->SetSamplerState(0,D3DSAMP_MIPMAPLODBIAS,0);
        // Interface texture: colours are premultiplied by the coverage built in the game thread.
        if(!v.empty()) {
            d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
            d->SetTexture(0,texture(p.hud));
            if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(v.size()/3),v.data(),sizeof(HudVertex)))) ++sDrawFailures;
        }
        if(!fv.empty()) {
            // Black of the game image (no dungeon behind the bars: engine view kept between the bars) = transparent.
            d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
            {static const float cut[8][4]={{.440f,.752f,.560f,.834f},{0.f,0.f,1.f,.068f},{0.f,0.f,.40f,.092f},{.015f,.690f,.740f,.762f},
                {.175f,.757f,.435f,.830f},{0.f,.767f,.830f,.823f},{.810f,.757f,1.f,.830f},{0.f,.833f,1.f,1.f}};
             d->SetPixelShaderConstantF(0,&cut[0][0],8);}
            d->SetTexture(0,frameTex);d->SetPixelShader(barShader.Get());
            if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(fv.size()/3),fv.data(),sizeof(HudVertex)))) ++sDrawFailures;
            d->SetPixelShader(nullptr);
        }
        if(!mv.empty()) {
            if(mapShader) {
                static const float mc[4]={.0078f,.762f,.1816f,.990f};
                d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
                d->SetPixelShaderConstantF(0,mc,1);d->SetPixelShader(mapShader.Get());d->SetTexture(0,frameTex);
                if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(mv.size()/3),mv.data(),sizeof(HudVertex)))) ++sDrawFailures;
                d->SetPixelShader(nullptr);
            } else {
            d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
            d->SetRenderState(D3DRS_TEXTUREFACTOR,0xffffffff);
            d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
            d->SetTexture(0,frameTex);
            if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(mv.size()/3),mv.data(),sizeof(HudVertex)))) ++sDrawFailures;
            d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
            }
        }
        const bool stencilUnion=!hv.empty()&&!sv.empty()&&p.screen&&texture(p.screen);
        if(stencilUnion) {   // 0.8.94.3: mark the pixels of the screen layer (stencil bit 0x80, alpha test keeps only its visible pixels)
            d->SetRenderState(D3DRS_STENCILENABLE,TRUE);d->SetRenderState(D3DRS_STENCILMASK,0x80);d->SetRenderState(D3DRS_STENCILWRITEMASK,0x80);
            d->SetRenderState(D3DRS_STENCILREF,0x80);d->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_ALWAYS);
            d->SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_REPLACE);d->SetRenderState(D3DRS_STENCILFAIL,D3DSTENCILOP_KEEP);d->SetRenderState(D3DRS_STENCILZFAIL,D3DSTENCILOP_KEEP);
        }
        if(!sv.empty()&&p.screen&&texture(p.screen)) {
            d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
            d->SetTexture(0,texture(p.screen));
            if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(sv.size()/3),sv.data(),sizeof(HudVertex)))) ++sDrawFailures;
        }
        if(!hv.empty()) {   // interface layer of the whole menu panel, only where the screen layer left nothing
            if(stencilUnion) {d->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_NOTEQUAL);d->SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_KEEP);}
            d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
            d->SetTexture(0,texture(p.hud));
            if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(hv.size()/3),hv.data(),sizeof(HudVertex)))) ++sDrawFailures;
        }
        if(stencilUnion) {d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_STENCILMASK,0xffffffff);d->SetRenderState(D3DRS_STENCILWRITEMASK,0xffffffff);}
        if(testWalls) {
            drawTestWalls(d,p,r);
        }
        d->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);d->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);d->SetRenderState(D3DRS_ALPHAREF,4);
        // The Keeper's hand (the game cursor itself, with its animations) on the pointing controller.
        const int h=(r.pointerFlags&HudLeftHanded)?0:1;
        IDirect3DTexture9* ct=texture(p.cursor);
        // 0.6.4: the game's own cursor image (Keeper's hand, animations, held creatures) takes precedence.
        Package keeperView;const Package* src=&p;
        if(p.keeperHandle&&p.keeperW>0&&p.keeperH>0) {
            if(p.keeperHandle!=keeperOpened) {
                keeperTexture.Reset();HANDLE hh=p.keeperHandle;
                if(SUCCEEDED(dev->CreateTexture(256,256,1,0,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&keeperTexture,&hh))) keeperOpened=p.keeperHandle;
                else {keeperOpened=nullptr;if(sOpenFailures++<5) log("VR thread: opening the Keeper hand texture failed");}
            }
            if(keeperTexture) {
                ct=keeperTexture.Get();
                keeperView.cursorUv[0]=0;keeperView.cursorUv[1]=0;keeperView.cursorUv[2]=p.keeperW/256.f;keeperView.cursorUv[3]=p.keeperH/256.f;
                keeperView.cursorW=float(p.keeperW);keeperView.cursorH=float(p.keeperH);
                keeperView.cursorHotX=float(p.keeperHotX)/p.keeperW;keeperView.cursorHotY=float(p.keeperHotY)/p.keeperH;
                keeperView.hudSizes=p.hudSizes;src=&keeperView;
            }
        }
        const Package& cp=*src;
        if(!ct) ++sCurNoTex; else if(!(r.pointerFlags&PointerAimValid)) ++sCurNoAim; else if(!(cp.cursorW>1&&cp.cursorH>1)) ++sCurNoSize; else ++sCurDraw;
        if(ct&&(r.pointerFlags&PointerAimValid)&&cp.cursorW>1&&cp.cursorH>1) {
            // Size: CursorSize metres for 64 game pixels (larger images, e.g. with held creatures, grow).
            // 0.6.7: the hand keeps the size of the pointing hand (67 px wide) whatever the animation frame
            // (the grabbing hand image is 118x112: it looked twice too big); held things keep the pixel scale.
            const float mpp=p.hudSizes.cursor/64.f,hk=cp.cursorW>67.f?67.f/cp.cursorW:1.f,sh=cp.cursorH*mpp*hk,sw=cp.cursorW*mpp*hk;
            const Vec3 head=scale(add(r.eyes[0].pose.position,r.eyes[1].pose.position),.5f);
            const Vec3 hot=add(r.rayOrigin,scale(r.rayDirection,.03f));
            Vec3 to=sub(head,hot);float l=std::sqrt(dot(to,to));if(l<1e-4f) return;to=scale(to,1/l);
            Vec3 right=cross({0,1,0},to);float rl=std::sqrt(dot(right,right));if(rl<1e-4f) return;right=scale(right,1/rl);
            Vec3 up=cross(to,right);
            // Sprite rectangle so that the hotspot (fraction of the sprite) sits on 'hot'.
            const bool mirror=false; // 0.8.87.1: no mirroring for the left hand; 0.8.85: primary hand LEFT: the Keeper's hand sprite is mirrored about its hotspot
            Vec3 tl=sub(sub(hot,scale(right,(mirror?1.f-cp.cursorHotX:cp.cursorHotX)*sw)),scale(up,-cp.cursorHotY*sh));
            Vec3 a=tl,b=add(tl,scale(right,sw)),c=sub(b,scale(up,sh)),dd=sub(tl,scale(up,sh));
            const float* uv=cp.cursorUv;
            const float u0=mirror?uv[2]:uv[0],u1=mirror?uv[0]:uv[2];
            HudVertex q[4]={{a,0xffffffff,u0,uv[1]},{b,0xffffffff,u1,uv[1]},{c,0xffffffff,u1,uv[3]},{dd,0xffffffff,u0,uv[3]}};
            HudVertex tri[6]={q[0],q[1],q[2],q[0],q[2],q[3]};
            // Things held (drawn under the hand): window of the held-things texture centred on the game cursor,
            // same pixel scale, centred on the hotspot. Premultiplied like the interface.
            if(IDirect3DTexture9* ht=texture(p.held); ht&&p.heldRadius>1&&p.hudWidth>0) {
                // 0.6.7: window R to the left, 2.5R to the right (tooltips next to the cursor were cut).
                // 0.6.7.1: same scale reduction as the hand (grabbing hand 118 px -> 67 px), window R left / 2.5R right,
                // R up / 2.2R down (the creature hangs below the hand and was cut).
                const float R=p.heldRadius,W=float(p.hudWidth),H=float(p.hudHeight),m2=mpp*.35f,sL=1.5f*R*m2,sR=2.5f*R*m2,sU=R*m2,sD=4.2f*R*m2; // 0.6.9.13: 2.2 -> 3.2 below; 0.6.9.14: 4.2 below, 1.5 left (still cut)
                // 0.8.26: the small icons drawn to the RIGHT of the hand (counts / info) overlapped the hand sprite: the upper-right block of the window
                // (right of HeldSideSplit game px from the cursor, down to HeldSideRows px below it) is moved HeldSideShift px further right. The
                // creature (lower rows) and everything left of the split stay where they were. INI: HeldSideSplit / HeldSideRows / HeldSideShift.
                static const float cfgSplit=iniFloat(L"HeldSideSplit",24.f,-200.f,400.f),cfgRows=iniFloat(L"HeldSideRows",64.f,0.f,400.f),cfgShift=iniFloat(L"HeldSideShift",56.f,0.f,400.f);
                // 0.8.86.4: the creature stays where the game draws it (same place for both hands, in the hand). Only the block of small
                // things beside the cursor (tooltip + icon, upper right of the window) moves: to the RIGHT of the Keeper's hand for a right-handed
                // player, to the LEFT of it for a left-handed player. It is moved, never flipped (the text stays readable).
                // (heldModelRight is no longer used: it measured the whole window, so the split always saturated.)
                const float kk=R/200.f,S=std::clamp(cfgSplit*kk,-1.5f*R,2.5f*R),Hr=cfgRows*kk,Xb=std::min(340.f*kk,2.5f*R);
                const float handSpan=(1.f-cp.cursorHotX)*sw/m2+25.f*kk;          // window pixels covered by the hand sprite on the side of the block
                const float blockShift=handSpan-S;
                auto pos=[&](float px,float py,float shiftPx){return add(hot,add(scale(right,(px+shiftPx)*m2),scale(up,-py*m2)));}; // px,py: window pixels relative to the cursor
                {   // 0.8.86.3 diagnostic (held creature, both hands): layout of the held window, at most one line every 2 s
                    static double lastHeldLog=0;const double now=milliseconds();
                    if(now-lastHeldLog>2000) {lastHeldLog=now;log("Held window: hand=%s mirror=%d R=%.1f split=%.1f handSpan=%.1f blockShift=%.1f rows=%.1f cursorHot=%.2f,%.2f sprite=%.0fx%.0f heldPx=%.0f,%.0f",
                        (r.pointerFlags&HudLeftHanded)?"LEFT":"RIGHT",int(mirror),R,S,handSpan,blockShift,Hr,cp.cursorHotX,cp.cursorHotY,cp.cursorW,cp.cursorH,p.heldPx,p.heldPy);}
                }
                HudVertex hq[72];int nq=0;   // 0.8.94.31: up to 9 parts (was 36 vertices / 6 parts)
                auto part=[&](float x0,float x1,float y0,float y1,float shiftPx){
                    if(x1<=x0||y1<=y0) return;
                    auto uu=[&](float x){return (p.heldPx+x)/W;};auto vv=[&](float y){return (p.heldPy+y)/H;};
                    HudVertex A{pos(x0,y0,shiftPx),0xffffffff,uu(x0),vv(y0)},B{pos(x1,y0,shiftPx),0xffffffff,uu(x1),vv(y0)},C{pos(x1,y1,shiftPx),0xffffffff,uu(x1),vv(y1)},D{pos(x0,y1,shiftPx),0xffffffff,uu(x0),vv(y1)};
                    hq[nq++]=A;hq[nq++]=B;hq[nq++]=C;hq[nq++]=A;hq[nq++]=C;hq[nq++]=D;};
                const float xl=-1.5f*R,xr=2.5f*R,yt=-R,yb=4.2f*R;
                // 0.8.94.3: only the hover tooltip (its rectangle is measured by the game thread: name + icon row) moves clear of the hand. 0.8.26-0.8.94.2 moved a
                // fixed upper-right block of the window: the part of the creature inside it was cut off and drawn further right (report 223726 capture 3).
                const float tx0=std::clamp(p.tipRect[0],xl,Xb),ty0=std::clamp(p.tipRect[1],yt,yb),tx1=std::clamp(p.tipRect[2],xl,Xb),ty1=std::clamp(p.tipRect[3],yt,yb);
                if(p.tipValid&&tx1>tx0+2&&ty1>ty0+2) {
                    // 0.8.94.31: the rectangle now holds the tooltip AND its small icons (game thread: flat 2D pieces right of the cursor). It is moved so that
                    // its left edge clears the hand sprite with a small margin (user: 'un peu plus sur la droite'). The held creature's box (3D pieces at the
                    // cursor, measured by the game thread, + 12 px) is NEVER moved: the part of the rectangle inside it stays where the game draws it, and
                    // the moved rectangle is pushed right of the creature when both share rows.
                    float tipShift=std::max(0.f,handSpan+12.f*kk-tx0);
                    bool keep=false;float k0x=0,k0y=0,k1x=0,k1y=0;
                    if(p.keepValid) {
                        k0x=std::max(p.keepRect[0]-12.f,xl);k0y=std::max(p.keepRect[1]-12.f,yt);k1x=std::min(p.keepRect[2]+12.f,Xb);k1y=std::min(p.keepRect[3]+12.f,yb);
                        keep=k1x>k0x&&k1y>k0y;
                        if(keep&&ty0<k1y&&ty1>k0y&&tx0+tipShift<k1x+6.f&&tx1+tipShift>k0x) tipShift=std::max(tipShift,k1x+6.f-tx0);
                    }
                    tipShift=std::min(tipShift,2.5f*R);
                    part(xl,Xb,yt,ty0,0.f);part(xl,Xb,ty1,yb,0.f);part(xl,tx0,ty0,ty1,0.f);part(tx1,Xb,ty0,ty1,0.f);   // everything else where the game draws it
                    const float c0x=std::max(tx0,k0x),c0y=std::max(ty0,k0y),c1x=std::min(tx1,k1x),c1y=std::min(ty1,k1y);
                    if(keep&&c1x>c0x&&c1y>c0y) {                                                                       // creature inside the rectangle: kept in place
                        part(c0x,c1x,c0y,c1y,0.f);
                        part(tx0,tx1,ty0,c0y,tipShift);part(tx0,tx1,c1y,ty1,tipShift);part(tx0,c0x,c0y,c1y,tipShift);part(c1x,tx1,c0y,c1y,tipShift);
                    } else part(tx0,tx1,ty0,ty1,tipShift);                                                             // the tooltip + icons beside the hand
                    static double lastTipLog=0;const double nowT=milliseconds();
                    if(nowT-lastTipLog>2000) {lastTipLog=nowT;log("Held window tooltip (0.8.94.34): rect %.0f,%.0f-%.0f,%.0f creature %s %.0f,%.0f-%.0f,%.0f (raw %.0f,%.0f-%.0f,%.0f) shift %.1f handSpan %.1f parts %d",
                        tx0,ty0,tx1,ty1,keep?"kept":"none",k0x,k0y,k1x,k1y,p.keepRect[0],p.keepRect[1],p.keepRect[2],p.keepRect[3],tipShift,handSpan,nq/6);}
                } else part(xl,Xb,yt,yb,0.f);  // no tooltip: the whole window unchanged
                (void)blockShift;(void)Hr;
                (void)xr;
                (void)sL;(void)sR;(void)sU;(void)sD;
                d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);d->SetRenderState(D3DRS_ALPHAREF,4);
                d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_BORDER);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_BORDER);d->SetSamplerState(0,D3DSAMP_BORDERCOLOR,0);
                d->SetTexture(0,ht);
                // 0.8.27: the window floats at the hand, close to the table: the table frame / dungeon relief cut into it (report 175651: the held creature was
                // half hidden). What is held is always shown on top: no depth test for this window (the hand sprite, drawn after, keeps its own).
                d->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
                if(nq>0&&FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(nq/3),hq,sizeof(HudVertex)))) ++sDrawFailures;
                d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
                d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
            }
            drawHeld3D(d,p,r,eye);
            d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
            d->SetRenderState(D3DRS_ALPHAREF,24);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
            d->SetTexture(0,ct);
            if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,2,tri,sizeof(HudVertex)))) ++sDrawFailures;
            (void)h;
        }
        d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
        d->SetTexture(0,nullptr);
    }
    void openResources(const Package& p) {
        if(cacheEpoch!=p.epoch) {textures.clear();shaders.clear();cacheEpoch=p.epoch;}
        for(auto& c:p.copies) {
            auto it=textures.find(c.id);
            if(it!=textures.end()) {it->second.lastFrame=frames;continue;}
            Opened o;HANDLE h=c.handle;
            HRESULT hr=dev->CreateTexture(c.w,c.h,c.levels,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&o.texture,&h);
            if(FAILED(hr)) {if(sOpenFailures++<5) log("VR thread: opening texture copy %u (%ux%u, %u levels) failed %08lX",c.id,c.w,c.h,c.levels,(unsigned long)hr);continue;}
            o.lastFrame=frames;o.w=std::max(c.w,c.h);o.levels=c.levels;++sizeSeen[o.w*100+std::min<UINT>(c.levels,99)];textures.emplace(c.id,std::move(o));++sOpened;
        }
        if(p.desktop&&textures.find(p.desktop)==textures.end()) {
            Opened o;HANDLE h=p.desktopHandle;
            if(SUCCEEDED(dev->CreateTexture(p.desktopWidth,p.desktopHeight,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&o.texture,&h))) {o.lastFrame=frames;textures.emplace(p.desktop,std::move(o));}
        } else if(p.desktop) textures[p.desktop].lastFrame=frames;
        if(p.held&&textures.find(p.held)==textures.end()) {
            Opened o;HANDLE h=p.heldHandle;
            if(SUCCEEDED(dev->CreateTexture(p.hudWidth,p.hudHeight,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&o.texture,&h))) {o.lastFrame=frames;textures.emplace(p.held,std::move(o));}
        } else if(p.held) textures[p.held].lastFrame=frames;
        if(p.hud&&textures.find(p.hud)==textures.end()) {
            Opened o;HANDLE h=p.hudHandle;
            if(SUCCEEDED(dev->CreateTexture(p.hudWidth,p.hudHeight,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&o.texture,&h))) {o.lastFrame=frames;textures.emplace(p.hud,std::move(o));}
        } else if(p.hud) textures[p.hud].lastFrame=frames;
        for(auto& s:p.shaders) {
            if(shaders.count(s.id)) continue;
            ComPtr<IDirect3DPixelShader9> ps;
            if(SUCCEEDED(dev->CreatePixelShader(s.code.data(),&ps))) shaders.emplace(s.id,ps);
        }
        if(frames%600==0) for(auto it=textures.begin();it!=textures.end();) {if(frames-it->second.lastFrame>900) it=textures.erase(it); else ++it;}
    }
    IDirect3DTexture9* texture(uint32_t id) {if(!id) return nullptr;auto it=textures.find(id);return it==textures.end()?nullptr:it->second.texture.Get();}
    void renderFrame(const Request& r) {
        const Package& p=*current;
        int slot=-1;
        for(unsigned i=0;i<SlotCount;++i) if(InterlockedCompareExchange(&ipc->data->slots[i].state,1,0)==0) {slot=int(i);break;}
        if(slot<0) {++sNoSlot;return;}
        auto release=[&]{InterlockedExchange(&ipc->data->slots[slot].state,0);};
        if(r.width<64||r.height<64||r.width>4096||r.height>4096||!p.desktopWidth) {release();return;}
        auto& im=images[slot];
        if(!ensureImages(im,r.width,r.height,p.desktopWidth,p.desktopHeight)) {release();failure();return;}
        {   // 0.9.25 PERF: GPU scheduling priority of this device (automatic A/B test or fixed GpuPriorityVR)
            auto& S=perfab::shared();
            const int want=S.enabled.load()?perfab::configs[std::clamp(S.config.load(),0,perfab::Configs-1)].vr:S.fixedVr.load();
            if(want!=vrPriorityApplied) {
                const HRESULT h=dev->SetGPUThreadPriority(want);vrPriorityApplied=want;
                if(vrPriorityLogs<8) {++vrPriorityLogs;log("PERF (0.9.25): VR thread GPU priority %+d (%s, %08lX)",want,SUCCEEDED(h)?"OK":"refused",(unsigned long)h);}
            }
        }
        double started=milliseconds();
        const bool packageSwitched=p.id!=lastPackageId;
        if(packageSwitched) {openResources(p);lastPackageId=p.id;++sPackages;sAge+=started-p.recordedMs;}
        ++frames;
        const bool menu=p.menu&&!p.possession;
        if(!menu) menuUiOk=false;   // 0.8.90.7: a new menu visit never shows the previous visit's interface layer
        const bool poss=p.possession; // 0.8.68: first-person world package (no table, hands, interface or room)
        lastPointerFlags=uint32_t(r.pointerFlags);if(!p.menu) interp.update(p,started);const unsigned char* vbase=p.menu?p.vertices.data():interp.vertices(p);
        // Record-LOCAL -> current LOCAL: newest table pose/width, and the pan still to be done by the game.
        float ratio=p.tableWidth>0?r.tableWidthMeters/p.tableWidth:1.f;
        Vec3 pan{0,0,0};
        // Pan prediction: the game moves its camera by the hand motion at each game frame; the part of the
        // motion made since this game frame is applied here, so the dungeon follows the hand at the headset rate.
        bool gripping=(r.handFlags&(p.panHand?HandRightGrip:HandLeftGrip))&&(r.handFlags&(p.panHand?HandRightValid:HandLeftValid))&&!(r.handFlags&(p.panHand?HandRightTable:HandLeftTable));
        if(gripping) {lastGrip=r.hands[p.panHand].position;lastGripMs=started;}
        if(p.panning) {
            ++panDiag.frames;
            if(lastGripMs+100<p.recordedMs) ++panDiag.stale;
        }
        if(panPrediction&&p.panning&&lastGripMs+100>=p.recordedMs) {
            pan=panInTable(sub(lastGrip,p.panFrom),r.table,ratio,panLimit);sPan+=std::sqrt(pan.x*pan.x+pan.z*pan.z);
            const Vec3 raw=panInTable(sub(lastGrip,p.panFrom),r.table,ratio,1e9f);
            const double rawLen=std::sqrt(double(raw.x)*raw.x+double(raw.z)*raw.z),usedLen=std::sqrt(double(pan.x)*pan.x+double(pan.z)*pan.z);
            ++panDiag.applied;panDiag.rawSum+=rawLen;panDiag.rawMax=std::max(panDiag.rawMax,rawLen);
            if(rawLen>usedLen+1e-4) {++panDiag.clamped;panDiag.clampLoss+=rawLen-usedLen;}
            // Continuity at a package switch: the dungeon shown with the new game frame = its own content shift (the hand moved
            // between the two frames) + the new pan; with the old frame it was the old pan. Any difference is a visible jump.
            if(packageSwitched&&panDiag.prevPan&&panDiag.prevHand==p.panHand&&p.id>panDiag.prevId&&p.id-panDiag.prevId<=3) {
                const Vec3 shift=panInTable(sub(p.panFrom,panDiag.prevFrom),r.table,ratio,1e9f);
                const Vec3 oldPan=panInTable(sub(lastGrip,panDiag.prevFrom),r.table,ratio,panLimit);
                const double jx=double(shift.x)+pan.x-oldPan.x,jz=double(shift.z)+pan.z-oldPan.z,j=std::sqrt(jx*jx+jz*jz);
                ++panDiag.switches;panDiag.jumpSum+=j;panDiag.jumpMax=std::max(panDiag.jumpMax,j);
                if(j>.005) ++panDiag.jump5;if(j>.010) ++panDiag.jump10;if(j>.020) ++panDiag.jump20;
            }
        }
        if(packageSwitched) {panDiag.prevPan=p.panning;panDiag.prevFrom=p.panFrom;panDiag.prevHand=p.panHand;panDiag.prevId=p.id;}
        // Turn prediction (two grips): the part of the hands' rotation made since this game frame.
        float yaw=0;
        const uint32_t both=HandLeftGrip|HandRightGrip|HandLeftValid|HandRightValid;
        if((r.handFlags&both)==both&&!(r.handFlags&(HandLeftTable|HandRightTable))) {lastTurn=handsAngle(r.hands[0].position,r.hands[1].position);lastTurnMs=started;}
        if(p.turning) {++panDiag.tFrames;if(lastTurnMs+100<p.recordedMs) ++panDiag.tStale;}
        if(panPrediction&&p.turning&&lastTurnMs+100>=p.recordedMs) {
            yaw=std::clamp(wrapRadians(lastTurn-p.turnFrom),-.6f,.6f);sTurn+=std::fabs(yaw);
            ++panDiag.tApplied;panDiag.tRawMax=std::max(panDiag.tRawMax,double(std::fabs(wrapRadians(lastTurn-p.turnFrom))));
            if(packageSwitched&&panDiag.prevTurn&&p.id>panDiag.prevId&&p.id-panDiag.prevId<=3) {
                const float shift=wrapRadians(p.turnFrom-panDiag.prevTurnFrom);
                const float oldYaw=std::clamp(wrapRadians(lastTurn-panDiag.prevTurnFrom),-.6f,.6f);
                const double j=std::fabs(double(shift)+yaw-oldYaw)*180./Pi;
                ++panDiag.tSwitches;panDiag.tJumpSum+=j;panDiag.tJumpMax=std::max(panDiag.tJumpMax,j);
                if(j>.5) ++panDiag.tJump05;if(j>1.) ++panDiag.tJump1;if(j>2.) ++panDiag.tJump2;
            }
        }
        if(packageSwitched) {panDiag.prevTurn=p.turning;panDiag.prevTurnFrom=p.turnFrom;
            if(!poss&&probePrev&&probePrev.get()!=&p) motionProbe.compare(*probePrev,p,r.table,ratio);
            probePrev=current;}
        // 0.8.66: thumbstick scroll/turn, integrated here at the headset rate and applied by the game at its own rate;
        // the part not yet applied is shown now (same mechanism as the grip prediction above).
        if(!poss) StickDrive::integrate(r,started);
        if(p.stickOn) {
            ++stickDiag.frames;
            if(panPrediction&&StickDrive::fresh(started)) {
                Vec3 h;float a;StickDrive::snapshot(h,a);
                const Vec3 sp=panInTable(sub(h,p.stickFrom),r.table,ratio,panLimit);
                const float sy=std::clamp(wrapRadians(a-p.stickTurnFrom),-.6f,.6f);
                pan=add(pan,sp);yaw+=sy;
                const double o=std::sqrt(double(sp.x)*sp.x+double(sp.z)*sp.z);
                ++stickDiag.applied;stickDiag.offSum+=o;stickDiag.offMax=std::max(stickDiag.offMax,o);
                stickDiag.yawSum+=std::fabs(sy);stickDiag.yawMax=std::max(stickDiag.yawMax,double(std::fabs(sy)));
            }
        }
        // 0.3.1: the physical table (base, clipping) never follows the pending pan/turn; only the dungeon does.
        Similarity tableFix=tableCorrection(p.table,r.table,ratio,{0,0,0});
        Similarity correction=dungeonCorrection(p.table,r.table,ratio,pan,yaw,p.pivot);
        if(poss) {
            // 0.8.68: record-LOCAL = LOCAL (the possession world is recorded in the headset space around the anchor);
            // the turn and walk the game has not drawn yet are measured on its geometry (PossessionMotion).
            possMotion.noteCommand(r,started);
            if(packageSwitched) possMotion.observe(current,started);
            float py=0;Vec3 pp{0,0,0};possMotion.extrapolate(p,r,started,py,pp);
            const Pose identityPose{{0,0,0,1},{0,0,0}};
            correction=dungeonCorrection(identityPose,identityPose,1.f,pp,py,p.possPivot);
        } else if(possMotion.prev) possMotion.reset();
        // 0.8.84 360 VIEW: cube map. The 6 faces of the same game frame are already in the record space of the normal view: they take exactly its correction.
        static const bool viewDebug=iniInt(L"Possession360Debug",0)!=0;   // colours every face with its own channel mix (diagnostic of the seams)
        const bool cubeOn=poss&&auxEnabled&&p.cube;
        if(poss&&auxEnabled) {++sAuxFrames;if(cubeOn) ++sAuxDrawn; else ++sAuxNoPlace;}
        const float depthRatio=p.tableWidth>0?p.tableDepth/p.tableWidth:.75f;
        // 0.6.1: HUD modules (layout shared with the game thread), laser length from this frame's hit.
        HudLayout hud;const bool hudOn=p.hud&&texture(p.hud)&&p.hudWidth>=64;
        if(hudOn) hud=layoutHud(r,p.hudSizes,tableAspect>0?tableAspect:depthRatio,p.hudTop,float(p.hudWidth),float(p.hudHeight),p.screenShown,p.pedestalSide,p.screenWhole||p.panelWhole,&p.msgRegion,p.statsShown);   // 0.8.94.3: whole band only for the game's own menu
        // 0.8.94 (first written in 0.8.93.2, the only part of it kept): camera cinematic (game camera mode 18): the game draws no bars, only a subtitle line at the bottom of the screen, which used to be cut between the minimap
        // module (start of the sentence on the pedestal plate) and the bottom module (end on the wall). One band over the whole width replaces every module.
        const bool cinematicHud=hudOn&&p.mode==18&&!menu&&!poss&&CinematicView::enabled;   // 0.8.94: game cinematic on the table
        if(cinematicHud&&hud.panel[HudBottom].shown) {
            HudPanel& b=hud.panel[HudBottom];
            const float ow=b.x1-b.x0,oh=b.y1-b.y0;
            if(ow>8&&oh>8) {
                const float iw=ow/(1.f-HudRegions[HudBottom].u0),ih=oh/(HudRegions[HudBottom].v1-HudRegions[HudBottom].v0);
                const float ox=b.x0-HudRegions[HudBottom].u0*iw,oy=b.y0-HudRegions[HudBottom].v0*ih;
                const float nx0=ox,nx1=ox+iw,ny0=oy+.84f*ih,ny1=oy+.995f*ih,nw=b.w*iw/ow*1.35f;
                b.x0=nx0;b.x1=nx1;b.y0=ny0;b.y1=ny1;b.h=nw*(ny1-ny0)/(nx1-nx0);b.w=nw;
                for(int i=0;i<HudCount;++i) if(i!=HudBottom) hud.panel[i].shown=false;
                hud.pedestal=false;hud.button=false;
            }
        }
        Request rr=r;
        bool buttonLaser=false;
        // 0.8.94.23: the nearest panel wins. A game panel hit by the laser in front of the wall options (report 20261005_120733 capture 1: the laser went
        // through the notification box to the OPTIONS panel behind it) is reported to the host, which then leaves the laser to the game.
        hudFront=false;
        if(hudOn&&(r.pointerFlags&PointerAimValid)) {float th=0,fx2=0,fy2=0,tc=0;
            if(hitHud(hud,r.rayOrigin,r.rayDirection,float(p.hudWidth),float(p.hudHeight),th,fx2,fy2)>=0&&comfortHit(r,r.rayOrigin,r.rayDirection,tc)&&th<tc) hudFront=true;}
        if(r.pointerFlags&PointerAimValid) {
            float t=0,fx=0,fy=0;int hit=-1;
            if(!(r.pointerFlags&PointerActive)&&hudOn) hit=hitHud(hud,r.rayOrigin,r.rayDirection,float(p.hudWidth),float(p.hudHeight),t,fx,fy);
            if(hit>=0&&p.pickDistance>0&&p.pickDistance<t) hit=-1;
            {float tb=0;const float lim=hit>=0?t:(p.pickDistance>0?p.pickDistance:4.f);
             if(hudOn&&!(r.pointerFlags&PointerActive)&&hitButton(hud,r.rayOrigin,r.rayDirection,tb,lim)) {hit=-1;rr.rayLength=tb;rr.pointerFlags|=PointerActive;buttonLaser=true;}}
            if(buttonLaser) {}
            else if(r.pointerFlags&PointerActive) rr.rayLength=r.rayLength>0?r.rayLength:.30f;
            else if(hit>=0) {rr.rayLength=t;rr.pointerFlags|=PointerActive;}
            else {
                // 0.6.6: laser stops on the table surface (same plane as the game-thread pointer).
                rr.rayLength=.30f;
                if(menu) {   // 0.9.47.47: main-menu room: the laser is a real pointer, it travels to the walls of the room (options block, screen) instead of a 30 cm stub
                    const float fy=(r.handFlags&RequestFloorKnown)?r.floorY:levelTable(r).position.y-.78f;
                    const Pose room{r.anchor.orientation,{r.anchor.position.x,fy,r.anchor.position.z}};
                    const Vec3 o=toLocal(room,r.rayOrigin),dv=rotate(conjugate(room.orientation),r.rayDirection);
                    float tm=4.5f;
                    auto plane=[&](float oc,float dc,float pos) {if(std::fabs(dc)>1e-4f) {const float t=(pos-oc)/dc;if(t>.05f&&t<tm) tm=t;}};
                    plane(o.x,dv.x,-ComfortWallX);plane(o.x,dv.x,ComfortWallX);plane(o.z,dv.z,MenuScreenZ);plane(o.y,dv.y,0.f);plane(o.y,dv.y,2.6f);
                    rr.rayLength=std::clamp(tm-.02f,.30f,4.5f);
                }
                if(p.pickDistance>0) {rr.rayLength=p.pickDistance;rr.pointerFlags|=PointerActive;}
                else if(const Vec3 up=rotate(r.table.orientation,Vec3{0,1,0});true) {const float den=dot(up,r.rayDirection);
                if(!menu&&tablePointerVr&&std::fabs(den)>1e-4f&&p.tableWidth>0) {
                    const float tt=dot(up,sub(r.table.position,r.rayOrigin))/den;
                    const Vec3 q=toLocal(r.table,add(r.rayOrigin,scale(r.rayDirection,tt)));
                    const float hw=r.tableWidthMeters*.5f,hd=p.tableDepth*r.tableWidthMeters/p.tableWidth*.5f;
                    if(tt>.02f&&tt<6.f&&std::fabs(q.x)<=hw&&std::fabs(q.z)<=hd) {rr.rayLength=tt;rr.pointerFlags|=PointerActive;}
                }}
            }
        } else rr.rayLength=0;
        const bool handsOn=showHands&&!poss; // 0.8.68: no controllers, glow or laser in the first-person world
        std::vector<OverlayVertex> hands;hands.reserve(400);if(handsOn) buildHands(rr,hands,false);
        std::vector<OverlayVertex> glow;if(handsOn) buildGlow(r,p.hudTop,glow);
        std::vector<OverlayVertex> laserMesh;if(handsOn) buildLaser(rr,laserMesh); // 0.8.23b: drawn AFTER the HUD panels (depth-tested): the laser no longer disappears behind the interface
        std::vector<OverlayVertex> pedestalMesh,pedestalPartsMesh;std::vector<PedVertex> pedestalTexMesh; // flat fallback / flat parts kept with the texture (button, arrow) / textured stone
        if(hudOn&&hud.pedestal) {buildPedestal(hud,pedestalMesh,p.buttonHover);buildPedestal(hud,pedestalPartsMesh,p.buttonHover,&pedestalTexMesh);}
        // 0.6.9.16: opaque stone slab with a darker frame behind every HUD panel (user request): the panels no longer
        // show what lies behind them. Minimap excluded (it lies on the pedestal plate).
        if(hudOn&&hudBacking) for(int i=0;i<HudCount;++i) {
            const HudPanel& q=hud.panel[i];if(!q.shown||i==HudMinimap||(i==HudScreen&&!p.screenWhole&&!p.panelWhole)) continue; // 0.6.9.17: not behind the message panel (often shown empty)
            const float w=q.w*.5f,h=q.h*.5f,f=.012f;
            coloredBox(pedestalMesh,q.pose,{-w-f,-h-f,-.020f},{w+f,h+f,-.010f},0xff2b2723,0xff1f1c19);       // frame
            coloredBox(pedestalMesh,q.pose,{-w,-h,-.010f},{w,h,-.003f},0xff4d4741,0xff35302b);               // stone face
        }
        env::Scene::FrameState envFrame;
        const int pmode=probeMode.load();
        Request menuReq=r;if(menu) menuReq.tableAnchor=0; // 0.8.88: no table in the menu room
        bool envOn=!poss&&environment&&pmode!=2&&pmode!=3&&!scene.failed&&(scene.ready||scene.init(dev.Get()))&&scene.prepare(dev.Get(),menuReq,tableAspect,depthRatio,envFrame);
        if(envOn) envFrame.menuFrame=menu;
        const bool passthrough=(r.comfortFlags&ComfortPassthrough)!=0&&!poss;scene.passthrough=passthrough;   // 0.8.94.4: no room, door, torches (table, interface and hands stay)
        const bool envLoading=!envOn&&!poss&&environment&&pmode!=2&&pmode!=3&&!scene.failed&&!scene.ready;   // 0.8.91: black room + loading gauge until the room is ready
        if(envLoading&&!loadPoseSet) {const Pose& h=r.eyes[0].pose;const Quat y=yawOnly(h.orientation);loadPose={y,toWorld(Pose{y,h.position},{0.f,-.05f,-1.6f})};loadPoseSet=true;}
        if(!envLoading) loadPoseSet=false;
        float loadAspect=1.f;IDirect3DTexture9* loadPic=envLoading?scene.loadAnimFrame(dev.Get(),milliseconds(),loadAspect):nullptr;
        if(!envLoading&&scene.ready) scene.releaseLoadAnim();
        if(!envOn&&!poss) buildHandles(r,depthRatio,hands); // 0.3.2 handles when the room is off
        D3DMATRIX identity{};identity._11=identity._22=identity._33=identity._44=1;
        bool ok=true;
        gpuTimer.collect();gpuTimer.begin();
        for(int e=0;e<2;++e) {
            IDirect3DDevice9Ex* d=dev.Get();
            const bool msaa=samples!=D3DMULTISAMPLE_NONE&&pmode!=1&&pmode!=3&&msaaColor&&msaaW==im.w&&msaaH==im.h;
            d->SetRenderTarget(0,msaa?msaaColor.Get():im.target[e].Get());d->SetDepthStencilSurface(msaa?msaaDepth.Get():im.depth.Get());
            D3DVIEWPORT9 vp{0,0,im.w,im.h,0,1};d->SetViewport(&vp);
            d->Clear(0,nullptr,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,envLoading?scene.loadBackground():passthrough?passthroughClear(r):0xff080b10,1,0);   // 0.8.94.4: passthrough test = pure green   // 0.8.94.2: pure black while loading (matches the loading picture)
            if(FAILED(d->BeginScene())) {ok=false;continue;}
            d->SetVertexShader(nullptr);d->SetPixelShader(nullptr);
            // Same convention as 0.2.9: VIEW = identity, so world space = eye space for the clip planes.
            // WORLD = record-LOCAL -> current LOCAL -> eye (dungeon, base) or current LOCAL -> eye (hands).
            const Similarity eyeInverse=inverse(similarity(r.eyes[e].pose));
            const Similarity toEye=then(correction,eyeInverse),tableToEye=then(tableFix,eyeInverse);
            D3DMATRIX world{},tableWorld{},eyeWorld{},proj{};
            matrixOf(toEye,&world._11);matrixOf(tableToEye,&tableWorld._11);
            matrixOf(inverse(similarity(r.eyes[e].pose)),&eyeWorld._11);
            projectionMatrix(r.eyes[e].fov,.015f,200.f,&proj._11);
            d->SetTransform(D3DTS_VIEW,&identity);d->SetTransform(D3DTS_PROJECTION,&proj);
            if(envLoading) {   // 0.8.91: nothing but a dark space and the loading gauge while the room is being prepared (the half-built scene looked broken)
                std::vector<OverlayVertex> g;g.reserve(72);const float f=std::clamp(scene.progress(),0.f,1.f);
                // 0.8.94.2: dark grey outline, black track, blood-red gauge (user request)
                coloredBox(g,loadPose,{-.62f,-.032f,-.02f},{.62f,.032f,0.f},0xff3a3a3a,0xff262626);   // 0.9.47.29: dark grey frame (brown was tried in 0.9.47.29), thinner gauge (user request)
                coloredBox(g,loadPose,{-.61f,-.022f,.001f},{.61f,.022f,.004f},0xff000000,0xff000000);
                if(f>.001f) coloredBox(g,loadPose,{-.61f,-.022f,.005f},{-.61f+1.22f*f,.022f,.012f},0xff8a0303,0xff5a0202);
                d->SetTransform(D3DTS_WORLD,&eyeWorld);d->SetPixelShader(nullptr);d->SetVertexShader(nullptr);d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
                for(int i=0;i<Snapshot::Samplers;++i) d->SetTexture(i,nullptr);
                d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_FOGENABLE,FALSE);
                d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);
                d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);d->SetRenderState(D3DRS_COLORWRITEENABLE,15);
                d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
                if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(g.size()/3),g.data(),sizeof(OverlayVertex)))) ++sDrawFailures;
                if(loadPic) {   // 0.8.94.2: animated picture centred above the gauge
                    const float hw=.36f,hh=hw/std::max(.2f,loadAspect),y0=.07f,y1=y0+2*hh;   // 0.9.47.29: picture 20 % smaller, closer to the thinner gauge
                    auto P=[&](float x,float y){return toWorld(loadPose,Vec3{x,y,0.f});};
                    const PedVertex q[6]={{P(-hw,y1),0xffffffff,0,0},{P(hw,y1),0xffffffff,1,0},{P(hw,y0),0xffffffff,1,1},{P(-hw,y1),0xffffffff,0,0},{P(hw,y0),0xffffffff,1,1},{P(-hw,y0),0xffffffff,0,1}};
                    d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);d->SetTexture(0,loadPic);
                    d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
                    d->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);d->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);d->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
                    d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
                    if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,2,q,sizeof(PedVertex)))) ++sDrawFailures;
                    d->SetTexture(0,nullptr);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
                }
                if(FAILED(d->EndScene())) ok=false;
                if(msaa&&FAILED(d->StretchRect(msaaColor.Get(),nullptr,im.target[e].Get(),nullptr,D3DTEXF_NONE))) ok=false;
                continue;
            }
            if(envOn) {
                double envStart=milliseconds();
                d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
                d->SetRenderState(D3DRS_FOGENABLE,FALSE);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);d->SetRenderState(D3DRS_CLIPPING,TRUE);
                scene.drawOpaque(d,envFrame,r.eyes[e].pose,proj);
                d->SetVertexShader(nullptr);d->SetPixelShader(nullptr);
                sEnv+=milliseconds()-envStart;
            }
            d->SetTransform(D3DTS_WORLD,&tableWorld);
            d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
            d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);d->SetRenderState(D3DRS_CLIPPING,TRUE);d->SetRenderState(D3DRS_FOGENABLE,FALSE);
            d->SetRenderState(D3DRS_DEPTHBIAS,0);d->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,0);
            // 0.6.8: pedestal before the tray (the tray writes no depth: drawn after, the pedestal showed through it).
            IDirect3DTexture9* pedestalTexture=pedestalMesh.empty()?nullptr:scene.pedestalTexture(d);
            if(pedestalTexture) pedestalMesh=pedestalPartsMesh;
            if(!pedestalMesh.empty()||pedestalTexture) {
                d->SetTransform(D3DTS_WORLD,&eyeWorld);d->SetPixelShader(nullptr);d->SetVertexShader(nullptr);d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
                for(int i=0;i<Snapshot::Samplers;++i) d->SetTexture(i,nullptr);
                d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
                d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);
                d->SetRenderState(D3DRS_COLORWRITEENABLE,15);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_LIGHTING,FALSE);
                d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
                if(!pedestalMesh.empty()&&FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(pedestalMesh.size()/3),pedestalMesh.data(),sizeof(OverlayVertex)))) ++sDrawFailures;
                // 0.9.4: the 0.9.3 dot3 path (normal map in fixed-function stages) lost the colour on the player's GPU (grey-white pedestal):
                // removed. The relief of the user's normal map is baked into piedestal_color at packaging time; one texture stage, as before.
                const bool pedLit=pedestalTexture&&!pedestalTexMesh.empty()&&envOn&&scene.pedestalLit(d);
                if(pedLit) {   // 0.9.6: lit by the room (user's colour + normal map, room shader)
                    std::vector<env::Vtx> lv;lv.reserve(pedestalTexMesh.size());
                    for(const PedVertex& q:pedestalTexMesh) {env::Vtx x{};x.p=q.p;x.n=q.n;x.t[0]=q.t[0];x.t[1]=q.t[1];x.t[2]=q.t[2];x.t[3]=q.t[3];x.u=q.u;x.v=q.v;x.ao=float((q.c>>16)&255)/255.f;x.color=q.c;lv.push_back(x);}
                    scene.drawPedestalLit(d,envFrame,r.eyes[e].pose,proj,lv);
                    d->SetTransform(D3DTS_WORLD,&eyeWorld);d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
                    d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
                    d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
                    d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
                }
                else if(pedestalTexture&&!pedestalTexMesh.empty()) {   // 0.8.93.1: slab texture on plate / rim / column
                    // 0.9.4: clean slate before the user texture: no other texture, transform, sRGB or blend state can mix into it
                    for(int i=1;i<Snapshot::Samplers;++i) d->SetTexture(i,nullptr);
                    d->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);d->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);d->SetTextureStageState(0,D3DTSS_RESULTARG,D3DTA_CURRENT);
                    d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);d->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
                    d->SetSamplerState(0,D3DSAMP_SRGBTEXTURE,0);d->SetSamplerState(0,D3DSAMP_MIPMAPLODBIAS,0);d->SetSamplerState(0,D3DSAMP_MAXMIPLEVEL,0);
                    d->SetRenderState(D3DRS_SRGBWRITEENABLE,FALSE);d->SetRenderState(D3DRS_SPECULARENABLE,FALSE);d->SetRenderState(D3DRS_WRAP0,0);
                    d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);d->SetTexture(0,pedestalTexture);
                    d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
                    d->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);d->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);d->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
                    d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);d->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
                    d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
                    if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(pedestalTexMesh.size()/3),pedestalTexMesh.data(),sizeof(PedVertex)))) ++sDrawFailures;
                    d->SetTexture(0,nullptr);d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
                    d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
                }
                d->SetTransform(D3DTS_WORLD,&tableWorld);
            }
            // Table base first, without depth: every dungeon level (pits, water) is drawn over it.
            const bool woodenTray=envOn&&scene.drawTray(d,envFrame,r.eyes[e].pose,proj);
            if(woodenTray) d->SetTransform(D3DTS_WORLD,&tableWorld);
            if(!p.base.empty()&&!woodenTray) {
                d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);d->SetTexture(0,nullptr);d->SetTexture(1,nullptr);
                d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
                d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
                // Tray: never writes depth (pits below it stay visible); with the wooden table it is depth
                // tested so the raised rim and the hands stay in front of it.
                d->SetRenderState(D3DRS_ZENABLE,envOn?D3DZB_TRUE:D3DZB_FALSE);d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
                d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_COLORWRITEENABLE,15);
                d->SetPixelShader(nullptr);d->SetRenderState(D3DRS_SRGBWRITEENABLE,FALSE);d->SetRenderState(D3DRS_FOGENABLE,FALSE);
                if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(p.base.size()/3),p.base.data(),sizeof(OverlayVertex)))) ++sDrawFailures;
            }
            if(!p.skirt.empty()&&p.clip&&!poss) {   // 0.8.92: the cut of the dungeon at the table edges is closed (depth written: what lies behind it stays hidden)
                d->SetTransform(D3DTS_WORLD,&tableWorld);d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);d->SetTexture(0,nullptr);d->SetTexture(1,nullptr);d->SetPixelShader(nullptr);d->SetVertexShader(nullptr);
                d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
                d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
                d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
                d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_COLORWRITEENABLE,15);
                d->SetRenderState(D3DRS_SRGBWRITEENABLE,FALSE);d->SetRenderState(D3DRS_FOGENABLE,FALSE);d->SetRenderState(D3DRS_LIGHTING,FALSE);
                if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(p.skirt.size()/3),p.skirt.data(),sizeof(OverlayVertex)))) ++sDrawFailures;
            }
            d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
            d->SetTransform(D3DTS_WORLD,&world); // dungeon: table pose + pending pan/turn
            if(p.clip) {
                const DWORD count=p.ceiling?5:4;
                for(DWORD i=0;i<count;++i) {
                    Plane plane{{p.planes[i][0],p.planes[i][1],p.planes[i][2]},p.planes[i][3]};
                    Plane eye=transformPlane(plane,tableToEye); // the clipping rectangle belongs to the table
                    float c[4]={eye.normal.x,eye.normal.y,eye.normal.z,eye.offset};
                    d->SetClipPlane(i,c);
                }
                d->SetRenderState(D3DRS_CLIPPLANEENABLE,p.ceiling?31:15);
            }
            else d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);
            uint32_t applied=UINT32_MAX;DWORD fvf=0;
            // 0.5.0: state cache. Only values that differ from what the device already holds are sent
            // (0.4.7 re-sent ~110 states + 3 textures at every snapshot change: up to 15 ms per image).
            // The cache starts empty for the dungeon pass of each eye (room/tray draws changed states).
            DWORD cRS[Snapshot::RS],cTS[Snapshot::Stages][Snapshot::TS],cSS[Snapshot::Samplers][Snapshot::SS];
            bool vRS[Snapshot::RS]{},vTS[Snapshot::Stages][Snapshot::TS]{},vSS[Snapshot::Samplers][Snapshot::SS]{};
            DWORD cMin[Snapshot::Samplers]{},cAniso[Snapshot::Samplers]{},cBias[Snapshot::Samplers]{};bool vX[Snapshot::Samplers]{};
            uint32_t cTex[Snapshot::Samplers];bool vTex[Snapshot::Samplers]{};uint32_t cShader=0;bool vShader=false;
            DWORD biasBits;memcpy(&biasBits,&lodBias,4);
            // 0.6.3: two passes. Pass 1 draws the Keeper's hand (native 3D cursor mesh, tagged in the game
            // thread) moved onto the pointing controller, scaled to CursorSize.
            const bool handOk=p.handCount>0&&(r.pointerFlags&PointerAimValid)&&p.handExtent>1e-4f;
            // 0.8.84: each cube face keeps ONLY its own pyramid (4 user clip planes through the camera): the 6 faces tile the sphere exactly once, a seam is a plane
            // shared by two faces (both sides are cut by the same plane), nothing is drawn twice.
            auto setFace=[&](int k) {
                for(int i=0;i<4;++i) {
                    const Vec3 n=p.faceN[k][i];Plane e=transformPlane(Plane{n,-dot(n,p.faceO)},toEye);
                    float c4[4]={e.normal.x,e.normal.y,e.normal.z,e.offset};d->SetClipPlane(i,c4);
                }
                d->SetRenderState(D3DRS_CLIPPLANEENABLE,15);
            };
            const bool menuStencil=menu&&!p.records.empty()&&p.menuFocal>0;bool menuStencilForce=false,menuBiasOn=false,menuScreenMode=false;float menuBiasScale=1.f;std::vector<unsigned char> screenCopy,spriteCopy,ribbonCopy;static const float ribbonMm=iniFloat(L"SelectionLineWidth",4.f,0.f,20.f);static uint64_t ribRecs=0,ribTris=0,ribDraws=0;static double ribLogMs=0;static uint64_t iconRebuilt=0,iconFailed=0,iconDraws=0;static double iconLogMs=0;   // 0.8.90: the menu relief is clipped to the window by the stencil
            auto drawRecs=[&](const Package& q,const unsigned char* vb,int pass,int view,int blend=-1,int overlayMode=0,int topMode=0) {   // overlayMode 0 all, 1 attack effects only, 2 without them
            bool overlayOn=false;   // 0.8.99: attack effects of the possessed creature: over everything, like in the game
            float biasSet=-12345.f;   // 0.9.8: depth bias last sent in pre-transformed (game depth) mode
            for(auto& rec:q.records) {
                if(rec.held3d) continue;
                if((overlayMode==1&&!rec.overlay)||(overlayMode==2&&rec.overlay)) continue;
                if(q.menu) rec.menuState.apply(d);
                if(menuBiasOn) {const float b=-std::min(float(&rec-q.records.data())*5e-8f,2e-5f);DWORD bb;memcpy(&bb,&b,4);d->SetRenderState(D3DRS_DEPTHBIAS,bb);}   // 0.8.90.5: later draw wins at equal depth (the game's painter order), no z-fighting (0.8.96: 5e-8 per draw, at most 2e-5: 2e-7 x 280 draws let far draws cut through near ones)
                if(rec.view!=view) continue;
                if((topMode==1&&rec.top)||(topMode==2&&!rec.top)) continue;   // 0.9.47.6: topMode 1 = all but the damage digits, 2 = only the damage digits
                if(menuScreenMode) {   // 0.9.8: the game's OWN depth bias (decals such as the banners in front of their wall: report 223007, the flat replay without it
                    // showed them lit / the wall fighting through them). In the thin slab (menuBiasScale) a bias is kept at >= 4 depth steps so it still counts.
                    float b=rec.depthBias*menuBiasScale;if(rec.depthBias!=0.f&&std::fabs(b)<2.4e-7f) b=rec.depthBias>0?2.4e-7f:-2.4e-7f;
                    if(b!=biasSet) {DWORD bb;memcpy(&bb,&b,4);d->SetRenderState(D3DRS_DEPTHBIAS,bb);biasSet=b;}
                }
                if(blend>=0&&rec.snapshot<q.snapshots.size()&&(q.snapshots[rec.snapshot].rs[4]!=0)!=(blend==1)) continue;   // 0.8.84: opaque / blended split of the cube faces
                if(rec.hand!=(pass==1)) continue;
                if(rec.snapshot!=applied) {
                    const auto& s=q.snapshots[rec.snapshot];
                    for(int i=0;i<Snapshot::Samplers;++i) if(!vTex[i]||cTex[i]!=s.copy[i]) {d->SetTexture(i,texture(s.copy[i]));cTex[i]=s.copy[i];vTex[i]=true;}
                    if(!vShader||cShader!=s.shader) {
                        ComPtr<IDirect3DPixelShader9> ps;if(s.shader) {auto it=shaders.find(s.shader);if(it!=shaders.end()) ps=it->second;}
                        d->SetPixelShader(ps.Get());cShader=s.shader;vShader=true;
                    }
                    for(size_t i=0;i<Snapshot::RS;++i) {const DWORD v=(menuStencilForce&&Snapshot::renderStates[i]==D3DRS_STENCILENABLE)?DWORD(TRUE):Snapshot::renderStates[i]==D3DRS_COLORWRITEENABLE?(viewDebug&&view>0?(view==1?15u:view==2?9u:view==3?10u:view==4?12u:view==5?11u:14u):15u):s.rs[i]; // 0.6.9.17: the headset always writes colour (0.8.83 debug: one channel per extra view)
                        if(!vRS[i]||cRS[i]!=v) {d->SetRenderState(Snapshot::renderStates[i],v);cRS[i]=v;vRS[i]=true;}}
                    for(int t=0;t<Snapshot::Stages;++t) for(size_t i=0;i<Snapshot::TS;++i)
                        if(!vTS[t][i]||cTS[t][i]!=s.ts[t][i]) {d->SetTextureStageState(t,Snapshot::stageStates[i],s.ts[t][i]);cTS[t][i]=s.ts[t][i];vTS[t][i]=true;}
                    for(int t=0;t<Snapshot::Samplers;++t) {
                        // samplerStates: 2 MAG, 3 MIN, 4 MIP, 5 LODBIAS, 6 MAXANISOTROPY. MIN/LODBIAS/MAXANISOTROPY are
                        // replaced (anisotropic filtering, VrLodBias) and handled by the override cache below.
                        for(size_t i=0;i<Snapshot::SS;++i) {
                            if(i==3||i==5||i==6) continue;
                            if(!vSS[t][i]||cSS[t][i]!=s.ss[t][i]) {d->SetSamplerState(t,Snapshot::samplerStates[i],s.ss[t][i]);cSS[t][i]=s.ss[t][i];vSS[t][i]=true;}
                        }
                        DWORD mn=s.ss[t][3],an=s.ss[t][6],bias=keepBias?s.ss[t][5]:biasBits;
                        if(s.copy[t]&&(mn==D3DTEXF_LINEAR||mn==D3DTEXF_ANISOTROPIC)&&anisotropy>1) {mn=D3DTEXF_ANISOTROPIC;an=DWORD(anisotropy);}
                        if(!vX[t]||cMin[t]!=mn) {d->SetSamplerState(t,D3DSAMP_MINFILTER,mn);cMin[t]=mn;}
                        if(!vX[t]||cAniso[t]!=an) {d->SetSamplerState(t,D3DSAMP_MAXANISOTROPY,an);cAniso[t]=an;}
                        if(!vX[t]||cBias[t]!=bias) {d->SetSamplerState(t,D3DSAMP_MIPMAPLODBIAS,bias);cBias[t]=bias;}
                        vX[t]=true;
                        if(t==0&&s.copy[t]&&sharpReports<4&&filterSeen.size()<32) {float b;memcpy(&b,&s.ss[t][5],4);
                            ++filterSeen[(s.ss[t][2]&15)|(s.ss[t][3]&15)<<4|(s.ss[t][4]&15)<<8|(uint32_t(int(std::clamp(b,-8.f,8.f)*4)+64)&255)<<12];}
                    }
                    applied=rec.snapshot;
                }
                if(rec.overlay!=overlayOn) {d->SetRenderState(D3DRS_ZFUNC,rec.overlay?D3DCMP_ALWAYS:D3DCMP_LESSEQUAL);overlayOn=rec.overlay;}
                if(rec.overlay&&e==0) ++sOverlayDraws;
                const DWORD rf=menuScreenMode?((rec.fvf&~D3DFVF_POSITION_MASK)|D3DFVF_XYZRHW):rec.fvf;
                const UINT rstride=menuScreenMode?rec.stride+4:rec.stride;
                const unsigned char* rv=menuScreenMode?screenCopy.data()+rec.screenVertices:vb+rec.vertices;
                if(e==0) {if(rec.top&&rec.digitVerts.empty()) ++gDigitStats.bare; else if(!rec.digitVerts.empty()&&(menuScreenMode||poss||rec.hand||q.digitGroups.empty())) ++gDigitStats.skippedHand;}
                if(!menuScreenMode&&!poss&&!rec.hand&&(!rec.sprites.empty()||(!rec.digitVerts.empty()&&!q.digitGroups.empty()))) {
                    spriteCopy.assign(vb+rec.vertices,vb+rec.vertices+size_t(rec.count)*rec.stride);
                    const Vec3 head=scale(add(r.eyes[0].pose.position,r.eyes[1].pose.position),.5f);
                    const Vec3 headRecord=apply(inverse(correction),head);
                    const SpriteReadingAxes reading{
                        rotate(conjugate(correction.rotation),scale(add(rotate(r.eyes[0].pose.orientation,{1,0,0}),rotate(r.eyes[1].pose.orientation,{1,0,0})),.5f)),
                        rotate(conjugate(correction.rotation),scale(add(rotate(r.eyes[0].pose.orientation,{0,0,-1}),rotate(r.eyes[1].pose.orientation,{0,0,-1})),.5f))};
                    for(const ReplaySprite& sprite:rec.sprites) {if(sprite.rebuild(spriteCopy,0,rec.stride,rec.count,headRecord,&reading)) ++iconRebuilt; else ++iconFailed;}   // 0.9.30: counted (log every 5 s)
                    if(++iconDraws&&iconLogMs+5000.<milliseconds()) {if(iconLogMs>0) log("Creature icons (0.9.30, VR thread, last 5 s): %llu pieces turned to the headset, %llu pieces could not be rebuilt (left as the game drew them), in %llu draws",(unsigned long long)iconRebuilt,(unsigned long long)iconFailed,(unsigned long long)iconDraws);iconLogMs=milliseconds();iconRebuilt=iconFailed=iconDraws=0;}
                    if(!rec.digitVerts.empty()&&!q.digitGroups.empty()) {   // 0.9.47.31: damage digits face the CURRENT headset position (same head for both eyes)
                        uint32_t viewGroup=0xffffffffu;digitnum::DigitView view;unsigned char* sc=spriteCopy.data();
                        if(e==0) ++gDigitStats.recs;
                        for(const auto& dv:rec.digitVerts) {
                            if(dv.group>=q.digitGroups.size()||dv.index>=rec.count) {if(e==0) ++gDigitStats.badIndex;continue;}
                            if(dv.group!=viewGroup) {
                                viewGroup=dv.group;view=digitnum::digitView(q.digitGroups[dv.group],headRecord);
                                if(e==0) {
                                    const double now=milliseconds();
                                    if(gDigitStats.logMs+2000.<now) {
                                        if(gDigitStats.logMs>0) log("Damage digits replay (0.9.47.33, since the last sample): %llu digit records drawn, %llu vertices turned to the headset, %llu groups with no usable view, %llu vertices out of range, %llu damage-digit records WITHOUT any linked vertex, %llu linked records not reached by the rule (hand / menu / possession)",
                                            (unsigned long long)gDigitStats.recs,(unsigned long long)gDigitStats.verts,(unsigned long long)gDigitStats.badView,(unsigned long long)gDigitStats.badIndex,(unsigned long long)gDigitStats.bare,(unsigned long long)gDigitStats.skippedHand);
                                        gDigitStats.logMs=now;gDigitStats.samples=0;gDigitStats.recs=gDigitStats.verts=gDigitStats.badView=gDigitStats.badIndex=gDigitStats.bare=gDigitStats.skippedHand=0;
                                    }
                                    if(gDigitStats.samples<2) {
                                        const auto& g=q.digitGroups[dv.group];++gDigitStats.samples;const Vec3 cx=rotate(correction.rotation,{1,0,0});
                                        log("Damage digits replay sample (0.9.47.33): head in record space %.2f %.2f %.2f | group centre %.2f %.2f %.2f up %.2f %.2f %.2f | right when recorded %.2f %.2f %.2f | right now %.2f %.2f %.2f (view ok %d) | correction factor %.3f, x axis %.2f %.2f %.2f",
                                            headRecord.x,headRecord.y,headRecord.z,g.centre.x,g.centre.y,g.centre.z,g.up.x,g.up.y,g.up.z,g.right.x,g.right.y,g.right.z,view.right.x,view.right.y,view.right.z,view.ok?1:0,correction.factor,cx.x,cx.y,cx.z);
                                    }
                                }
                            }
                            if(!view.ok) {if(e==0) ++gDigitStats.badView;continue;}
                            const Vec3 pt=digitnum::digitPoint(q.digitGroups[dv.group],view,dv.ox,dv.oy);memcpy(sc+size_t(dv.index)*rec.stride,&pt,12);if(e==0) ++gDigitStats.verts;
                        }
                    }
                    rv=spriteCopy.data();
                    static bool notified=false;if(!notified) {notified=true;log("Creature icon facing (0.9.24): rebuilt every VR frame, current central eye transformed through inverse dungeon correction, common plane for both eyes, vertical and UV centre preserved");}
                }
                // 0.9.47.58: thin untextured slivers (selection outline of tiles / rooms) rebuilt as ribbons facing this eye, at least SelectionLineWidth mm wide (0 = off)
                unsigned ribPrims=0;
                if(ribbonMm>0.f&&!menuScreenMode&&!poss&&!rec.hand&&!rec.top&&!rec.overlay&&rec.type==D3DPT_TRIANGLELIST&&(rec.fvf&D3DFVF_POSITION_MASK)==D3DFVF_XYZ
                   &&rec.count>=3&&rec.count<=240&&rec.primitives<=160&&rec.stride>=16&&rec.sprites.empty()&&rec.digitVerts.empty()
                   &&rec.snapshot<q.snapshots.size()&&q.snapshots[rec.snapshot].copy[0]==0) {
                    const Vec3 eyeRec=apply(inverse(correction),r.eyes[e&1].pose.position);
                    const float fac=correction.factor>1e-6f?correction.factor:1.f;
                    unsigned thin=0;
                    ribPrims=ribbon::build(rv,rstride,rec.count,rec.indexed?q.indices.data()+rec.indices:nullptr,rec.indexFormat==D3DFMT_INDEX32,rec.primitives,eyeRec,ribbonMm*.001f/fac,ribbonMm*.001f/fac,ribbonCopy,thin);
                    if(ribPrims) {ribRecs+=thin?1:0;ribTris+=thin;++ribDraws;}
                }
                if(rf!=fvf) {d->SetFVF(rf);fvf=rf;}
                if(ribPrims) {
                    if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,ribPrims,ribbonCopy.data(),rstride))) ++sDrawFailures;
                    if(e==0&&ribLogMs+5000.<milliseconds()) {if(ribLogMs>0) log("Selection ribbons (0.9.47.58, last 5 s): %llu draws rebuilt, %llu thin triangles widened to %.1f mm (SelectionLineWidth), facing each eye",(unsigned long long)ribDraws,(unsigned long long)ribTris,ribbonMm);ribLogMs=milliseconds();ribRecs=ribTris=ribDraws=0;}
                    continue;
                }
                HRESULT hr=rec.indexed?d->DrawIndexedPrimitiveUP(rec.type,0,rec.count,rec.primitives,q.indices.data()+rec.indices,rec.indexFormat,rv,rstride):
                    d->DrawPrimitiveUP(rec.type,rec.primitives,rv,rstride);
                if(FAILED(hr)) ++sDrawFailures;
            }
            if(overlayOn) d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
            };
            if(cubeOn) {   // 0.8.84: the 6 faces replace the normal view (which only had the front)
                // all opaque geometry of the 6 faces first, then the blended (torches, particles): a blended piece near a seam is never covered by the opaque
                // geometry of the next face (each face keeps the game's own order inside its pass)
                // 0.8.99.1 adaptive 360: a face the game did not run this frame shows the normal view inside its pyramid, only if this eye can see that face
                unsigned seen=0;
                if((p.faceMask&63u)!=63u) {
                    float N[6][4][3];for(int k=0;k<6;++k) for(int q4=0;q4<4;++q4) {N[k][q4][0]=p.faceN[k][q4].x;N[k][q4][1]=p.faceN[k][q4].y;N[k][q4][2]=p.faceN[k][q4].z;}
                    const Quat back=conjugate(toEye.rotation);const Fov& f=r.eyes[e].fov;
                    seen=cubeFacesReached(N,f.left,f.right,f.up,f.down,3.f*float(Pi)/180.f,[&](const float* in,float* o){const Vec3 v=rotate(back,Vec3{in[0],in[1],in[2]});o[0]=v.x;o[1]=v.y;o[2]=v.z;});
                }
                for(int b=0;b<2;++b) for(int k=0;k<6;++k) {
                    if(p.faceMask&(1u<<k)) {setFace(k);applied=UINT32_MAX;drawRecs(p,vbase,0,k+1,b);}
                    else if(seen&(1u<<k)) {setFace(k);applied=UINT32_MAX;drawRecs(p,vbase,0,0,b,2);if(e==0&&b==0) ++sFaceFilled;}
                    else if(e==0&&b==0) ++sFaceSkipped;
                }
                d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);applied=UINT32_MAX;
                drawRecs(p,vbase,0,0,-1,1);applied=UINT32_MAX;   // 0.8.99: the attack effects (drawn by the game in its normal view only) over the 6 faces
            }
            for(int pass=0;pass<2;++pass) {
            if(pass==1) {
                if(!handOk) break;
                const float k=p.hudSizes.cursor/p.handExtent;
                const Vec3 target=add(r.rayOrigin,scale(r.rayDirection,.07f));
                Similarity m{{0,0,0,1},1,scale(p.handCenter,-1)};
                m=then(m,Similarity{{0,0,0,1},k,{}});m=then(m,Similarity{{0,0,0,1},1,target});m=then(m,eyeInverse);
                D3DMATRIX hw{};matrixOf(m,&hw._11);d->SetTransform(D3DTS_WORLD,&hw);d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);
            }
            static const bool opaqueFirst=iniInt(L"OpaqueFirst",1)!=0;   // 0.9.36: all opaque geometry first, then the transparent draws in the game's order
            if(!(cubeOn&&pass==0)&&!(menuStencil&&pass==0)) {if(pass==0&&opaqueFirst&&!menu){drawRecs(p,vbase,0,0,0);applied=UINT32_MAX;drawRecs(p,vbase,0,0,1,0,1);applied=UINT32_MAX;drawRecs(p,vbase,0,0,1,0,2);applied=UINT32_MAX;} else drawRecs(p,vbase,pass,0);}   // 0.8.90.3: the menu decor is replayed only inside its window
            }
            if(envOn&&!passthrough) {d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);scene.drawFlames(d,envFrame,r.eyes[e].pose);}
            // Controllers: current LOCAL, current head, no clipping.
            if(!hands.empty()) {
                d->SetTransform(D3DTS_WORLD,&eyeWorld);d->SetPixelShader(nullptr);d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
                for(int i=0;i<Snapshot::Samplers;++i) d->SetTexture(i,nullptr);
                d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
                d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);
                d->SetRenderState(D3DRS_COLORWRITEENABLE,15);
                d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
                if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(hands.size()/3),hands.data(),sizeof(OverlayVertex)))) ++sDrawFailures;
            }
            if(!glow.empty()) {
                d->SetTransform(D3DTS_WORLD,&eyeWorld);d->SetPixelShader(nullptr);d->SetVertexShader(nullptr);d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
                for(int i=0;i<Snapshot::Samplers;++i) d->SetTexture(i,nullptr);
                d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_LIGHTING,FALSE);
                d->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);d->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
                d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
                d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_COLORWRITEENABLE,15);
                d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
                if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(glow.size()/3),glow.data(),sizeof(OverlayVertex)))) ++sDrawFailures;
                d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
            }
            // 0.8.86.3: the options panel is drawn BEFORE the game's interface and never writes depth: the interface (bars, game menu) always covers it
            if(menuStencil&&e==0) {   // 0.8.90.8: UI layer = presented game image where it differs from the decor read back by the game before its 2D interface
                if(menuUiForEpoch!=p.epoch) {menuUiOk=false;menuUiForEpoch=p.epoch;}
                // 0.8.95: the 2D layer separated by the game thread (exact, same frame as the decor): one upload per new layer
                const bool layerOk=p.menuUiLayer&&p.menuUiGen&&p.menuUiW>=320&&p.menuUiH>=240&&p.menuUiLayer->size()==size_t(p.menuUiW)*p.menuUiH;
                if(!layerOk&&menuCpuTex&&menuU==menuCpuTex) {menuUiOk=false;menuU.Reset();}   // no current layer: flat screen (never an old interface)
                if(layerOk&&!menuCpuFailed) {
                    if(!menuCpuTex||menuCpuW!=p.menuUiW||menuCpuH!=p.menuUiH) {
                        menuCpuTex.Reset();menuCpuKey=0;
                        if(SUCCEEDED(dev->CreateTexture(p.menuUiW,p.menuUiH,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&menuCpuTex,nullptr))) {menuCpuW=p.menuUiW;menuCpuH=p.menuUiH;
                            log("VR thread: menu 2D layer %ux%u from the game thread (0.8.95): decor in stereo relief, 2D in front at the wall",menuCpuW,menuCpuH);}
                        else {menuCpuW=menuCpuH=0;menuCpuFailed=true;log("VR thread: menu 2D layer texture %ux%u failed: flat menu screen",p.menuUiW,p.menuUiH);}
                    }
                    if(menuCpuTex&&menuCpuKey!=p.menuUiGen) {
                        D3DLOCKED_RECT lr{};
                        if(SUCCEEDED(menuCpuTex->LockRect(0,&lr,nullptr,D3DLOCK_DISCARD))) {
                            const auto& v=*p.menuUiLayer;
                            for(UINT y=0;y<menuCpuH;++y) memcpy(static_cast<unsigned char*>(lr.pBits)+size_t(y)*size_t(lr.Pitch),v.data()+size_t(y)*menuCpuW,size_t(menuCpuW)*4);
                            menuCpuTex->UnlockRect(0);menuCpuKey=p.menuUiGen;++menuCpuUploads;
                        }
                    }
                    if(menuCpuTex&&menuCpuKey) {menuU=menuCpuTex;menuUiOk=true;}
                }
                IDirect3DTexture9* dt=p.menuUiLayer?nullptr:texture(p.desktop);   // 0.8.95: the 0.8.90.8 GPU comparison runs only without the game thread's layer
                if(!menuShaderTried) {
                    menuShaderTried=true;menuUiDebug=iniInt(L"MenuUiDebug",0)!=0;menuUiThreshold=iniFloat(L"MenuUiThreshold",.03f,.002f,.5f);
                    menuLagFixed=iniInt(L"MenuUiLag",-1);if(menuLagFixed>2) menuLagFixed=-1;if(menuLagFixed>=0) menuLagChosen=menuLagFixed;
                    using Compile=HRESULT(WINAPI*)(LPCVOID,SIZE_T,LPCSTR,const D3D_SHADER_MACRO*,ID3DInclude*,LPCSTR,LPCSTR,UINT,UINT,ID3DBlob**,ID3DBlob**);
                    HMODULE dc=LoadLibraryW(L"d3dcompiler_47.dll");
                    auto compile=dc?reinterpret_cast<Compile>(GetProcAddress(dc,"D3DCompile")):nullptr;
                    const char* src="sampler d:register(s0);sampler t:register(s1);float4 k:register(c0);"
                        "float4 main(float2 a:TEXCOORD0,float2 b:TEXCOORD1):COLOR{float3 c=tex2D(d,a).rgb;float3 e=abs(c-tex2D(t,b).rgb);"
                        "float m=max(e.r,max(e.g,e.b));float al=saturate((m-k.z)*k.w);return float4(c*al,al);}";
                    ComPtr<ID3DBlob> code,err;
                    if(compile&&SUCCEEDED(compile(src,strlen(src),nullptr,nullptr,nullptr,"main","ps_2_0",0,0,&code,&err))&&SUCCEEDED(dev->CreatePixelShader(static_cast<const DWORD*>(code->GetBufferPointer()),&menuUiShader)))
                        log("VR thread: menu interface separation shader ready (0.8.90.8, exact read-back, frame lag %s)",menuLagFixed>=0?"fixed by MenuUiLag":"measured");
                    else log("VR thread: menu interface separation shader unavailable (%s): flat menu screen",dc?"compile failed":"d3dcompiler_47 missing");
                }
                const UINT dw=p.menuDecorW,dh=p.menuDecorH;
                bool anyLag=false;for(int k=0;k<3;++k) anyLag|=p.menuDecorLag[k]&&p.menuDecorLag[k]->size()==size_t(dw)*dh;
                if(dt&&menuUiShader&&anyLag&&dw>=320&&dh>=240&&p.desktopWidth>=dw&&p.desktopHeight>=dh) {
                    if(menuTw!=dw||menuTh!=dh) {
                        for(auto& t:menuLagTex) {t.t.Reset();t.key=nullptr;}
                        for(int k=0;k<3;++k) {menuUk[k].Reset();menuUks[k].Reset();}
                        menuU.Reset();menuUiOk=false;bool okAll=true;
                        for(auto& t:menuLagTex) okAll&=SUCCEEDED(dev->CreateTexture(dw,dh,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&t.t,nullptr));
                        for(int k=0;k<3&&okAll;++k) {okAll&=SUCCEEDED(dev->CreateTexture(dw,dh,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&menuUk[k],nullptr));if(okAll) menuUk[k]->GetSurfaceLevel(0,&menuUks[k]);}
                        for(int k=0;k<3&&okAll;++k) if(!menuQ[k]) okAll&=SUCCEEDED(dev->CreateQuery(D3DQUERYTYPE_OCCLUSION,&menuQ[k]));
                        if(okAll) {menuTw=dw;menuTh=dh;log("VR thread: menu interface layers %ux%u, frame lag candidates 0/1/2 (0.8.90.8)",dw,dh);}
                        else {log("VR thread: menu interface textures %ux%u failed: flat menu screen",dw,dh);for(auto& t:menuLagTex) {t.t.Reset();t.key=nullptr;}for(int k=0;k<3;++k) {menuUk[k].Reset();menuUks[k].Reset();}menuTw=menuTh=0;}
                    }
                    if(menuTw==dw&&menuTh==dh) {
                        // results of the previous measurements (never waits): the lag whose decor matches leaves the fewest "interface" pixels
                        {bool all=true;DWORD res[3]{};
                         for(int k=0;k<3;++k) {
                             if(!menuQBusy[k]) {all=false;continue;}
                             DWORD n=0;const HRESULT q=menuQ[k]->GetData(&n,sizeof(n),0);
                             if(q==S_OK) {menuQBusy[k]=false;menuQDone[k]=true;menuQRes[k]=n;} else if(q!=S_FALSE) {menuQBusy[k]=false;menuQDone[k]=false;}
                         }
                         for(int k=0;k<3;++k) {all&=menuQDone[k]&&menuQGen[k]==menuQGen[0];res[k]=menuQRes[k];}
                         if(all) {
                             int win=0;for(int k=1;k<3;++k) if(res[k]<res[win]) win=k;
                             const double tot=double(dw)*dh;
                             if(res[win]+tot*.002<std::min(std::min(res[(win+1)%3],res[(win+2)%3]),DWORD(~0u))) // a clear difference only (static screens give a tie)
                                 for(int k=0;k<3;++k) menuLagScore[k]=menuLagScore[k]*.95+(k==win?.05:0.);
                             for(int k=0;k<3;++k) {menuUiFrac[k]+=res[k]/tot;menuQDone[k]=false;}
                             ++menuLagSamples;
                             if(menuLagFixed<0) {int best=0;for(int k=1;k<3;++k) if(menuLagScore[k]>menuLagScore[best]) best=k;
                                 if(best!=menuLagChosen&&menuLagScore[best]>menuLagScore[menuLagChosen]+.15) {menuLagChosen=best;log("VR thread: menu interface frame lag now %d (scores %.2f/%.2f/%.2f)",best,menuLagScore[0],menuLagScore[1],menuLagScore[2]);}}
                         }
                         if(milliseconds()-menuLagLog>10000&&menuLagSamples) {
                             log("Menu interface separation (0.8.90.8): lag used %d, scores %.2f/%.2f/%.2f, interface pixels lag0 %.1f%% lag1 %.1f%% lag2 %.1f%% (%llu samples)",menuLagChosen,
                                 menuLagScore[0],menuLagScore[1],menuLagScore[2],100.*menuUiFrac[0]/menuLagSamples,100.*menuUiFrac[1]/menuLagSamples,100.*menuUiFrac[2]/menuLagSamples,(unsigned long long)menuLagSamples);
                             menuLagLog=milliseconds();menuLagSamples=0;for(auto& f:menuUiFrac) f=0;
                         }}
                        // decor textures, one upload per new read-back (a read-back is lag 0, then 1, then 2 in the next packages)
                        IDirect3DTexture9* lagTex[3]{};
                        for(int k=0;k<3;++k) {
                            const auto& v=p.menuDecorLag[k];if(!v||v->size()!=size_t(dw)*dh) continue;
                            for(auto& t:menuLagTex) if(t.key==v.get()) lagTex[k]=t.t.Get();
                            if(lagTex[k]) continue;
                            for(auto& t:menuLagTex) {
                                bool used=false;for(int j=0;j<3;++j) used|=p.menuDecorLag[j]&&t.key==p.menuDecorLag[j].get();
                                if(used) continue;
                                D3DLOCKED_RECT lr{};if(FAILED(t.t->LockRect(0,&lr,nullptr,D3DLOCK_DISCARD))) break;
                                for(UINT y=0;y<dh;++y) memcpy(static_cast<unsigned char*>(lr.pBits)+size_t(y)*size_t(lr.Pitch),v->data()+size_t(y)*dw,size_t(dw)*4);
                                t.t->UnlockRect(0);t.key=v.get();lagTex[k]=t.t.Get();break;
                            }
                        }
                        const float W=float(p.desktopWidth),H=float(p.desktopHeight);
                        const float sH=H/float(dh),cw=float(dw)*sH,cx0=(W-cw)*.5f;
                        ComPtr<IDirect3DSurface9> oldRt,oldDs;d->GetRenderTarget(0,&oldRt);d->GetDepthStencilSurface(&oldDs);
                        d->SetDepthStencilSurface(nullptr);
                        // pixel i of the 640x480 grid samples the presented screen at the centre of its stretched block (exact for a nearest or bilinear stretch)
                        struct DV {float x,y,z,rhw,u0,v0,u1,v1;};
                        const float fw=float(dw),fh=float(dh),u0=cx0/W,u1=(cx0+cw)/W;
                        const DV qv[6]={{-.5f,-.5f,0,1,u0,0,0,0},{fw-.5f,-.5f,0,1,u1,0,1,0},{-.5f,fh-.5f,0,1,u0,1,0,1},
                                         {fw-.5f,-.5f,0,1,u1,0,1,0},{fw-.5f,fh-.5f,0,1,u1,1,1,1},{-.5f,fh-.5f,0,1,u0,1,0,1}};
                        d->SetPixelShader(menuUiShader.Get());d->SetVertexShader(nullptr);d->SetFVF(D3DFVF_XYZRHW|D3DFVF_TEX2);
                        d->SetTexture(0,dt);for(int i=2;i<Snapshot::Samplers;++i) d->SetTexture(i,nullptr);
                        for(int t=0;t<2;++t) {d->SetSamplerState(t,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);d->SetSamplerState(t,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
                            d->SetSamplerState(t,D3DSAMP_MAGFILTER,D3DTEXF_POINT);d->SetSamplerState(t,D3DSAMP_MINFILTER,D3DTEXF_POINT);d->SetSamplerState(t,D3DSAMP_MIPFILTER,D3DTEXF_NONE);
                            d->SetSamplerState(t,D3DSAMP_MIPMAPLODBIAS,0);d->SetSamplerState(t,D3DSAMP_MAXANISOTROPY,1);d->SetSamplerState(t,D3DSAMP_SRGBTEXTURE,FALSE);}
                        const float kc[4]={1.f/fw,1.f/fh,menuUiThreshold,40.f};d->SetPixelShaderConstantF(0,kc,1);
                        d->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
                        d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);
                        d->SetRenderState(D3DRS_FOGENABLE,FALSE);d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_SRGBWRITEENABLE,FALSE);d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);
                        ++menuGen;bool built=false;const bool allIdle=!menuQBusy[0]&&!menuQBusy[1]&&!menuQBusy[2];
                        for(int k=0;k<3;++k) {
                            if(!lagTex[k]) continue;
                            d->SetRenderTarget(0,menuUks[k].Get());
                            D3DVIEWPORT9 tvp{0,0,dw,dh,0.f,1.f};d->SetViewport(&tvp);
                            d->SetTexture(1,lagTex[k]);
                            d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_COLORWRITEENABLE,15);
                            d->Clear(0,nullptr,D3DCLEAR_TARGET,0x00000000,1.f,0);
                            if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,2,qv,sizeof(DV)))) {++sDrawFailures;continue;}
                            if(k==menuLagChosen) built=true;
                            if(menuLagFixed<0&&allIdle) {   // count the "interface" pixels of this candidate (no colour written)
                                d->SetRenderState(D3DRS_COLORWRITEENABLE,0);d->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
                                d->SetRenderState(D3DRS_ALPHAREF,0x80);d->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);
                                menuQ[k]->Issue(D3DISSUE_BEGIN);d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,2,qv,sizeof(DV));menuQ[k]->Issue(D3DISSUE_END);
                                menuQBusy[k]=true;menuQGen[k]=menuGen;menuQDone[k]=false;
                                d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_COLORWRITEENABLE,15);
                            }
                        }
                        if(built) {menuU=menuUk[menuLagChosen];menuUiOk=true;++menuUiBuilt;}
                        d->SetPixelShader(nullptr);for(int i=0;i<Snapshot::Samplers;++i) d->SetTexture(i,nullptr);
                        d->SetRenderTarget(0,oldRt.Get());d->SetDepthStencilSurface(oldDs.Get());
                        D3DVIEWPORT9 evp{0,0,im.w,im.h,0.f,1.f};d->SetViewport(&evp);
                        d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
                        applied=UINT32_MAX;fvf=0;
                    }
                }
                // no decor read-back in this package: the interface layer of the previous package stays (same screen, one frame older)
            }
            const bool reliefOn=menuStencil&&menuUiOk;
            if(menu&&reliefOn&&!splashDone) {splashDone=true;splashTex.Reset();splashRt.Reset();splashSys.Reset();}   // 0.9.47.39: the menu itself is on the wall: start-up picture over
            if(menu&&!reliefOn) drawMenuScreen(d,p,rr,eyeWorld);
            if(reliefOn) {   // 0.8.90: stereo relief of the menu decor inside a window of the front wall, UI flattened at the wall surface
                // 0.9.6 DIAGNOSTIC (MenuReplayCheck=1): every 1.5 s, the package is replayed FLAT (the game's own pre-transformed vertices, game z) into a
                // 640x480 target and compared with the game's decor read-back of the same frame. Same image = the draws, textures and states handed to the
                // headset are right (a blink would come from the relief); different (log 'Menu replay check', images DK2VR_calque_9NN_menu-rejeu*) = what differs.
                static const bool mcOn=iniInt(L"MenuReplayCheck",0)!=0;
                if(mcOn&&!mcFailed&&e==0&&!p.screenVerts.empty()&&p.checkDecorW>0&&p.checkDecorH>0&&p.checkDecor&&p.checkDecor->size()==size_t(p.checkDecorW)*p.checkDecorH&&p.id!=mcLast&&milliseconds()-mcMs>1500) {
                    mcLast=p.id;mcMs=milliseconds();
                    const UINT dw=p.checkDecorW,dh=p.checkDecorH;
                    if(!mcRt||mcW!=dw||mcH!=dh) {
                        mcRt.Reset();mcRts.Reset();mcDs.Reset();mcSys.Reset();mcW=dw;mcH=dh;
                        if(FAILED(d->CreateTexture(dw,dh,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&mcRt,nullptr))||FAILED(mcRt->GetSurfaceLevel(0,&mcRts))||
                           FAILED(d->CreateDepthStencilSurface(dw,dh,D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,TRUE,&mcDs,nullptr))||
                           FAILED(d->CreateOffscreenPlainSurface(dw,dh,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&mcSys,nullptr))) {mcFailed=true;log("Menu replay check (0.9.6): surfaces failed, check off");}
                    }
                    if(!mcFailed) {
                        ComPtr<IDirect3DSurface9> oldRt,oldDs;D3DVIEWPORT9 oldVp{};d->GetRenderTarget(0,&oldRt);d->GetDepthStencilSurface(&oldDs);d->GetViewport(&oldVp);
                        d->SetRenderTarget(0,mcRts.Get());d->SetDepthStencilSurface(mcDs.Get());D3DVIEWPORT9 cvp{0,0,dw,dh,0.f,1.f};d->SetViewport(&cvp);
                        d->Clear(0,nullptr,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,0xff000000,1.f,0);
                        d->SetPixelShader(nullptr);d->SetVertexShader(nullptr);d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);
                        d->SetRenderState(D3DRS_FOGENABLE,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);
                        d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);d->SetRenderState(D3DRS_DEPTHBIAS,0);d->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,0);
                        d->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_ALWAYS);d->SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_KEEP);
                        auto resetCaches=[&]{applied=UINT32_MAX;fvf=0;std::fill(&vTex[0],&vTex[0]+Snapshot::Samplers,false);vShader=false;for(size_t i=0;i<Snapshot::RS;++i) vRS[i]=false;
                            for(int t=0;t<Snapshot::Stages;++t) for(size_t i=0;i<Snapshot::TS;++i) vTS[t][i]=false;for(int t=0;t<Snapshot::Samplers;++t) {for(size_t i=0;i<Snapshot::SS;++i) vSS[t][i]=false;vX[t]=false;}};
                        resetCaches();
                        screenCopy.assign(p.screenVerts.begin(),p.screenVerts.end());
                        menuScreenMode=true;menuStencilForce=false;menuBiasOn=false;menuBiasScale=1.f;
                        drawRecs(p,p.vertices.data(),0,0);
                        menuScreenMode=false;resetCaches();d->SetRenderState(D3DRS_DEPTHBIAS,0);
                        d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetPixelShader(nullptr);for(int i=0;i<Snapshot::Samplers;++i) d->SetTexture(i,nullptr);
                        const bool got=SUCCEEDED(d->GetRenderTargetData(mcRts.Get(),mcSys.Get()));
                        d->SetRenderTarget(0,oldRt.Get());d->SetDepthStencilSurface(oldDs.Get());d->SetViewport(&oldVp);
                        D3DLOCKED_RECT lr{};
                        if(got&&SUCCEEDED(mcSys->LockRect(&lr,nullptr,D3DLOCK_READONLY))) {
                            // the decor of this frame or of one of the 2 previous ones (read-back lag): the closest one is the reference
                            const uint32_t* dec=nullptr;int cnt[16*12]{},tot[16*12]{};size_t diffAll=SIZE_MAX;int lagUsed=-1;
                            for(int lag=0;lag<1;++lag) {   // 0.9.7: the decor of this very frame (CPU 2D path)
                                const auto& v=p.checkDecor;if(!v||v->size()!=size_t(dw)*dh) continue;
                                int c2[16*12]{},t2[16*12]{};size_t all=0;const uint32_t* dl=v->data();
                                for(UINT y=0;y<dh;y+=2) {const uint32_t* row=reinterpret_cast<const uint32_t*>(static_cast<const unsigned char*>(lr.pBits)+size_t(y)*lr.Pitch);
                                    for(UINT x=0;x<dw;x+=2) {const uint32_t a=row[x],b=dl[size_t(y)*dw+x];int m=0;for(int c=0;c<3;++c) m=std::max(m,std::abs(int((a>>(c*8))&255)-int((b>>(c*8))&255)));
                                        const int k=int(y*12/dh)*16+int(x*16/dw);++t2[k];if(m>24) {++c2[k];++all;}}}
                                if(all<diffAll) {diffAll=all;dec=dl;lagUsed=lag;memcpy(cnt,c2,sizeof(cnt));memcpy(tot,t2,sizeof(tot));}
                            }
                            if(lagUsed>=0) ++mcLag[lagUsed];
                            if(!dec) {mcSys->UnlockRect();dec=nullptr;}
                            else {
                            float worst=0;int wk=0;
                            for(int k=0;k<16*12;++k) {const float f=tot[k]?float(cnt[k])/tot[k]:0.f;mcCell[k]+=f;mcCellMax[k]=std::max(mcCellMax[k],f);if(f>worst) {worst=f;wk=k;}}
                            ++mcChecks;mcSum+=double(diffAll)/(double(dw/2)*(dh/2));
                            if(worst>.35f&&mcDumps<3) {   // a big local difference: both images saved (collected by the report)
                                ++mcDumps;std::error_code ec;auto folder=configurationPath().parent_path()/L"Captures";std::filesystem::create_directories(folder,ec);
                                auto bmp=[&](const wchar_t* tag,const uint32_t* px,size_t stride){wchar_t n[96];swprintf(n,96,L"DK2VR_calque_%03d_menu-%ls.bmp",900+mcDumps,tag);
                                    FILE* f=_wfopen((folder/n).c_str(),L"wb");if(!f) return;const uint32_t bytes=dw*dh*4;unsigned char hd[54]{};hd[0]='B';hd[1]='M';uint32_t sz=54+bytes,off=54,dib=40;int32_t hh=-int32_t(dh);uint16_t pl=1,bp=32;
                                    memcpy(hd+2,&sz,4);memcpy(hd+10,&off,4);memcpy(hd+14,&dib,4);memcpy(hd+18,&dw,4);memcpy(hd+22,&hh,4);memcpy(hd+26,&pl,2);memcpy(hd+28,&bp,2);memcpy(hd+34,&bytes,4);
                                    fwrite(hd,1,54,f);std::vector<uint32_t> rowv(dw);for(UINT y=0;y<dh;++y) {for(UINT x=0;x<dw;++x) rowv[x]=px[size_t(y)*stride+x]|0xff000000u;fwrite(rowv.data(),4,dw,f);}fclose(f);};
                                bmp(L"rejeu",static_cast<const uint32_t*>(lr.pBits),size_t(lr.Pitch/4));bmp(L"rejeu-decor",dec,dw);
                                log("Menu replay check (0.9.6): package %llu differs from the game decor (lag %d) in cell %d,%d (%.0f %% of its pixels): images calque %03d saved",(unsigned long long)p.id,lagUsed,wk%16,wk/16,worst*100.f,900+mcDumps);
                            }
                            mcSys->UnlockRect();
                            }
                        }
                        if(milliseconds()-mcLogMs>5000&&mcChecks) {
                            mcLogMs=milliseconds();char b[1400];int n=snprintf(b,sizeof(b),"Menu replay check (0.9.6, %llu checks, decor lag 0/1/2 %llu/%llu/%llu): pixels differing from the game decor %.2f %% on average | cells (x,y of 16x12) mean/max %%:",(unsigned long long)mcChecks,(unsigned long long)mcLag[0],(unsigned long long)mcLag[1],(unsigned long long)mcLag[2],100.*mcSum/mcChecks);
                            int order[16*12];for(int k=0;k<16*12;++k) order[k]=k;std::sort(order,order+16*12,[&](int a,int b2){return mcCellMax[a]>mcCellMax[b2];});
                            for(int j=0;j<8&&n>0&&n<int(sizeof(b))-40;++j) {const int k=order[j];n+=snprintf(b+n,sizeof(b)-n," %d,%d %.0f/%.0f",k%16,k/16,100.f*mcCell[k]/mcChecks,100.f*mcCellMax[k]);}
                            log("%s",b);mcChecks=0;mcSum=0;mcLag[0]=mcLag[1]=mcLag[2]=0;std::fill(mcCell,mcCell+16*12,0.f);std::fill(mcCellMax,mcCellMax+16*12,0.f);
                        }
                    }
                }
                const float Dcam=MenuScreenWidth*p.menuFocal/(2.f*std::max(p.menuCx,1.f));
                const float sc=Dcam/MenuRefDepth;
                const Pose sp=menuScreenPose(rr);
                const Pose camPose{sp.orientation,toWorld(sp,{0.f,0.f,Dcam})};
                // 0.8.99.4: overscan. The decor is enlarged about its point on the wall (reference plane) so that the parallax never leaves an uncovered edge inside the window (the stencil still clips it to the exact window).
                static const float overscan=iniFloat(L"MenuReliefOverscan",1.04f,1.f,1.5f);
                const Similarity ms0{camPose.orientation,sc,camPose.position};
                const Vec3 cWall=apply(ms0,{0.f,0.f,MenuRefDepth});
                const Similarity ms{ms0.rotation,ms0.factor*overscan,add(scale(ms0.translation,overscan),scale(cWall,1.f-overscan))};
                D3DMATRIX mw{};matrixOf(then(ms,eyeInverse),&mw._11);
                const float sh=MenuScreenWidth*std::max(p.menuCy,1.f)/std::max(p.menuCx,1.f);
                std::vector<HudVertex> wv;wv.reserve(6);
                quad(wv,sp,MenuScreenWidth,sh,0.f,0.f,1.f,1.f);
                // 1) window: depth forced to the far plane + stencil 1, no colour
                D3DVIEWPORT9 wvp{0,0,im.w,im.h,1.f,1.f};d->SetViewport(&wvp);
                d->SetTransform(D3DTS_WORLD,&eyeWorld);d->SetPixelShader(nullptr);d->SetVertexShader(nullptr);
                d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
                for(int i=0;i<Snapshot::Samplers;++i) d->SetTexture(i,nullptr);
                d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
                d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_FOGENABLE,FALSE);
                d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
                d->SetRenderState(D3DRS_COLORWRITEENABLE,0);d->SetRenderState(D3DRS_DEPTHBIAS,0);d->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,0);
                d->SetRenderState(D3DRS_STENCILENABLE,TRUE);d->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_ALWAYS);d->SetRenderState(D3DRS_STENCILREF,1);
                d->SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_REPLACE);d->SetRenderState(D3DRS_STENCILFAIL,D3DSTENCILOP_KEEP);d->SetRenderState(D3DRS_STENCILZFAIL,D3DSTENCILOP_KEEP);
                d->SetRenderState(D3DRS_STENCILMASK,255);d->SetRenderState(D3DRS_STENCILWRITEMASK,255);
                d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
                // 0.8.99.7: the window is painted black (the menu's own background) instead of staying transparent: where the decor geometry stops short of the image edge the wall no longer shows through
                d->SetRenderState(D3DRS_TEXTUREFACTOR,0xff000000);d->SetRenderState(D3DRS_COLORWRITEENABLE,15);
                d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
                if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(wv.size()/3),wv.data(),sizeof(HudVertex)))) ++sDrawFailures;
                d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
                // 0.9.10: replay original textures at eye resolution with a separate full-range depth buffer.
                // Composite through the ORIGINAL room stencil afterwards: no screen-image projection.
                static const bool isolatedDepthOn=iniInt(L"MenuIsolatedDepth",1)!=0;
                const bool isolatedDepth=isolatedDepthOn&&menuDepth.begin(d,im.w,im.h);
                if(isolatedDepth) d->SetRenderState(D3DRS_STENCILENABLE,FALSE);
                // 2) replay of the menu draws, clipped by the stencil, normal depth range
                D3DVIEWPORT9 nvp{0,0,im.w,im.h,0.f,1.f};d->SetViewport(&nvp);
                d->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_EQUAL);d->SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_KEEP);d->SetRenderState(D3DRS_STENCILWRITEMASK,0);
                d->SetRenderState(D3DRS_COLORWRITEENABLE,15);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
                d->SetTransform(D3DTS_WORLD,&mw);d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);
                applied=UINT32_MAX;fvf=0;
                std::fill(&vTex[0],&vTex[0]+Snapshot::Samplers,false);vShader=false;
                for(size_t i=0;i<Snapshot::RS;++i) vRS[i]=false;
                for(int t=0;t<Snapshot::Stages;++t) for(size_t i=0;i<Snapshot::TS;++i) vTS[t][i]=false;
                for(int t=0;t<Snapshot::Samplers;++t) for(size_t i=0;i<Snapshot::SS;++i) vSS[t][i]=false;
                for(int t=0;t<Snapshot::Samplers;++t) vX[t]=false;
                // 0.9.5: GAME-ORDERED DEPTH (report 211853: banners at the back of the menu blinking lit / dark). The relief squeezes the far content
                // (banners a few cm in front of their wall), the edge-band tessellation may approximate it by several cm and the painter bias of each
                // draw reached ~3 cm there (near plane 1.5 cm): who hides whom changed from frame to frame. Now each vertex keeps its relief POSITION
                // (same picture, same stereo) but is sent pre-transformed with, as depth, the GAME's own z mapped into a thin slab behind the wall:
                // the depth test inside the window is exactly the game's own (nothing can flip), no painter bias needed. MenuReliefGameDepth=0: old path.
                static const bool gameDepthOn=iniInt(L"MenuReliefGameDepth",1)!=0;
                bool gameDepth=false;float slabZ0=0.f;static int menuGameDepthFallbacks=0;static bool menuGameDepthLogged=false;
                if(gameDepthOn&&!p.screenVerts.empty()) {
                    auto rowMul=[](const float* v4,const float* m,float* o){for(int c=0;c<4;++c) o[c]=v4[0]*m[c]+v4[1]*m[4+c]+v4[2]*m[8+c]+v4[3]*m[12+c];};
                    float mvp[16];for(int rr2=0;rr2<4;++rr2) rowMul(&mw._11+rr2*4,&proj._11,mvp+rr2*4);
                    float wallZ=0.f;   // farthest point of the window (its 4 corners) in device depth: the slab starts behind it
                    for(const HudVertex& hv:wv) {float a[4]={hv.p.x,hv.p.y,hv.p.z,1.f},b[4],c[4];rowMul(a,&eyeWorld._11,b);rowMul(b,&proj._11,c);if(c[3]>1e-4f) wallZ=std::max(wallZ,c[2]/c[3]);}
                    const float z0=isolatedDepth?0.f:wallZ+2e-4f,z1=isolatedDepth?1.f:.99999f;   // 0.9.7: the slab starts right behind the window (more depth precision than the .997 floor of 0.9.5)
                    if(z1-z0>4e-4f) {
                        screenCopy.resize(p.screenVerts.size());bool ok=true;
                        const float W=float(im.w),H=float(im.h);
                        for(const auto& rec:p.records) {
                            const UINT os=rec.stride+4;const size_t need=rec.screenVertices+size_t(rec.count)*os;
                            if(need>p.screenVerts.size()||rec.vertices+size_t(rec.count)*rec.stride>p.vertices.size()) {ok=false;break;}
                            memcpy(screenCopy.data()+rec.screenVertices,p.screenVerts.data()+rec.screenVertices,size_t(rec.count)*os);
                            for(UINT i=0;i<rec.count;++i) {
                                float rp[4]={0,0,0,1.f},c[4],sv[4];memcpy(rp,p.vertices.data()+rec.vertices+size_t(i)*rec.stride,12);
                                unsigned char* o=screenCopy.data()+rec.screenVertices+size_t(i)*os;memcpy(sv,o,16);
                                rowMul(rp,mvp,c);
                                if(!(c[3]>.02f)||!std::isfinite(c[0])||!std::isfinite(c[1])) {ok=false;break;}   // behind / at the eye (head in the wall): old path
                                const float iw=1.f/c[3],X=(c[0]*iw+1.f)*.5f*W,Y=(1.f-c[1]*iw)*.5f*H;
                                if(std::fabs(X)>32768.f||std::fabs(Y)>32768.f) {ok=false;break;}
                                const float g=std::clamp(sv[2],0.f,1.f);   // 0.9.7: the WHOLE game z range (0.9.5 clamped below .7: in the sub-menu close-ups, z down to .23, every near surface landed on one depth -> black / wrong pieces, report 221111)
                                const float out4[4]={X,Y,z0+(z1-z0)*g,iw};memcpy(o,out4,16);
                            }
                            if(!ok) break;
                        }
                        gameDepth=ok;slabZ0=z0;menuBiasScale=z1-z0;
                    }
                    if(!gameDepth&&menuGameDepthFallbacks++<3) log("Menu relief (0.9.5): game-ordered depth not usable this frame (window depth %.6f), previous depth path used",wallZ);
                }
                d->SetRenderState(D3DRS_DEPTHBIAS,0);
                menuStencilForce=!isolatedDepth;menuBiasOn=!gameDepth;menuScreenMode=gameDepth;
                drawRecs(p,p.vertices.data(),0,0);
                menuStencilForce=false;menuBiasOn=false;menuScreenMode=false;d->SetRenderState(D3DRS_DEPTHBIAS,0);
                if(isolatedDepth) {
                    menuDepth.finish(d);
                    applied=UINT32_MAX;fvf=0;std::fill(&vTex[0],&vTex[0]+Snapshot::Samplers,false);vShader=false;
                    for(size_t i=0;i<Snapshot::RS;++i) vRS[i]=false;
                    for(int t=0;t<Snapshot::Stages;++t) for(size_t i=0;i<Snapshot::TS;++i) vTS[t][i]=false;
                    for(int t=0;t<Snapshot::Samplers;++t) {for(size_t i=0;i<Snapshot::SS;++i) vSS[t][i]=false;vX[t]=false;}
                    static bool reported=false;if(!reported) {reported=true;log("Menu isolated depth (0.9.10): %ux%u per eye, original textures, full 0..1 game depth, native depth/fog states, room stencil preserved",im.w,im.h);}
                }
                d->SetRenderState(D3DRS_FOGENABLE,FALSE);
                if(gameDepth&&!isolatedDepth&&!menuGameDepthLogged) {menuGameDepthLogged=true;log("Menu relief (0.9.5): game-ordered depth on (relief positions, game z in the slab %.6f..%.6f behind the window)",slabZ0,.99999f);}
                d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_STENCILWRITEMASK,255);
                d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
                d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
                d->SetRenderState(D3DRS_COLORWRITEENABLE,15);d->SetPixelShader(nullptr);
                for(int i=0;i<Snapshot::Samplers;++i) d->SetTexture(i,nullptr);
                applied=UINT32_MAX;
                for(int i=0;i<Snapshot::Samplers;++i) vTex[i]=false;vShader=false;
                // 3) interface layer at the wall surface, always in front of the decor (no depth test), straight alpha
                {
                    std::vector<HudVertex> uv;uv.reserve(6);quad(uv,sp,MenuScreenWidth,sh,0.f,0.f,1.f,1.f);
                    d->SetTransform(D3DTS_WORLD,&eyeWorld);d->SetPixelShader(nullptr);d->SetVertexShader(nullptr);d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
                    for(int i=1;i<Snapshot::Samplers;++i) d->SetTexture(i,nullptr);
                    d->SetTexture(0,menuU.Get());
                    d->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
                    d->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);d->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
                    d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);   // 0.8.90.7: premultiplied interface layer (0.8.95: the DESTBLEND call was inside the comment, the previous blend mode stayed)
                    d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_LIGHTING,FALSE);
                    d->SetRenderState(D3DRS_COLORWRITEENABLE,15);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_FOGENABLE,FALSE);d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);
                    d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
                    d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
                    d->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);d->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
                    d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);d->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
                    d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
                    d->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);d->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);d->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_NONE);
                    if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(uv.size()/3),uv.data(),sizeof(HudVertex)))) ++sDrawFailures;
                    d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
                    drawMenuCursor(d,p,rr,eyeWorld);
                    if(menuUiDebug) {   // 0.8.90.2 diagnostic: the three layers as thumbnails on the wall under the frame: game image | decor replayed flat | interface mask
                        const float W=float(p.desktopWidth),H=float(p.desktopHeight);
                        float cw=W,cx0=0;if(W>H*1.34f) {cw=std::floor(H*4.f/3.f+.5f);cx0=std::floor((W-cw)*.5f);}
                        IDirect3DTexture9* tex3[3]={texture(p.desktop),menuLagTex[0].t.Get(),menuU.Get()};
                        d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
                        for(int k=0;k<3;++k) {
                            if(!tex3[k]) continue;
                            Pose tp=sp;tp.position=toWorld(sp,{(k-1)*0.75f,-1.20f,0.f});
                            std::vector<HudVertex> tv;tv.reserve(6);
                            if(k==0) quad(tv,tp,.66f,.50f,cx0/W,0.f,(cx0+cw)/W,1.f); else quad(tv,tp,.66f,.50f,0.f,0.f,1.f,1.f);
                            d->SetTexture(0,tex3[k]);
                            d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(tv.size()/3),tv.data(),sizeof(HudVertex));
                        }
                    }
                    d->SetTexture(0,nullptr);
                }
            }
            // 0.8.94.4: hidden only while the game's own menu is open (0.8.94.3 also hid it for 'full page' message states: it blinked, report 230831 captures 1-2)
            if(!poss&&(menu||!p.screenWhole)) drawComfortWall(d,rr,eyeWorld,(hudOn&&!menu)?&hud:nullptr,p.screenWhole||p.panelWhole||p.panelUi,menu);
            if(!poss&&(menu||!p.screenWhole)) drawComfortWall(d,rr,eyeWorld,(hudOn&&!menu)?&hud:nullptr,p.screenWhole||p.panelWhole||p.panelUi,menu,1);   // 0.9.47.49: CONTROLS wall (opposite side)   // 0.9.47.46: in the main-menu room the interface is on the front wall: no HUD panel can hide the side wall   // 0.8.94.24: a game panel (box / page) always hides the wall options behind it   // 0.8.94.1: hidden whenever a game panel is open (laser went through it)   // 0.8.92: hidden while the game's own options menu is open
            if(hudOn) drawHud(d,hud,p,rr,e,eyeWorld);
            if(poss) drawPossessionSkills(d,p,rr,eyeWorld);   // 0.9.47.39
            if(!poss) drawKeyboard(d,rr,eyeWorld,menu);   // 0.9.47.34: VR keyboard, over the interface, before the laser
            if(!laserMesh.empty()) { // 0.8.23b: laser after the HUD, additive, depth-tested (panels write depth)
                d->SetTransform(D3DTS_WORLD,&eyeWorld);d->SetPixelShader(nullptr);d->SetVertexShader(nullptr);d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
                for(int i=0;i<Snapshot::Samplers;++i) d->SetTexture(i,nullptr);
                d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_LIGHTING,FALSE);
                d->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);d->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
                d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
                d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZFUNC,menu?D3DCMP_ALWAYS:D3DCMP_LESSEQUAL);d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_COLORWRITEENABLE,15);   // 0.9.47.48: main-menu room: the depth left by the menu relief hid the laser outside the screen window
                d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
                if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(laserMesh.size()/3),laserMesh.data(),sizeof(OverlayVertex)))) ++sDrawFailures;
                d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
            }
            if(!poss) drawPossessionVignette(d,rr,im.w,im.h,rr.mode==2||(rr.mode==0&&(p.mode==1||p.mode==2))); // 0.8.68: the host compositor owns the possession vignette
            if(FAILED(d->EndScene())) ok=false;
            if(msaa&&FAILED(d->StretchRect(msaaColor.Get(),nullptr,im.target[e].Get(),nullptr,D3DTEXF_NONE))) ok=false;
        }
        HRESULT hr=S_OK;
        // 0.6.1.2: the shared screen texture is written only when shown (panel/menus); the interface texture
        // goes there once every 120 images, for the F11 capture (every image cost ~25 headset fps).
        if(auto* desk=texture(p.desktop?p.desktop:((frames%120)==0?p.hud:0))) {
            ComPtr<IDirect3DSurface9> s;
            if(SUCCEEDED(desk->GetSurfaceLevel(0,&s))) hr=dev->StretchRect(s.Get(),nullptr,im.target[2].Get(),nullptr,D3DTEXF_NONE);
        }
        double renderMs=milliseconds()-started;
        gpuTimer.end();
        if(SUCCEEDED(hr)) hr=query->Issue(D3DISSUE_END);
        double waitStart=milliseconds();
        if(SUCCEEDED(hr)) {
            uint64_t deadline=GetTickCount64()+200;
            // 0.9.25: after 0.5 ms the wait sleeps (1 ms steps) instead of spinning a CPU core for the whole GPU time (~4 ms per image).
            do {hr=query->GetData(nullptr,0,D3DGETDATA_FLUSH);
                if(hr==S_FALSE) {
                    if(vrWaitSleep&&milliseconds()-waitStart>.5) {
                        const double s0=milliseconds();Sleep(1);const double slept=milliseconds()-s0;
                        // a 1 ms sleep that lasts > 3 ms (Windows timer not at 1 ms, e.g. game window hidden): back to the 0.9.24 spin for the session
                        if(slept>3.&&++vrSleepSlow>=3) {vrWaitSleep=false;log("PERF (0.9.25): VR wait sleep too coarse (%.1f ms for 1 ms): spinning wait restored (as 0.9.24)",slept);}
                    } else SwitchToThread();
                }
            } while(hr==S_FALSE&&GetTickCount64()<deadline);
        }
        sWait+=milliseconds()-waitStart;sRender+=renderMs;sWorst=std::max(sWorst,milliseconds()-started);
        {   // 0.9.25 PERF: headset images of the VR thread per scene / A/B configuration
            auto& S=perfab::shared();const double nowMs=milliseconds();
            const double wall=perfLastImageMs>0?std::min(100.,nowMs-perfLastImageMs):0.;perfLastImageMs=nowMs;
            const uint64_t wallUs=uint64_t(wall*1000.),waitUs=uint64_t((nowMs-waitStart)*1000.);
            S.vrWindow.add(1,wallUs,waitUs);
            if(!S.settling.load()) S.vr[std::clamp(S.mode.load(),0,perfab::Modes-1)][std::clamp(S.config.load(),0,perfab::Configs-1)].add(1,wallUs,waitUs);
        }
        if(!ok||hr!=S_OK) {release();log("VR thread: frame failed (scene=%u, GPU %08lX)",ok,(unsigned long)hr);failure();return;}
        consecutiveFailures=0;
        auto& out=ipc->data->slots[slot];
        out.generation=im.generation;out.frame=++sharedFrames;
        for(int i=0;i<3;++i) out.handles[i]=reinterpret_cast<uintptr_t>(im.handles[i]);
        out.width=im.w;out.height=im.h;out.desktopWidth=im.dw;out.desktopHeight=im.dh;
        memcpy(out.eyes,r.eyes,sizeof(out.eyes));out.anchor=r.anchor;
        out.worldDraws=std::max<uint32_t>(p.world,1);out.uiDraws=p.ui;out.rejectedDraws=p.rejected|(menu?SlotMenuFlag:0u)|((!menu&&!poss&&p.screenWhole)?SlotGameMenuFlag:0u)|((!menu&&!poss&&hudFront)?SlotHudFrontFlag:0u)|((p.pedestalSide==2||(p.pedestalSide==0&&(r.pointerFlags&HudLeftHanded)))?SlotPedestalRightFlag:0u);out.cameraMode=p.mode|CameraModeDecoupled;out.possessionTurn=0.f;
        MemoryBarrier();InterlockedExchange(&out.state,2);SetEvent(ipc->ready);
        lastPublishMs=milliseconds();++published;++sFrames;
    }
    void failure() {
        if(++consecutiveFailures>=30&&!failed) {failed=true;active=false;log("VR thread: 30 consecutive failures, decoupled rendering disabled (0.2.9 path resumes)");}
    }
    void report() {
        const int pmNow=probeMode.load();
        if(pmNow==reportMode&&GetTickCount64()-lastReport<5000) return;
        double elapsed=std::max(1.,milliseconds()-sStart);double n=double(std::max<uint64_t>(1,sFrames));
        log("VR GPU true (probe=%d): %.2f ms avg, %.2f ms max over %llu headset frames (disjoint=%llu) -> VR uses %.1f%% of GPU time; capped/skipped requests=%llu",
            reportMode,gpuTimer.count?gpuTimer.total/gpuTimer.count:0.,gpuTimer.maximum,(unsigned long long)gpuTimer.count,(unsigned long long)gpuTimer.disjoint,
            100.*gpuTimer.total/elapsed,(unsigned long long)sCapped);
        gpuTimer.total=gpuTimer.maximum=0;gpuTimer.count=gpuTimer.disjoint=0;sCapped=0;
        log("VR thread (last 5 s, probe=%d): %.1f headset images/s, render CPU %.2f ms, GPU wait %.2f ms, game frames used=%llu (%.1f/s, age %.1f ms), idle=%llu, no free slot=%llu, textures open=%zu (+%llu, %llu failed), draw failures=%llu, pan prediction avg %.1f mm, turn prediction avg %.2f deg; room CPU %.2f ms, worst frame %.1f ms",
            reportMode,1000.*sFrames/elapsed,sRender/n,sWait/n,(unsigned long long)sPackages,1000.*sPackages/elapsed,sPackages?sAge/sPackages:0.,
            (unsigned long long)sIdle,(unsigned long long)sNoSlot,textures.size(),(unsigned long long)sOpened,(unsigned long long)sOpenFailures,
            (unsigned long long)sDrawFailures,1000.*sPan/n,sTurn/n*180/Pi,sEnv/n,sWorst);
        log("Hand cursor in VR (last 5 s): drawn %llu images, no texture %llu, no aim %llu, no size %llu; last pointer flags %08X",(unsigned long long)sCurDraw,(unsigned long long)sCurNoTex,(unsigned long long)sCurNoAim,(unsigned long long)sCurNoSize,(unsigned)lastPointerFlags);
        sCurDraw=sCurNoTex=sCurNoAim=sCurNoSize=0;
        if(interp.enabled) {
            auto& I=interp;
            log("Interpolation (last 5 s): packages=%llu, interpolated=%llu, reset(layout)=%llu, camera-moved=%llu; moving draws=%llu (%llu vertices), snapped=%llu, unmatched=%llu, static=%llu; adopt %.2f ms, update %.3f ms/frame; span %.1f ms",
                (unsigned long long)I.nAdopt,(unsigned long long)I.nActive,(unsigned long long)I.nReset,(unsigned long long)I.nCamera,(unsigned long long)I.nMovRec,(unsigned long long)I.nMovVert,
                (unsigned long long)I.nSnap,(unsigned long long)I.nUnmatched,(unsigned long long)I.nStaticRec,I.nAdopt?I.adoptCostMs/I.nAdopt:0.,I.nUpdates?I.updateCostMs/I.nUpdates:0.,I.delta*I.shrink);
            I.nAdopt=I.nActive=I.nReset=I.nCamera=I.nMovRec=I.nMovVert=I.nSnap=I.nUnmatched=I.nStaticRec=I.nUpdates=0;I.adoptCostMs=I.updateCostMs=0;
        }
        if(sharpReports<4&&!filterSeen.empty()) {
            ++sharpReports;char line[1400];int n2=0;
            n2+=snprintf(line+n2,sizeof(line)-n2,"VR textures: anisotropy x%d, LOD bias %s %.2f; game filters (mag/min/mip/bias: draws):",anisotropy,keepBias?"game":"forced",lodBias);
            for(auto& [k,c]:filterSeen) if(n2<1200) n2+=snprintf(line+n2,sizeof(line)-n2," %u/%u/%u/%+.2f:%llu",k&15,(k>>4)&15,(k>>8)&15,(int((k>>12)&255)-64)/4.,(unsigned long long)c);
            n2+=snprintf(line+n2,sizeof(line)-n2,"; copies (size x levels: n):");
            for(auto& [k,c]:sizeSeen) if(n2<1380) n2+=snprintf(line+n2,sizeof(line)-n2," %ux%u:%llu",k/100,k%100,(unsigned long long)c);
            log("%s",line);
        }
        motionProbe.report();
        possMotion.report();
        if(sAuxFrames||sOverlayDraws) {
            log("Possession attack effects (0.8.99, VR thread, last 5 s): %llu effect pieces drawn in the headset (left eye count)",(unsigned long long)sOverlayDraws);sOverlayDraws=0;
        }
        if(sAuxFrames) {
            log("Possession 360 view (0.8.84 cube map, VR thread, last 5 s): headset frames in possession %llu, with the cube %llu, reference view only %llu | 0.8.99.1 adaptive: faces not run by the game shown with the normal view %llu, not visible (not drawn) %llu (left eye)",
                (unsigned long long)sAuxFrames,(unsigned long long)sAuxDrawn,(unsigned long long)sAuxNoPlace,(unsigned long long)sFaceFilled,(unsigned long long)sFaceSkipped);
            sAuxFrames=sAuxDrawn=sAuxNoPlace=0;sFaceFilled=sFaceSkipped=0;
        }
        if(stickDiag.frames) {
            log("Stick prediction (0.8.66, last 5 s): headset frames on a stick-driven game frame %llu | prediction applied %llu | pending scroll avg %.1f mm, max %.1f mm | pending turn avg %.2f deg, max %.2f deg | game applied %llu scrolls and %llu turns so far",
                (unsigned long long)stickDiag.frames,(unsigned long long)stickDiag.applied,stickDiag.applied?1000.*stickDiag.offSum/stickDiag.applied:0.,1000.*stickDiag.offMax,
                stickDiag.applied?stickDiag.yawSum/stickDiag.applied*180./Pi:0.,stickDiag.yawMax*180./Pi,
                (unsigned long long)StickDrive::scrolls.load(),(unsigned long long)StickDrive::turns.load());
            stickDiag=StickDiag();
        }
        if(panDiag.frames) {
            log("Pan prediction (0.8.65, last 5 s): headset frames while the game pans %llu | prediction applied %llu, grip too old %llu | clamped at the %.0f mm limit %llu frames (%.1f mm lost on average when clamped) | raw offset avg %.1f mm, max %.1f mm | package switches while panning %llu: visible jump avg %.2f mm, max %.1f mm, >5 mm %llu, >10 mm %llu, >20 mm %llu",
                (unsigned long long)panDiag.frames,(unsigned long long)panDiag.applied,(unsigned long long)panDiag.stale,1000.*panLimit,(unsigned long long)panDiag.clamped,
                panDiag.clamped?1000.*panDiag.clampLoss/panDiag.clamped:0.,panDiag.applied?1000.*panDiag.rawSum/panDiag.applied:0.,1000.*panDiag.rawMax,
                (unsigned long long)panDiag.switches,panDiag.switches?1000.*panDiag.jumpSum/panDiag.switches:0.,1000.*panDiag.jumpMax,
                (unsigned long long)panDiag.jump5,(unsigned long long)panDiag.jump10,(unsigned long long)panDiag.jump20);
        }
        if(panDiag.tFrames) {
            log("Turn prediction (0.8.65, last 5 s): headset frames while the game turns %llu | applied %llu, grip too old %llu | raw angle max %.1f deg | package switches while turning %llu: visible jump avg %.2f deg, max %.2f deg, >0.5 deg %llu, >1 deg %llu, >2 deg %llu",
                (unsigned long long)panDiag.tFrames,(unsigned long long)panDiag.tApplied,(unsigned long long)panDiag.tStale,panDiag.tRawMax*180./Pi,(unsigned long long)panDiag.tSwitches,
                panDiag.tSwitches?panDiag.tJumpSum/panDiag.tSwitches:0.,panDiag.tJumpMax,(unsigned long long)panDiag.tJump05,(unsigned long long)panDiag.tJump1,(unsigned long long)panDiag.tJump2);
        }
        {const bool pp=panDiag.prevPan;const Vec3 pf=panDiag.prevFrom;const int ph=panDiag.prevHand;const uint64_t pi=panDiag.prevId;const bool pt=panDiag.prevTurn;const float ptf=panDiag.prevTurnFrom;panDiag=PanDiag{};panDiag.prevPan=pp;panDiag.prevFrom=pf;panDiag.prevHand=ph;panDiag.prevId=pi;panDiag.prevTurn=pt;panDiag.prevTurnFrom=ptf;}
        sFrames=sPackages=sNoSlot=sOpened=sDrawFailures=sIdle=0;sRender=sWait=sAge=sPan=sTurn=sEnv=sWorst=0;sStart=milliseconds();lastReport=GetTickCount64();reportMode=pmNow;
    }
};
