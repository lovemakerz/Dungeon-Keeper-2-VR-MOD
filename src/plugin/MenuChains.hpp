// made by LoVeMaKeRz
// Only exact opposite copies of the observed chain UV triangles, including subsets.
#pragma once
#include "MenuMaterials.hpp"
namespace MenuChains {
struct Stats {unsigned removed=0,lit=0;};
using Key=std::array<std::uint32_t,21>;
inline Stats correct(const void* input,unsigned stride,unsigned count,const void* indices,bool index32,unsigned primitives,
                     std::vector<unsigned char>& vertices,std::vector<std::uint32_t>& output) {
    Stats result{};vertices.clear();output.clear();
    if(!input||!indices||stride!=28||count==0||count>4096||primitives<2||primitives>1024)return result;
    const auto* v=static_cast<const unsigned char*>(input);
    std::vector<std::uint32_t> ix(primitives*3);
    for(unsigned i=0;i<ix.size();++i){
        if(index32)std::memcpy(&ix[i],static_cast<const unsigned char*>(indices)+i*4,4);
        else {std::uint16_t n;std::memcpy(&n,static_cast<const unsigned char*>(indices)+i*2,2);ix[i]=n;}
        if(ix[i]>=count)return result;
    }
    std::map<Key,std::vector<unsigned>> pairs;std::vector<bool> known(primitives,false),keep(primitives,true);
    for(unsigned t=0;t<primitives;++t){
        auto uv=MenuFaces::uvKey(v,stride,ix.data()+t*3);
        auto first=std::begin(MenuMaterials::chainProfile),end=std::end(MenuMaterials::chainProfile);
        auto found=std::lower_bound(first,end,uv);if(found==end||*found!=uv)continue;
        known[t]=true;
        std::array<std::array<std::uint32_t,7>,3> points{};
        for(unsigned j=0;j<3;++j){const auto* p=v+ix[t*3+j]*stride;
            std::memcpy(points[j].data(),p,16);std::memcpy(points[j].data()+4,p+20,8);
            points[j][6]=MenuFaces::color(v,stride,ix[t*3+j])>>24;
        }
        std::sort(points.begin(),points.end());Key k{};
        for(unsigned j=0;j<3;++j)std::copy(points[j].begin(),points[j].end(),k.begin()+j*7);
        pairs[k].push_back(t);
    }
    auto area=[&](unsigned t){float q[3][2];for(unsigned j=0;j<3;++j)std::memcpy(q[j],v+ix[t*3+j]*stride,8);
        return (double(q[1][0])-q[0][0])*(double(q[2][1])-q[0][1])-(double(q[1][1])-q[0][1])*(double(q[2][0])-q[0][0]);};
    std::vector<bool> selected(count,false),other(count,false);
    for(const auto& p:pairs)if(p.second.size()==2){unsigned a=p.second[0],b=p.second[1];
        if(!(area(a)*area(b)<0))continue;
        auto brightness=[&](unsigned t){unsigned c=0;for(unsigned j=0;j<3;++j)c+=MenuFaces::light(MenuFaces::color(v,stride,ix[t*3+j]));return c;};
        unsigned chosen=brightness(a)>=brightness(b)?a:b;keep[chosen==a?b:a]=false;++result.removed;
        for(unsigned j=0;j<3;++j)selected[ix[chosen*3+j]]=true;
    }
    if(!result.removed)return result;
    for(unsigned t=0;t<primitives;++t)if(!known[t])for(unsigned j=0;j<3;++j)other[ix[t*3+j]]=true;
    for(unsigned i=0;i<count;++i)if(selected[i]&&!other[i]){
        auto old=MenuFaces::color(v,stride,i),c=old|0xffffffu;if(old==c)continue;
        if(vertices.empty())vertices.assign(v,v+std::size_t(count)*stride);
        std::memcpy(vertices.data()+i*stride+16,&c,4);++result.lit;
    }
    for(unsigned t=0;t<primitives;++t)if(keep[t])output.insert(output.end(),ix.begin()+t*3,ix.begin()+t*3+3);
    return result;
}
}
