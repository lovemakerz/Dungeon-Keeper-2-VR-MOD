#include <windows.h>
#include <tlhelp32.h>
#include <dbghelp.h>
#include <cstdio>
#include <string>
#include <filesystem>
int wmain(int argc,wchar_t** argv) {
    if(argc<2)return 2;
    std::filesystem::path exe=argv[1];std::wstring command=L"\""+exe.wstring()+L"\"";
    if(argc>2)command+=L" "+std::wstring(argv[2]);
    STARTUPINFOW si{sizeof(si)};PROCESS_INFORMATION pi{};
    if(!CreateProcessW(exe.c_str(),command.data(),nullptr,nullptr,FALSE,DEBUG_ONLY_THIS_PROCESS,nullptr,exe.parent_path().c_str(),&si,&pi)) {printf("CreateProcess=%lu\n",GetLastError());return 1;}
    printf("PID=%lu\n",pi.dwProcessId);fflush(stdout);CloseHandle(pi.hThread);
    auto until=GetTickCount64()+15000;bool done=false,crashed=false;
    while(!done&&GetTickCount64()<until) {
        DEBUG_EVENT e{};if(!WaitForDebugEvent(&e,500))continue;DWORD status=DBG_CONTINUE;
        if(e.dwDebugEventCode==CREATE_PROCESS_DEBUG_EVENT) {if(e.u.CreateProcessInfo.hFile)CloseHandle(e.u.CreateProcessInfo.hFile);}
        if(e.dwDebugEventCode==LOAD_DLL_DEBUG_EVENT) {if(e.u.LoadDll.hFile)CloseHandle(e.u.LoadDll.hFile);}
        if(e.dwDebugEventCode==EXCEPTION_DEBUG_EVENT) {
            const auto& record=e.u.Exception.ExceptionRecord;DWORD code=record.ExceptionCode;
            if(code!=EXCEPTION_BREAKPOINT&&code!=0x406d1388) {
                printf("EXCEPTION=%08lX address=%p first=%lu info=%lu/%08lX\n",code,record.ExceptionAddress,e.u.Exception.dwFirstChance,record.ExceptionInformation[0],record.ExceptionInformation[1]);
                HANDLE thread=OpenThread(THREAD_GET_CONTEXT|THREAD_QUERY_INFORMATION,FALSE,e.dwThreadId);CONTEXT ctx{};ctx.ContextFlags=CONTEXT_FULL;
                if(GetThreadContext(thread,&ctx)) {
                    printf("EIP=%08lX ESP=%08lX EBP=%08lX EAX=%08lX EBX=%08lX ECX=%08lX EDX=%08lX ESI=%08lX EDI=%08lX\n",ctx.Eip,ctx.Esp,ctx.Ebp,ctx.Eax,ctx.Ebx,ctx.Ecx,ctx.Edx,ctx.Esi,ctx.Edi);
                    DWORD stack[40]{};SIZE_T count=0;ReadProcessMemory(pi.hProcess,reinterpret_cast<void*>(ctx.Esp),stack,sizeof(stack),&count);
                    for(int i=0;i<int(count/4);++i)printf("%08lX%c",stack[i],i%8==7?'\n':' ');printf("\n");
                    unsigned char bytes[48]{};ReadProcessMemory(pi.hProcess,reinterpret_cast<void*>(ctx.Eip),bytes,sizeof(bytes),&count);for(unsigned i=0;i<count;++i)printf("%02x",bytes[i]);printf("\n");
                    HANDLE mods=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,pi.dwProcessId);MODULEENTRY32 me{sizeof(me)};
                    if(Module32First(mods,&me))do {printf("MODULE %p %08lX %ls\n",me.modBaseAddr,me.modBaseSize,me.szModule);}while(Module32Next(mods,&me));CloseHandle(mods);
                }
                if(thread)CloseHandle(thread);status=DBG_EXCEPTION_NOT_HANDLED;
                if(!e.u.Exception.dwFirstChance) {crashed=true;done=true;}
                fflush(stdout);
            }
        }
        if(e.dwDebugEventCode==EXIT_PROCESS_DEBUG_EVENT) {printf("EXIT=%lu\n",e.u.ExitProcess.dwExitCode);done=true;}
        ContinueDebugEvent(e.dwProcessId,e.dwThreadId,status);
    }
    DebugActiveProcessStop(pi.dwProcessId);CloseHandle(pi.hProcess);return crashed?1:0;
}
