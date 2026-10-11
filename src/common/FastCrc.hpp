#pragma once
// DK2VR 0.8.59 - exact, faster replacement for the engine's change-detection checksum.
//
// DKII's texture update path (0x590C30, called for every 128x32 strip of the 2D interface image, each frame)
// computes a 16-bit reflected CRC byte by byte (table at 0x66FF20, register started at 0xFBEA, chained over the rows,
// no final xor): ~6 cycles per byte on the dependency chain = about 14 % of the game thread. This header gives the
// SAME value with a "slicing-by-16" evaluation (about 0.6 cycle per byte). The tables are built from the engine's own
// 256-entry table and validated first (16-bit entries, linear in the index), so any other table is refused.
#include <cstdint>
#include <cstddef>
#include <cstring>

namespace fastcrc {

struct Tables {
    uint32_t t[16][256];
};

// 'engine' = the 256 32-bit entries of the engine table. Returns false when it is not a linear 16-bit CRC table.
inline bool build(const uint32_t* engine,Tables& out) {
    if(engine[0]!=0) return false;
    for(int i=0;i<256;++i) if(engine[i]>0xffffu) return false;
    for(int i=0;i<256;++i) for(int b=0;b<8;++b) if(engine[i^(1<<b)]!=(engine[i]^engine[1<<b])) return false;
    for(int i=0;i<256;++i) out.t[0][i]=engine[i];
    for(int k=1;k<16;++k) for(int i=0;i<256;++i) {const uint32_t v=out.t[k-1][i];out.t[k][i]=(v>>8)^engine[v&0xff];}
    return true;
}

// Byte-by-byte reference (exactly the engine's loop).
inline uint32_t referenceUpdate(const uint32_t* engine,uint32_t c,const unsigned char* p,size_t n) {
    while(n--) c=(c>>8)^engine[(c^*p++)&0xff];
    return c;
}
inline uint32_t referenceRows(const uint32_t* engine,uint32_t rowBytes,const unsigned char* base,uint32_t rows,uint32_t stride) {
    uint32_t c=0xfbea;
    for(uint32_t r=0;r<rows;++r) c=referenceUpdate(engine,c,base+size_t(r)*stride,rowBytes);
    return c;
}

inline uint32_t load32(const unsigned char* p) {uint32_t v;std::memcpy(&v,p,4);return v;}

inline uint32_t update(const Tables& T,uint32_t c,const unsigned char* p,size_t n) {
    while(n&&(reinterpret_cast<uintptr_t>(p)&3)) {c=(c>>8)^T.t[0][(c^*p++)&0xff];--n;}
    while(n>=16) {
        const uint32_t a=load32(p)^c,b=load32(p+4),d=load32(p+8),e=load32(p+12);
        c=T.t[15][a&0xff]^T.t[14][(a>>8)&0xff]^T.t[13][(a>>16)&0xff]^T.t[12][a>>24]
         ^T.t[11][b&0xff]^T.t[10][(b>>8)&0xff]^T.t[9][(b>>16)&0xff]^T.t[8][b>>24]
         ^T.t[7][d&0xff]^T.t[6][(d>>8)&0xff]^T.t[5][(d>>16)&0xff]^T.t[4][d>>24]
         ^T.t[3][e&0xff]^T.t[2][(e>>8)&0xff]^T.t[1][(e>>16)&0xff]^T.t[0][e>>24];
        p+=16;n-=16;
    }
    while(n--) c=(c>>8)^T.t[0][(c^*p++)&0xff];
    return c;
}

// The engine's loop: one register chained over 'rows' rows of 'rowBytes' bytes, 'stride' bytes apart.
inline uint32_t rows(const Tables& T,uint32_t rowBytes,const unsigned char* base,uint32_t rowCount,uint32_t stride) {
    uint32_t c=0xfbea;
    for(uint32_t r=0;r<rowCount;++r) c=update(T,c,base+size_t(r)*stride,rowBytes);
    return c;
}

} // namespace fastcrc
