// DK2VR 0.3.0 - decoupled VR rendering (included by Plugin.cpp inside its anonymous namespace).
//
// The game thread (device A, the game's own D3D9Ex device) records each game frame as before, but no
// longer draws the two eyes. It hands a "package" (vertices in record-LOCAL space, draw states,
// texture copies) to a VR thread that owns a second D3D9Ex device (device B) on the same GPU.
// The VR thread redraws the newest package for every headset frame with the newest head pose, table
// pose and hands. Game textures are copied on device A into shared render-target textures that device
// B opens. A copy is refreshed when the game changes its texture (vtable hooks on the unlock/update
// paths) and a copy still possibly read by the VR thread is never overwritten (a new one is used).

std::atomic<uint64_t> sharedFrames{0};      // slot frame numbers: game-thread and VR-thread images share one sequence
std::atomic<uint32_t> sharedGenerations{0}; // slot texture generations (the host reopens handles when it changes)
constexpr uint64_t NoPackage=~0ull;

// ---------------------------------------------------------------------------------------------
// Texture modification tracking. Game textures are D3DPOOL_DEFAULT|DYNAMIC (dxwrapper D3d9to9Ex)
// and are filled through IDirect3DTexture9::LockRect/UnlockRect (profiled at wrapper RVA 0x1213A9),
// possibly through surface locks, UpdateSurface/UpdateTexture, StretchRect, ColorFill or rendering.
// Every one of these paths increments the version of the texture if it is watched.
// Vtable slot patches (one entry per distinct vtable, original kept for the hook).
struct Patch {void** vtable;void* original;};
struct PatchSet {
    Patch items[16]{};volatile LONG count=0;
    void* original(void* self) {
        void** vt=*reinterpret_cast<void***>(self);LONG n=count;
        for(LONG i=0;i<n;++i) if(items[i].vtable==vt) return items[i].original;
        return nullptr;
    }
    bool has(void** vt) {LONG n=count;for(LONG i=0;i<n;++i) if(items[i].vtable==vt) return true;return false;}
    bool install(void* object,int index,void* hook) {
        void** vt=*reinterpret_cast<void***>(object);
        if(has(vt)) return true;
        if(count>=16) return false;
        if(vt[index]==hook) return true;
        DWORD old=0;if(!VirtualProtect(&vt[index],sizeof(void*),PAGE_READWRITE,&old)) return false;
        items[count]={vt,vt[index]};MemoryBarrier();InterlockedIncrement(&count);
        vt[index]=hook;DWORD ignored=0;VirtualProtect(&vt[index],sizeof(void*),old,&ignored);
        return true;
    }
};
// 0.5.3: CPU shadow of the game's textures. dxwrapper (D3d9to9Ex) turns the game's MANAGED textures into
// DEFAULT|DYNAMIC ones; every LockRect without DISCARD then waits until the GPU has finished with the texture
// (lockBuf 0x590AC0 -> wrapper LockRect RVA 0x1213A9: 55-70 % of the game thread in the 0.5.2 profile,
// game ~40 fps even with the VR paused). Each texture gets a SYSTEMMEM twin: the game locks the twin (no wait)
// and on UnlockRect the changes are sent with UpdateTexture (queued on the GPU, no wait). The twin is filled
// once from the texture (one wait) and dropped when the texture is written by another path.
static const GUID ShadowTag={0x6b1d2a31,0x5c1e,0x4b8e,{0x9a,0x21,0x3f,0x44,0x10,0x7d,0xe2,0x55}};
struct TextureShadow {
    ComPtr<IDirect3DTexture9> sys;UINT w=0,h=0;D3DFORMAT format=D3DFMT_UNKNOWN;bool locked=false,readOnly=false,valid=false;
};
struct TextureWatch {
    static inline PatchSet texUnlock,texLock,surfUnlock,updSurface,updTexture,stretch,fill,setTarget;
    static inline std::unordered_map<void*,TextureShadow>* shadows=nullptr;static inline bool shadowOn=true,shadowLogged=false;
    static inline volatile LONG shadowLocks=0,shadowFills=0,shadowFailures=0,directLocks=0;
    static inline CRITICAL_SECTION lock;static inline bool ready=false,hooksOk=true;
    static inline std::unordered_map<void*,uint32_t>* versions=nullptr;
    static inline volatile LONG changes=0;
    static void init() {if(ready) return;InitializeCriticalSection(&lock);versions=new std::unordered_map<void*,uint32_t>();versions->reserve(4096);shadows=new std::unordered_map<void*,TextureShadow>();shadows->reserve(1024);shadowOn=iniInt(L"GameTextureShadow",1)!=0;ready=true;}
    static void touch(void* texture) {
        if(!ready||!texture) return;
        EnterCriticalSection(&lock);
        auto it=versions->find(texture);if(it!=versions->end()) {++it->second;InterlockedIncrement(&changes);}
        LeaveCriticalSection(&lock);
    }
    static void dropShadow(void* texture) {
        if(!ready||!texture) return;
        EnterCriticalSection(&lock);auto it=shadows->find(texture);if(it!=shadows->end()&&!it->second.locked) it->second.valid=false;LeaveCriticalSection(&lock);
    }
    static void touchSurface(IDirect3DSurface9* s) {
        if(!ready||!s) return;
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
    static void clear() {if(!ready) return;EnterCriticalSection(&lock);versions->clear();LeaveCriticalSection(&lock);}
    // Hooks (vtable slots: texture UnlockRect 20; surface UnlockRect 14; device UpdateSurface 30,
    // UpdateTexture 31, StretchRect 34, ColorFill 35, SetRenderTarget 37).
    using LockFn=HRESULT(__stdcall*)(IDirect3DTexture9*,UINT,D3DLOCKED_RECT*,const RECT*,DWORD);
    using UnlockFn=HRESULT(__stdcall*)(IDirect3DTexture9*,UINT);
    // Returns the shadow of a texture that can use one (level 0, single level, DEFAULT|DYNAMIC), filling it
    // from the texture the first time. Called under 'lock'.
    static TextureShadow* shadowFor(IDirect3DTexture9* self) {
        auto it=shadows->find(self);
        D3DSURFACE_DESC desc{};if(FAILED(self->GetLevelDesc(0,&desc))) return nullptr;
        DWORD tag=0,tagSize=sizeof(tag);
        const bool same=SUCCEEDED(self->GetPrivateData(ShadowTag,&tag,&tagSize))&&tag==0x5348444f; // same object as when the twin was made
        if(it!=shadows->end()&&same&&it->second.valid&&it->second.w==desc.Width&&it->second.h==desc.Height&&it->second.format==desc.Format) return &it->second;
        if(it!=shadows->end()&&!same) it->second.valid=false;
        if(self->GetLevelCount()!=1||desc.Pool!=D3DPOOL_DEFAULT||!(desc.Usage&D3DUSAGE_DYNAMIC)) return nullptr;
        TextureShadow& sh=(*shadows)[self];
        if(!sh.sys||sh.w!=desc.Width||sh.h!=desc.Height||sh.format!=desc.Format) {
            sh=TextureShadow{};ComPtr<IDirect3DDevice9> dev;if(FAILED(self->GetDevice(&dev))) return nullptr;
            if(FAILED(dev->CreateTexture(desc.Width,desc.Height,1,0,desc.Format,D3DPOOL_SYSTEMMEM,&sh.sys,nullptr))) {++shadowFailures;shadows->erase(self);return nullptr;}
            sh.w=desc.Width;sh.h=desc.Height;sh.format=desc.Format;
        }
        // Fill from the texture (waits once for the GPU).
        auto lk=reinterpret_cast<LockFn>(texLock.original(self));auto ul=reinterpret_cast<UnlockFn>(texUnlock.original(self));
        D3DLOCKED_RECT src{},dst{};
        if(!lk||!ul||FAILED(lk(self,0,&src,nullptr,D3DLOCK_READONLY))) {++shadowFailures;shadows->erase(self);return nullptr;}
        bool ok=SUCCEEDED(sh.sys->LockRect(0,&dst,nullptr,0));
        if(ok) {
            UINT rowBytes=UINT(std::min(src.Pitch,dst.Pitch));UINT rows=desc.Height;
            if(desc.Format==D3DFMT_DXT1||desc.Format==D3DFMT_DXT3||desc.Format==D3DFMT_DXT5) rows=(desc.Height+3)/4;
            for(UINT y=0;y<rows;++y) memcpy(static_cast<unsigned char*>(dst.pBits)+size_t(y)*dst.Pitch,static_cast<unsigned char*>(src.pBits)+size_t(y)*src.Pitch,rowBytes);
            sh.sys->UnlockRect(0);
        }
        ul(self,0);
        if(!ok) {++shadowFailures;shadows->erase(self);return nullptr;}
        tag=0x5348444f;self->SetPrivateData(ShadowTag,&tag,sizeof(tag),0);
        sh.valid=true;++shadowFills;
        return &sh;
    }
    static HRESULT __stdcall hTexLock(IDirect3DTexture9* self,UINT level,D3DLOCKED_RECT* out,const RECT* rect,DWORD flags) {
        auto f=reinterpret_cast<LockFn>(texLock.original(self));
        if(!f) return E_FAIL;
        if(shadowOn&&level==0&&out) {
            EnterCriticalSection(&lock);
            TextureShadow* sh=shadowFor(self);
            if(sh&&!sh->locked) {
                DWORD sf=flags&~(D3DLOCK_DISCARD|D3DLOCK_NOOVERWRITE|D3DLOCK_DONOTWAIT);
                if(SUCCEEDED(sh->sys->LockRect(0,out,rect,sf))) {
                    sh->locked=true;sh->readOnly=(flags&D3DLOCK_READONLY)!=0;LeaveCriticalSection(&lock);InterlockedIncrement(&shadowLocks);
                    if(!shadowLogged) {shadowLogged=true;log("Game texture shadow: active (locks served from system memory, uploads queued)");}
                    return D3D_OK;
                }
                ++shadowFailures;sh->valid=false;
            }
            LeaveCriticalSection(&lock);
        }
        InterlockedIncrement(&directLocks);
        return f(self,level,out,rect,flags);
    }
    static HRESULT __stdcall hTexUnlock(IDirect3DTexture9* self,UINT level) {
        auto f=reinterpret_cast<UnlockFn>(texUnlock.original(self));
        if(level==0&&ready) {
            EnterCriticalSection(&lock);
            auto it=shadows->find(self);
            if(it!=shadows->end()&&it->second.locked) {
                TextureShadow& sh=it->second;sh.locked=false;
                HRESULT hr=sh.sys->UnlockRect(0);bool readOnly=sh.readOnly;ComPtr<IDirect3DTexture9> sys=sh.sys;
                LeaveCriticalSection(&lock);
                if(!readOnly) {
                    ComPtr<IDirect3DDevice9> dev;
                    // Queued copy of the changed area (dirty region of the system-memory twin).
                    if(SUCCEEDED(self->GetDevice(&dev))) {
                        auto u=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,IDirect3DBaseTexture9*,IDirect3DBaseTexture9*)>(updTexture.original(dev.Get()));
                        HRESULT r2=u?u(dev.Get(),sys.Get(),self):dev->UpdateTexture(sys.Get(),self);
                        if(FAILED(r2)) {InterlockedIncrement(&shadowFailures);dropShadow(self);}
                    }
                    touch(self);
                }
                return hr;
            }
            LeaveCriticalSection(&lock);
        }
        HRESULT hr=f?f(self,level):E_FAIL;dropShadow(self);touch(self);return hr;
    }
    static void reportShadow() {
        if(!ready) return;
        log("Game texture shadow: %s; locks from shadow %ld, direct %ld, fills %ld, failures %ld, textures %zu",shadowOn?"on":"off (GameTextureShadow=0)",
            (long)InterlockedExchange(&shadowLocks,0),(long)InterlockedExchange(&directLocks,0),(long)InterlockedExchange(&shadowFills,0),(long)shadowFailures,shadows->size());
    }
    static HRESULT __stdcall hSurfUnlock(IDirect3DSurface9* self) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DSurface9*)>(surfUnlock.original(self));
        HRESULT hr=f?f(self):E_FAIL;touchSurface(self);return hr;
    }
    static HRESULT __stdcall hUpdSurface(IDirect3DDevice9* d,IDirect3DSurface9* src,const RECT* r,IDirect3DSurface9* dst,const POINT* p) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,IDirect3DSurface9*,const RECT*,IDirect3DSurface9*,const POINT*)>(updSurface.original(d));
        HRESULT hr=f?f(d,src,r,dst,p):E_FAIL;touchSurface(dst);return hr;
    }
    static HRESULT __stdcall hUpdTexture(IDirect3DDevice9* d,IDirect3DBaseTexture9* src,IDirect3DBaseTexture9* dst) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,IDirect3DBaseTexture9*,IDirect3DBaseTexture9*)>(updTexture.original(d));
        HRESULT hr=f?f(d,src,dst):E_FAIL;dropShadow(dst);touch(dst);return hr; // the shadow's own uploads call the original directly
    }
    static HRESULT __stdcall hStretch(IDirect3DDevice9* d,IDirect3DSurface9* src,const RECT* a,IDirect3DSurface9* dst,const RECT* b,D3DTEXTUREFILTERTYPE t) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,IDirect3DSurface9*,const RECT*,IDirect3DSurface9*,const RECT*,D3DTEXTUREFILTERTYPE)>(stretch.original(d));
        HRESULT hr=f?f(d,src,a,dst,b,t):E_FAIL;touchSurface(dst);return hr;
    }
    static HRESULT __stdcall hFill(IDirect3DDevice9* d,IDirect3DSurface9* s,const RECT* r,D3DCOLOR c) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,IDirect3DSurface9*,const RECT*,D3DCOLOR)>(fill.original(d));
        HRESULT hr=f?f(d,s,r,c):E_FAIL;touchSurface(s);return hr;
    }
    static HRESULT __stdcall hSetTarget(IDirect3DDevice9* d,DWORD i,IDirect3DSurface9* s) {
        auto f=reinterpret_cast<HRESULT(__stdcall*)(IDirect3DDevice9*,DWORD,IDirect3DSurface9*)>(setTarget.original(d));
        touchSurface(s);return f?f(d,i,s):E_FAIL;
    }
    static void installDevice(IDirect3DDevice9* d) {
        init();
        bool ok=updSurface.install(d,30,reinterpret_cast<void*>(&hUpdSurface))&&updTexture.install(d,31,reinterpret_cast<void*>(&hUpdTexture))&&
            stretch.install(d,34,reinterpret_cast<void*>(&hStretch))&&fill.install(d,35,reinterpret_cast<void*>(&hFill))&&
            setTarget.install(d,37,reinterpret_cast<void*>(&hSetTarget));
        if(!ok&&hooksOk) {hooksOk=false;log("Texture tracking: device hooks unavailable (textures refreshed periodically only)");}
    }
    static void installTexture(IDirect3DTexture9* t) {
        void** vt=*reinterpret_cast<void***>(t);
        if(!texUnlock.has(vt)) {
            if(!texLock.install(t,19,reinterpret_cast<void*>(&hTexLock))) {if(hooksOk) log("Texture tracking: texture lock hook unavailable");}
            if(!texUnlock.install(t,20,reinterpret_cast<void*>(&hTexUnlock))) {if(hooksOk) log("Texture tracking: texture hook unavailable (periodic refresh only)");hooksOk=false;return;}
            log("Texture tracking: texture class hooked (%u)",unsigned(texUnlock.count));
            ComPtr<IDirect3DSurface9> s;
            if(SUCCEEDED(t->GetSurfaceLevel(0,&s))&&s) surfUnlock.install(s.Get(),14,reinterpret_cast<void*>(&hSurfUnlock));
        }
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
struct Package {
    uint64_t id=0;uint32_t epoch=0;
    std::vector<unsigned char> vertices,indices;std::vector<DrawRecord> records;std::vector<PackageSnapshot> snapshots;
    std::vector<CopyRef> copies;std::vector<ShaderRef> shaders;
    std::vector<OverlayVertex> base;              // record-LOCAL
    bool clip=false,ceiling=false;float planes[5][4]{};         // record-LOCAL (fixed-function clip planes are in world space)
    int pedestalSide=0;bool buttonHover=false;         // 0.6.8: minimap pedestal side (0 auto, 1 left, 2 right), laser on its button
    uint32_t screen=0;                              // 0.6.9: wall screen texture (only what lies between the bars)
    bool screenShown=false;                         // 0.6.7.1: wall screen (options / menus) visible
    bool screenWhole=true;                          // 0.6.9.8: options (whole area, HUD texture) vs messages (screen texture)
    float pickDistance=0;                           // 0.6.7: laser distance to the dungeon geometry (0: none)
    Pose table{};float tableWidth=1;              // table pose/width of the request used by this game frame
    bool panning=false;int panHand=0;Vec3 panFrom{}; // hand position whose native pan produced this frame
    bool turning=false;float turnFrom=0;Vec3 pivot{};
    float tableDepth=0;                                // 0.3.2: depth of the table (metres of the recorded table)  // 0.3.1: hands angle whose native rotation produced this frame
    uint32_t desktop=0;UINT desktopWidth=0,desktopHeight=0;HANDLE desktopHandle=nullptr;
    // 0.6.1: transparent interface texture (HUD modules) and the game cursor sprite.
    uint32_t hud=0;HANDLE hudHandle=nullptr;UINT hudWidth=0,hudHeight=0;float hudTop=0;HudSizes hudSizes{};
    uint32_t held=0;HANDLE heldHandle=nullptr;float heldPx=0,heldPy=0,heldRadius=0; // 0.6.5 things in hand (interface pieces)
    HANDLE keeperHandle=nullptr;int keeperW=0,keeperH=0,keeperHotX=0,keeperHotY=0; // 0.6.4 software cursor image
    uint32_t handCount=0;Vec3 handCenter{};float handExtent=0; // 0.6.3: native 3D hand records
    uint32_t cursor=0;float cursorUv[4]{};float cursorW=0,cursorH=0,cursorHotX=0,cursorHotY=0;
    uint32_t world=0,ui=0,rejected=0,mode=0;
    double recordedMs=0,handedMs=0;
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
// 0.6.8: rusted iron pedestal (colour variations per part) + side button with an arrow on the column.
void buildPedestal(const HudLayout& hud,std::vector<OverlayVertex>& mesh,bool hover=false) {
    const Pose& t=hud.pedestalTop;
    // Plate: the minimap lies on its front face (+Z), 3 cm thick. (0.6.9.5: restored on request, removed in 0.6.9.3.)
    coloredBox(mesh,t,{-hud.plateW*.5f,-hud.plateH*.5f,-.035f},{hud.plateW*.5f,hud.plateH*.5f,-.004f},0xff5a3522,0xff3b2216);
    // Rim of darker iron around the plate.
    coloredBox(mesh,t,{-hud.plateW*.5f-.012f,-hud.plateH*.5f-.012f,-.040f},{hud.plateW*.5f+.012f,hud.plateH*.5f+.012f,-.030f},0xff3e2518,0xff2a170e);
    // Column: vertical, from the floor to under the plate.
    // 0.6.9.6: column top under the back of the plate (it showed through the plate's lower edge).
    Vec3 under=toWorld(t,{0,-hud.plateH*.40f,-.06f});under.y-=.02f;
    float bottom=hud.pedestalFoot.y,topY=under.y;
    if(topY-bottom>.05f) {
        Pose col{yawOnly(t.orientation),{under.x,0,under.z}};
        coloredBox(mesh,col,{-.05f,bottom,-.05f},{.05f,topY,.05f},0xff64391f,0xff4a2a17);
        coloredBox(mesh,col,{-.12f,bottom,-.12f},{.12f,bottom+.035f,.12f},0xff4d2c18,0xff2f1a0f);
        coloredBox(mesh,col,{-.09f,bottom+.035f,-.09f},{.09f,bottom+.06f,.09f},0xff5a3320,0xff3a2114);
        // Rust streaks: thin lighter bands on the column.
        for(float y: {bottom+.25f,bottom+.52f,bottom+.71f}) if(y<topY-.06f) coloredBox(mesh,col,{-.052f,y,-.052f},{.052f,y+.012f,.052f},0xff7a4424,0xff7a4424);
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
    const float limit=r.table.position.y+std::max(dungeonTop,0.f)+.03f;
    for(int h=0;h<2;++h) {
        if(!(r.handFlags&(h?HandRightValid:HandLeftValid))) continue;
        const Vec3 c=toWorld(r.hands[h],Vec3{0,0,-.03f});
        if(c.y>limit) continue;
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
    if(len>0.01f&&len<20.f&&std::isfinite(len)) {
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
struct VrThread {
    // Shared with the game thread.
    std::mutex m;std::shared_ptr<const Package> mailbox;uint64_t inUse=NoPackage;
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
    struct Opened {ComPtr<IDirect3DTexture9> texture;uint64_t lastFrame=0;UINT w=0,levels=0;};
    // 0.4.7: texture sharpness in VR. The table is seen at a grazing angle: trilinear filtering then picks
    // small mip levels (blurry floor). Game samplers using LINEAR minification become ANISOTROPIC (x16);
    // the game's LOD bias is replaced by VrLodBias. Diagnostics: filters and texture sizes seen.
    int anisotropy=16;float lodBias=0;bool keepBias=false;
    std::map<uint32_t,uint64_t> filterSeen;std::map<UINT,uint64_t> sizeSeen;int sharpReports=0;
    std::unordered_map<uint32_t,Opened> textures;std::unordered_map<uint32_t,ComPtr<IDirect3DPixelShader9>> shaders;
    uint32_t cacheEpoch=0;uint64_t frames=0;
    struct Images {ComPtr<IDirect3DTexture9> color[3];ComPtr<IDirect3DSurface9> target[3],depth;HANDLE handles[3]{};UINT w=0,h=0,dw=0,dh=0;uint32_t generation=0;};
    Images images[SlotCount];ComPtr<IDirect3DQuery9> query;
    std::shared_ptr<const Package> current;uint64_t lastPackageId=0;
    Vec3 lastGrip{};double lastGripMs=-1e9;float lastTurn=0;double lastTurnMs=-1e9;double sTurn=0,sEnv=0,sWorst=0;
    // Statistics (VR thread).
    uint64_t sFrames=0,sPackages=0,sNoSlot=0,sOpened=0,sOpenFailures=0,sDrawFailures=0,sIdle=0;double sRender=0,sWait=0,sAge=0,sStart=0,sPan=0;uint64_t lastReport=0;
    uint32_t consecutiveFailures=0;

    static LRESULT CALLBACK windowProc(HWND h,UINT m,WPARAM w,LPARAM l) {return DefWindowProcW(h,m,w,l);}
    // Creates device B on the game's adapter and verifies texture sharing A -> B (level 0 and mip levels).
    bool create(IDirect3DDevice9* a,Ipc* channel) {
        ipc=channel;
        panPrediction=iniInt(L"PanPrediction",1)!=0;panLimit=iniFloat(L"PanPredictionLimit",.05f,0.f,.5f);showHands=iniInt(L"ShowControllers",1)!=0;tablePointerVr=iniInt(L"TablePointer",1)!=0;
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
        std::lock_guard<std::mutex> g(m);mailbox=std::move(p);
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
        log("VR thread started (%lu)",(unsigned long)GetCurrentThreadId());
        try {t->loop();} catch(...) {log("VR thread exception; decoupled rendering stopped");t->failed=true;t->active=false;}
        return 0;
    }
    void releaseCurrent() {
        if(!current) return;
        std::lock_guard<std::mutex> g(m);current.reset();inUse=NoPackage;
    }
    void loop() {
        LONG lastSerial=-1;
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
                if(mailbox&&(!current||mailbox->id>current->id)) {current=mailbox;inUse=current->id;}
            }
            if(!current||!r.valid) {++sIdle;continue;}
            if(current->epoch!=epoch.load()) {++sIdle;continue;}
            renderFrame(r);
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
    void drawHud(IDirect3DDevice9Ex* d,const HudLayout& hud,const Package& p,const Request& r,int eye,const D3DMATRIX& eyeWorld) {
        (void)eye;
        std::vector<HudVertex> v;v.reserve(64);
        const float W=float(p.hudWidth),H=float(p.hudHeight);
        std::vector<HudVertex> sv;
        for(int i=0;i<HudCount;++i) {
            const HudPanel& q=hud.panel[i];if(!q.shown) continue;
            quad((i==HudScreen&&!p.screenWhole)?sv:v,q.pose,q.w,q.h,q.x0/W,q.y0/H,q.x1/W,q.y1/H); // 0.6.9.3: options from the full interface texture; 0.6.9.8: messages from the screen texture
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
        if(!sv.empty()&&p.screen&&texture(p.screen)) {
            d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
            d->SetTexture(0,texture(p.screen));
            if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(sv.size()/3),sv.data(),sizeof(HudVertex)))) ++sDrawFailures;
        }
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
            Vec3 tl=sub(sub(hot,scale(right,cp.cursorHotX*sw)),scale(up,-cp.cursorHotY*sh));
            Vec3 a=tl,b=add(tl,scale(right,sw)),c=sub(b,scale(up,sh)),dd=sub(tl,scale(up,sh));
            const float* uv=cp.cursorUv;
            HudVertex q[4]={{a,0xffffffff,uv[0],uv[1]},{b,0xffffffff,uv[2],uv[1]},{c,0xffffffff,uv[2],uv[3]},{dd,0xffffffff,uv[0],uv[3]}};
            HudVertex tri[6]={q[0],q[1],q[2],q[0],q[2],q[3]};
            // Things held (drawn under the hand): window of the held-things texture centred on the game cursor,
            // same pixel scale, centred on the hotspot. Premultiplied like the interface.
            if(IDirect3DTexture9* ht=texture(p.held); ht&&p.heldRadius>1&&p.hudWidth>0) {
                // 0.6.7: window R to the left, 2.5R to the right (tooltips next to the cursor were cut).
                // 0.6.7.1: same scale reduction as the hand (grabbing hand 118 px -> 67 px), window R left / 2.5R right,
                // R up / 2.2R down (the creature hangs below the hand and was cut).
                const float R=p.heldRadius,W=float(p.hudWidth),H=float(p.hudHeight),m2=mpp*.35f,sL=R*m2,sR=2.5f*R*m2,sU=R*m2,sD=2.2f*R*m2;
                Vec3 c0=add(sub(hot,scale(right,sL)),scale(up,sU));
                Vec3 a2=c0,b2=add(c0,scale(right,sL+sR)),cc2=sub(b2,scale(up,sU+sD)),d2=sub(c0,scale(up,sU+sD));
                float u0=(p.heldPx-R)/W,v0=(p.heldPy-R)/H,u1=(p.heldPx+2.5f*R)/W,v1=(p.heldPy+2.2f*R)/H;
                HudVertex hq[6]={{a2,0xffffffff,u0,v0},{b2,0xffffffff,u1,v0},{cc2,0xffffffff,u1,v1},{a2,0xffffffff,u0,v0},{cc2,0xffffffff,u1,v1},{d2,0xffffffff,u0,v1}};
                d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);d->SetRenderState(D3DRS_ALPHAREF,4);
                d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_BORDER);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_BORDER);d->SetSamplerState(0,D3DSAMP_BORDERCOLOR,0);
                d->SetTexture(0,ht);
                if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,2,hq,sizeof(HudVertex)))) ++sDrawFailures;
                d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
            }
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
        double started=milliseconds();
        if(p.id!=lastPackageId) {openResources(p);lastPackageId=p.id;++sPackages;sAge+=started-p.recordedMs;}
        ++frames;
        // Record-LOCAL -> current LOCAL: newest table pose/width, and the pan still to be done by the game.
        float ratio=p.tableWidth>0?r.tableWidthMeters/p.tableWidth:1.f;
        Vec3 pan{0,0,0};
        // Pan prediction: the game moves its camera by the hand motion at each game frame; the part of the
        // motion made since this game frame is applied here, so the dungeon follows the hand at the headset rate.
        bool gripping=(r.handFlags&(p.panHand?HandRightGrip:HandLeftGrip))&&(r.handFlags&(p.panHand?HandRightValid:HandLeftValid))&&!(r.handFlags&(p.panHand?HandRightTable:HandLeftTable));
        if(gripping) {lastGrip=r.hands[p.panHand].position;lastGripMs=started;}
        if(panPrediction&&p.panning&&lastGripMs+100>=p.recordedMs) {pan=panInTable(sub(lastGrip,p.panFrom),r.table,ratio,panLimit);sPan+=std::sqrt(pan.x*pan.x+pan.z*pan.z);}
        // Turn prediction (two grips): the part of the hands' rotation made since this game frame.
        float yaw=0;
        const uint32_t both=HandLeftGrip|HandRightGrip|HandLeftValid|HandRightValid;
        if((r.handFlags&both)==both&&!(r.handFlags&(HandLeftTable|HandRightTable))) {lastTurn=handsAngle(r.hands[0].position,r.hands[1].position);lastTurnMs=started;}
        if(panPrediction&&p.turning&&lastTurnMs+100>=p.recordedMs) {yaw=std::clamp(wrapRadians(lastTurn-p.turnFrom),-.6f,.6f);sTurn+=std::fabs(yaw);}
        // 0.3.1: the physical table (base, clipping) never follows the pending pan/turn; only the dungeon does.
        Similarity tableFix=tableCorrection(p.table,r.table,ratio,{0,0,0});
        Similarity correction=dungeonCorrection(p.table,r.table,ratio,pan,yaw,p.pivot);
        const float depthRatio=p.tableWidth>0?p.tableDepth/p.tableWidth:.75f;
        // 0.6.1: HUD modules (layout shared with the game thread), laser length from this frame's hit.
        HudLayout hud;const bool hudOn=p.hud&&texture(p.hud)&&p.hudWidth>=64;
        if(hudOn) hud=layoutHud(r,p.hudSizes,tableAspect>0?tableAspect:depthRatio,p.hudTop,float(p.hudWidth),float(p.hudHeight),p.screenShown,p.pedestalSide,p.screenWhole);
        Request rr=r;
        bool buttonLaser=false;
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
                if(p.pickDistance>0) {rr.rayLength=p.pickDistance;rr.pointerFlags|=PointerActive;}
                else if(const Vec3 up=rotate(r.table.orientation,Vec3{0,1,0});true) {const float den=dot(up,r.rayDirection);
                if(tablePointerVr&&std::fabs(den)>1e-4f&&p.tableWidth>0) {
                    const float tt=dot(up,sub(r.table.position,r.rayOrigin))/den;
                    const Vec3 q=toLocal(r.table,add(r.rayOrigin,scale(r.rayDirection,tt)));
                    const float hw=r.tableWidthMeters*.5f,hd=p.tableDepth*r.tableWidthMeters/p.tableWidth*.5f;
                    if(tt>.02f&&tt<6.f&&std::fabs(q.x)<=hw&&std::fabs(q.z)<=hd) {rr.rayLength=tt;rr.pointerFlags|=PointerActive;}
                }}
            }
        } else rr.rayLength=0;
        std::vector<OverlayVertex> hands;hands.reserve(400);if(showHands) buildHands(rr,hands,false);
        std::vector<OverlayVertex> glow;if(showHands) buildGlow(r,p.hudTop,glow);
        std::vector<OverlayVertex> pedestalMesh;if(hudOn&&hud.pedestal) buildPedestal(hud,pedestalMesh,p.buttonHover);
        env::Scene::FrameState envFrame;
        bool envOn=environment&&!scene.failed&&(scene.ready||scene.init(dev.Get()))&&scene.prepare(dev.Get(),r,tableAspect,depthRatio,envFrame);
        if(!envOn) buildHandles(r,depthRatio,hands); // 0.3.2 handles when the room is off
        D3DMATRIX identity{};identity._11=identity._22=identity._33=identity._44=1;
        bool ok=true;
        for(int e=0;e<2;++e) {
            IDirect3DDevice9Ex* d=dev.Get();
            const bool msaa=samples!=D3DMULTISAMPLE_NONE&&msaaColor&&msaaW==im.w&&msaaH==im.h;
            d->SetRenderTarget(0,msaa?msaaColor.Get():im.target[e].Get());d->SetDepthStencilSurface(msaa?msaaDepth.Get():im.depth.Get());
            D3DVIEWPORT9 vp{0,0,im.w,im.h,0,1};d->SetViewport(&vp);
            d->Clear(0,nullptr,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,0xff080b10,1,0);
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
            if(!pedestalMesh.empty()) {
                d->SetTransform(D3DTS_WORLD,&eyeWorld);d->SetPixelShader(nullptr);d->SetVertexShader(nullptr);d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
                for(int i=0;i<Snapshot::Samplers;++i) d->SetTexture(i,nullptr);
                d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
                d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);
                d->SetRenderState(D3DRS_COLORWRITEENABLE,15);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_LIGHTING,FALSE);
                d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
                d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
                if(FAILED(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(pedestalMesh.size()/3),pedestalMesh.data(),sizeof(OverlayVertex)))) ++sDrawFailures;
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
            for(int pass=0;pass<2;++pass) {
            if(pass==1) {
                if(!handOk) break;
                const float k=p.hudSizes.cursor/p.handExtent;
                const Vec3 target=add(r.rayOrigin,scale(r.rayDirection,.07f));
                Similarity m{{0,0,0,1},1,scale(p.handCenter,-1)};
                m=then(m,Similarity{{0,0,0,1},k,{}});m=then(m,Similarity{{0,0,0,1},1,target});m=then(m,eyeInverse);
                D3DMATRIX hw{};matrixOf(m,&hw._11);d->SetTransform(D3DTS_WORLD,&hw);d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);
            }
            for(auto& rec:p.records) {
                if(rec.hand!=(pass==1)) continue;
                if(rec.snapshot!=applied) {
                    const auto& s=p.snapshots[rec.snapshot];
                    for(int i=0;i<Snapshot::Samplers;++i) if(!vTex[i]||cTex[i]!=s.copy[i]) {d->SetTexture(i,texture(s.copy[i]));cTex[i]=s.copy[i];vTex[i]=true;}
                    if(!vShader||cShader!=s.shader) {
                        ComPtr<IDirect3DPixelShader9> ps;if(s.shader) {auto it=shaders.find(s.shader);if(it!=shaders.end()) ps=it->second;}
                        d->SetPixelShader(ps.Get());cShader=s.shader;vShader=true;
                    }
                    for(size_t i=0;i<Snapshot::RS;++i) if(!vRS[i]||cRS[i]!=s.rs[i]) {d->SetRenderState(Snapshot::renderStates[i],s.rs[i]);cRS[i]=s.rs[i];vRS[i]=true;}
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
                if(rec.fvf!=fvf) {d->SetFVF(rec.fvf);fvf=rec.fvf;}
                HRESULT hr=rec.indexed?d->DrawIndexedPrimitiveUP(rec.type,0,rec.count,rec.primitives,p.indices.data()+rec.indices,rec.indexFormat,p.vertices.data()+rec.vertices,rec.stride):
                    d->DrawPrimitiveUP(rec.type,rec.primitives,p.vertices.data()+rec.vertices,rec.stride);
                if(FAILED(hr)) ++sDrawFailures;
            }
            }
            if(envOn) {d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);scene.drawFlames(d,envFrame,r.eyes[e].pose);}
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
            if(hudOn) drawHud(d,hud,p,rr,e,eyeWorld);
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
        if(SUCCEEDED(hr)) hr=query->Issue(D3DISSUE_END);
        double waitStart=milliseconds();
        if(SUCCEEDED(hr)) {
            uint64_t deadline=GetTickCount64()+200;
            do {hr=query->GetData(nullptr,0,D3DGETDATA_FLUSH);if(hr==S_FALSE) SwitchToThread();} while(hr==S_FALSE&&GetTickCount64()<deadline);
        }
        sWait+=milliseconds()-waitStart;sRender+=renderMs;sWorst=std::max(sWorst,milliseconds()-started);
        if(!ok||hr!=S_OK) {release();log("VR thread: frame failed (scene=%u, GPU %08lX)",ok,(unsigned long)hr);failure();return;}
        consecutiveFailures=0;
        auto& out=ipc->data->slots[slot];
        out.generation=im.generation;out.frame=++sharedFrames;
        for(int i=0;i<3;++i) out.handles[i]=reinterpret_cast<uintptr_t>(im.handles[i]);
        out.width=im.w;out.height=im.h;out.desktopWidth=im.dw;out.desktopHeight=im.dh;
        memcpy(out.eyes,r.eyes,sizeof(out.eyes));out.anchor=r.anchor;
        out.worldDraws=std::max<uint32_t>(p.world,1);out.uiDraws=p.ui;out.rejectedDraws=p.rejected;out.cameraMode=p.mode|CameraModeDecoupled;
        MemoryBarrier();InterlockedExchange(&out.state,2);SetEvent(ipc->ready);
        lastPublishMs=milliseconds();++published;++sFrames;
    }
    void failure() {
        if(++consecutiveFailures>=30&&!failed) {failed=true;active=false;log("VR thread: 30 consecutive failures, decoupled rendering disabled (0.2.9 path resumes)");}
    }
    void report() {
        if(GetTickCount64()-lastReport<5000) return;
        double elapsed=std::max(1.,milliseconds()-sStart);double n=double(std::max<uint64_t>(1,sFrames));
        log("VR thread (last 5 s): %.1f headset images/s, render CPU %.2f ms, GPU wait %.2f ms, game frames used=%llu (%.1f/s, age %.1f ms), idle=%llu, no free slot=%llu, textures open=%zu (+%llu, %llu failed), draw failures=%llu, pan prediction avg %.1f mm, turn prediction avg %.2f deg; room CPU %.2f ms, worst frame %.1f ms",
            1000.*sFrames/elapsed,sRender/n,sWait/n,(unsigned long long)sPackages,1000.*sPackages/elapsed,sPackages?sAge/sPackages:0.,
            (unsigned long long)sIdle,(unsigned long long)sNoSlot,textures.size(),(unsigned long long)sOpened,(unsigned long long)sOpenFailures,
            (unsigned long long)sDrawFailures,1000.*sPan/n,sTurn/n*180/Pi,sEnv/n,sWorst);
        if(sharpReports<4&&!filterSeen.empty()) {
            ++sharpReports;char line[1400];int n2=0;
            n2+=snprintf(line+n2,sizeof(line)-n2,"VR textures: anisotropy x%d, LOD bias %s %.2f; game filters (mag/min/mip/bias: draws):",anisotropy,keepBias?"game":"forced",lodBias);
            for(auto& [k,c]:filterSeen) if(n2<1200) n2+=snprintf(line+n2,sizeof(line)-n2," %u/%u/%u/%+.2f:%llu",k&15,(k>>4)&15,(k>>8)&15,(int((k>>12)&255)-64)/4.,(unsigned long long)c);
            n2+=snprintf(line+n2,sizeof(line)-n2,"; copies (size x levels: n):");
            for(auto& [k,c]:sizeSeen) if(n2<1380) n2+=snprintf(line+n2,sizeof(line)-n2," %ux%u:%llu",k/100,k%100,(unsigned long long)c);
            log("%s",line);
        }
        sFrames=sPackages=sNoSlot=sOpened=sDrawFailures=sIdle=0;sRender=sWait=sAge=sPan=sTurn=sEnv=sWorst=0;sStart=milliseconds();lastReport=GetTickCount64();
    }
};
