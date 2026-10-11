#pragma once
// DK2VR 0.9.47.15 - made by LoVeMaKeRz
// FastBlt: the DxWrapper copy of the game's software blit (DDRAW.dll, the row loop at RVA 0x11AA80 .. 0x11AB2E) converts every destination pixel with a
// call to the C runtime's float->int helper and copies it one 32-bit pixel at a time. Measured on the 0.9.47.12 report: that loop alone is 8 to 12 percent of the
// game thread's CPU time in the menu, in the dungeon and in possession (texture uploads of the engine's 128x128 holders, menu surfaces).
// This header holds (1) the exact bytes of the original loop (a patch is applied ONLY if the wrapper's code is byte-identical), (2) a replacement row
// routine with the same results for every input (same float->int truncation, same colour-key rule, same mirror rule), (3) the entry stub that is jumped to.
// The pure logic is tested against the original machine code itself (tests/FastBltMachineTests.cpp, run natively as 32-bit code).
#include <cstdint>
#include <cstring>
#include <initializer_list>

namespace fastblt {

constexpr uint32_t PatchRva = 0x11AA80, PatchLength = 174;     // first byte of the row loop .. first byte after it (resume point)
constexpr uint32_t ResumeRva = PatchRva + PatchLength;           // 0x11AB2E
static const unsigned char expected[PatchLength + 6] = {
    0x33, 0xFF, 0x39, 0xBD, 0x1C, 0xFE, 0xFF, 0xFF, 0x0F, 0x8E, 0xA0, 0x00, 0x00, 0x00, 0x80, 0xBD,
    0x08, 0xFE, 0xFF, 0xFF, 0x00, 0x74, 0x52, 0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x66, 0x0F, 0x6E, 0xC7, 0x0F, 0x5B, 0xC0, 0xF3, 0x0F, 0x59, 0xC1, 0xE8, 0x30, 0xFA, 0x0A, 0x00,
    0x8B, 0x8D, 0x18, 0xFE, 0xFF, 0xFF, 0x2B, 0xC8, 0x80, 0xBD, 0x58, 0xFE, 0xFF, 0xFF, 0x00, 0x8B,
    0x85, 0x20, 0xFE, 0xFF, 0xFF, 0x8B, 0x44, 0x88, 0xFC, 0x74, 0x08, 0x3B, 0x85, 0x3C, 0xFE, 0xFF,
    0xFF, 0x74, 0x03, 0x89, 0x04, 0xBE, 0xF3, 0x0F, 0x10, 0x8D, 0x4C, 0xFE, 0xFF, 0xFF, 0x47, 0x3B,
    0xBD, 0x1C, 0xFE, 0xFF, 0xFF, 0x7C, 0xB9, 0xEB, 0x45, 0x33, 0xFF, 0x0F, 0x1F, 0x44, 0x00, 0x00,
    0x66, 0x0F, 0x6E, 0xC7, 0x0F, 0x5B, 0xC0, 0xF3, 0x0F, 0x59, 0xC1, 0xE8, 0xE0, 0xF9, 0x0A, 0x00,
    0x80, 0xBD, 0x58, 0xFE, 0xFF, 0xFF, 0x00, 0x8B, 0x8D, 0x20, 0xFE, 0xFF, 0xFF, 0x8B, 0x04, 0x81,
    0x74, 0x08, 0x3B, 0x85, 0x3C, 0xFE, 0xFF, 0xFF, 0x74, 0x03, 0x89, 0x04, 0xBE, 0xF3, 0x0F, 0x10,
    0x8D, 0x4C, 0xFE, 0xFF, 0xFF, 0x47, 0x3B, 0xBD, 0x1C, 0xFE, 0xFF, 0xFF, 0x7C, 0xC2,
    0x8B, 0x85, 0x24, 0xFE, 0xFF, 0xFF   // the six bytes after the loop: mov eax,[ebp-0x1DC]
};

// 0.9.47.14: the mod itself already replaces the two float->int helper calls of this loop (FastStretch, offsets 0x2B and 0x7B) by "cvttss2si eax,xmm0 ; nop" at start-up,
// so the memory seen at install time is either the wrapper's file bytes or the same bytes with those two 5-byte call sites rewritten. Both are accepted; anything else is not.
constexpr uint32_t CallSiteA = 0x2B, CallSiteB = 0x7B;
static const unsigned char cvtSite[5] = {0xF3, 0x0F, 0x2C, 0xC0, 0x90};
inline bool codeMatches(const unsigned char* at) {
    for (uint32_t i = 0; i < sizeof(expected); ++i) {
        if ((i >= CallSiteA && i < CallSiteA + 5) || (i >= CallSiteB && i < CallSiteB + 5)) continue;
        if (at[i] != expected[i]) return false;
    }
    for (uint32_t site : {CallSiteA, CallSiteB}) {
        const bool original = std::memcmp(at + site, expected + site, 5) == 0;
        const bool rewritten = std::memcmp(at + site, cvtSite, 5) == 0;
        if (!original && !rewritten) return false;
    }
    return true;
}

// The original semantics, one pixel at a time.
inline void referenceRow(uint32_t* dst, const uint32_t* src, int width, float scale, int mirror, int srcW, int ckOn, uint32_t key) {
    for (int i = 0; i < width; ++i) {
        const int idx = static_cast<int>(static_cast<float>(i) * scale);
        const uint32_t p = mirror ? src[srcW - idx - 1] : src[idx];
        if (!ckOn || p != key) dst[i] = p;
    }
}

inline bool overlap(const uint32_t* a, const uint32_t* b, int n) {
    const uintptr_t x = reinterpret_cast<uintptr_t>(a), y = reinterpret_cast<uintptr_t>(b), len = uintptr_t(n) * 4u;
    return x < y + len && y < x + len;
}

// 0.9.47.15: a stretched blit sends hundreds of rows with the SAME geometry (width, source width, scale, mirror). The source index of every destination pixel then depends on
// nothing but that geometry, so it is computed once (the very same float multiply and truncation as the original) and reused by the following rows.
struct IndexCache {
    static constexpr int MaxWidth = 8192, MaxSpan = 8192;
    int width = -1, srcW = 0, mirror = 0;
    uint32_t scaleBits = 0;
    int minIdx = 0, maxIdx = 0;             // range of source pixels the row reads
    unsigned long long hits = 0, builds = 0, dupRows = 0;
    bool outValid = false;
    unsigned long long outBuild = 0;
    int idx[MaxWidth];
    uint32_t snap[MaxSpan];                  // the source pixels of the last computed row (range minIdx..maxIdx)
    uint32_t out[MaxWidth];                  // the last computed destination row
    const int* get(int w, int sw, float scale, int mir) {
        if (w <= 0 || w > MaxWidth) return nullptr;
        uint32_t sb; std::memcpy(&sb, &scale, 4);
        if (w == width && sw == srcW && mir == mirror && sb == scaleBits) {++hits; return idx;}
        int lo = 0, hi = 0;
        for (int i = 0; i < w; ++i) {
            const int id = static_cast<int>(static_cast<float>(i) * scale);
            const int v = mir ? sw - id - 1 : id;
            idx[i] = v;
            if (i == 0 || v < lo) lo = v;
            if (i == 0 || v > hi) hi = v;
        }
        width = w; srcW = sw; mirror = mir; scaleBits = sb; minIdx = lo; maxIdx = hi; ++builds; outValid = false;
        return idx;
    }
    // Two consecutive destination rows of a vertical upscale are built from the SAME source row: when the source pixels this row reads are byte for byte
    // those of the row computed just before (same geometry, no colour key), its result is simply copied.
    bool spanOk() const {return maxIdx >= minIdx && (long long)maxIdx - minIdx + 1 <= MaxSpan;}
};

// Same results as referenceRow, faster for the usual case (scale exactly 1: straight copy).
inline void fastRow(uint32_t* dst, const uint32_t* src, int width, float scale, int mirror, int srcW, int ckOn, uint32_t key, IndexCache* cache = nullptr) {
    if (width <= 0) return;
    if (!mirror && scale == 1.0f) {
        if (!ckOn) {
            if (!overlap(dst, src, width)) std::memcpy(dst, src, size_t(width) * 4u);
            else for (int i = 0; i < width; ++i) dst[i] = src[i];
        } else {
            for (int i = 0; i < width; ++i) {const uint32_t p = src[i]; if (p != key) dst[i] = p;}
        }
        return;
    }
    if (cache) {
        if (const int* tab = cache->get(width, srcW, scale, mirror)) {
            if (!ckOn && cache->spanOk()) {
                const uint32_t* base = src + cache->minIdx;
                const size_t span = size_t(cache->maxIdx - cache->minIdx + 1);
                if (!overlap(dst, base, int(span)) && !overlap(dst, src, width)) {
                    if (cache->outValid && cache->outBuild == cache->builds && std::memcmp(cache->snap, base, span * 4u) == 0) {
                        std::memcpy(dst, cache->out, size_t(width) * 4u);
                        ++cache->dupRows;
                        return;
                    }
                    for (int i = 0; i < width; ++i) cache->out[i] = src[tab[i]];
                    std::memcpy(dst, cache->out, size_t(width) * 4u);
                    std::memcpy(cache->snap, base, span * 4u);
                    cache->outValid = true; cache->outBuild = cache->builds;
                    return;
                }
            }
            // element by element, in order: identical to the original even when source and destination overlap
            if (!ckOn) for (int i = 0; i < width; ++i) dst[i] = src[tab[i]];
            else for (int i = 0; i < width; ++i) {const uint32_t p = src[tab[i]]; if (p != key) dst[i] = p;}
            return;
        }
    }
    referenceRow(dst, src, width, scale, mirror, srcW, ckOn, key);
}

}  // namespace fastblt

