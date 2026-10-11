// made by LoVeMaKeRz
#include "../src/plugin/MenuChains.hpp"
#include <cassert>
#include <cstdio>
struct V{float x,y,z,rhw;std::uint32_t color;float u,v;};
int main(){
    std::vector<V> v;std::vector<std::uint32_t> ix;std::vector<unsigned char> out;std::vector<std::uint32_t> oi;
    auto k=MenuMaterials::chainProfile[2]; // A visible fragment, no first-profile anchor or full chain.
    for(unsigned i=0;i<3;++i)v.push_back({float(i),float(i==2),.9f,.5f,0x7f202020,k[i*2]/1000000.f,k[i*2+1]/1000000.f});
    for(unsigned i=0;i<3;++i){auto p=v[i];p.color=0x7f000000;v.push_back(p);}ix={0,1,2,5,4,3};auto saved=v;
    auto run=[&](){return MenuChains::correct(v.data(),28,v.size(),ix.data(),true,ix.size()/3,out,oi);};
    auto st=run();assert(st.removed==1&&st.lit==3&&oi==std::vector<std::uint32_t>({0,1,2}));
    for(unsigned i=0;i<3;++i){assert(MenuFaces::color(out.data(),28,i)==0x7fffffff);assert(std::memcmp(out.data()+i*28,&v[i],16)==0);assert(std::memcmp(out.data()+i*28+20,&v[i].u,8)==0);}
    assert(std::memcmp(v.data(),saved.data(),v.size()*28)==0);
    std::reverse(ix.begin(),ix.end());assert(run().removed==1);
    v[3].z+=.000001f;assert(run().removed==0&&oi.empty());v=saved;
    v[3].color=0x6f000000;assert(run().removed==0);v=saved;
    for(auto& p:v)p.u=.1f;assert(run().removed==0);v=saved;
    ix={0,1,2,3,4,5};assert(run().removed==0); // Same winding.
    ix={0,1,2,5,4,3,0,1,2};assert(run().removed==0); // Three layers are not an opposite pair.
    ix={0,1,2,5,4,999999};assert(run().removed==0&&oi.empty());ix={0,1,2,5,4,3};
    std::vector<std::uint16_t> small(ix.begin(),ix.end());
    st=MenuChains::correct(v.data(),28,v.size(),small.data(),false,2,out,oi);assert(st.removed==1);
    std::puts("PASS MenuChainsTests: partial chain pair, no full-profile anchor, exact Z/UV/alpha, opposite winding, order, input immutability, unknown materials/layers/indices and 16/32-bit indices");
}
