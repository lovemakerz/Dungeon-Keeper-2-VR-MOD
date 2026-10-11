#pragma once
// DK2VR 0.9.47.16 - made by LoVeMaKeRz
// Which Direct3D state reads a draw's snapshot really needs. A snapshot used to read 95 values from the device per draw (3 textures, 22 render states, 15 states x 3
// texture stages, 8 states x 3 samplers). Two groups cannot influence what the draw looks like:
//   - the states of texture stage 1 or 2 when that stage's COLOROP is DISABLE (Direct3D ignores a disabled stage and every stage after it);
//   - the sampler states of a sampler that has no texture bound.
// Their values are taken from the last time they were really read (so a snapshot stays fully valid for the replay), and everything is read again for the first 64
// snapshots and every 256th one. Pure logic, tested without Direct3D (tests/SnapshotPolicyTests.cpp).
#include <cstdint>
namespace snapshotpolicy {
template<class T, int Stages, int TS, int Samplers, int SS>
struct State {
    T lastTs[Stages][TS]{}, lastSs[Samplers][SS]{};
    unsigned captures = 0;
    unsigned long long skipped = 0, done = 0;
    bool fast = false;
    // readTs(stage, index) / readSs(sampler, index) read one value from the device; present[s] = a texture is bound to sampler s; index 0 of the stage states is COLOROP.
    template<class RTs, class RSs>
    void run(T (&ts)[Stages][TS], T (&ss)[Samplers][SS], const bool (&present)[Samplers], RTs readTs, RSs readSs, T disableOp = 1) {
        const bool full = !fast || captures < 64 || (captures & 255u) == 0;
        ++captures;
        for (int t = 0; t < Stages; ++t) {
            if (t == 0 || full) {
                for (int i = 0; i < TS; ++i) lastTs[t][i] = ts[t][i] = readTs(t, i);
                done += TS;
                continue;
            }
            lastTs[t][0] = ts[t][0] = readTs(t, 0);
            ++done;
            if (ts[t][0] == disableOp) {
                for (int i = 1; i < TS; ++i) ts[t][i] = lastTs[t][i];
                skipped += TS - 1;
            } else {
                for (int i = 1; i < TS; ++i) lastTs[t][i] = ts[t][i] = readTs(t, i);
                done += TS - 1;
            }
        }
        for (int t = 0; t < Samplers; ++t) {
            if (!full && !present[t]) {
                for (int i = 0; i < SS; ++i) ss[t][i] = lastSs[t][i];
                skipped += SS;
            } else {
                for (int i = 0; i < SS; ++i) lastSs[t][i] = ss[t][i] = readSs(t, i);
                done += SS;
            }
        }
    }
};
}  // namespace snapshotpolicy
