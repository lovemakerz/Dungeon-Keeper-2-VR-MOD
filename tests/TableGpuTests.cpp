// New GPU regression test for the four tabletop clipping planes. No headset.
#include "../src/common/Tabletop.hpp"
#include <d3d9.h>
#include <wrl/client.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace dk2vr;
using Microsoft::WRL::ComPtr;
void must(bool ok,const char* what) {if(!ok){printf("FAIL: %s\n",what);exit(1);}}
void check(HRESULT result,const char* what) {if(FAILED(result)){printf("FAIL: %s (%08lX)\n",what,result);exit(1);}}
int main() {
    constexpr UINT size=256;
    constexpr DWORD ink=0xff12ab34,background=0xff080b10;
    HWND window=CreateWindowExW(0,L"STATIC",L"DK2VR rectangular clipping test",WS_OVERLAPPEDWINDOW,0,0,128,128,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
    must(window!=nullptr,"hidden test window");
    ComPtr<IDirect3D9Ex> api;check(Direct3DCreate9Ex(D3D_SDK_VERSION,&api),"D3D9Ex");
    D3DPRESENT_PARAMETERS pp{};pp.Windowed=TRUE;pp.SwapEffect=D3DSWAPEFFECT_DISCARD;
    pp.BackBufferFormat=D3DFMT_X8R8G8B8;pp.BackBufferWidth=pp.BackBufferHeight=size;pp.hDeviceWindow=window;
    ComPtr<IDirect3DDevice9Ex> device;
    check(api->CreateDeviceEx(0,D3DDEVTYPE_HAL,window,D3DCREATE_HARDWARE_VERTEXPROCESSING,&pp,nullptr,&device),"hardware device");
    D3DCAPS9 caps{};check(device->GetDeviceCaps(&caps),"device caps");must(caps.MaxUserClipPlanes>=4,"four user clip planes supported");
    ComPtr<IDirect3DSurface9> target,readback;
    check(device->CreateRenderTarget(size,size,D3DFMT_A8R8G8B8,D3DMULTISAMPLE_NONE,0,FALSE,&target,nullptr),"render target");
    check(device->CreateOffscreenPlainSurface(size,size,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&readback,nullptr),"readback");
    check(device->SetRenderTarget(0,target.Get()),"bind target");
    D3DVIEWPORT9 viewport{0,0,size,size,0,1};check(device->SetViewport(&viewport),"viewport");
    D3DMATRIX identity{};identity._11=identity._22=identity._33=identity._44=1;
    check(device->SetTransform(D3DTS_WORLD,&identity),"world identity");
    check(device->SetTransform(D3DTS_VIEW,&identity),"view identity");
    D3DMATRIX projection{};Fov fov{-Pi/4,Pi/4,Pi/4,-Pi/4};projectionMatrix(fov,.015f,200,&projection._11);
    check(device->SetTransform(D3DTS_PROJECTION,&projection),"projection");
    check(device->SetRenderState(D3DRS_LIGHTING,FALSE),"unlit");
    check(device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE),"two-sided");
    check(device->SetRenderState(D3DRS_ZENABLE,FALSE),"no depth");
    check(device->SetRenderState(D3DRS_CLIPPING,TRUE),"enable clipping");
    check(device->SetRenderState(D3DRS_CLIPPLANEENABLE,15),"enable four planes");
    check(device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1),"diffuse color");
    check(device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE),"diffuse source");
    check(device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1),"diffuse alpha");
    check(device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE),"alpha source");
    check(device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE),"disable later stage");
    check(device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE),"position and color");
    Projection lens{500,500,319.5f,239.5f};Vec3 up{0,.6427876f,.7660444f};
    auto table=makeTabletop(up,scale(up,-1.4f),lens,0,0,640,480,.92f);must(table.valid,"table footprint");
    Pose source{{0,0,0,1},{0,0,0}};
    unsigned checked=0;
    for(Pose eye:{Pose{{0,0,0,1},{-.032f,0,0}},Pose{pitch(-.08f),{.15f,.04f,.08f}}}) {
        auto planes=table.planes();
        for(DWORD i=0;i<4;++i) {
            Plane p=transformPlane(planes[i],source,eye);
            float c[4]={p.normal.x,p.normal.y,p.normal.z,p.offset};
            check(device->SetClipPlane(i,c),"set plane");
        }
        for(float height:{0.f,.25f}) {
            struct Vertex {Vec3 p;DWORD color;};
            std::vector<Vertex> vertices;
            for(auto corner: {Vec3{-3,0,-3},Vec3{3,0,-3},Vec3{3,0,3},Vec3{-3,0,-3},Vec3{3,0,3},Vec3{-3,0,3}})
                vertices.push_back({toLocal(eye,table.point(corner.x*table.halfWidth,corner.z*table.halfDepth,height)),ink});
            check(device->Clear(0,nullptr,D3DCLEAR_TARGET,background,1,0),"clear");
            check(device->BeginScene(),"begin scene");
            check(device->DrawPrimitiveUP(D3DPT_TRIANGLELIST,2,vertices.data(),sizeof(Vertex)),"clipped draw");
            check(device->EndScene(),"end scene");
            check(device->GetRenderTargetData(target.Get(),readback.Get()),"GPU readback");
            D3DLOCKED_RECT locked{};check(readback->LockRect(&locked,nullptr,D3DLOCK_READONLY),"lock pixels");
            unsigned inside=0,outside=0,errors=0;
            for(UINT y=0;y<size;++y) for(UINT x=0;x<size;++x) {
                // D3D9 raster sample centers are integer viewport coordinates.
                Vec3 ray=rotate(eye.orientation,{2.f*x/size-1,1-2.f*y/size,-1});
                float divisor=dot(table.up,ray);
                if(std::fabs(divisor)<1e-5f) continue;
                float distance=(dot(table.up,table.point(0,0,height))-dot(table.up,eye.position))/divisor;
                if(distance<=.015f||distance>100) continue;
                Vec3 hit=add(eye.position,scale(ray,distance));
                float edgeDistance=1000;
                for(auto p:planes) edgeDistance=std::min(edgeDistance,p.distance(hit));
                // Ignore rasterization boundary tolerances, test both sides.
                if(std::fabs(edgeDistance)<.025f) continue;
                DWORD actual=reinterpret_cast<const DWORD*>(static_cast<const unsigned char*>(locked.pBits)+y*locked.Pitch)[x];
                bool isInside=edgeDistance>0;
                if(isInside) ++inside;else ++outside;
                if(actual!=(isInside?ink:background)) ++errors;
            }
            readback->UnlockRect();
            printf("height=%.2f eyeX=%.3f inside=%u outside=%u mismatches=%u\n",height,eye.position.x,inside,outside,errors);
            must(inside>200&&outside>200,"interior and exterior coverage");
            must(errors==0,"GPU boundary matches ground-space rectangle at both heights and eye poses");
            checked+=inside+outside;
        }
    }
    device.Reset();DestroyWindow(window);
    printf("PASS: D3D9Ex hardware clipping, %u checked pixels, two heights, two eye poses.\n",checked);
}
