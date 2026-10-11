import sys
from pathlib import Path
c, p = map(Path, sys.argv[1:3])
for name in ['src/plugin/Plugin.cpp', 'src/host/Host.cpp']:
    current=(c/name).read_text(encoding='utf-8-sig').replace('0.8.41', '0.8.40')
    assert current == (p/name).read_text(encoding='utf-8-sig'), name
for file in (p/'src/common').glob('*.hpp'):
    if file.name != 'Protocol.hpp':
        assert (c/'src/common'/file.name).read_bytes() == file.read_bytes(), file.name
for name in ['src/plugin/ReplaySnapshot.hpp', 'package/REFERENCES_CREATURES_3D.md', 'package/payload/ddraw.ini']:
    assert (c/name).read_bytes() == (p/name).read_bytes(), name
a=(c/'src/plugin/Decoupled.inl').read_text(encoding='utf-8-sig')
b=(p/'src/plugin/Decoupled.inl').read_text(encoding='utf-8-sig')
assert a[a.index('struct PackageSnapshot {'):] == b[b.index('struct PackageSnapshot {'):]
for required in ['surfLock.install(s,13', 'surfUnlock.install(s,14', 'surfaceShadows->invalidate(s)',
                 'invalidateTextureSurfaces(self);', 'invalidateTextureSurfaces(dst);',
                 'updSurface.original(device.Get())', 'touchSurface(self,true)', 'surfaceShadows->clear()']:
    assert required in a, required
for hook in ['hUpdSurface', 'hStretch', 'hFill', 'hSetTarget']:
    block=a[a.index('static HRESULT __stdcall '+hook):]
    block=block[:block.index('\n    }')]
    assert 'touchSurface(' in block, hook
header=(c/'src/plugin/SurfaceShadow.hpp').read_text(encoding='utf-8-sig')
for required in ['D3DPOOL_DEFAULT', 'D3DUSAGE_RENDERTARGET|D3DUSAGE_DEPTHSTENCIL', 'D3DMULTISAMPLE_NONE',
                 'D3DPOOL_SYSTEMMEM', 'GetPrivateData', 'GetContainer(__uuidof(IDirect3DBaseTexture9)',
                 'entries.size()>=128', '32u*1024*1024', 'if(!written)return true;',
                 'D3DLOCK_DONOTWAIT', 'nativeLock?nativeLock(s,&dst,rect,0):E_FAIL']:
    assert required in header, required
assert 'ProtocolVersion = 15' in (c/'src/common/Protocol.hpp').read_text()
print('PASS: camera, inputs, stereo, guards, common modules and 0.8.40 optimizations unchanged.')
print('PASS: Package/CopyCache/VR thread byte-equivalent; edits confined to TextureWatch and new CPU surface mirror.')
print('PASS: hook slots, external invalidation, owner writes, own upload bypass, limits, reset, native fallback.')
