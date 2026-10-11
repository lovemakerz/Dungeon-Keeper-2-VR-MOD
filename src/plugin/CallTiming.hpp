#pragma once
// DK2VR 0.8.62 adds Trace (the last device calls before a slow lock). DK2VR 0.8.60 - MEASUREMENT ONLY. Where the game thread spends its time inside Direct3D 9 and what each
// lock really costs. Nothing here changes what the game or the mod draws or writes.
//  * CallTiming : duration of the main Direct3D 9 device calls made by the game thread (vtable hooks that call
//                 the original unchanged), in 6 duration classes.
//  * LockDetail : every outermost texture/surface lock of the game thread, by kind, pool, size, flags, wrapper
//                 caller and engine caller; plus lock cost against the time since the GPU last used the resource.
#include <windows.h>
#include <psapi.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <atomic>
#include <algorithm>
#include <string>
#include <vector>

struct Clock {
    static long long now() {LARGE_INTEGER v;QueryPerformanceCounter(&v);return v.QuadPart;}
    static double freq() {static const double f=[]{LARGE_INTEGER q;QueryPerformanceFrequency(&q);return double(q.QuadPart);}();return f;}
    static double micros(long long ticks) {return double(ticks)*1e6/freq();}
    static int bucketOf(double us) {return us<2?0:us<10?1:us<50?2:us<200?3:us<1000?4:5;}
};

// Plausible callers of a hook: first return address inside the wrapper (ddraw.dll) and first call-preceded
// return address inside the game executable found on the stack above the hook.
struct CallerScan {
    static inline uintptr_t wrapperBase=0,wrapperSize=0,exeBase=0,exeSize=0;static inline bool ready=false;
    static void init() {
        if(ready) return;
        MODULEINFO mi{};
        if(HMODULE w=GetModuleHandleW(L"ddraw.dll")) if(GetModuleInformation(GetCurrentProcess(),w,&mi,sizeof(mi))) {wrapperBase=uintptr_t(mi.lpBaseOfDll);wrapperSize=mi.SizeOfImage;}
        if(HMODULE e=GetModuleHandleW(nullptr)) if(GetModuleInformation(GetCurrentProcess(),e,&mi,sizeof(mi))) {exeBase=uintptr_t(mi.lpBaseOfDll);exeSize=mi.SizeOfImage;}
        ready=true;
    }
    static bool inExe(uintptr_t a) {return a>=exeBase+16&&a<exeBase+exeSize;}
    // The engine's own lock/unlock helpers (RVA 0x190AC0..0x190B80: lockBuf, unlockBuf) are skipped: the caller that matters
    // is the one that called them (paintSurf at 0x19049B, ...).
    static bool inHelper(uintptr_t a) {return a>=exeBase+0x190AC0&&a<exeBase+0x190B80;}
    static bool callPreceded(uintptr_t a) {
        const unsigned char* p=reinterpret_cast<const unsigned char*>(a);
        if(IsBadReadPtr(p-6,6)) return false;
        return p[-5]==0xe8||(p[-6]==0xff&&(p[-5]==0x15||p[-5]==0x95))||(p[-2]==0xff&&(p[-1]&0x38)==0x10)||(p[-3]==0xff&&(p[-2]&0x38)==0x10)||(p[-4]==0xff&&(p[-3]&0x38)==0x10);
    }
    // 'frame' = __builtin_frame_address(0) of the hook, 'ret' = its return address.
    static void scan(void** frame,uintptr_t ret,uint32_t& wrapperRva,uint32_t& engineRva) {
        init();wrapperRva=engineRva=0;
        if(wrapperBase&&ret>=wrapperBase&&ret<wrapperBase+wrapperSize) wrapperRva=uint32_t(ret-wrapperBase);
        else if(inExe(ret)&&!inHelper(ret)&&callPreceded(ret)) {engineRva=uint32_t(ret-exeBase);return;}
        if(!frame||IsBadReadPtr(frame,4*160)) return;
        for(int i=1;i<160;++i) {
            const uintptr_t v=reinterpret_cast<uintptr_t>(frame[i]);
            if(!wrapperRva&&wrapperBase&&v>=wrapperBase&&v<wrapperBase+wrapperSize&&callPreceded(v)) wrapperRva=uint32_t(v-wrapperBase);
            if(inExe(v)&&!inHelper(v)&&callPreceded(v)) {engineRva=uint32_t(v-exeBase);break;}
        }
    }
};

