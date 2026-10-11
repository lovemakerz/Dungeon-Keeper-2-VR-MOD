#!/bin/sh
# DK2VR 0.9.47.23 - made by LoVeMaKeRz
# Delivered build: Linux + MinGW-w64 GCC 13 (mingw-w64 packages).
# Requires: OpenXR-SDK headers at commit f2448a8797c85814aa892efc1ab8707900fbcc78 in $OPENXR (default ../deps/OpenXR-SDK)
# and the redistributed openxr_loader.dll (x64) in out/ for linking.
set -e
cd "$(dirname "$0")/.."
OPENXR=${OPENXR:-deps/OpenXR-SDK}
mkdir -p out
FL="-std=c++17 -O2 -s -Wall -Wno-unused-variable -Wno-missing-field-initializers -DNOMINMAX -DWIN32_LEAN_AND_MEAN -DUNICODE -D_UNICODE -static -static-libgcc -static-libstdc++"
i686-w64-mingw32-g++-posix $FL -msse2 -mfpmath=sse -shared -o out/DK2VR.dll src/plugin/Plugin.cpp src/plugin/exports.def -ld3d9 -lpsapi -lwinmm -lole32 -luuid -lwindowscodecs -lgdi32
# 0.6.9.3: import library generated with the exact name "openxr_loader.dll" (a loader built with mingw is named
# libopenxr_loader.dll internally: linking against it made the host look for libopenxr_loader.dll at start).
{ echo "LIBRARY openxr_loader.dll"; echo EXPORTS; x86_64-w64-mingw32-objdump -p out/openxr_loader.dll | awk '/\[Ordinal\/Name Pointer\] Table/{f=1;next} f&&/\]/{print $NF} f&&/^$/{if(n++)exit}'; } > out/openxr_loader.def
x86_64-w64-mingw32-dlltool -d out/openxr_loader.def -l out/openxr_loader.imp.a
x86_64-w64-mingw32-g++-posix $FL -municode -I"$OPENXR/include" -o out/DK2VRHost.exe src/host/Host.cpp -Lout -l:openxr_loader.imp.a -ld3d11 -ldxgi -ld3dcompiler_47 -lshlwapi -lole32 -lwindowscodecs -luuid
# Pure geometry tests (run on the build machine).
for t in MathTests TableTests AnchorTests GrabTests DecoupleTests HeldModelTests ComfortTests FrameGuardTests DrawBatchTests CadenceTests FastCrcTests PatchSetTests MotionFitTests CubeFacesTests MenuSplitTests TableTiltTests UprightSpriteTests MenuCursorTests BadgePlaneTests BadgeReadingTests BadgeDepthTests MenuFacesTests MenuMaterialsTests MenuChainsTests PerfProbeTests FastMathTests NativeSkipTests AffinityPolicyTests SnapshotPolicyTests PoolGuardTests FanRuleTests DigitNumberTests NumberSpriteTests KeyboardTests MirrorLayoutTests IniRepairTests; do
    g++ -std=c++17 -O1 -Itests/linux_stub -o out/$t tests/$t.cpp
    ./out/$t
done
# 0.9.47.13: the FastBlt replacement is compared with the wrapper's REAL machine code (32-bit x86 code run natively; skipped only where the build machine cannot build 32-bit programs).
if echo 'int main(){}' | g++ -m32 -x c++ - -o out/m32probe 2>/dev/null; then
    g++ -m32 -std=c++17 -O1 -msse2 -mfpmath=sse -no-pie -fno-pie -o out/FastBltMachineTests tests/FastBltMachineTests.cpp
    ./out/FastBltMachineTests
else
    echo "SKIP: FastBltMachineTests (no 32-bit native build available on this machine)"
fi
# 0.8.68: the REAL possession-motion code of the VR thread (extracted from Decoupled.inl) is checked offline on a synthetic world.
{ sed -n '/^struct MotionInterp {/,/^\/\/ 0.8.65.1 DIAGNOSTIC/p' src/plugin/Decoupled.inl | sed '$d'; sed -n '/^\/\/ 0.8.69: first-person world seen/,/^struct VrThread {/p' src/plugin/Decoupled.inl | sed '$d'; } > out/PossessionMotionExtract.inl
g++ -std=c++17 -O1 -Itests/linux_stub -Iout -o out/PossessionMotionTests tests/PossessionMotionTests.cpp
./out/PossessionMotionTests
i686-w64-mingw32-g++-posix $FL -o out/SnapshotDeltaTests.exe tests/SnapshotDeltaTests.cpp
i686-w64-mingw32-g++-posix $FL -o out/ReplayGpuTests.exe tests/ReplayGpuTests.cpp -ld3d9
i686-w64-mingw32-g++-posix $FL -o out/SurfaceShadowGpuTests.exe tests/SurfaceShadowGpuTests.cpp -ld3d9 -luuid
i686-w64-mingw32-g++-posix $FL -o out/MenuDepthGpuTests.exe tests/MenuDepthGpuTests.cpp -ld3d9


# Actual pedestal mesh helper, independent geometric regression.
sed -n '/^struct PedVertex {/,/^\/\/ 0.6.8: rusted/p' src/plugin/Decoupled.inl | sed '$d' > out/PedestalExtract.inl
g++ -std=c++17 -O1 -Itests/linux_stub -o out/PedestalEdgeTests tests/PedestalEdgeTests.cpp
./out/PedestalEdgeTests

i686-w64-mingw32-g++-posix $FL -msse2 -mfpmath=sse -o out/NativeMenuReplay.exe tools/NativeMenuReplay.cpp -ld3d9

i686-w64-mingw32-g++-posix $FL -msse2 -mfpmath=sse -o out/MenuMaterialsGpuTests.exe tests/MenuMaterialsGpuTests.cpp -ld3d9

i686-w64-mingw32-g++-posix $FL -msse2 -mfpmath=sse -o out/MenuChainsGpuTests.exe tests/MenuChainsGpuTests.cpp -ld3d9
