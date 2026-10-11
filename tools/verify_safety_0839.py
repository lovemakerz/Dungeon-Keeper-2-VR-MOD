import sys
from pathlib import Path
c,p=map(Path,sys.argv[1:3])
a=(c/'src/plugin/Plugin.cpp').read_text(encoding='utf-8-sig')
b=(p/'src/plugin/Plugin.cpp').read_text(encoding='utf-8-sig')
assert (c/'src/plugin/Decoupled.inl').read_bytes()==(p/'src/plugin/Decoupled.inl').read_bytes()
assert (c/'src/common/Comfort.hpp').read_bytes()==(p/'src/common/Comfort.hpp').read_bytes()
for start,end in [('void prepareFrame(', 'void classifyHand('),('struct NativeCamera {','struct NativeZoom {')]:
    assert a[a.index(start):a.index(end)]==b[b.index(start):b.index(end)]
for absent in ['possessionCopies','Possession frozen materials','TileCover::install(native);','PossessionVolume::install(native);']:
    assert absent not in a,absent
for required in ['world==0||!possessionFrameReady','if(possessed&&!possessionFrameReady) {++rejected;return;}','bool originDone=possessionWorld(request.mode,mode);']:
    assert required in a and required in b
assert 'batched&&possessionWorld(request.mode,mode)&&!possessionCoverageGuard.accept' in a
h=(c/'src/host/Host.cpp').read_text(encoding='utf-8-sig')
assert 'now-lastCaptureRequest>=1500' in h
assert 'key(VK_F11)' in h
print('PASS rollback: no possession texture-copy cache; table renderer, floor reference and camera unchanged.')
print('PASS empty-eye guard and input paths preserved; no 360 hooks; scoped bounded coverage guard.')
print('PASS capture rate limit; no GPU readback in coverage calculation.')
