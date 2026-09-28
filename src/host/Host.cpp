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
    ComPtr<ID3D11SamplerState> sampler;
    void create(ID3D11Device* d,bool srgb) {
        const char* vertex="struct V{float4 p:SV_Position;float2 uv:TEXCOORD;};V main(uint id:SV_VertexID){V v;v.uv=float2((id<<1)&2,id&2);v.p=float4(v.uv.x*2-1,1-v.uv.y*2,0,1);return v;}";
        const char* pixel="Texture2D t:register(t0);SamplerState s:register(s0);float4 main(float4 p:SV_Position,float2 uv:TEXCOORD):SV_Target{float3 c=t.Sample(s,uv).rgb;\n#ifdef SRGB\nc=lerp(c/12.92,pow((c+.055)/1.055,2.4),step(.04045,c));\n#endif\nreturn float4(c,1);}";
        ComPtr<ID3DBlob> code,error;
        dx(D3DCompile(vertex,strlen(vertex),nullptr,nullptr,nullptr,"main","vs_4_0",0,0,&code,&error),"Compile vertex shader");
        dx(d->CreateVertexShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&vs),"CreateVertexShader");
        code.Reset();error.Reset();D3D_SHADER_MACRO macros[]={{"SRGB","1"},{nullptr,nullptr}};
        dx(D3DCompile(pixel,strlen(pixel),nullptr,srgb?macros:nullptr,nullptr,"main","ps_4_0",0,0,&code,&error),"Compile pixel shader");
        dx(d->CreatePixelShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&ps),"CreatePixelShader");
        D3D11_SAMPLER_DESC sd{};sd.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;sd.AddressU=sd.AddressV=sd.AddressW=D3D11_TEXTURE_ADDRESS_CLAMP;sd.MaxLOD=D3D11_FLOAT32_MAX;
        dx(d->CreateSamplerState(&sd,&sampler),"CreateSamplerState");
    }
    void copy(ID3D11DeviceContext* c,Swapchain& swap,ID3D11ShaderResourceView* source) {
        uint32_t index=0;XrSwapchainImageAcquireInfo ai{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
        check(xrAcquireSwapchainImage(swap.handle,&ai,&index),"xrAcquireSwapchainImage");
        XrSwapchainImageWaitInfo wi{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};wi.timeout=XR_INFINITE_DURATION;
        check(xrWaitSwapchainImage(swap.handle,&wi),"xrWaitSwapchainImage");
        ID3D11RenderTargetView* target=swap.targets[index].Get();c->OMSetRenderTargets(1,&target,nullptr);
        D3D11_VIEWPORT vp{0,0,float(swap.width),float(swap.height),0,1};c->RSSetViewports(1,&vp);
        c->IASetInputLayout(nullptr);c->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        c->VSSetShader(vs.Get(),nullptr,0);c->PSSetShader(ps.Get(),nullptr,0);
        c->PSSetShaderResources(0,1,&source);ID3D11SamplerState* smp=sampler.Get();c->PSSetSamplers(0,1,&smp);
        c->Draw(3,0);ID3D11ShaderResourceView* empty=nullptr;c->PSSetShaderResources(0,1,&empty);
        c->OMSetRenderTargets(0,nullptr,nullptr);
        XrSwapchainImageReleaseInfo ri{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};check(xrReleaseSwapchainImage(swap.handle,&ri),"xrReleaseSwapchainImage");
    }
};
// OpenXR controllers: grip pose, squeeze (grab), trigger (reserved for gameplay), stick click (recenter).
struct Controllers {
    XrActionSet set=XR_NULL_HANDLE;
    XrAction pose=XR_NULL_HANDLE,squeeze=XR_NULL_HANDLE,trigger=XR_NULL_HANDLE,recenter=XR_NULL_HANDLE,aim=XR_NULL_HANDLE,secondary=XR_NULL_HANDLE,stick=XR_NULL_HANDLE;float stickXY[2][2]{};
    XrSpace aimSpaces[2]{XR_NULL_HANDLE,XR_NULL_HANDLE};Pose aimPoses[2]{};bool aimValid[2]{},second[2]{};
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
        stick=action("scroll","Defiler la carte",XR_ACTION_TYPE_VECTOR2F_INPUT);
        suggest(instance,"/interaction_profiles/oculus/touch_controller",{{pose,"/input/grip/pose"},{squeeze,"/input/squeeze/value"},{trigger,"/input/trigger/value"},{recenter,"/input/thumbstick/click"},{aim,"/input/aim/pose"},{stick,"/input/thumbstick"}},{{secondary,"/user/hand/right/input/a/click"},{secondary,"/user/hand/left/input/x/click"}});
        suggest(instance,"/interaction_profiles/valve/index_controller",{{pose,"/input/grip/pose"},{squeeze,"/input/squeeze/value"},{trigger,"/input/trigger/value"},{recenter,"/input/thumbstick/click"},{aim,"/input/aim/pose"},{secondary,"/input/a/click"},{stick,"/input/thumbstick"}});
        suggest(instance,"/interaction_profiles/htc/vive_controller",{{pose,"/input/grip/pose"},{squeeze,"/input/squeeze/click"},{trigger,"/input/trigger/value"},{recenter,"/input/trackpad/click"},{aim,"/input/aim/pose"},{secondary,"/input/menu/click"}});
        suggest(instance,"/interaction_profiles/microsoft/motion_controller",{{pose,"/input/grip/pose"},{squeeze,"/input/squeeze/click"},{trigger,"/input/trigger/value"},{recenter,"/input/thumbstick/click"},{aim,"/input/aim/pose"},{secondary,"/input/menu/click"},{stick,"/input/thumbstick"}});
        suggest(instance,"/interaction_profiles/khr/simple_controller",{{pose,"/input/grip/pose"},{trigger,"/input/select/click"},{recenter,"/input/menu/click"},{aim,"/input/aim/pose"}});
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
    void clear() {for(int h=0;h<2;++h) {valid[h]=grip[h]=trig[h]=click[h]=aimValid[h]=second[h]=false;squeezeValue[h]=triggerValue[h]=0;}}
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
            XrActionStateVector2f v{XR_TYPE_ACTION_STATE_VECTOR2F};gi.action=stick;stickXY[h][0]=stickXY[h][1]=0;
            if(XR_SUCCEEDED(xrGetActionStateVector2f(session,&gi,&v))&&v.isActive) {stickXY[h][0]=v.currentState.x;stickXY[h][1]=v.currentState.y;}
        }
    }
    bool recenterPressed() const {return (click[0]&&!previousClick[0])||(click[1]&&!previousClick[1]);}
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
    bool running=false,exit=false,centered=false,panel=false; // 0.5.16: game screen panel off by default (F3)
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
    ComPtr<ID3D11Query> gpuFence;
    Swapchain eyes[2],desktop;Blitter blitter;Ipc ipc;
    int64_t colorFormat=0;Request request{};Slot last{};
    uint64_t lastReceived=0,frames=0,lastReport=0;uint32_t lastGeneration[SlotCount]{};
    // 0.3.0: wait briefly for the image of the pose just sent when the game DLL renders at the headset rate.
    int freshWaitMs=0;bool tableFixed=true;
    // 0.4.1: adaptive wait. If the VR image often arrives too late, waiting would make this loop miss the
    // headset's frame deadline (frame rate drop that stays): pause the wait for 3 s and log it.
    uint32_t windowTries=0,windowMisses=0;uint64_t freshPausedUntil=0,freshPauses=0,framesAtReport=0;double loopWorst=0;uint64_t slowLoops=0;bool gripBefore[2]{},holding[2]{};uint64_t freshHits=0,freshMisses=0,newImages=0;
    bool capture=false;unsigned captureCount=0;std::wstring captureStamp;
    Controllers controllers;TableGrab grab;
    // Table (Demeo style) and panel placement, from DK2VR.ini.
    float tableTopHeight=0.f,placedHeight=.50f;bool floorPlaced=false;
    float tableDistance=.80f,tableHeight=.40f,tableWidth=.80f,tableAspect=.75f;
    XrSpace stageSpace=XR_NULL_HANDLE;bool floorLogged=false;
    int panelPosition=1;float panelDistance=2.2f,panelHeight=.35f,panelWidth=1.8f;
    bool useControllers=true;
    // 0.6.0 HUD and pointer (DK2VR.ini: Hud, LeftHanded, MinimapMode, HudBottomPlace, HudWallWidth...)
    bool hud=true,leftHanded=false;uint64_t worldSeenTick=0;bool autoRecentered=false;int minimapMode=0,bottomPlace=0;
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
        configuration=ini;
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
            log("Runtime OpenXR : %s",props.runtimeName);
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
        readTableSettings(ini);request.tableWidthMeters=tableWidth;
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
        hud=setting(ini,L"Hud",1,0,1)>.5f;leftHanded=setting(ini,L"LeftHanded",0,0,1)>.5f;
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
    // Laser from the aim pose of the pointing hand; the nearest module hit becomes the game cursor.
    void pointHud() {
        request.pointerFlags=(leftHanded?HudLeftHanded:0u)|(bottomPlace==1?HudBottomNear:0u)|(uint32_t(minimapMode)<<HudMinimapShift);
        request.rayLength=0;pointerTarget=-1;
        const int h=leftHanded?0:1;
        {   // 0.6.2: thumbstick of the other hand scrolls the map (dead zone 0.15, rescaled).
            float x=controllers.stickXY[1-h][0],y=controllers.stickXY[1-h][1],l=std::sqrt(x*x+y*y);
            if(!std::isfinite(l)||l<.15f) x=y=0; else {float k=std::min(1.f,(l-.15f)/.85f)/l;x*=k;y*=k;}
            request.stickX=x;request.stickY=y;
        }
        const bool trig=controllers.trig[h],second=controllers.second[h];
        if(!controllers.aimValid[h]) {trigLatched=secondLatched=false;trigBefore=trig;secondBefore=second;return;}
        const Pose aim=controllers.aimPoses[h];const Vec3 dir=rotate(aim.orientation,{0,0,-1});
        request.pointerFlags|=PointerAimValid|(trig?PointerTriggerRaw:0u)|(second?PointerSecondRaw:0u);
        request.rayOrigin=aim.position;request.rayDirection=dir;
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
        request.table=compose(request.anchor,Pose{{0,0,0,1},{0,-placedHeight,-tableDistance}});
        request.tableWidthMeters=tableWidth;grab.release();
    }
    void launch(const fs::path& game,const std::wstring& arguments) {
        if(!fs::is_regular_file(game)) throw std::runtime_error("Executable du jeu introuvable. Executer PREPARER_JEU_VR.cmd.");
        SetEnvironmentVariableW(L"DK2VR_SESSION",token.c_str());
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
    bool consume() {
        int chosen=-1;uint64_t newest=last.frame;
        for(unsigned i=0;i<SlotCount;++i) if(InterlockedCompareExchange(&ipc.data->slots[i].state,2,2)==2&&ipc.data->slots[i].frame>newest) {newest=ipc.data->slots[i].frame;chosen=int(i);}
        if(chosen<0||InterlockedCompareExchange(&ipc.data->slots[chosen].state,3,2)!=2) return false;
        auto& in=ipc.data->slots[chosen];
        for(unsigned i=0;i<SlotCount;++i) if(int(i)!=chosen&&ipc.data->slots[i].frame<newest) InterlockedCompareExchange(&ipc.data->slots[i].state,0,2);
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
            for(int e=0;e<2;++e) blitter.copy(context.Get(),eyes[e],sharedViews[chosen][e].Get());
            if(panel||in.worldDraws==0||request.mode==3) blitter.copy(context.Get(),desktop,sharedViews[chosen][2].Get());
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
            XrFrameWaitInfo wi{XR_TYPE_FRAME_WAIT_INFO};XrFrameState fsr{XR_TYPE_FRAME_STATE};check(xrWaitFrame(session,&wi,&fsr),"xrWaitFrame");
            LARGE_INTEGER loopStart{},qpf{};QueryPerformanceCounter(&loopStart);QueryPerformanceFrequency(&qpf);
            XrFrameBeginInfo bi{XR_TYPE_FRAME_BEGIN_INFO};check(xrBeginFrame(session,&bi),"xrBeginFrame");
            XrViewLocateInfo li{XR_TYPE_VIEW_LOCATE_INFO};li.viewConfigurationType=XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;li.displayTime=fsr.predictedDisplayTime;li.space=space;
            XrViewState vs{XR_TYPE_VIEW_STATE};std::array<XrView,2> views{{{XR_TYPE_VIEW},{XR_TYPE_VIEW}}};uint32_t n=0;
            check(xrLocateViews(session,&li,&vs,2,&n,views.data()),"xrLocateViews");
            const XrViewStateFlags required=XR_VIEW_STATE_ORIENTATION_VALID_BIT|XR_VIEW_STATE_POSITION_VALID_BIT;
            bool valid=n==2&&(vs.viewStateFlags&required)==required;
            request.valid=valid&&fsr.shouldRender;
            if(key(VK_F3)||key(VK_F6)) {panel=!panel;log("Panneau du jeu (F3) : %s",panel?"affiche":"masque");}
            if(key(VK_F4)) {hud=!hud;log("HUD VR (F4) : %s",hud?"affiche":"masque");}
            if(key(VK_F2)) {minimapMode=(minimapMode+1)%3;log("Minicarte (F2) : %s",minimapMode==0?"piedestal":minimapMode==1?"montre (toujours)":"montre (quand on la regarde)");}
            if(key(VK_F11)||fs::exists(outputDirectory/L"CAPTURE.request")) {
                capture=true;std::error_code ec;fs::remove(outputDirectory/L"CAPTURE.request",ec);
            }
            if(fs::exists(outputDirectory/L"STOP.request")) {
                std::error_code ec;fs::remove(outputDirectory/L"STOP.request",ec);exit=true;
            }
            if(key(VK_F7)) {request.mode=(request.mode+1)%4;log("Mode : %u (0 auto, 1 plateau, 2 possession, 3 ecran)",request.mode);}
            if(key(VK_F9)) {
                request.unitsPerMeter=std::max(.1f,request.unitsPerMeter/1.15f);request.tableWidthMeters=std::min(6.f,request.tableWidthMeters*1.15f);
                log("Echelle : table %.2f m (ancien mode %.3f unites/metre)",request.tableWidthMeters,request.unitsPerMeter);
            }
            if(key(VK_F10)) {
                request.unitsPerMeter=std::min(1000.f,request.unitsPerMeter*1.15f);request.tableWidthMeters=std::max(.2f,request.tableWidthMeters/1.15f);
                log("Echelle : table %.2f m (ancien mode %.3f unites/metre)",request.tableWidthMeters,request.unitsPerMeter);
            }
            if(key(VK_PRIOR)) {request.tablePitchDegrees=std::max(0.f,request.tablePitchDegrees-5);log("Inclinaison : %.1f degres",request.tablePitchDegrees);}
            if(key(VK_NEXT)) {request.tablePitchDegrees=std::min(85.f,request.tablePitchDegrees+5);log("Inclinaison : %.1f degres",request.tablePitchDegrees);}
            if(key(VK_F5)||fs::exists(outputDirectory/L"RELOAD.request")) {
                std::error_code ec;fs::remove(outputDirectory/L"RELOAD.request",ec);
                request.unitsPerMeter=setting(configuration,L"TableUnitsPerMeter",request.unitsPerMeter,.1f,1000);
                request.possessionUnitsPerMeter=setting(configuration,L"PossessionUnitsPerMeter",request.possessionUnitsPerMeter,.05f,100);
                request.tablePitchDegrees=setting(configuration,L"TablePitchDegrees",request.tablePitchDegrees,0,85);
                readTableSettings(configuration);placeTable();
                log("Reglages relus : echelle=%.3f possession=%.3f inclinaison=%.1f table=%u largeur=%.2f",request.unitsPerMeter,request.possessionUnitsPerMeter,request.tablePitchDegrees,request.tableAnchor,request.tableWidthMeters);
            }
            controllers.update(session,space,fsr.predictedDisplayTime,sessionState==XR_SESSION_STATE_FOCUSED);
            bool recenter=key(VK_F8)||controllers.recenterPressed();
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
            if(request.tableAnchor&&centered&&tableFixed) {
                // 0.3.2: metal handles. A grip that starts within reach of a handle holds the table until released:
                // one hand turns the table on itself, two hands carry it. Other grips act on the map (game DLL).
                for(int h=0;h<2;++h) {
                    bool g=controllers.valid[h]&&controllers.grip[h];
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
                if(grab.update(held,controllers.poses,request.table,request.tableWidthMeters,.2f,6.f))
                    log("Table relachee : centre=(%.2f,%.2f,%.2f)",request.table.position.x,request.table.position.y,request.table.position.z);
            }
            if(request.tableAnchor&&centered&&!tableFixed) {
                int before=grab.hands;
                if(grab.update(controllers.grip,controllers.poses,request.table,request.tableWidthMeters,.2f,6.f))
                    log("Table relachee : centre=(%.2f,%.2f,%.2f) largeur=%.2f m",request.table.position.x,request.table.position.y,request.table.position.z,request.tableWidthMeters);
                else if(grab.hands>0&&grab.hands!=before) log("Table saisie a %d main(s)",grab.hands);
            }
            pointHud();
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
            if(consume()) ++newImages;
            std::array<XrCompositionLayerProjectionView,2> pv{{{XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW},{XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW}}};
            XrCompositionLayerProjection projection{XR_TYPE_COMPOSITION_LAYER_PROJECTION};projection.space=space;projection.viewCount=2;projection.views=pv.data();
            XrCompositionLayerQuad quad{XR_TYPE_COMPOSITION_LAYER_QUAD};quad.space=space;quad.eyeVisibility=XR_EYE_VISIBILITY_BOTH;
            std::vector<const XrCompositionLayerBaseHeader*> layers;
            std::array<XrCompositionLayerQuad,4> hudLayers{};
            if(valid&&fsr.shouldRender&&last.frame&&GetTickCount64()-lastReceived<1500) {
                bool world=last.worldDraws>0&&request.mode!=3;
                if(world) {
                    for(int e=0;e<2;++e) {pv[e].pose=xrPose(last.eyes[e].pose);pv[e].fov=xrFov(last.eyes[e].fov);pv[e].subImage.swapchain=eyes[e].handle;pv[e].subImage.imageRect.extent={int32_t(request.width),int32_t(request.height)};}
                    layers.push_back(reinterpret_cast<const XrCompositionLayerBaseHeader*>(&projection));
                }
                Pose head=request.eyes[0].pose;head.position=scale(add(request.eyes[0].pose.position,request.eyes[1].pose.position),.5f);
                Pose panelPose{};float panelW=0,panelH=0;
                if(!world||panel) {
                    Pose p=request.anchor;
                    // In game, the full game screen (with its interface) hangs like a wall screen
                    // beyond the table; PanelPosition=0 restores the 0.1.0 side panel.
                    Vec3 offset=!world?Vec3{0,0,-1.6f}:panelPosition==1?Vec3{0,panelHeight,-panelDistance}:Vec3{-.85f,-.25f,-1.1f};
                    p.position=toWorld(request.anchor,offset);
                    quad.pose=xrPose(p);quad.size.width=!world?1.6f:panelPosition==1?panelWidth:.7f;quad.size.height=quad.size.width*last.desktopHeight/last.desktopWidth;
                    quad.subImage.swapchain=desktop.handle;quad.subImage.imageRect.extent={int32_t(last.desktopWidth),int32_t(last.desktopHeight)};
                    layers.push_back(reinterpret_cast<const XrCompositionLayerBaseHeader*>(&quad));
                    panelPose=p;panelW=quad.size.width;panelH=quad.size.height;
                }
                placeHud(world,!world||panel,panelPose,panelW,panelH,head);
            } else {for(auto& q:hudQuads) q.shown=false;
            }
            XrFrameEndInfo ei{XR_TYPE_FRAME_END_INFO};ei.displayTime=fsr.predictedDisplayTime;ei.environmentBlendMode=blend;ei.layerCount=uint32_t(layers.size());ei.layers=layers.data();
            check(xrEndFrame(session,&ei),"xrEndFrame");++frames;
            {LARGE_INTEGER now{};QueryPerformanceCounter(&now);double ms=double(now.QuadPart-loopStart.QuadPart)*1000./double(qpf.QuadPart);loopWorst=std::max(loopWorst,ms);if(ms>8) ++slowLoops;}
            if(GetTickCount64()-lastReport>5000) {
                log("Pointeur : cible=%d clics=%llu. VR frames=%llu (%.1f/s) image=%llu world=%u UI=%u rejected=%u camera=%u source=%s new images=%llu (fresh %llu/%llu, pauses %llu) host loop worst %.1f ms, >8 ms: %llu",
                    pointerTarget,pointerClicks,frames,(frames-framesAtReport)*1000./double(GetTickCount64()-lastReport),last.frame,last.worldDraws,last.uiDraws,last.rejectedDraws,
                    last.cameraMode&~CameraModeDecoupled,(last.cameraMode&CameraModeDecoupled)?"VR thread (headset rate)":"game frame",newImages,freshHits,freshHits+freshMisses,freshPauses,loopWorst,slowLoops);
                newImages=freshHits=freshMisses=0;lastReport=GetTickCount64();framesAtReport=frames;loopWorst=0;slowLoops=0;freshPauses=0;
            }
        }
    }
};
}
int wmain(int argc,wchar_t** argv) {
    wchar_t module[MAX_PATH]{};GetModuleFileNameW(nullptr,module,MAX_PATH);fs::path root=fs::path(module).parent_path();
    logfile=_wfsopen((root/L"DK2VR-host.log").c_str(),L"w",_SH_DENYNO);
    log("Dungeon Keeper II VR 0.7.3 - prototype experimental OpenXR x64 (protocole %u)",ProtocolVersion);
    bool probe=false,noLaunch=false;fs::path game=root/L"Game"/L"DKII-DX.exe";std::wstring arguments=L"-NOMOVIES";
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
    try {Host host;host.initialize(root/L"DK2VR.ini",probe);if(!probe) {if(!noLaunch) host.launch(game,arguments);host.loop();}}
    catch(const std::exception& e) {log("ERREUR : %s",e.what());result=1;}
    if(logfile) {fclose(logfile);logfile=nullptr;}
    return result;
}
