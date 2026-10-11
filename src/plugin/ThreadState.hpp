#pragma once
// DK2VR 0.9.47.10 DIAGNOSTIC ONLY - how the SYSTEM sees the game thread, without touching it.
// Report 150258: in the dungeon the game thread spends 21.4 ms of a 34.6 ms frame either running (CPU user + kernel 14.3 ms) or in the five
// measured waits (7.1 ms); about 13 ms per frame is in neither. This probe asks Windows itself, 25 times a second, in which state the thread
// is (Running / Ready = waiting for a free core / Waiting + the wait reason), how much CPU every other thread of the process uses (driver
// worker threads, the VR thread, ...) and how busy each logical core is. The game thread is never suspended or hooked by it.
// NtQuerySystemInformation is read from a small low-priority thread; every result is validated (our own process and our own threads must
// be found with the expected ids) and the probe switches itself off with a note in the log when the layout is not the expected one.
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include "WaitProbe.hpp"

struct TsThAcc {uint64_t n = 0, st[8]{}, wr[40]{}, cpu100 = 0, ctx = 0; uint32_t start = 0; std::string name;};
struct TsScene {   // outside the class: default member initializers
    uint64_t samples = 0, gameN = 0, gameSt[8]{}, gameWr[40]{}, gameCtx = 0, gameCpu100 = 0;
    std::map<DWORD, TsThAcc> threads;
    double coreBusy[64]{}; uint64_t coreN[64]{}, coreHot[64]{}; int cores = 0;
    uint64_t coreSamples = 0;
};
struct TsPrev {uint64_t cpu = 0; uint32_t ctx = 0; bool have = false;};

struct ThreadStateProbe {
    using QueryFn = LONG(NTAPI*)(ULONG, PVOID, ULONG, PULONG);
    using ThAcc = TsThAcc; using SceneAcc = TsScene; using Prev = TsPrev;

    static inline QueryFn query = nullptr;
    static inline CRITICAL_SECTION cs{}; static inline bool csReady = false;
    static inline volatile LONG running = 0, scene = 3;
    static inline DWORD gameTid = 0, pid = 0;
    static inline int stride = 0, infoClass = 0, cpus = 0;
    static inline bool failed = false, tried = false;   // 0.9.47.12: install() runs ONCE (0.9.47.10/11 retried every frame after a failure: 60 % of the game thread)
    static inline int offStart = 28;
    static inline std::string note;
    static inline std::vector<unsigned char> buf;
    static inline SceneAcc acc[4];
    static inline std::map<DWORD, Prev> prev;
    static inline std::map<DWORD, std::string> names;
    static inline uint64_t prevIdle[64]{}, prevTotal[64]{}; static inline bool haveCores = false;
    static inline uint64_t okSamples = 0, badSamples = 0; static inline double lastMs = 0, maxMs = 0, sumMs = 0;

    // x86 layouts as returned to a 32-bit process (WOW64 converts them).
    static constexpr int ProcHeader = 184, ThreadBasic = 60, ThreadExt = 88;
    static constexpr int OffPid = 68, OffThreads = 4;
    static uint32_t u32(const unsigned char* p, int o) {uint32_t v; std::memcpy(&v, p + o, 4); return v;}
    static uint64_t u64(const unsigned char* p, int o) {uint64_t v; std::memcpy(&v, p + o, 8); return v;}

