// made by LoVeMaKeRz
#include "../src/plugin/MenuFaces.hpp"
#include <cassert>
#include <cstdio>
struct V {float x,y,z,rhw;std::uint32_t color;float u,v;};
int main() {
    std::vector<V> v;std::vector<std::uint32_t> ix;
    for(unsigned t=0;t<16;++t) {
        const unsigned n=v.size();float x=float(t)*10;
        v.push_back({x,0,.9f,1,0xffffffff,0,0});v.push_back({x+4,0,.9f,1,0xffffffff,1,0});v.push_back({x,4,.9f,1,0xffffffff,0,1});
        for(unsigned i=0;i<3;++i){auto a=v[n+i];a.color=0xff404040;v.push_back(a);}
        ix.insert(ix.end(),{n,n+1,n+2,n+5,n+4,n+3});
    }
    auto original=v;std::vector<unsigned char> out;std::vector<std::uint32_t> oi;
    auto run=[&](){return MenuFaces::correct(v.data(),sizeof(V),v.size(),ix.data(),true,ix.size()/3,out,oi);};
    auto st=run();assert(st.flags==16&&st.caps==0&&oi.size()==48&&out.empty());
    for(unsigned t=0;t<16;++t)assert(oi[t*3]==t*6); // all original lit front faces, including reversed input order below.
    assert(std::memcmp(v.data(),original.data(),v.size()*sizeof(V))==0);
    std::reverse(ix.begin(),ix.end());st=run();assert(st.flags==16&&oi.size()==48);
    // Real geometry is not a duplicate: different Z, UV, partial pair or same winding fails closed.
    v[3].z+=.000001f;assert(run().flags==0&&oi.empty());v=original;
    v[3].u=.01f;assert(run().flags==0);v=original;
    ix.resize(ix.size()-3);assert(run().flags==0);ix={0,1,999999};assert(run().flags==0&&oi.empty());
    // End-cap colours only: complete asset profile plus lit counterpart, no global black-face whitening.
    static const float capUV[22][6]={
        {22831.f/1000000.f,677749.f/1000000.f,30189.f/1000000.f,635310.f/1000000.f,158209.f/1000000.f,658178.f/1000000.f},
        {22831.f/1000000.f,677749.f/1000000.f,41725.f/1000000.f,710846.f/1000000.f,140405.f/1000000.f,696796.f/1000000.f},
        {22831.f/1000000.f,677749.f/1000000.f,140405.f/1000000.f,696796.f/1000000.f,158209.f/1000000.f,658178.f/1000000.f},
        {30189.f/1000000.f,635310.f/1000000.f,64525.f/1000000.f,595089.f/1000000.f,158209.f/1000000.f,658178.f/1000000.f},
        {41725.f/1000000.f,710846.f/1000000.f,94744.f/1000000.f,718104.f/1000000.f,140405.f/1000000.f,696796.f/1000000.f},
        {64525.f/1000000.f,595089.f/1000000.f,140829.f/1000000.f,545000.f/1000000.f,142888.f/1000000.f,611482.f/1000000.f},
        {64525.f/1000000.f,595089.f/1000000.f,142888.f/1000000.f,611482.f/1000000.f,158209.f/1000000.f,658178.f/1000000.f},
        {140829.f/1000000.f,545000.f/1000000.f,142888.f/1000000.f,611482.f/1000000.f,225005.f/1000000.f,572177.f/1000000.f},
        {140829.f/1000000.f,545000.f/1000000.f,225005.f/1000000.f,572177.f/1000000.f,226670.f/1000000.f,515754.f/1000000.f},
        {225005.f/1000000.f,572177.f/1000000.f,226670.f/1000000.f,515754.f/1000000.f,312148.f/1000000.f,554741.f/1000000.f},
        {226670.f/1000000.f,515754.f/1000000.f,312148.f/1000000.f,554741.f/1000000.f,313450.f/1000000.f,504409.f/1000000.f},
        {312148.f/1000000.f,554741.f/1000000.f,313450.f/1000000.f,504409.f/1000000.f,387725.f/1000000.f,557816.f/1000000.f},
        {313450.f/1000000.f,504409.f/1000000.f,387725.f/1000000.f,557816.f/1000000.f,397838.f/1000000.f,510248.f/1000000.f},
        {387725.f/1000000.f,557816.f/1000000.f,397838.f/1000000.f,510248.f/1000000.f,463514.f/1000000.f,569834.f/1000000.f},
        {397838.f/1000000.f,510248.f/1000000.f,463514.f/1000000.f,569834.f/1000000.f,478623.f/1000000.f,526345.f/1000000.f},
        {463514.f/1000000.f,569834.f/1000000.f,478623.f/1000000.f,526345.f/1000000.f,759977.f/1000000.f,674054.f/1000000.f},
        {478623.f/1000000.f,526345.f/1000000.f,759977.f/1000000.f,674054.f/1000000.f,767426.f/1000000.f,621711.f/1000000.f},
        {759977.f/1000000.f,674054.f/1000000.f,767426.f/1000000.f,621711.f/1000000.f,794071.f/1000000.f,698873.f/1000000.f},
        {767426.f/1000000.f,621711.f/1000000.f,794071.f/1000000.f,698873.f/1000000.f,819415.f/1000000.f,598925.f/1000000.f},
        {794071.f/1000000.f,698873.f/1000000.f,819415.f/1000000.f,598925.f/1000000.f,996094.f/1000000.f,686286.f/1000000.f},
        {819415.f/1000000.f,598925.f/1000000.f,865560.f/1000000.f,555717.f/1000000.f,996094.f/1000000.f,686286.f/1000000.f},
        {865560.f/1000000.f,555717.f/1000000.f,926088.f/1000000.f,552214.f/1000000.f,996094.f/1000000.f,686286.f/1000000.f},
    };
    v.clear();ix.clear();
    for(unsigned t=0;t<22;++t)for(unsigned side=0;side<2;++side){
        unsigned n=v.size();for(unsigned i=0;i<3;++i)v.push_back({float(n+i),float(i==2),.8f,1,side?0xffffffffu:0xff000000u,capUV[t][i*2],capUV[t][i*2+1]});
        ix.insert(ix.end(),{n,n+1,n+2});
    }
    unsigned floor=v.size();v.push_back({0,0,.1f,1,0xff000000,0,0});v.push_back({0,4,.1f,1,0xff000000,0,.1f});v.push_back({4,0,.1f,1,0xff000000,.1f,0});ix.insert(ix.end(),{floor,floor+1,floor+2});
    original=v;st=run();assert(st.flags==0&&st.caps==22&&oi==ix&&out.size()==v.size()*sizeof(V));
    for(unsigned t=0;t<22;++t)for(unsigned i=0;i<3;++i)assert(MenuFaces::color(out.data(),sizeof(V),t*6+i)==0xffffffffu);
    assert(MenuFaces::color(out.data(),sizeof(V),floor)==0xff000000u);assert(std::memcmp(v.data(),original.data(),v.size()*sizeof(V))==0);
    for(auto& a:v)if(a.color==0xffffffff)a.color=0xfffffffb;st=run();assert(st.caps==22&&MenuFaces::color(out.data(),sizeof(V),0)==0xfffffffb);v=original;
    v[0].u+=.01f;v[3].u+=.01f;assert(run().caps==0&&out.empty());v=original;
    for(auto& a:v)if(a.color==0xffffffff)a.color=0xff000000;assert(run().caps==0&&out.empty());
    std::puts("PASS MenuFacesTests: lit flag pairs, order, input immutability, native depth/UV separation, incomplete geometry and invalid indices");
}
