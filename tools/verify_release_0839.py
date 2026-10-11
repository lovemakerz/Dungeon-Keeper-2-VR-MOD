import sys
import zipfile
import hashlib
from pathlib import Path
import pefile

release = Path(sys.argv[1])
previous = Path(sys.argv[2])
for name, architecture in [('payload/DK2VR.dll', 0x14c), ('DK2VRHost.exe', 0x8664), ('openxr_loader.dll', 0x8664), ('payload/ddraw.dll', 0x14c)]:
    pe = pefile.PE(str(release / name))
    assert pe.FILE_HEADER.Machine == architecture, name
    dependencies = [d.dll.decode() for d in pe.DIRECTORY_ENTRY_IMPORT]
    assert not any('libgcc' in d.lower() or 'libstdc' in d.lower() or 'libwinpthread' in d.lower() for d in dependencies), dependencies
    print('PASS architecture/static runtime:', name)
for name in ['openxr_loader.dll', 'payload/ddraw.dll', 'payload/ddraw.ini', 'REFERENCES_CREATURES_3D.md']:
    assert (release/name).read_bytes() == (previous/name).read_bytes(), name
print('PASS: unchanged loader, wrapper, wrapper settings, held-creature references.')
ini = (release/'DK2VR.ini').read_text(encoding='utf-8-sig')
for value in ['PossessionSnapTurn=0', 'PossessionVignette=0', 'HeldCreature3D=0', 'Possession360=0']:
    assert value in ini, value
for name in ['Prepare-Game.ps1', 'LIRE_MOI_FR.md']:
    assert '0.8.39' in (release/name).read_text(encoding='utf-8-sig')
assert not (release/'Game').exists()
with zipfile.ZipFile(str(release)+'.zip') as archive:
    assert archive.testzip() is None
    for name in ['DK2VRHost.exe', 'payload/DK2VR.dll', 'payload/ddraw.dll', 'openxr_loader.dll', 'VALIDATION_0.8.39.md']:
        assert archive.read(release.name+'/'+name) == (release/name).read_bytes(), name
print('PASS: ready-to-test ZIP integrity, defaults and matching packaged binaries.')
for name in ['DK2VRHost.exe', 'payload/DK2VR.dll']:
    print(name, hashlib.sha256((release/name).read_bytes()).hexdigest())
