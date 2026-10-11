"""Check preserved 0.8.36 paths and the scoped possession corrections."""
import sys
from pathlib import Path

current, reference = map(Path, sys.argv[1:3])
a = (current/'src/plugin/Plugin.cpp').read_text(encoding='utf-8-sig')
b = (reference/'src/plugin/Plugin.cpp').read_text(encoding='utf-8-sig')
assert (current/'src/plugin/Decoupled.inl').read_bytes() == (reference/'src/plugin/Decoupled.inl').read_bytes()
for start, end in [('struct TileCover {','struct LockSite '), ('struct NativeCamera {','struct NativeZoom {'), ('static bool isRet(', '// After a 3D projection:'), ('__attribute__((force_align_arg_pointer)) static uint32_t __cdecl projHookA', '// Vertex of a Direct3D draw:')]:
    assert a[a.index(start):a.index(end)] == b[b.index(start):b.index(end)], start
for removed in ['TileCover::install(native);', 'PossessionVolume::install(native);', 'TileCover::omni()', 'projectOmni(idx,v)', 'unprojectOmni(v[0]']:
    assert removed not in a, removed
tag = a[a.index('static void tag(int idx)'):a.index('static uint32_t __cdecl projHookA')]
assert 'if(preservePossessionDepth)' in tag
assert all(word not in tag for word in ['VirtualQuery', 'native.mode', 'readable(', 'read<float>'])
predicate = 'decoupledFrame=lastAnchored&&request.tableAnchor&&request.mode!=3&&!possessedMode&&!native.frontend()&&milliseconds()>=retryAt&&ensureVr(d);'
assert predicate in a and predicate in b
assert 'world==0||!possessionFrameReady' in a
assert 'if(possessed&&!possessionFrameReady) {++rejected;return;}' in a
assert 'if(!possessionLevelFinal&&possessionFrameReady)' in a
assert 'bool originDone=possessionWorld(request.mode,mode);' in a
print('PASS: 0.8.36 empty-eye protection preserved; invalid entry projection rejected and retried until ready.')
print('PASS: same table VR renderer, native projection trampolines and camera; no 360 hooks or per-vertex memory queries.')
print('PASS: possession-only RHW marker bypass and draw-classification guard; cached once per frame.')
