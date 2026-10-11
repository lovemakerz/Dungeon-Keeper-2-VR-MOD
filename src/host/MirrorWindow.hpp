#pragma once
// DK2VR 0.9.47.61 - PC mirror window (for streamers / video makers). Made by LoVeMaKeRz.
// A normal resizable window (always 16:9) showing ONE complete image (one eye, never stretched, MirrorLayout.hpp).
// Off by default. Switched on from the OPTIONS panel (PC MIRROR) or with MirrorWindow=1 in DK2VR.ini. Costs one small GPU draw
// at most 60 times per second while ON, nothing while OFF (no window, no swap chain).
// The window lives on its own thread (message loop); all D3D11 work stays on the caller's thread.
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <atomic>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <thread>
#include "../common/MirrorLayout.hpp"

namespace dk2vr {
struct MirrorWindow {
    template<class T> using ComPtr=Microsoft::WRL::ComPtr<T>;
    HWND hwnd=nullptr;std::thread pump;std::atomic<bool> ready{false},closedByUser{false},quit{false};
    std::atomic<int> wantW{1280},wantH{720};   // client size seen by the window thread
    ComPtr<IDXGISwapChain> chain;ComPtr<ID3D11RenderTargetView> rtv;ComPtr<ID3D11VertexShader> vs;ComPtr<ID3D11PixelShader> ps;ComPtr<ID3D11SamplerState> smp;ComPtr<ID3D11RasterizerState> rs;
    UINT backW=0,backH=0;ULONGLONG lastDraw=0;bool shown=false,failed=false;DWORD style=WS_OVERLAPPEDWINDOW;
    char error[160]{};bool onTop=true;ULONGLONG lastTopCheck=0;   // 0.9.47.62: MirrorOnTop=1 (default): the window stays in front of the game window
    void raise() {if(hwnd) SetWindowPos(hwnd,onTop?HWND_TOPMOST:HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);}   // never takes the focus away from the game