    static const char* stateName(int s) {
        static const char* n[8] = {"initialisation", "pret (sans coeur libre)", "en cours d'execution", "suivant sur un coeur", "termine", "en attente", "transition", "pret (differe)"};
        return n[s & 7];
    }
    static const char* waitName(int r) {
        static const char* n[39] = {"Executive", "FreePage", "PageIn", "PoolAllocation", "DelayExecution", "Suspended", "UserRequest", "WrExecutive", "WrFreePage", "WrPageIn",
            "WrPoolAllocation", "WrDelayExecution", "WrSuspended", "WrUserRequest", "WrEventPair", "WrQueue", "WrLpcReceive", "WrLpcReply", "WrVirtualMemory", "WrPageOut",
            "WrRendezvous", "WrKeyedEvent", "WrTerminated", "WrProcessInSwap", "WrCpuRateControl", "WrCalloutStack", "WrKernel", "WrResource", "WrPushLock", "WrMutex",
            "WrQuantumEnd", "WrDispatchInt", "WrPreempted", "WrYieldExecution", "WrFastMutex", "WrGuardedMutex", "WrRundown", "WrAlertByThreadId", "WrDeferredPreempt"};
        return (r >= 0 && r < 39) ? n[r] : "?";
    }

    // Offset of our process entry in the list, or -1.
    static int locate(ULONG got) {
        size_t off = 0;
        for (int guard = 0; guard < 4096 && off + ProcHeader <= got; ++guard) {
            const unsigned char* e = buf.data() + off;
            if (u32(e, OffPid) == pid) return int(off);
            const uint32_t next = u32(e, 0);
            if (!next) break;
            off += next;
        }
        return -1;
    }
    // A thread record size is accepted only if EVERY record carries our process id and a thread id, and the game thread is among them.
    static bool strideFits(const unsigned char* e, ULONG got, size_t off, int s) {
        const uint32_t nthreads = u32(e, OffThreads);
        if (nthreads == 0 || nthreads > 4000) return false;
        if (off + ProcHeader + size_t(nthreads) * s > got) return false;
        bool sawGame = false;
        for (uint32_t i = 0; i < nthreads; ++i) {
            const unsigned char* t = e + ProcHeader + size_t(i) * s;
            if (u32(t, 32) != pid || u32(t, 36) == 0) return false;
            if (u32(t, 36) == gameTid) sawGame = true;
        }
        return sawGame;
    }
    // Returns the offset of our process entry or -1. The first call also finds the thread record size (0.9.47.12: x86 structures holding a LARGE_INTEGER are
    // padded to a multiple of 8, so 64 / 96 bytes are tried before 60 / 88).
    static int findProcess(ULONG got) {
        const int off = locate(got);
        if (off < 0) return -1;
        const unsigned char* e = buf.data() + off;
        if (stride > 0) return strideFits(e, got, size_t(off), stride) ? off : -1;
        static const int c57[] = {96, 88, 104, 112, 0}, c5[] = {64, 60, 0};
        for (const int* c = infoClass == 57 ? c57 : c5; *c; ++c)
            if (strideFits(e, got, size_t(off), *c)) {stride = *c; offStart = *c >= 96 ? 72 : *c >= 88 ? 68 : 28; return off;}
        return -1;
    }
    // Facts for the log when no layout matches (numbers only, nothing personal).
    template <class Log> static void diagnose(ULONG got, Log log) {
        char b[1000]; int n = 0;
        const int off = locate(got);
        n += std::snprintf(b + n, sizeof(b) - n, "returned %lu bytes, class %d, pid %lu, game thread %lu, ", (unsigned long)got, infoClass, (unsigned long)pid, (unsigned long)gameTid);
        if (off < 0) n += std::snprintf(b + n, sizeof(b) - n, "process entry NOT found with the process id at offset %d", OffPid);
        else {
            const unsigned char* e = buf.data() + off;
            n += std::snprintf(b + n, sizeof(b) - n, "entry at %d, next=%lu, threads=%lu, first bytes after the %d byte header:", off, (unsigned long)u32(e, 0), (unsigned long)u32(e, OffThreads), ProcHeader);
            for (int i = 0; i < 104 && size_t(off) + ProcHeader + i < got && n < int(sizeof(b)) - 4; ++i) n += std::snprintf(b + n, sizeof(b) - n, "%s%02X", (i % 4) ? "" : " ", e[ProcHeader + i]);
        }
        log("Thread state probe diagnostic: %s", b);
    }

