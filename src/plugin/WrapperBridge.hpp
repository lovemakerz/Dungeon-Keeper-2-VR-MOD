#pragma once
// DK2VR alteration: optional bridge loaded only by the dedicated VR launcher.
#include <windows.h>
#include <d3d9.h>
namespace dk2vr_wrapper {
struct Api {
    using Draw = void (__cdecl*)(IDirect3DDevice9*,D3DPRIMITIVETYPE,UINT,const void*,UINT,const void*,D3DFORMAT,UINT,UINT);
    using Present = void (__cdecl*)(IDirect3DDevice9*);
    using Reset = void (__cdecl*)();
    using Adapter = UINT (__cdecl*)(IDirect3D9*,UINT);
    Draw draw=nullptr; Present present=nullptr; Reset reset=nullptr; Adapter adapter=nullptr;
    Api() {
        wchar_t token[80];
        if(!GetEnvironmentVariableW(L"DK2VR_SESSION",token,80)) return;
        wchar_t path[MAX_PATH]; GetModuleFileNameW(nullptr,path,MAX_PATH);
        wchar_t* slash=wcsrchr(path,L'\\'); if(!slash) return;
        wcscpy_s(slash+1,MAX_PATH-(slash+1-path),L"DK2VR.dll");
        HMODULE dll=LoadLibraryW(path); if(!dll) return;
        draw=reinterpret_cast<Draw>(GetProcAddress(dll,"DK2VR_Draw"));
        present=reinterpret_cast<Present>(GetProcAddress(dll,"DK2VR_Present"));
        reset=reinterpret_cast<Reset>(GetProcAddress(dll,"DK2VR_Reset"));
        adapter=reinterpret_cast<Adapter>(GetProcAddress(dll,"DK2VR_Adapter"));
    }
};
inline Api& api() { static Api a; return a; }
inline IDirect3DDevice9* real(IDirect3DDevice9* d) {
    IDirect3DDevice9* p=nullptr;
    // dxwrapper's private query deliberately returns a borrowed pointer.
    return SUCCEEDED(d->QueryInterface(IID_GetRealInterface,reinterpret_cast<void**>(&p)))&&p?p:d;
}
inline HRESULT draw(IDirect3DDevice9* d,D3DPRIMITIVETYPE t,UINT n,const void* v,UINT stride) {
    if(api().draw) api().draw(real(d),t,n,v,stride,nullptr,D3DFMT_INDEX16,0,0);
    return d->DrawPrimitiveUP(t,n,v,stride);
}
inline HRESULT indexed(IDirect3DDevice9* d,D3DPRIMITIVETYPE t,UINT min,UINT num,UINT n,const void* i,D3DFORMAT f,const void* v,UINT stride) {
    if(api().draw) api().draw(real(d),t,n,v,stride,i,f,min,num);
    return d->DrawIndexedPrimitiveUP(t,min,num,n,i,f,v,stride);
}
inline void present(IDirect3DDevice9* d) { if(api().present) api().present(real(d)); }
inline void reset() { if(api().reset) api().reset(); }
inline UINT adapter(IDirect3D9* d,UINT fallback) { return api().adapter?api().adapter(d,fallback):fallback; }
}
