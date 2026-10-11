// DK2VR 0.9.47.19 - made by LoVeMaKeRz
#include "../src/common/FanRule.hpp"
#include <cstdio>
#include <vector>
#define CHECK(c, ...) do {if(!(c)) {std::printf("FAIL: " __VA_ARGS__); std::printf("\n"); return 1;}} while(0)
static std::vector<unsigned char> mesh(const std::vector<uint32_t>& colours, unsigned stride = 28) {
    std::vector<unsigned char> b(colours.size() * stride, 0);
    for(size_t i = 0; i < colours.size(); ++i) std::memcpy(&b[i * stride + 16], &colours[i], 4);
    return b;
}
int main() {
    const double minLight = 96;
    // Colours seen in the water capture (rules off, report 232654): alpha 0xB2, R 0x64..0xFF, G and B 0xFF / 0xFA / 0xE8.
    {
        std::vector<uint32_t> water; const uint32_t c[] = {0xB2FFFFFF, 0xB27EFFFF, 0xB271FFFF, 0xB27DFFFF, 0xB264FFFA, 0xB276FFE8};
        for(int i = 0; i < 300; ++i) water.push_back(c[i % 6]);
        const auto b = mesh(water); const double l = fanrule::meanLight(b.data(), 28, unsigned(water.size()));
        CHECK(fanrule::paleMesh(l, minLight), "water mesh must be pale (mean %.1f)", l);
        CHECK(l > 200 && l < 256, "water mean brightness %.1f", l);
    }
    // Dark soot / shadow fans (black to dark brown, any alpha) must stay hidden.
    {
        const uint32_t soot[] = {0x80000000, 0xA0101010, 0x60202020, 0x40301810, 0xFF000000, 0x30403020};
        for(uint32_t s : soot) {
            std::vector<uint32_t> fan(227, s); const auto b = mesh(fan);
            const double l = fanrule::meanLight(b.data(), 28, 227);
            CHECK(!fanrule::paleMesh(l, minLight), "dark fan 0x%08X must not be pale (mean %.1f)", s, l);
        }
        // a fan with a dark hub and a slightly lighter rim (average still dark)
        std::vector<uint32_t> fan; for(int i = 0; i < 227; ++i) fan.push_back(i == 0 ? 0xFF000000 : 0x50505050);
        const auto b = mesh(fan); CHECK(!fanrule::paleMesh(fanrule::meanLight(b.data(), 28, 227), minLight), "dark fan with lighter rim");
    }
    // Cannot be measured -> not pale (the rule keeps hiding, as in 0.9.47.18).
    {
        CHECK(fanrule::meanLight(nullptr, 28, 10) < 0 && !fanrule::paleMesh(fanrule::meanLight(nullptr, 28, 10), minLight), "null vertices");
        const auto b = mesh({0xFFFFFFFF}); CHECK(fanrule::meanLight(b.data(), 28, 0) < 0, "zero count");
        CHECK(fanrule::meanLight(b.data(), 16, 1) < 0, "stride too small for a colour");
    }
    // Boundary: exactly at minLight is pale, one below is not.
    {
        const auto b = mesh({0xFF606060}); CHECK(fanrule::paleMesh(fanrule::meanLight(b.data(), 28, 1), 96), "0x60 = 96 is pale at 96");
        const auto c = mesh({0xFF5F5F5F}); CHECK(!fanrule::paleMesh(fanrule::meanLight(c.data(), 28, 1), 96), "0x5F = 95 is not pale at 96");
    }
    std::printf("PASS: FanRuleTests (water colours of the capture are pale, dark soot fans of every alpha stay hidden, unmeasurable meshes stay hidden, boundary 96)\n");
    return 0;
}