    static bool fetch(ULONG& got) {
        for (int attempt = 0; attempt < 4; ++attempt) {
            if (buf.size() < (1u << 20)) buf.resize(1u << 20);
            got = 0;
            const LONG st = query(infoClass, buf.data(), ULONG(buf.size()), &got);
            if (st == LONG(0xC0000004)) {buf.resize(std::max<size_t>(buf.size() * 2, got + (256u << 10))); continue;}   // STATUS_INFO_LENGTH_MISMATCH
            return st >= 0;
        }
        return false;
    }

    static std::string describeThread(DWORD tid) {
        auto it = names.find(tid);
        if (it != names.end()) return it->second;
        std::string n;
        using GetDesc = HRESULT(WINAPI*)(HANDLE, PWSTR*);
        static GetDesc gd = reinterpret_cast<GetDesc>(GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetThreadDescription"));
        if (gd) {
            HANDLE h = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, tid);
            if (h) {
                PWSTR w = nullptr;
                if (SUCCEEDED(gd(h, &w)) && w) {char c[96]{}; WideCharToMultiByte(CP_ACP, 0, w, -1, c, 95, nullptr, nullptr); n = c; LocalFree(w);}
                CloseHandle(h);
            }
        }
        names[tid] = n;
        return n;
    }

    static void sampleOnce() {
        ULONG got = 0;
        if (!fetch(got)) {++badSamples; return;}
        const int po = findProcess(got);
        if (po < 0) {++badSamples; if (badSamples > 20 && okSamples == 0) {failed = true; note = "disposition des structures inattendue";} return;}
        const unsigned char* e = buf.data() + po;
        const uint32_t n = u32(e, OffThreads);
        EnterCriticalSection(&cs);
        const int sceneNow = std::clamp<int>(int(scene), 0, 3);
        SceneAcc& a = acc[sceneNow];
        ++a.samples; ++okSamples;
        bool sawGame = false;
        for (uint32_t i = 0; i < n; ++i) {
            const unsigned char* t = e + ProcHeader + size_t(i) * stride;
            const DWORD tid = u32(t, 36);
            const uint64_t cpu = u64(t, 0) + u64(t, 8);
            const uint32_t ctx = u32(t, 48), state = u32(t, 52), reason = u32(t, 56);
            const uint32_t start = u32(t, offStart);
            Prev& p = prev[tid];
            const uint64_t dCpu = p.have && cpu >= p.cpu ? cpu - p.cpu : 0;
            const uint32_t dCtx = p.have && ctx >= p.ctx ? ctx - p.ctx : 0;
            p.cpu = cpu; p.ctx = ctx; p.have = true;
            ThAcc& th = a.threads[tid];
            ++th.n; ++th.st[state & 7]; if (state == 5 && reason < 40) ++th.wr[reason];
            th.cpu100 += dCpu; th.ctx += dCtx; th.start = start;
            if (th.name.empty() && th.n == 1) th.name = describeThread(tid);
            if (tid == gameTid) {
                sawGame = true; ++a.gameN; ++a.gameSt[state & 7]; if (state == 5 && reason < 40) ++a.gameWr[reason];
                a.gameCtx += dCtx; a.gameCpu100 += dCpu;
            }
        }
        (void)sawGame;
        LeaveCriticalSection(&cs);
        // logical cores
        {
            EnterCriticalSection(&cs);
            std::vector<unsigned char> cb(64 * 128);
            ULONG g2 = 0;
            if (cpus > 0 && cpus <= 64 && query(8, cb.data(), ULONG(cb.size()), &g2) >= 0 && g2 >= ULONG(cpus) * 40 && g2 % ULONG(cpus) == 0) {
                const int s = int(g2 / ULONG(cpus));
                a.cores = cpus;
                for (int c = 0; c < cpus; ++c) {
                    const unsigned char* q = cb.data() + size_t(c) * s;
                    const uint64_t idle = u64(q, 0), total = u64(q, 8) + u64(q, 16);   // kernel time includes idle
                    if (haveCores) {
                        const uint64_t dt = total - prevTotal[c], di = idle - prevIdle[c];
                        if (dt > 0 && total >= prevTotal[c]) {
                            const double busy = 1. - std::min<double>(1., double(di) / double(dt));
                            a.coreBusy[c] += busy; ++a.coreN[c]; if (busy > .9) ++a.coreHot[c]; ++a.coreSamples;
                        }
                    }
                    prevIdle[c] = idle; prevTotal[c] = total;
                }
                haveCores = true;
            }
            LeaveCriticalSection(&cs);
        }
    }

