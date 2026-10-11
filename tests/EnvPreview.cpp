// DK2VR 0.4.0 - offline preview of the room and table (same Environment.inl as the VR thread).
// Renders views to BMP files with D3D9Ex (runs under Windows or Wine). Not shipped.
#include "../src/common/Protocol.hpp"
#include "../src/common/Tabletop.hpp"
#include "../src/common/Decouple.hpp"
#include "../src/common/TableModel.hpp"
#include <d3d9.h>
#include <d3dcommon.h>
#include <wrl/client.h>
#include <vector>
#include <functional>
#include <string>
#include <cstdio>
#include <cstring>
#include <algorithm>
using Microsoft::WRL::ComPtr;
using namespace dk2vr;
namespace {
double milliseconds() {static LARGE_INTEGER f{};if(!f.QuadPart) QueryPerformanceFrequency(&f);LARGE_INTEGER n{};QueryPerformanceCounter(&n);return double(n.QuadPart)*1000./double(f.QuadPart);}
void log(const char* fmt,...) {va_list a;va_start(a,fmt);vprintf(fmt,a);va_end(a);printf("\n");}
#include "../src/plugin/Environment.inl"
}
static void saveBmp(const char* name,IDirect3DDevice9Ex* d,IDirect3DSurface9* rt,UINT w,UINT h) {
    ComPtr<IDirect3DSurface9> sys;d->CreateOffscreenPlainSurface(w,h,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&sys,nullptr);
    d->GetRenderTargetData(rt,sys.Get());D3DLOCKED_RECT lr{};sys->LockRect(&lr,nullptr,D3DLOCK_READONLY);
    FILE* f=fopen(name,"wb");BITMAPFILEHEADER fh{};BITMAPINFOHEADER ih{};
    fh.bfType=0x4d42;fh.bfOffBits=sizeof(fh)+sizeof(ih);fh.bfSize=fh.bfOffBits+w*h*4;
    ih.biSize=sizeof(ih);ih.biWidth=w;ih.biHeight=-int(h);ih.biPlanes=1;ih.biBitCount=32;
    fwrite(&fh,sizeof(fh),1,f);fwrite(&ih,sizeof(ih),1,f);
    for(UINT y=0;y<h;++y) fwrite(static_cast<unsigned char*>(lr.pBits)+y*lr.Pitch,4,w,f);
    fclose(f);sys->UnlockRect();
}
int main() {
    HWND wnd=CreateWindowExW(0,L"STATIC",L"prev",WS_POPUP,0,0,16,16,nullptr,nullptr,nullptr,nullptr);
    ComPtr<IDirect3D9Ex> api;Direct3DCreate9Ex(D3D_SDK_VERSION,&api);
    D3DPRESENT_PARAMETERS pp{};pp.Windowed=TRUE;pp.SwapEffect=D3DSWAPEFFECT_DISCARD;pp.BackBufferFormat=D3DFMT_UNKNOWN;pp.BackBufferWidth=pp.BackBufferHeight=16;pp.hDeviceWindow=wnd;
    ComPtr<IDirect3DDevice9Ex> d;
    if(FAILED(api->CreateDeviceEx(0,D3DDEVTYPE_HAL,wnd,D3DCREATE_HARDWARE_VERTEXPROCESSING,&pp,nullptr,&d))) {printf("device failed\n");return 1;}
    const UINT W=1280,H=960;
    ComPtr<IDirect3DSurface9> rt,ds;d->CreateRenderTarget(W,H,D3DFMT_A8R8G8B8,D3DMULTISAMPLE_NONE,0,TRUE,&rt,nullptr);
    d->CreateDepthStencilSurface(W,H,D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,TRUE,&ds,nullptr);
    env::ShaderLights="8";
    env::Scene scene;int calls=0;double worst=0;scene.startBackground(L"preview_room.cache");
    while(!scene.init(d.Get())) {if(scene.failed) return 2;double t=milliseconds();Sleep(11);(void)t;++calls;}
    printf("ready after %d frame calls\n",calls);
    {Request q{};q.tableAnchor=1;q.tableWidthMeters=.8f;q.anchor={{0,0,0,1},{0,1.65f,0}};q.table={{0,0,0,1},{0,1.05f,-.8f}};q.handFlags=RequestFloorKnown;q.floorY=0;
     env::Scene::FrameState f;scene.prepare(d.Get(),q,.75f,.75f,f);
     const char* names[]={"floor","walls","vault","ribs","plinth","door","surround","iron","sticks"};
     size_t roomT=0;for(size_t i=0;i<scene.room.size();++i) {printf("room %-9s %6zu triangles\n",i<9?names[i]:"?",scene.room[i].v.size()/3);roomT+=scene.room[i].v.size()/3;}
     size_t tab=scene.table[0].v.size()/3+scene.table[1].v.size()/3,h=0;for(size_t i=2;i<scene.table.size();++i) h+=scene.table[i].v.size()/3;
     printf("ROOM %zu | table frame %zu, legs/apron/stretchers %zu | 8 handles %zu (%zu each)\n",roomT,scene.table[0].v.size()/3,scene.table[1].v.size()/3,h,h/8);}
    Request r{};r.tableAnchor=1;r.tableWidthMeters=.8f;
    r.anchor={{0,0,0,1},{0,1.65f,0}};
    r.table={{0,0,0,1},{0,1.05f,-.8f}};
    r.handFlags=RequestFloorKnown|HandRightValid|HandLeftValid;r.floorY=0;
    r.hands[0]={{0,0,0,1},{-.55f,.98f,-.8f}};r.hands[1]={{0,0,0,1},{.26f,.98f,-.39f}};
    struct View {const char* name;Vec3 eye;float yaw,pitchDeg;} views[]={
        {"preview_player.bmp",{0,1.65f,0},0,-38},{"preview_room.bmp",{0,1.65f,.6f},0,-8},
        {"preview_side.bmp",{-.9f,1.3f,-.5f},-1.2f,-22},{"preview_handle.bmp",{.75f,1.15f,-.55f},.9f,-35},
        {"preview_back.bmp",{0,1.7f,-.2f},3.14159f,-15},{"preview_vault.bmp",{0,1.6f,.8f},0,25},{"preview_low.bmp",{.2f,.75f,-.2f},.35f,-5},{"preview_edge.bmp",{-.15f,1.3f,-.3f},0.f,-62},{"preview_edge2.bmp",{.3f,1.15f,-.1f},-.3f,-30}};
    for(auto& v:views) {
        d->SetVertexDeclaration(nullptr);
        Quat q=normalized(multiply(yawQuat(v.yaw),pitch(v.pitchDeg*Pi/180)));
        r.eyes[0]=r.eyes[1]={{q,v.eye},{-0.75f,0.75f,0.6f,-0.6f}};
        env::Scene::FrameState f;scene.prepare(d.Get(),r,.75f,.75f,f);
        d->SetRenderTarget(0,rt.Get());d->SetDepthStencilSurface(ds.Get());
        d->Clear(0,nullptr,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0xff080b10,1,0);d->BeginScene();
        D3DMATRIX id{};id._11=id._22=id._33=id._44=1;D3DMATRIX proj{};projectionMatrix(r.eyes[0].fov,.015f,200,&proj._11);
        d->SetTransform(D3DTS_VIEW,&id);d->SetTransform(D3DTS_PROJECTION,&proj);
        d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
        scene.drawOpaque(d.Get(),f,r.eyes[0].pose,proj);
        D3DMATRIX table{};matrixOf(then(similarity(f.table),inverse(similarity(r.eyes[0].pose))),&table._11);
        d->SetVertexShader(nullptr);d->SetPixelShader(nullptr);
        scene.drawTray(d.Get(),f,r.eyes[0].pose,proj);
        scene.drawFlames(d.Get(),f,r.eyes[0].pose);
        d->EndScene();saveBmp(v.name,d.Get(),rt.Get(),W,H);printf("%s\n",v.name);
    }
    return 0;
}
