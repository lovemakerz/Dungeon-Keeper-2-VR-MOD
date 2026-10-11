// made by LoVeMaKeRz
#include <windows.h>
#include <d3d9.h>
#include <wrl/client.h>
#include <cassert>
#include <cstdio>
#include "../src/plugin/MenuChains.hpp"
using Microsoft::WRL::ComPtr;
struct V {float x,y,z,rhw;DWORD color;float u,v;};
int main(){
    HWND win=CreateWindowExW(0,L"STATIC",L"DK2VR hidden chain check",0,0,0,64,64,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
    ComPtr<IDirect3D9Ex> api;assert(SUCCEEDED(Direct3DCreate9Ex(D3D_SDK_VERSION,&api)));
    D3DPRESENT_PARAMETERS pp{};pp.Windowed=TRUE;pp.SwapEffect=D3DSWAPEFFECT_DISCARD;pp.BackBufferWidth=pp.BackBufferHeight=64;
    pp.BackBufferFormat=D3DFMT_X8R8G8B8;pp.hDeviceWindow=win;
    ComPtr<IDirect3DDevice9Ex> d;assert(SUCCEEDED(api->CreateDeviceEx(0,D3DDEVTYPE_HAL,win,D3DCREATE_HARDWARE_VERTEXPROCESSING,&pp,nullptr,&d)));
    std::vector<V> native;std::vector<std::uint16_t> ix;
    for(auto& k:MenuMaterials::chainProfile){unsigned n=native.size();for(unsigned j=0;j<3;++j){
        float u=k[j*2]/1000000.f,v=k[j*2+1]/1000000.f;
        if(k[j*2]==823169)u=.823169469833374f; // Exact float from the actual chain capture; catches D3D x87 precision differences.
        native.push_back({u*60+2,v*60+2,.5f,1,0x7f000000,u,v});}
        ix.insert(ix.end(),{std::uint16_t(n),std::uint16_t(n+1),std::uint16_t(n+2)});}
    auto original=native;auto originalIx=ix;
    for(unsigned i=0;i<original.size();++i){auto p=original[i];p.color=0x7fffffff;native.push_back(p);}
    for(unsigned t=0;t<4;++t)ix.insert(ix.end(),{std::uint16_t(original.size()+t*3+2),std::uint16_t(original.size()+t*3+1),std::uint16_t(original.size()+t*3)});
    auto saved=native;std::vector<unsigned char> lit;std::vector<std::uint32_t> filtered;
    auto stats=MenuChains::correct(native.data(),28,native.size(),ix.data(),false,8,lit,filtered);
    assert(stats.removed==4&&filtered.size()==12);
    const void* litVertices=lit.empty()?static_cast<const void*>(native.data()):static_cast<const void*>(lit.data());
    assert(std::memcmp(saved.data(),native.data(),native.size()*28)==0);
    std::vector<std::uint16_t> fixedIx(filtered.begin(),filtered.end());
    ComPtr<IDirect3DSurface9> rt,sys;ComPtr<IDirect3DTexture9> tex;
    assert(SUCCEEDED(d->CreateRenderTarget(64,64,D3DFMT_A8R8G8B8,D3DMULTISAMPLE_NONE,0,FALSE,&rt,nullptr)));
    assert(SUCCEEDED(d->CreateOffscreenPlainSurface(64,64,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&sys,nullptr)));
    assert(SUCCEEDED(d->CreateTexture(4,4,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&tex,nullptr)));
    d->SetRenderTarget(0,rt.Get());d->SetDepthStencilSurface(nullptr);D3DVIEWPORT9 vp{0,0,64,64,0,1};d->SetViewport(&vp);
    d->SetRenderState(D3DRS_ZENABLE,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_LIGHTING,FALSE);
    d->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
    d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);d->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
    d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);d->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
    d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);d->SetTexture(0,tex.Get());d->SetFVF(0x144);
    auto render=[&](DWORD rgba,const void* vertices,const std::vector<std::uint16_t>& drawIx){
        D3DLOCKED_RECT lock{};assert(SUCCEEDED(tex->LockRect(0,&lock,nullptr,D3DLOCK_DISCARD)));
        for(unsigned y=0;y<4;++y)for(unsigned x=0;x<4;++x)std::memcpy((char*)lock.pBits+y*lock.Pitch+x*4,&rgba,4);tex->UnlockRect(0);
        d->Clear(0,nullptr,D3DCLEAR_TARGET,0xff112233,1,0);d->BeginScene();assert(SUCCEEDED(d->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,native.size(),drawIx.size()/3,drawIx.data(),D3DFMT_INDEX16,vertices,28)));d->EndScene();
        assert(SUCCEEDED(d->GetRenderTargetData(rt.Get(),sys.Get())));assert(SUCCEEDED(sys->LockRect(&lock,nullptr,D3DLOCK_READONLY)));
        std::vector<DWORD> pixels(4096);for(unsigned y=0;y<64;++y)std::memcpy(pixels.data()+y*64,(char*)lock.pBits+y*lock.Pitch,256);sys->UnlockRect();return pixels;
    };
    auto a=render(0x00ffffff,native.data(),ix),b=render(0x00ffffff,litVertices,fixedIx);assert(a==b);
    a=render(0x80ffffff,native.data(),ix);std::reverse(ix.begin(),ix.end());b=render(0x80ffffff,native.data(),ix);
    unsigned changed=0;for(unsigned i=0;i<4096;++i)changed+=(a[i]&0xffffff)!=(b[i]&0xffffff);assert(changed>1000);
    std::vector<unsigned char> reversedLit;std::vector<std::uint32_t> reversedFiltered;
    stats=MenuChains::correct(native.data(),28,native.size(),ix.data(),false,8,reversedLit,reversedFiltered);assert(stats.removed==4);
    std::vector<std::uint16_t> reversedIx(reversedFiltered.begin(),reversedFiltered.end());
    auto fixed=render(0x80ffffff,litVertices,fixedIx);
    auto reversed=render(0x80ffffff,reversedLit.empty()?static_cast<const void*>(native.data()):static_cast<const void*>(reversedLit.data()),reversedIx);
    assert(fixed==reversed);
    d->SetTexture(0,nullptr);DestroyWindow(win);
    std::printf("PASS MenuChainsGpuTests: hardware D3D9Ex, alpha-zero 4096 pixels unchanged; native reverse-order artefact on %u pixels; filtered reverse-order images 4096 pixels identical; exact native UV and alpha retained; no game/headset validation.\n",changed);
}
