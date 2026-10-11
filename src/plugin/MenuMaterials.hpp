// made by LoVeMaKeRz
// Targeted menu ambient light: complete observed UV profiles, no texture reads.
#pragma once
#include "MenuFaces.hpp"
namespace MenuMaterials {
struct Stats {unsigned flags=0,lamps=0,chains=0;};
using MenuFaces::UV;
static constexpr UV flagProfile[]={
    UV{{9296,4391,9296,128293,257040,4391}},
    UV{{9296,128293,9296,252195,257040,128293}},
    UV{{9296,128293,257040,4391,257040,128293}},
    UV{{9296,252195,9296,376067,257040,252195}},
    UV{{9296,252195,257040,128293,257040,252195}},
    UV{{9296,376067,9296,499970,257040,376067}},
    UV{{9296,376067,257040,252195,257040,376067}},
    UV{{9296,499970,9296,623872,257040,499970}},
    UV{{9296,499970,257040,376067,257040,499970}},
    UV{{9296,623872,9296,747774,257040,623872}},
    UV{{9296,623872,257040,499970,257040,623872}},
    UV{{9296,747774,9296,871677,257040,747774}},
    UV{{9296,747774,257040,623872,257040,747774}},
    UV{{9296,871677,9296,995579,257040,747774}},
    UV{{257040,4391,257040,128293,504784,4391}},
    UV{{257040,128293,257040,252195,504784,128293}},
    UV{{257040,128293,504784,4391,504784,128293}},
    UV{{257040,252195,257040,376067,504784,252195}},
    UV{{257040,252195,504784,128293,504784,252195}},
    UV{{257040,376067,257040,499970,504784,376067}},
    UV{{257040,376067,504784,252195,504784,376067}},
    UV{{257040,499970,257040,623872,504784,499970}},
    UV{{257040,499970,504784,376067,504784,499970}},
    UV{{257040,623872,257040,747774,504784,623872}},
    UV{{257040,623872,504784,499970,504784,623872}},
    UV{{257040,747774,504784,623872,504784,747774}},
    UV{{257040,747774,504784,747774,504784,871677}},
    UV{{257040,747774,504784,871677,504784,995579}},
    UV{{505390,4391,505390,128293,753134,4391}},
    UV{{505390,128293,505390,252195,753134,128293}},
    UV{{505390,128293,753134,4391,753134,128293}},
    UV{{505390,252195,505390,376067,753134,252195}},
    UV{{505390,252195,753134,128293,753134,252195}},
    UV{{505390,376067,505390,499970,753134,376067}},
    UV{{505390,376067,753134,252195,753134,376067}},
    UV{{505390,499970,505390,623872,753134,499970}},
    UV{{505390,499970,753134,376067,753134,499970}},
    UV{{505390,623872,505390,747774,753134,623872}},
    UV{{505390,623872,753134,499970,753134,623872}},
    UV{{505390,747774,505390,871677,753134,747774}},
    UV{{505390,747774,753134,623872,753134,747774}},
    UV{{505390,871677,505390,995579,753134,747774}},
    UV{{753134,4391,753134,128293,996094,4391}},
    UV{{753134,128293,753134,252195,996094,128293}},
    UV{{753134,128293,996094,4391,996094,128293}},
    UV{{753134,252195,753134,376067,996094,252195}},
    UV{{753134,252195,996094,128293,996094,252195}},
    UV{{753134,376067,753134,499970,996094,376067}},
    UV{{753134,376067,996094,252195,996094,376067}},
    UV{{753134,499970,753134,623872,996094,499970}},
    UV{{753134,499970,996094,376067,996094,499970}},
    UV{{753134,623872,753134,747774,996094,623872}},
    UV{{753134,623872,996094,499970,996094,623872}},
    UV{{753134,747774,996094,623872,996094,747774}},
    UV{{753134,747774,996094,747774,996094,871677}},
    UV{{753134,747774,996094,871677,996094,995579}},
};
static constexpr UV lampProfile[]={
    UV{{503906,544675,503906,552857,541342,544675}},
    UV{{503906,544675,541342,544675,564446,504025}},
    UV{{503906,544675,564446,504025,633108,544675}},
    UV{{503906,552857,503906,579752,541342,552857}},
    UV{{503906,552857,541342,544675,541342,552857}},
    UV{{503906,579752,503906,737853,541342,579752}},
    UV{{503906,579752,541342,552857,541342,579752}},
    UV{{503906,737853,541342,579752,541342,737853}},
    UV{{503906,737853,541342,737853,625000,745968}},
    UV{{541342,544675,541342,552857,587557,544675}},
    UV{{541342,544675,564446,504025,587557,544675}},
    UV{{541342,552857,541342,579752,587557,552857}},
    UV{{541342,552857,587557,544675,587557,552857}},
    UV{{541342,579752,541342,737853,587557,579752}},
    UV{{541342,579752,587557,552857,587557,579752}},
    UV{{541342,737853,587557,579752,587557,737853}},
    UV{{541342,737853,587557,737853,625000,745968}},
    UV{{564446,504025,587557,544675,633108,544675}},
    UV{{587557,544675,587557,552857,633108,544675}},
    UV{{587557,552857,587557,579752,633108,552857}},
    UV{{587557,552857,633108,544675,633108,552857}},
    UV{{587557,579752,587557,737853,633108,579752}},
    UV{{587557,579752,633108,552857,633108,579752}},
    UV{{587557,737853,625000,745968,633108,737853}},
    UV{{587557,737853,633108,579752,633108,737853}},
    UV{{625000,745968,633108,737853,685547,737853}},
    UV{{625000,745968,685547,737853,737978,737853}},
    UV{{633108,544675,633108,552857,685547,544675}},
    UV{{633108,552857,633108,579752,685547,552857}},
    UV{{633108,552857,685547,544675,685547,552857}},
    UV{{633108,579752,633108,737853,685547,579752}},
    UV{{633108,579752,685547,552857,685547,579752}},
    UV{{633108,737853,685547,579752,685547,737853}},
    UV{{685547,544675,685547,552857,737978,544675}},
    UV{{685547,544675,737978,544675,746094,504025}},
    UV{{685547,552857,685547,579752,737978,552857}},
    UV{{685547,552857,737978,544675,737978,552857}},
    UV{{685547,579752,685547,737853,737978,579752}},
    UV{{685547,579752,737978,552857,737978,579752}},
    UV{{685547,737853,737978,579752,737978,737853}},
};
static constexpr UV chainProfile[]={
    UV{{4391,97257,270908,4391,363441,995337}},
    UV{{270908,4391,363441,995337,823169,145431}},
    UV{{363441,995337,542391,995579,995579,211894}},
    UV{{363441,995337,823169,145431,995579,211894}},
};
inline std::uint32_t ambient(std::uint32_t c,unsigned minimum) {
    std::uint32_t out=c&0xff000000u;
    for(unsigned shift:{0u,8u,16u})out|=std::max(minimum,(c>>shift)&255u)<<shift;
    return out;
}
inline Stats correct(const void* input,unsigned stride,unsigned count,const void* indices,
                     bool index32,unsigned primitives,bool pairedFlags,bool opaque,bool alphaChains,
                     std::vector<unsigned char>& output) {
    Stats stats{};output.clear();
    if(!input||!indices||stride!=28||count==0||count>4096||primitives==0||primitives>4096||(!opaque&&!alphaChains))return stats;
    const auto* v=static_cast<const unsigned char*>(input);
    // Every full profile contains its first UV point. Fast reject unrelated room/effect draws.
    auto hasAnchor=[&](const UV* profile){
        const float u=profile[0][0]/1000000.f,w=profile[0][1]/1000000.f;
        for(unsigned i=0;i<count;++i){float q[2];std::memcpy(q,v+i*stride+20,8);
            if(std::fabs(q[0]-u)<.000001f&&std::fabs(q[1]-w)<.000001f)return true;}
        return false;
    };
    if(opaque&&!pairedFlags&&!hasAnchor(lampProfile))return stats;
    if(alphaChains&&!hasAnchor(chainProfile))return stats;
    std::vector<std::uint32_t> ix(primitives*3);
    for(unsigned i=0;i<ix.size();++i){
        if(index32)std::memcpy(&ix[i],static_cast<const unsigned char*>(indices)+i*4,4);
        else {std::uint16_t n;std::memcpy(&n,static_cast<const unsigned char*>(indices)+i*2,2);ix[i]=n;}
        if(ix[i]>=count)return stats;
    }
    auto apply=[&](const UV* profile,unsigned size,bool full,unsigned minimum,unsigned& triangles) {
        std::uint64_t mask=0;std::vector<bool> matches(primitives,false);
        unsigned found=0;
        for(unsigned t=0;t<primitives;++t){
            auto k=MenuFaces::uvKey(v,stride,ix.data()+t*3);auto end=profile+size;auto it=std::lower_bound(profile,end,k);
            if(it!=end&&*it==k){mask|=std::uint64_t(1)<<unsigned(it-profile);matches[t]=true;++found;}
        }
        if(full ? mask!=((std::uint64_t(1)<<size)-1) : found!=primitives)return;
        // Vertices shared with other materials are left completely unchanged.
        std::vector<bool> selected(count,false),other(count,false);
        for(unsigned t=0;t<primitives;++t)for(unsigned j=0;j<3;++j)(matches[t]?selected:other)[ix[t*3+j]]=true;
        for(unsigned t=0;t<primitives;++t)if(matches[t]){
            bool changed=false;
            for(unsigned j=0;j<3;++j){auto n=ix[t*3+j];if(!selected[n]||other[n])continue;
                auto c=MenuFaces::color(v,stride,n),lit=ambient(c,minimum);
                if(c==lit)continue;
                if(output.empty())output.assign(v,v+std::size_t(count)*stride);
                std::memcpy(output.data()+n*stride+16,&lit,4);changed=true;
            }
            if(changed)++triangles;
        }
    };
    if(opaque&&pairedFlags)apply(flagProfile,unsigned(std::size(flagProfile)),false,224,stats.flags);
    if(opaque&&!pairedFlags)apply(lampProfile,unsigned(std::size(lampProfile)),true,208,stats.lamps);
    if(alphaChains)apply(chainProfile,unsigned(std::size(chainProfile)),true,208,stats.chains);
    return stats;
}
}
