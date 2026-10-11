// DK2VR 0.9.47.58 - selection outlines (blue / red cube drawn on the floor tiles and rooms) kept visible in the headset.
// The game draws every segment of the outline as a flat sliver (a triangle a few pixels long and 0.5-2 px wide, double sided). With the zoomed-out dungeon the sliver is
// only ~1.5 mm wide in the table; seen from the headset at an angle it is almost edge-on and vanishes by pieces (report 20261010_004800: dashes and stray pieces).
// Here every such thin triangle of an untextured replayed draw is rebuilt, at each headset frame and for each eye, as a ribbon of at least minWidth that FACES that eye,
// slightly lifted toward it so that the floor does not cut half of it. Everything else in the draw is copied unchanged.
#pragma once
#include "Math.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <vector>
namespace dk2vr {
namespace ribbon {
// Returns the number of triangles written to 'out' (vertex bytes, stride 'stride', triangle list), or 0 when no triangle is thin (the caller then draws the original).
// 'thin' receives the number of triangles rebuilt as ribbons. Positions are the first 3 floats of every vertex.
inline unsigned build(const unsigned char* verts,unsigned stride,unsigned vcount,const unsigned char* idx,bool idx32,unsigned prims,Vec3 eye,float minWidth,float lift,
                      std::vector<unsigned char>& out,unsigned& thin) {
    thin=0;out.clear();
    if(!verts||stride<12||prims==0||prims>512||!(minWidth>0)||!std::isfinite(eye.x+eye.y+eye.z)) return 0;
    auto pos=[&](unsigned i)->Vec3 {Vec3 p;std::memcpy(&p,verts+size_t(i)*stride,12);return p;};
    auto vi=[&](unsigned n)->unsigned {if(!idx) return n;return idx32?reinterpret_cast<const uint32_t*>(idx)[n]:reinterpret_cast<const uint16_t*>(idx)[n];};
    auto dot=[](Vec3 a,Vec3 b){return a.x*b.x+a.y*b.y+a.z*b.z;};
    auto push=[&](unsigned src,Vec3 p) {const size_t at=out.size();out.resize(at+stride);std::memcpy(out.data()+at,verts+size_t(src)*stride,stride);std::memcpy(out.data()+at,&p,12);};
    unsigned tris=0;
    for(unsigned t=0;t<prims;++t) {
        const unsigned i[3]={vi(t*3),vi(t*3+1),vi(t*3+2)};
        if(i[0]>=vcount||i[1]>=vcount||i[2]>=vcount) {out.clear();thin=0;return 0;}   // anything unexpected: the original draw, unchanged
        const Vec3 a[3]={pos(i[0]),pos(i[1]),pos(i[2])};
        // longest edge = the axis of the line
        int p0=0,p1=1,r=2;float best=-1;
        for(int k=0;k<3;++k) {const Vec3 d=sub(a[(k+1)%3],a[k]);const float l2=dot(d,d);if(l2>best) {best=l2;p0=k;p1=(k+1)%3;r=(k+2)%3;}}
        const float L=std::sqrt(best);
        bool widened=false;
        if(std::isfinite(L)&&L>1e-7f) {
            const Vec3 axis=scale(sub(a[p1],a[p0]),1.f/L);
            const Vec3 c=cross(sub(a[p1],a[p0]),sub(a[r],a[p0]));
            const float width=std::sqrt(dot(c,c))/L;   // height of the triangle over its long edge
            if(width<minWidth*.9f&&width<L*.5f) {
                const Vec3 mid=scale(add(a[p0],a[p1]),.5f);Vec3 view=sub(eye,mid);const float vl=std::sqrt(dot(view,view));
                if(std::isfinite(vl)&&vl>1e-6f) {
                    view=scale(view,1.f/vl);Vec3 n=cross(axis,view);const float nl=std::sqrt(dot(n,n));
                    if(std::isfinite(nl)&&nl>.05f) {   // not seen along its own length
                        n=scale(n,.5f*minWidth/nl);const Vec3 up=scale(view,lift);
                        const Vec3 P=add(a[p0],up),Q=add(a[p1],up);
                        const Vec3 pa=sub(P,n),pb=add(P,n),qa=sub(Q,n),qb=add(Q,n);
                        const unsigned sp=i[p0],sq=i[p1];
                        push(sp,pa);push(sp,pb);push(sq,qb);push(sp,pa);push(sq,qb);push(sq,qa);   // front
                        push(sp,pb);push(sp,pa);push(sq,qb);push(sq,qb);push(sp,pa);push(sq,qa);   // back (both windings: the game's cull state is not relied on)
                        tris+=4;++thin;widened=true;
                    }
                }
            }
        }
        if(!widened) {for(int k=0;k<3;++k) push(i[k],a[k]);++tris;}
    }
    if(thin==0) {out.clear();return 0;}
    return tris;
}
}
}
