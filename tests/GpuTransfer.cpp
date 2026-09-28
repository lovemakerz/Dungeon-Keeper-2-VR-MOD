// Real D3D9Ex (x86) -> shared texture -> D3D11 (x64) integration test.
#include "../src/common/Ipc.hpp"
#include <d3d9.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <cstdio>
#include <string>
#include <filesystem>
#include <vector>
using Microsoft::WRL::ComPtr;
using namespace dk2vr;
void must(bool c,const char* s) {if(!c) {printf("FAIL %s (Win32=%lu)\n",s,GetLastError());exit(1);}}
int wmain(int argc,wchar_t** argv) {
#if !defined(_WIN64)
    must(argc==2,"token argument");Ipc ipc;must(ipc.open(argv[1],false),"open IPC");
    HWND window=CreateWindowExW(0,L"STATIC",L"DK2VR GPU test",WS_OVERLAPPEDWINDOW,0,0,128,128,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
    must(window!=nullptr,"create hidden test window");
    ComPtr<IDirect3D9Ex> api;must(SUCCEEDED(Direct3DCreate9Ex(D3D_SDK_VERSION,&api)),"Direct3DCreate9Ex");
    UINT adapter=0;bool found=false;
    for(UINT a=0;a<api->GetAdapterCount();++a) {LUID id{};api->GetAdapterLUID(a,&id);if(id.LowPart==ipc.data->request.adapterLow&&id.HighPart==ipc.data->request.adapterHigh) {adapter=a;found=true;break;}}
    must(found,"matching GPU adapter");
    D3DPRESENT_PARAMETERS pp{};pp.Windowed=TRUE;pp.SwapEffect=D3DSWAPEFFECT_DISCARD;pp.BackBufferFormat=D3DFMT_X8R8G8B8;pp.BackBufferWidth=pp.BackBufferHeight=64;pp.hDeviceWindow=window;
    ComPtr<IDirect3DDevice9Ex> d;must(SUCCEEDED(api->CreateDeviceEx(adapter,D3DDEVTYPE_HAL,window,D3DCREATE_HARDWARE_VERTEXPROCESSING,&pp,nullptr,&d)),"CreateDeviceEx");
    ComPtr<IDirect3DTexture9> texture;ComPtr<IDirect3DSurface9> surface;HANDLE shared=nullptr;
    must(SUCCEEDED(d->CreateTexture(64,64,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&texture,&shared)),"shared D3D9 texture");
    texture->GetSurfaceLevel(0,&surface);d->SetRenderTarget(0,surface.Get());
    must(SUCCEEDED(d->Clear(0,nullptr,D3DCLEAR_TARGET,0xff12ab34,1,0)),"clear test color");
    ComPtr<IDirect3DQuery9> fence;d->CreateQuery(D3DQUERYTYPE_EVENT,&fence);fence->Issue(D3DISSUE_END);
    auto deadline=GetTickCount64()+3000;HRESULT hr;
    do {hr=fence->GetData(nullptr,0,D3DGETDATA_FLUSH);if(hr==S_FALSE) Sleep(1);}while(hr==S_FALSE&&GetTickCount64()<deadline);
    must(hr==S_OK,"producer GPU fence");
    ipc.data->slots[0].handles[0]=reinterpret_cast<uintptr_t>(shared);InterlockedExchange(&ipc.data->slots[0].state,2);SetEvent(ipc.ready);
    deadline=GetTickCount64()+10000;
    while(ipc.data->slots[0].state!=0&&GetTickCount64()<deadline) Sleep(5);
    must(ipc.data->slots[0].state==0,"consumer acknowledgement");
    DestroyWindow(window);printf("PASS x86 producer\n");return 0;
#else
    must(argc==2,"path to x86 producer");Ipc ipc;auto token=L"test_"+std::to_wstring(GetCurrentProcessId());must(ipc.open(token,true),"create IPC");
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
    must(SUCCEEDED(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,D3D11_CREATE_DEVICE_BGRA_SUPPORT,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context)),"D3D11CreateDevice");
    ComPtr<IDXGIDevice> dxgi;device.As(&dxgi);ComPtr<IDXGIAdapter> adapter;dxgi->GetAdapter(&adapter);DXGI_ADAPTER_DESC desc{};adapter->GetDesc(&desc);
    ipc.data->request.adapterLow=desc.AdapterLuid.LowPart;ipc.data->request.adapterHigh=desc.AdapterLuid.HighPart;
    std::wstring command=L"\""+std::wstring(argv[1])+L"\" "+token;STARTUPINFOW si{sizeof(si)};si.dwFlags=STARTF_USESHOWWINDOW;si.wShowWindow=SW_HIDE;PROCESS_INFORMATION pi{};
    must(CreateProcessW(argv[1],command.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW,nullptr,nullptr,&si,&pi)!=0,"launch x86 producer");CloseHandle(pi.hThread);
    must(WaitForSingleObject(ipc.ready,10000)==WAIT_OBJECT_0,"wait for shared texture");
    ComPtr<ID3D11Texture2D> shared;must(SUCCEEDED(device->OpenSharedResource(reinterpret_cast<HANDLE>(uintptr_t(ipc.data->slots[0].handles[0])),IID_PPV_ARGS(&shared))),"cross-process OpenSharedResource");
    ComPtr<ID3D11ShaderResourceView> srv;must(SUCCEEDED(device->CreateShaderResourceView(shared.Get(),nullptr,&srv)),"shared texture is shader-readable");
    D3D11_TEXTURE2D_DESC td{};shared->GetDesc(&td);td.Usage=D3D11_USAGE_STAGING;td.BindFlags=0;td.CPUAccessFlags=D3D11_CPU_ACCESS_READ;td.MiscFlags=0;
    ComPtr<ID3D11Texture2D> staging;must(SUCCEEDED(device->CreateTexture2D(&td,nullptr,&staging)),"create staging");
    context->CopyResource(staging.Get(),shared.Get());D3D11_MAPPED_SUBRESOURCE mapped{};must(SUCCEEDED(context->Map(staging.Get(),0,D3D11_MAP_READ,0,&mapped)),"map readback");
    uint32_t pixel=*static_cast<const uint32_t*>(mapped.pData);context->Unmap(staging.Get(),0);
    must(pixel==0xff12ab34,"exact pixel transferred x86 D3D9 -> x64 D3D11");
    InterlockedExchange(&ipc.data->slots[0].state,0);must(WaitForSingleObject(pi.hProcess,5000)==WAIT_OBJECT_0,"producer exits");DWORD code=1;GetExitCodeProcess(pi.hProcess,&code);CloseHandle(pi.hProcess);must(code==0,"producer success");
    printf("PASS cross-process D3D9Ex x86 -> D3D11 x64: exact BGRA pixel %08X, shared SRV, IPC synchronization.\n",pixel);return 0;
#endif
}
