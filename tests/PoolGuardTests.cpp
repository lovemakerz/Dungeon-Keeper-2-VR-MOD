// DK2VR 0.9.47.17 - made by LoVeMaKeRz
#include "../src/common/PoolGuard.hpp"
#include <cstdio>
#include <cstdint>
static uint32_t seed = 4242;
static uint32_t rnd() {seed = seed * 1664525u + 1013904223u; return seed >> 8;}
#define CHECK(c, ...) do {if(!(c)) {std::printf("FAIL: " __VA_ARGS__); std::printf("\n"); return 1;}} while(0)
// The guard as it was until 0.9.47.16 (fixed ceiling 24, release 12).
struct OldGuard {
    uint32_t ceiling = 24, release = 12; bool tripped = false;
    bool safe(uint32_t n) {
        if(tripped) {if(n <= release) tripped = false; else return false;}
        else if(n >= ceiling) {tripped = true; return false;}
        return true;
    }
};
int main() {
    // 1. Initial behaviour is the old one: same trip level (24), same release level (12).
    {
        poolguard::Guard g;
        CHECK(g.tripLevel() == 24 && g.releaseLevel() == 12, "initial trip/release levels %u/%u", g.tripLevel(), g.releaseLevel());
        CHECK(g.offLevel() == 60, "initial cube 'off' level %u (was 60)", g.offLevel());
    }
    // 2. Random pools that keep moving (no plateau of 8 s) behave exactly like the old guard.
    long compared = 0;
    for(int run = 0; run < 2000; ++run) {
        poolguard::Guard g; OldGuard o; uint32_t n = 1; uint64_t t = 1000;
        for(int step = 0; step < 300; ++step) {
            t += 1 + rnd() % 200;                       // calls are at most 200 ms apart ...
            if(rnd() % 3 == 0) n += rnd() % 4;          // ... and the counter changes at least every few calls (never 8 s of plateau)
            if(n > 126) n = 126;
            if(rnd() % 40 == 0) n = 1 + rnd() % 14;     // (only a model: the real counter does not go down)
            // keep every plateau under 8 s by forcing a change when the counter has been still too long
            if(t - g.lastChangeMs > 5000 && g.started) n = (n < 126 ? n + 1 : n - 1);
            const bool a = g.safe(n, t), b = o.safe(n);
            CHECK(a == b, "run %d step %d: n=%u new=%d old=%d", run, step, n, int(a), int(b));
            ++compared;
        }
    }
    // 3. The real report: pool 1 -> 44 in one frame (zoom-out to the limit), then it stays there for ever.
    {
        poolguard::Guard g; uint64_t t = 100000;
        for(int i = 0; i < 50; ++i) {t += 16; CHECK(g.safe(1, t), "pool 1 must be safe");}
        t += 16; CHECK(!g.safe(44, t) && g.justTripped, "the jump to 44 must trip the guard (a jump is what the guard exists for)");
        bool allowedBefore = false;
        for(int i = 0; i < 400; ++i) {t += 16; if(g.safe(44, t)) allowedBefore = true;}
        // 400 calls x 16 ms = 6.4 s: still held (plateau shorter than 8 s)
        CHECK(!allowedBefore, "the guard must stay tripped during the first 8 s of the plateau");
        bool released = false; uint64_t tRelease = 0;
        for(int i = 0; i < 200; ++i) {t += 16; if(g.safe(44, t)) {released = true; tRelease = t; break;}}
        CHECK(released && g.justRebased && g.reference == 44, "a 44-block plateau of 8 s must become the new reference (reference %u)", g.reference);
        CHECK(g.tripLevel() == 67 && g.offLevel() == 100, "levels after the rebase: trip %u off %u", g.tripLevel(), g.offLevel());
        for(int i = 0; i < 1000; ++i) {t += 16; CHECK(g.safe(44, t), "after the rebase a pool at 44 must stay safe (possession 360 and view expansion run again)");}
        // growth of 23 more blocks above the plateau trips again
        t += 16; CHECK(g.safe(60, t), "60 is under the new trip level 67");
        t += 16; CHECK(!g.safe(67, t) && g.justTripped, "67 = 44 + 23 must trip");
        (void)tRelease;
    }
    // 4. Hard limits: a pool above rebaseMax is never accepted, however long it stays there.
    {
        poolguard::Guard g; uint64_t t = 5000; g.safe(1, t);
        t += 16; CHECK(!g.safe(90, t), "90 trips");
        for(int i = 0; i < 5000; ++i) {t += 16; CHECK(!g.safe(90, t), "a pool of 90 blocks must never be accepted (call %d)", i);}
        poolguard::Guard h; t = 5000; h.safe(1, t);
        t += 16; h.safe(80, t);
        bool ok = false; for(int i = 0; i < 1000 && !ok; ++i) {t += 16; ok = h.safe(80, t);}
        CHECK(ok && h.reference == 80, "a plateau of exactly rebaseMax (80) is accepted");
        CHECK(h.tripLevel() == 96, "trip level never above the hard cap (%u)", h.tripLevel());
        t += 16; CHECK(!h.safe(96, t), "96 must trip even after a rebase at 80");
        for(int i = 0; i < 5000; ++i) {t += 16; CHECK(!h.safe(96, t), "96 is never accepted");}
        CHECK(h.offLevel() == 100, "cube 'off' level never above its cap (%u)", h.offLevel());
    }
    // 5. A growing pool is never accepted while it grows (every change restarts the 8 s).
    {
        uint64_t t = 1000;
        // +1 block every 7 s keeps it tripped for ever
        poolguard::Guard h; t = 1000; h.safe(1, t); t += 16; h.safe(30, t);
        uint32_t m = 30; bool everSafe = false;
        for(int i = 0; i < 40; ++i) {t += 7000; if(m < 79) ++m; if(h.safe(m, t)) everSafe = true;}
        CHECK(!everSafe, "a pool that grows every 7 s must stay held");
    }
    std::printf("PASS: PoolGuardTests (%ld random calls identical to the 0.9.47.16 guard while the pool keeps moving; 1 -> 44 jump held 8 s then accepted as the reference; +23 above it trips again; 90+ never accepted; growth never accepted)\n", compared);
    return 0;
}