struct CallTiming {
    enum Id {BeginScene,EndScene,Clear,SetTexture,DrawPrimitive,DrawIndexed,DrawPrimitiveUP,DrawIndexedUP,GetRenderTargetData,UpdateSurface,UpdateTexture,StretchRect,ColorFill,SetRenderTarget,Present,Count};
    static constexpr const char* names[Count]={"BeginScene","EndScene","Clear","SetTexture","DrawPrimitive","DrawIndexedPrimitive","DrawPrimitiveUP","DrawIndexedPrimitiveUP","GetRenderTargetData","UpdateSurface","UpdateTexture","StretchRect","ColorFill","SetRenderTarget","Present"};
    static inline std::atomic<DWORD> gameThread{0};
    static inline bool enabled=true;
    static inline std::atomic<bool> sampling{false}; // set by the game thread for 1 frame in 4: keeps the timing overhead ~0.1 ms/frame
    static inline volatile LONG anyCalls[Count]{},gameCalls[Count]{}; // 0.8.61: every call of a hook, in every frame (are the hooks reached at all?)
    static inline volatile LONG64 ticks[Count]{};
    static inline volatile LONG calls[Count]{};
    static inline volatile LONG bucket[Count][6]{};
    // Paint calls (engine texture uploads) count the texture/surface locks they cause on this thread.
    static inline thread_local int paintActive=0,paintLocks=0;
    // 0.9.26 PERF: which paintSurf caller is active (PaintProbe index; 5 = cache eviction) and what the evictions spend their time on (game thread only).
    static inline thread_local int paintKind=-1;
    struct Evict {
        static inline long long lockT[2][4]{},unlockT=0,callT[15]{};static inline long lockN[2][4]{},unlockN=0,callN[15]{};
        static bool active() {return paintActive&&paintKind==5;}
        static void clear() {for(auto& a:lockT) for(auto& v:a) v=0;for(auto& a:lockN) for(auto& v:a) v=0;unlockT=0;unlockN=0;for(auto& v:callT) v=0;for(auto& v:callN) v=0;}
    };
    struct EvictUnlock {long long t0;bool on;EvictUnlock():t0(0),on(Evict::active()) {if(on) t0=Clock::now();} ~EvictUnlock() {if(on) {Evict::unlockT+=Clock::now()-t0;++Evict::unlockN;}}};
    static void add(Id id,long long dt) {
        InterlockedExchangeAdd64(&ticks[id],dt);InterlockedIncrement(&calls[id]);InterlockedIncrement(&bucket[id][Clock::bucketOf(Clock::micros(dt))]);
    }
    // 0.8.62: ring of the last device calls of the game thread (all but draws and SetTexture, which are only counted). Written and read by the
    // game thread alone (the lock hooks read it right after a slow lock), so no synchronisation. 'a'/'b' are the resources of the call.
    struct TraceEntry {long long tick;uintptr_t ret;void* a;void* b;uint32_t draws;uint8_t id;};
    static constexpr int TraceSize=64;
    static inline TraceEntry ring[TraceSize]{};static inline unsigned ringHead=0;static inline uint32_t drawsSince=0;
    static inline std::atomic<DWORD> vrThread{0};   // the VR thread's id: its calls (second device, same vtable) are not the game's
    static inline int slowBudget=3;                 // slow-lock dumps left in this 5 s window
    static bool isDraw(Id i) {return i==SetTexture||i==DrawPrimitive||i==DrawIndexed||i==DrawPrimitiveUP||i==DrawIndexedUP;}
    static void trace(Id i,void* a,void* b,uintptr_t ret) {
        if(isDraw(i)) {++drawsSince;return;}
        TraceEntry& e=ring[ringHead++%TraceSize];e={Clock::now(),ret,a,b,drawsSince,uint8_t(i)};drawsSince=0;
    }
    static std::string moduleSpot(uintptr_t address) {
        if(!address) return "";
        HMODULE m=nullptr;char path[MAX_PATH]{};char out[160];
        if(GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(address),&m)&&m&&GetModuleFileNameA(m,path,MAX_PATH)) {
            const char* slash=strrchr(path,'\\');snprintf(out,sizeof(out),"%s+%X",slash?slash+1:path,unsigned(address-reinterpret_cast<uintptr_t>(m)));return out;
        }
        snprintf(out,sizeof(out),"%X",unsigned(address));return out;
    }
    // Newest first: what the game thread called before 'now', and which calls touched 'resource'.
    static std::string dumpTrace(long long now,void* resource,int maxEntries) {
        std::string s;char b[200];int shown=0;
        for(unsigned k=1;k<=unsigned(TraceSize)&&shown<maxEntries;++k) {
            if(ringHead<k) break;
            const TraceEntry& e=ring[(ringHead-k)%TraceSize];
            if(!e.tick||e.tick>now) continue;
            snprintf(b,sizeof(b)," | %s %.0f us ago%s (%u draws before) from %s",names[e.id],Clock::micros(now-e.tick),(resource&&(e.a==resource||e.b==resource))?" ON THIS SURFACE":"",unsigned(e.draws),moduleSpot(e.ret).c_str());
            s+=b;++shown;
        }
        return s;
    }
    struct Scope {
        Id id;long long t0;bool on;
        explicit Scope(Id i,void* a=nullptr,void* b=nullptr,uintptr_t ret=0):id(i),t0(0),on(false) {
#ifdef DK2VR_PUBLIC
            return;   // 0.9.47.60 public build: the hooks stay (main menu), nothing is counted, traced or timed
#endif
            if(!enabled) return;
            const bool game=GetCurrentThreadId()==gameThread.load(std::memory_order_relaxed);
            InterlockedIncrement(game?&gameCalls[i]:&anyCalls[i]);
            if(game) trace(i,a,b,ret);
            on=game&&sampling.load(std::memory_order_relaxed);evict=game&&Evict::active();if(on||evict) t0=Clock::now();
        }
        bool evict=false;
        ~Scope() {if(!on&&!evict) return;const long long dt=Clock::now()-t0;if(on) add(id,dt);if(evict&&id<15) {Evict::callT[id]+=dt;++Evict::callN[id];}}
    };
    template<class L> static void report(double frames,double elapsedMs,L logf) {
#ifdef DK2VR_PUBLIC
        return;
#endif
        if(!enabled) return;
        struct Row {int id;long long t;LONG n;LONG b[6];};
        std::vector<Row> rows;long long total=0;
        for(int i=0;i<Count;++i) {
            Row r{i,InterlockedExchange64(&ticks[i],0),InterlockedExchange(&calls[i],0),{}};
            for(int k=0;k<6;++k) r.b[k]=InterlockedExchange(&bucket[i][k],0);
            if(r.n) {rows.push_back(r);total+=r.t;}
        }
        std::sort(rows.begin(),rows.end(),[](const Row& a,const Row& b){return a.t>b.t;});
        std::string line;char b[320];
        snprintf(b,sizeof(b),"Direct3D calls of the game thread (last 5 s; n per frame x average us = ms per frame; classes <2/<10/<50/<200/<1000/>=1000 us); all timed calls %.2f ms of %.2f ms per frame:",
            Clock::micros(total)/1000./std::max(1.,frames),elapsedMs/std::max(1.,frames));line=b;
        for(size_t i=0;i<rows.size()&&i<12;++i) {
            const Row& r=rows[i];
            snprintf(b,sizeof(b)," | %s %.1f x %.1f = %.2f ms [%d/%d/%d/%d/%d/%d]",names[r.id],double(r.n)/std::max(1.,frames),Clock::micros(r.t)/std::max<LONG>(1,r.n),
                Clock::micros(r.t)/1000./std::max(1.,frames),int(r.b[0]),int(r.b[1]),int(r.b[2]),int(r.b[3]),int(r.b[4]),int(r.b[5]));
            line+=b;
        }
        if(rows.empty()) line+=" none";
        logf("%s",line.c_str());
        std::string hits="Direct3D hook calls (last 5 s, every frame; game thread / other threads):";
        for(int i=0;i<Count;++i) {const LONG g=InterlockedExchange(&gameCalls[i],0),o=InterlockedExchange(&anyCalls[i],0);if(g||o) {snprintf(b,sizeof(b)," | %s %ld / %ld",names[i],long(g),long(o));hits+=b;}}
        logf("%s",hits.c_str());
    }
};

