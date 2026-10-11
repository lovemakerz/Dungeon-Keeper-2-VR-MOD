// DK2VR 0.9.47.12 - made by LoVeMaKeRz
#include "../src/common/AffinityPolicy.hpp"
#include <cstdio>
#include <cstdlib>
using namespace affinity;
static int fails = 0;
#define CHECK(c, msg) do { if (!(c)) { std::printf("FAIL: %s\n", msg); ++fails; } } while (0)
int main() {
    CHECK(freeModule("C:\\Windows\\System32\\DriverStore\\FileRepository\\nv_dispi.inf\\nvd3dum.dll"), "driver worker threads are free");
    CHECK(freeModule("D:\\Game\\DK2VR.dll"), "mod threads are free");
    CHECK(freeModule("C:\\Windows\\SysWOW64\\D3D9.DLL"), "case-insensitive d3d9");
    CHECK(freeModule("dxgi.dll") && freeModule("d3d11.dll"), "runtime");
    CHECK(!freeModule("D:\\Game\\DKII-DX.exe"), "game exe stays pinned");
    CHECK(!freeModule("C:\\Windows\\SysWOW64\\DSOUND.dll"), "audio stays pinned");
    CHECK(!freeModule("QMIXER.dll") && !freeModule("weanetr.dll") && !freeModule("DDRAW.dll"), "game side modules stay pinned");
    CHECK(!freeModule("") && !freeModule("ntdll.dll") && !freeModule("kernel32.dll"), "unknown / generic start modules stay pinned");
    CHECK(!freeModule("nvidia_unrelated_tool.dll"), "an 'nv' prefix alone is not enough");
    CHECK(maskFor(true, "nvd3dum.dll", 1, 0xFFFF) == 1, "game thread keeps its mask even if its start module looked free");
    CHECK(maskFor(false, "nvd3dum.dll", 1, 0xFFFF) == 0xFFFF, "driver thread gets every core");
    CHECK(maskFor(false, "qmixer.dll", 1, 0xFFFF) == 1, "game thread of the audio keeps the single core");
    CHECK(restricted(1, 0xFFFF), "single core of 16 = restricted");
    CHECK(!restricted(0xFFFF, 0xFFFF), "unrestricted process untouched");
    CHECK(!restricted(1, 1), "single-core PC untouched");
    CHECK(!restricted(0, 0xFFFF), "zero mask (query failed) untouched");
    CHECK(!restricted(0x10000, 0xFFFF), "mask outside the system mask untouched");
    CHECK(restricted(0x3, 0xFFFF), "two cores of 16 = restricted");
    if (fails) return 1;
    std::printf("PASS: AffinityPolicyTests (only driver / Direct3D / DK2VR threads leave the single core)\n");
    return 0;
}