#if defined(__i386__)
#ifdef _WIN32
#define DK2_ASM_SYM "_"
#else
#define DK2_ASM_SYM ""
#endif
// Entry stub: reached by a jump from the first bytes of the original loop, with the wrapper function's frame (ebp) and the destination row pointer in esi.
// It reads the loop's inputs from that frame (offsets read from the original machine code), calls dk2vr_bltRow, then continues after the loop. Free
// registers at the resume point: eax, ecx, edx, edi, xmm0 (verified in the code after the loop); ebx, esi, ebp are preserved; xmm1 is left at the scale,
// exactly as the original loop leaves it.
extern "C" {
void* dk2vr_bltResume = nullptr;
void dk2vr_bltStub();
__attribute__((used, force_align_arg_pointer)) void __attribute__((cdecl)) dk2vr_bltRow(uint32_t* dst, const uint32_t* src, int width, float scale, int mirror, int srcW, int ckOn, uint32_t key);
}
asm(".text\n.globl " DK2_ASM_SYM "dk2vr_bltStub\n" DK2_ASM_SYM "dk2vr_bltStub:\n"
    "\txorl %edi,%edi\n"
    "\tcmpl %edi,-0x1e4(%ebp)\n"
    "\tjle 1f\n"
    "\tpushl -0x1c4(%ebp)\n"
    "\tmovzbl -0x1a8(%ebp),%eax\n\tpushl %eax\n"
    "\tpushl -0x1e8(%ebp)\n"
    "\tmovzbl -0x1f8(%ebp),%eax\n\tpushl %eax\n"
    "\tpushl -0x1b4(%ebp)\n"
    "\tpushl -0x1e4(%ebp)\n"
    "\tpushl -0x1e0(%ebp)\n"
    "\tpushl %esi\n"
    "\tcall " DK2_ASM_SYM "dk2vr_bltRow\n"
    "\taddl $32,%esp\n"
    "\tmovss -0x1b4(%ebp),%xmm1\n"
    "1:\n\tjmp *" DK2_ASM_SYM "dk2vr_bltResume\n");
#endif
