#pragma once
// made by LoVeMaKeRz
#include <vector>
#include <cstdint>
#include <algorithm>
#include <cstdlib>
#include <cmath>
namespace dk2vr {
// Remove only pixels verified against the captured native cursor; never erase a bounding box.
inline bool removeMenuCursor(std::vector<uint32_t>& ui,const std::vector<uint32_t>& final,int W,int H,
 const std::vector<uint32_t>& hand,int w,int h,int left,int top) {
 if(W<=0||H<=0||w<=0||h<=0||ui.size()!=size_t(W)*H||final.size()!=ui.size()||hand.size()!=size_t(w)*h)return false;
 std::vector<size_t> matches;size_t visible=0;
 for(int y=0;y<h;++y)for(int x=0;x<w;++x){uint32_t c=hand[size_t(y)*w+x];int xx=left+x,yy=top+y;
  if(!(c>>24)||xx<0||yy<0||xx>=W||yy>=H)continue;
  ++visible;size_t k=size_t(yy)*W+xx;uint32_t p=final[k];bool same=true;
  for(int shift:{0,8,16})if(std::abs(int((p>>shift)&255)-int((c>>shift)&255))>16)same=false;
  if(same)matches.push_back(k);
 }
 if(matches.size()<16||matches.size()*10<visible*7)return false;
 for(size_t k:matches)ui[k]=0;
 return true;
}
// 0.9.30: the menu image (640x480) is smaller than the game window (1024x768): the game draws its hand at window size and the image is
// reduced by 'scale' (< 1), so the hand in the image is a reduced copy and the 1:1 comparison above never matched (0 of 196 frames, report 202644):
// the game's own hand stayed in the interface layer as a translucent ghost next to the VR hand. The hand is reduced the same way (area average),
// aligned within +-2 pixels, and only pixels that really look like the hand are taken out of the interface layer (never a bounding box).
struct ScaledHand {
 std::vector<uint32_t> px;int w=0,h=0;float scale=0;uint32_t serial=0;const void* source=nullptr;size_t sourceSize=0;
 bool build(const std::vector<uint32_t>& hand,int hw,int hh,float k) {
  if(!(k>0.05f&&k<=1.0f)||hw<=0||hh<=0||hand.size()!=size_t(hw)*hh)return false;
  const int nw=std::max(1,int(std::ceil(hw*k))),nh=std::max(1,int(std::ceil(hh*k)));
  px.assign(size_t(nw)*nh,0);
  for(int y=0;y<nh;++y)for(int x=0;x<nw;++x){
   const float x0=x/k,x1=std::min(float(hw),(x+1)/k),y0=y/k,y1=std::min(float(hh),(y+1)/k);
   double a=0,r=0,g=0,b=0,area=0;
   for(int sy=int(y0);sy<int(std::ceil(y1))&&sy<hh;++sy)for(int sx=int(x0);sx<int(std::ceil(x1))&&sx<hw;++sx){
    const float wx=std::min(float(sx+1),x1)-std::max(float(sx),x0),wy=std::min(float(sy+1),y1)-std::max(float(sy),y0);if(wx<=0||wy<=0)continue;
    const double wgt=double(wx)*wy;const uint32_t c=hand[size_t(sy)*hw+sx];const double ca=(c>>24)&255;
    area+=wgt;a+=wgt*ca;r+=wgt*ca*((c>>16)&255);g+=wgt*ca*((c>>8)&255);b+=wgt*ca*(c&255);}
   if(area<=0||a<=0)continue;
   const double alpha=a/area;r/=a;g/=a;b/=a;
   px[size_t(y)*nw+x]=(uint32_t(std::lround(alpha))<<24)|(uint32_t(std::lround(r))<<16)|(uint32_t(std::lround(g))<<8)|uint32_t(std::lround(b));}
  w=nw;h=nh;scale=k;return true;
 }
};
inline bool removeScaledMenuCursor(std::vector<uint32_t>& ui,const std::vector<uint32_t>& final,int W,int H,const ScaledHand& hand,int left,int top) {
 if(W<=0||H<=0||hand.w<=0||hand.h<=0||ui.size()!=size_t(W)*H||final.size()!=ui.size())return false;
 auto count=[&](int dx,int dy,size_t& visible)->size_t{
  size_t good=0;visible=0;
  for(int y=0;y<hand.h;++y)for(int x=0;x<hand.w;++x){const uint32_t c=hand.px[size_t(y)*hand.w+x];if((c>>24)<200)continue;
   const int xx=left+dx+x,yy=top+dy+y;if(xx<0||yy<0||xx>=W||yy>=H)continue;++visible;
   const uint32_t p=final[size_t(yy)*W+xx];bool same=true;for(int shift:{0,8,16})if(std::abs(int((p>>shift)&255)-int((c>>shift)&255))>40)same=false;if(same)++good;}
  return good;};
 int bestDx=0,bestDy=0;size_t best=0,bestVisible=0;
 for(int dy=-2;dy<=2;++dy)for(int dx=-2;dx<=2;++dx){size_t vis=0;const size_t g=count(dx,dy,vis);if(g>best){best=g;bestDx=dx;bestDy=dy;bestVisible=vis;}}
 if(best<12||best*10<bestVisible*6)return false;
 for(int y=0;y<hand.h;++y)for(int x=0;x<hand.w;++x){const uint32_t c=hand.px[size_t(y)*hand.w+x];const unsigned a=c>>24;if(a<24)continue;
  const int xx=left+bestDx+x,yy=top+bestDy+y;if(xx<0||yy<0||xx>=W||yy>=H)continue;
  const size_t k=size_t(yy)*W+xx;
  if(a<200) {ui[k]=0;continue;}   // blended edge of the reduced hand: belongs to the hand
  const uint32_t p=final[k];bool same=true;for(int shift:{0,8,16})if(std::abs(int((p>>shift)&255)-int((c>>shift)&255))>40)same=false;
  if(same)ui[k]=0;}
 return true;
}

// 0.9.31/0.9.32: the menu hand is drawn by the game at ITS OWN cursor position, one or more game frames before the live VR ray position, so a fast ray movement
// leaves it several pixels away from the reported position (report 213437: only 22-45 % of the frames matched with a +-6 px window). Every recent distinct hand image
// is tried at several reductions in a wide window; the score is the shape/colour agreement with the final image (loose colour tolerance, the hand is lit and blended);
// the winner (image, reduction, offset to the reported position) is remembered and re-checked first. The hand pixels are then taken out of the interface layer.
struct HandCand { ScaledHand hand; float hotX=0,hotY=0; int image=0; bool keyed=false; };   // keyed: the image has a real transparent background
struct HandLock { int cand=-1; int dx=0,dy=0; unsigned fails=0; unsigned weak=0; };   // weak: frames locked with a poor agreement (0.9.47.26)
struct HandStats { size_t bestGood=0,bestVisible=0; int bestCand=-1; int bestDx=0,bestDy=0; unsigned lockedFrames=0,searchedFrames=0,skippedFrames=0; };
inline size_t scoreScaledHand(const std::vector<uint32_t>& ui,const std::vector<uint32_t>& final,int W,int H,const ScaledHand& hand,int left,int top,size_t& visible,int step=2,uint64_t* error=nullptr) {
 size_t good=0;visible=0;if(error)*error=0;
 for(int y=0;y<hand.h;y+=step)for(int x=0;x<hand.w;x+=step){const uint32_t c=hand.px[size_t(y)*hand.w+x];if((c>>24)<200)continue;
  const int xx=left+x,yy=top+y;if(xx<0||yy<0||xx>=W||yy>=H)continue;++visible;
  const size_t k=size_t(yy)*W+xx;if(!ui[k])continue;   // the hand is part of the interface layer
  const uint32_t p=final[k];bool same=true;int e=0;for(int shift:{0,8,16}){const int d=std::abs(int((p>>shift)&255)-int((c>>shift)&255));e+=d;if(d>72)same=false;}if(same){++good;if(error)*error+=uint64_t(e);}}
 return good;
}
// clears the pixels of the hand (no re-verification: the position was found by the score)
inline void clearScaledHand(std::vector<uint32_t>& ui,int W,int H,const ScaledHand& hand,int left,int top) {
 for(int y=0;y<hand.h;++y)for(int x=0;x<hand.w;++x){const uint32_t c=hand.px[size_t(y)*hand.w+x];if((c>>24)<96)continue;
  const int xx=left+x,yy=top+y;if(xx<0||yy<0||xx>=W||yy>=H)continue;ui[size_t(yy)*W+xx]=0;}
}
// 0.9.34: the hand of the game is drawn OPAQUE in the final image, but the 2D/decor separation gives it a low opacity wherever its colour is close to the decor
// behind it: in the headset it looked transparent (report 232249: found at the exact cursor position, scale 1, but only 47 % colour agreement). Instead of
// removing it and drawing another hand at the VR ray (which lagged), its pixels are made fully opaque with the colour of the final image: the game's own
// hand, at its own place, solid. Wrong placement can only show the flat game image on a hand-shaped area.
// 0.9.35: the menu cursor image may come without colour key (all pixels opaque): the opaque background then made a SQUARE around the hand (report 000353).
// When no pixel is transparent and the four corners share one colour, that colour is the key.
inline std::vector<uint32_t> keyedHand(const std::vector<uint32_t>& px,int w,int h) {
 std::vector<uint32_t> out=px;if(w<2||h<2||px.size()!=size_t(w)*h)return out;
 for(uint32_t c:px)if((c>>24)==0)return out;   // already keyed
 const uint32_t k=px[0]&0xffffffu;auto nearKey=[&](uint32_t c){for(int sh:{0,8,16})if(std::abs(int((c>>sh)&255)-int((k>>sh)&255))>6)return false;return true;};
 if(!nearKey(px[size_t(w)-1])||!nearKey(px[size_t(h-1)*w])||!nearKey(px[size_t(h)*w-1]))return out;
 for(auto& c:out)if(nearKey(c))c=0;
 return out;
}
// decor (optional, same size): only pixels where the final image differs from the decor are the hand; the others keep the interface layer as it is
inline void solidifyScaledHand(std::vector<uint32_t>& ui,const std::vector<uint32_t>& final,int W,int H,const ScaledHand& hand,int left,int top,const std::vector<uint32_t>* decor=nullptr,bool edgesOnly=false) {
 if(decor&&decor->size()!=final.size())decor=nullptr;
 auto solidAt=[&](int x,int y){return x>=0&&y>=0&&x<hand.w&&y<hand.h&&(hand.px[size_t(y)*hand.w+x]>>24)>=128;};
 for(int y=0;y<hand.h;++y)for(int x=0;x<hand.w;++x){const uint32_t c=hand.px[size_t(y)*hand.w+x];if((c>>24)<128)continue;
  const int xx=left+x,yy=top+y;if(xx<0||yy<0||xx>=W||yy>=H)continue;const size_t k=size_t(yy)*W+xx;
  if(decor&&(!edgesOnly||!(solidAt(x-1,y)&&solidAt(x+1,y)&&solidAt(x,y-1)&&solidAt(x,y+1)))){const uint32_t f=final[k],dd=(*decor)[k];int m=0;for(int sh:{0,8,16})m=std::max(m,std::abs(int((f>>sh)&255)-int((dd>>sh)&255)));if(m<=20)continue;}
  ui[k]=0xff000000u|(final[k]&0xffffffu);}
}
// 0.9.46 (report 103623: 'halo' around the menu hand while moving): the edge pixels of the hand are anti-aliased, their colour in the final image is half decor.
// Made opaque they leave a ring of decor-coloured pixels that does not line up with the decor drawn in 3D (relief). Edge pixels are made opaque only where they
// really differ from the decor; and the pixels around the hand that are not the hand and look like the decor are made transparent again.
inline void haloCleanScaledHand(std::vector<uint32_t>& ui,const std::vector<uint32_t>& final,int W,int H,const ScaledHand& hand,int left,int top,const std::vector<uint32_t>* decor,int margin=10) {
 if(!decor||decor->size()!=final.size()||hand.w<=0||hand.h<=0)return;
 // 0.9.47.51 (report 220959: irregular light patches around the hand when it sits on a translucent game panel, e.g. 'Instructions mission'): the panel is a SOFT
 // layer (small alpha); where it darkens the decor by less than the tolerance below the cleaning below erased it, leaving brighter blotches. The ring is only
 // cleaned when the surroundings (a band just outside the cleaned zone) hold no interface: on bare decor, as before.
 {uint64_t sum=0,cnt=0;const int in=margin+1,out=margin+7;
  for(int y=-out;y<hand.h+out;++y)for(int x=-out;x<hand.w+out;++x){
   if(x>=-in&&x<hand.w+in&&y>=-in&&y<hand.h+in)continue;
   const int xx=left+x,yy=top+y;if(xx<0||yy<0||xx>=W||yy>=H)continue;
   sum+=ui[size_t(yy)*W+xx]>>24;++cnt;}
  if(cnt&&sum>cnt*6)return;}   // mean alpha of the band > 6/255: an interface panel is there, nothing is cleaned
 auto solid=[&](int x,int y){return x>=0&&y>=0&&x<hand.w&&y<hand.h&&(hand.px[size_t(y)*hand.w+x]>>24)>=128;};
 for(int y=-margin;y<hand.h+margin;++y)for(int x=-margin;x<hand.w+margin;++x){
  const int xx=left+x,yy=top+y;if(xx<0||yy<0||xx>=W||yy>=H)continue;
  if(solid(x,y)||solid(x-1,y)||solid(x+1,y)||solid(x,y-1)||solid(x,y+1))continue;   // the hand and one pixel around it stay as they are
  const size_t k=size_t(yy)*W+xx;if((ui[k]>>24)==0)continue;
  const uint32_t f=final[k],dd=(*decor)[k];int m=0;for(int sh:{0,8,16})m=std::max(m,std::abs(int((f>>sh)&255)-int((dd>>sh)&255)));
  if(m<=24)ui[k]=0;}
}
inline bool matchMenuCursor(std::vector<uint32_t>& ui,const std::vector<uint32_t>& final,int W,int H,const std::vector<HandCand>& cands,float cx,float cy,HandLock& lock,HandStats& st,bool solid=false,const std::vector<uint32_t>* decor=nullptr,bool haloClean=false) {
 if(W<=0||H<=0||ui.size()!=size_t(W)*H||final.size()!=ui.size()||cands.empty())return false;
 auto pass=[&](size_t good,size_t vis){return solid?(good>=20&&good*100>=vis*25):(good>=8&&good*100>=vis*50);};
 // 0.9.36: with a true hand shape (keyed image) EVERY hand pixel is made opaque (0.9.35 skipped those close to the decor colour: they stayed see-through, report 004324);
 // the decor test only protects an image without transparent background
 auto apply=[&](const HandCand& hc,int l,int t){if(solid){solidifyScaledHand(ui,final,W,H,hc.hand,l,t,(hc.keyed&&!haloClean)?nullptr:decor,hc.keyed&&haloClean);if(haloClean)haloCleanScaledHand(ui,final,W,H,hc.hand,l,t,decor);}else clearScaledHand(ui,W,H,hc.hand,l,t);};
 auto leftOf=[&](const HandCand& c,int dx){return int(std::lround(cx-c.hotX))+dx;};
 auto topOf=[&](const HandCand& c,int dy){return int(std::lround(cy-c.hotY))+dy;};
 auto refine=[&](const HandCand& c,int cdx,int cdy,int radius,size_t& bg,size_t& bv,int& bdx,int& bdy){
  bg=0;bv=0;bdx=cdx;bdy=cdy;
  uint64_t be=~0ull;
  for(int dy=cdy-radius;dy<=cdy+radius;++dy)for(int dx=cdx-radius;dx<=cdx+radius;++dx){size_t v=0;uint64_t e=0;const size_t g=scoreScaledHand(ui,final,W,H,c.hand,leftOf(c,dx),topOf(c,dy),v,1,&e);
   if(g>bg||(g==bg&&g>0&&e<be)){bg=g;bv=v;be=e;bdx=dx;bdy=dy;}}};   // equal agreement: the smallest colour error wins (exact alignment)
 bool weakOk=false;int weakDx=0,weakDy=0,weakCand=-1;
 if(lock.cand>=0&&(std::abs(lock.dx)>12||std::abs(lock.dy)>12))lock.cand=-1;   // 0.9.35: drifted lock (offsets +24,+21 seen) -> new search
 if(lock.cand>=0&&lock.cand<int(cands.size())) {
  const HandCand& c=cands[lock.cand];size_t bg,bv;int bdx,bdy;refine(c,lock.dx,lock.dy,4,bg,bv,bdx,bdy);
  // 0.9.47.26 (report 090200: after a game is loaded the game has 8 hand images instead of 1 and the lock could sit on a big in-game hand with 25 % agreement:
  // a hand-shaped white halo around the menu cursor). A lock whose agreement is poor (< 60 %) is re-searched every 8th frame and left for a better candidate.
  const bool strong=pass(bg,bv)&&bv>0&&bg*100>=bv*60;
  if(strong||(pass(bg,bv)&&(++lock.weak&7)!=0)) {apply(c,leftOf(c,bdx),topOf(c,bdy));lock.dx=bdx;lock.dy=bdy;lock.fails=0;if(strong)lock.weak=0;++st.lockedFrames;return true;}
  if(pass(bg,bv)) {weakOk=true;weakDx=bdx;weakDy=bdy;weakCand=lock.cand;} else ++lock.fails;
 }
 // full search only every 4th failed frame (the hand may be hidden): bounded cost
 if(lock.fails>8&&(lock.fails&3)!=0){++lock.fails;++st.skippedFrames;return false;}   // 0.9.35: full search on every one of the first failed frames (fast movement)
 ++st.searchedFrames;
 size_t bg=0,bv=0;int bc=-1,bdx=0,bdy=0;
 for(int i=0;i<int(cands.size());++i){const HandCand& c=cands[i];if(c.hand.w<=0)continue;
  size_t cg=0,cv=0;int cdx=0,cdy=0;
  for(int dy=-24;dy<=24;dy+=4)for(int dx=-24;dx<=24;dx+=4){size_t v=0;const size_t g=scoreScaledHand(ui,final,W,H,c.hand,leftOf(c,dx),topOf(c,dy),v,3);if(g>cg){cg=g;cv=v;cdx=dx;cdy=dy;}}
  if(cg>st.bestGood){st.bestGood=cg;st.bestVisible=cv;st.bestCand=i;st.bestDx=cdx;st.bestDy=cdy;}
  if(cv==0||cg*100<cv*10)continue;   // 0.9.47.26: judged by the SHARE of the hand that agrees, not by the pixel count (a big wrong hand always had more)
  size_t rg,rv;int rdx,rdy;refine(c,cdx,cdy,3,rg,rv,rdx,rdy);
  if(rv>0&&pass(rg,rv)&&(bc<0||double(rg)*bv>double(bg)*rv||(double(rg)*bv==double(bg)*rv&&rg>bg))){bg=rg;bv=rv;bc=i;bdx=rdx;bdy=rdy;}}
 if(bc<0){
  if(weakOk) {const HandCand& w=cands[weakCand];apply(w,leftOf(w,weakDx),topOf(w,weakDy));lock.dx=weakDx;lock.dy=weakDy;lock.fails=0;return true;}   // nothing better than the weak lock: keep it
  ++lock.fails;return false;}
 lock.weak=0;
 const HandCand& c=cands[bc];apply(c,leftOf(c,bdx),topOf(c,bdy));
 lock.cand=bc;lock.dx=bdx;lock.dy=bdy;lock.fails=0;return true;
}
}
