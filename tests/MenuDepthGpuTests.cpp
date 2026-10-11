// made by LoVeMaKeRz
#include <windows.h>
#include <d3d9.h>
#include <wrl/client.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../src/plugin/MenuDepthTarget.hpp"
using Microsoft::WRL::ComPtr;
void check(HRESULT hr,const char* s){if(FAILED(hr)){printf("FAIL %s %08lX\n",s,(unsigned long)hr);exit(1);}}
struct V {float x,y,z,w;DWORD c;};
int main(){
 HWND win=CreateWindowExW(0,L"STATIC",L"DK2VR menu verification",0,0,0,64,64,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
 ComPtr<IDirect3D9Ex> api;check(Direct3DCreate9Ex(D3D_SDK_VERSION,&api),"api");
 D3DPRESENT_PARAMETERS pp{};pp.Windowed=TRUE;pp.SwapEffect=D3DSWAPEFFECT_DISCARD;pp.BackBufferWidth=pp.BackBufferHeight=64;pp.BackBufferFormat=D3DFMT_X8R8G8B8;pp.hDeviceWindow=win;
 ComPtr<IDirect3DDevice9Ex> d;check(api->CreateDeviceEx(0,D3DDEVTYPE_HAL,win,D3DCREATE_HARDWARE_VERTEXPROCESSING,&pp,nullptr,&d),"device");
 ComPtr<IDirect3DSurface9> rt,ds,sys;check(d->CreateRenderTarget(64,64,D3DFMT_A8R8G8B8,D3DMULTISAMPLE_NONE,0,FALSE,&rt,nullptr),"rt");
 check(d->CreateDepthStencilSurface(64,64,D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,TRUE,&ds,nullptr),"depth");check(d->CreateOffscreenPlainSurface(64,64,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&sys,nullptr),"sys");
 d->SetRenderTarget(0,rt.Get());d->SetDepthStencilSurface(ds.Get());D3DVIEWPORT9 vp{0,0,64,64,0,1};d->SetViewport(&vp);
 auto plain=[&]{d->SetTexture(0,nullptr);d->SetVertexShader(nullptr);d->SetPixelShader(nullptr);d->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_FOGENABLE,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);};
 auto rect=[&](float xmax,float z,DWORD c){V q[4]={{-.5f,-.5f,z,1,c},{xmax-.5f,-.5f,z,1,c},{-.5f,63.5f,z,1,c},{xmax-.5f,63.5f,z,1,c}};check(d->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,q,sizeof(V)),"draw");};
 MenuDepthTarget menu;
 for(int iteration=0;iteration<3;++iteration){
  check(d->Clear(0,nullptr,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,0xff00a000,.4f,0),"clear scene");check(d->BeginScene(),"begin");plain();
  d->SetRenderState(D3DRS_ZENABLE,FALSE);d->SetRenderState(D3DRS_COLORWRITEENABLE,0);d->SetRenderState(D3DRS_STENCILENABLE,TRUE);d->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_ALWAYS);d->SetRenderState(D3DRS_STENCILREF,1);d->SetRenderState(D3DRS_STENCILWRITEMASK,255);d->SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_REPLACE);rect(32,.4f,0);
  d->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_EQUAL);d->SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_KEEP);d->SetRenderState(D3DRS_STENCILWRITEMASK,0);d->SetRenderState(D3DRS_COLORWRITEENABLE,15);
  if(!menu.begin(d.Get(),64,64)){puts("FAIL isolated target");return 2;}plain();d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_ZENABLE,TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
  rect(64,.900001f,0xffc03020);rect(64,.900003f,0xff2030d0);menu.finish(d.Get());
  plain();d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_ZENABLE,TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);rect(64,.7f,0xffffffff); // must fail against untouched scene depth .4
  check(d->EndScene(),"end");check(d->GetRenderTargetData(rt.Get(),sys.Get()),"readback");D3DLOCKED_RECT lr{};check(sys->LockRect(&lr,nullptr,D3DLOCK_READONLY),"lock");
  for(int y=0;y<64;++y)for(int x=0;x<64;++x){DWORD got;memcpy(&got,(char*)lr.pBits+y*lr.Pitch+x*4,4);DWORD expected=x<32?0xffc03020:0xff00a000;if(got!=expected){printf("FAIL pixel %d,%d = %08lX expected %08lX\n",x,y,(unsigned long)got,(unsigned long)expected);return 3;}}
  sys->UnlockRect();
 }
 d->SetRenderState(D3DRS_FOGENABLE,TRUE);d->SetRenderState(D3DRS_FOGCOLOR,0xff102030);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESS);MenuDrawState state;state.capture(d.Get());d->SetRenderState(D3DRS_FOGENABLE,FALSE);d->SetRenderState(D3DRS_FOGCOLOR,0);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);state.apply(d.Get());DWORD v;d->GetRenderState(D3DRS_FOGENABLE,&v);if(v!=TRUE)return 4;d->GetRenderState(D3DRS_FOGCOLOR,&v);if(v!=0xff102030)return 5;d->GetRenderState(D3DRS_ZFUNC,&v);if(v!=D3DCMP_LESS)return 6;
 for(int stage=0;stage<3;++stage){d->SetSamplerState(stage,D3DSAMP_SRGBTEXTURE,stage==1);d->SetSamplerState(stage,D3DSAMP_MAXMIPLEVEL,stage+2);d->SetTextureStageState(stage,D3DTSS_CONSTANT,0xff102030+stage);}
 state.capture(d.Get());for(int stage=0;stage<3;++stage){d->SetSamplerState(stage,D3DSAMP_SRGBTEXTURE,stage!=1);d->SetSamplerState(stage,D3DSAMP_MAXMIPLEVEL,0);d->SetTextureStageState(stage,D3DTSS_CONSTANT,0);}
 state.apply(d.Get());for(int stage=0;stage<3;++stage){d->GetSamplerState(stage,D3DSAMP_SRGBTEXTURE,&v);if(v!=DWORD(stage==1))return 7;d->GetSamplerState(stage,D3DSAMP_MAXMIPLEVEL,&v);if(v!=DWORD(stage+2))return 8;d->GetTextureStageState(stage,D3DTSS_CONSTANT,&v);if(v!=DWORD(0xff102030+stage))return 9;}
 // Pixel proof: a leftover sRGB sampler darkens a 128 gray texel; native state restores 128.
 ComPtr<IDirect3DTexture9> tex;check(d->CreateTexture(2,2,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&tex,nullptr),"color texture");
 D3DLOCKED_RECT tx{};check(tex->LockRect(0,&tx,nullptr,D3DLOCK_DISCARD),"texture lock");for(int y=0;y<2;++y)for(int x=0;x<2;++x)*reinterpret_cast<DWORD*>((char*)tx.pBits+y*tx.Pitch+x*4)=0xff808080;tex->UnlockRect(0);
 plain();d->SetRenderState(D3DRS_ZENABLE,FALSE);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_SRGBWRITEENABLE,FALSE);d->SetSamplerState(0,D3DSAMP_SRGBTEXTURE,FALSE);d->SetSamplerState(0,D3DSAMP_MAXMIPLEVEL,0);state.capture(d.Get());
 d->SetTexture(0,tex.Get());d->SetFVF(D3DFVF_XYZRHW|D3DFVF_TEX1);d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
 d->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_POINT);d->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_POINT);d->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_NONE);
 struct TV {float x,y,z,w,u,v;};auto colorRect=[&](float left,float right){TV q[4]={{left-.5f,-.5f,0,1,0,0},{right-.5f,-.5f,0,1,1,0},{left-.5f,63.5f,0,1,0,1},{right-.5f,63.5f,0,1,1,1}};check(d->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,q,sizeof(TV)),"color draw");};
 check(d->BeginScene(),"color scene");d->SetSamplerState(0,D3DSAMP_SRGBTEXTURE,TRUE);colorRect(0,32);state.apply(d.Get());colorRect(32,64);check(d->EndScene(),"color end");check(d->GetRenderTargetData(rt.Get(),sys.Get()),"color readback");check(sys->LockRect(&tx,nullptr,D3DLOCK_READONLY),"color lock");DWORD a,b;memcpy(&a,(char*)tx.pBits+32*tx.Pitch+16*4,4);memcpy(&b,(char*)tx.pBits+32*tx.Pitch+48*4,4);sys->UnlockRect();
 if((a&255)>65||std::abs(int(b&255)-128)>2){printf("FAIL native color leak: dirty=%lu restored=%lu\n",(unsigned long)(a&255),(unsigned long)(b&255));return 10;}
 printf("PASS menu color leak reproduction: dirty sRGB texel %lu, restored native %lu (expected 128). Mechanism proved; game cause unconfirmed.\n",(unsigned long)(a&255),(unsigned long)(b&255));
 DestroyWindow(win);puts("PASS MenuDepthGpuTests: 12288 exact pixels, precise near/far ordering, original scene depth and stencil mask preserved, repeated target reuse, native fog/depth states. No game/headset validation.");
}
