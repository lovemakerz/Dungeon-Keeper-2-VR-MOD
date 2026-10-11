#pragma once
#include "../common/SnapshotPolicy.hpp"   // 0.9.47.16
// Included after the D3D9 declarations, inside the renderer's namespace.
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
    // 0.9.47.16: which device reads are really needed (see SnapshotPolicy.hpp). Snapshot::policy.fast is switched on by the plugin (FastSnapshot=1).
    inline static snapshotpolicy::State<DWORD,Stages,TS,Samplers,SS> policy{};
    void release() {for(auto& t:texture) if(t){t->Release();t=nullptr;} if(pixel){pixel->Release();pixel=nullptr;}}
    bool capture(IDirect3DDevice9* d) {
        for(int i=0;i<Samplers;++i) if(FAILED(d->GetTexture(i,&texture[i]))) return false;
        if(FAILED(d->GetPixelShader(&pixel))) return false;
        for(size_t i=0;i<RS;++i) d->GetRenderState(renderStates[i],&rs[i]);
        bool present[Samplers];for(int t=0;t<Samplers;++t) present[t]=texture[t]!=nullptr;
        policy.run(ts,ss,present,
            [&](int t,int i){DWORD v=0;d->GetTextureStageState(t,stageStates[i],&v);return v;},
            [&](int t,int i){DWORD v=0;d->GetSamplerState(t,samplerStates[i],&v);return v;},D3DTOP_DISABLE);
        return true;
    }
    bool same(const Snapshot& o) const {
        return !memcmp(texture,o.texture,sizeof(texture))&&pixel==o.pixel&&!memcmp(rs,o.rs,sizeof(rs))&&!memcmp(ts,o.ts,sizeof(ts))&&!memcmp(ss,o.ss,sizeof(ss));
    }
    template<class Device> void apply(Device* d) const {
        for(int i=0;i<Samplers;++i) d->SetTexture(i,texture[i]);
        d->SetPixelShader(pixel);
        for(size_t i=0;i<RS;++i) d->SetRenderState(renderStates[i],rs[i]);
        for(int t=0;t<Stages;++t) for(size_t i=0;i<TS;++i) d->SetTextureStageState(t,stageStates[i],ts[t][i]);
        for(int t=0;t<Samplers;++t) for(size_t i=0;i<SS;++i) d->SetSamplerState(t,samplerStates[i],ss[t][i]);
    }
    template<class Device> uint32_t applyDelta(Device* d,const Snapshot* previous) const {
        if(!previous) {apply(d);return 0;}
        uint32_t avoided=0;
        for(int i=0;i<Samplers;++i) {if(texture[i]!=previous->texture[i]) d->SetTexture(i,texture[i]);else ++avoided;}
        if(pixel!=previous->pixel) d->SetPixelShader(pixel);else ++avoided;
        for(size_t i=0;i<RS;++i) {if(rs[i]!=previous->rs[i]) d->SetRenderState(renderStates[i],rs[i]);else ++avoided;}
        for(int t=0;t<Stages;++t) for(size_t i=0;i<TS;++i) {if(ts[t][i]!=previous->ts[t][i]) d->SetTextureStageState(t,stageStates[i],ts[t][i]);else ++avoided;}
        for(int t=0;t<Samplers;++t) for(size_t i=0;i<SS;++i) {if(ss[t][i]!=previous->ss[t][i]) d->SetSamplerState(t,samplerStates[i],ss[t][i]);else ++avoided;}
        return avoided;
    }
};
