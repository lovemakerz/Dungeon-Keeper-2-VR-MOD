// made by LoVeMaKeRz
#include "../src/common/BadgePlane.hpp"
#include "../src/common/Decouple.hpp"
#include <cassert>
#include <cstdio>
using namespace dk2vr;
int main(){unsigned checks=0;
 for(float side:{-2.f,0.f,2.f})for(float pitchAngle:{-.6f,0.f,.6f})for(float separation:{.0001f,.001f,.004f}){
  SpriteReadingAxes axes{rotate(pitch(pitchAngle),{1,0,0}),rotate(pitch(pitchAngle),{0,0,-1})};Vec3 up=cross(axes.right,axes.forward),eye{};
  Vec3 pivot=add(scale(axes.forward,4),scale(axes.right,side));
  ReplaySprite glyph,disc;for(auto* s:{&glyph,&disc}){s->sharedPlane=true;s->sharedPivot=pivot;s->up={0,1,0};s->sharedRight={1,0,0};s->sharedDown={0,0,1};}
  struct V{Vec3 p;unsigned color;float u,v;};
  for(unsigned i=0;i<4;++i){Vec3 p=add(pivot,{(i&1)?.1f:-.1f,0,(i>>1)?.1f:-.1f});disc.corners.push_back({i,float(i&1),float(i>>1),p});glyph.corners.push_back({i,float(i&1),float(i>>1),add(p,{0,separation,0})});}
  std::vector<unsigned char> a(4*sizeof(V)),b(4*sizeof(V));
  for(unsigned i=0;i<4;++i){V v{disc.corners[i].original,0xff223344,disc.corners[i].u,disc.corners[i].v};memcpy(a.data()+i*sizeof(V),&v,sizeof(v));v.p=glyph.corners[i].original;memcpy(b.data()+i*sizeof(V),&v,sizeof(v));}
  assert(disc.rebuild(a,0,sizeof(V),4,eye,&axes));assert(glyph.rebuild(b,0,sizeof(V),4,eye,&axes));
  Vec3 n=sub(eye,pivot);n=scale(n,1/std::sqrt(dot(n,n)));
  for(unsigned i=0;i<4;++i){V d,g;memcpy(&d,a.data()+i*sizeof(V),sizeof(d));memcpy(&g,b.data()+i*sizeof(V),sizeof(g));Vec3 gap=sub(g.p,d.p);assert(std::abs(dot(gap,n)-separation)<1e-6f);assert(dot(gap,n)>0);assert(std::abs(dot(gap,cross(up,n)))<1e-6f);assert(g.color==d.color&&g.u==d.u&&g.v==d.v);checks+=4;}
  // Old flattening loses this gap entirely, allowing a later disc to cover
  // the glyph; actual production transform must retain it for all corners.
 }
 printf("PASS BadgeDepthTests: %u checks; native glyph/disc separation retained at every corner, left/centre/right and HMD pitch; UV/color preserved.\n",checks);
}