    static LRESULT CALLBACK proc(HWND h,UINT m,WPARAM w,LPARAM l) {
        MirrorWindow* self=reinterpret_cast<MirrorWindow*>(GetWindowLongPtrW(h,GWLP_USERDATA));
        switch(m) {
        case WM_CREATE: SetWindowLongPtrW(h,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCTW*>(l)->lpCreateParams));break;
        case WM_CLOSE: if(self) self->closedByUser=true;ShowWindow(h,SW_HIDE);return 0;   // the X button = switch the mirror OFF (the host turns the option off)
        case WM_SIZE: if(self&&w!=SIZE_MINIMIZED) {self->wantW=int(LOWORD(l));self->wantH=int(HIWORD(l));}break;
        case WM_SIZING: {   // keep 16:9 while dragging an edge
            RECT* r=reinterpret_cast<RECT*>(l);RECT nc{0,0,0,0};AdjustWindowRect(&nc,WS_OVERLAPPEDWINDOW,FALSE);
            const int extraW=(nc.right-nc.left),extraH=(nc.bottom-nc.top);
            int cw=(r->right-r->left)-extraW,ch=(r->bottom-r->top)-extraH;
            const bool heightDrives=(w==WMSZ_TOP||w==WMSZ_BOTTOM);
            mirrorKeep169(cw,ch,heightDrives);
            if(w==WMSZ_LEFT||w==WMSZ_TOPLEFT||w==WMSZ_BOTTOMLEFT) r->left=r->right-(cw+extraW); else r->right=r->left+(cw+extraW);
            if(w==WMSZ_TOP||w==WMSZ_TOPLEFT||w==WMSZ_TOPRIGHT) r->top=r->bottom-(ch+extraH); else r->bottom=r->top+(ch+extraH);
            return TRUE;}
        case WM_ERASEBKGND: return 1;
        }
        return DefWindowProcW(h,m,w,l);
    }
    void threadMain() {
        WNDCLASSW wc{};wc.lpfnWndProc=proc;wc.hInstance=GetModuleHandleW(nullptr);wc.lpszClassName=L"DK2VRMirrorWindow";wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
        RegisterClassW(&wc);
        RECT r{0,0,1280,720};AdjustWindowRect(&r,style,FALSE);
        hwnd=CreateWindowExW(0,wc.lpszClassName,L"DK2 VR - Miroir PC / PC mirror (Made by LoVeMaKeRz)",style,CW_USEDEFAULT,CW_USEDEFAULT,r.right-r.left,r.bottom-r.top,nullptr,nullptr,wc.hInstance,this);
        ready=true;
        if(!hwnd) return;
        MSG m;while(!quit&&GetMessageW(&m,nullptr,0,0)>0) {TranslateMessage(&m);DispatchMessageW(&m);}
        DestroyWindow(hwnd);hwnd=nullptr;
    }
    bool start(ID3D11Device* device) {
        if(hwnd) return true;
        if(failed) return false;
        quit=false;ready=false;pump=std::thread([this]{threadMain();});
        for(int i=0;i<200&&!ready;++i) Sleep(5);
        if(!hwnd) {failed=true;snprintf(error,sizeof(error),"fenetre non creee");return false;}
        try {
            ComPtr<IDXGIDevice> dd;if(FAILED(device->QueryInterface(IID_PPV_ARGS(&dd)))) throw 1;
            ComPtr<IDXGIAdapter> ad;if(FAILED(dd->GetAdapter(&ad))) throw 2;
            ComPtr<IDXGIFactory> fa;if(FAILED(ad->GetParent(IID_PPV_ARGS(&fa)))) throw 3;
            DXGI_SWAP_CHAIN_DESC sd{};sd.BufferCount=1;sd.BufferDesc.Width=1280;sd.BufferDesc.Height=720;sd.BufferDesc.Format=DXGI_FORMAT_B8G8R8A8_UNORM;
            sd.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;sd.OutputWindow=hwnd;sd.SampleDesc.Count=1;sd.Windowed=TRUE;sd.SwapEffect=DXGI_SWAP_EFFECT_DISCARD;
            if(FAILED(fa->CreateSwapChain(device,&sd,&chain))) throw 4;
            fa->MakeWindowAssociation(hwnd,DXGI_MWA_NO_ALT_ENTER);
            const char* vertex="struct V{float4 p:SV_Position;float2 uv:TEXCOORD;};V main(uint id:SV_VertexID){V v;v.uv=float2((id<<1)&2,id&2);v.p=float4(v.uv.x*2-1,1-v.uv.y*2,0,1);return v;}";
            const char* pixel="Texture2D t:register(t0);SamplerState s:register(s0);float4 main(float4 p:SV_Position,float2 uv:TEXCOORD):SV_Target{return float4(t.Sample(s,uv).rgb,1);}";
            ComPtr<ID3DBlob> code,err;
            if(FAILED(D3DCompile(vertex,strlen(vertex),nullptr,nullptr,nullptr,"main","vs_4_0",0,0,&code,&err))) throw 5;
            if(FAILED(device->CreateVertexShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&vs))) throw 6;
            code.Reset();err.Reset();
            if(FAILED(D3DCompile(pixel,strlen(pixel),nullptr,nullptr,nullptr,"main","ps_4_0",0,0,&code,&err))) throw 7;
            if(FAILED(device->CreatePixelShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&ps))) throw 8;
            D3D11_SAMPLER_DESC ss{};ss.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;ss.AddressU=ss.AddressV=ss.AddressW=D3D11_TEXTURE_ADDRESS_CLAMP;ss.MaxLOD=D3D11_FLOAT32_MAX;
            if(FAILED(device->CreateSamplerState(&ss,&smp))) throw 9;
            D3D11_RASTERIZER_DESC rd{};rd.FillMode=D3D11_FILL_SOLID;rd.CullMode=D3D11_CULL_NONE;rd.DepthClipEnable=TRUE;
            if(FAILED(device->CreateRasterizerState(&rd,&rs))) throw 10;
        } catch(int step) {failed=true;snprintf(error,sizeof(error),"etape %d",step);stop();return false;}
        return true;
    }
    void stop() {
        if(pump.joinable()) {quit=true;if(hwnd) PostMessageW(hwnd,WM_QUIT,0,0);if(hwnd) PostMessageW(hwnd,WM_NULL,0,0);pump.join();}
        rtv.Reset();chain.Reset();hwnd=nullptr;shown=false;
    }
    ~MirrorWindow() {stop();}
    void setVisible(bool on) {
        if(!hwnd) return;
        if(on&&!shown) {ShowWindow(hwnd,SW_SHOWNOACTIVATE);shown=true;closedByUser=false;raise();}
        else if(!on&&shown) {ShowWindow(hwnd,SW_HIDE);shown=false;}
    }
    bool takeClosed() {return closedByUser.exchange(false);}
    // Draw ONE eye as a complete image (crop to 16:9 when fill, otherwise the whole eye with bars). ~60 Hz at most.
    void draw(ID3D11Device* device,ID3D11DeviceContext* c,ID3D11ShaderResourceView* eye,float eyeW,float eyeH,bool fill,float centreY) {
        if(!hwnd||!chain||!shown||!eye) return;
        const ULONGLONG now=GetTickCount64();if(now-lastDraw<15) return;lastDraw=now;
        if(onTop&&now-lastTopCheck>1000) {lastTopCheck=now;if(!(GetWindowLongW(hwnd,GWL_EXSTYLE)&WS_EX_TOPMOST)||GetTopWindow(nullptr)!=hwnd) raise();}   // the fullscreen game window may come back on top: take the top place again (checked once a second)
        if(IsIconic(hwnd)) return;
        const UINT w=UINT(std::max(2,wantW.load())),h=UINT(std::max(2,wantH.load()));
        if(!rtv||w!=backW||h!=backH) {
            rtv.Reset();
            if(FAILED(chain->ResizeBuffers(1,w,h,DXGI_FORMAT_UNKNOWN,0))) return;
            ComPtr<ID3D11Texture2D> bb;if(FAILED(chain->GetBuffer(0,IID_PPV_ARGS(&bb)))) return;
            if(FAILED(device->CreateRenderTargetView(bb.Get(),nullptr,&rtv))) return;
            backW=w;backH=h;
        }
        const MirrorRect r=mirrorLayout(float(backW),float(backH),eyeW,eyeH,fill,centreY);
        ID3D11RenderTargetView* t=rtv.Get();c->OMSetRenderTargets(1,&t,nullptr);
        const float black[4]={0,0,0,1};c->ClearRenderTargetView(t,black);
        c->IASetInputLayout(nullptr);c->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        c->VSSetShader(vs.Get(),nullptr,0);c->PSSetShader(ps.Get(),nullptr,0);c->RSSetState(rs.Get());
        ID3D11SamplerState* s=smp.Get();c->PSSetSamplers(0,1,&s);
        D3D11_VIEWPORT vp{r.x,r.y,r.w,r.h,0,1};c->RSSetViewports(1,&vp);   // a viewport larger than the window is clipped by the target: that is the crop
        c->PSSetShaderResources(0,1,&eye);c->Draw(3,0);
        ID3D11ShaderResourceView* none=nullptr;c->PSSetShaderResources(0,1,&none);c->OMSetRenderTargets(0,nullptr,nullptr);c->RSSetState(nullptr);
        chain->Present(0,DXGI_PRESENT_DO_NOT_WAIT);   // never blocks the headset loop; a busy present is simply skipped
    }
};
}
