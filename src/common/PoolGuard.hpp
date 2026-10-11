#pragma once
// DK2VR 0.9.47.17 - made by LoVeMaKeRz
// PoolGuard: watches the game's render memory pool (number of 2 MB blocks the engine has allocated, address 0x765B08 of DKII-DX.exe 1.70).
// The engine NEVER gives a block back: the counter is a high-water mark of the biggest frame ever rendered. Until 0.9.47.16 the mod compared that
// counter with a fixed ceiling (24 blocks) and, above it, stopped everything it ADDS to the game's render list (view expansion, 360 cube map) for
// ever: one very wide table view (zoom-out to the limit: 1 -> 44 blocks in report 20261008_220935) switched the 360 degree possession off for the
// rest of the session, although the cube map itself needs next to nothing (every face frees its items before the next one).
// What really protects the engine (exit(-1) when the pool is full, limit 127 blocks) is GROWTH, so the ceiling now works from a REFERENCE LEVEL:
//   trip level   = reference + ceiling - 1, never above 'hardCap'          (with the initial reference 1 this is exactly the old ceiling of 24)
//   release      = reference + ceiling / 2 - 1                              (with the initial reference 1 this is the old release of 12)
//   when the guard is tripped, the pool stopped growing for 'stableMs' and sits at 'rebaseMax' blocks or less, that plateau becomes the new reference.
// Above 'rebaseMax' nothing is accepted any more (the old, permanent stand-down), so the pool can never be pushed past hardCap by this rule.
// Pure logic (no Windows): tests/PoolGuardTests.cpp.
#include <algorithm>
#include <cstdint>

namespace poolguard {

struct Guard {
    uint32_t ceiling = 24, hardCap = 96, rebaseMax = 80, offSpan = 59, offCap = 100;
    uint64_t stableMs = 8000;
    uint32_t reference = 1;
    bool tripped = false, started = false;
    uint32_t last = 0, peak = 0;
    uint64_t lastChangeMs = 0, trips = 0, rebases = 0;
    bool justTripped = false, justRebased = false;   // set by safe(), read once by the caller for its log

    uint32_t tripLevel() const {return std::min(hardCap, reference + ceiling - 1);}
    uint32_t releaseLevel() const {return reference + ceiling / 2 - 1;}
    // The level above which the 360 cube map stops for good (it used to be a fixed 60).
    uint32_t offLevel() const {return std::min(offCap, reference + offSpan);}

    // n = blocks in use now, nowMs = a monotonic clock. true = the mod may add to the render list.
    bool safe(uint32_t n, uint64_t nowMs) {
        justTripped = justRebased = false;
        if(n > peak) peak = n;
        if(!started) {started = true; last = n; lastChangeMs = nowMs;}
        if(n != last) {last = n; lastChangeMs = nowMs;}
        if(tripped) {
            if(n <= releaseLevel()) {tripped = false; return true;}
            if(n <= rebaseMax && nowMs - lastChangeMs >= stableMs) {
                reference = n; tripped = false; ++rebases; justRebased = true;
                return true;
            }
            return false;
        }
        if(n >= tripLevel()) {tripped = true; ++trips; justTripped = true; return false;}
        return true;
    }
};

}  // namespace poolguard
