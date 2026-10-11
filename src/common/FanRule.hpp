#pragma once
// DK2VR 0.9.47.19 - made by LoVeMaKeRz
// Forge smoke fan rule, water exemption: the rule hides flat alpha-blended meshes drawn at a single depth (the DARK soot fans of the forge).
// The big draws of the WATER SURFACE fit the same size / depth test, but their vertex colours are pale turquoise (0xB27EFFFF), so a mesh whose
// mean vertex brightness (mean of R, G, B over all vertices, 0..255) reaches 'minLight' is not a soot fan. Pure logic, tests/FanRuleTests.cpp.
#include <cstddef>
#include <cstdint>
#include <cstring>
namespace fanrule {
// 'verts' = vertex bytes, diffuse colour (D3DCOLOR 0xAARRGGBB) at byte 'colourOffset' of each vertex. Returns the mean brightness, or -1 if it cannot be measured.
inline double meanLight(const unsigned char* verts, unsigned stride, unsigned count, unsigned colourOffset = 16) {
    if(!verts || count == 0 || stride < colourOffset + 4) return -1.0;
    double sum = 0;
    for(unsigned i = 0; i < count; ++i) {
        uint32_t c = 0; std::memcpy(&c, verts + size_t(i) * stride + colourOffset, 4);
        sum += double((c >> 16) & 255u) + double((c >> 8) & 255u) + double(c & 255u);
    }
    return sum / (3.0 * double(count));
}
inline bool paleMesh(double light, double minLight) {return light >= 0 && light >= minLight;}
}  // namespace fanrule
