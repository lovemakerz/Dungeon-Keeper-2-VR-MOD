#include <d3d9.h>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <cstdio>
#include "../src/plugin/ReplaySnapshot.hpp"
struct Device {
    DWORD rs[256]{},ts[3][64]{},ss[3][16]{};IDirect3DBaseTexture9* tex[3]{};IDirect3DPixelShader9* shader=nullptr;unsigned calls=0;
    void SetTexture(unsigned i,IDirect3DBaseTexture9* t){tex[i]=t;++calls;}
    void SetPixelShader(IDirect3DPixelShader9* p){shader=p;++calls;}
    void SetRenderState(D3DRENDERSTATETYPE i,DWORD v){rs[i]=v;++calls;}
    void SetTextureStageState(unsigned t,D3DTEXTURESTAGESTATETYPE i,DWORD v){ts[t][i]=v;++calls;}
    void SetSamplerState(unsigned t,D3DSAMPLERSTATETYPE i,DWORD v){ss[t][i]=v;++calls;}
    bool same(const Device& b){return !memcmp(rs,b.rs,sizeof(rs))&&!memcmp(ts,b.ts,sizeof(ts))&&!memcmp(ss,b.ss,sizeof(ss))&&!memcmp(tex,b.tex,sizeof(tex))&&shader==b.shader;}
};
int main(){
    Device full,delta;Snapshot current{},previous{};uint64_t saved=0;
    for(unsigned n=0;n<600;++n) {
        if(n%4==0) current.rs[n%Snapshot::RS]=n;
        if(n%4==1) current.ts[n%3][n%Snapshot::TS]=n;
        if(n%4==2) current.ss[n%3][n%Snapshot::SS]=n;
        if(n%4==3) current.texture[n%3]=reinterpret_cast<IDirect3DBaseTexture9*>(uintptr_t(n+1));
        current.apply(&full);saved+=current.applyDelta(&delta,n?&previous:nullptr);
        assert(full.same(delta));previous=current;
    }
    assert(saved>40000);assert(full.calls>delta.calls*10);
    // Every eye starts with a full application, irrespective of the preceding eye.
    Device nextEye;current.applyDelta(&nextEye,nullptr);assert(full.same(nextEye));
    printf("PASS: 600 real Snapshot sequences equivalent; mock setter calls %u -> %u (not a GPU/FPS benchmark).\n",full.calls,delta.calls);
}
