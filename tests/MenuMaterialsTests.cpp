// made by LoVeMaKeRz
#include "../src/plugin/MenuMaterials.hpp"
#include <cassert>
#include <cstdio>
struct V {float x,y,z,rhw;std::uint32_t c;float u,v;};
struct Fixture {
    std::vector<V> v;std::vector<std::uint32_t> ix;std::vector<unsigned char> out;
    Fixture(const MenuFaces::UV* profile,unsigned size) {
        for(unsigned t=0;t<size;++t){unsigned n=v.size();for(unsigned j=0;j<3;++j)
            v.push_back({float(n+j),float(j==2),.9f,.5f,0x7f101010,profile[t][j*2]/1000000.f,profile[t][j*2+1]/1000000.f});
            ix.insert(ix.end(),{n,n+1,n+2});}
    }
    MenuMaterials::Stats run(bool flags,bool opaque,bool chains) {
        return MenuMaterials::correct(v.data(),28,v.size(),ix.data(),true,ix.size()/3,flags,opaque,chains,out);
    }
    void verify(unsigned minimum) {
        assert(out.size()==v.size()*28);
        for(unsigned i=0;i<v.size();++i){assert(std::memcmp(out.data()+i*28,&v[i],16)==0);
            assert(std::memcmp(out.data()+i*28+20,&v[i].u,8)==0);
            assert((MenuFaces::color(out.data(),28,i)>>24)==0x7f);
            assert((MenuFaces::color(out.data(),28,i)&0xffffff)==minimum*0x010101);}
    }
};
int main(){
    Fixture flags(MenuMaterials::flagProfile,std::size(MenuMaterials::flagProfile));auto st=flags.run(true,true,false);
    assert(st.flags==flags.ix.size()/3&&st.lamps==0&&st.chains==0);flags.verify(224);
    assert(flags.run(false,true,false).flags==0&&flags.out.empty()); // no flag classification = no whitening.
    Fixture lamp(MenuMaterials::lampProfile,std::size(MenuMaterials::lampProfile));st=lamp.run(false,true,false);
    assert(st.lamps==40&&st.flags==0&&st.chains==0);lamp.verify(208);
    lamp.ix.resize(lamp.ix.size()-3);assert(lamp.run(false,true,false).lamps==0&&lamp.out.empty());
    Fixture chain(MenuMaterials::chainProfile,std::size(MenuMaterials::chainProfile));st=chain.run(false,false,true);
    assert(st.chains==4&&st.flags==0&&st.lamps==0);chain.verify(208);
    assert(chain.run(false,true,false).chains==0&&chain.out.empty());
    assert(chain.run(false,false,false).chains==0&&chain.out.empty()); // additive/unknown blending rejected by caller.
    chain.ix[0]=999999;assert(chain.run(false,false,true).chains==0&&chain.out.empty());
    // A room floor in the SAME batch retains its complete black material.
    Fixture mixed(MenuMaterials::lampProfile,std::size(MenuMaterials::lampProfile));unsigned floor=mixed.v.size();
    for(unsigned i=0;i<3;++i)mixed.v.push_back({float(i),2,.8f,.2f,0xff000000,float(i)*.1f,.2f});
    mixed.ix.insert(mixed.ix.end(),{floor,floor+1,floor+2});auto original=mixed.v;st=mixed.run(false,true,false);
    assert(st.lamps==40&&std::memcmp(mixed.out.data()+floor*28,&mixed.v[floor],3*28)==0);
    assert(std::memcmp(mixed.v.data(),original.data(),original.size()*28)==0);
    // Even a vertex shared with a different material is protected.
    mixed.ix.back()=0;st=mixed.run(false,true,false);assert(st.lamps>0);
    assert(MenuFaces::color(mixed.out.data(),28,0)==original[0].c);
    std::vector<std::uint16_t> small(chain.ix.size());chain.ix[0]=0;for(unsigned i=0;i<small.size();++i)small[i]=chain.ix[i];
    st=MenuMaterials::correct(chain.v.data(),28,chain.v.size(),small.data(),false,small.size()/3,false,false,true,chain.out);
    assert(st.chains==4);chain.verify(208);
    std::puts("PASS MenuMaterialsTests: dark flag material, lamp/chain profiles, partial and unrelated geometry, shared vertices, alpha/UV/depth/input preserved, 16/32-bit indices");
}
