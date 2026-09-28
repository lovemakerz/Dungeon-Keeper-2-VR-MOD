#include <windows.h>
#include <tlhelp32.h>
#include <dbghelp.h>
#include <cstdio>
#include <cstdlib>
int main(int argc,char** argv) {
    if(argc!=2) return 2;
    DWORD pid=strtoul(argv[1],nullptr,10);
    HANDLE p=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid);
    if(!p) {printf("OpenProcess error %lu\n",GetLastError());return 1;}
    SymSetOptions(SYMOPT_UNDNAME|SYMOPT_DEFERRED_LOADS);
    SymInitialize(p,nullptr,TRUE);
    HANDLE mods=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,pid);
    MODULEENTRY32 me{sizeof(me)};
    if(Module32First(mods,&me)) do {printf("MODULE %08lX %08lX %ls\n",DWORD(reinterpret_cast<uintptr_t>(me.modBaseAddr)),me.modBaseSize,me.szModule);}while(Module32Next(mods,&me));
    CloseHandle(mods);
    HANDLE threads=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);THREADENTRY32 te{sizeof(te)};
    if(Thread32First(threads,&te)) do {
        if(te.th32OwnerProcessID!=pid) continue;
        HANDLE t=OpenThread(THREAD_GET_CONTEXT|THREAD_SUSPEND_RESUME|THREAD_QUERY_INFORMATION,FALSE,te.th32ThreadID);if(!t) continue;
        if(SuspendThread(t)==DWORD(-1)) {CloseHandle(t);continue;}
        CONTEXT ctx{};ctx.ContextFlags=CONTEXT_FULL;
        if(GetThreadContext(t,&ctx)) {
            printf("THREAD %lu EIP=%08lX ESP=%08lX EBP=%08lX\n",te.th32ThreadID,ctx.Eip,ctx.Esp,ctx.Ebp);
            STACKFRAME64 frame{};frame.AddrPC.Offset=ctx.Eip;frame.AddrPC.Mode=AddrModeFlat;frame.AddrStack.Offset=ctx.Esp;frame.AddrStack.Mode=AddrModeFlat;frame.AddrFrame.Offset=ctx.Ebp;frame.AddrFrame.Mode=AddrModeFlat;
            for(int i=0;i<18;++i) {
                if(!StackWalk64(IMAGE_FILE_MACHINE_I386,p,t,&frame,&ctx,nullptr,SymFunctionTableAccess64,SymGetModuleBase64,nullptr)) break;
                char buffer[sizeof(SYMBOL_INFO)+512]{};auto* sym=reinterpret_cast<SYMBOL_INFO*>(buffer);sym->SizeOfStruct=sizeof(SYMBOL_INFO);sym->MaxNameLen=511;DWORD64 delta=0;
                if(SymFromAddr(p,frame.AddrPC.Offset,&delta,sym)) printf("  %08llX %s + %llX\n",frame.AddrPC.Offset,sym->Name,delta);else printf("  %08llX\n",frame.AddrPC.Offset);
                if(!frame.AddrPC.Offset) break;
            }
        }
        ResumeThread(t);CloseHandle(t);
    }while(Thread32Next(threads,&te));
    CloseHandle(threads);SymCleanup(p);CloseHandle(p);return 0;
}
