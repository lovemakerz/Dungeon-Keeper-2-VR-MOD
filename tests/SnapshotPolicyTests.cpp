// DK2VR 0.9.47.16 - made by LoVeMaKeRz
#include "../src/common/SnapshotPolicy.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstdint>
static uint32_t seed = 12345;
static uint32_t rnd() {seed = seed * 1664525u + 1013904223u; return seed >> 8;}
int main() {
    using P = snapshotpolicy::State<uint32_t, 3, 15, 3, 8>;
    P p; p.fast = true;
    uint32_t devTs[3][15], devSs[3][8]; bool present[3];
    long mismatchUsed = 0, skippedSeen = 0, checks = 0, fullCalls = 0, callsFast = 0;
    const int N = 5000;
    for (int n = 0; n < N; ++n) {
        // random device state: stage 1 / 2 disabled about half of the time, samplers without texture about half of the time
        for (int t = 0; t < 3; ++t) {
            for (int i = 0; i < 15; ++i) devTs[t][i] = rnd();
            if (t > 0) devTs[t][0] = (rnd() & 1) ? 1u : 2u + rnd() % 20;
            else devTs[t][0] = 2u + rnd() % 20;
            for (int i = 0; i < 8; ++i) devSs[t][i] = rnd();
            present[t] = (rnd() & 1) != 0;
        }
        uint32_t ts[3][15] = {}, ss[3][8] = {};
        long calls = 0;
        p.run(ts, ss, present, [&](int t, int i) {++calls; return devTs[t][i];}, [&](int t, int i) {++calls; return devSs[t][i];});
        callsFast += calls; fullCalls += 15 * 3 + 8 * 3;
        for (int t = 0; t < 3; ++t) {
            const bool disabled = t > 0 && devTs[t][0] == 1u;
            for (int i = 0; i < 15; ++i) {
                ++checks;
                if (i == 0 || !disabled) {if (ts[t][i] != devTs[t][i]) ++mismatchUsed;}   // anything that matters must be the device's real value
                else ++skippedSeen;
            }
            for (int i = 0; i < 8; ++i) {
                ++checks;
                if (present[t]) {if (ss[t][i] != devSs[t][i]) ++mismatchUsed;}
                else ++skippedSeen;
            }
        }
    }
    if (mismatchUsed) {std::printf("FAIL: %ld values that matter differ from the device\n", mismatchUsed); return 1;}
    if (p.skipped == 0 || callsFast >= fullCalls * 9 / 10) {std::printf("FAIL: nothing saved (%ld calls instead of %ld)\n", callsFast, fullCalls); return 1;}
    // a stage / sampler skipped before it was ever read must still hold real values after the seeding period: first 64 snapshots read everything
    P q; q.fast = true;
    uint32_t ts[3][15] = {}, ss[3][8] = {};
    for (int t = 0; t < 3; ++t) {for (int i = 0; i < 15; ++i) devTs[t][i] = 100 * t + i + 7; for (int i = 0; i < 8; ++i) devSs[t][i] = 1000 * t + i + 3; present[t] = false;}
    devTs[1][0] = 1; devTs[2][0] = 1;
    q.run(ts, ss, present, [&](int t, int i) {return devTs[t][i];}, [&](int t, int i) {return devSs[t][i];});
    if (ts[2][5] != devTs[2][5] || ss[1][4] != devSs[1][4]) {std::printf("FAIL: the first snapshots must read everything\n"); return 1;}
    // after the seeding period a disabled stage returns the last REAL values, then a changed device is followed again
    for (int n = 0; n < 100; ++n) q.run(ts, ss, present, [&](int t, int i) {return devTs[t][i];}, [&](int t, int i) {return devSs[t][i];});
    uint32_t keep = devTs[2][9];
    devTs[2][9] = 4242;   // changes while the stage stays disabled: not read
    q.run(ts, ss, present, [&](int t, int i) {return devTs[t][i];}, [&](int t, int i) {return devSs[t][i];});
    if (ts[2][9] != keep) {std::printf("FAIL: a disabled stage must keep the last read values (%u vs %u)\n", ts[2][9], keep); return 1;}
    devTs[2][0] = 5;      // the stage is switched on: everything read again at once
    q.run(ts, ss, present, [&](int t, int i) {return devTs[t][i];}, [&](int t, int i) {return devSs[t][i];});
    if (ts[2][9] != 4242 || ts[2][0] != 5) {std::printf("FAIL: an enabled stage must be read in full\n"); return 1;}
    present[1] = true; devSs[1][2] = 77;
    q.run(ts, ss, present, [&](int t, int i) {return devTs[t][i];}, [&](int t, int i) {return devSs[t][i];});
    if (ss[1][2] != 77) {std::printf("FAIL: a sampler with a texture must be read\n"); return 1;}
    // fast = false: always everything
    P r; r.fast = false; long c = 0;
    r.run(ts, ss, present, [&](int t, int i) {++c; return devTs[t][i];}, [&](int t, int i) {++c; return devSs[t][i];});
    if (c != 15 * 3 + 8 * 3) {std::printf("FAIL: fast=false must read all %ld values (read %ld)\n", long(15 * 3 + 8 * 3), c); return 1;}
    std::printf("PASS: SnapshotPolicyTests (%d random device states: every value that can matter equals the device's, %.0f %% of the device reads avoided, %ld values left from the last real read)\n",
        N, 100. * double(fullCalls - callsFast) / double(fullCalls), skippedSeen);
    return 0;
}
