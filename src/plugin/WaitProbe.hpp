#pragma once
// DK2VR 0.8.54 DIAGNOSTIC ONLY - exact attribution of every blocking wait of the game render thread.
// Inline hooks (first 5 bytes, "mov eax,syscall#") on ntdll NtWaitForSingleObject / NtWaitForMultipleObjects /
// NtDelayExecution / NtSignalAndWaitForSingleObject / NtWaitForAlertByThreadId. The original call is made unchanged
// (same arguments, same result); the hook only measures its duration and remembers who called it
// (plausible return addresses found on the stack). Calls from other threads are passed straight through.
// Nothing is skipped, shortened or reordered. Refuses to install when the stub bytes are not exactly as expected.
#include <windows.h>
#include <psapi.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <atomic>
#include <vector>
#include <string>
#include <map>
#include <mmsystem.h>
#include <algorithm>

struct WaitSceneAcc {   // 0.9.47.7 (outside the class: default member initializers)
    double frames=0,userMs=0,kernelMs=0,waitMs[5]{};long cpuN=0;
    std::map<std::string,double> sites,mods,hot,chains,sysf;
};
struct WaitProbe {
    enum Kind {Single,Multiple,Delay,SignalWait,AlertId,KindCount};
    static constexpr const char* names[KindCount]={"NtWaitForSingleObject","NtWaitForMultipleObjects","NtDelayExecution","NtSignalAndWait","NtWaitForAlertByThreadId"};
    struct Entry {uint32_t key;uint32_t frames[8];uint32_t count;uint64_t ticks,maxTicks;uint8_t kind;};
    static constexpr int Slots=1024;
    static inline Entry table[Slots]={};
    static inline uint64_t kindTicks[KindCount]{},kindCalls[KindCount]{},shortTicks[KindCount]{},shortCalls[KindCount]{};
    static inline uint64_t overflow=0;
    static inline std::atomic<DWORD> renderThread{0};
    static inline bool installed=false;
    static inline volatile LONG inWait=0;
    static inline uint64_t frequency=0,lastReport=0;
    struct Range {uintptr_t begin,end;bool system;};
    static inline std::vector<Range> ranges;
    static inline uintptr_t stackBase=0;
    using F1=LONG(NTAPI*)(HANDLE,BOOLEAN,PLARGE_INTEGER);
    using F2=LONG(NTAPI*)(ULONG,HANDLE*,ULONG,BOOLEAN,PLARGE_INTEGER);
    using F3=LONG(NTAPI*)(BOOLEAN,PLARGE_INTEGER);
    using F4=LONG(NTAPI*)(HANDLE,HANDLE,BOOLEAN,PLARGE_INTEGER);
    using F5=LONG(NTAPI*)(PVOID,PLARGE_INTEGER);
    static inline F1 oSingle=nullptr;static inline F2 oMultiple=nullptr;static inline F3 oDelay=nullptr;static inline F4 oSignal=nullptr;static inline F5 oAlert=nullptr;

