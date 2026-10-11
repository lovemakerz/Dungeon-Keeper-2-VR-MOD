#pragma once
// made by LoVeMaKeRz
#include "UprightSprite.hpp"
#include <numeric>
namespace dk2vr {
// Metadata only comes from ABOVE-floor candidates. Never extend selection
// to floor-height pieces, even if they overlap the badge in screen space.
inline unsigned joinBadgePlanes(const std::vector<ReplaySprite*>& pieces) {
 const size_t n=pieces.size();if(n>512)return 0;
 std::vector<size_t> parent(n);std::iota(parent.begin(),parent.end(),0);
 auto root=[&](size_t a){while(parent[a]!=a){parent[a]=parent[parent[a]];a=parent[a];}return a;};
 for(size_t i=0;i<n;++i)for(size_t j=0;j<i;++j){auto& a=*pieces[i];auto& b=*pieces[j];
  // Much tighter than V0.9.13: projected floor .2481 vs badge .2513
  // from report 093556 must NEVER share a plane (1.3 percent apart).
  if(a.noJoin||b.noJoin)continue;
  if(a.rhw<=0||b.rhw<=0||std::abs(a.rhw-b.rhw)>.002f*std::max(a.rhw,b.rhw))continue;
  if(std::abs(a.cx-b.cx)>.55f*std::max(a.width,b.width)||std::abs(a.cy-b.cy)>.55f*std::max(a.height,b.height))continue;
  parent[root(i)]=root(j);
 }
 std::vector<ReplaySprite*> anchor(n,nullptr);
 for(size_t i=0;i<n;++i){auto* p=pieces[i];if(!p->hasAxes||p->noJoin)continue;auto*& a=anchor[root(i)];
  // Prefer the original glyph quad's UV axes to curved ring segments.
  auto score=[](const ReplaySprite& x){return x.width*x.height*(x.corners.size()<=6?100.f:1.f);};
  if(!a||score(*p)>score(*a))a=p;
 }
 unsigned joined=0;
 for(size_t i=0;i<n;++i){auto& p=*pieces[i];if(p.noJoin)continue;auto* a=anchor[root(i)];if(!a)continue;
  p.sharedPlane=true;p.sharedPivot=a->originalCenter;p.sharedRight=a->sourceRight;p.sharedDown=a->sourceDown;p.up=a->up;++joined;
 }
 return joined;
}
}
