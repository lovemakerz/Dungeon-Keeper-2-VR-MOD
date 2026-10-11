#pragma once
// made by LoVeMaKeRz
#include <d3d9.h>
#include <wrl/client.h>
#include <array>
struct MenuDrawState {
    static constexpr D3DRENDERSTATETYPE states[]={D3DRS_ZENABLE,D3DRS_ZFUNC,D3DRS_FOGENABLE,D3DRS_FOGCOLOR,D3DRS_FOGTABLEMODE,D3DRS_FOGVERTEXMODE,D3DRS_FOGSTART,D3DRS_FOGEND,D3DRS_FOGDENSITY,D3DRS_RANGEFOGENABLE};
    DWORD value[sizeof(states)/sizeof(states[0])]{};
    DWORD srgb[3]{},mip[3]{},constant[3]{};
    void capture(IDirect3DDevice9* d) {for(int j=0;j<3;++j){d->GetSamplerState(j,D3DSAMP_SRGBTEXTURE,&srgb[j]);d->GetSamplerState(j,D3DSAMP_MAXMIPLEVEL,&mip[j]);d->GetTextureStageState(j,D3DTSS_CONSTANT,&constant[j]);}for(size_t i=0;i<std::size(states);++i) d->GetRenderState(states[i],&value[i]);}
    void apply(IDirect3DDevice9* d) const {for(int j=0;j<3;++j){d->SetSamplerState(j,D3DSAMP_SRGBTEXTURE,srgb[j]);d->SetSamplerState(j,D3DSAMP_MAXMIPLEVEL,mip[j]);d->SetTextureStageState(j,D3DTSS_CONSTANT,constant[j]);}for(size_t i=0;i<std::size(states);++i) d->SetRenderState(states[i],value[i]);}
};
// Full resolution render of the actual meshes and textures; native Z is never compressed.
// The original eye depth/stencil stays intact and clips the final color to the visible window.
struct MenuDepthTarget {
    template<class T> using Ptr=Microsoft::WRL::ComPtr<T>;
    Ptr<IDirect3DTexture9> color;Ptr<IDirect3DSurface9> target,depth,oldTarget,oldDepth;
    Ptr<IDirect3DStateBlock9> restore;D3DVIEWPORT9 oldViewport{};UINT width=0,height=0;
    bool begin(IDirect3DDevice9* d,UINT w,UINT h) {
        if(!color||w!=width||h!=height) {
            color.Reset();target.Reset();depth.Reset();width=w;height=h;
            if(FAILED(d->CreateTexture(w,h,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&color,nullptr))||
                FAILED(color->GetSurfaceLevel(0,&target))||FAILED(d->CreateDepthStencilSurface(w,h,D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,TRUE,&depth,nullptr))) {color.Reset();return false;}
        }
        if(!restore&&FAILED(d->CreateStateBlock(D3DSBT_ALL,&restore))) return false;
        if(FAILED(restore->Capture())||FAILED(d->GetRenderTarget(0,&oldTarget))||FAILED(d->GetDepthStencilSurface(&oldDepth))||FAILED(d->GetViewport(&oldViewport))) return false;
        d->SetTexture(0,nullptr);d->SetDepthStencilSurface(nullptr);
        if(FAILED(d->SetRenderTarget(0,target.Get()))||FAILED(d->SetDepthStencilSurface(depth.Get()))) {cancel(d);return false;}
        D3DVIEWPORT9 vp{0,0,w,h,0,1};d->SetViewport(&vp);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);
        if(FAILED(d->Clear(0,nullptr,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,0xff000000,1.f,0))) {cancel(d);return false;}
        return true;
    }
    void cancel(IDirect3DDevice9* d) {d->SetDepthStencilSurface(nullptr);d->SetRenderTarget(0,oldTarget.Get());d->SetDepthStencilSurface(oldDepth.Get());restore->Apply();d->SetViewport(&oldViewport);oldTarget.Reset();oldDepth.Reset();}
    void finish(IDirect3DDevice9* d) {
        d->SetDepthStencilSurface(nullptr);d->SetRenderTarget(0,oldTarget.Get());d->SetDepthStencilSurface(oldDepth.Get());restore->Apply();d->SetViewport(&oldViewport);
        d->SetVertexShader(nullptr);d->SetPixelShader(nullptr);d->SetFVF(D3DFVF_XYZRHW|D3DFVF_TEX1);
        d->SetRenderState(D3DRS_ZENABLE,FALSE);d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);d->SetRenderState(D3DRS_FOGENABLE,FALSE);
        d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_COLORWRITEENABLE,15);
        d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);
        d->SetRenderState(D3DRS_STENCILENABLE,TRUE);d->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_EQUAL);d->SetRenderState(D3DRS_STENCILREF,1);
        d->SetRenderState(D3DRS_STENCILMASK,255);d->SetRenderState(D3DRS_STENCILWRITEMASK,0);d->SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_KEEP);
        d->SetRenderState(D3DRS_STENCILFAIL,D3DSTENCILOP_KEEP);d->SetRenderState(D3DRS_STENCILZFAIL,D3DSTENCILOP_KEEP);
        d->SetRenderState(D3DRS_SRGBWRITEENABLE,FALSE);d->SetSamplerState(0,D3DSAMP_SRGBTEXTURE,FALSE);
        d->SetTexture(0,color.Get());d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
        d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
        d->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);d->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
        d->SetTextureStageState(0,D3DTSS_RESULTARG,D3DTA_CURRENT);d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);d->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
        d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
        d->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_POINT);d->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_POINT);d->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_NONE);
        d->SetSamplerState(0,D3DSAMP_MIPMAPLODBIAS,0);d->SetSamplerState(0,D3DSAMP_MAXMIPLEVEL,0);
        struct V {float x,y,z,w,u,v;};float x=float(width)-.5f,y=float(height)-.5f;
        const V quad[4]={{-.5f,-.5f,0,1,0,0},{x,-.5f,0,1,1,0},{-.5f,y,0,1,0,1},{x,y,0,1,1,1}};
        d->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,quad,sizeof(V));d->SetTexture(0,nullptr);
        restore->Apply();d->SetViewport(&oldViewport);oldTarget.Reset();oldDepth.Reset();
    }
};