    static uint64_t now(){LARGE_INTEGER t{};QueryPerformanceCounter(&t);return uint64_t(t.QuadPart);}
    static void refreshRanges() {
        ranges.clear();HMODULE mods[512];DWORD need=0;
        if(!EnumProcessModules(GetCurrentProcess(),mods,sizeof(mods),&need)) return;
        for(DWORD i=0;i<std::min<DWORD>(need/sizeof(HMODULE),512);++i) {
            MODULEINFO mi{};if(!GetModuleInformation(GetCurrentProcess(),mods[i],&mi,sizeof(mi))) continue;
            char n[64]{};GetModuleBaseNameA(GetCurrentProcess(),mods[i],n,64);
            std::string s=n;for(auto& c:s) c=char(tolower(c));
            bool sys=s=="ntdll.dll"||s=="kernelbase.dll"||s=="kernel32.dll"||s=="ucrtbase.dll"||s=="msvcrt.dll"||s=="win32u.dll"||s=="user32.dll";
            ranges.push_back({reinterpret_cast<uintptr_t>(mi.lpBaseOfDll),reinterpret_cast<uintptr_t>(mi.lpBaseOfDll)+mi.SizeOfImage,sys});
        }
    }
    static const Range* find(uintptr_t a) {for(auto& r:ranges) if(a>=r.begin&&a<r.end) return &r;return nullptr;}
    static bool afterCall(uintptr_t ra) {
        const unsigned char* p=reinterpret_cast<const unsigned char*>(ra);
        if(p[-5]==0xE8) return true;                       // call rel32
        if(p[-6]==0xFF&&(p[-5]==0x15||(p[-5]&0xF8)==0x90||(p[-5]&0xF8)==0x10)) return true; // call [abs32] / [reg+disp32]
        if(p[-3]==0xFF&&((p[-2]&0xF8)==0x50||(p[-2]&0xF8)==0x10)) return true;   // call [reg+disp8]
        if(p[-2]==0xFF&&((p[-1]&0xF8)==0xD0||(p[-1]&0xF8)==0x10)) return true;   // call reg / [reg]
        return false;
    }
    static void record(int kind,uint64_t t0,uint64_t t1,uintptr_t sp) {
        const uint64_t d=t1-t0;
        kindTicks[kind]+=d;++kindCalls[kind];
        if(d*1000000ull<frequency*20ull) {shortTicks[kind]+=d;++shortCalls[kind];return;} // < 20 us: counted, no stack
        uint32_t fr[8]{};int n=0;uint32_t h=2166136261u;
        const uintptr_t top=std::min<uintptr_t>(stackBase?stackBase:sp+4096,sp+4096);
        for(uintptr_t a=sp;a+4<=top&&n<8;a+=4) {
            uintptr_t v=*reinterpret_cast<const uintptr_t*>(a);
            const Range* r=find(v);if(!r||r->system) continue;
            if(v<r->begin+8||!afterCall(v)) continue;
            fr[n++]=uint32_t(v);h=(h^uint32_t(v))*16777619u;
        }
        h=(h^uint32_t(kind))*16777619u;if(!h) h=1;
        for(int i=0;i<Slots;++i) {
            Entry& e=table[(h+i)%Slots];
            if(e.key==0) {e.key=h;memcpy(e.frames,fr,sizeof(fr));e.kind=uint8_t(kind);}
            if(e.key==h) {++e.count;e.ticks+=d;e.maxTicks=std::max(e.maxTicks,d);return;}
        }
        ++overflow;
    }
    static bool mine() {return GetCurrentThreadId()==renderThread.load(std::memory_order_relaxed);}
    static LONG NTAPI hSingle(HANDLE h,BOOLEAN a,PLARGE_INTEGER t) {
        if(!mine()) return oSingle(h,a,t);
        uintptr_t sp=reinterpret_cast<uintptr_t>(__builtin_frame_address(0));uint64_t t0=now();inWait=1;LONG r=oSingle(h,a,t);inWait=0;record(Single,t0,now(),sp);return r;}
    static LONG NTAPI hMultiple(ULONG c,HANDLE* h,ULONG w,BOOLEAN a,PLARGE_INTEGER t) {
        if(!mine()) return oMultiple(c,h,w,a,t);
        uintptr_t sp=reinterpret_cast<uintptr_t>(__builtin_frame_address(0));uint64_t t0=now();inWait=1;LONG r=oMultiple(c,h,w,a,t);inWait=0;record(Multiple,t0,now(),sp);return r;}
    static LONG NTAPI hDelay(BOOLEAN a,PLARGE_INTEGER t) {
        if(!mine()) return oDelay(a,t);
        uintptr_t sp=reinterpret_cast<uintptr_t>(__builtin_frame_address(0));uint64_t t0=now();inWait=1;LONG r=oDelay(a,t);inWait=0;record(Delay,t0,now(),sp);return r;}
    static LONG NTAPI hSignal(HANDLE s,HANDLE w,BOOLEAN a,PLARGE_INTEGER t) {
        if(!mine()) return oSignal(s,w,a,t);
        uintptr_t sp=reinterpret_cast<uintptr_t>(__builtin_frame_address(0));uint64_t t0=now();inWait=1;LONG r=oSignal(s,w,a,t);inWait=0;record(SignalWait,t0,now(),sp);return r;}
    static LONG NTAPI hAlert(PVOID p,PLARGE_INTEGER t) {
        if(!mine()) return oAlert(p,t);
        uintptr_t sp=reinterpret_cast<uintptr_t>(__builtin_frame_address(0));uint64_t t0=now();inWait=1;LONG r=oAlert(p,t);inWait=0;record(AlertId,t0,now(),sp);return r;}


