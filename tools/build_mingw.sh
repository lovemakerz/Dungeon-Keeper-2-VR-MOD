#!/bin/sh
# DK2VR 0.2.0 - build used for the delivered binaries (Linux + MinGW-w64 GCC 13, Ubuntu package mingw-w64).
# Requires: OpenXR-SDK headers at commit f2448a8797c85814aa892efc1ab8707900fbcc78 in $OPENXR (default ../deps/OpenXR-SDK)
# and the redistributed openxr_loader.dll (x64) in out/ for linking.
set -e
cd "$(dirname "$0")/.."
OPENXR=${OPENXR:-../deps/OpenXR-SDK}
mkdir -p out
FL="-std=c++17 -O2 -s -Wall -Wno-unused-variable -Wno-missing-field-initializers -DNOMINMAX -DWIN32_LEAN_AND_MEAN -DUNICODE -D_UNICODE -static -static-libgcc -static-libstdc++"
i686-w64-mingw32-g++-posix $FL -msse2 -mfpmath=sse -shared -o out/DK2VR.dll src/plugin/Plugin.cpp src/plugin/exports.def -ld3d9 -lpsapi -lwinmm
x86_64-w64-mingw32-g++-posix $FL -municode -I"$OPENXR/include" -o out/DK2VRHost.exe src/host/Host.cpp -Lout -l:openxr_loader.dll -ld3d11 -ldxgi -ld3dcompiler_47 -lshlwapi -lole32 -lwindowscodecs -luuid
# Pure geometry tests (run on the build machine).
for t in MathTests TableTests AnchorTests GrabTests DecoupleTests; do g++ -std=c++17 -O1 -Itests/linux_stub -o out/$t tests/$t.cpp && ./out/$t; done