// Time since the GPU last used a resource (copy source/destination, render target, fill): tells whether a slow lock
// is waiting for a copy still in flight.
struct GpuUse {
    static inline CRITICAL_SECTION cs;static inline bool ready=false;
    struct Slot {void* key=nullptr;long long tick=0;};
    static constexpr int Slots=4096;
    static inline Slot* table=nullptr;
    static void init() {if(ready) return;InitializeCriticalSection(&cs);table=new Slot[Slots]();ready=true;}
    static unsigned slotOf(void* p) {uintptr_t v=reinterpret_cast<uintptr_t>(p)>>4;return unsigned((v*2654435761u)>>20)&(Slots-1);}
    static void note(void* resource) {
        if(!ready||!resource) return;
        const long long t=Clock::now();EnterCriticalSection(&cs);
        unsigned s=slotOf(resource);
        for(int k=0;k<8;++k,s=(s+1)&(Slots-1)) if(!table[s].key||table[s].key==resource) {table[s]={resource,t};break;}
        else if(k==7) table[slotOf(resource)]={resource,t};
        LeaveCriticalSection(&cs);
    }
    static long long tickOf(void* resource) {
        if(!ready||!resource) return 0;
        EnterCriticalSection(&cs);
        unsigned s=slotOf(resource);long long t=0;
        for(int k=0;k<8;++k,s=(s+1)&(Slots-1)) {if(!table[s].key) break;if(table[s].key==resource) {t=table[s].tick;break;}}
        LeaveCriticalSection(&cs);
        return t;
    }
    // Time between the last GPU use and 'at': 0 never seen (or older than 1 s), 1 <50 us, 2 <500 us, 3 <5 ms, 4 <50 ms, 5 older.
    static int bucketAt(long long lastTick,long long at) {
        if(!lastTick||at<lastTick) return 0;
        const double us=Clock::micros(at-lastTick);
        return us>1e6?0:us<50?1:us<500?2:us<5000?3:us<50000?4:5;
    }
    static void forget(void* resource) {
        if(!ready||!resource) return;
        EnterCriticalSection(&cs);
        unsigned s=slotOf(resource);
        for(int k=0;k<8;++k,s=(s+1)&(Slots-1)) {if(!table[s].key) break;if(table[s].key==resource) {table[s].tick=0;break;}}
        LeaveCriticalSection(&cs);
    }
};