    // ---- CPU sampler restricted to NON-waiting time (the older Profiler counted blocked time as CPU) ----
    struct Sample {uint32_t eip;uint32_t stack[96];};
    static constexpr int SampleCap=8000;
    static inline Sample* buffers[2]={nullptr,nullptr};
    static inline volatile LONG activeBuffer=0,counts[2]={0,0};
    static inline volatile LONG waitSamples=0,totalSamples=0,running=0;
    static inline HANDLE renderHandle=nullptr,samplerThread=nullptr;
    static inline volatile LONG samplerPaused=0; // 0.9.47.7: set by the game thread for alternate windows (probe cost = difference of game i/s with / without sampling)
    static DWORD WINAPI samplerLoop(void*) {
        timeBeginPeriod(1);
        // 0.9.47.7: high-resolution waitable timer (1 ms) when Windows offers it; plain Sleep(1) otherwise.
        HANDLE timer=CreateWaitableTimerExW(nullptr,nullptr,0x00000002/*CREATE_WAITABLE_TIMER_HIGH_RESOLUTION*/,TIMER_ALL_ACCESS);
        while(running) {
            if(timer) {LARGE_INTEGER due{};due.QuadPart=-10000LL;SetWaitableTimer(timer,&due,0,nullptr,nullptr,FALSE);WaitForSingleObject(timer,20);} else Sleep(1);
            if(samplerPaused) continue;
            InterlockedIncrement(&totalSamples);
            if(inWait) {InterlockedIncrement(&waitSamples);continue;}
            LONG b=activeBuffer;LONG n=counts[b];if(n>=SampleCap) continue;
            if(SuspendThread(renderHandle)==DWORD(-1)) continue;
            CONTEXT c{};c.ContextFlags=CONTEXT_CONTROL;
            if(GetThreadContext(renderHandle,&c)&&!(userOnly&&[&]{const Range* q=find(c.Eip);return !q||q->system;}())) {
                Sample& sm=buffers[b][n];sm.eip=c.Eip;
                MEMORY_BASIC_INFORMATION m{};uintptr_t sp=c.Esp;memset(sm.stack,0,sizeof(sm.stack));
                if(VirtualQuery(reinterpret_cast<void*>(sp),&m,sizeof(m))&&m.State==MEM_COMMIT&&!(m.Protect&(PAGE_GUARD|PAGE_NOACCESS))) {
                    size_t got=std::min<size_t>(96,(reinterpret_cast<uintptr_t>(m.BaseAddress)+m.RegionSize-sp)/4);memcpy(sm.stack,reinterpret_cast<void*>(sp),got*4);
                }
                counts[b]=n+1;
            }
            ResumeThread(renderHandle);
        }
        return 0;
    }
    static inline bool userOnly=false; // 0.8.56.3: sampler-only mode (no ntdll hooks): keep only samples whose EIP is in game/wrapper/mod code
    static inline std::map<std::string,int> cumBuckets,cumChains;static inline long cumN=0;
    static inline bool cpuSampler=false; // 0.8.55: the CPU sampler suspends the render thread every ms; off unless CpuSampler=1
    static void startSampler(DWORD tid) {
        renderHandle=OpenThread(THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_QUERY_INFORMATION,FALSE,tid);if(!renderHandle) return;
        for(auto& b:buffers) b=static_cast<Sample*>(VirtualAlloc(nullptr,sizeof(Sample)*SampleCap,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
        if(!buffers[0]||!buffers[1]) return;
        running=1;samplerThread=CreateThread(nullptr,0,&samplerLoop,nullptr,0,nullptr);
    }
    // 0.9.47.7 MEASURE: everything the probes see is also accumulated per scene (0 menu, 1 dungeon, 2 possession, 3 other).
    using SceneAcc=WaitSceneAcc;
    static inline SceneAcc sc[4];static inline int scene=3;
    template<class Out> static void sceneDump(int s,Out out) {
        const SceneAcc& a=sc[s];const double f=std::max(1.,a.frames);
        out("  CPU du fil du jeu : %.2f ms/image en mode utilisateur + %.2f ms/image en noyau (GetThreadTimes, %.0f images)",a.userMs/f,a.kernelMs/f,a.frames);
        double tw=0;for(int k=0;k<KindCount;++k) tw+=a.waitMs[k];
        out("  Attentes bloquantes du fil du jeu : %.2f ms/image au total",tw/f);
        for(int k=0;k<KindCount;++k) if(a.waitMs[k]>0) out("    %s : %.2f ms/image",names[k],a.waitMs[k]/f);
        auto top=[&](const std::map<std::string,double>& m,const char* title,size_t limit,double denom,bool pct) {
            std::vector<std::pair<double,std::string>> v;for(auto& [k,c]:m) v.push_back({c,k});std::sort(v.rbegin(),v.rend());
            if(v.empty()) return;out("  %s",title);
            for(size_t i=0;i<v.size()&&i<limit;++i) {if(pct) out("    %5.1f %%  %s",100.*v[i].first/std::max(1.,denom),v[i].second.c_str());else out("    %6.2f ms/image  %s",v[i].first/f,v[i].second.c_str());}
        };
        top(a.sites,"Points d'attente (appelants, du plus long au plus court) :",12,0,false);
        if(a.cpuN>=20) {
            out("  Echantillonneur CPU (%ld echantillons hors attente) :",a.cpuN);
            top(a.mods,"Modules (part du temps de calcul) :",8,double(a.cpuN),true);
            top(a.hot,"Code chaud (module+decalage, blocs de 64 octets) :",30,double(a.cpuN),true);
            top(a.chains,"Chaines d'appels (EIP < appelants) :",20,double(a.cpuN),true);
            top(a.sysf,"Fonctions Windows en cours d'execution (fonction <- appelant) :",20,double(a.cpuN),true);
        }
    }
    // 0.9.47.9: name of the Windows function a sample sits in (nearest export at or before the address), e.g. win32u.dll!NtUserSetCursorPos
    static std::string exportAt(uintptr_t base,uint32_t rva) {
        auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return "?";
        auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(base+dos->e_lfanew);
        auto& dir=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        if(!dir.VirtualAddress) return "?";
        auto* ex=reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(base+dir.VirtualAddress);
        auto* names=reinterpret_cast<const DWORD*>(base+ex->AddressOfNames);
        auto* ords=reinterpret_cast<const WORD*>(base+ex->AddressOfNameOrdinals);
        auto* funcs=reinterpret_cast<const DWORD*>(base+ex->AddressOfFunctions);
        uint32_t best=0;const char* name="?";
        for(DWORD i=0;i<ex->NumberOfNames;++i) {const uint32_t f=funcs[ords[i]];if(f<=rva&&f>best) {best=f;name=reinterpret_cast<const char*>(base+names[i]);}}
        return name;
    }
    static inline uint64_t lastUser=0,lastKernel=0;
    template<class Log> static void reportCpu(double frames,Log log,bool accumulate=false) {
        if(!running) return;
        FILETIME c,e,k,u;uint64_t ku=0,uu=0;
        if(GetThreadTimes(renderHandle,&c,&e,&k,&u)) {ku=(uint64_t(k.dwHighDateTime)<<32|k.dwLowDateTime);uu=(uint64_t(u.dwHighDateTime)<<32|u.dwLowDateTime);}
        double userMs=(uu-lastUser)/10000.,kernelMs=(ku-lastKernel)/10000.;lastUser=uu;lastKernel=ku;
        {SceneAcc& a=sc[std::clamp(scene,0,3)];a.userMs+=userMs;a.kernelMs+=kernelMs;a.frames+=frames;}
        LONG old=activeBuffer;activeBuffer=1-old;Sleep(3);
        LONG n=counts[old];counts[old]=0;
        LONG tot=InterlockedExchange(&totalSamples,0),wt=InterlockedExchange(&waitSamples,0);
        log("CPU sampler: render thread user %.2f ms/frame + kernel %.2f ms/frame (GetThreadTimes); %ld samples, %ld in measured waits, %ld non-wait kept",userMs/std::max(1.,frames),kernelMs/std::max(1.,frames),(long)tot,(long)wt,(long)n);
        if(n<20) return;
        std::map<std::string,int> mods,buckets,chains;
        for(LONG i=0;i<n;++i) {
            const Sample& sm=buffers[old][i];
            const Range* r=find(sm.eip);std::string w=where(sm.eip);
            std::string mod=w.substr(0,w.find('+'));++mods[mod];
            char key[96];
            if(r) snprintf(key,sizeof(key),"%s",(mod+"+"+[&]{char t[16];snprintf(t,16,"%X",unsigned((sm.eip-r->begin)&~63u));return std::string(t);}()).c_str()); else snprintf(key,sizeof(key),"?");
            ++buckets[key];
            std::string ch;int cnt=0;
            if(r&&!r->system) {ch+=where(sm.eip);++cnt;}
            for(int j=0;j<96&&cnt<4;++j) {
                uintptr_t v=sm.stack[j];const Range* q=find(v);if(!q||q->system||v<q->begin+8) continue;
                if(!afterCall(v)) continue;
                if(cnt) ch+=" < ";ch+=where(v);++cnt;
            }
            ++chains[mod+" :: "+ch];
        }
        auto top=[&](std::map<std::string,int>& m,const char* title,size_t limit) {
            std::vector<std::pair<int,std::string>> v;for(auto& [k,c]:m) v.push_back({c,k});std::sort(v.rbegin(),v.rend());
            std::string line=title;for(size_t i=0;i<v.size()&&i<limit;++i) {char t[220];snprintf(t,sizeof(t)," | %s %.1f%%",v[i].second.c_str(),100.*v[i].first/n);line+=t;}
            log("%s",line.c_str());
        };
        if(userOnly&&accumulate) {
            for(LONG i=0;i<n;++i) {
                const Sample& sm=buffers[old][i];const Range* r=find(sm.eip);if(!r) continue;
                std::string w=where(sm.eip);std::string mod=w.substr(0,w.find('+'));char k[96];snprintf(k,sizeof(k),"%s+%X",mod.c_str(),unsigned((sm.eip-r->begin)&~15u));++cumBuckets[k];
                std::string ch=where(sm.eip&~15u);int cnt=1;
                for(int j=0;j<96&&cnt<5;++j) {uintptr_t v=sm.stack[j];const Range* q=find(v);if(!q||q->system||v<q->begin+8||!afterCall(v)) continue;ch+=" < "+where(v);++cnt;}
                ++cumChains[ch];++cumN;
            }
            auto dump=[&](std::map<std::string,int>& m,const char* title,size_t limit) {
                std::vector<std::pair<int,std::string>> v;for(auto& [k,cn]:m) v.push_back({cn,k});std::sort(v.rbegin(),v.rend());
                log("%s (%ld user samples so far)",title,cumN);
                for(size_t i=0;i<v.size()&&i<limit;++i) log("  %5.2f%%  %s",100.*v[i].first/std::max<long>(1,cumN),v[i].second.c_str());
            };
            dump(cumBuckets,"ENGINE PROFILE hot code (16-byte blocks, table windows only)",45);
            dump(cumChains,"ENGINE PROFILE call chains (EIP < callers)",30);
        }
        {   // 0.9.47.7: per-scene accumulation (same buckets as the 5 s lines, summed over the session)
            SceneAcc& a=sc[std::clamp(scene,0,3)];a.cpuN+=n;
            for(auto& [k,c]:mods) a.mods[k]+=c;
            for(auto& [k,c]:buckets) a.hot[k]+=c;
            for(auto& [k,c]:chains) if(a.chains.size()<400||a.chains.count(k)) a.chains[k]+=c;
            // Windows function of each sample taken inside a system DLL + the first non-system caller on the stack
            for(LONG i=0;i<n;++i) {
                const Sample& sm=buffers[old][i];const Range* r=find(sm.eip);if(!r||!r->system) continue;
                std::string fn=where(sm.eip);const std::string mod=fn.substr(0,fn.find('+'));
                fn=mod+"!"+exportAt(r->begin,uint32_t(sm.eip-r->begin));
                std::string caller="(aucun)";
                for(int j=0;j<96;++j) {uintptr_t v=sm.stack[j];const Range* q=find(v);if(!q||q->system||v<q->begin+8||!afterCall(v)) continue;caller=where(v);break;}
                const std::string key=fn+"  <-  "+caller;
                if(a.sysf.size()<300||a.sysf.count(key)) a.sysf[key]+=1;
            }
        }
        top(mods,"CPU sampler modules (non-wait samples):",10);
        top(buckets,"CPU sampler hot code (module+RVA/64B):",24);
        top(chains,"CPU sampler call chains (EIP module :: frames):",14);
    }

    // Returns the trampoline (copy of the 5-byte prologue + jump back) or nullptr (nothing patched).
    static void* hook(HMODULE ntdll,const char* name,void* handler,const char*& why) {
        auto* fn=reinterpret_cast<unsigned char*>(GetProcAddress(ntdll,name));
        if(!fn) {why="export absent";return nullptr;}
        if(fn[0]!=0xB8) {why="unexpected stub (first byte not mov eax)";return nullptr;}
        auto* t=static_cast<unsigned char*>(VirtualAlloc(nullptr,16,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));
        if(!t) {why="alloc failed";return nullptr;}
        memcpy(t,fn,5);t[5]=0xE9;int32_t back=int32_t(reinterpret_cast<uintptr_t>(fn+5)-reinterpret_cast<uintptr_t>(t+10));memcpy(t+6,&back,4);
        DWORD old=0;if(!VirtualProtect(fn,5,PAGE_EXECUTE_READWRITE,&old)) {why="protect failed";return nullptr;}
        unsigned char jump[5]={0xE9,0,0,0,0};int32_t rel=int32_t(reinterpret_cast<uintptr_t>(handler)-reinterpret_cast<uintptr_t>(fn+5));memcpy(jump+1,&rel,4);
        memcpy(fn,jump,5);DWORD ig=0;VirtualProtect(fn,5,old,&ig);FlushInstructionCache(GetCurrentProcess(),fn,5);FlushInstructionCache(GetCurrentProcess(),t,16);
        why="ok";return t;
    }
    template<class Log> static void install(DWORD threadId,Log log) {
        if(installed) return;
        installed=true;
        LARGE_INTEGER f{};QueryPerformanceFrequency(&f);frequency=uint64_t(f.QuadPart);
        refreshRanges();
        NT_TIB* tib=reinterpret_cast<NT_TIB*>(NtCurrentTeb());stackBase=reinterpret_cast<uintptr_t>(tib->StackBase);
        HMODULE nt=GetModuleHandleA("ntdll.dll");if(!nt) {log("Wait probe: ntdll not found");return;}
        const char* why="";
        oSingle=reinterpret_cast<F1>(hook(nt,"NtWaitForSingleObject",reinterpret_cast<void*>(&hSingle),why));log("Wait probe: NtWaitForSingleObject %s",why);
        oMultiple=reinterpret_cast<F2>(hook(nt,"NtWaitForMultipleObjects",reinterpret_cast<void*>(&hMultiple),why));log("Wait probe: NtWaitForMultipleObjects %s",why);
        oDelay=reinterpret_cast<F3>(hook(nt,"NtDelayExecution",reinterpret_cast<void*>(&hDelay),why));log("Wait probe: NtDelayExecution %s",why);
        oSignal=reinterpret_cast<F4>(hook(nt,"NtSignalAndWaitForSingleObject",reinterpret_cast<void*>(&hSignal),why));log("Wait probe: NtSignalAndWaitForSingleObject %s",why);
        oAlert=reinterpret_cast<F5>(hook(nt,"NtWaitForAlertByThreadId",reinterpret_cast<void*>(&hAlert),why));log("Wait probe: NtWaitForAlertByThreadId %s",why);
        renderThread.store(threadId);lastReport=GetTickCount64();if(cpuSampler) startSampler(threadId);
        log("Wait probe: active on render thread %lu (measure only; waits are executed unchanged)",(unsigned long)threadId);
    }
    template<class Log> static void installSamplerOnly(DWORD threadId,Log log) {
        if(installed||running) return;
        LARGE_INTEGER f{};QueryPerformanceFrequency(&f);frequency=uint64_t(f.QuadPart);
        refreshRanges();userOnly=true;renderThread.store(threadId);startSampler(threadId);
        log("Engine profile: sampler only (no hooks), user-mode samples of the render thread %lu",(unsigned long)threadId);
    }
    static std::string where(uintptr_t a) {
        for(auto& r:ranges) if(a>=r.begin&&a<r.end) {
            char name[64]{};HMODULE m=nullptr;
            GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(a),&m);
            if(m) GetModuleBaseNameA(GetCurrentProcess(),m,name,64);
            char out[96];snprintf(out,96,"%s+%X",name,unsigned(a-r.begin));return out;
        }
        return "?";
    }
    template<class Log> static void report(double frames,double windowMs,Log log) {
        if(!installed||!renderThread.load()) return;
        refreshRanges();
        double total=0;for(int k=0;k<KindCount;++k) total+=double(kindTicks[k]);
        const double tick=1000./double(frequency);
        log("Wait probe window: render thread blocked %.2f ms/frame of %.2f ms/frame (%.0f%%); per kind ms/frame (calls/frame, of which <20us ms/frame):",
            total*tick/std::max(1.,frames),windowMs/std::max(1.,frames),100.*total*tick/std::max(1.,windowMs));
        for(int k=0;k<KindCount;++k) if(kindCalls[k])
            log("  %s: %.2f ms/frame (%.1f calls/frame, short %.2f ms)",names[k],double(kindTicks[k])*tick/std::max(1.,frames),double(kindCalls[k])/std::max(1.,frames),double(shortTicks[k])*tick/std::max(1.,frames));
        std::vector<const Entry*> v;for(auto& e:table) if(e.key) v.push_back(&e);
        std::sort(v.begin(),v.end(),[](const Entry* a,const Entry* b){return a->ticks>b->ticks;});
        {   // 0.9.47.7: per-scene accumulation of the waits
            SceneAcc& a=sc[std::clamp(scene,0,3)];
            for(int k=0;k<KindCount;++k) a.waitMs[k]+=double(kindTicks[k])*tick;
            for(size_t i=0;i<v.size()&&i<14;++i) {const Entry& e=*v[i];std::string s=names[e.kind];s+=" : ";for(int j=0;j<8&&e.frames[j];++j) {if(j) s+=" < ";s+=where(e.frames[j]);}a.sites[s]+=double(e.ticks)*tick;}
        }
        for(size_t i=0;i<v.size()&&i<10;++i) {
            const Entry& e=*v[i];std::string s;for(int j=0;j<8&&e.frames[j];++j) {if(j) s+=" < ";s+=where(e.frames[j]);}
            log("  wait site #%zu: %.2f ms/frame, %.2f calls/frame, max %.2f ms, %s: %s",i+1,double(e.ticks)*tick/std::max(1.,frames),double(e.count)/std::max(1.,frames),double(e.maxTicks)*tick,names[e.kind],s.c_str());
        }
        if(overflow) log("  wait probe table overflow: %llu",(unsigned long long)overflow);
        memset(static_cast<void*>(table),0,sizeof(table));memset(kindTicks,0,sizeof(kindTicks));memset(kindCalls,0,sizeof(kindCalls));memset(shortTicks,0,sizeof(shortTicks));memset(shortCalls,0,sizeof(shortCalls));overflow=0;
    }
};
