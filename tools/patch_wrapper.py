from pathlib import Path
root = Path(__file__).resolve().parents[1]
folder = root / 'research/dk2-dxwrapper/ddraw'
header = folder / 'IDirectDrawX.h'
content = header.read_text(encoding='utf-8-sig')
updated = content.replace('const UINT AdapterIndex = D3DADAPTER_DEFAULT;', 'UINT AdapterIndex = D3DADAPTER_DEFAULT; // DK2VR: match the OpenXR adapter')
if updated != content:
    header.write_text(updated, encoding='utf-8')
include = '#include "../../../src/plugin/WrapperBridge.hpp" // DK2VR modification\n'
device = folder / 'IDirect3DDeviceX.cpp'
text = device.read_text(encoding='utf-8-sig')
if 'dk2vr_wrapper::' not in text:
    text = text.replace('#include "ddraw.h"', '#include "ddraw.h"\n' + include)
    assert text.count('(*d3d9Device)->DrawPrimitiveUP(') == 3
    assert text.count('(*d3d9Device)->DrawIndexedPrimitiveUP(') == 3
    text = text.replace('(*d3d9Device)->DrawPrimitiveUP(', 'dk2vr_wrapper::draw(*d3d9Device, ')
    text = text.replace('(*d3d9Device)->DrawIndexedPrimitiveUP(', 'dk2vr_wrapper::indexed(*d3d9Device, ')
    # Preserve upstream transformed-VB/strided routing even with Remix conversion disabled.
    marker = 'if (Config.DdrawConvertHomogeneousW && (dwVertexTypeDesc & 0x0E) == D3DFVF_XYZRHW)'
    text = text.replace(marker, 'if ((Config.DdrawConvertHomogeneousW || dk2vr_wrapper::api().draw) && (dwVertexTypeDesc & 0x0E) == D3DFVF_XYZRHW)')
    device.write_text(text,encoding='utf-8')
draw = folder / 'IDirectDrawX.cpp'
text = draw.read_text(encoding='utf-8-sig')
if 'dk2vr_wrapper::' not in text:
    text = text.replace('#include "ddraw.h"', '#include "ddraw.h"\n'+include)
    text = text.replace('hr = d3d9Device->Present(pSourceRect, pDestRect, nullptr, nullptr);',
                        'dk2vr_wrapper::present(d3d9Device);\n\t\t\thr = d3d9Device->Present(pSourceRect, pDestRect, nullptr, nullptr);')
    text = text.replace('d3d9Device->Present(nullptr, nullptr, nullptr, nullptr);',
                        'dk2vr_wrapper::present(d3d9Device);\n\t\t\t\t\t\t\td3d9Device->Present(nullptr, nullptr, nullptr, nullptr);')
    text = text.replace('hr = d3d9Device->Reset(', 'dk2vr_wrapper::reset();\n\t\thr = d3d9Device->Reset(')
    text = text.replace('void m_IDirectDrawX::ReleaseD9Device()\n{', 'void m_IDirectDrawX::ReleaseD9Device()\n{\n\tdk2vr_wrapper::reset();')
    # Adapter must match the LUID required by xrGetD3D11GraphicsRequirementsKHR.
    text = text.replace('d3d9Object = Direct3DCreate9(D3D_SDK_VERSION);',
                        'd3d9Object = Direct3DCreate9(D3D_SDK_VERSION);\n\t\tif (d3d9Object) AdapterIndex = dk2vr_wrapper::adapter(d3d9Object, AdapterIndex);')
    draw.write_text(text,encoding='utf-8')
# Avoid patching process-wide wait functions: these optional hooks use static
# thread-local storage from third-party threads during DLL initialization.
# Windhawk exposes that upstream race before DKII can create its first window.
wait_file = folder.parent / 'DDrawCompat/v0.3.2/Win32/WaitFunctions.cpp'
text = wait_file.read_text(encoding='utf-8-sig')
if '// DK2VR: preserve native Windows wait functions.' not in text:
    marker = '\t\tvoid installHooks()\n\t\t{'
    assert text.count(marker) == 1
    text = text.replace(marker, marker + '\n\t\t\t// DK2VR: preserve native Windows wait functions.\n\t\t\treturn;')
    wait_file.write_text(text, encoding='utf-8')
# In native ddraw.dll mode no system DLL is loaded for Dd7to9. The fork's
# conditional ShimProc rejects null original exports; bind the converter directly.
main = folder.parent / 'Dllmain/Dllmain.cpp'
text = main.read_text(encoding='utf-8-sig')
old = ('// Add Dd7to9 to the chain\n\t\t\tif (Config.Dd7to9)\n\t\t\t{\n'
       '\t\t\t\tusing namespace ddraw;\n\t\t\t\tusing namespace DdrawWrapper;\n'
       '\t\t\t\tVISIT_PROCS_DDRAW(SHIM_WRAPPED_PROC);\n'
       '\t\t\t\tVISIT_PROCS_DDRAW_SHARED(SHIM_WRAPPED_PROC);')
new = old.replace('VISIT_PROCS_DDRAW(SHIM_WRAPPED_PROC);',
    '// DK2VR: initialize converter exports without a system DirectDraw DLL.\n\t\t\t\tVISIT_PROCS_DDRAW(SET_WRAPPED_PROC);').replace(
    'VISIT_PROCS_DDRAW_SHARED(SHIM_WRAPPED_PROC);','VISIT_PROCS_DDRAW_SHARED(SET_WRAPPED_PROC);')
if '// DK2VR: initialize converter exports' not in text:
    assert text.count(old) == 1
    main.write_text(text.replace(old,new), encoding='utf-8')
print('DK2VR wrapper integration ready')
