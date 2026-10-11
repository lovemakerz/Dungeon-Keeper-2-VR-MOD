// DK2VR 0.9.47.13 - made by LoVeMaKeRz
// FastBlt checked against the ORIGINAL MACHINE CODE of the wrapper's row loop (the 174 bytes in FastBlt.hpp are executed, with the float->int helper
// replaced by a truncating clone) and against the replacement stub + row routine, on thousands of random rows (stretch / mirror / colour key).
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#endif
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "../src/plugin/FastBlt.hpp"

extern "C" __attribute__((cdecl)) void dk2vr_bltRow(uint32_t* dst, const uint32_t* src, int width, float scale, int mirror, int srcW, int ckOn, uint32_t key) {
    fastblt::fastRow(dst, src, width, scale, mirror, srcW, ckOn, key);
}

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { ++fails; if (fails < 12) {std::printf("FAIL: "); std::printf(__VA_ARGS__); std::printf("\n");} } } while (0)

static unsigned char* code;      // original loop copy, stub landing
static uint32_t rnd() {static uint32_t s = 0x1234567u; s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s;}

struct Frame {alignas(16) unsigned char b[0x400];};

static void setFrame(Frame& f, const uint32_t* src, int width, int srcW, int mirror, float scale, int ck, uint32_t key) {
    unsigned char* ebp = f.b + 0x300;
    std::memset(f.b, 0xCC, sizeof(f.b));
    auto w32 = [&](int off, uint32_t v) {std::memcpy(ebp + off, &v, 4);};
    w32(-0x1e4, uint32_t(width)); w32(-0x1e0, uint32_t(uintptr_t(src))); w32(-0x1e8, uint32_t(srcW));
    ebp[-0x1f8] = mirror ? 1 : 0; std::memcpy(ebp - 0x1b4, &scale, 4); ebp[-0x1a8] = ck ? 1 : 0; w32(-0x1c4, key);
}

// Runs code at 'entry' as the loop would run inside the wrapper: ebp = frame, esi = dst, xmm1 = scale. Reports ebx / esi / ebp afterwards.
static void run(void* entry, Frame& f, uint32_t* dst, float scale, void*& ebxOut, void*& esiOut, void*& ebpOut) {
    void* e = entry;
    void* ebxv = f.b + 0x300;
    void* esiv = dst;
    void* ebpv = nullptr;
    asm volatile(
        "movss %[sc],%%xmm1\n\t"
        "pushl %%ebp\n\t"
        "movl %%ebx,%%ebp\n\t"
        "call *%%eax\n\t"
        "movl %%ebp,%%edi\n\t"
        "popl %%ebp\n\t"
        : "+a"(e), "+b"(ebxv), "+S"(esiv), "=D"(ebpv)
        : [sc]"m"(scale)
        : "ecx", "edx", "xmm0", "xmm1", "memory", "cc");
    ebxOut = ebxv; esiOut = esiv; ebpOut = ebpv;
}

