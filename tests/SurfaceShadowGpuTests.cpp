#include <windows.h>
#include <d3d9.h>
#include <wrl/client.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "../src/plugin/SurfaceShadow.hpp"
using Microsoft::WRL::ComPtr;
static SurfaceShadows mirrors;
struct Hooks {void** vt;SurfaceShadows::LockFn lock;SurfaceShadows::UnlockFn unlock;};
static std::vector<Hooks> hooks;
static unsigned hookLocks=0,hookUnlocks=0;
void require(bool condition,const char* name){if(!condition){printf("FAIL %s\n",name);exit(1);}}
void check(HRESULT hr,const char* name){if(FAILED(hr)){printf("FAIL %s %08lX\n",name,(unsigned long)hr);exit(1);}}
Hooks original(IDirect3DSurface9* s){void** vt=*reinterpret_cast<void***>(s);for(auto h:hooks)if(h.vt==vt)return h;puts("FAIL unknown surface class");exit(1);}
HRESULT __stdcall realUpload(IDirect3DDevice9* d,IDirect3DSurface9* src,const RECT* r,IDirect3DSurface9* dst,const POINT* p){return d->UpdateSurface(src,r,dst,p);}
HRESULT __stdcall rejectedUpload(IDirect3DDevice9*,IDirect3DSurface9*,const RECT*,IDirect3DSurface9*,const POINT*){return D3DERR_INVALIDCALL;}
HRESULT __stdcall hookedLock(IDirect3DSurface9* s,D3DLOCKED_RECT* out,const RECT* rect,DWORD flags){
    ++hookLocks;auto h=original(s);if(mirrors.isLocked(s))return D3DERR_INVALIDCALL;if(mirrors.lock(s,out,rect,flags,h.lock,h.unlock))return D3D_OK;return h.lock(s,out,rect,flags);
}
HRESULT __stdcall hookedUnlock(IDirect3DSurface9* s){
    ++hookUnlocks;auto h=original(s);HRESULT hr=D3D_OK;bool written=false;
    if(mirrors.unlock(s,h.lock,h.unlock,realUpload,hr,written))return hr;
    hr=h.unlock(s);mirrors.invalidate(s);return hr;
}
void patch(void** slot,void* value){DWORD old=0;require(VirtualProtect(slot,sizeof(void*),PAGE_READWRITE,&old)!=FALSE,"vtable writable");*slot=value;DWORD ignored;VirtualProtect(slot,sizeof(void*),old,&ignored);}
void install(IDirect3DSurface9* s){
    void** vt=*reinterpret_cast<void***>(s);for(auto h:hooks)if(h.vt==vt)return;
    hooks.push_back({vt,reinterpret_cast<SurfaceShadows::LockFn>(vt[13]),reinterpret_cast<SurfaceShadows::UnlockFn>(vt[14])});
    patch(vt+13,reinterpret_cast<void*>(hookedLock));patch(vt+14,reinterpret_cast<void*>(hookedUnlock));
}
std::vector<DWORD> readGpu(IDirect3DSurface9* s){
    D3DSURFACE_DESC desc{};check(s->GetDesc(&desc),"desc");auto h=original(s);D3DLOCKED_RECT r{};
    check(h.lock(s,&r,nullptr,D3DLOCK_READONLY),"native verification lock");
    std::vector<DWORD> result(desc.Width*desc.Height);
    for(UINT y=0;y<desc.Height;++y)std::memcpy(result.data()+y*desc.Width,static_cast<unsigned char*>(r.pBits)+y*r.Pitch,desc.Width*4);
    check(h.unlock(s),"native verification unlock");return result;
}
void writeRect(const D3DLOCKED_RECT& r,UINT w,UINT h,DWORD color){for(UINT y=0;y<h;++y)for(UINT x=0;x<w;++x)reinterpret_cast<DWORD*>(static_cast<unsigned char*>(r.pBits)+y*r.Pitch)[x]=color;}
void initNative(IDirect3DSurface9* s,UINT w,UINT h,DWORD color){auto f=original(s);D3DLOCKED_RECT r{};check(f.lock(s,&r,nullptr,0),"initialize native");writeRect(r,w,h,color);check(f.unlock(s),"initialize unlock");}
void exercise(IDirect3DSurface9* s,IDirect3DDevice9* d,IDirect3DBaseTexture9* owner,const char* label){
    const UINT w=16,h=12;D3DLOCKED_RECT r{};initNative(s,w,h,0xff183247);
    auto fills=mirrors.stats.fills,uploads=mirrors.stats.uploads;
    for(int i=0;i<100;++i){
        check(s->LockRect(&r,nullptr,D3DLOCK_READONLY),"mirrored read");require(*static_cast<DWORD*>(r.pBits)==0xff183247,"initial pixels retained");check(s->UnlockRect(),"read unlock");
    }
    require(mirrors.stats.fills==fills+1,"one initialization for repeated reads");require(mirrors.stats.uploads==uploads,"read-only does not upload");
    check(s->LockRect(&r,nullptr,0),"whole write");writeRect(r,w,h,0xff568923);check(s->UnlockRect(),"whole upload");
    auto expected=std::vector<DWORD>(w*h,0xff568923);require(readGpu(s)==expected,"whole GPU pixels match");
    RECT rect{3,2,9,7};check(s->LockRect(&r,&rect,0),"partial write");writeRect(r,6,5,0xffbd237f);check(s->UnlockRect(),"partial upload");
    for(int y=2;y<7;++y)for(int x=3;x<9;++x)expected[y*w+x]=0xffbd237f;
    require(readGpu(s)==expected,"partial update preserves outside pixels");
    // Simulates UpdateSurface/UpdateTexture/StretchRect/ColorFill invalidation.
    ComPtr<IDirect3DSurface9> source;check(d->CreateOffscreenPlainSurface(w,h,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&source,nullptr),"external source");install(source.Get());
    check(source->LockRect(&r,nullptr,0),"external source lock");writeRect(r,w,h,0xff475869);check(source->UnlockRect(),"external source unlock");
    check(d->UpdateSurface(source.Get(),nullptr,s,nullptr),"external upload");
    if(owner)mirrors.invalidateTexture(owner);else mirrors.invalidate(s);
    check(s->LockRect(&r,nullptr,D3DLOCK_READONLY),"refill after external write");require(*static_cast<DWORD*>(r.pBits)==0xff475869,"external change visible");check(s->UnlockRect(),"refill unlock");
    // Original GPU upload rejected: fallback copy must still preserve the write.
    auto f=original(s);require(mirrors.lock(s,&r,&rect,0,f.lock,f.unlock),"fallback write shadow");writeRect(r,6,5,0xffeecdab);
    HRESULT result;bool written=false;require(mirrors.unlock(s,f.lock,f.unlock,rejectedUpload,result,written),"fallback handled");check(result,"native fallback copy");require(written,"fallback write flag");
    expected.assign(w*h,0xff475869);for(int y=2;y<7;++y)for(int x=3;x<9;++x)expected[y*w+x]=0xffeecdab;
    require(readGpu(s)==expected,"failed-upload fallback GPU pixels match");
    // An address is not identity: erase private tag and change pixels without invalidating the map.
    const GUID tag={0xabe741cd,0x3a98,0x49f3,{0x94,0xe7,0x88,0x33,0x04,0x18,0x99,0xab}};
    check(s->FreePrivateData(tag),"remove identity");initNative(s,w,h,0xff123456);
    check(s->LockRect(&r,nullptr,D3DLOCK_READONLY),"identity refill");require(*static_cast<DWORD*>(r.pBits)==0xff123456,"new identity not stale");check(s->UnlockRect(),"identity unlock");
    RECT invalid{-1,0,5,5};require(!mirrors.lock(s,&r,&invalid,0,f.lock,f.unlock),"invalid rectangle native fallback");
    require(!mirrors.lock(s,&r,nullptr,D3DLOCK_DONOTWAIT,f.lock,f.unlock),"nonblocking flags preserved by native fallback");
    check(s->LockRect(&r,nullptr,D3DLOCK_READONLY),"nested outer lock");D3DLOCKED_RECT other{};
    require(s->LockRect(&other,nullptr,D3DLOCK_READONLY)==D3DERR_INVALIDCALL,"nested lock rejected");check(s->UnlockRect(),"nested outer unlock");
    printf("PASS %s: repeated reads, whole/partial writes, invalidation, identity, failed-upload fallback\n",label);
}
int main(){
    HWND win=CreateWindowExW(0,L"STATIC",L"DK2VR hidden surface verification",0,0,0,64,64,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);require(win!=nullptr,"hidden window");
    ComPtr<IDirect3D9Ex> api;check(Direct3DCreate9Ex(D3D_SDK_VERSION,&api),"D3D9Ex");
    D3DPRESENT_PARAMETERS pp{};pp.Windowed=TRUE;pp.SwapEffect=D3DSWAPEFFECT_DISCARD;pp.BackBufferWidth=pp.BackBufferHeight=64;pp.BackBufferFormat=D3DFMT_X8R8G8B8;pp.hDeviceWindow=win;
    ComPtr<IDirect3DDevice9Ex> d;check(api->CreateDeviceEx(0,D3DDEVTYPE_HAL,win,D3DCREATE_HARDWARE_VERTEXPROCESSING,&pp,nullptr,&d),"hardware device");
    ComPtr<IDirect3DSurface9> plain,system,target;ComPtr<IDirect3DTexture9> texture;ComPtr<IDirect3DSurface9> level;
    check(d->CreateOffscreenPlainSurface(16,12,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&plain,nullptr),"DEFAULT offscreen");install(plain.Get());
    check(d->CreateOffscreenPlainSurface(16,12,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&system,nullptr),"SYSTEMMEM");install(system.Get());
    check(d->CreateTexture(16,12,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&texture,nullptr),"dynamic texture");check(texture->GetSurfaceLevel(0,&level),"texture surface");install(level.Get());
    check(d->CreateRenderTarget(16,12,D3DFMT_A8R8G8B8,D3DMULTISAMPLE_NONE,0,TRUE,&target,nullptr),"render target");install(target.Get());
    exercise(plain.Get(),d.Get(),nullptr,"DEFAULT offscreen surface");exercise(level.Get(),d.Get(),texture.Get(),"dynamic texture surface");
    D3DLOCKED_RECT r{};auto f=original(system.Get());require(!mirrors.lock(system.Get(),&r,nullptr,0,f.lock,f.unlock),"SYSTEMMEM untouched");
    f=original(target.Get());require(!mirrors.lock(target.Get(),&r,nullptr,0,f.lock,f.unlock),"render target untouched");
    // Bound both resource count and cache lifetime; all surfaces remain valid COM objects.
    std::vector<ComPtr<IDirect3DSurface9>> many;
    for(int i=0;i<140;++i){ComPtr<IDirect3DSurface9> s;check(d->CreateOffscreenPlainSurface(4,4,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&s,nullptr),"cache bound surface");install(s.Get());initNative(s.Get(),4,4,0xff112233);check(s->LockRect(&r,nullptr,D3DLOCK_READONLY),"cache bound lock");check(s->UnlockRect(),"cache bound unlock");many.push_back(s);}
    require(mirrors.size()<=128,"bounded cache");require(hookLocks>400&&hookUnlocks>400,"real vtable hooks and CPU reentry exercised");
    mirrors.clear();require(mirrors.size()==0,"reset releases mirrors");
    for(auto h:hooks){patch(h.vt+13,reinterpret_cast<void*>(h.lock));patch(h.vt+14,reinterpret_cast<void*>(h.unlock));}
    DestroyWindow(win);puts("PASS hardware D3D9Ex surface hooks 13/14, CPU reentry, bounded cache/reset. No in-game FPS or headset claim.");return 0;
}
