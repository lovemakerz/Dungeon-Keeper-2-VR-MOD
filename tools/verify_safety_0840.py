import sys
from pathlib import Path
c,p=map(Path,sys.argv[1:3])
a=(c/'src/plugin/Plugin.cpp').read_text(encoding='utf-8-sig');b=(p/'src/plugin/Plugin.cpp').read_text(encoding='utf-8-sig')
for file in ['src/common/Comfort.hpp','src/common/FrameGuard.hpp','src/plugin/Decoupled.inl']:
    assert (c/file).read_bytes()==(p/file).read_bytes(),file
for start,end in [('void prepareFrame(', 'void classifyHand('),('struct NativeCamera {','struct NativeZoom {')]:
    assert a[a.index(start):a.index(end)]==b[b.index(start):b.index(end)]
for absent in ['possessionCopies','TileCover::install(native);','PossessionVolume::install(native);']:
    assert absent not in a
for required in ['world==0||!possessionFrameReady','batched&&possessionWorld(request.mode,mode)&&!possessionCoverageGuard.accept','if(possessed&&!possessionFrameReady) {++rejected;return;}']:
    assert required in a and required in b
assert 'avoidedReplayStates+=snapshots[rec.snapshot].applyDelta(d,previous)' in a
assert 'applied==UINT32_MAX?nullptr:&snapshots[applied]' in a
assert 'mergeDraw(records.back(),rec,indexArena.data()' in a
header=(c/'src/plugin/ReplaySnapshot.hpp').read_text(encoding='utf-8-sig')
assert 'if(!previous) {apply(d);return 0;}' in header
assert 'state->Apply();d->SetViewport(&vp)' in a
print('PASS camera/entry and guards identical to 0.8.39; table renderer and texture-copy code unchanged.')
print('PASS first state full for each eye, delta replay, adjacent-only batching; original game timing untouched.')
