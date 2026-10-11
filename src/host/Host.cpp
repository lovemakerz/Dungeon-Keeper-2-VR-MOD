#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>
#include <wincodec.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <wrl/client.h>
#include "../common/Ipc.hpp"
#include "../common/Math.hpp"
#include "../common/Grab.hpp"
#include "../common/TableModel.hpp"
#include "../common/HudLayout.hpp"
#include "../common/Comfort.hpp"
#include "../common/Keyboard.hpp"
#include "../common/TableTilt.hpp"
#include "../common/IniRepair.hpp"
#include "MirrorWindow.hpp"
#include <filesystem>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdio>
#include <cstdarg>
#include <share.h>
#include <algorithm>
#include <array>
#include <chrono>
using Microsoft::WRL::ComPtr;
using namespace dk2vr;
namespace fs=std::filesystem;
namespace {
FILE* logfile=nullptr;
void log(const char* fmt,...) {
    va_list a;va_start(a,fmt);va_list b;va_copy(b,a);
    vprintf(fmt,a);printf("\n");fflush(stdout);
    if(logfile) {fprintf(logfile,"[%llu] ",GetTickCount64());vfprintf(logfile,fmt,b);fputc('\n',logfile);fflush(logfile);}
    va_end(b);va_end(a);
}
void check(XrResult r,const char* where) {if(XR_FAILED(r)) throw std::runtime_error(std::string(where)+" : OpenXR "+std::to_string(r));}
void dx(HRESULT r,const char* where) {if(FAILED(r)) {char b[30];sprintf_s(b,"0x%08lX",r);throw std::runtime_error(std::string(where)+" : "+b);}}
XrPosef xrPose(const Pose& p) {return {{p.orientation.x,p.orientation.y,p.orientation.z,p.orientation.w},{p.position.x,p.position.y,p.position.z}};}
Pose dk2vrPose(const XrPosef& p) {return {{p.orientation.x,p.orientation.y,p.orientation.z,p.orientation.w},{p.position.x,p.position.y,p.position.z}};}
XrFovf xrFov(const Fov& f) {return {f.left,f.right,f.up,f.down};}
Fov fov(const XrFovf& f) {return {f.angleLeft,f.angleRight,f.angleUp,f.angleDown};}
bool key(int code) {static bool previous[256]{};bool now=(GetAsyncKeyState(code)&0x8000)!=0;bool edge=now&&!previous[code];previous[code]=now;return edge;}
float setting(const fs::path& ini,const wchar_t* name,float value,float min,float max) {
    wchar_t buffer[64]{};GetPrivateProfileStringW(L"VR",name,L"",buffer,64,ini.c_str());
    if(!buffer[0]) return value;
    wchar_t* end=nullptr;float parsed=wcstof(buffer,&end);
    return end!=buffer&&std::isfinite(parsed)?std::clamp(parsed,min,max):value;
}
// 0.8.86: the options chosen on the VR panel live in a per-user file (%APPDATA%\DK2VR\UserSettings.ini) that survives new version folders,
// new unzips and the launchers' resets. pref() reads it first and falls back to DK2VR.ini; userSave() writes both.
fs::path gUserSettings;
fs::path userSettingsPath(const fs::path& ini) {
    wchar_t base[MAX_PATH]{};DWORD n=GetEnvironmentVariableW(L"APPDATA",base,MAX_PATH);
    fs::path dir=n&&n<MAX_PATH?fs::path(base)/L"DK2VR":ini.parent_path();
    std::error_code ec;fs::create_directories(dir,ec);
    return dir/L"UserSettings.ini";
}
float pref(const fs::path& ini,const wchar_t* name,float value,float min,float max) {
    if(!gUserSettings.empty()) {
        wchar_t buffer[64]{};GetPrivateProfileStringW(L"VR",name,L"",buffer,64,gUserSettings.c_str());
        if(buffer[0]) {wchar_t* end=nullptr;float parsed=wcstof(buffer,&end);if(end!=buffer&&std::isfinite(parsed)) return std::clamp(parsed,min,max);}
    }
    return setting(ini,name,value,min,max);
}
void userSave(const fs::path& ini,const wchar_t* key,const wchar_t* value) {
    WritePrivateProfileStringW(L"VR",key,value,ini.c_str());
    if(!gUserSettings.empty()) WritePrivateProfileStringW(L"VR",key,value,gUserSettings.c_str());
}
// Captures (F11): numbered PNG files in Captures\, never overwritten. BMP fallback if WIC fails.
bool savePng(const fs::path& path,UINT w,UINT h,UINT pitch,const void* pixels) {
    ComPtr<IWICImagingFactory> factory;ComPtr<IWICStream> stream;ComPtr<IWICBitmapEncoder> encoder;ComPtr<IWICBitmapFrameEncode> frame;
    if(FAILED(CoCreateInstance(CLSID_WICImagingFactory,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&factory)))) return false;
    if(FAILED(factory->CreateStream(&stream))||FAILED(stream->InitializeFromFilename(path.c_str(),GENERIC_WRITE))) return false;
    if(FAILED(factory->CreateEncoder(GUID_ContainerFormatPng,nullptr,&encoder))||FAILED(encoder->Initialize(stream.Get(),WICBitmapEncoderNoCache))) return false;
    if(FAILED(encoder->CreateNewFrame(&frame,nullptr))||FAILED(frame->Initialize(nullptr))||FAILED(frame->SetSize(w,h))) return false;
    // 0.2.5: SetPixelFormat may substitute another format (Windows turned 32bppBGR into 24bpp -> striped
    // grey captures in 0.2.4). Use BGRA, which PNG supports natively, force opaque alpha, and verify.
    WICPixelFormatGUID format=GUID_WICPixelFormat32bppBGRA;
    if(FAILED(frame->SetPixelFormat(&format))||format!=GUID_WICPixelFormat32bppBGRA) return false;
    std::vector<BYTE> opaque(size_t(w)*h*4);
    for(UINT y=0;y<h;++y) {
        const BYTE* src=static_cast<const BYTE*>(pixels)+size_t(y)*pitch;BYTE* dst=opaque.data()+size_t(y)*w*4;
        memcpy(dst,src,size_t(w)*4);for(UINT x=0;x<w;++x) dst[x*4+3]=255;
    }
    if(FAILED(frame->WritePixels(h,w*4,UINT(opaque.size()),opaque.data()))) return false;
    return SUCCEEDED(frame->Commit())&&SUCCEEDED(encoder->Commit());
}
// 0.6.9.3 : captures en JPEG (qualite 0,88), les yeux > 2048 px reduits de moitie (moyenne 2x2),
// pour que le rapport tienne sous la limite d'envoi (~30 Mo). Repli PNG puis BMP si WIC echoue.
bool saveJpg(const fs::path& path,UINT w,UINT h,UINT pitch,const void* pixels) {
    const UINT f=w>2048?2:1;const UINT ow=w/f,oh=h/f;
    std::vector<BYTE> rgb(size_t(ow)*oh*3);
    for(UINT y=0;y<oh;++y) for(UINT x=0;x<ow;++x) {
        UINT s[3]{};
        for(UINT dy=0;dy<f;++dy) for(UINT dx2=0;dx2<f;++dx2) {
            const BYTE* p=static_cast<const BYTE*>(pixels)+size_t(y*f+dy)*pitch+size_t(x*f+dx2)*4;
            s[0]+=p[0];s[1]+=p[1];s[2]+=p[2];
        }
        BYTE* d=rgb.data()+(size_t(y)*ow+x)*3;for(int c=0;c<3;++c) d[c]=BYTE(s[c]/(f*f));
    }
    ComPtr<IWICImagingFactory> factory;ComPtr<IWICStream> stream;ComPtr<IWICBitmapEncoder> encoder;ComPtr<IWICBitmapFrameEncode> frame;ComPtr<IPropertyBag2> props;
    if(FAILED(CoCreateInstance(CLSID_WICImagingFactory,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&factory)))) return false;
    if(FAILED(factory->CreateStream(&stream))||FAILED(stream->InitializeFromFilename(path.c_str(),GENERIC_WRITE))) return false;
    if(FAILED(factory->CreateEncoder(GUID_ContainerFormatJpeg,nullptr,&encoder))||FAILED(encoder->Initialize(stream.Get(),WICBitmapEncoderNoCache))) return false;
    if(FAILED(encoder->CreateNewFrame(&frame,&props))) return false;
    PROPBAG2 option{};option.pstrName=const_cast<LPOLESTR>(L"ImageQuality");
    VARIANT value{};value.vt=VT_R4;value.fltVal=0.88f;props->Write(1,&option,&value);
    if(FAILED(frame->Initialize(props.Get()))||FAILED(frame->SetSize(ow,oh))) return false;
    WICPixelFormatGUID format=GUID_WICPixelFormat24bppBGR;
    if(FAILED(frame->SetPixelFormat(&format))||format!=GUID_WICPixelFormat24bppBGR) return false;
    if(FAILED(frame->WritePixels(oh,ow*3,UINT(rgb.size()),rgb.data()))) return false;
    return SUCCEEDED(frame->Commit())&&SUCCEEDED(encoder->Commit());
}
bool saveBmp(const fs::path& path,UINT w,UINT h,UINT pitch,const void* pixels) {
    FILE* f=nullptr;_wfopen_s(&f,path.c_str(),L"wb");if(!f) return false;
    BITMAPFILEHEADER header{};BITMAPINFOHEADER info{};
    header.bfType=0x4d42;header.bfOffBits=sizeof(header)+sizeof(info);header.bfSize=header.bfOffBits+w*h*4;
    info.biSize=sizeof(info);info.biWidth=LONG(w);info.biHeight=-LONG(h);info.biPlanes=1;info.biBitCount=32;info.biCompression=BI_RGB;
    fwrite(&header,sizeof(header),1,f);fwrite(&info,sizeof(info),1,f);
    for(UINT y=0;y<h;++y) fwrite(static_cast<const char*>(pixels)+size_t(y)*pitch,4,w,f);
    fclose(f);return true;
}
// 0.9.47.37: picture file (JPEG / PNG) decoded to 32-bit pixels, BGRA or RGBA, for the start-up picture of the headset.
bool loadPicture(const fs::path& path,bool bgra,uint32_t& w,uint32_t& h,std::vector<BYTE>& out) {
    ComPtr<IWICImagingFactory> factory;ComPtr<IWICBitmapDecoder> decoder;ComPtr<IWICBitmapFrameDecode> frame;ComPtr<IWICFormatConverter> conv;
    if(FAILED(CoCreateInstance(CLSID_WICImagingFactory,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&factory)))) return false;
    if(FAILED(factory->CreateDecoderFromFilename(path.c_str(),nullptr,GENERIC_READ,WICDecodeMetadataCacheOnDemand,&decoder))) return false;
    if(FAILED(decoder->GetFrame(0,&frame))||FAILED(factory->CreateFormatConverter(&conv))) return false;
    if(FAILED(conv->Initialize(frame.Get(),bgra?GUID_WICPixelFormat32bppBGRA:GUID_WICPixelFormat32bppRGBA,WICBitmapDitherTypeNone,nullptr,0,WICBitmapPaletteTypeCustom))) return false;
    if(FAILED(conv->GetSize(&w,&h))||w<16||h<16||w>4096||h>4096) return false;
    out.resize(size_t(w)*h*4);
    return SUCCEEDED(conv->CopyPixels(nullptr,w*4,UINT(out.size()),out.data()));
}
struct Swapchain {
    XrSwapchain handle=XR_NULL_HANDLE;uint32_t width=0,height=0;
    std::vector<XrSwapchainImageD3D11KHR> images;
    std::vector<ComPtr<ID3D11RenderTargetView>> targets;
    void destroy() {targets.clear();images.clear();if(handle) xrDestroySwapchain(handle);handle=XR_NULL_HANDLE;}
    ~Swapchain(){destroy();}
    void create(XrSession session,ID3D11Device* device,int64_t format,uint32_t w,uint32_t h) {
        if(handle&&width==w&&height==h) return;
        destroy();width=w;height=h;
        XrSwapchainCreateInfo ci{XR_TYPE_SWAPCHAIN_CREATE_INFO};
        ci.usageFlags=XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT|XR_SWAPCHAIN_USAGE_SAMPLED_BIT;ci.format=format;
        ci.sampleCount=1;ci.width=w;ci.height=h;ci.faceCount=ci.arraySize=ci.mipCount=1;
        check(xrCreateSwapchain(session,&ci,&handle),"xrCreateSwapchain");
        uint32_t n=0;check(xrEnumerateSwapchainImages(handle,0,&n,nullptr),"xrEnumerateSwapchainImages");
        images.assign(n,{XR_TYPE_SWAPCHAIN_IMAGE_D3D11_KHR});
        check(xrEnumerateSwapchainImages(handle,n,&n,reinterpret_cast<XrSwapchainImageBaseHeader*>(images.data())),"xrEnumerateSwapchainImages");
        targets.resize(n);
        D3D11_RENDER_TARGET_VIEW_DESC rtv{};rtv.Format=static_cast<DXGI_FORMAT>(format);rtv.ViewDimension=D3D11_RTV_DIMENSION_TEXTURE2D;
        for(uint32_t i=0;i<n;++i) dx(device->CreateRenderTargetView(images[i].texture,&rtv,&targets[i]),"CreateRenderTargetView");
    }
};
struct Blitter {
    ComPtr<ID3D11VertexShader> vs;ComPtr<ID3D11PixelShader> ps;
    ComPtr<ID3D11SamplerState> sampler;ComPtr<ID3D11Buffer> comfort;
    void create(ID3D11Device* d,bool srgb) {
        const char* vertex="struct V{float4 p:SV_Position;float2 uv:TEXCOORD;};V main(uint id:SV_VertexID){V v;v.uv=float2((id<<1)&2,id&2);v.p=float4(v.uv.x*2-1,1-v.uv.y*2,0,1);return v;}";
        const char* pixel="Texture2D t:register(t0);SamplerState s:register(s0);cbuffer Comfort:register(b0){float4 vignette;}float4 main(float4 p:SV_Position,float2 uv:TEXCOORD):SV_Target{float3 c=t.Sample(s,uv).rgb;\n#ifdef SRGB\nc=lerp(c/12.92,pow((c+.055)/1.055,2.4),step(.04045,c));\n#endif\nfloat2 q=(uv-.5)*float2(vignette.z,1);float f=saturate((length(q)-vignette.x)/max(.001,vignette.y-vignette.x));f=f*f*(3-2*f);c*=1-f*vignette.w;return float4(c,1);}";
        ComPtr<ID3DBlob> code,error;
        dx(D3DCompile(vertex,strlen(vertex),nullptr,nullptr,nullptr,"main","vs_4_0",0,0,&code,&error),"Compile vertex shader");
        dx(d->CreateVertexShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&vs),"CreateVertexShader");
        code.Reset();error.Reset();D3D_SHADER_MACRO macros[]={{"SRGB","1"},{nullptr,nullptr}};
        dx(D3DCompile(pixel,strlen(pixel),nullptr,srgb?macros:nullptr,nullptr,"main","ps_4_0",0,0,&code,&error),"Compile pixel shader");
        dx(d->CreatePixelShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&ps),"CreatePixelShader");
        D3D11_SAMPLER_DESC sd{};sd.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;sd.AddressU=sd.AddressV=sd.AddressW=D3D11_TEXTURE_ADDRESS_CLAMP;sd.MaxLOD=D3D11_FLOAT32_MAX;
        dx(d->CreateSamplerState(&sd,&sampler),"CreateSamplerState");
        D3D11_BUFFER_DESC cb{};cb.ByteWidth=16;cb.Usage=D3D11_USAGE_DEFAULT;cb.BindFlags=D3D11_BIND_CONSTANT_BUFFER;
        dx(d->CreateBuffer(&cb,nullptr,&comfort),"Create comfort parameters");
    }
    void copy(ID3D11DeviceContext* c,Swapchain& swap,ID3D11ShaderResourceView* source,uint32_t vignetteLevel=0,const fs::path& capturePath={}) {
        uint32_t index=0;XrSwapchainImageAcquireInfo ai{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
        check(xrAcquireSwapchainImage(swap.handle,&ai,&index),"xrAcquireSwapchainImage");
        XrSwapchainImageWaitInfo wi{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};wi.timeout=XR_INFINITE_DURATION;
        check(xrWaitSwapchainImage(swap.handle,&wi),"xrWaitSwapchainImage");
        ID3D11RenderTargetView* target=swap.targets[index].Get();c->OMSetRenderTargets(1,&target,nullptr);
        D3D11_VIEWPORT vp{0,0,float(swap.width),float(swap.height),0,1};c->RSSetViewports(1,&vp);
        c->IASetInputLayout(nullptr);c->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        c->VSSetShader(vs.Get(),nullptr,0);c->PSSetShader(ps.Get(),nullptr,0);
        const float params[4]={vignetteLevel==1?.35f:vignetteLevel==2?.25f:.14f,
            vignetteLevel==1?.52f:vignetteLevel==2?.48f:.41f,
            float(swap.width)/float(std::max(1u,swap.height)),
            vignetteLevel==1?.70f:vignetteLevel==2?.94f:vignetteLevel>=3?1.f:0.f};
        c->UpdateSubresource(comfort.Get(),0,nullptr,params,0,0);
        ID3D11Buffer* cb=comfort.Get();c->PSSetConstantBuffers(0,1,&cb);
        c->PSSetShaderResources(0,1,&source);ID3D11SamplerState* smp=sampler.Get();c->PSSetSamplers(0,1,&smp);
        c->Draw(3,0);ID3D11ShaderResourceView* empty=nullptr;c->PSSetShaderResources(0,1,&empty);
        c->OMSetRenderTargets(0,nullptr,nullptr);
        if(!capturePath.empty()) {
            D3D11_TEXTURE2D_DESC desc{};swap.images[index].texture->GetDesc(&desc);
            desc.Usage=D3D11_USAGE_STAGING;desc.BindFlags=0;desc.MiscFlags=0;desc.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
            ComPtr<ID3D11Device> dev;swap.images[index].texture->GetDevice(&dev);
            ComPtr<ID3D11Texture2D> staging;
            if(SUCCEEDED(dev->CreateTexture2D(&desc,nullptr,&staging))) {
                c->CopyResource(staging.Get(),swap.images[index].texture);
                D3D11_MAPPED_SUBRESOURCE mapped{};
                if(SUCCEEDED(c->Map(staging.Get(),0,D3D11_MAP_READ,0,&mapped))) {
                    const bool saved=saveJpg(capturePath,desc.Width,desc.Height,mapped.RowPitch,mapped.pData);
                    log("Capture compositeur casque: %ls (%s)",capturePath.c_str(),saved?"OK":"ECHEC");
                    c->Unmap(staging.Get(),0);
                }
            }
        }
        XrSwapchainImageReleaseInfo ri{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};check(xrReleaseSwapchainImage(swap.handle,&ri),"xrReleaseSwapchainImage");
    }
};
// OpenXR controllers: grip pose, squeeze (grab), trigger (reserved for gameplay), stick click (recenter).
struct Controllers {
    XrActionSet set=XR_NULL_HANDLE;
    XrAction pose=XR_NULL_HANDLE,squeeze=XR_NULL_HANDLE,trigger=XR_NULL_HANDLE,recenter=XR_NULL_HANDLE,aim=XR_NULL_HANDLE,secondary=XR_NULL_HANDLE,tertiary=XR_NULL_HANDLE,stick=XR_NULL_HANDLE,haptic=XR_NULL_HANDLE;float stickXY[2][2]{};
    XrSpace aimSpaces[2]{XR_NULL_HANDLE,XR_NULL_HANDLE};Pose aimPoses[2]{};bool aimValid[2]{},second[2]{},third[2]{};
    XrPath hand[2]{};XrSpace spaces[2]{XR_NULL_HANDLE,XR_NULL_HANDLE};
    bool ready=false;
    Pose poses[2]{};bool valid[2]{},grip[2]{},trig[2]{},click[2]{},previousClick[2]{};
    float squeezeValue[2]{},triggerValue[2]{};
    XrPath path(XrInstance instance,const char* text) {XrPath p=XR_NULL_PATH;xrStringToPath(instance,text,&p);return p;}
    XrAction action(const char* name,const char* label,XrActionType type) {
        XrActionCreateInfo ci{XR_TYPE_ACTION_CREATE_INFO};ci.actionType=type;
        strcpy_s(ci.actionName,name);strcpy_s(ci.localizedActionName,label);
        ci.countSubactionPaths=2;ci.subactionPaths=hand;
        XrAction a=XR_NULL_HANDLE;check(xrCreateAction(set,&ci,&a),"xrCreateAction");return a;
    }
    void suggest(XrInstance instance,const char* profile,std::initializer_list<std::pair<XrAction,const char*>> list,std::initializer_list<std::pair<XrAction,const char*>> exact={}) {
        std::vector<XrActionSuggestedBinding> bindings;
        for(auto& b:list) for(const char* side:{"left","right"}) {
            std::string full=std::string("/user/hand/")+side+b.second;
            bindings.push_back({b.first,path(instance,full.c_str())});
        }
        for(auto& b:exact) bindings.push_back({b.first,path(instance,b.second)});
        XrInteractionProfileSuggestedBinding sb{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
        sb.interactionProfile=path(instance,profile);sb.countSuggestedBindings=uint32_t(bindings.size());sb.suggestedBindings=bindings.data();
        XrResult r=xrSuggestInteractionProfileBindings(instance,&sb);
        log("Controleurs : profil %s %s (%d)",profile,XR_SUCCEEDED(r)?"pris en charge":"ignore par ce runtime",int(r));
    }
    void create(XrInstance instance,XrSession session) {
        XrActionSetCreateInfo si{XR_TYPE_ACTION_SET_CREATE_INFO};
        strcpy_s(si.actionSetName,"dk2vr");strcpy_s(si.localizedActionSetName,"Dungeon Keeper II VR");si.priority=0;
        check(xrCreateActionSet(instance,&si,&set),"xrCreateActionSet");
        hand[0]=path(instance,"/user/hand/left");hand[1]=path(instance,"/user/hand/right");
        pose=action("grip_pose","Position de la main",XR_ACTION_TYPE_POSE_INPUT);
        squeeze=action("grab","Saisir la table",XR_ACTION_TYPE_FLOAT_INPUT);
        trigger=action("trigger","Gachette",XR_ACTION_TYPE_FLOAT_INPUT);
        recenter=action("recenter","Recentrer",XR_ACTION_TYPE_BOOLEAN_INPUT);
        // 0.6.0: laser pointer (aim pose) and right click (A / X).
        aim=action("aim_pose","Pointeur",XR_ACTION_TYPE_POSE_INPUT);
        secondary=action("secondary","Clic droit",XR_ACTION_TYPE_BOOLEAN_INPUT);
        tertiary=action("tertiary","Capacite secondaire en possession",XR_ACTION_TYPE_BOOLEAN_INPUT);
        stick=action("scroll","Defiler la carte",XR_ACTION_TYPE_VECTOR2F_INPUT);
        haptic=action("haptic","Vibration",XR_ACTION_TYPE_VIBRATION_OUTPUT);   // 0.9.0: notches of the table lever
        suggest(instance,"/interaction_profiles/oculus/touch_controller",{{pose,"/input/grip/pose"},{haptic,"/output/haptic"},{squeeze,"/input/squeeze/value"},{trigger,"/input/trigger/value"},{recenter,"/input/thumbstick/click"},{aim,"/input/aim/pose"},{stick,"/input/thumbstick"}},{{secondary,"/user/hand/right/input/a/click"},{secondary,"/user/hand/left/input/x/click"},{tertiary,"/user/hand/right/input/b/click"},{tertiary,"/user/hand/left/input/y/click"}});
        suggest(instance,"/interaction_profiles/valve/index_controller",{{pose,"/input/grip/pose"},{haptic,"/output/haptic"},{squeeze,"/input/squeeze/value"},{trigger,"/input/trigger/value"},{recenter,"/input/thumbstick/click"},{aim,"/input/aim/pose"},{secondary,"/input/a/click"},{tertiary,"/input/b/click"},{stick,"/input/thumbstick"}});
        suggest(instance,"/interaction_profiles/htc/vive_controller",{{pose,"/input/grip/pose"},{haptic,"/output/haptic"},{squeeze,"/input/squeeze/click"},{trigger,"/input/trigger/value"},{recenter,"/input/trackpad/click"},{aim,"/input/aim/pose"},{secondary,"/input/menu/click"}});
        suggest(instance,"/interaction_profiles/microsoft/motion_controller",{{pose,"/input/grip/pose"},{haptic,"/output/haptic"},{squeeze,"/input/squeeze/click"},{trigger,"/input/trigger/value"},{recenter,"/input/thumbstick/click"},{aim,"/input/aim/pose"},{secondary,"/input/menu/click"},{stick,"/input/thumbstick"}});
        suggest(instance,"/interaction_profiles/khr/simple_controller",{{pose,"/input/grip/pose"},{haptic,"/output/haptic"},{trigger,"/input/select/click"},{recenter,"/input/menu/click"},{aim,"/input/aim/pose"}});
        for(int h=0;h<2;++h) {
            XrActionSpaceCreateInfo ci{XR_TYPE_ACTION_SPACE_CREATE_INFO};ci.action=pose;ci.subactionPath=hand[h];ci.poseInActionSpace.orientation.w=1;
            check(xrCreateActionSpace(session,&ci,&spaces[h]),"xrCreateActionSpace");
            ci.action=aim;check(xrCreateActionSpace(session,&ci,&aimSpaces[h]),"xrCreateActionSpace aim");
        }
        XrSessionActionSetsAttachInfo ai{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};ai.countActionSets=1;ai.actionSets=&set;
        check(xrAttachSessionActionSets(session,&ai),"xrAttachSessionActionSets");
        ready=true;log("Controleurs VR prets : une poignee = faire glisser le donjon, deux poignees = zoom (ecarter/rapprocher) + tourner/deplacer la table, clic du stick = recentrer.");
    }
    void profile(XrInstance instance,XrSession session) {
        for(int h=0;h<2;++h) {
            XrInteractionProfileState st{XR_TYPE_INTERACTION_PROFILE_STATE};
            if(XR_SUCCEEDED(xrGetCurrentInteractionProfile(session,hand[h],&st))&&st.interactionProfile!=XR_NULL_PATH) {
                char text[XR_MAX_PATH_LENGTH]{};uint32_t n=0;xrPathToString(instance,st.interactionProfile,XR_MAX_PATH_LENGTH,&n,text);
                log("Main %s : %s",h?"droite":"gauche",text);
            }
        }
    }
    void clear() {for(int h=0;h<2;++h) {valid[h]=grip[h]=trig[h]=click[h]=aimValid[h]=second[h]=third[h]=false;squeezeValue[h]=triggerValue[h]=0;}}
    void update(XrSession session,XrSpace base,XrTime time,bool focused) {
        for(int h=0;h<2;++h) previousClick[h]=click[h];
        if(!ready||!focused) {clear();return;}
        XrActiveActionSet active{set,XR_NULL_PATH};XrActionsSyncInfo sync{XR_TYPE_ACTIONS_SYNC_INFO};sync.countActiveActionSets=1;sync.activeActionSets=&active;
        if(XR_FAILED(xrSyncActions(session,&sync))) {clear();return;}
        for(int h=0;h<2;++h) {
            squeezeValue[h]=triggerValue[h]=0;click[h]=false;
            XrActionStateGetInfo gi{XR_TYPE_ACTION_STATE_GET_INFO};gi.subactionPath=hand[h];
            XrActionStateFloat f{XR_TYPE_ACTION_STATE_FLOAT};
            gi.action=squeeze;if(XR_SUCCEEDED(xrGetActionStateFloat(session,&gi,&f))&&f.isActive) squeezeValue[h]=f.currentState;
            f={XR_TYPE_ACTION_STATE_FLOAT};gi.action=trigger;if(XR_SUCCEEDED(xrGetActionStateFloat(session,&gi,&f))&&f.isActive) triggerValue[h]=f.currentState;
            XrActionStateBoolean b{XR_TYPE_ACTION_STATE_BOOLEAN};gi.action=recenter;
            if(XR_SUCCEEDED(xrGetActionStateBoolean(session,&gi,&b))&&b.isActive) click[h]=b.currentState;
            // Hysteresis avoids flicker around the half-pressed position.
            grip[h]=grip[h]?squeezeValue[h]>.35f:squeezeValue[h]>.65f;
            trig[h]=trig[h]?triggerValue[h]>.35f:triggerValue[h]>.65f;
            XrSpaceLocation loc{XR_TYPE_SPACE_LOCATION};
            const XrSpaceLocationFlags need=XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
            valid[h]=XR_SUCCEEDED(xrLocateSpace(spaces[h],base,time,&loc))&&(loc.locationFlags&need)==need;
            if(valid[h]) poses[h]=dk2vrPose(loc.pose); else grip[h]=false;
            XrSpaceLocation al{XR_TYPE_SPACE_LOCATION};
            aimValid[h]=XR_SUCCEEDED(xrLocateSpace(aimSpaces[h],base,time,&al))&&(al.locationFlags&need)==need;
            if(aimValid[h]) aimPoses[h]=dk2vrPose(al.pose);
            XrActionStateBoolean sb{XR_TYPE_ACTION_STATE_BOOLEAN};gi.action=secondary;second[h]=false;
            if(XR_SUCCEEDED(xrGetActionStateBoolean(session,&gi,&sb))&&sb.isActive) second[h]=sb.currentState;
            sb={XR_TYPE_ACTION_STATE_BOOLEAN};gi.action=tertiary;third[h]=false;
            if(XR_SUCCEEDED(xrGetActionStateBoolean(session,&gi,&sb))&&sb.isActive) third[h]=sb.currentState;
            XrActionStateVector2f v{XR_TYPE_ACTION_STATE_VECTOR2F};gi.action=stick;stickXY[h][0]=stickXY[h][1]=0;
            if(XR_SUCCEEDED(xrGetActionStateVector2f(session,&gi,&v))&&v.isActive) {stickXY[h][0]=v.currentState.x;stickXY[h][1]=v.currentState.y;}
        }
    }
    // 0.9.0: short vibration (lever notch). Silently nothing when the runtime has no haptics.
    void pulse(XrSession session,int h,float amplitude,float seconds) {
        if(!ready||haptic==XR_NULL_HANDLE||h<0||h>1) return;
        XrHapticVibration v{XR_TYPE_HAPTIC_VIBRATION};v.amplitude=amplitude;v.duration=XrDuration(seconds*1e9);v.frequency=XR_FREQUENCY_UNSPECIFIED;
        XrHapticActionInfo hi{XR_TYPE_HAPTIC_ACTION_INFO};hi.action=haptic;hi.subactionPath=hand[h];
        xrApplyHapticFeedback(session,&hi,reinterpret_cast<const XrHapticBaseHeader*>(&v));
    }
    bool recenterPressed(bool possessed) const {return click[1]&&!previousClick[1];}   // 0.9.47.36: the LEFT stick click opens the VR keyboard instead of recentring (F8 and the right stick click still recentre)
};
// 0.6.0: HUD modules. Each module is a quad layer that shows a rectangle of the game screen copy
// (same swapchain, different imageRect). Rectangles are fractions of the 4:3 game area inside the
// back buffer, measured on the 2560x1440 capture of 0.5.16 (game area 1920x1440 centred).
struct HudModule {const char* name;float u0,v0,u1,v1;};
constexpr HudModule HudModules[4]={
    {"barre du haut (vie, or)",0.f,.004f,.393f,.089f},
    {"barre des statistiques",.007f,.693f,.730f,.760f},
    {"minicarte",.007f,.760f,.183f,.990f},
    {"panneau creatures/sorts/salles",.183f,.760f,1.f,.990f}};
struct HudQuad {bool shown=false;Pose pose{};float w=0,h=0;XrRect2Di rect{};};
Quat quatFromAxisX(float degrees) {return pitch(degrees*Pi/180.f);}
struct Host {
    XrInstance instance=XR_NULL_HANDLE;XrSystemId system=0;XrSession session=XR_NULL_HANDLE;
    XrSpace space=XR_NULL_HANDLE;XrSessionState sessionState=XR_SESSION_STATE_UNKNOWN;
    XrEnvironmentBlendMode blend=XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    bool running=false,exit=false,centered=false,panel=false,menuScreen=false,gameMenuOpen=false,hudFrontPanel=false; // 0.8.88: menuScreen = the VR thread draws the menu room (game screen on the front wall)
    int32_t menuRect[4]{0,0,0,0}; // 0.8.88
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
    ComPtr<ID3D11Query> gpuFence;
    // 0.9.25: the shared VR image slot is handed back to the game DLL when the GPU copy is done, checked at the next
    // frames WITHOUT waiting in the frame loop (0.9.24 waited for the copy before xrEndFrame: a GPU wait inside every headset frame).
    bool asyncFence=true;int gpuPriorityHost=7;
    ComPtr<ID3D11Query> slotFence[SlotCount];bool slotPending[SlotCount]{};uint64_t slotPendingSince[SlotCount]{};
    // 0.9.25 PERF probe of the headset loop (QueryPerformanceCounter): phases, frames missed by the runtime, per scene.
    struct LoopPerf {
        double sum[5]{},worst[5]{};uint64_t frames=0,missed=0,released=0,stillPending=0;XrTime lastDisplay=0;XrDuration period=0;
        uint64_t modeFrames[4]{},modeMissed[4]{};double modeMs[4]{};
        void reset5() {for(int i=0;i<5;++i) sum[i]=worst[i]=0;frames=missed=released=stillPending=0;}
    } perf;
    LARGE_INTEGER perfFreq{};uint64_t perfLastBilan=0,perfMissedNow=0;std::string runtimeName="?";
    Swapchain eyes[2],desktop;Blitter blitter;Ipc ipc;
    int64_t colorFormat=0;Request request{};Slot last{};
    uint64_t lastReceived=0,frames=0,lastReport=0;uint32_t lastGeneration[SlotCount]{};
    // 0.3.0: wait briefly for the image of the pose just sent when the game DLL renders at the headset rate.
    int freshWaitMs=0;bool tableFixed=true;
    // 0.4.1: adaptive wait. If the VR image often arrives too late, waiting would make this loop miss the
    // headset's frame deadline (frame rate drop that stays): pause the wait for 3 s and log it.
    uint32_t windowTries=0,windowMisses=0;uint64_t freshPausedUntil=0,freshPauses=0,framesAtReport=0;double loopWorst=0;uint64_t slowLoops=0;bool gripBefore[2]{},holding[2]{};uint64_t freshHits=0,freshMisses=0,newImages=0;
    bool capture=false;unsigned captureCount=0;std::wstring captureStamp;uint64_t lastCaptureRequest=0;
    Controllers controllers;TableGrab grab;
    // 0.9.0 tilting table: tableBase = LEVEL pose (moved by the handles); request.table = tableBase tilted on its axle (TableTilt.hpp).
    Pose tableBase{{0,0,0,1},{0,0,0}};float tableTilt=0,tiltTarget=0,leverGrabAngle=0,leverGrabTilt=0;int leverHand=-1,leverNotch=0;bool leverGripBefore[2]{};
    // Table (Demeo style) and panel placement, from DK2VR.ini.
    float tableTopHeight=0.f,placedHeight=.50f;bool floorPlaced=false;
    float tableDistance=.80f,tableHeight=.40f,tableWidth=.80f,tableAspect=.75f;
    XrSpace stageSpace=XR_NULL_HANDLE;bool floorLogged=false;
    int panelPosition=1;float panelDistance=2.2f,panelHeight=.35f,panelWidth=1.8f;
    bool useControllers=true;
    // 0.6.0 HUD and pointer (DK2VR.ini: Hud, LeftHanded, MinimapMode, HudBottomPlace, HudWallWidth...)
    MirrorWindow mirror;bool mirrorOn=false,mirrorFill=true;int mirrorEye=0;float mirrorCentreY=.5f;   // 0.9.47.61: PC mirror window (OPTIONS > PC MIRROR, or MirrorWindow=1)
    bool hud=true,leftHanded=false,hideOrb=false,wallRight=false,ctlOpen=false,ctlFrench=false;uint64_t worldSeenTick=0;bool autoRecentered=false;int minimapMode=0,bottomPlace=0;
    bool panelOpen=false,snapTurn=false,wasPossessed=false,snapLatched=false,comfortBefore=false;
    // 0.9.47.34: VR virtual keyboard. B / Y of the pointing hand opens and closes it; the trigger presses a key (Keyboard.hpp).
    // 0.9.47.37: start-up picture (Textures\\demarrage.jpg): shown on a quad of the headset from the launch until the game's first image arrives (before that the headset stayed black).
    XrCompositionLayerQuad splashLayer{XR_TYPE_COMPOSITION_LAYER_QUAD};Swapchain splash;bool splashTried=false,splashReady=false,gameShownOnce=false,splashFloating=false;   // 0.9.47.39: the picture is now on the wall screen of the room (game DLL); the floating image stays available with StartupPictureFloating=1
   float splashAspect=.75f;
    void prepareSplash() {
        if(splashTried) return;splashTried=true;
        try {
            uint32_t w=0,h=0;std::vector<BYTE> px;
            const bool bgra=colorFormat==DXGI_FORMAT_B8G8R8A8_UNORM||colorFormat==DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
            const fs::path file=configuration.parent_path()/L"Textures"/L"demarrage.jpg";
            if(!fs::exists(file)) {log("Image de demarrage : %ls absente, ecran noir comme avant",file.c_str());return;}
            if(!loadPicture(file,bgra,w,h,px)) {log("Image de demarrage : lecture impossible (%ls)",file.c_str());return;}
            D3D11_TEXTURE2D_DESC desc{};desc.Width=w;desc.Height=h;desc.MipLevels=desc.ArraySize=1;desc.Format=static_cast<DXGI_FORMAT>(colorFormat);desc.SampleDesc.Count=1;desc.Usage=D3D11_USAGE_DEFAULT;desc.BindFlags=D3D11_BIND_SHADER_RESOURCE;
            D3D11_SUBRESOURCE_DATA init{px.data(),w*4,0};ComPtr<ID3D11Texture2D> tex;
            dx(device->CreateTexture2D(&desc,&init,&tex),"Image de demarrage");
            splash.create(session,device.Get(),colorFormat,w,h);
            uint32_t index=0;XrSwapchainImageAcquireInfo ai{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};check(xrAcquireSwapchainImage(splash.handle,&ai,&index),"xrAcquireSwapchainImage");
            XrSwapchainImageWaitInfo wi{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};wi.timeout=XR_INFINITE_DURATION;check(xrWaitSwapchainImage(splash.handle,&wi),"xrWaitSwapchainImage");
            context->CopyResource(splash.images[index].texture,tex.Get());context->Flush();
            XrSwapchainImageReleaseInfo ri{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};check(xrReleaseSwapchainImage(splash.handle,&ri),"xrReleaseSwapchainImage");
            splashAspect=float(h)/float(w);splashReady=true;log("Image de demarrage : %ls %ux%u affichee dans le casque jusqu'a la premiere image du jeu",file.c_str(),w,h);
        } catch(const std::exception& e) {log("Image de demarrage : %s",e.what());}
    }
    bool kbOpen=false,kbShift=false,kbToggleBefore=false,kbCloseOnEnter=true;int kbHeld=-1;ULONGLONG kbHeldAt=0,kbLockUntil=0;Pose kbPose{{0,0,0,1},{0,0,0}};uint32_t kbTyped=0;
    uint32_t vignetteLevel=0,laserColour=0;bool invertTurn=false; // 0.8.85: vignette removed (always 0), laser colour 0..6, inverted map rotation
    bool passthroughVd=false;   // 0.8.94.4
    float passRgb[3]={0.f,255.f,0.f};uint32_t dragSlider=0;
    float tableSizePct=100.f;   // 0.9.35: TABLE SIZE slider (50..250 % of TableWidth), saved as TableSizePercent
    float zoomExtraPct=100.f;   // 0.8.94.15: extra zoom-out of the table, 0..100 % (ZoomOutExtra x100)   // 0.8.94.5: PASSTHROUGH VD colour (sliders 0..255), slider being dragged (0 none)
    // 0.8.94.5: light smoothing of the pointing ray (One Euro filter: strong at rest = hand tremor removed, almost none while moving = no lag).
    float smoothStrength=.5f;bool aimInit=false;Vec3 aimPosS{},aimDirS{0,0,-1},aimPosRaw{},aimDirRaw{0,0,-1},aimPosD{},aimDirD{};double aimTick=0;
    uint32_t comfortFlagsNow() const {return (snapTurn?ComfortSnapTurn:0u)|((laserColour<<ComfortLaserShift)&ComfortLaserMask)|(invertTurn?ComfortInvertTurn:0u)|(panelOpen?ComfortPanelOpen:0u)|(passthroughVd?ComfortPassthrough:0u)|(hideOrb?ComfortHideOrb:0u)|(wallRight?ComfortWallRight:0u)|(ctlOpen?ComfortControlsOpen:0u)|(ctlFrench?ComfortControlsFrench:0u)|(mirrorOn?ComfortMirrorOn:0u);}
   float possessionYaw=0.f,smoothTurnSpeed=120.f,snapDegrees=30.f;
    double possessionTick=0;int lastCompositedVignette=-1; // 0.8.69: milliseconds from QueryPerformanceCounter (GetTickCount64 advances in 15.6 ms steps = irregular turn)
    uint32_t sparsePossessionHold=0;uint64_t sparsePossessionSkipped=0;
    // 0.8.67: possession turn prediction. The image is drawn by the game at its own rate; the part of the stick turn it does not contain yet
    // is applied here, every headset frame, as a rotation of the layer pose about the vertical axis through the head.
    bool possessionTurnPrediction=true;uint64_t turnPredFrames=0;double turnPredSum=0,turnPredMax=0;
    float hudWallWidth=1.4f,hudBottomWidth=.55f,hudMinimapWidth=.13f;
    HudQuad hudQuads[5];bool trigLatched=false,secondLatched=false,trigBefore=false,secondBefore=false;
    int pointerTarget=-1;uint64_t pointerClicks=0;
    fs::path outputDirectory;
    fs::path configuration;
    ComPtr<ID3D11Texture2D> sharedTextures[SlotCount][3];
    ComPtr<ID3D11ShaderResourceView> sharedViews[SlotCount][3];
    HANDLE gameProcess=nullptr;
    std::wstring token;
    ~Host() {
        if(controllers.set) {xrDestroyActionSet(controllers.set);controllers.set=XR_NULL_HANDLE;}
        if(ipc.data) InterlockedExchange(&ipc.data->hostAlive,0);
        if(gameProcess) CloseHandle(gameProcess);
        if(context) {context->ClearState();context->Flush();}
        desktop.destroy();for(auto& eye:eyes) eye.destroy();
        if(space) xrDestroySpace(space);
        if(session) xrDestroySession(session);
        if(instance) xrDestroyInstance(instance);
    }
    void initialize(const fs::path& ini,bool probe) {
        outputDirectory=ini.parent_path();
        configuration=ini;gUserSettings=userSettingsPath(ini);
        // 0.6.9.2: Virtual Desktop's own OpenXR runtime (VDXR) is preferred when it is installed: through SteamVR
        // the same Quest ran at 64 images/s (host loop > 8 ms every frame) instead of 90 (0.6.9 / 0.6.9.1 reports).
        // OpenXRRuntime=auto tries VDXR first and falls back to the system runtime if it has no headset.
        // 0.6.9.3: default is now "system" (user request: no runtime detection, plain OpenXR).
        wchar_t choice[32]{};GetPrivateProfileStringW(L"VR",L"OpenXRRuntime",L"system",choice,32,ini.c_str());
        std::wstring vdxr;
        if(_wcsicmp(choice,L"system")!=0&&!GetEnvironmentVariableW(L"XR_RUNTIME_JSON",nullptr,0)) {
            HKEY key=nullptr;
            if(RegOpenKeyExW(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Khronos\\OpenXR\\1\\AvailableRuntimes",0,KEY_READ|KEY_WOW64_64KEY,&key)==ERROR_SUCCESS) {
                for(DWORD i=0;;++i) {
                    wchar_t name[1024];DWORD len=1024;
                    if(RegEnumValueW(key,i,name,&len,nullptr,nullptr,nullptr,nullptr)!=ERROR_SUCCESS) break;
                    std::wstring lower(name);for(auto& c:lower) c=towlower(c);
                    if(lower.find(L"virtual")!=std::wstring::npos&&lower.find(L"desktop")!=std::wstring::npos&&lower.find(L"32")==std::wstring::npos&&fs::exists(name)) {vdxr=name;break;}
                }
                RegCloseKey(key);
            }
            if(vdxr.empty()) log("Runtime OpenXR : Virtual Desktop (VDXR) non installe, runtime du systeme utilise");
        }
        auto create=[&](){
            uint32_t n=0;check(xrEnumerateInstanceExtensionProperties(nullptr,0,&n,nullptr),"Aucun runtime OpenXR accessible. Connecter le casque puis selectionner OpenXR dans Virtual Desktop ou SteamVR");
            std::vector<XrExtensionProperties> ext(n,{XR_TYPE_EXTENSION_PROPERTIES});
            check(xrEnumerateInstanceExtensionProperties(nullptr,n,&n,ext.data()),"xrEnumerateInstanceExtensionProperties");
            bool found=false;for(auto& e:ext) if(!strcmp(e.extensionName,XR_KHR_D3D11_ENABLE_EXTENSION_NAME)) found=true;
            if(!found) throw std::runtime_error("Le runtime OpenXR ne propose pas D3D11.");
            const char* names[]={XR_KHR_D3D11_ENABLE_EXTENSION_NAME};
            XrInstanceCreateInfo ci{XR_TYPE_INSTANCE_CREATE_INFO};
            strcpy_s(ci.applicationInfo.applicationName,"Dungeon Keeper II VR");ci.applicationInfo.applicationVersion=209;
            strcpy_s(ci.applicationInfo.engineName,"DK2VR");ci.applicationInfo.engineVersion=200;
            ci.applicationInfo.apiVersion=XR_MAKE_VERSION(1,0,0);ci.enabledExtensionCount=1;ci.enabledExtensionNames=names;
            check(xrCreateInstance(&ci,&instance),"xrCreateInstance");
            XrInstanceProperties props{XR_TYPE_INSTANCE_PROPERTIES};check(xrGetInstanceProperties(instance,&props),"xrGetInstanceProperties");
            log("Runtime OpenXR : %s",props.runtimeName);runtimeName=props.runtimeName;
            XrSystemGetInfo si{XR_TYPE_SYSTEM_GET_INFO};si.formFactor=XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
            check(xrGetSystem(instance,&si,&system),"Casque non disponible (xrGetSystem)");
        };
        bool done=false;
        if(!vdxr.empty()) {
            SetEnvironmentVariableW(L"XR_RUNTIME_JSON",vdxr.c_str());
            try {create();done=true;log("Runtime OpenXR : Virtual Desktop (VDXR) choisi automatiquement (plus fluide que SteamVR ; OpenXRRuntime=system pour l'eviter)");}
            catch(const std::exception& e) {
                log("Runtime OpenXR : VDXR indisponible (%s), runtime du systeme utilise",e.what());
                if(instance) {xrDestroyInstance(instance);instance=XR_NULL_HANDLE;}
                system=XR_NULL_SYSTEM_ID;SetEnvironmentVariableW(L"XR_RUNTIME_JSON",nullptr);
            }
        }
        if(!done) create();
        XrSystemProperties sp{XR_TYPE_SYSTEM_PROPERTIES};check(xrGetSystemProperties(instance,system,&sp),"xrGetSystemProperties");
        log("Casque : %s; orientation=%u position=%u",sp.systemName,sp.trackingProperties.orientationTracking,sp.trackingProperties.positionTracking);
        if(!sp.trackingProperties.orientationTracking||!sp.trackingProperties.positionTracking) throw std::runtime_error("Le casque doit proposer le suivi de position et de rotation (6DOF).");
        PFN_xrGetD3D11GraphicsRequirementsKHR requirements=nullptr;
        check(xrGetInstanceProcAddr(instance,"xrGetD3D11GraphicsRequirementsKHR",reinterpret_cast<PFN_xrVoidFunction*>(&requirements)),"D3D11 requirements entry point");
        XrGraphicsRequirementsD3D11KHR req{XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR};check(requirements(instance,system,&req),"xrGetD3D11GraphicsRequirementsKHR");
        request.adapterLow=req.adapterLuid.LowPart;request.adapterHigh=req.adapterLuid.HighPart;
        ComPtr<IDXGIFactory1> factory;dx(CreateDXGIFactory1(IID_PPV_ARGS(&factory)),"CreateDXGIFactory1");
        ComPtr<IDXGIAdapter1> adapter,selected;
        for(UINT i=0;factory->EnumAdapters1(i,&adapter)!=DXGI_ERROR_NOT_FOUND;++i) {
            DXGI_ADAPTER_DESC1 desc{};dx(adapter->GetDesc1(&desc),"GetDesc1");
            if(desc.AdapterLuid.LowPart==req.adapterLuid.LowPart&&desc.AdapterLuid.HighPart==req.adapterLuid.HighPart) {selected=adapter;break;}
            adapter.Reset();
        }
        if(!selected) throw std::runtime_error("La carte graphique demandee par OpenXR est introuvable.");
        std::vector<D3D_FEATURE_LEVEL> levels;
        for(auto level:{D3D_FEATURE_LEVEL_11_1,D3D_FEATURE_LEVEL_11_0,D3D_FEATURE_LEVEL_10_1,D3D_FEATURE_LEVEL_10_0}) if(level>=req.minFeatureLevel) levels.push_back(level);
        if(levels.empty()) throw std::runtime_error("Niveau D3D11 demande non pris en charge.");
        dx(D3D11CreateDevice(selected.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,D3D11_CREATE_DEVICE_BGRA_SUPPORT,levels.data(),UINT(levels.size()),D3D11_SDK_VERSION,&device,nullptr,&context),"D3D11CreateDevice");
        if(probe) {log("Diagnostic OpenXR et carte graphique : OK.");return;}
        XrGraphicsBindingD3D11KHR binding{XR_TYPE_GRAPHICS_BINDING_D3D11_KHR};binding.device=device.Get();
        XrSessionCreateInfo sci{XR_TYPE_SESSION_CREATE_INFO};sci.next=&binding;sci.systemId=system;
        check(xrCreateSession(instance,&sci,&session),"xrCreateSession");
        XrReferenceSpaceCreateInfo rs{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};rs.referenceSpaceType=XR_REFERENCE_SPACE_TYPE_LOCAL;rs.poseInReferenceSpace.orientation.w=1;
        check(xrCreateReferenceSpace(session,&rs,&space),"xrCreateReferenceSpace LOCAL");
        // 0.4.0: STAGE space (floor at y=0) locates the real floor for the room and the table legs. Optional.
        {
            XrReferenceSpaceCreateInfo st{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};st.referenceSpaceType=XR_REFERENCE_SPACE_TYPE_STAGE;st.poseInReferenceSpace.orientation.w=1;
            if(XR_FAILED(xrCreateReferenceSpace(session,&st,&stageSpace))) {stageSpace=XR_NULL_HANDLE;log("Espace STAGE indisponible : hauteur du sol estimee.");}
        }
        uint32_t n=0;check(xrEnumerateEnvironmentBlendModes(instance,system,XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,0,&n,nullptr),"xrEnumerateEnvironmentBlendModes");
        std::vector<XrEnvironmentBlendMode> blends(n);check(xrEnumerateEnvironmentBlendModes(instance,system,XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,n,&n,blends.data()),"xrEnumerateEnvironmentBlendModes");
        if(std::find(blends.begin(),blends.end(),XR_ENVIRONMENT_BLEND_MODE_OPAQUE)==blends.end()) throw std::runtime_error("Cette version demande un casque VR avec composition opaque.");
        check(xrEnumerateViewConfigurationViews(instance,system,XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,0,&n,nullptr),"xrEnumerateViewConfigurationViews");
        if(n!=2) throw std::runtime_error("Deux vues stereoscopiques sont necessaires.");
        std::vector<XrViewConfigurationView> views(n,{XR_TYPE_VIEW_CONFIGURATION_VIEW});
        check(xrEnumerateViewConfigurationViews(instance,system,XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,n,&n,views.data()),"xrEnumerateViewConfigurationViews");
        float resolution=setting(ini,L"ResolutionScale",1.f,.25f,1.5f);
        const uint32_t maxDimension=uint32_t(setting(ini,L"MaxEyeDimension",4096,512,4096));
        uint32_t maximumW=std::min({views[0].maxImageRectWidth,views[1].maxImageRectWidth,maxDimension});
        uint32_t maximumH=std::min({views[0].maxImageRectHeight,views[1].maxImageRectHeight,maxDimension});
        request.width=std::clamp(uint32_t(std::max(views[0].recommendedImageRectWidth,views[1].recommendedImageRectWidth)*resolution),64u,maximumW);
        request.height=std::clamp(uint32_t(std::max(views[0].recommendedImageRectHeight,views[1].recommendedImageRectHeight)*resolution),64u,maximumH);
        request.unitsPerMeter=setting(ini,L"TableUnitsPerMeter",10,.1f,1000);
        request.possessionUnitsPerMeter=setting(ini,L"PossessionUnitsPerMeter",1,.05f,100);
        request.tablePitchDegrees=setting(ini,L"TablePitchDegrees",50,0,85);
        request.mode=uint32_t(setting(ini,L"Mode",0,0,3));
        snapTurn=pref(ini,L"PossessionSnapTurn",0,0,1)>.5f;
        vignetteLevel=0; // 0.8.85: the vignette comfort option is removed (PossessionVignette is ignored)
        laserColour=uint32_t(pref(ini,L"LaserColor",0,0,6)+.5f);invertTurn=pref(ini,L"InvertMapRotation",0,0,1)>.5f;passthroughVd=pref(ini,L"PassthroughVD",0,0,1)>.5f;passRgb[0]=pref(ini,L"PassthroughR",0,0,255);passRgb[1]=pref(ini,L"PassthroughG",255,0,255);passRgb[2]=pref(ini,L"PassthroughB",0,0,255);smoothStrength=setting(ini,L"PointerSmoothing",.5f,0,1);zoomExtraPct=std::clamp(pref(ini,L"ZoomOutExtra",1.f,0.f,4.f),0.f,1.f)*100.f;
        smoothTurnSpeed=setting(ini,L"PossessionTurnSpeed",120,30,300);
        possessionTurnPrediction=setting(ini,L"PossessionTurnPrediction",0,0,1)>.5f;
        snapDegrees=pref(ini,L"PossessionSnapAngle",30,15,90);
        readTableSettings(ini);tableSizePct=pref(ini,L"TableSizePercent",100,TableSizeMinPct,TableSizeMaxPct);request.tableWidthMeters=tableSizedWidth();
        tableTilt=tiltTarget=degToRad(pref(ini,L"TableTilt",0,0,TiltMaxDeg));request.tableTilt=tableTilt;request.leverFlags=0;   // 0.9.0: tilt kept from the last session
        useControllers=setting(ini,L"Controllers",1,0,1)>.5f;
        if(useControllers) {
            try {controllers.create(instance,session);}
            catch(const std::exception& e) {log("Controleurs indisponibles (%s). Le clavier et la souris restent utilisables.",e.what());}
        }
        check(xrEnumerateSwapchainFormats(session,0,&n,nullptr),"xrEnumerateSwapchainFormats");
        std::vector<int64_t> formats(n);check(xrEnumerateSwapchainFormats(session,n,&n,formats.data()),"xrEnumerateSwapchainFormats");
        for(int64_t f:{DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,DXGI_FORMAT_B8G8R8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM})
            if(std::find(formats.begin(),formats.end(),f)!=formats.end()) {colorFormat=f;break;}
        if(!colorFormat) throw std::runtime_error("Aucun format couleur compatible.");
        for(auto& eye:eyes) eye.create(session,device.Get(),colorFormat,request.width,request.height);
        blitter.create(device.Get(),colorFormat==DXGI_FORMAT_B8G8R8A8_UNORM_SRGB||colorFormat==DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
        D3D11_QUERY_DESC q{D3D11_QUERY_EVENT,0};dx(device->CreateQuery(&q,&gpuFence),"CreateQuery");
        // 0.9.25 performance: asynchronous slot release, GPU scheduling priority of this small copy device, process priority.
        asyncFence=setting(ini,L"HostAsyncFence",1,0,1)>.5f;
        for(auto& f:slotFence) dx(device->CreateQuery(&q,&f),"CreateQuery");
        gpuPriorityHost=int(std::lround(setting(ini,L"GpuPriorityHost",7,-7,7)));
        {
            ComPtr<IDXGIDevice> dxgi;HRESULT h=device.As(&dxgi);
            if(SUCCEEDED(h)&&gpuPriorityHost!=0) h=dxgi->SetGPUThreadPriority(gpuPriorityHost);
            log("Performance 0.9.25 : copie vers le casque %s ; priorite GPU de l'hote %+d (%s)",asyncFence?"sans attente dans la boucle (liberation a l'image suivante)":"avec attente (comme 0.9.24)",gpuPriorityHost,SUCCEEDED(h)?"OK":"refusee par le pilote");
        }
        if(setting(ini,L"HostPriority",1,0,1)>.5f) log("Performance 0.9.25 : priorite du processus hote %s",SetPriorityClass(GetCurrentProcess(),ABOVE_NORMAL_PRIORITY_CLASS)?"au-dessus de la normale":"inchangee (refusee)");
        QueryPerformanceFrequency(&perfFreq);
        token=std::to_wstring(GetCurrentProcessId())+L"_"+std::to_wstring(GetTickCount64());
        if(!ipc.open(token,true)) throw std::runtime_error("Creation du lien avec le jeu impossible.");
        ipc.data->request=request;InterlockedExchange(&ipc.data->hostAlive,1);
        log("Stereo : %ux%u par oeil. Table=%u largeur=%.2f m. F8 recentrer; F7 mode; F3 panneau; F9/F10 taille; Page haut/bas inclinaison (ancien mode); F5 relire reglages.",request.width,request.height,request.tableAnchor,request.tableWidthMeters);
    }
    void readTableSettings(const fs::path& ini) {
        request.tableAnchor=setting(ini,L"TableAnchor",1,0,1)>.5f?1u:0u;
        // GrabMode 2 (0.3.1, default): fixed table; one grip pans, two grips zoom and turn the dungeon (game camera).
        // GrabMode 1 (0.2.1): one grip pans the dungeon, two grips zoom the game camera and turn/move the table.
        // GrabMode 0 (0.2.0): one grip moves the table, two grips turn and scale it.
        int grabMode=int(setting(ini,L"GrabMode",2,0,2)+.5f);
        bool demeo=grabMode>=1;tableFixed=grabMode==2;
        grab.oneHandMoves=!demeo;grab.twoHandScales=!demeo;grab.oneHandTurns=tableFixed;
        tableDistance=setting(ini,L"TableDistance",.80f,.1f,3.f);
        tableHeight=setting(ini,L"TableHeight",.50f,-.5f,2.f);
        tableTopHeight=setting(ini,L"TableTopHeight",0.f,0.f,2.f);
        tableWidth=setting(ini,L"TableWidth",.80f,.2f,6.f);
        tableAspect=setting(ini,L"TableAspect",.75f,0.f,3.f);
        panelPosition=int(setting(ini,L"PanelPosition",1,0,1));
        panelDistance=setting(ini,L"PanelDistance",2.2f,.5f,10.f);
        panelHeight=setting(ini,L"PanelHeight",.35f,-2.f,3.f);
        panelWidth=setting(ini,L"PanelWidth",1.8f,.3f,8.f);
        freshWaitMs=int(setting(ini,L"FreshFrameWaitMs",0,0,10));
        hud=setting(ini,L"Hud",1,0,1)>.5f;kbCloseOnEnter=setting(ini,L"KeyboardCloseOnEnter",1,0,1)>.5f;leftHanded=pref(ini,L"LeftHanded",0,0,1)>.5f;hideOrb=pref(ini,L"HideLeftHandOrb",0,0,1)>.5f;mirrorOn=pref(ini,L"MirrorWindow",0,0,1)>.5f;mirrorEye=setting(ini,L"MirrorEye",0,0,1)>.5f?1:0;mirrorFill=setting(ini,L"MirrorFill",1,0,1)>.5f;mirror.onTop=setting(ini,L"MirrorOnTop",1,0,1)>.5f;mirrorCentreY=setting(ini,L"MirrorCentreY",.5f,0.f,1.f);ctlFrench=pref(ini,L"ControlsFrench",0,0,1)>.5f;
        minimapMode=int(setting(ini,L"MinimapMode",0,0,2)+.5f);bottomPlace=int(setting(ini,L"HudBottomPlace",0,0,1)+.5f);
        hudWallWidth=setting(ini,L"HudWallWidth",1.4f,.3f,5.f);hudBottomWidth=setting(ini,L"HudBottomWidth",.55f,.15f,2.f);
        hudMinimapWidth=setting(ini,L"HudMinimapWidth",.13f,.05f,.6f);
    }
    XrRect2Di hudRect(const HudModule& m,uint32_t W,uint32_t H) const {
        const float iw=std::min(float(W),H*4.f/3.f),ih=std::min(float(H),W*3.f/4.f),x0=(W-iw)*.5f,y0=(H-ih)*.5f;
        XrRect2Di r{};r.offset={int32_t(x0+m.u0*iw),int32_t(y0+m.v0*ih)};
        r.extent={std::max(1,int32_t((m.u1-m.u0)*iw)),std::max(1,int32_t((m.v1-m.v0)*ih))};return r;
    }
    // Places the modules (index 0..3) and the full game screen panel (index 4) for this frame.
    void placeHud(bool world,bool panelOn,const Pose& panelPose,float panelW,float panelH,const Pose& head) {
        const uint32_t W=last.desktopWidth,H=last.desktopHeight;
        for(auto& q:hudQuads) q.shown=false;
        if(W<64||H<64) return;
        auto set=[&](int i,const Pose& p,float width){
            HudQuad& q=hudQuads[i];q.rect=hudRect(HudModules[i],W,H);q.pose=p;q.w=width;q.h=width*q.rect.extent.height/float(q.rect.extent.width);q.shown=true;};
        if(panelOn) {HudQuad& q=hudQuads[4];q.shown=true;q.pose=panelPose;q.w=panelW;q.h=panelH;q.rect.offset={0,0};q.rect.extent={int32_t(W),int32_t(H)};}
        // 0.6.1: the HUD modules are drawn and pointed by the game DLL (transparent interface, depth).
    }
    // 0.8.94.5: One Euro filter on the aim ray (origin and direction). PointerSmoothing=0 disables it, 1 = strongest.
    void smoothAim(Vec3& pos,Vec3& dir) {
        if(smoothStrength<.01f||!std::isfinite(dir.x+dir.y+dir.z+pos.x+pos.y+pos.z)) {aimInit=false;return;}
        LARGE_INTEGER q{},f{};QueryPerformanceCounter(&q);QueryPerformanceFrequency(&f);
        const double now=double(q.QuadPart)*1000./double(f.QuadPart);
        const float dt=aimInit?std::clamp(float((now-aimTick)*.001),.002f,.1f):.011f;aimTick=now;
        auto alpha=[&](float cutoff){const float tau=1.f/(2.f*Pi*std::max(cutoff,.01f));return dt/(dt+tau);};
        auto lerp3=[](Vec3 a,Vec3 b,float t){return Vec3{a.x+(b.x-a.x)*t,a.y+(b.y-a.y)*t,a.z+(b.z-a.z)*t};};
        auto len=[](Vec3 a){return std::sqrt(a.x*a.x+a.y*a.y+a.z*a.z);};
        if(!aimInit||dot3(aimDirS,dir)<.8f||len(sub(pos,aimPosS))>.3f) {   // first frame / hand teleport: no smoothing
            aimPosS=pos;aimDirS=dir;aimPosRaw=pos;aimDirRaw=dir;aimPosD={};aimDirD={};aimInit=true;return;
        }
        const float a1=alpha(1.5f);
        aimDirD=lerp3(aimDirD,scale(sub(dir,aimDirRaw),1.f/dt),a1);aimPosD=lerp3(aimPosD,scale(sub(pos,aimPosRaw),1.f/dt),a1);
        aimDirRaw=dir;aimPosRaw=pos;
        const float minCut=3.2f-2.6f*std::clamp(smoothStrength,0.f,1.f);   // 1.9 Hz at the default strength .5, 0.6 Hz at 1
        const float cutDir=minCut+5.f*len(aimDirD),cutPos=minCut+25.f*len(aimPosD);
        aimDirS=lerp3(aimDirS,dir,alpha(cutDir));aimPosS=lerp3(aimPosS,pos,alpha(cutPos));
        const float l=len(aimDirS);if(l>1e-4f) aimDirS=scale(aimDirS,1.f/l);else aimDirS=dir;
        pos=aimPosS;dir=aimDirS;
    }
    // Laser from the aim pose of the pointing hand; the nearest module hit becomes the game cursor.
    void pointHud(bool possessed) {
        request.pointerFlags=(leftHanded?HudLeftHanded:0u)|(bottomPlace==1?HudBottomNear:0u)|(uint32_t(minimapMode)<<HudMinimapShift);
        request.rayLength=0;pointerTarget=-1;
        request.comfortHover=0;
        request.possessionMoveX=request.possessionMoveY=0;
        request.possessionTurn=possessionYaw;
        request.possessionFlags=0;request.possessionSkill=uint32_t(snapDegrees);
        if(mirror.takeClosed()&&mirrorOn) {mirrorOn=false;userSave(configuration,L"MirrorWindow",L"0");log("Options : fenetre miroir PC fermee par la croix = OFF");}   // 0.9.47.61
        request.comfortFlags=comfortFlagsNow();
        {   // 0.9.47.35 PROBE for automatic keyboard: which interface states change when a text field (save name) is clicked?
            static int lm=-1,lg=-1,lh=-1,lt=-1;static bool lk=false;
            const int m=menuScreen?1:0,g=gameMenuOpen?1:0,hf=hudFrontPanel?1:0;
            if(m!=lm||g!=lg||hf!=lh||kbOpen!=lk) {lm=m;lg=g;lh=hf;lk=kbOpen;log("Etat interface : menuScreen=%d gameMenuOpen=%d hudFrontPanel=%d clavier=%d",m,g,hf,kbOpen?1:0);}
        }
        request.keyboardFlags=0;request.keyboardPressed=0;request.keyboardHover=0;request.keyboardReserved=0;request.keyboardPose=kbPose;
        request.wheelPose.position={passRgb[0],passRgb[1],passRgb[2]};
        request.wheelPose.orientation.x=zoomExtraPct;   // 0.8.94.15
        request.wheelPose.orientation.y=tableSizePct;   // 0.9.35
        if(possessed) {
            if(kbOpen) log("Clavier VR : ferme (controle d'une creature)");
            kbOpen=false;kbHeld=-1;kbShift=false;kbToggleBefore=false;
            // Game keys are injected by the x86 bridge only while the DKII window is focused.
            auto dz=[](float v){float a=std::fabs(v);return (!std::isfinite(a)||a<.17f)?0.f:std::copysign(std::min(1.f,(a-.17f)/.83f),v);};
            request.possessionMoveX=dz(controllers.stickXY[0][0]);
            request.possessionMoveY=dz(controllers.stickXY[0][1]);
            const float turn=dz(controllers.stickXY[1][0]);
            // 0.8.69: the turn is integrated with the real elapsed time (GetTickCount64 has a 15.6 ms resolution: increments of 0 / 15.6 ms at 90 Hz
            // made the commanded turn itself lumpy, +-2 degrees at 130 deg/s, before the game even saw it).
            LARGE_INTEGER qNow{},qFreq{};QueryPerformanceCounter(&qNow);QueryPerformanceFrequency(&qFreq);
            const double now=double(qNow.QuadPart)*1000./double(qFreq.QuadPart);
            const float dt=possessionTick>0?std::clamp(float((now-possessionTick)*.001),0.f,.05f):0.f;possessionTick=now;
            if(snapTurn) {
                if(std::fabs(turn)<.35f) snapLatched=false;
                if(std::fabs(turn)>.65f&&!snapLatched) {possessionYaw+=std::copysign(snapDegrees*Pi/180.f,turn);snapLatched=true;}
            } else possessionYaw+=turn*smoothTurnSpeed*Pi/180.f*dt;
            request.possessionTurn=possessionYaw;
            if(controllers.trig[1]) request.possessionFlags|=PossessionAttack;
            if(controllers.second[1]) request.possessionFlags|=PossessionAbilityA;
            if(controllers.third[1]) request.possessionFlags|=PossessionAbilityB;
            if(controllers.second[0]) request.possessionFlags|=PossessionLeave;   // 0.9.47.39: left X leaves the possession
            if(controllers.click[0]) request.possessionFlags|=PossessionRun;
            request.stickX=request.stickY=request.turnStick=request.zoomStick=0;
            trigLatched=secondLatched=false;trigBefore=controllers.trig[1];secondBefore=controllers.second[1];
            return;
        }
        possessionTick=0;snapLatched=false;
        const int h=leftHanded?0:1;
        {   // 0.6.2: thumbstick of the other hand scrolls the map (dead zone 0.15, rescaled).
            float x=controllers.stickXY[1-h][0],y=controllers.stickXY[1-h][1],l=std::sqrt(x*x+y*y);
            if(!std::isfinite(l)||l<.15f) x=y=0; else {float k=std::min(1.f,(l-.15f)/.85f)/l;x*=k;y*=k;}
            request.stickX=x;request.stickY=y;
        }
        {   // 0.8.19: thumbstick of the pointing hand: left/right = rotate the dungeon, up/down = zoom (dead zone 0.2, rescaled).
            float x=controllers.stickXY[h][0],y=controllers.stickXY[h][1];
            const float ax=std::fabs(x),ay=std::fabs(y);
            auto dz=[](float v){float a=std::fabs(v);if(!std::isfinite(a)||a<.2f) return 0.f;return std::copysign(std::min(1.f,(a-.2f)/.8f),v);};
            // One axis at a time (the dominant one) so that a slightly tilted stick does not turn and zoom together.
            request.turnStick=ax>=ay?dz(x):0.f;request.zoomStick=ay>ax?dz(y):0.f;
            if(invertTurn) request.turnStick=-request.turnStick; // 0.8.85: inverted map rotation (stick of the pointing hand)
        }
        const bool trig=controllers.trig[h],second=controllers.second[h];
        {   // 0.9.47.34 (0.9.47.36: left stick click): opens / closes the VR keyboard. Works in every state (table, game menu, save dialog).
            const bool b=(controllers.valid[0]||controllers.ready)&&controllers.click[0];   // 0.9.47.36: left stick click (was B / Y of the pointing hand)
            if(b&&!kbToggleBefore) {
                kbOpen=!kbOpen;kbHeld=-1;kbShift=false;
                if(kbOpen) {
                    const Vec3 head=scale(add(request.eyes[0].pose.position,request.eyes[1].pose.position),.5f);
                    kbPose=kbOpenPose(head,request.eyes[0].pose.orientation);
                    log("Clavier VR : ouvert (clic du stick gauche)");
                } else log("Clavier VR : ferme (clic du stick gauche), %u touche(s) tapee(s)",kbTyped);
            }
            kbToggleBefore=b;
            if(kbOpen) {
                const ULONGLONG now=GetTickCount64();
                if(kbHeld>=0&&!trig&&now-kbHeldAt>=ULONGLONG(KbMinHoldMs)) {   // key released (after its minimum hold)
                    KbKey keys[KbMaxKeys];const int n=kbLayout(keys);
                    const uint8_t kind=kbHeld<n?keys[kbHeld].kind:uint8_t(KbNormal);
                    if(kind!=KbShift) kbShift=false;   // one-shot SHIFT is used up by the key it modified
                    if(kind==KbEnter&&kbCloseOnEnter) {kbOpen=false;log("Clavier VR : ferme apres ENTREE, %u touche(s) tapee(s)",kbTyped);}
                    kbHeld=-1;kbLockUntil=now+ULONGLONG(KbMinHoldMs);
                }
                request.keyboardPose=kbPose;
                request.keyboardFlags=(kbOpen?KeyboardOpen:0u)|(kbShift?KeyboardShift:0u);
                request.keyboardPressed=kbHeld>=0?uint32_t(kbHeld)+1:0u;
            } else kbHeld=-1;
        }
        if(!controllers.aimValid[h]) {aimInit=false;dragSlider=0;trigLatched=secondLatched=false;trigBefore=trig;secondBefore=second;return;}
        Pose aim=controllers.aimPoses[h];Vec3 dir=rotate(aim.orientation,{0,0,-1});smoothAim(aim.position,dir);   // 0.8.94.5: smoothed ray (pointer, laser and options panel)
        request.pointerFlags|=PointerAimValid|(trig?PointerTriggerRaw:0u)|(second?PointerSecondRaw:0u);
        request.rayOrigin=aim.position;request.rayDirection=dir;
        // 0.8.85: the options panel is on the LEFT wall of the VR room (snap turn, laser colour, inverted map rotation, primary hand).
        // It never synthesizes a click in the original game's interface.
        if(kbOpen) {   // 0.9.47.34: the keyboard is in front of everything (menus, game panels, wall options): its keys take the laser first
            float t=0;const int idx=kbHit(kbPose,aim.position,dir,t);
            if(idx!=-1) {
                request.rayLength=t;request.pointerFlags|=PointerActive|PointerComfort;
                if(idx>=0) request.keyboardHover=uint32_t(idx)+1;
                const ULONGLONG now=GetTickCount64();
                if(trig&&!comfortBefore&&idx>=0&&kbHeld<0&&now>=kbLockUntil) {
                    KbKey keys[KbMaxKeys];kbLayout(keys);
                    const uint8_t kind=keys[idx].kind;
                    if(kind==KbClose) {kbOpen=false;kbShift=false;log("Clavier VR : ferme (touche FERMER), %u touche(s) tapee(s)",kbTyped);}
                    else if(kind==KbShift) {kbShift=!kbShift;kbLockUntil=now+ULONGLONG(KbMinHoldMs);}
                    else {kbHeld=idx;kbHeldAt=now;++kbTyped;if(kbTyped<=5) log("Clavier VR : touche %d (scancode 0x%02X) enfoncee",idx,unsigned(keys[idx].scan));}
                    request.keyboardFlags=(kbOpen?KeyboardOpen:0u)|(kbShift?KeyboardShift:0u);
                    request.keyboardPressed=kbHeld>=0?uint32_t(kbHeld)+1:0u;
                }
                comfortBefore=trig;trigBefore=trig;secondBefore=second;trigLatched=secondLatched=false;return;
            }
        }
        {
            if(dragSlider) {   // 0.8.94.5: slider held by the trigger: follows the laser even when it leaves the bar, never reaches the game
                if(trig&&!gameMenuOpen&&comfortPanelOpen(request)) {   // 0.9.47.41: the options panel also exists in the menu room

                    float v=0,td=0;
                    const bool zs=dragSlider==ComfortItemSliderZoom,ts=dragSlider==ComfortItemSliderTable;
                    if(comfortSliderValue(request,aim.position,dir,v,td,zs,ts)) {
                        if(ts) {tableSizePct=std::round(v);request.tableWidthMeters=tableSizedWidth();}
                        else if(zs) zoomExtraPct=std::round(v); else passRgb[dragSlider-ComfortItemSliderR]=std::round(v);
                        request.rayLength=td;
                    }
                    request.wheelPose.position={passRgb[0],passRgb[1],passRgb[2]};request.wheelPose.orientation.x=zoomExtraPct;request.wheelPose.orientation.y=tableSizePct;
                    request.comfortHover=dragSlider;request.pointerFlags|=PointerActive|PointerComfort;
                    comfortBefore=trig;trigBefore=trig;secondBefore=second;trigLatched=secondLatched=false;return;
                }
                {wchar_t value[16];
                 if(dragSlider==ComfortItemSliderTable) saveTableSize();
                 else if(dragSlider==ComfortItemSliderZoom) {swprintf(value,16,L"%.2f",zoomExtraPct*.01f);userSave(configuration,L"ZoomOutExtra",value);
                     log("Options : dezoom de la table %.0f %% (ZoomOutExtra=%.2f)",zoomExtraPct,zoomExtraPct*.01f);}
                 else {const wchar_t* keys[3]={L"PassthroughR",L"PassthroughG",L"PassthroughB"};
                     for(int c=0;c<3;++c) {swprintf(value,16,L"%.0f",passRgb[c]);userSave(configuration,keys[c],value);}
                     log("Options : couleur du passthrough R=%.0f G=%.0f B=%.0f",passRgb[0],passRgb[1],passRgb[2]);}}
                dragSlider=0;
            }
            float t=0;const uint32_t item=(gameMenuOpen||hudFrontPanel)?0u:comfortHit(request,aim.position,dir,t);   // 0.8.94.23: nor when a game panel is in front of it on the laser // 0.8.92: nor while the game's own options menu is open // 0.8.88: no options panel in the menu room
            if(item) {
                request.comfortHover=item;
                request.rayLength=t;request.pointerFlags|=PointerActive|PointerComfort;
                if(trig&&!comfortBefore&&item!=0xffffffffu) {
                    if(item==ComfortItemOpen) {panelOpen=!panelOpen;log("Options : panneau %s",panelOpen?"ouvert":"ferme");}
                    else if(item==ComfortItemClose) {panelOpen=false;log("Options : panneau ferme (croix)");}
                    auto save=[&](const wchar_t* key,const wchar_t* value){userSave(configuration,key,value);};
                    wchar_t value[16];
                    if(item==ComfortItemSnap) {cycleSnapTurn(snapTurn,snapDegrees);swprintf(value,16,L"%.0f",snapDegrees);save(L"PossessionSnapAngle",value);save(L"PossessionSnapTurn",snapTurn?L"1":L"0");log("Options : Snap Turn %s %.0f degres",snapTurn?"ON":"OFF",snapDegrees);}
                    else if(comfortIsSlider(item)) {dragSlider=item;float v=0,td=0;const bool zs=item==ComfortItemSliderZoom,ts=item==ComfortItemSliderTable;
                        if(comfortSliderValue(request,aim.position,dir,v,td,zs,ts)) {if(ts) {tableSizePct=std::round(v);request.tableWidthMeters=tableSizedWidth();} else if(zs) zoomExtraPct=std::round(v); else passRgb[item-ComfortItemSliderR]=std::round(v);}
                        request.wheelPose.position={passRgb[0],passRgb[1],passRgb[2]};request.wheelPose.orientation.x=zoomExtraPct;request.wheelPose.orientation.y=tableSizePct;}
                    else if(item==ComfortItemPassthrough) {passthroughVd=!passthroughVd;save(L"PassthroughVD",passthroughVd?L"1":L"0");log("Options : Passthrough VD %s (salle remplacee par du vert pur)",passthroughVd?"ON":"OFF");}
                    else if(item==ComfortItemInvert) {invertTurn=!invertTurn;save(L"InvertMapRotation",invertTurn?L"1":L"0");log("Options : rotation de la carte inversee %s",invertTurn?"ON":"OFF");}
                    else if(item==ComfortItemHandLeft||item==ComfortItemHandRight) {const bool wantLeft=item==ComfortItemHandLeft;if(wantLeft!=leftHanded) {leftHanded=wantLeft;save(L"LeftHanded",leftHanded?L"1":L"0");log("Options : main principale %s",leftHanded?"GAUCHE":"DROITE");}}
                    else if(item==ComfortItemMirror) {mirrorOn=!mirrorOn;save(L"MirrorWindow",mirrorOn?L"1":L"0");log("Options : fenetre miroir PC %s",mirrorOn?"ON":"OFF");}
                    else if(item==ComfortItemOrb) {hideOrb=!hideOrb;save(L"HideLeftHandOrb",hideOrb?L"1":L"0");log("Options : boule de la main libre %s",hideOrb?"MASQUEE au-dessus de la table":"TOUJOURS VISIBLE");}
                    else if(item>=ComfortItemColour&&item<ComfortItemColour+ComfortColours) {laserColour=item-ComfortItemColour;swprintf(value,16,L"%u",laserColour);save(L"LaserColor",value);log("Options : couleur du laser %u",laserColour);}
                    request.pointerFlags=(leftHanded?HudLeftHanded:0u)|(request.pointerFlags&~HudLeftHanded);
                    request.comfortFlags=comfortFlagsNow();
                    request.possessionSkill=uint32_t(snapDegrees);
                }
                comfortBefore=trig;trigBefore=trig;secondBefore=second;
                trigLatched=secondLatched=false;return;
            }
        }
        {   // 0.9.47.49: CONTROLS panel (controller guide) on the wall opposite to the options: button, close cross, English / Francais tabs. Never reaches the game.
            float t=0;const uint32_t item=(gameMenuOpen||hudFrontPanel)?0u:controlsHit(request,aim.position,dir,t,ControlsDungeonFit);
            if(item) {
                request.comfortHover=item;
                request.rayLength=t;request.pointerFlags|=PointerActive|PointerComfort;
                if(trig&&!comfortBefore&&item!=0xffffffffu) {
                    if(item==ControlsItemOpen) {ctlOpen=!ctlOpen;log("Controles : panneau %s",ctlOpen?"ouvert":"ferme");}
                    else if(item==ControlsItemClose) {ctlOpen=false;log("Controles : panneau ferme (croix)");}
                    else if(item==ControlsItemTabEn||item==ControlsItemTabFr) {const bool fr=item==ControlsItemTabFr;if(fr!=ctlFrench) {ctlFrench=fr;userSave(configuration,L"ControlsFrench",fr?L"1":L"0");log("Controles : langue %s",fr?"Francais":"English");}}
                    request.comfortFlags=comfortFlagsNow();
                }
                comfortBefore=trig;trigBefore=trig;secondBefore=second;
                trigLatched=secondLatched=false;return;
            }
        }
        comfortBefore=trig;
        float best=3.f;float fx=0,fy=0;
        for(int i=4;i<5;++i) {
            const HudQuad& q=hudQuads[i];if(!q.shown) continue;
            Vec3 o=toLocal(q.pose,aim.position),d=rotate(conjugate(q.pose.orientation),dir);
            if(d.z>-1e-4f||o.z<=0) continue; // from the front only
            float t=-o.z/d.z;if(t<=0||t>=best) continue;
            float x=o.x+d.x*t,y=o.y+d.y*t;
            if(std::fabs(x)>q.w*.5f||std::fabs(y)>q.h*.5f) continue;
            float u=x/q.w+.5f,v=.5f-y/q.h;
            fx=(q.rect.offset.x+u*q.rect.extent.width)/float(last.desktopWidth);
            fy=(q.rect.offset.y+v*q.rect.extent.height)/float(last.desktopHeight);
            best=t;pointerTarget=i;
        }
        const bool on=pointerTarget>=0;
        // A click starts only on a panel (press edge while pointing); it is released when leaving it.
        trigLatched=on&&trig&&(trigLatched||!trigBefore);secondLatched=on&&second&&(secondLatched||!secondBefore);
        if(trigLatched&&!trigBefore) ++pointerClicks;
        trigBefore=trig;secondBefore=second;
        request.rayLength=on?best:0.f; // host panel hit only; the DLL draws the laser
        if(on) {request.pointerX=fx;request.pointerY=fy;request.pointerFlags|=PointerActive|(trigLatched?PointerLeft:0u)|(secondLatched?PointerRight:0u);}
    }
    void placeTable() {
        // Centre of the dungeon floor, in front of and below the eyes at recenter time.
        // 0.4.6: with the real floor known (STAGE), the table top is TableTopHeight above the floor,
        // but at least 0.30 m below the eyes; otherwise TableHeight below the eyes.
        placedHeight=tableHeight;floorPlaced=false;
        if(tableTopHeight>0&&(request.handFlags&RequestFloorKnown)) {
            float eyesAboveFloor=request.anchor.position.y-request.floorY;
            placedHeight=std::max(.30f,eyesAboveFloor-tableTopHeight);floorPlaced=true;
        }
        tableBase=compose(request.anchor,Pose{{0,0,0,1},{0,-placedHeight,-tableDistance}});
        request.tableWidthMeters=tableSizedWidth();grab.release();applyTilt();
    }
    float tableSizedWidth() const {return std::clamp(tableWidth*tableSizePct*.01f,.2f,6.f);}
    void saveTableSize() {wchar_t value[16];swprintf(value,16,L"%.0f",tableSizePct);userSave(configuration,L"TableSizePercent",value);
        log("Options : taille de la table %.0f %% (largeur %.2f m)",tableSizePct,request.tableWidthMeters);}
    // 0.9.0: the pose sent to the game = the level table tilted on its axle towards the player (recenter yaw).
    void applyTilt() {
        request.table=tiltTable(tableBase,yawOnly(request.anchor.orientation),tableTilt);request.tableTilt=tableTilt;
    }
    void saveTilt() {
        wchar_t v[32];swprintf_s(v,L"%.1f",tableTilt*180.f/float(Pi));userSave(configuration,L"TableTilt",v);
    }
    // 0.9.0: dungeon floor lever beside the near corner. Grip on its handle and pull it towards you to tilt the table (0..45 deg),
    // push it away to bring it back level. A notch every 5 deg (magnet + short vibration); released, the table settles on the nearest notch.
    void updateLever(bool possessed) {
        request.leverFlags=0;
        const bool on=!possessed&&request.tableAnchor&&centered&&!menuScreen&&controllers.ready;
        const float notch=degToRad(TiltNotchDeg);
        if(!on) {
            if(leverHand>=0) {leverHand=-1;tiltTarget=std::round(tableTilt/notch)*notch;saveTilt();}
            for(bool& b:leverGripBefore) b=false;
        } else {
            const TableModel model=tableModel(request.tableWidthMeters,tableAspect);
            const float floorT=(request.handFlags&RequestFloorKnown)?request.floorY-tableBase.position.y:-.78f;
            const LeverRig rig=leverRig(model,floorT,leftHanded);
            const Pose stand{yawOnly(request.anchor.orientation),tableBase.position};
            const Vec3 knob=toWorld(stand,leverKnobLocal(rig,tableTilt));
            for(int h=0;h<2;++h) {
                const bool g=controllers.valid[h]&&controllers.grip[h];
                const Vec3 d=sub(controllers.poses[h].position,knob);
                const bool reach=controllers.valid[h]&&dot(d,d)<LeverGrabRadius*LeverGrabRadius;
                if(reach) request.leverFlags|=h?LeverHoverRight:LeverHoverLeft;
                if(leverHand<0&&g&&!leverGripBefore[h]&&reach&&!holding[h]) {
                    const Vec3 l=sub(toLocal(stand,controllers.poses[h].position),rig.pivot);
                    leverHand=h;leverGrabAngle=std::atan2(l.z,l.y);leverGrabTilt=tableTilt;leverNotch=int(std::floor(tableTilt/notch+.5f));
                    controllers.pulse(session,h,.35f,.02f);
                    log("Levier de la table saisi (main %s), inclinaison %.0f degres",h?"droite":"gauche",tableTilt*180.f/float(Pi));
                }
                leverGripBefore[h]=g;
            }
            if(leverHand>=0) {
                const int h=leverHand;
                if(!(controllers.valid[h]&&controllers.grip[h])) {
                    leverHand=-1;tiltTarget=std::clamp(std::round(tableTilt/notch)*notch,0.f,degToRad(TiltMaxDeg));saveTilt();
                    log("Levier de la table relache : inclinaison %.0f degres",tiltTarget*180.f/float(Pi));
                } else {
                    const Vec3 l=sub(toLocal(stand,controllers.poses[h].position),rig.pivot);
                    float t=tiltFromLever(leverAngle(leverGrabTilt)+(std::atan2(l.z,l.y)-leverGrabAngle));
                    const float n=std::round(t/notch)*notch;
                    if(std::fabs(t-n)<degToRad(1.2f)) t=n;   // the lever drops into the notch
                    const int idx=int(std::floor(t/notch+.5f));
                    if(idx!=leverNotch) {leverNotch=idx;controllers.pulse(session,h,.55f,.018f);}
                    tableTilt=tiltTarget=std::clamp(t,0.f,degToRad(TiltMaxDeg));
                    request.leverFlags|=h?LeverHeldRight:LeverHeldLeft;
                    request.handFlags|=h?HandRightTable:HandLeftTable;   // this grip never pans the map
                }
            }
        }
        if(leverHand<0&&tableTilt!=tiltTarget) {   // settle on the notch (about 120 deg/s)
            const float step=degToRad(1.4f);
            tableTilt=std::fabs(tiltTarget-tableTilt)<=step?tiltTarget:tableTilt+(tiltTarget>tableTilt?step:-step);
        }
    }
    void launch(const fs::path& game,const std::wstring& arguments) {
        if(!fs::is_regular_file(game)) throw std::runtime_error("Executable du jeu introuvable. Executer PREPARER_JEU_VR.cmd.");
        SetEnvironmentVariableW(L"DK2VR_SESSION",token.c_str());
        SetEnvironmentVariableW(L"DK2VR_DIR",outputDirectory.wstring().c_str());   // 0.9.2: the game DLL finds DK2VR.ini, Textures and Captures here
        // Compatibility only for the child. No permanent registry compatibility changes.
        SetEnvironmentVariableW(L"__COMPAT_LAYER",L"RunAsInvoker");
        std::wstring command=L"\""+game.wstring()+L"\" "+arguments;
        STARTUPINFOW startup{sizeof(startup)};PROCESS_INFORMATION process{};
        if(!CreateProcessW(game.c_str(),command.data(),nullptr,nullptr,FALSE,0,nullptr,game.parent_path().c_str(),&startup,&process))
            throw std::runtime_error("Lancement du jeu impossible, erreur Windows "+std::to_string(GetLastError()));
        CloseHandle(process.hThread);gameProcess=process.hProcess;
        log("Jeu lance, PID %u. Le casque peut prendre quelques secondes pour afficher la premiere image.",process.dwProcessId);
    }
    void events() {
        XrEventDataBuffer event{XR_TYPE_EVENT_DATA_BUFFER};
        while(xrPollEvent(instance,&event)==XR_SUCCESS) {
            if(event.type==XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED) {
                auto& e=*reinterpret_cast<XrEventDataSessionStateChanged*>(&event);sessionState=e.state;
                log("Session OpenXR : %d",int(sessionState));
                if(e.state==XR_SESSION_STATE_READY) {XrSessionBeginInfo bi{XR_TYPE_SESSION_BEGIN_INFO};bi.primaryViewConfigurationType=XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;check(xrBeginSession(session,&bi),"xrBeginSession");running=true;}
                if(e.state==XR_SESSION_STATE_STOPPING) {check(xrEndSession(session),"xrEndSession");running=false;}
                if(e.state==XR_SESSION_STATE_EXITING||e.state==XR_SESSION_STATE_LOSS_PENDING) exit=true;
            } else if(event.type==XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING) exit=true;
            else if(event.type==XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED&&controllers.ready) controllers.profile(instance,session);
            else if(event.type==XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING) centered=false;
            event={XR_TYPE_EVENT_DATA_BUFFER};
        }
    }
    // 0.9.25: slots whose GPU copy is finished go back to the game DLL (never waits; a copy stuck for 1 s stops the host as before).
    void releaseFinished(bool count=false) {
        for(unsigned i=0;i<SlotCount;++i) if(slotPending[i]) {
            const HRESULT hr=context->GetData(slotFence[i].Get(),nullptr,0,D3D11_ASYNC_GETDATA_DONOTFLUSH);
            if(hr==S_OK) {slotPending[i]=false;InterlockedExchange(&ipc.data->slots[i].state,0);++perf.released;}
            else if(hr!=S_FALSE||GetTickCount64()-slotPendingSince[i]>1000) throw std::runtime_error("Le transfert des images vers le casque ne repond plus.");
            else if(count) ++perf.stillPending;
        }
    }
    // 0.9.47.61: PC mirror window. Called right after the eye copies, inside the same GPU fence as the headset frame.
    void mirrorFrame(int chosen,uint32_t eyeW,uint32_t eyeH) {
        try {
            if(!mirrorOn) {mirror.setVisible(false);return;}
            if(!mirror.hwnd) {
                if(!mirror.start(device.Get())) {log("Fenetre miroir PC : echec (%s). Option remise sur OFF.",mirror.error);mirrorOn=false;userSave(configuration,L"MirrorWindow",L"0");return;}
                log("Fenetre miroir PC : ouverte (une image complete, oeil %s, %s, 16/9)",mirrorEye?"droit":"gauche",mirrorFill?"remplit la fenetre":"image entiere");
            }
            mirror.setVisible(true);
            mirror.draw(device.Get(),context.Get(),sharedViews[chosen][mirrorEye].Get(),float(eyeW),float(eyeH),mirrorFill,mirrorCentreY);
        } catch(const std::exception& ex) {log("Fenetre miroir PC : erreur %s",ex.what());mirrorOn=false;}
    }
    bool consume() {
        if(asyncFence) releaseFinished();
        int chosen=-1;uint64_t newest=last.frame;
        for(unsigned i=0;i<SlotCount;++i) if(InterlockedCompareExchange(&ipc.data->slots[i].state,2,2)==2&&ipc.data->slots[i].frame>newest) {newest=ipc.data->slots[i].frame;chosen=int(i);}
        if(chosen<0||InterlockedCompareExchange(&ipc.data->slots[chosen].state,3,2)!=2) return false;
        auto& in=ipc.data->slots[chosen];
        for(unsigned i=0;i<SlotCount;++i) if(int(i)!=chosen&&ipc.data->slots[i].frame<newest) InterlockedCompareExchange(&ipc.data->slots[i].state,0,2);
        const bool incomingPossession=possessionWorld(request.mode,in.cameraMode&~CameraModeDecoupled);
        const bool previousPossession=possessionWorld(request.mode,last.cameraMode&~CameraModeDecoupled);
        // A one-frame visibility dropout must not flash a nearly black eye image.
        // Limit the hold to two frames so turning toward genuinely unseen space
        // never freezes the headset view for longer than a few milliseconds.
        if(incomingPossession&&previousPossession&&last.worldDraws>20&&in.worldDraws<5&&sparsePossessionHold<2&&GetTickCount64()-lastReceived<40) {
            ++sparsePossessionHold;++sparsePossessionSkipped;
            InterlockedExchange(&in.state,0);
            return false;
        }
        sparsePossessionHold=0;
        // Do not release this slot until the consumer GPU has finished reading the shared textures.
        try {
            if(in.generation!=lastGeneration[chosen]) {
                for(int j=0;j<3;++j) {
                    sharedViews[chosen][j].Reset();sharedTextures[chosen][j].Reset();
                    dx(device->OpenSharedResource(reinterpret_cast<HANDLE>(uintptr_t(in.handles[j])),IID_PPV_ARGS(&sharedTextures[chosen][j])),"OpenSharedResource (jeu et OpenXR doivent utiliser la meme carte graphique)");
                    dx(device->CreateShaderResourceView(sharedTextures[chosen][j].Get(),nullptr,&sharedViews[chosen][j]),"CreateShaderResourceView");
                }
                lastGeneration[chosen]=in.generation;
            }
            const bool captureComposited=capture;
            if(capture) {
                capture=false;
                log("Capture: anchor=(%.3f,%.3f,%.3f) q=(%.3f,%.3f,%.3f,%.3f) eye=(%.3f,%.3f,%.3f) q=(%.3f,%.3f,%.3f,%.3f) world=%u pitch=%.1f scale=%.3f",
                    in.anchor.position.x,in.anchor.position.y,in.anchor.position.z,in.anchor.orientation.x,in.anchor.orientation.y,in.anchor.orientation.z,in.anchor.orientation.w,
                    in.eyes[0].pose.position.x,in.eyes[0].pose.position.y,in.eyes[0].pose.position.z,in.eyes[0].pose.orientation.x,in.eyes[0].pose.orientation.y,in.eyes[0].pose.orientation.z,in.eyes[0].pose.orientation.w,in.worldDraws,request.tablePitchDegrees,request.unitsPerMeter);
                for(int j=0;j<3;++j) {
                    D3D11_TEXTURE2D_DESC desc{};sharedTextures[chosen][j]->GetDesc(&desc);
                    desc.Usage=D3D11_USAGE_STAGING;desc.BindFlags=0;desc.MiscFlags=0;desc.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
                    ComPtr<ID3D11Texture2D> staging;
                    dx(device->CreateTexture2D(&desc,nullptr,&staging),"Capture texture");
                    context->CopyResource(staging.Get(),sharedTextures[chosen][j].Get());
                    D3D11_MAPPED_SUBRESOURCE mapped{};dx(context->Map(staging.Get(),0,D3D11_MAP_READ,0,&mapped),"Capture map");
                    const wchar_t* names[]={L"oeil-gauche",L"oeil-droit",L"ecran"};
                    fs::path folder=outputDirectory/L"Captures";std::error_code ec;fs::create_directories(folder,ec);
                    if(j==0) {
                        SYSTEMTIME t{};GetLocalTime(&t);wchar_t stamp[64];
                        swprintf(stamp,64,L"DK2VR_%04u%02u%02u_%02u%02u%02u_%03u",t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond,++captureCount);
                        captureStamp=stamp;
                    }
                    fs::path file=folder/(captureStamp+L"_"+names[j]+L".jpg");
                    if(!saveJpg(file,desc.Width,desc.Height,mapped.RowPitch,mapped.pData)) {
                        file.replace_extension(L".png");
                        if(!savePng(file,desc.Width,desc.Height,mapped.RowPitch,mapped.pData)) {
                            file.replace_extension(L".bmp");saveBmp(file,desc.Width,desc.Height,mapped.RowPitch,mapped.pData);
                        }
                    }
                    context->Unmap(staging.Get(),0);
                }
                log("Capture %u enregistree dans Captures (frame %llu) : %ls",captureCount,in.frame,captureStamp.c_str());
            }
            desktop.create(session,device.Get(),colorFormat,in.desktopWidth,in.desktopHeight);
            const bool possession=possessionWorld(request.mode,in.cameraMode&~CameraModeDecoupled);
            const uint32_t vignette=0; // 0.8.85: vignette removed
            if(int(vignette)!=lastCompositedVignette) {lastCompositedVignette=int(vignette);log("VR eye compositor: possession=%d vignette=%u",int(possession),vignette);}
            for(int e=0;e<2;++e) {
                fs::path composed=captureComposited?outputDirectory/L"Captures"/(captureStamp+(e?L"_casque-oeil-droit.jpg":L"_casque-oeil-gauche.jpg")):fs::path{};
                blitter.copy(context.Get(),eyes[e],sharedViews[chosen][e].Get(),vignette,composed);
            }
            mirrorFrame(chosen,in.width,in.height);
            if(captureComposited) log("Capture du compositeur casque demandee pour les deux yeux (vignette=%u)",vignette);
            if(panel||(in.rejectedDraws&SlotMenuFlag)||(!possession&&in.worldDraws==0)||request.mode==3) blitter.copy(context.Get(),desktop,sharedViews[chosen][2].Get());
            if(asyncFence) {
                context->End(slotFence[chosen].Get());context->Flush();
                memcpy(&last,&in,sizeof(last));lastReceived=GetTickCount64();
                slotPending[chosen]=true;slotPendingSince[chosen]=GetTickCount64();   // released by releaseFinished() once the GPU copy is done
                return true;
            }
            context->End(gpuFence.Get());context->Flush();
            HRESULT hr=S_FALSE;uint64_t deadline=GetTickCount64()+1000;
            do {hr=context->GetData(gpuFence.Get(),nullptr,0,0);if(hr==S_FALSE) Sleep(0);} while(hr==S_FALSE&&GetTickCount64()<deadline);
            if(hr!=S_OK) throw std::runtime_error("Le transfert des images vers le casque ne repond plus.");
            memcpy(&last,&in,sizeof(last));lastReceived=GetTickCount64();
            InterlockedExchange(&in.state,0);
            return true;
        } catch(...) {
            // A failed GPU fence leaves ownership with the consumer. Shutdown, never overwrite an in-use texture.
            throw;
        }
    }
    void loop() {
        while(!exit) {
            events();
            if(gameProcess&&WaitForSingleObject(gameProcess,0)==WAIT_OBJECT_0) {DWORD code=0;GetExitCodeProcess(gameProcess,&code);log("Jeu termine, code %lu",code);break;}
            if(!running) {Sleep(20);continue;}
            LARGE_INTEGER waitStart{};QueryPerformanceCounter(&waitStart);
            XrFrameWaitInfo wi{XR_TYPE_FRAME_WAIT_INFO};XrFrameState fsr{XR_TYPE_FRAME_STATE};check(xrWaitFrame(session,&wi,&fsr),"xrWaitFrame");
            LARGE_INTEGER loopStart{},qpf{};QueryPerformanceCounter(&loopStart);QueryPerformanceFrequency(&qpf);
            auto msSince=[&](const LARGE_INTEGER& a) {LARGE_INTEGER t{};QueryPerformanceCounter(&t);return double(t.QuadPart-a.QuadPart)*1000./double(qpf.QuadPart);};
            double phase[5]{};phase[0]=double(loopStart.QuadPart-waitStart.QuadPart)*1000./double(qpf.QuadPart);   // 0.9.25 PERF: wait | begin | prepare | copy | end
            // 0.9.25 PERF: a predicted display time that jumps by more than one period = frame(s) the runtime showed without a new image from us
            if(fsr.predictedDisplayPeriod>0) {
                perf.period=fsr.predictedDisplayPeriod;
                if(perf.lastDisplay&&fsr.predictedDisplayTime>perf.lastDisplay) {const double steps=double(fsr.predictedDisplayTime-perf.lastDisplay)/double(fsr.predictedDisplayPeriod);if(steps>1.5&&steps<30.) {const uint64_t m=uint64_t(steps-.5);perf.missed+=m;perfMissedNow=m;}}
                perf.lastDisplay=fsr.predictedDisplayTime;
            }
            LARGE_INTEGER mark{};QueryPerformanceCounter(&mark);
            XrFrameBeginInfo bi{XR_TYPE_FRAME_BEGIN_INFO};check(xrBeginFrame(session,&bi),"xrBeginFrame");
            phase[1]=msSince(mark);QueryPerformanceCounter(&mark);
            if(asyncFence) releaseFinished(true);   // 0.9.25: last frame's copy is normally done: its slot is free BEFORE the next request goes to the VR thread
            XrViewLocateInfo li{XR_TYPE_VIEW_LOCATE_INFO};li.viewConfigurationType=XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;li.displayTime=fsr.predictedDisplayTime;li.space=space;
            XrViewState vs{XR_TYPE_VIEW_STATE};std::array<XrView,2> views{{{XR_TYPE_VIEW},{XR_TYPE_VIEW}}};uint32_t n=0;
            check(xrLocateViews(session,&li,&vs,2,&n,views.data()),"xrLocateViews");
            const XrViewStateFlags required=XR_VIEW_STATE_ORIENTATION_VALID_BIT|XR_VIEW_STATE_POSITION_VALID_BIT;
            bool valid=n==2&&(vs.viewStateFlags&required)==required;
            request.valid=valid&&fsr.shouldRender;
            if(key(VK_F3)||key(VK_F6)) {panel=!panel;log("Panneau du jeu (F3) : %s",panel?"affiche":"masque");}
            if(key(VK_F4)) {hud=!hud;log("HUD VR (F4) : %s",hud?"affiche":"masque");}
            if(key(VK_F2)) {minimapMode=(minimapMode+1)%3;log("Minicarte (F2) : %s",minimapMode==0?"piedestal":minimapMode==1?"montre (toujours)":"montre (quand on la regarde)");}
#ifndef DK2VR_PUBLIC   // 0.9.47.60: no capture (F11 / CAPTURE.request) in the public build, and no file check at every frame
            if(key(VK_F11)||fs::exists(outputDirectory/L"CAPTURE.request")) {
                const uint64_t now=GetTickCount64();
                if(!lastCaptureRequest||now-lastCaptureRequest>=1500) {capture=true;lastCaptureRequest=now;}
                std::error_code ec;fs::remove(outputDirectory/L"CAPTURE.request",ec);
            }
            if(fs::exists(outputDirectory/L"STOP.request")) {
                std::error_code ec;fs::remove(outputDirectory/L"STOP.request",ec);exit=true;
            }
#endif
            if(key(VK_F7)) {request.mode=(request.mode+1)%4;log("Mode : %u (0 auto, 1 plateau, 2 possession, 3 ecran)",request.mode);}
            if(key(VK_F9)) {
                request.unitsPerMeter=std::max(.1f,request.unitsPerMeter/1.15f);tableSizePct=std::min(TableSizeMaxPct,std::round(tableSizePct*1.15f));request.tableWidthMeters=tableSizedWidth();request.wheelPose.orientation.y=tableSizePct;saveTableSize();
                log("Echelle : table %.2f m (ancien mode %.3f unites/metre)",request.tableWidthMeters,request.unitsPerMeter);
            }
            if(key(VK_F10)) {
                request.unitsPerMeter=std::min(1000.f,request.unitsPerMeter*1.15f);tableSizePct=std::max(TableSizeMinPct,std::round(tableSizePct/1.15f));request.tableWidthMeters=tableSizedWidth();request.wheelPose.orientation.y=tableSizePct;saveTableSize();
                log("Echelle : table %.2f m (ancien mode %.3f unites/metre)",request.tableWidthMeters,request.unitsPerMeter);
            }
            if(key(VK_PRIOR)) {request.tablePitchDegrees=std::max(0.f,request.tablePitchDegrees-5);log("Inclinaison : %.1f degres",request.tablePitchDegrees);}
            if(key(VK_NEXT)) {request.tablePitchDegrees=std::min(85.f,request.tablePitchDegrees+5);log("Inclinaison : %.1f degres",request.tablePitchDegrees);}
            if(key(VK_F5)||fs::exists(outputDirectory/L"RELOAD.request")) {
                std::error_code ec;fs::remove(outputDirectory/L"RELOAD.request",ec);
                request.unitsPerMeter=setting(configuration,L"TableUnitsPerMeter",request.unitsPerMeter,.1f,1000);
                request.possessionUnitsPerMeter=setting(configuration,L"PossessionUnitsPerMeter",request.possessionUnitsPerMeter,.05f,100);
                request.tablePitchDegrees=setting(configuration,L"TablePitchDegrees",request.tablePitchDegrees,0,85);
                snapTurn=pref(configuration,L"PossessionSnapTurn",snapTurn?1.f:0.f,0,1)>.5f;
                smoothStrength=setting(configuration,L"PointerSmoothing",smoothStrength,0,1);passRgb[0]=pref(configuration,L"PassthroughR",passRgb[0],0,255);passRgb[1]=pref(configuration,L"PassthroughG",passRgb[1],0,255);passRgb[2]=pref(configuration,L"PassthroughB",passRgb[2],0,255);zoomExtraPct=std::clamp(pref(configuration,L"ZoomOutExtra",zoomExtraPct*.01f,0.f,4.f),0.f,1.f)*100.f;
                laserColour=uint32_t(pref(configuration,L"LaserColor",float(laserColour),0,6)+.5f);invertTurn=pref(configuration,L"InvertMapRotation",invertTurn?1.f:0.f,0,1)>.5f;leftHanded=pref(configuration,L"LeftHanded",leftHanded?1.f:0.f,0,1)>.5f;
                snapDegrees=pref(configuration,L"PossessionSnapAngle",snapDegrees,15,90);
                smoothTurnSpeed=setting(configuration,L"PossessionTurnSpeed",smoothTurnSpeed,30,300);
                readTableSettings(configuration);placeTable();
                log("Reglages relus : echelle=%.3f possession=%.3f inclinaison=%.1f table=%u largeur=%.2f",request.unitsPerMeter,request.possessionUnitsPerMeter,request.tablePitchDegrees,request.tableAnchor,request.tableWidthMeters);
            }
            controllers.update(session,space,fsr.predictedDisplayTime,sessionState==XR_SESSION_STATE_FOCUSED);
            const uint32_t nativeMode=last.cameraMode&~CameraModeDecoupled;
            const bool possessed=request.mode==2||(request.mode==0&&(nativeMode==1||nativeMode==2));
            bool recenter=key(VK_F8)||controllers.recenterPressed(possessed);
            // 0.6.9.8: automatic recenter 5 s after the launch (first valid headset pose), before the main menu
            // (0.6.9.7 waited for the dungeon: too late, the player asked for it at the start).
            if(valid&&!worldSeenTick) worldSeenTick=GetTickCount64();
            if(!autoRecentered&&worldSeenTick&&GetTickCount64()-worldSeenTick>5000) {autoRecentered=true;recenter=true;log("Recentrage automatique (5 s apres le lancement)");}
            if(valid) {
                for(int e=0;e<2;++e) request.eyes[e]={dk2vrPose(views[e].pose),fov(views[e].fov)};
                if(!centered||recenter) {
                    request.anchor.position=scale(add(request.eyes[0].pose.position,request.eyes[1].pose.position),.5f);
                    request.anchor.orientation=yawOnly(request.eyes[0].pose.orientation);++request.recenterSerial;centered=true;
                    placeTable();
                    log("Vue recentree. Table placee devant soi, %.2f m sous les yeux (%s), largeur %.2f m.",placedHeight,floorPlaced?"selon le sol reel":"TableHeight",request.tableWidthMeters);
                }
            }
            uint32_t floorFlag=request.handFlags&RequestFloorKnown;request.handFlags=0;
            if(panel) request.handFlags|=RequestPanelShown;
            if(hud&&request.mode!=3) request.handFlags|=RequestHudShown;
            if(stageSpace) {
                XrSpaceLocation loc{XR_TYPE_SPACE_LOCATION};
                if(XR_SUCCEEDED(xrLocateSpace(stageSpace,space,fsr.predictedDisplayTime,&loc))&&(loc.locationFlags&XR_SPACE_LOCATION_POSITION_VALID_BIT)) {
                    float y=loc.pose.position.y;
                    if(std::isfinite(y)&&y<request.anchor.position.y-.3f&&y>request.anchor.position.y-2.5f) {
                        request.floorY=y;request.handFlags|=RequestFloorKnown;
                        if(!floorLogged) {
                            log("Sol reel : %.2f m sous les yeux (espace STAGE).",request.anchor.position.y-y);floorLogged=true;
                            // First floor measure after the automatic placement: place the table again at the right height.
                            if(!floorFlag&&!floorPlaced&&tableTopHeight>0) {placeTable();log("Table replacee selon le sol reel : %.2f m sous les yeux.",placedHeight);}
                        }
                    }
                }
            }
            for(int h=0;h<2;++h) if(controllers.valid[h]) {
                request.hands[h]=controllers.poses[h];
                request.handFlags|=(h?HandRightValid:HandLeftValid)|(controllers.grip[h]?(h?HandRightGrip:HandLeftGrip):0u)|(controllers.trig[h]?(h?HandRightTrigger:HandLeftTrigger):0u);
            }
            updateLever(possessed);   // 0.9.0
            {   // 0.9.1: SPACE / ESCAPE on the two face buttons of the free hand: left X / Y (right A / B for a left-handed player at the table;
                // in possession the right A / B stay the creature's abilities, so the left X / Y are used whatever the hand)
                const int kh=(!possessed&&leftHanded)?1:0;
                if(controllers.valid[kh]||controllers.ready) {
                    if(controllers.second[kh]) request.handFlags|=HandKeySpace;
                    if(controllers.third[kh]&&!possessed) request.handFlags|=HandKeyEscape;   // 0.9.47.43: Y does nothing in a possession (ESCAPE made the game bug)
                }
            }
            if(!possessed&&request.tableAnchor&&centered&&tableFixed) {
                // 0.3.2: metal handles. A grip that starts within reach of a handle holds the table until released:
                // one hand turns the table on itself, two hands carry it. Other grips act on the map (game DLL).
                for(int h=0;h<2;++h) {
                    bool g=controllers.valid[h]&&controllers.grip[h]&&leverHand!=h;   // 0.9.0: the lever hand never takes a handle
                    if(g&&!gripBefore[h]) {
                        holding[h]=false;
                        // 0.4.0: four hanging handles under the wooden frame (TableModel.hpp).
                        const TableModel model=tableModel(request.tableWidthMeters,tableAspect);
                        for(int k=0;k<TableModel::HandleCount;++k) {
                            Vec3 d=sub(controllers.poses[h].position,toWorld(request.table,model.handle(k)));
                            if(d.x*d.x+d.y*d.y+d.z*d.z<TableModel::GrabRadius*TableModel::GrabRadius) holding[h]=true;
                        }
                        if(holding[h]) log("Poignee de la table saisie (main %s)",h?"droite":"gauche");
                    }
                    if(!g) holding[h]=false;
                    gripBefore[h]=g;
                    if(holding[h]) request.handFlags|=h?HandRightTable:HandLeftTable;
                }
                bool held[2]={holding[0],holding[1]};
                if(grab.update(held,controllers.poses,tableBase,request.tableWidthMeters,.2f,6.f))
                    log("Table relachee : centre=(%.2f,%.2f,%.2f)",tableBase.position.x,tableBase.position.y,tableBase.position.z);
            }
            if(!possessed&&request.tableAnchor&&centered&&!tableFixed) {
                int before=grab.hands;
                bool gl[2]={controllers.grip[0]&&leverHand!=0,controllers.grip[1]&&leverHand!=1};
                if(grab.update(gl,controllers.poses,tableBase,request.tableWidthMeters,.2f,6.f))
                    log("Table relachee : centre=(%.2f,%.2f,%.2f) largeur=%.2f m",tableBase.position.x,tableBase.position.y,tableBase.position.z,request.tableWidthMeters);
                else if(grab.hands>0&&grab.hands!=before) log("Table saisie a %d main(s)",grab.hands);
            }
            applyTilt();   // 0.9.0: level pose (handles) + tilt (lever) -> the pose the game draws
            if(possessed!=wasPossessed) {wasPossessed=possessed;possessionTick=0;holding[0]=holding[1]=false;gripBefore[0]=gripBefore[1]=false;log("Possession VR : %s",possessed?"controles actifs":"retour au plateau");}
            pointHud(possessed);
            const bool fresh=freshWaitMs>0&&(last.cameraMode&CameraModeDecoupled)&&GetTickCount64()-lastReceived<500&&GetTickCount64()>=freshPausedUntil;
            if(fresh) ResetEvent(ipc.ready);
            if(ipc.lock(10)) {ipc.data->request=request;InterlockedIncrement(&ipc.data->requestSerial);ipc.unlock();if(ipc.requestEvent) SetEvent(ipc.requestEvent);}
            if(fresh) {
                bool hit=WaitForSingleObject(ipc.ready,DWORD(freshWaitMs))==WAIT_OBJECT_0;
                if(hit) ++freshHits; else {++freshMisses;++windowMisses;}
                if(++windowTries>=90) {
                    if(windowMisses*100>windowTries*30) {freshPausedUntil=GetTickCount64()+3000;++freshPauses;log("Attente de l'image fraiche suspendue 3 s (%u retards sur %u)",windowMisses,windowTries);}
                    windowTries=windowMisses=0;
                }
            }
            phase[2]=msSince(mark);QueryPerformanceCounter(&mark);
            if(consume()) ++newImages;
            phase[3]=msSince(mark);
            {const bool g=(last.rejectedDraws&SlotGameMenuFlag)!=0&&(last.cameraMode&CameraModeDecoupled)&&GetTickCount64()-lastReceived<1500;if(g!=gameMenuOpen) {gameMenuOpen=g;log("Menu options du jeu %s : bouton OPTIONS du mur %s",g?"ouvert":"ferme",g?"masque et inactif":"disponible");}}
            wallRight=(last.rejectedDraws&SlotPedestalRightFlag)!=0&&(last.cameraMode&CameraModeDecoupled);   // 0.9.47.42: options panel on the pedestal's side
            hudFrontPanel=(last.rejectedDraws&SlotHudFrontFlag)!=0&&(last.cameraMode&CameraModeDecoupled)&&GetTickCount64()-lastReceived<300;   // 0.8.94.23
            {const bool m=(last.rejectedDraws&SlotMenuFlag)!=0&&(last.cameraMode&CameraModeDecoupled)&&GetTickCount64()-lastReceived<1500;if(m!=menuScreen) {menuScreen=m;log("Salle de menu : %s",m?"ecran du jeu sur le mur en face":"quittee");}}
            std::array<XrCompositionLayerProjectionView,2> pv{{{XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW},{XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW}}};
            XrCompositionLayerProjection projection{XR_TYPE_COMPOSITION_LAYER_PROJECTION};projection.space=space;projection.viewCount=2;projection.views=pv.data();
            XrCompositionLayerQuad quad{XR_TYPE_COMPOSITION_LAYER_QUAD};quad.space=space;quad.eyeVisibility=XR_EYE_VISIBILITY_BOTH;
            std::vector<const XrCompositionLayerBaseHeader*> layers;
            std::array<XrCompositionLayerQuad,4> hudLayers{};
            if(valid&&fsr.shouldRender&&last.frame&&GetTickCount64()-lastReceived<1500) {
                // An empty native frustum is still a possession world frame. Never
                // flash the PC quad across the headset on zero-draw frames.
                gameShownOnce=true;
                bool world=showWorldProjection(request.mode,last.cameraMode&~CameraModeDecoupled,last.worldDraws);
                if(world) {
                    float turnAhead=0;
                    if(possessionTurnPrediction&&possessed&&!(last.cameraMode&CameraModeDecoupled)&&possessionWorld(request.mode,last.cameraMode&~CameraModeDecoupled)&&std::isfinite(request.possessionTurn)&&std::isfinite(last.possessionTurn)) {
                        float g=ipc.data->possessionGain;if(!(g>.3f&&g<3.f)) g=1.f;
                        turnAhead=std::clamp(g*(request.possessionTurn-last.possessionTurn),-.8f,.8f);
                        ++turnPredFrames;turnPredSum+=std::fabs(turnAhead)*180./Pi;turnPredMax=std::max(turnPredMax,double(std::fabs(turnAhead))*180./Pi);
                    }
                    const Quat ahead=yawQuat(turnAhead);
                    Vec3 centre{0,0,0};for(int e=0;e<2;++e) centre=add(centre,scale(last.eyes[e].pose.position,.5f));
                    for(int e=0;e<2;++e) {
                        pv[e].pose=xrPose(last.eyes[e].pose);
                        if(turnAhead!=0.f) {
                            // Positive turn = the creature turned right: the world now lies to the left, so the image pose is rotated left (+Y) about the head.
                            Pose q=last.eyes[e].pose;q.orientation=normalized(multiply(ahead,q.orientation));q.position=add(centre,rotate(ahead,sub(q.position,centre)));
                            pv[e].pose=xrPose(q);
                        }
                        pv[e].fov=xrFov(last.eyes[e].fov);pv[e].subImage.swapchain=eyes[e].handle;pv[e].subImage.imageRect.extent={int32_t(request.width),int32_t(request.height)};}
                    layers.push_back(reinterpret_cast<const XrCompositionLayerBaseHeader*>(&projection));
                }
                Pose head=request.eyes[0].pose;head.position=scale(add(request.eyes[0].pose.position,request.eyes[1].pose.position),.5f);
                Pose panelPose{};float panelW=0,panelH=0;
                const bool menuWall=menuScreen&&world;
                if(!world||panel||menuWall) {
                    Pose p=request.anchor;
                    // In game, the full game screen (with its interface) hangs like a wall screen
                    // beyond the table; PanelPosition=0 restores the 0.1.0 side panel.
                    Vec3 offset=!world?Vec3{0,0,-1.6f}:panelPosition==1?Vec3{0,panelHeight,-panelDistance}:Vec3{-.85f,-.25f,-1.1f};
                    p.position=toWorld(request.anchor,offset);
                    quad.pose=xrPose(p);quad.size.width=!world?1.6f:panelPosition==1?panelWidth:.7f;quad.size.height=quad.size.width*last.desktopHeight/last.desktopWidth;
                    quad.subImage.swapchain=desktop.handle;quad.subImage.imageRect.extent={int32_t(last.desktopWidth),int32_t(last.desktopHeight)};
                    if(menuWall) {
                        // 0.8.88: game screen (4:3 area of the back buffer) on the front wall of the room
                        const int32_t W=int32_t(last.desktopWidth),H=int32_t(last.desktopHeight);
                        int32_t cw=W,cx=0;if(W>int32_t(H*1.34f)) {cw=int32_t(H*4/3+.5f);cx=(W-cw)/2;}
                        quad.pose=xrPose(menuScreenPose(request));
                        quad.size.width=MenuScreenWidth;quad.size.height=MenuScreenWidth*float(H)/float(cw);
                        quad.subImage.imageRect.offset={cx,0};quad.subImage.imageRect.extent={cw,H};
                        menuRect[0]=cx;menuRect[1]=0;menuRect[2]=cw;menuRect[3]=H;
                    }
                    if(!menuWall||panel) layers.push_back(reinterpret_cast<const XrCompositionLayerBaseHeader*>(&quad)); // 0.8.89.2: the menu screen is drawn inside the room images (no compositor layer)
                    panelPose=p;panelW=quad.size.width;panelH=quad.size.height;
                }
                if(menuWall) {panelPose=menuScreenPose(request);panelW=quad.size.width;panelH=quad.size.height;}
                placeHud(world,!world||panel||menuWall,panelPose,panelW,panelH,head);
                if(menuWall&&menuRect[2]>0) {HudQuad& hq=hudQuads[4];hq.rect.offset={menuRect[0],menuRect[1]};hq.rect.extent={menuRect[2],menuRect[3]};}
            } else {for(auto& q:hudQuads) q.shown=false;
                if(!gameShownOnce&&fsr.shouldRender&&splashFloating) {   // 0.9.47.37: before the first image of the game: the start-up picture instead of black
                    prepareSplash();
                    if(splashReady) {
                        XrCompositionLayerQuad sq{XR_TYPE_COMPOSITION_LAYER_QUAD};sq.space=space;sq.eyeVisibility=XR_EYE_VISIBILITY_BOTH;
                        Pose p=request.anchor;p.position=toWorld(request.anchor,Vec3{0,0,-1.6f});
                        sq.pose=xrPose(p);sq.size.width=1.6f;sq.size.height=1.6f*splashAspect;
                        sq.subImage.swapchain=splash.handle;sq.subImage.imageRect.offset={0,0};sq.subImage.imageRect.extent={int32_t(splash.width),int32_t(splash.height)};
                        splashLayer=sq;layers.push_back(reinterpret_cast<const XrCompositionLayerBaseHeader*>(&splashLayer));
                    }
                }
            }
            XrFrameEndInfo ei{XR_TYPE_FRAME_END_INFO};ei.displayTime=fsr.predictedDisplayTime;ei.environmentBlendMode=blend;ei.layerCount=uint32_t(layers.size());ei.layers=layers.data();
            QueryPerformanceCounter(&mark);
            check(xrEndFrame(session,&ei),"xrEndFrame");++frames;
            phase[4]=msSince(mark);
            if(asyncFence) releaseFinished();   // 0.9.25: the copy is normally finished after xrEndFrame: slot free again within the same frame
            {   // 0.9.25 PERF
                for(int i=0;i<5;++i) {perf.sum[i]+=phase[i];perf.worst[i]=std::max(perf.worst[i],phase[i]);}
                ++perf.frames;
                const int pm=menuScreen?0:wasPossessed?2:(last.worldDraws>0&&GetTickCount64()-lastReceived<1500)?1:3;   // menu | table | possession | other
                ++perf.modeFrames[pm];perf.modeMissed[pm]+=perfMissedNow;perf.modeMs[pm]+=perf.period>0?double(perf.period)*1e-6*double(1+perfMissedNow):0.;perfMissedNow=0;
            }
            {LARGE_INTEGER now{};QueryPerformanceCounter(&now);double ms=double(now.QuadPart-loopStart.QuadPart)*1000./double(qpf.QuadPart);loopWorst=std::max(loopWorst,ms);if(ms>8) ++slowLoops;}
            if(GetTickCount64()-lastReport>5000) {
                log("Pointeur : cible=%d clics=%llu. VR frames=%llu (%.1f/s) image=%llu world=%u UI=%u rejected=%u camera=%u source=%s new images=%llu (fresh %llu/%llu, pauses %llu) host loop worst %.1f ms, >8 ms: %llu",
                    pointerTarget,pointerClicks,frames,(frames-framesAtReport)*1000./double(GetTickCount64()-lastReport),last.frame,last.worldDraws,last.uiDraws,last.rejectedDraws,
                    last.cameraMode&~CameraModeDecoupled,(last.cameraMode&CameraModeDecoupled)?"VR thread (headset rate)":"game frame",newImages,freshHits,freshHits+freshMisses,freshPauses,loopWorst,slowLoops);
                if(turnPredFrames) {log("Possession turn prediction (0.8.67, last 5 s): %llu headset frames, rotation applied avg %.2f deg, max %.2f deg, game gain %.2f",(unsigned long long)turnPredFrames,turnPredSum/turnPredFrames,turnPredMax,ipc.data->possessionGain);turnPredFrames=0;turnPredSum=turnPredMax=0;}
                if(sparsePossessionSkipped) {log("Possession: %llu sparse frames briefly held to suppress black flashes",(unsigned long long)sparsePossessionSkipped);sparsePossessionSkipped=0;}
                {   // 0.9.25 PERF line of the headset loop
                    const double n=double(std::max<uint64_t>(1,perf.frames));const double hz=perf.period>0?1e9/double(perf.period):0.;
                    log("PERF casque (0.9.25, 5 s) : %.1f images/s pour un affichage a %.1f Hz, images ratees par le runtime %llu (%.1f %%), nouvelles images VR %llu | boucle moy/max ms : attente %.2f/%.2f | debut %.2f/%.2f | preparation %.2f/%.2f | copie %.2f/%.2f | fin %.2f/%.2f | emplacements liberes %llu, encore en copie %llu",
                        perf.frames*1000./double(std::max<uint64_t>(1,GetTickCount64()-lastReport)),hz,(unsigned long long)perf.missed,100.*double(perf.missed)/double(std::max<uint64_t>(1,perf.missed+perf.frames)),(unsigned long long)newImages,
                        perf.sum[0]/n,perf.worst[0],perf.sum[1]/n,perf.worst[1],perf.sum[2]/n,perf.worst[2],perf.sum[3]/n,perf.worst[3],perf.sum[4]/n,perf.worst[4],(unsigned long long)perf.released,(unsigned long long)perf.stillPending);
                    perf.reset5();
                    if(GetTickCount64()-perfLastBilan>30000) {
                        perfLastBilan=GetTickCount64();
                        static const char* names[4]={"menu","plateau","possession","autre"};
                        char line[600];int k=snprintf(line,sizeof(line),"BILAN PERF casque (0.9.25, depuis le lancement, runtime %s) :",runtimeName.c_str());
                        for(int m=0;m<4;++m) if(perf.modeFrames[m]>=90) k+=snprintf(line+k,sizeof(line)-k," | %s %.1f images/s, ratees %.1f %%",names[m],perf.modeMs[m]>0?1000.*perf.modeFrames[m]/perf.modeMs[m]:0.,100.*perf.modeMissed[m]/double(perf.modeFrames[m]+perf.modeMissed[m]));
                        log("%s",line);
                    }
                }
                newImages=freshHits=freshMisses=0;lastReport=GetTickCount64();framesAtReport=frames;loopWorst=0;slowLoops=0;freshPauses=0;
            }
        }
    }
};
}
int wmain(int argc,wchar_t** argv) {
    wchar_t module[MAX_PATH]{};GetModuleFileNameW(nullptr,module,MAX_PATH);fs::path root=fs::path(module).parent_path();
    logfile=_wfsopen((root/L"DK2VR-host.log").c_str(),L"w",_SH_DENYNO);
    log("Dungeon Keeper II VR 0.9.47.6 - prototype experimental OpenXR x64 (protocole %u)",ProtocolVersion);
    bool probe=false,noLaunch=false;fs::path game=root/L"Game"/L"DKII-DX.exe";
    // 0.9.47.37: the intro videos / logos are played again (they appear on the wall screen of the room like any other full-screen game image).
    // Before, the host always launched the game with -NOMOVIES (prototype choice). DK2VR.ini: IntroMovies=0 brings the old behaviour back.
    {   // 0.9.47.63: heal DK2VR.ini before anything reads it (byte-order mark / repeated [VR] section, see common/IniRepair.hpp)
        dk2vr::inirepair::Result fix;bool healed=false;try {healed=dk2vr::inirepair::repairFile(root/L"DK2VR.ini",fix);} catch(...) {}
        if(healed) log("DK2VR.ini repare : marque d'ordre des octets %s, %d section(s) [VR] repetee(s) fusionnee(s) (%d reglages) ; l'original est garde dans DK2VR.ini.avant_reparation",fix.bom?"retiree":"absente",fix.mergedSections,fix.mergedKeys);
    }
    gUserSettings=userSettingsPath(root/L"DK2VR.ini");
    const bool introMovies=pref(root/L"DK2VR.ini",L"IntroMovies",1,0,1)>.5f;
    std::wstring arguments=introMovies?L"":L"-NOMOVIES";
    log("Videos d'introduction : %s (DK2VR.ini IntroMovies)",introMovies?"jouees sur l'ecran de la salle":"supprimees (-NOMOVIES)");
    for(int i=1;i<argc;++i) {
        std::wstring a=argv[i];
        if(a==L"--probe") probe=true;
        else if(a==L"--no-launch") noLaunch=true;
        else if(a==L"--game"&&i+1<argc) game=argv[++i];
        else if(a==L"--args"&&i+1<argc) arguments=argv[++i];
        else {log("Argument inconnu.");return 2;}
    }
    int result=0;
    CoInitializeEx(nullptr,COINIT_MULTITHREADED);
    try {Host host;host.initialize(root/L"DK2VR.ini",probe);host.splashFloating=pref(root/L"DK2VR.ini",L"StartupPictureFloating",0,0,1)>.5f;if(!probe) {if(!noLaunch) host.launch(game,arguments);host.loop();}}
    catch(const std::exception& e) {log("ERREUR : %s",e.what());result=1;}
    if(logfile) {fclose(logfile);logfile=nullptr;}
    return result;
}
