#include <windows.h>
#include <d3d9.h>
#include <wrl/client.h>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "../src/plugin/ReplaySnapshot.hpp"
using Microsoft::WRL::ComPtr;
void check(HRESULT hr,const char* name){if(FAILED(hr)){printf("FAIL %s %08lX\n",name,(unsigned long)hr);exit(1);}}
struct V{float x,y,z,w;DWORD color;};
int main(){
    HWND win=CreateWindowExW(0,L"STATIC",L"DK2VR hidden replay verification",0,0,0,64,64,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
    if(!win)return 2;
    ComPtr<IDirect3D9Ex> api;check(Direct3DCreate9Ex(D3D_SDK_VERSION,&api),"D3D9Ex");
    D3DPRESENT_PARAMETERS pp{};pp.Windowed=TRUE;pp.SwapEffect=D3DSWAPEFFECT_DISCARD;pp.BackBufferWidth=pp.BackBufferHeight=64;pp.BackBufferFormat=D3DFMT_X8R8G8B8;pp.hDeviceWindow=win;
    ComPtr<IDirect3DDevice9Ex> d;check(api->CreateDeviceEx(0,D3DDEVTYPE_HAL,win,D3DCREATE_HARDWARE_VERTEXPROCESSING,&pp,nullptr,&d),"hardware device");
    ComPtr<IDirect3DSurface9> target,sys;check(d->CreateRenderTarget(64,64,D3DFMT_A8R8G8B8,D3DMULTISAMPLE_NONE,0,FALSE,&target,nullptr),"target");
    check(d->CreateOffscreenPlainSurface(64,64,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&sys,nullptr),"readback");
    check(d->SetRenderTarget(0,target.Get()),"bind");d->SetDepthStencilSurface(nullptr);D3DVIEWPORT9 vp{0,0,64,64,0,1};d->SetViewport(&vp);
    d->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);d->SetVertexShader(nullptr);d->SetPixelShader(nullptr);d->SetTexture(0,nullptr);
    d->SetRenderState(D3DRS_ZENABLE,FALSE);d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
    d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
    d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
    Snapshot opaque{},blend{};d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);if(!opaque.capture(d.Get()))return 3;
    d->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);if(!blend.capture(d.Get()))return 4;
    const V triangles[6]={{4,4,.5f,1,0xff20a040},{54,4,.5f,1,0xff20a040},{4,54,.5f,1,0xff20a040},{54,4,.5f,1,0xff2060c0},{54,54,.5f,1,0xff2060c0},{4,54,.5f,1,0xff2060c0}};
    const V over[3]={{12,12,.5f,1,0x80e03020},{48,12,.5f,1,0x80e03020},{30,48,.5f,1,0x80e03020}};
    auto render=[&](bool optimized){
        check(d->Clear(0,nullptr,D3DCLEAR_TARGET,0xff080b10,1,0),"clear");check(d->BeginScene(),"begin");
        opaque.applyDelta(d.Get(),nullptr);
        if(optimized)check(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,2,triangles,sizeof(V)),"merged");
        else {check(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,1,triangles,sizeof(V)),"first");opaque.apply(d.Get());check(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,1,triangles+3,sizeof(V)),"second");}
        if(optimized)blend.applyDelta(d.Get(),&opaque);else blend.apply(d.Get());
        check(d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,1,over,sizeof(V)),"alpha");check(d->EndScene(),"end");
        check(d->GetRenderTargetData(target.Get(),sys.Get()),"pixels");D3DLOCKED_RECT lr{};check(sys->LockRect(&lr,nullptr,D3DLOCK_READONLY),"lock");
        std::vector<unsigned char> pixels(64*64*4);for(int y=0;y<64;++y)memcpy(pixels.data()+y*64*4,static_cast<const unsigned char*>(lr.pBits)+y*lr.Pitch,64*4);sys->UnlockRect();return pixels;
    };
    auto reference=render(false),optimized=render(true);if(reference!=optimized){puts("FAIL GPU pixels differ");return 5;}
    DWORD center=0;memcpy(&center,optimized.data()+(32*64+32)*4,4);if(center==0xff080b10){puts("FAIL empty fixture");return 6;}
    opaque.release();blend.release();DestroyWindow(win);puts("PASS hardware D3D9Ex: 4096 pixels identical, merged triangles and alpha state delta (no headset/FPS claim).");
}
