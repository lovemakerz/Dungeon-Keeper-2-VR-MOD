// Minimal stand-in used ONLY to run the pure geometry tests on a non-Windows build machine.
#pragma once
typedef int LONG; // Windows LONG is always 32-bit
typedef void* HANDLE;
// 0.8.62: just enough for PatchSet.hpp (tests/PatchSetTests.cpp); the other tests do not use these.
typedef unsigned int DWORD;
typedef int BOOL;
#define PAGE_READWRITE 4
inline BOOL VirtualProtect(void*,unsigned long,DWORD,DWORD* old) {if(old) *old=0;return 1;}
inline LONG InterlockedIncrement(volatile LONG* p) {return __sync_add_and_fetch(p,1);}
inline void MemoryBarrier() {__sync_synchronize();}
