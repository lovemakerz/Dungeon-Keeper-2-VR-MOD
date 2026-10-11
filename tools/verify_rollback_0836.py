"""Verify the safety rollback against preserved 0.8.34 sources."""
import sys
from pathlib import Path

current, reference = map(Path, sys.argv[1:3])
a = (current/'src/plugin/Plugin.cpp').read_text(encoding='utf-8-sig')
b = (reference/'src/plugin/Plugin.cpp').read_text(encoding='utf-8-sig')
assert (current/'src/plugin/Decoupled.inl').read_bytes() == (reference/'src/plugin/Decoupled.inl').read_bytes()
for start, end in [('struct TileCover {','struct LockSite '), ('struct EngineOrigin {','void engineOriginNote'), ('struct NativeCamera {','struct NativeZoom {')]:
    assert a[a.index(start):a.index(end)] == b[b.index(start):b.index(end)], start
for removed in ['TileCover::install(native);', 'PossessionVolume::install(native);', 'TileCover::omni()', 'projectOmni(idx,v)', 'unprojectOmni(v[0]']:
    assert removed not in a, removed
predicate = 'decoupledFrame=lastAnchored&&request.tableAnchor&&request.mode!=3&&!possessedMode&&!native.frontend()&&milliseconds()>=retryAt&&ensureVr(d);'
assert predicate in a and predicate in b
assert 'camera.orientation,possessionLevel' in a
assert 'possessionWorld(request.mode,mode)&&world==0' in a
assert 'Possession360=0' in (current/'package/DK2VR.ini').read_text(encoding='utf-8-sig')
print('PASS: native camera/projection/tile code and VR thread restored byte-for-byte to 0.8.34 source blocks.')
print('PASS: 360 hooks absent from installation and vertex paths; possession uses original immediate stereo rendering.')
print('PASS: floor-level rotation and empty-eye rejection retained; 360 disabled in defaults.')
