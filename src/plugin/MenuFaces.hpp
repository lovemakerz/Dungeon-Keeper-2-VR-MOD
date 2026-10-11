// made by LoVeMaKeRz
// DKII 1.70 menu only. Called BEFORE relief/tessellation, never for dungeon draws.
#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <map>
#include <vector>

namespace MenuFaces {
struct Stats { unsigned flags=0, caps=0; };
using Key=std::array<std::uint32_t,18>;
using UV=std::array<int,6>;
inline UV uvKey(const unsigned char* v,unsigned stride,const std::uint32_t* tri) {
    std::array<std::array<int,2>,3> p{};
    for(unsigned i=0;i<3;++i) for(unsigned j=0;j<2;++j) {
        float x;std::memcpy(&x,v+tri[i]*stride+20+j*4,4);
        if(!std::isfinite(x)||std::fabs(x)>2.f) return UV{};
        p[i][j]=int(std::lround(double(x)*1000000.));
    }
    std::sort(p.begin(),p.end());UV k{};
    for(unsigned i=0;i<3;++i) for(unsigned j=0;j<2;++j)k[i*2+j]=p[i][j];
    return k;
}
inline std::uint32_t color(const unsigned char* v,unsigned stride,unsigned i) {
    std::uint32_t c;std::memcpy(&c,v+i*stride+16,4);return c;
}
inline unsigned light(std::uint32_t c) {return (c&255)+((c>>8)&255)+((c>>16)&255);}
inline Stats correct(const void* input,unsigned stride,unsigned count,const void* indices,
                     bool index32,unsigned primitives,std::vector<unsigned char>& vertices,
                     std::vector<std::uint32_t>& output) {
    Stats result{};vertices.clear();output.clear();
    if(!input||!indices||stride!=28||count==0||count>4096||primitives==0||primitives>4096)return result;
    const auto* v=static_cast<const unsigned char*>(input);
    std::vector<std::uint32_t> ix(primitives*3);
    for(unsigned i=0;i<ix.size();++i) {
        if(index32)std::memcpy(&ix[i],static_cast<const unsigned char*>(indices)+i*4,4);
        else {std::uint16_t n;std::memcpy(&n,static_cast<const unsigned char*>(indices)+i*2,2);ix[i]=n;}
        if(ix[i]>=count)return result;
    }
    // A flag batch consists entirely of pairs with EXACT XYZRHW and UV,
    // reversed winding, and different diffuse colours. No approximation of Z.
    bool candidate=false;
    if(primitives>=32&&(primitives%2)==0)for(unsigned t=1;t<primitives&&!candidate;++t) {
        bool match=true;
        for(unsigned i=0;i<3&&match;++i) {
            const auto* b=v+ix[t*3+i]*stride;bool found=false;
            for(unsigned j=0;j<3;++j){const auto* a=v+ix[j]*stride;
                if(std::memcmp(a,b,16)==0&&std::memcmp(a+20,b+20,8)==0){found=true;break;}}
            match=found;
        }
        candidate=match;
    }
    std::map<Key,std::vector<unsigned>> pairs;
    for(unsigned t=0;candidate&&t<primitives;++t) {
        std::array<std::array<std::uint32_t,6>,3> p{};
        for(unsigned i=0;i<3;++i) {
            auto* a=v+ix[t*3+i]*stride;
            std::memcpy(p[i].data(),a,16);std::memcpy(p[i].data()+4,a+20,8);
        }
        std::sort(p.begin(),p.end());Key k{};
        for(unsigned i=0;i<3;++i)std::copy(p[i].begin(),p[i].end(),k.begin()+i*6);
        pairs[k].push_back(t);
    }
    bool paired=candidate&&pairs.size()*2==primitives;
    std::vector<bool> keep(primitives,true);
    auto area=[&](unsigned t) {float p[3][2];for(unsigned i=0;i<3;++i)std::memcpy(p[i],v+ix[t*3+i]*stride,8);
        return (double(p[1][0])-p[0][0])*(double(p[2][1])-p[0][1])-(double(p[1][1])-p[0][1])*(double(p[2][0])-p[0][0]);};
    if(paired)for(const auto& pair:pairs) {
        if(pair.second.size()!=2||area(pair.second[0])*area(pair.second[1])>=0){paired=false;break;}
    }
    if(paired)for(const auto& pair:pairs) {
        auto brightness=[&](unsigned t){unsigned n=0;for(unsigned i=0;i<3;++i)n+=light(color(v,stride,ix[t*3+i]));return n;};
        unsigned a=pair.second[0],b=pair.second[1];keep[brightness(a)>=brightness(b)?b:a]=false;++result.flags;
    }
    // Full 22-triangle UV profile of the hanger end cap, observed on both
    // actual F11 frames. Atlas IDs and screen positions are NOT identifiers.
    // All profile triangles must be present, including a uniformly lit counterpart.
    static constexpr UV cap[] = {
        UV{{22831,677749,30189,635310,158209,658178}},
        UV{{22831,677749,41725,710846,140405,696796}},
        UV{{22831,677749,140405,696796,158209,658178}},
        UV{{30189,635310,64525,595089,158209,658178}},
        UV{{41725,710846,94744,718104,140405,696796}},
        UV{{64525,595089,140829,545000,142888,611482}},
        UV{{64525,595089,142888,611482,158209,658178}},
        UV{{140829,545000,142888,611482,225005,572177}},
        UV{{140829,545000,225005,572177,226670,515754}},
        UV{{225005,572177,226670,515754,312148,554741}},
        UV{{226670,515754,312148,554741,313450,504409}},
        UV{{312148,554741,313450,504409,387725,557816}},
        UV{{313450,504409,387725,557816,397838,510248}},
        UV{{387725,557816,397838,510248,463514,569834}},
        UV{{397838,510248,463514,569834,478623,526345}},
        UV{{463514,569834,478623,526345,759977,674054}},
        UV{{478623,526345,759977,674054,767426,621711}},
        UV{{759977,674054,767426,621711,794071,698873}},
        UV{{767426,621711,794071,698873,819415,598925}},
        UV{{794071,698873,819415,598925,996094,686286}},
        UV{{819415,598925,865560,555717,996094,686286}},
        UV{{865560,555717,926088,552214,996094,686286}},
    };
    bool capCandidate=false;
    for(unsigned t=0;t<primitives&&!capCandidate;++t) {
        bool allBlack=true;for(unsigned i=0;i<3;++i)allBlack&=(color(v,stride,ix[t*3+i])&0xffffff)==0;
        if(allBlack){auto k=uvKey(v,stride,ix.data()+t*3);auto it=std::lower_bound(std::begin(cap),std::end(cap),k);capCandidate=it!=std::end(cap)&&*it==k;}
    }
    std::uint32_t mask=0, litColor=0;unsigned litBrightness=0;
    std::vector<unsigned> black;
    for(unsigned t=0;capCandidate&&t<primitives;++t) {
        const auto k=uvKey(v,stride,ix.data()+t*3);
        auto it=std::lower_bound(std::begin(cap),std::end(cap),k);
        if(it==std::end(cap)||*it!=k)continue;
        mask|=1u<<unsigned(it-std::begin(cap));
        bool allBlack=true,uniform=true;
        const auto first=color(v,stride,ix[t*3])&0xffffff;
        for(unsigned i=0;i<3;++i){auto c=color(v,stride,ix[t*3+i])&0xffffff;allBlack&=c==0;uniform&=c==first;}
        if(uniform&&light(first)>=576&&light(first)>litBrightness){litBrightness=light(first);litColor=first;}
        if(allBlack)black.push_back(t);
    }
    if(mask==((1u<<22)-1)&&litBrightness&&!black.empty()) {
        vertices.assign(v,v+std::size_t(count)*stride);
        for(unsigned t:black)for(unsigned i=0;i<3;++i) {
            auto n=ix[t*3+i];auto c=(color(v,stride,n)&0xff000000u)|litColor;
            std::memcpy(vertices.data()+n*stride+16,&c,4);
        }
        result.caps=unsigned(black.size());
    }
    if(result.flags||result.caps)for(unsigned t=0;t<primitives;++t)if(keep[t])output.insert(output.end(),ix.begin()+t*3,ix.begin()+t*3+3);
    return result;
}
}
