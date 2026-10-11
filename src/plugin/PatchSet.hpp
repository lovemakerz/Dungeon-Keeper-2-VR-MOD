#pragma once
// DK2VR 0.8.62 - vtable slot patches, moved out of Decoupled.inl so that the logic can be tested without Direct3D (tests/PatchSetTests.cpp).
// One PatchSet = one slot index; one entry per distinct vtable (the original is kept for the hook).
#include <windows.h>

struct Patch {void** vtable;void* original;};
struct PatchSet {
    Patch items[16]{};volatile LONG count=0;
    void* original(void* self) {
        void** vt=*reinterpret_cast<void***>(self);LONG n=count;
        for(LONG i=0;i<n;++i) if(items[i].vtable==vt) return items[i].original;
        return nullptr;
    }
    bool has(void** vt) {LONG n=count;for(LONG i=0;i<n;++i) if(items[i].vtable==vt) return true;return false;}
    bool install(void* object,int index,void* hook) {
        void** vt=*reinterpret_cast<void***>(object);
        if(has(vt)) return true;
        if(count>=16) return false;
        if(vt[index]==hook) return true;
        DWORD old=0;if(!VirtualProtect(&vt[index],sizeof(void*),PAGE_READWRITE,&old)) return false;
        items[count]={vt,vt[index]};MemoryBarrier();InterlockedIncrement(&count);
        vt[index]=hook;DWORD ignored=0;VirtualProtect(&vt[index],sizeof(void*),old,&ignored);
        return true;
    }
    // 0.8.62: puts the hook back when something else rewrote the slot (the Direct3D 9 runtime refills the device vtable it shares with the
    // second device when that device is created). The new content becomes the original to call. True when the slot had to be repaired.
    bool refresh(void* object,int index,void* hook,void** previous=nullptr) {
        void** vt=*reinterpret_cast<void***>(object);
        for(LONG i=0;i<count;++i) if(items[i].vtable==vt) {
            if(vt[index]==hook) return false;
            if(previous) *previous=vt[index];
            DWORD old=0;if(!VirtualProtect(&vt[index],sizeof(void*),PAGE_READWRITE,&old)) return false;
            items[i].original=vt[index];MemoryBarrier();vt[index]=hook;DWORD ignored=0;VirtualProtect(&vt[index],sizeof(void*),old,&ignored);
            return true;
        }
        return false;
    }
};
