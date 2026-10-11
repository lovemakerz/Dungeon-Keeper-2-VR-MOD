// DK2VR 0.8.59 - the fast change-detection checksum must give exactly the engine's value.
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "../src/common/FastCrc.hpp"

static int failures=0;
#define CHECK(c) do{if(!(c)){std::printf("FAIL line %d: %s\n",__LINE__,#c);++failures;}}while(0)

int main() {
    // The engine's table (CRC-16/ARC, reflected 0xA001) in 32-bit entries, as read at 0x66FF20 in DKII 1.70.
    uint32_t engine[256];
    for(uint32_t i=0;i<256;++i) {uint32_t c=i;for(int k=0;k<8;++k) c=(c&1)?(c>>1)^0xA001u:(c>>1);engine[i]=c;}
    CHECK(engine[1]==0xc0c1&&engine[255]==0x4040);
    static fastcrc::Tables T;
    CHECK(fastcrc::build(engine,T));
    // A table that is not a 16-bit linear CRC table is refused.
    uint32_t bad[256];std::memcpy(bad,engine,sizeof(bad));bad[7]^=1;CHECK(!fastcrc::build(bad,T));
    uint32_t wide[256];std::memcpy(wide,engine,sizeof(wide));wide[3]|=0x10000;CHECK(!fastcrc::build(wide,T));
    CHECK(fastcrc::build(engine,T));

    std::srand(12345);
    std::vector<unsigned char> buffer(1024*768*4+64);
    for(auto& b:buffer) b=static_cast<unsigned char>(std::rand());
    // Whole 2D image as the engine strips it: 128-pixel wide strips of 32 rows, 32 bpp, pitch 4096, any alignment.
    for(unsigned offset=0;offset<8;++offset) {
        const unsigned char* base=buffer.data()+offset;
        CHECK(fastcrc::rows(T,512,base,32,4096)==fastcrc::referenceRows(engine,512,base,32,4096));
        CHECK(fastcrc::rows(T,256,base,32,2048)==fastcrc::referenceRows(engine,256,base,32,2048));
    }
    // Odd sizes, tails, one row, one byte, strides smaller than the row (overlapping reads are legal for the engine).
    int cases=0;
    for(unsigned rowBytes:{1u,2u,3u,7u,15u,16u,17u,31u,33u,64u,100u,512u,513u}) for(unsigned rows:{1u,2u,5u,32u}) for(unsigned stride:{1u,rowBytes,rowBytes+3u,4096u}) {
        for(unsigned offset:{0u,1u,2u,3u}) {
            const unsigned char* base=buffer.data()+offset;
            if(size_t(rows)*stride+rowBytes>buffer.size()-8) continue;
            ++cases;
            CHECK(fastcrc::rows(T,rowBytes,base,rows,stride)==fastcrc::referenceRows(engine,rowBytes,base,rows,stride));
        }
    }
    // Chaining across calls equals one call.
    const uint32_t a=fastcrc::update(T,0xfbea,buffer.data()+3,1000);
    CHECK(fastcrc::update(T,a,buffer.data()+1003,777)==fastcrc::referenceUpdate(engine,0xfbea,buffer.data()+3,1777));
    // Constant data and all-zero data.
    std::vector<unsigned char> zero(65536,0),ff(65536,0xff);
    CHECK(fastcrc::update(T,0xfbea,zero.data(),zero.size())==fastcrc::referenceUpdate(engine,0xfbea,zero.data(),zero.size()));
    CHECK(fastcrc::update(T,0xfbea,ff.data(),ff.size())==fastcrc::referenceUpdate(engine,0xfbea,ff.data(),ff.size()));
    std::printf("FastCrcTests: %s (%d shapes)\n",failures?"FAILED":"all passed",cases);
    return failures?1:0;
}
