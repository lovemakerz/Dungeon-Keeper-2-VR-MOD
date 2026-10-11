#pragma once
#include <unordered_map>
#include <cstring>
#include <cstdint>

// CPU mirrors for LockRect on DEFAULT surfaces, not just IDirect3DTexture9.
// Caller serializes access. No reference to the original is retained: PrivateData
// identifies a recycled COM address. Unknown formats/targets keep native locking.
class SurfaceShadows {
public:
    using LockFn=HRESULT(__stdcall*)(IDirect3DSurface9*,D3DLOCKED_RECT*,const RECT*,DWORD);
    using UnlockFn=HRESULT(__stdcall*)(IDirect3DSurface9*);
    using UploadFn=HRESULT(__stdcall*)(IDirect3DDevice9*,IDirect3DSurface9*,const RECT*,IDirect3DSurface9*,const POINT*);
    struct Counters {uint64_t locks=0,fills=0,uploads=0,direct=0,failures=0,invalidations=0,stale=0;};
    Counters stats;
private:
    struct Entry {
        Microsoft::WRL::ComPtr<IDirect3DSurface9> cpu;
        void* owner=nullptr;
        UINT w=0,h=0,bpp=0;D3DFORMAT format=D3DFMT_UNKNOWN;
        bool valid=false,locked=false,readOnly=false,full=false;
        RECT rect{};uint64_t used=0;size_t bytes=0;DWORD lockedAt=0;
    };
    std::unordered_map<IDirect3DSurface9*,Entry> entries;
    size_t bytes=0;uint64_t sequence=0;
    static inline const GUID tagId={0xabe741cd,0x3a98,0x49f3,{0x94,0xe7,0x88,0x33,0x04,0x18,0x99,0xab}};
    static constexpr DWORD tagValue=0x53534841;
    static UINT pixelBytes(D3DFORMAT f) {
        switch(f) {
        case D3DFMT_A8R8G8B8:case D3DFMT_X8R8G8B8:case D3DFMT_A8B8G8R8:case D3DFMT_X8B8G8R8: return 4;
        case D3DFMT_R5G6B5:case D3DFMT_X1R5G5B5:case D3DFMT_A1R5G5B5:case D3DFMT_A4R4G4B4:case D3DFMT_A8L8: return 2;
        case D3DFMT_L8:case D3DFMT_A8: return 1;
        default:return 0;
        }
    }
    static bool copyRows(const D3DLOCKED_RECT& src,const D3DLOCKED_RECT& dst,UINT rowBytes,UINT rows) {
        if(!src.pBits||!dst.pBits||src.Pitch<0||dst.Pitch<0||UINT(src.Pitch)<rowBytes||UINT(dst.Pitch)<rowBytes) return false;
        for(UINT y=0;y<rows;++y) std::memcpy(static_cast<unsigned char*>(dst.pBits)+size_t(y)*dst.Pitch,
            static_cast<const unsigned char*>(src.pBits)+size_t(y)*src.Pitch,rowBytes);
        return true;
    }
    void erase(IDirect3DSurface9* s) {auto it=entries.find(s);if(it!=entries.end()){bytes-=it->second.bytes;entries.erase(it);}}
    bool room(size_t n) {
        constexpr size_t MaxBytes=32u*1024*1024;
        if(n>8u*1024*1024)return false;
        while(entries.size()>=128||bytes+n>MaxBytes) {
            auto oldest=entries.end();
            for(auto it=entries.begin();it!=entries.end();++it) if(!it->second.locked&&(oldest==entries.end()||it->second.used<oldest->second.used))oldest=it;
            if(oldest==entries.end())return false;
            bytes-=oldest->second.bytes;entries.erase(oldest);
        }
        return true;
    }
public:
    size_t size() const {return entries.size();}
    // 0.9.47.27: a 'locked' state only counts for the SAME surface object that was locked. A surface released by the game while locked (level load) and a new one
    // created at the same address used to inherit it: every later lock of the new surface answered INVALIDCALL forever (texture never painted again).
    // Stale = the object carries no tag any more (a different object), or the lock has been open for 8 s. The entry is dropped.
    bool isLocked(IDirect3DSurface9* s) {
        auto it=entries.find(s);if(it==entries.end()||!it->second.locked)return false;
        DWORD tag=0,tagSize=sizeof(tag);
        const bool same=SUCCEEDED(s->GetPrivateData(tagId,&tag,&tagSize))&&tagSize==sizeof(tag)&&tag==tagValue;
        if(same&&DWORD(GetTickCount()-it->second.lockedAt)<=8000)return true;
        ++stats.stale;lastStaleSame=same;
        if(it->second.cpu)it->second.cpu->UnlockRect();
        erase(s);return false;
    }
    bool lastStaleSame=false;
    void clear() {entries.clear();bytes=0;}
    void invalidate(IDirect3DSurface9* s) {
        auto it=entries.find(s);if(it!=entries.end()&&it->second.valid){it->second.valid=false;++stats.invalidations;}
    }
    void invalidateTexture(void* texture) {
        for(auto& item:entries) if(item.second.owner==texture&&item.second.valid){item.second.valid=false;++stats.invalidations;}
    }
    // false means untouched: the hook calls the original with the original flags.
    bool lock(IDirect3DSurface9* s,D3DLOCKED_RECT* out,const RECT* rect,DWORD flags,LockFn nativeLock,UnlockFn nativeUnlock) {
        if(!s||!out||!nativeLock||!nativeUnlock)return false;
        D3DSURFACE_DESC desc{};
        if(FAILED(s->GetDesc(&desc))||desc.Pool!=D3DPOOL_DEFAULT||desc.MultiSampleType!=D3DMULTISAMPLE_NONE||
           (desc.Usage&(D3DUSAGE_RENDERTARGET|D3DUSAGE_DEPTHSTENCIL)))return false;
        const UINT bpp=pixelBytes(desc.Format);
        if(!bpp||!desc.Width||!desc.Height)return false;
        constexpr DWORD allowed=D3DLOCK_DISCARD|D3DLOCK_DONOTWAIT|D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY;
        if((flags&~allowed)||(flags&D3DLOCK_DONOTWAIT)||
           ((flags&D3DLOCK_DISCARD)&&(rect||!(desc.Usage&D3DUSAGE_DYNAMIC)||(flags&D3DLOCK_READONLY))))return false;
        RECT area=rect?*rect:RECT{0,0,LONG(desc.Width),LONG(desc.Height)};
        if(area.left<0||area.top<0||area.right<=area.left||area.bottom<=area.top||
           area.right>LONG(desc.Width)||area.bottom>LONG(desc.Height))return false;
        auto it=entries.find(s);
        if(isLocked(s))return false;
        it=entries.find(s);
        DWORD tag=0,tagSize=sizeof(tag);
        bool same=SUCCEEDED(s->GetPrivateData(tagId,&tag,&tagSize))&&tagSize==sizeof(tag)&&tag==tagValue;
        if(it!=entries.end()&&(!same||it->second.w!=desc.Width||it->second.h!=desc.Height||it->second.format!=desc.Format)) {
            erase(s);it=entries.end();
        }
        if(it==entries.end()) {
            const size_t n=size_t(desc.Width)*desc.Height*bpp;
            if(!room(n))return false;
            Microsoft::WRL::ComPtr<IDirect3DDevice9> device;Entry e;
            if(FAILED(s->GetDevice(&device))||FAILED(device->CreateOffscreenPlainSurface(desc.Width,desc.Height,desc.Format,D3DPOOL_SYSTEMMEM,&e.cpu,nullptr))||
               FAILED(s->SetPrivateData(tagId,&tagValue,sizeof(tagValue),0))) {++stats.failures;return false;}
            e.w=desc.Width;e.h=desc.Height;e.bpp=bpp;e.format=desc.Format;e.bytes=n;
            void* owner=nullptr;if(SUCCEEDED(s->GetContainer(__uuidof(IDirect3DBaseTexture9),&owner))&&owner) {e.owner=owner;static_cast<IUnknown*>(owner)->Release();}
            it=entries.emplace(s,std::move(e)).first;bytes+=n;
        }
        Entry& e=it->second;e.used=++sequence;
        if(!e.valid) {
            D3DLOCKED_RECT src{},dst{};
            // Initialization preserves existing pixels, including outside partial writes.
            if(FAILED(nativeLock(s,&src,nullptr,D3DLOCK_READONLY))) {++stats.failures;return false;}
            HRESULT hr=e.cpu->LockRect(&dst,nullptr,0);
            bool copied=SUCCEEDED(hr)&&copyRows(src,dst,e.w*e.bpp,e.h);
            if(SUCCEEDED(hr)&&FAILED(e.cpu->UnlockRect()))copied=false;
            if(FAILED(nativeUnlock(s)))copied=false;
            if(!copied) {++stats.failures;return false;}
            e.valid=true;++stats.fills;
        }
        DWORD safeFlags=flags&~(D3DLOCK_DISCARD|D3DLOCK_NOOVERWRITE|D3DLOCK_DONOTWAIT);
        if(FAILED(e.cpu->LockRect(out,rect,safeFlags))) {++stats.failures;e.valid=false;return false;}
        e.locked=true;e.lockedAt=GetTickCount();e.readOnly=(flags&D3DLOCK_READONLY)!=0;e.full=rect==nullptr;e.rect=area;++stats.locks;
        return true;
    }
    // Upload failures fall back to a native lock/copy; no silent loss of CPU writes.
    bool unlock(IDirect3DSurface9* s,LockFn nativeLock,UnlockFn nativeUnlock,UploadFn upload,HRESULT& result,bool& written) {
        auto it=entries.find(s);if(it==entries.end()||!it->second.locked)return false;
        Entry& e=it->second;e.locked=false;written=!e.readOnly;result=e.cpu->UnlockRect();
        if(FAILED(result)){e.valid=false;++stats.failures;return true;}
        if(!written)return true;
        Microsoft::WRL::ComPtr<IDirect3DDevice9> device;
        POINT point{e.rect.left,e.rect.top};const RECT* rect=e.full?nullptr:&e.rect;
        result=s->GetDevice(&device);
        if(SUCCEEDED(result))result=upload?upload(device.Get(),e.cpu.Get(),rect,s,e.full?nullptr:&point):device->UpdateSurface(e.cpu.Get(),rect,s,e.full?nullptr:&point);
        if(SUCCEEDED(result)){++stats.uploads;return true;}
        ++stats.failures;
        D3DLOCKED_RECT src{},dst{};
        HRESULT hr=e.cpu->LockRect(&src,rect,D3DLOCK_READONLY);
        if(SUCCEEDED(hr)) {
            hr=nativeLock?nativeLock(s,&dst,rect,0):E_FAIL;
            if(SUCCEEDED(hr)) {
                bool copied=copyRows(src,dst,UINT(e.rect.right-e.rect.left)*e.bpp,UINT(e.rect.bottom-e.rect.top));
                hr=nativeUnlock?nativeUnlock(s):E_FAIL;if(!copied)hr=E_FAIL;
            }
            e.cpu->UnlockRect();
        }
        e.valid=false;result=hr;
        return true;
    }
};