int main() {
#ifdef _WIN32
    code = static_cast<unsigned char*>(VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
#else
    code = static_cast<unsigned char*>(mmap(nullptr, 4096, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (code == MAP_FAILED) code = nullptr;
#endif
    if (!code) {std::printf("FAIL: no executable memory\n"); return 1;}
    // original loop + a 'ret' where the wrapper continues
    std::memcpy(code, fastblt::expected, fastblt::PatchLength);
    code[fastblt::PatchLength] = 0xC3;
    // the two calls of the original loop go to a truncating clone of the runtime helper
    const unsigned char helper[] = {0xF3, 0x0F, 0x2C, 0xC0, 0x99, 0xC3};   // cvttss2si eax,xmm0 ; cdq ; ret
    std::memcpy(code + 256, helper, sizeof(helper));
    int calls = 0;
    for (int off : {0x2B, 0x7B}) {
        if (code[off] != 0xE8) {std::printf("FAIL: call not where expected at %d\n", off); return 1;}
        const int rel = int((code + 256) - (code + off + 5));
        std::memcpy(code + off + 1, &rel, 4); ++calls;
    }
    // variant 2: the loop as it is in memory once FastStretch has rewritten its two calls (cvttss2si ; nop)
    unsigned char* code2 = code + 1024;
    std::memcpy(code2, fastblt::expected, fastblt::PatchLength);
    code2[fastblt::PatchLength] = 0xC3;
    for (int off : {0x2B, 0x7B}) std::memcpy(code2 + off, fastblt::cvtSite, 5);
    // acceptance rule for the bytes found in memory
    {
        unsigned char m[sizeof(fastblt::expected)];
        std::memcpy(m, fastblt::expected, sizeof(m));
        CHECK(fastblt::codeMatches(m), "file bytes must be accepted");
        std::memcpy(m + 0x2B, fastblt::cvtSite, 5); CHECK(fastblt::codeMatches(m), "one rewritten call must be accepted");
        std::memcpy(m + 0x7B, fastblt::cvtSite, 5); CHECK(fastblt::codeMatches(m), "two rewritten calls must be accepted");
        m[0x7C] ^= 1; CHECK(!fastblt::codeMatches(m), "a damaged call site must be refused");
        std::memcpy(m + 0x7B, fastblt::cvtSite, 5);
        for (unsigned i = 0; i < sizeof(m); ++i) {
            if ((i >= 0x2B && i < 0x30) || (i >= 0x7B && i < 0x80)) continue;
            m[i] ^= 0x10; CHECK(!fastblt::codeMatches(m), "a changed byte at %u must be refused", i); m[i] ^= 0x10;
        }
    }
    dk2vr_bltResume = code + fastblt::PatchLength;   // the stub resumes on the 'ret'
    void* stub = reinterpret_cast<void*>(&dk2vr_bltStub);

    static fastblt::IndexCache cache;
    long rowsChecked = 0, stretch = 0, mirrorRows = 0, keyRows = 0, ones = 0;
    for (int iter = 0; iter < 30000; ++iter) {
        const int width = 1 + int(rnd() % 300);
        float scale;
        switch (rnd() % 5) {case 0: case 1: scale = 1.0f; break; case 2: scale = 0.5f; break; case 3: scale = 2.0f; break; default: scale = 0.25f + float(rnd() % 3000) / 1000.f;}
        const int mirror = (rnd() % 4) == 0;
        const int ck = (rnd() % 2) == 0;
        int srcW = int((width - 1) * scale) + 1 + int(rnd() % 3);
        std::vector<uint32_t> srcv(size_t(srcW) + 32), a(size_t(width) + 32), b(size_t(width) + 32), c(size_t(width) + 32), d(size_t(width) + 32);
        for (auto& v : srcv) v = rnd();
        const uint32_t key = (rnd() % 3) ? srcv[rnd() % srcW] : rnd();
        for (int i = 0; i < srcW; ++i) if (rnd() % 5 == 0) srcv[i] = key;
        for (size_t i = 0; i < a.size(); ++i) a[i] = b[i] = c[i] = d[i] = rnd();
        std::vector<uint32_t> e(a);
        uint32_t* da = a.data() + 16; uint32_t* db = b.data() + 16; uint32_t* dc = c.data() + 16; uint32_t* dd = d.data() + 16;
        const uint32_t* s = srcv.data();
        Frame fa, fb;
        void *e1, *e2, *e3, *x1, *x2, *x3;
        std::vector<uint32_t> a2(a);   // same random destination for the FastStretch-rewritten variant
        // A: the original machine code
        setFrame(fa, s, width, srcW, mirror, scale, ck, key);
        run(code, fa, da, scale, e1, x1, x2);
        // A2: the original code with its two calls rewritten by FastStretch (cvttss2si instead of the helper call)
        Frame fa2; void *e4, *x4, *x5;
        setFrame(fa2, s, width, srcW, mirror, scale, ck, key);
        run(code2, fa2, a2.data() + 16, scale, e4, x4, x5);
        CHECK(std::memcmp(a.data(), a2.data(), a.size() * 4) == 0, "FastStretch-rewritten loop differs from the original (iter %d)", iter);
        // B: the replacement stub (as patched into the wrapper)
        setFrame(fb, s, width, srcW, mirror, scale, ck, key);
        void* fbEbp = fb.b + 0x300;
        run(stub, fb, db, scale, e2, x3, e3);
        CHECK(e2 == fbEbp && x3 == db && e3 == fbEbp, "stub changed ebx / esi / ebp (iter %d)", iter);
        // C: replacement row routine called directly; D: reference transcription
        fastblt::fastRow(dc, s, width, scale, mirror, srcW, ck, key);
        // E: through the index cache, twice in a row (the second call reuses the table)
        fastblt::fastRow(e.data() + 16, s, width, scale, mirror, srcW, ck, key, &cache);
        fastblt::fastRow(e.data() + 16, s, width, scale, mirror, srcW, ck, key, &cache);
        CHECK(std::memcmp(a.data(), e.data(), a.size() * 4) == 0, "cached fastRow differs from the original code (iter %d width %d scale %g mirror %d ck %d)", iter, width, scale, mirror, ck);
        fastblt::referenceRow(dd, s, width, scale, mirror, srcW, ck, key);
        CHECK(std::memcmp(a.data(), b.data(), a.size() * 4) == 0, "stub differs from the original code (iter %d width %d scale %g mirror %d ck %d)", iter, width, scale, mirror, ck);
        CHECK(std::memcmp(a.data(), c.data(), a.size() * 4) == 0, "fastRow differs from the original code (iter %d width %d scale %g mirror %d ck %d)", iter, width, scale, mirror, ck);
        CHECK(std::memcmp(a.data(), d.data(), a.size() * 4) == 0, "referenceRow differs from the original code (iter %d)", iter);
        ++rowsChecked; if (scale != 1.0f) ++stretch; else ++ones; if (mirror) ++mirrorRows; if (ck) ++keyRows;
    }
    // overlapping source / destination (same surface): forward element-wise semantics must be kept
    for (int iter = 0; iter < 2000; ++iter) {
        const int width = 2 + int(rnd() % 100), shift = 1 + int(rnd() % 3);
        std::vector<uint32_t> a(size_t(width) + 16), b(a);
        for (size_t i = 0; i < a.size(); ++i) a[i] = b[i] = rnd();
        fastblt::fastRow(a.data() + 1 + shift, a.data() + 1, width, 1.0f, 0, width, 0, 0);
        fastblt::referenceRow(b.data() + 1 + shift, b.data() + 1, width, 1.0f, 0, width, 0, 0);
        CHECK(a == b, "overlapping copy differs (iter %d)", iter);
    }
    // many rows with one geometry but different pixels (what a real stretched blit does): the table must not depend on the pixel content
    for (int iter = 0; iter < 400; ++iter) {
        const int width = 1 + int(rnd() % 400);
        const float scale = (rnd() % 3) ? 0.2f + float(rnd() % 4000) / 1000.f : 1.0f;
        const int mirror = (rnd() % 5) == 0, ck = (rnd() % 3) == 0;
        const int srcW = int((width - 1) * scale) + 1 + int(rnd() % 3);
        for (int row = 0; row < 12; ++row) {
            std::vector<uint32_t> srcv(size_t(srcW) + 8), a(size_t(width) + 8), b;
            for (auto& v : srcv) v = rnd();
            for (auto& v : a) v = rnd();
            b = a;
            const uint32_t key = srcv[rnd() % srcW];
            for (int i = 0; i < srcW; ++i) if (rnd() % 6 == 0) srcv[i] = key;
            fastblt::fastRow(a.data(), srcv.data(), width, scale, mirror, srcW, ck, key, &cache);
            fastblt::referenceRow(b.data(), srcv.data(), width, scale, mirror, srcW, ck, key);
            CHECK(a == b, "cached rows with one geometry differ (iter %d row %d)", iter, row);
        }
    }
    // same width / source width / mirror, only the scale differs: the table must be rebuilt
    for (int iter = 0; iter < 300; ++iter) {
        const int width = 8 + int(rnd() % 200), srcW = 700;
        for (int k = 0; k < 6; ++k) {
            const float scale = 0.2f + float(rnd() % 2000) / 1000.f;
            const int mirror = (iter & 1);
            std::vector<uint32_t> srcv(size_t(srcW) + 8), a(size_t(width) + 8), b;
            for (auto& v : srcv) v = rnd();
            for (auto& v : a) v = rnd();
            b = a;
            fastblt::fastRow(a.data(), srcv.data(), width, scale, mirror, srcW, 0, 0, &cache);
            fastblt::referenceRow(b.data(), srcv.data(), width, scale, mirror, srcW, 0, 0);
            CHECK(a == b, "scale change not noticed by the index cache (iter %d)", iter);
        }
    }
    // vertical upscale pattern: the same source row several times in a row, sometimes changed by ONE pixel (anywhere in the range read), geometry sometimes changed
    for (int iter = 0; iter < 300; ++iter) {
        int width = 1 + int(rnd() % 500);
        float scale = (rnd() % 4) ? 0.2f + float(rnd() % 3000) / 1000.f : 1.0f;
        int mirror = (rnd() % 6) == 0;
        int srcW = int((width - 1) * scale) + 1 + int(rnd() % 3);
        std::vector<uint32_t> srcv(size_t(srcW) + 8);
        for (auto& v : srcv) v = rnd();
        for (int row = 0; row < 40; ++row) {
            const unsigned r = rnd() % 10;
            if (r == 0) {srcv[rnd() % srcW] = rnd();}                                  // one pixel changes
            else if (r == 1) {for (auto& v : srcv) v = rnd();}                          // whole new source row
            else if (r == 2) {                                                          // geometry changes
                width = 1 + int(rnd() % 500); scale = 0.2f + float(rnd() % 3000) / 1000.f; mirror = (rnd() % 6) == 0;
                srcW = int((width - 1) * scale) + 1 + int(rnd() % 3); srcv.assign(size_t(srcW) + 8, 0); for (auto& v : srcv) v = rnd();
            }
            std::vector<uint32_t> a(size_t(width) + 8), b;
            for (auto& v : a) v = rnd();
            b = a;
            fastblt::fastRow(a.data(), srcv.data(), width, scale, mirror, srcW, 0, 0, &cache);
            fastblt::referenceRow(b.data(), srcv.data(), width, scale, mirror, srcW, 0, 0);
            CHECK(a == b, "repeated-row cache differs (iter %d row %d width %d scale %g mirror %d)", iter, row, width, scale, mirror);
        }
    }
    CHECK(cache.dupRows > 500, "repeated rows never copied (%llu)", cache.dupRows);
    CHECK(cache.hits > 1000 && cache.builds > 100, "index cache never reused (%llu hits, %llu builds)", cache.hits, cache.builds);
    if (fails) return 1;
    std::printf("PASS: FastBltMachineTests (%ld random rows identical to the wrapper's original machine code: %ld at scale 1, %ld stretched, %ld mirrored, %ld with colour key; stub keeps ebx / esi / ebp)\n",
        rowsChecked, ones, stretch, mirrorRows, keyRows);
    return 0;
}
