#pragma once
// DK2VR 0.9.47.10 - made by LoVeMaKeRz
// NativeSkip: a dungeon draw that is HIDDEN in the game's own image (colour writes off, depth writes off) changes nothing on screen, but the
// game's device still receives it (a driver submission + GPU work + texture "in use" marks that make the next texture lock wait).
// The mod's draw hook runs just BEFORE the wrapper's real DrawPrimitiveUP / DrawIndexedPrimitiveUP; it arms this gate with the identity
// of that call, and the device-level hook of the same call drops it when (and only when) every argument matches.
// Pure logic (no Windows, no Direct3D) so that it is unit-tested on any host.
#include <cstdint>

namespace nativeskip {

struct Call {
    int kind = 0;                       // 0 = DrawPrimitiveUP, 1 = DrawIndexedPrimitiveUP
    uint32_t type = 0, primitives = 0, stride = 0;
    uint32_t minIndex = 0, numVertices = 0, indexFormat = 0;
    const void* verts = nullptr;
    const void* indices = nullptr;
    bool operator==(const Call& o) const {
        return kind == o.kind && type == o.type && primitives == o.primitives && stride == o.stride && verts == o.verts &&
               (kind == 0 || (minIndex == o.minIndex && numVertices == o.numVertices && indexFormat == o.indexFormat && indices == o.indices));
    }
};

struct Gate {
    Call armed{};
    bool isArmed = false;
    bool enabled = true;                // switched off for good after too many anomalies
    uint64_t armedTotal = 0, dropped = 0, stale = 0, mismatched = 0;
    static constexpr uint64_t AnomalyLimit = 64;

    // Called by the mod's draw hook for a hidden draw. A previous arming that was never consumed is an anomaly (the wrapper did not send the draw).
    void arm(const Call& c) {
        if(isArmed) ++stale;
        armed = c; isArmed = true; ++armedTotal;
        check();
    }
    // Called at the start of every draw hook and at the end of the frame: whatever is still armed was never sent.
    void clear() {
        if(isArmed) {++stale; isArmed = false; check();}
    }
    // Called by the device-level hook. true = drop this call.
    bool take(const Call& c) {
        if(!isArmed) return false;
        isArmed = false;
        if(armed == c) {++dropped; return true;}
        ++mismatched; check();
        return false;
    }
    bool active() const {return enabled;}
    // "Lean" mode (the colour / depth states of the hidden draw are not even written) only after VerifyDrops drops were CONFIRMED with no anomaly at all.
    // Before that (and for ever after the first anomaly) the draw is still hidden the old way (colour off, depth off), so that a wrapper that behaves
    // differently than expected can never show a piece of the dungeon in the game's image: the gate then merely drops the call, which is harmless.
    static constexpr uint64_t VerifyDrops = 300;
    bool lean() const {return enabled && dropped >= VerifyDrops && stale + mismatched == 0;}
    void check() {if(stale + mismatched > AnomalyLimit) enabled = false;}
};

}  // namespace nativeskip
