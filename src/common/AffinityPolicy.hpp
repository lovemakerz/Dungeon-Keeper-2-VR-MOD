#pragma once
// DK2VR 0.9.47.12 - made by LoVeMaKeRz
// WorkerAffinity: the game process is started with a single allowed core (measured: affinity 0x1 of 0xFFFF on a 16-thread PC). The graphics driver's
// worker threads, the DK2VR threads and the Direct3D runtime then share that ONE core with the game thread: each hand-over to the driver worker is a
// full context switch on the same core. This pure-logic policy says which threads may use every core: only those whose start address lies in the
// graphics driver, the Direct3D / DXGI runtime or DK2VR itself. Everything else (the game's own threads, audio, network, unknown) keeps the
// single core, exactly as before. Pure logic (no Windows): unit-tested on any host.
#include <string>
#include <cstdint>

namespace affinity {

inline std::string baseNameLower(const std::string& path) {
    size_t p = path.find_last_of("\\/");
    std::string n = p == std::string::npos ? path : path.substr(p + 1);
    for (char& c : n) if (c >= 'A' && c <= 'Z') c = char(c - 'A' + 'a');
    return n;
}

// true = this module's threads may run on every core.
inline bool freeModule(const std::string& path) {
    const std::string n = baseNameLower(path);
    if (n.empty()) return false;
    static const char* const prefixes[] = {"nvd3dum", "nvwgf2um", "nvgpucomp", "nvapi", "nvoglv", "nvldumd", "nvspcap", "nvtopps",
                                           "dk2vr", "d3d9", "d3d11", "dxgi", "d3d10", "discordhook"};
    for (const char* p : prefixes) if (n.compare(0, std::char_traits<char>::length(p), p) == 0) return true;
    return false;
}

// The mask a thread must carry. game = the game's main thread (keeps its original mask).
inline uint64_t maskFor(bool isGameThread, const std::string& startModule, uint64_t originalMask, uint64_t systemMask) {
    if (isGameThread) return originalMask;
    return freeModule(startModule) ? systemMask : originalMask;
}

// The process is worth touching only if it is really restricted: a proper subset of the system mask, with more than one core available.
inline bool restricted(uint64_t processMask, uint64_t systemMask) {
    return processMask != 0 && processMask != systemMask && (systemMask & (systemMask - 1)) != 0 && (processMask & systemMask) == processMask;
}

}  // namespace affinity