struct LockDetail {
    struct Key {
        uint8_t kind,pool,path,flags;uint16_t w,h,rw,rh;uint32_t wrapperRva,engineRva,object; // rw/rh 0 = whole resource; no padding (24 bytes)
        bool operator==(const Key& o) const {return !memcmp(this,&o,sizeof(Key));}
    };
    static_assert(sizeof(Key)==24,"LockDetail::Key must have no padding");
    struct Stat {Key key;long long ticks=0;LONG count=0;LONG bucket[6]{};bool used=false;};
    static constexpr int Slots=192;
    static inline Stat* table=nullptr;static inline CRITICAL_SECTION cs;static inline bool ready=false,enabled=true;
    static inline LONG dropped=0;
    static inline double lastMsPerFrame=0;   // 0.9.25 PERF: lock waits of the game thread in the last window (ms per frame)
    // Lock cost by time since the GPU used the resource: [group][bucket]; group 0 = surface in a system-memory pool,
    // 1 = texture served from the twin, 2 = texture/surface mirror served from memory, 3 = other native locks.
    static inline long long gapTicks[4][6]{};static inline LONG gapCount[4][6]{};
    static void init() {if(ready) return;InitializeCriticalSection(&cs);table=new Stat[Slots]();ready=true;}
    static uint8_t flagBits(DWORD f) {return uint8_t((f&0x10?1:0)|(f&0x2000?2:0)|(f&0x1000?4:0)|(f&0x4000?8:0)|(f&0x800?16:0)|(f&~DWORD(0x10|0x2000|0x1000|0x4000|0x800)?32:0));}
    static void record(const Key& k,long long dt,int gapGroup,int gapBucket) {
        if(!ready||!enabled) return;
        const int bk=Clock::bucketOf(Clock::micros(dt));
        EnterCriticalSection(&cs);
        if(gapGroup>=0&&gapGroup<4) {gapTicks[gapGroup][gapBucket]+=dt;++gapCount[gapGroup][gapBucket];}
        uint32_t h=2166136261u;const unsigned char* p=reinterpret_cast<const unsigned char*>(&k);
        for(size_t i=0;i<sizeof(Key);++i) h=(h^p[i])*16777619u;
        Stat* s=nullptr;
        for(int i=0;i<Slots;++i) {Stat& c=table[(h+i)%Slots];if(!c.used) {c.used=true;c.key=k;s=&c;break;}if(c.key==k) {s=&c;break;}}
        if(s) {s->ticks+=dt;++s->count;++s->bucket[bk];} else ++dropped;
        LeaveCriticalSection(&cs);
    }
    template<class L> static void report(double frames,L logf) {
        CallTiming::slowBudget=3; // game thread (the reporter) is also the one that dumps slow locks
        if(!ready||!enabled) return;
        std::vector<Stat> rows;long long gt[4][6];LONG gc[4][6];LONG lost;
        EnterCriticalSection(&cs);
        for(int i=0;i<Slots;++i) if(table[i].used&&table[i].count) rows.push_back(table[i]);
        memcpy(gt,gapTicks,sizeof(gt));memcpy(gc,gapCount,sizeof(gc));lost=dropped;
        for(int i=0;i<Slots;++i) table[i]=Stat();memset(gapTicks,0,sizeof(gapTicks));memset(gapCount,0,sizeof(gapCount));dropped=0;
        LeaveCriticalSection(&cs);
        std::sort(rows.begin(),rows.end(),[](const Stat& a,const Stat& b){return a.ticks>b.ticks;});
        static const char* kinds[2]={"texture","surface"};static const char* paths[4]={"native","twin","memory mirror","other"};
        long long total=0;for(auto& r:rows) total+=r.ticks;
        lastMsPerFrame=Clock::micros(total)/1000./std::max(1.,frames);   // 0.9.25 PERF
        logf("Lock detail (last 5 s, game thread): %.2f ms per frame in %zu kinds of lock%s; top by time (n per frame x average us; classes <2/<10/<50/<200/<1000/>=1000 us):",
            Clock::micros(total)/1000./std::max(1.,frames),rows.size(),lost?" (table full, some not counted)":"");
        for(size_t i=0;i<rows.size()&&i<9;++i) {
            const Stat& r=rows[i];const Key& k=r.key;
            char area[24];if(k.rw) snprintf(area,sizeof(area),"rect %ux%u",unsigned(k.rw),unsigned(k.rh));else snprintf(area,sizeof(area),"whole");
            logf("  %s %ux%u pool %u %s flags %s%s%s%s%s%s %s wrapper+%06X engine+%06X obj %08X: %.1f x %.1f us = %.2f ms [%d/%d/%d/%d/%d/%d]",
                kinds[k.kind&1],unsigned(k.w),unsigned(k.h),unsigned(k.pool),paths[k.path&3],k.flags&1?"R":"W",k.flags&2?"+discard":"",k.flags&4?"+nooverwrite":"",k.flags&8?"+donotwait":"",k.flags&16?"+nosyslock":"",k.flags&32?"+other":"",
                area,unsigned(k.wrapperRva),unsigned(k.engineRva),unsigned(k.object),double(r.count)/std::max(1.,frames),Clock::micros(r.ticks)/std::max<LONG>(1,r.count),
                Clock::micros(r.ticks)/1000./std::max(1.,frames),int(r.bucket[0]),int(r.bucket[1]),int(r.bucket[2]),int(r.bucket[3]),int(r.bucket[4]),int(r.bucket[5]));
        }
        static const char* groups[4]={"system-memory surfaces","texture twins","memory mirrors","other native locks"};
        static const char* gaps[6]={"no recent GPU use","<50us after GPU use","<500us","<5ms","<50ms",">=50ms"};
        std::string line="Lock cost against GPU use:";char b[200];
        for(int g=0;g<4;++g) {
            LONG n=0;for(int k=0;k<6;++k) n+=gc[g][k];
            if(!n) continue;
            snprintf(b,sizeof(b)," | %s:",groups[g]);line+=b;
            for(int k=0;k<6;++k) if(gc[g][k]) {snprintf(b,sizeof(b)," %s %d x %.1f us;",gaps[k],int(gc[g][k]),Clock::micros(gt[g][k])/gc[g][k]);line+=b;}
        }
        logf("%s",line.c_str());
    }
};
