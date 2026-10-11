#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
namespace dk2vr {
template<class Record> bool mergeDraw(Record& previous,const Record& next,unsigned char* indices,int triangleList,int index16) {
    if(previous.type!=triangleList||next.type!=triangleList||previous.hand||next.hand||previous.held3d||next.held3d||
       previous.snapshot!=next.snapshot||previous.fvf!=next.fvf||previous.stride!=next.stride||
       previous.indexed!=next.indexed||previous.indexFormat!=next.indexFormat||
       previous.vertices+size_t(previous.count)*previous.stride!=next.vertices||
       previous.count>32768||next.count>32768-previous.count) return false;
    if(previous.indexed) {
        const unsigned bytes=int(next.indexFormat)==index16?2:4;
        if(previous.indices+size_t(previous.primitives)*3*bytes!=next.indices) return false;
        const size_t n=size_t(next.primitives)*3;
        for(size_t i=0;i<n;++i) {
            uint32_t value=0;memcpy(&value,indices+next.indices+i*bytes,bytes);
            if(value>=next.count) return false;
        }
        for(size_t i=0;i<n;++i) {
            uint32_t value=0;memcpy(&value,indices+next.indices+i*bytes,bytes);value+=previous.count;
            memcpy(indices+next.indices+i*bytes,&value,bytes);
        }
    } else if(previous.count!=previous.primitives*3||next.count!=next.primitives*3) return false;
    previous.count+=next.count;previous.primitives+=next.primitives;return true;
}
}