    static DWORD WINAPI loop(void*) {
        while (running && !failed) {
            LARGE_INTEGER a, b, f; QueryPerformanceCounter(&a);
            sampleOnce();
            QueryPerformanceCounter(&b); QueryPerformanceFrequency(&f);
            lastMs = 1000. * double(b.QuadPart - a.QuadPart) / double(f.QuadPart); sumMs += lastMs; maxMs = std::max(maxMs, lastMs);
            Sleep(100);   // 0.9.47.12: 10 per second (was 25)
        }
        return 0;
    }

    template <class Log> static void install(DWORD threadId, Log log) {
        if (running || tried) return;
        tried = true;
        query = reinterpret_cast<QueryFn>(GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQuerySystemInformation"));
        if (!query) {log("Thread state probe: NtQuerySystemInformation not found"); return;}
        if (!csReady) {InitializeCriticalSection(&cs); csReady = true;}
        gameTid = threadId; pid = GetCurrentProcessId();
        SYSTEM_INFO si{}; GetSystemInfo(&si); cpus = int(si.dwNumberOfProcessors);
        // class 57 = SystemExtendedProcessInformation (thread start addresses); class 5 = SystemProcessInformation
        ULONG lastGot = 0;
        for (int c : {57, 5}) {
            infoClass = c; stride = 0; ULONG got = 0;
            if (fetch(got) && findProcess(got) >= 0) {
                running = 1;
                HANDLE t = CreateThread(nullptr, 0, &loop, nullptr, 0, nullptr);
                if (t) {SetThreadPriority(t, THREAD_PRIORITY_BELOW_NORMAL); CloseHandle(t);
                    log("Thread state probe (0.9.47.12): active, class %d, records of %d bytes, %d logical processors; the game thread is only observed", c, stride, cpus); return;}
                running = 0; log("Thread state probe: thread not created"); return;
            }
            lastGot = got;
            if (c == 57) diagnose(got, log);
        }
        diagnose(lastGot, log);
        failed = true; note = "structures non reconnues";
        log("Thread state probe: process list not in the expected layout, probe off");
    }

    static std::string systemLine(DWORD gameThread) {
        DWORD_PTR pm = 0, sm = 0; GetProcessAffinityMask(GetCurrentProcess(), &pm, &sm);
        const DWORD pc = GetPriorityClass(GetCurrentProcess());
        HANDLE h = OpenThread(THREAD_QUERY_INFORMATION, FALSE, gameThread);
        int tp = 99; BOOL boostOff = FALSE;
        if (h) {tp = GetThreadPriority(h); GetThreadPriorityBoost(h, &boostOff); CloseHandle(h);}
        BOOL procBoostOff = FALSE; GetProcessPriorityBoost(GetCurrentProcess(), &procBoostOff);
        char b[300];
        std::snprintf(b, sizeof(b), "Processus du jeu : %d coeurs logiques, affinite du processus 0x%llX sur 0x%llX | classe de priorite 0x%lX (0x20 = normale, 0x80 = haute) | priorite du fil du jeu %d | bonus de priorite %s",
            cpus, (unsigned long long)pm, (unsigned long long)sm, (unsigned long)pc, tp, (boostOff || procBoostOff) ? "desactive" : "actif");
        return b;
    }

    template <class Out> static void dump(int s, double frames, Out out) {
        if (!csReady) return;
        EnterCriticalSection(&cs);
        struct Unlock {~Unlock() {LeaveCriticalSection(&cs);}} unlock;
        const SceneAcc& a = acc[s];
        if (a.samples < 20 || a.gameN < 20) return;
        const double gn = double(a.gameN), f = std::max(1., frames);
        out("  Fil du jeu vu du systeme (%llu observations, 10 par seconde, sans arret du fil) :", (unsigned long long)a.gameN);
        out("    en cours d'execution %.1f %% | PRET mais sans coeur libre (preempte) %.1f %% | en attente %.1f %% | autres %.1f %%",
            100. * (a.gameSt[2] + a.gameSt[3]) / gn, 100. * (a.gameSt[1] + a.gameSt[7]) / gn, 100. * a.gameSt[5] / gn, 100. * (a.gameSt[0] + a.gameSt[4] + a.gameSt[6]) / gn);
        std::vector<std::pair<uint64_t, int>> w;
        for (int r = 0; r < 40; ++r) if (a.gameWr[r]) w.push_back({a.gameWr[r], r});
        std::sort(w.rbegin(), w.rend());
        if (!w.empty()) {
            std::string l = "    causes d'attente :";
            for (size_t i = 0; i < w.size() && i < 6; ++i) {char t[80]; std::snprintf(t, sizeof(t), " %s %.1f %%;", waitName(w[i].second), 100. * w[i].first / gn); l += t;}
            out("%s", l.c_str());
        }
        out("    changements de contexte du fil du jeu : %.0f par image | CPU vu par le systeme : %.2f ms par image", double(a.gameCtx) / f, double(a.gameCpu100) / 10000. / f);
        // other threads of the process
        std::vector<std::pair<uint64_t, DWORD>> v;
        for (auto& [tid, th] : a.threads) v.push_back({th.cpu100, tid});
        std::sort(v.rbegin(), v.rend());
        out("    fils du processus (CPU ms par image du jeu | changements de contexte par image | etat le plus frequent | depart) :");
        int shown = 0;
        for (auto& [cpu, tid] : v) {
            if (shown >= 8) break;
            const ThAcc& th = a.threads.at(tid);
            if (tid != gameTid && cpu < 5000 * 100) continue;   // below 0.5 ms of CPU in the whole scene: not worth a line
            int best = 0; for (int i = 1; i < 8; ++i) if (th.st[i] > th.st[best]) best = i;
            char start[96] = "?";
            if (th.start) {std::string w2 = WaitProbeWhere(th.start); std::snprintf(start, sizeof(start), "%s", w2.c_str());}
            out("      %s%-6lu %6.2f ms | %6.1f | %s %.0f %% | %s %s", tid == gameTid ? "[JEU] " : "", (unsigned long)tid, double(cpu) / 10000. / f, double(th.ctx) / f, stateName(best), 100. * th.st[best] / std::max<double>(1., double(th.n)), start, th.name.c_str());
            ++shown;
        }
        if (a.coreSamples > 0 && a.cores > 0) {
            double sum = 0, mx = 0; int hot = 0, idle = 0, n = 0;
            for (int c = 0; c < a.cores; ++c) if (a.coreN[c]) {
                const double m = a.coreBusy[c] / double(a.coreN[c]); sum += m; mx = std::max(mx, m); ++n; if (m > .85) ++hot; if (m < .2) ++idle;
            }
            if (n) out("    coeurs logiques du PC : occupation moyenne %.0f %% | le plus charge %.0f %% | coeurs charges a plus de 85 %% : %d | coeurs presque libres (moins de 20 %%) : %d sur %d", 100. * sum / n, 100. * mx, hot, idle, n);
        }
    }
    // resolved by the caller (WaitProbe::where is defined in WaitProbe.hpp, which is included first)
    static std::string WaitProbeWhere(uint32_t a) {return WaitProbe::where(a);}
};
