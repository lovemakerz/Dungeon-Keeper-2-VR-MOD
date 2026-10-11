#pragma once
#include "Protocol.hpp"
#include <string>
#include <stdexcept>

namespace dk2vr {
class Ipc {
public:
    HANDLE mapping=nullptr, mutex=nullptr, ready=nullptr;
    // 0.3.0: signalled by the host after each new request (optional: absent with an older host).
    HANDLE requestEvent=nullptr;
    Shared* data=nullptr;
    Ipc()=default;
    Ipc(const Ipc&)=delete;
    ~Ipc() { close(); }
    void close() {
        if(data) UnmapViewOfFile(data);
        if(mapping) CloseHandle(mapping);
        if(mutex) CloseHandle(mutex);
        if(ready) CloseHandle(ready);
        if(requestEvent) CloseHandle(requestEvent);
        mapping=mutex=ready=requestEvent=nullptr; data=nullptr;
    }
    bool open(const std::wstring& token,bool create) {
        close();
        const auto base=L"Local\\DK2VR_"+token;
        if(create) {
            mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(Shared),(base+L"_map").c_str());
            if(!mapping || GetLastError()==ERROR_ALREADY_EXISTS) { close(); return false; }
            mutex=CreateMutexW(nullptr,FALSE,(base+L"_mutex").c_str());
            ready=CreateEventW(nullptr,FALSE,FALSE,(base+L"_ready").c_str());
            requestEvent=CreateEventW(nullptr,FALSE,FALSE,(base+L"_request").c_str());
        } else {
            mapping=OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE,(base+L"_map").c_str());
            mutex=OpenMutexW(SYNCHRONIZE|MUTEX_MODIFY_STATE,FALSE,(base+L"_mutex").c_str());
            ready=OpenEventW(EVENT_MODIFY_STATE|SYNCHRONIZE,FALSE,(base+L"_ready").c_str());
            requestEvent=OpenEventW(EVENT_MODIFY_STATE|SYNCHRONIZE,FALSE,(base+L"_request").c_str());
        }
        if(!mapping||!mutex||!ready) { close(); return false; }
        data=static_cast<Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(Shared)));
        if(!data) { close(); return false; }
        if(create) {
            ZeroMemory(data,sizeof(Shared));
            data->magic=Magic; data->version=ProtocolVersion; data->size=sizeof(Shared); data->hostPid=GetCurrentProcessId();
        } else if(data->magic!=Magic||data->version!=ProtocolVersion||data->size!=sizeof(Shared)) {
            close(); return false;
        }
        return true;
    }
    bool lock(DWORD timeout=2) {
        DWORD r=WaitForSingleObject(mutex,timeout);
        return r==WAIT_OBJECT_0||r==WAIT_ABANDONED;
    }
    void unlock() { ReleaseMutex(mutex); }
};
}
