// made by LoVeMaKeRz
#include "../src/common/BadgePlane.hpp"
#include "../src/common/Decouple.hpp"
#include <cassert>
#include <cstdio>
using namespace dk2vr;
int main(){
 unsigned checks=0;
 for(float yaw:{-.6f,0.f,.8f})for(float tilt:{-.7f,0.f,.5f})for(float roll:{-.4f,0.f,.4f})for(float side:{-2.f,0.f,2.f})for(float height:{-1.f,0.f,1.f}){
  Quat q=multiply(multiply(yawQuat(yaw),pitch(tilt)),Quat{0,0,std::sin(roll*.5f),std::cos(roll*.5f)});
  SpriteReadingAxes axes{rotate(q,{1,0,0}),rotate(q,{0,0,-1})};Vec3 eye{.3f,.7f,-.2f};Vec3 hUp=cross(axes.right,axes.forward);
  Vec3 pivot=add(eye,add(scale(axes.forward,4),add(scale(axes.right,side),scale(hUp,height))));
  struct V {Vec3 p;unsigned color;float u,v;};
  ReplaySprite r;r.sharedPlane=true;r.sharedPivot=pivot;r.up={0,1,0};r.sharedRight={1,0,0};r.sharedDown={0,0,1};
  // A glyph's vertical centreline and circle points, originally on one table plane.
  r.corners={{0,.5f,0,add(pivot,{0,0,-.1f})},{1,.5f,1,add(pivot,{0,0,.1f})},{2,1,.5f,add(pivot,{.1f,0,0})}};
  std::vector<unsigned char> bytes(3*sizeof(V));for(unsigned i=0;i<3;++i){V v{r.corners[i].original,0xffaabbcc,r.corners[i].u,r.corners[i].v};memcpy(bytes.data()+i*sizeof(V),&v,sizeof(v));}
  assert(r.rebuild(bytes,0,sizeof(V),3,eye,&axes));V v[3];memcpy(v,bytes.data(),sizeof(v));
  auto projectX=[&](Vec3 p){Vec3 d=sub(p,eye);return dot(d,axes.right)/dot(d,axes.forward);};
  auto projectY=[&](Vec3 p){Vec3 d=sub(p,eye);return dot(d,hUp)/dot(d,axes.forward);};
  assert(std::abs(projectX(v[0].p)-projectX(v[1].p))<1e-5f);assert(projectY(v[0].p)>projectY(v[1].p));
  Vec3 normal=sub(eye,pivot);normal=scale(normal,1/std::sqrt(dot(normal,normal)));
  for(unsigned i=0;i<3;++i){Vec3 d=sub(v[i].p,pivot);assert(std::abs(dot(d,normal))<1e-5f);assert(std::abs(std::sqrt(dot(d,d))-.1f)<1e-5f);assert(v[i].color==0xffaabbcc&&v[i].u==r.corners[i].u&&v[i].v==r.corners[i].v);}
  checks+=11;
 }
 printf("PASS BadgeReadingTests: %u checks, perspective glyph stays screen-upright left/centre/right, HMD pitch/yaw/roll, plane facing/size/UV/color preserved.\n",checks);
}
