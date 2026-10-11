// made by LoVeMaKeRz
#include "../src/common/Decouple.hpp"
#include <vector>
#include <cassert>
#include <cstdio>
using namespace dk2vr;
using DWORD=unsigned;
#include "../out/PedestalExtract.inl"
int main(){std::vector<PedVertex> v;Pose p{{0,0,0,1},{0,0,0}};texturedBox(v,p,{-.2f,-.3f,-.04f},{.2f,.3f,0},0xffffffff,0xffb8b8b8);assert(v.size()==324);int dark=0,light=0;
for(auto q:v){assert(q.p.x>=-.200001f&&q.p.x<=.200001f&&q.p.y>=-.300001f&&q.p.y<=.300001f&&q.p.z>=-.040001f&&q.p.z<=.000001f);assert(std::abs(dot(q.n,q.n)-1)<1e-5f);assert(q.u>=0&&q.v>=0);if((q.c&255)<100)++dark;else++light;}
assert(dark&&light);puts("PASS PedestalEdgeTests: production mesh, bounds/normals/UVs retained, darker actual object edges, central stone retained.");}
