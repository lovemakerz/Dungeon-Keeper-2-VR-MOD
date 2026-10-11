// made by LoVeMaKeRz
#include "../src/common/MenuCursor.hpp"
#include <cassert>
#include <cstdio>
using namespace dk2vr;
int main(){
 const int W=20,H=15,w=6,h=5;std::vector<uint32_t> hand(w*h,0xffc08040),final(W*H,0xff102030),ui(W*H,0x80102030);
 hand[0]=0;for(int y=0;y<h;++y)for(int x=0;x<w;++x)if(hand[y*w+x])final[(y+4)*W+x+7]=hand[y*w+x];
 auto old=ui;assert(removeMenuCursor(ui,final,W,H,hand,w,h,7,4));
 for(int y=0;y<H;++y)for(int x=0;x<W;++x){bool erased=x>=7&&x<13&&y>=4&&y<9&&hand[(y-4)*w+x-7];assert(ui[y*W+x]==(erased?0:old[y*W+x]));}
 ui=old;assert(!removeMenuCursor(ui,final,W,H,hand,w,h,0,0));assert(ui==old);
 assert(!removeMenuCursor(ui,final,W,H,hand,w,h,100,-100));assert(ui==old);
 for(int y=0;y<h;++y)for(int x=0;x<w;++x)if(hand[y*w+x])final[(y+4)*W+x+7]=0xffb87838;
 assert(removeMenuCursor(ui,final,W,H,hand,w,h,7,4));
 // 0.9.30: hand drawn at window size, menu image reduced by 0.625 (1024x768 -> 640x480): reduced copy must be found and removed
 {
  const int W2=80,H2=60,hw=48,hh=48;const float k=.625f;
  std::vector<uint32_t> big(hw*hh,0xff785030);for(int i=0;i<hw;++i)big[i]=0;   // transparent first row
  ScaledHand sh;assert(sh.build(big,hw,hh,k)&&sh.w==30&&sh.h==30);
  std::vector<uint32_t> fin(W2*H2,0xff102030),u2(W2*H2,0x80102030);
  for(int y=0;y<sh.h;++y)for(int x=0;x<sh.w;++x)if(sh.px[y*sh.w+x]>>24)fin[(y+10)*W2+x+20+1]=sh.px[y*sh.w+x]|0xff000000;   // drawn 1 px off the expected place
  auto before=u2;assert(removeScaledMenuCursor(u2,fin,W2,H2,sh,20,10));
  size_t cleared=0;for(size_t i=0;i<u2.size();++i){if(u2[i]==0)++cleared;else assert(u2[i]==before[i]);}
  assert(cleared>=size_t(sh.w)*(sh.h-1)*8/10);
  u2=before;std::vector<uint32_t> empty(W2*H2,0xff102030);assert(!removeScaledMenuCursor(u2,empty,W2,H2,sh,20,10));assert(u2==before);   // nothing there: nothing erased
  assert(!sh.build(big,hw,hh,1.5f));
 }
 // 0.9.31: several hand images, unknown scale, position off by a few pixels: the right image/scale is found, remembered, and the wrong ones are not
 {
  const int W3=120,H3=90;auto mk=[&](int w,int h,uint32_t base){std::vector<uint32_t> v(w*h);for(int y=0;y<h;++y)for(int x=0;x<w;++x)v[y*w+x]=0xff000000u|((base+uint32_t(x*5+y*3)*0x010203u)&0xffffffu);return v;};
  auto a=mk(60,60,0x203040),b=mk(64,40,0x805020);
  std::vector<HandCand> cands;
  for(int img=0;img<2;++img)for(float sc:{.625f,.5f}){HandCand c;const auto& px=img?b:a;if(!c.hand.build(px,img?64:60,img?40:60,sc))continue;c.hotX=3*sc;c.hotY=6*sc;c.image=img;cands.push_back(std::move(c));}
  assert(cands.size()==4);
  const HandCand& truth=cands[3];   // image b at 0.5
  std::vector<uint32_t> fin(W3*H3,0xff102030),ui(W3*H3,0x80102030);
  const int left=50,top=30;for(int y=0;y<truth.hand.h;++y)for(int x=0;x<truth.hand.w;++x)fin[(top+y)*W3+left+x]=truth.hand.px[y*truth.hand.w+x]|0xff000000u;
  HandLock lock;HandStats st;const float cx=left+truth.hotX-4,cy=top+truth.hotY+3;   // reported position is 4 px / 3 px off
  assert(matchMenuCursor(ui,fin,W3,H3,cands,cx,cy,lock,st));assert(lock.cand==3);
  size_t cleared=0;for(auto q:ui)if(q==0)++cleared;assert(cleared>=size_t(truth.hand.w)*truth.hand.h*8/10);
  ui.assign(W3*H3,0x80102030);assert(matchMenuCursor(ui,fin,W3,H3,cands,cx,cy,lock,st));assert(lock.cand==3);   // second frame uses the lock
  {HandLock l3;HandStats s3;ui.assign(W3*H3,0x80102030);assert(matchMenuCursor(ui,fin,W3,H3,cands,truth.hotX+left+22,truth.hotY+top-18,l3,s3));assert(l3.cand==3);}   // hand 22 px / 18 px behind the reported position (lag)
  std::vector<uint32_t> none(W3*H3,0xff102030);ui.assign(W3*H3,0x80102030);auto keep=ui;HandLock l2;HandStats s2;
  assert(!matchMenuCursor(ui,none,W3,H3,cands,cx,cy,l2,s2));assert(ui==keep);
 }
 // 0.9.34: solid mode: the hand pixels of the interface layer become opaque copies of the final image, everything else unchanged
 {
  const int W4=60,H4=40;std::vector<uint32_t> big(20*20);for(int i=0;i<400;++i)big[i]=0xff000000u|uint32_t(0x304050+i*0x010101);
  std::vector<HandCand> cands;HandCand c;assert(c.hand.build(big,20,20,1.f));c.hotX=2;c.hotY=3;cands.push_back(c);
  std::vector<uint32_t> fin(W4*H4,0xff102030),ui(W4*H4,0);
  for(int y=0;y<20;++y)for(int x=0;x<20;++x){fin[(y+10)*W4+x+15]=big[y*20+x];ui[(y+10)*W4+x+15]=(x/5+y/5)%2?0x40101010u:0;}   // half the hand lost by the separation
  auto before=ui;HandLock l;HandStats st;{bool ok=matchMenuCursor(ui,fin,W4,H4,cands,17,13,l,st,true);if(!ok||l.dx||l.dy)fprintf(stderr,"solid ok %d dx %d dy %d best %zu/%zu\n",ok,l.dx,l.dy,st.bestGood,st.bestVisible);assert(ok);}
  for(int y=0;y<H4;++y)for(int x=0;x<W4;++x){const size_t k=size_t(y)*W4+x;const bool in=x>=15&&x<35&&y>=10&&y<30;if(!(in?ui[k]==(0xff000000u|(fin[k]&0xffffff)):ui[k]==before[k]))fprintf(stderr,"px %d %d in %d ui %08x fin %08x before %08x\n",x,y,in,ui[k],fin[k],before[k]);assert(in?ui[k]==(0xff000000u|(fin[k]&0xffffff)):ui[k]==before[k]);}
 }
 // 0.9.35: cursor image without colour key (green background, all opaque): the corners give the key; solid mode keeps decor-coloured pixels untouched
 {
  std::vector<uint32_t> img(10*8,0xff00ff00u);for(int y=2;y<6;++y)for(int x=3;x<7;++x)img[y*10+x]=0xffc0a080u;
  auto k=keyedHand(img,10,8);size_t opaque=0;for(auto c:k)if(c>>24)++opaque;assert(opaque==16);
  std::vector<uint32_t> already=img;already[0]=0;assert(keyedHand(already,10,8)==already);
  ScaledHand h;assert(h.build(img,10,8,1.f));   // unkeyed: square
  std::vector<uint32_t> fin(20*20,0xff304050),dec(20*20,0xff304050),ui(20*20,0);
  for(int y=2;y<6;++y)for(int x=3;x<7;++x)fin[(y+5)*20+x+5]=0xffc0a080u;
  solidifyScaledHand(ui,fin,20,20,h,5,5,&dec);size_t made=0;for(auto c:ui)if(c)++made;assert(made==16);   // the square background is NOT painted
 }
 // 0.9.47.26 (report 090200): after a game was loaded the game has 8 hand images; a BIG in-game hand that agrees on only ~35 % of its (many) pixels used to beat the
 // real menu hand (high share, fewer pixels) and drew a hand-shaped halo. The share of agreeing pixels decides, in the search and for a weak lock.
 {
  const int W5=160,H5=120;
  std::vector<uint32_t> small(12*12),big(30*30);
  for(int y=0;y<12;++y)for(int x=0;x<12;++x)small[y*12+x]=0xff000000u|(0x304050u+uint32_t(x*9+y*5)*0x010101u);
  for(int y=0;y<30;++y)for(int x=0;x<30;++x)big[y*30+x]=((x+y)%3==0)?0xff102030u:0xffe0e0e0u;
  std::vector<HandCand> cands;
  {HandCand c;c.hand.build(big,30,30,1.f);c.hotX=2;c.hotY=2;c.image=0;cands.push_back(std::move(c));}
  {HandCand c;c.hand.build(small,12,12,1.f);c.hotX=2;c.hotY=2;c.image=1;cands.push_back(std::move(c));}
  std::vector<uint32_t> fin(W5*H5,0xff102030),ui(W5*H5,0x80102030);
  const int left=60,top=40;for(int y=0;y<12;++y)for(int x=0;x<12;++x)fin[(top+y)*W5+left+x]=small[y*12+x];
  HandLock lock;HandStats st;
  const bool ok=matchMenuCursor(ui,fin,W5,H5,cands,left+2,top+2,lock,st,true);
  if(!ok||lock.cand!=1)fprintf(stderr,"big wrong hand: ok %d cand %d\n",ok,lock.cand);
  assert(ok&&lock.cand==1);
  HandLock bad;bad.cand=0;bad.dx=0;bad.dy=0;HandStats st2;bool switched=false;
  for(int f=0;f<10&&!switched;++f){ui.assign(W5*H5,0x80102030);matchMenuCursor(ui,fin,W5,H5,cands,left+2,top+2,bad,st2,true);switched=bad.cand==1;}
  assert(switched);
 }
 puts("PASS MenuCursorTests: matched cursor only, transparent holes/UI preserved, wrong position rejected, 16-bit color tolerance, share-of-agreement choice among 8 hand images. No game/headset validation.");
}
