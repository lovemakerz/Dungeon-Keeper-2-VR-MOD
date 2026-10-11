// made by LoVeMaKeRz
#include "../src/common/BadgePlane.hpp"
#include <cassert>
#include <cstdio>
using namespace dk2vr;
static float dot(Vec3 a,Vec3 b){return a.x*b.x+a.y*b.y+a.z*b.z;}
int main(){
 struct V {Vec3 p;unsigned color;float u,v;};
 ReplaySprite digit,ring,floor;digit.up=ring.up=floor.up={0,1,0};
 digit.hasAxes=true;digit.originalCenter={0,1,0};digit.sourceRight={0,0,1};digit.sourceDown={1,0,0};digit.cx=ring.cx=floor.cx=100;digit.cy=ring.cy=floor.cy=100;
 digit.width=digit.height=20;ring.width=ring.height=floor.width=floor.height=40;digit.rhw=ring.rhw=.2513f;floor.rhw=.2481f;
 for(unsigned i=0;i<4;++i)digit.corners.push_back({i,float(i&1),float(i>>1),{(i>>1)? .1f:-.1f,1,(i&1)? .1f:-.1f}});
 for(unsigned i=0;i<8;++i){float a=float(i)*Pi*.25f;ring.corners.push_back({i,0,0,{.2f*std::sin(a),1,.2f*std::cos(a)}});}
 assert(joinBadgePlanes({&digit,&ring,&floor})==2);assert(!floor.sharedPlane);assert(ring.sharedPlane);
 for(Vec3 eye:std::vector<Vec3>{{0,4,2},{-2,2,1},{3,-1,-2}}){
  std::vector<unsigned char> d(4*sizeof(V)),r(8*sizeof(V));
  for(unsigned i=0;i<4;++i){V v{digit.corners[i].original,0xffaabbcc,float(i&1),float(i>>1)};memcpy(d.data()+i*sizeof(V),&v,sizeof(v));}
  for(unsigned i=0;i<8;++i){V v{ring.corners[i].original,0xff112233,0,0};memcpy(r.data()+i*sizeof(V),&v,sizeof(v));}
  assert(digit.rebuild(d,0,sizeof(V),4,eye));assert(ring.rebuild(r,0,sizeof(V),8,eye));
  Vec3 normal=sub(eye,digit.sharedPivot);normal=scale(normal,1/std::sqrt(dot(normal,normal)));
  for(unsigned i=0;i<8;++i){V v;memcpy(&v,r.data()+i*sizeof(V),sizeof(v));Vec3 delta=sub(v.p,digit.sharedPivot);assert(std::abs(dot(delta,normal))<1e-5f);assert(std::abs(std::sqrt(dot(delta,delta))-.2f)<1e-5f);assert(v.color==0xff112233&&v.u==0&&v.v==0);}
  for(unsigned i=0;i<4;++i){V v;memcpy(&v,d.data()+i*sizeof(V),sizeof(v));assert(std::abs(dot(sub(v.p,digit.sharedPivot),normal))<1e-5f);assert(v.color==0xffaabbcc&&v.u==float(i&1)&&v.v==float(i>>1));}
 }
 puts("PASS BadgePlaneTests: common glyph/ring plane at all HMD elevations; original size/shape/color/UV retained; degenerate-UV ring accepted; real report floor depth .2481 excluded from badge .2513.");
}
