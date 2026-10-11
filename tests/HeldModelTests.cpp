#include <vector>
#include "../src/common/HeldModel.hpp"
#include <cassert>
#include <cstdio>
using namespace dk2vr;
int main(){
    auto dot=[](Vec3 a,Vec3 b){return heldDot(a,b);};
    float depth=0;
    // One model vertex transformed by XY scale 80 and independent depth scale .01.
    assert(heldDepthPixels(7.f+2.f*.01f,7.f,80.f,.01f,depth));assert(std::fabs(depth-160.f)<.01f);
    auto p=heldModelPoint(520.f,430.f,depth,500.f,400.f);
    assert(p.x==20.f&&p.y==-30.f&&std::fabs(p.z+160.f)<.01f);
    assert(!heldDepthPixels(2,0,80,0,depth));assert(!heldDepthPixels(2,0,0,1,depth));
    assert(heldIconSplit(24.f,120.f)==120.f);assert(heldIconSplit(24.f,0.f)==24.f);
    // Separate limbs must use the same native Z origin as the torso.
    float torso=0,limb=0;assert(heldDepthPixels(7.02f,7.f,80.f,.01f,torso));
    assert(heldDepthPixels(7.04f,7.f,80.f,.01f,limb));
    assert(std::fabs((limb-torso)-160.f)<.01f);
    assert(heldNeedsUpright(180,90));assert(!heldNeedsUpright(90,180));
    auto up=heldUprightPoint({80,0,5},0,0);assert(up.y==80&&up.z==5);
    // The model has real depth: moving the eye changes its projection, not just a flat sprite.
    Projection lens{100,100,0,0};Vec3 a,b;
    assert(unproject(20,30,.5f,lens,1,a));assert(unproject(20,30,.25f,lens,1,b));
    assert(b.z==2*a.z&&b.x==2*a.x);
    auto q=toWorld(Pose{yawQuat(Pi/2),{1,2,3}},Vec3{1,-1,-2});
    assert(std::fabs(q.x+1)<1e-5&&std::fabs(q.y-1)<1e-5&&std::fabs(q.z-2)<1e-5);
    // 0.9.47.22: the 3D model sits in EXACTLY the frame of the flat held window; x/y keep the game's own screen coordinates (orientation, swing), depth is toward the head.
    {Vec3 hot{.3f,1.1f,-.4f},head{0.f,1.6f,0.f},R{},U{},T{};
     assert(heldWindowAxes(hot,head,R,U,T));
     assert(std::fabs(dot(R,R)-1)<1e-5f&&std::fabs(dot(U,U)-1)<1e-5f&&std::fabs(dot(T,T)-1)<1e-5f&&std::fabs(dot(R,U))<1e-5f&&std::fabs(dot(R,T))<1e-5f&&std::fabs(dot(U,T))<1e-5f);
     assert(dot(cross(R,U),T)>.999f);                                   // right-handed, like the quaternion built from these axes
     assert(std::fabs(R.y)<1e-5f&&U.y>0);                               // vertical kept: right is horizontal, up points up
     const float m=.0049f;Vec3 w=heldWorldPoint(hot,R,U,T,m,{20.f,-30.f,0.f});
     // the flat window's formula: hot + right*(px*m) + up*(-py*m) with p = (sx-cx, sy-cy) = (20, 30)
     Vec3 flat=add(hot,add(scale(R,20.f*m),scale(U,-30.f*m)));
     assert(std::fabs(w.x-flat.x)<1e-6f&&std::fabs(w.y-flat.y)<1e-6f&&std::fabs(w.z-flat.z)<1e-6f);
     Vec3 near=heldWorldPoint(hot,R,U,T,m,{0.f,0.f,100.f});assert(dot(sub(near,hot),T)>0);   // positive local z = toward the head
     // a swing (rotation in the screen plane) is the same rotation in the world frame: the angle between two model points is preserved
     const float a=.6f;Vec3 p0{50.f,0.f,0.f},p1{50.f*std::cos(a),50.f*std::sin(a),0.f};
     Vec3 d0=sub(heldWorldPoint(hot,R,U,T,m,p0),hot),d1=sub(heldWorldPoint(hot,R,U,T,m,p1),hot);
     assert(std::fabs(dot(d0,d1)/(std::sqrt(dot(d0,d0))*std::sqrt(dot(d1,d1)))-std::cos(a))<1e-4f);
     assert(!heldWindowAxes(hot,hot,R,U,T));Vec3 above{.3f,2.1f,-.4f};assert(!heldWindowAxes(hot,above,R,U,T));}   // degenerate: head on the hotspot / straight above
    // depth direction measured from the game's own depth values
    {HeldDepthSign m;for(int i=0;i<10;++i) m.add(float(i)*10.f,.2f+.01f*float(i));double c=0;
     assert(m.result(true,-1,c)==1&&c>.99);assert(m.result(false,1,c)==-1);          // z grows with depth: larger = farther (LESS); GREATER flips
     HeldDepthSign r;for(int i=0;i<10;++i) r.add(float(i)*10.f,.9f-.01f*float(i));
     assert(r.result(true,1,c)==-1&&c<-.99);
     HeldDepthSign flat;for(int i=0;i<10;++i) flat.add(5.f,.5f);assert(flat.result(true,-1,c)==-1&&c==0);   // constant depth: previous kept
     HeldDepthSign few;few.add(1,1);few.add(2,2);assert(few.result(true,1,c)==1);
     HeldDepthSign noisy;const float zs[8]={.5f,.1f,.9f,.3f,.7f,.2f,.8f,.4f},ds[8]={1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f};for(int i=0;i<8;++i) noisy.add(ds[i]*50.f,zs[i]);
     assert(noisy.result(true,1,c)==1||std::fabs(c)>.5);}
    // 0.9.47.23 - depth direction from the faces of a closed model: UV sphere drawn with the game's culling rule, both depth conventions
    {
        struct P {float x,y,z;};   // z toward the viewer
        std::vector<P> pts;const int NA=24,NB=16;
        for(int b=0;b<=NB;++b)for(int a=0;a<NA;++a){const float th=3.14159265f*float(b)/NB,ph=2.f*3.14159265f*float(a)/NA;pts.push_back({50.f*std::sin(th)*std::cos(ph),50.f*std::cos(th),50.f*std::sin(th)*std::sin(ph)});}
        for(int convention=0;convention<2;++convention) {
            HeldFaceVote v;
            for(int b=0;b<NB;++b)for(int a=0;a<NA;++a){
                const int i0=b*NA+a,i1=b*NA+(a+1)%NA,i2=(b+1)*NA+a,i3=(b+1)*NA+(a+1)%NA;
                const int tris[2][3]={{i0,i1,i2},{i1,i3,i2}};
                for(auto& t3:tris){
                    P A=pts[t3[0]],B=pts[t3[1]],C=pts[t3[2]];const float cz=(A.z+B.z+C.z)/3.f;
                    float a2[2]={A.x,-A.y},b2[2]={B.x,-B.y},c2[2]={C.x,-C.y};
                    float area=heldTriArea(a2,b2,c2);
                    if((area>=0.f)!=(cz>0.f)) {std::swap(b2[0],c2[0]);std::swap(b2[1],c2[1]);std::swap(B,C);area=heldTriArea(a2,b2,c2);}   // wound like the game's meshes: visible (area >= 0) = turned toward the viewer
                    const float depth=(convention==0?-1.f:1.f)*cz;   // convention 0: larger depth = farther
                    v.add(area,depth);
                }
            }
            const double c=v.correlation();
            assert(convention==0?c<-.5:c>.5);
            HeldDepthDecider dd;dd.add(v);assert(dd.sign==(convention==0?1:-1));
        }
        HeldFaceVote none;none.add(10.f,5.f);none.add(-10.f,6.f);assert(none.correlation()==0);        // too few triangles
        HeldFaceVote one;for(int i=0;i<40;++i) one.add(10.f,float(i));assert(one.correlation()==0);     // front faces only
        HeldFaceVote flat;for(int i=0;i<40;++i) flat.add(i&1?10.f:-10.f,3.f);assert(flat.correlation()==0);   // a flat model has no direction
        // the decision is kept across calls and flips only with strong, repeated opposite evidence
        HeldDepthDecider dd;HeldFaceVote A,B;
        for(int i=0;i<60;++i){A.add(10.f,-float(i%10)-(i&1?0.f:10.f));}                                 // front -> smaller depth (negative corr) => +1
        for(int i=0;i<200;++i) {HeldFaceVote x;for(int j=0;j<50;++j) x.add(j&1?10.f:-10.f,(j&1?-8.f:8.f)+float(j%3));dd.add(x);}
        assert(dd.sign==1);
        HeldFaceVote weak;for(int j=0;j<50;++j) weak.add(j&1?10.f:-10.f,float(j%7)+(j&1?0.2f:-0.2f)*0.f);dd.add(weak);assert(dd.sign==1);
        for(int i=0;i<400;++i) {HeldFaceVote x;for(int j=0;j<50;++j) x.add(j&1?10.f:-10.f,(j&1?8.f:-8.f)+float(j%3));dd.add(x);}
        assert(dd.sign==-1);
    }
    // 0.9.47.24 - overlap vote on a closed model (both conventions), and on a thin double-sided plate where the mean-depth comparison was wrong
    {
        struct P {float x,y,z;};std::vector<P> pts;const int NA=24,NB=16;
        for(int b=0;b<=NB;++b)for(int a=0;a<NA;++a){const float th=3.14159265f*float(b)/NB,ph=2.f*3.14159265f*float(a)/NA;pts.push_back({50.f*std::sin(th)*std::cos(ph),50.f*std::cos(th),50.f*std::sin(th)*std::sin(ph)});}
        for(int convention=0;convention<2;++convention) {
            std::vector<HeldTri> tris;
            for(int b=0;b<NB;++b)for(int a=0;a<NA;++a){
                const int i0=b*NA+a,i1=b*NA+(a+1)%NA,i2=(b+1)*NA+a,i3=(b+1)*NA+(a+1)%NA;const int T[2][3]={{i0,i1,i2},{i1,i3,i2}};
                for(auto& t3:T){P A=pts[t3[0]],B=pts[t3[1]],C=pts[t3[2]];const float cz=(A.z+B.z+C.z)/3.f;
                    float a2[2]={A.x,-A.y},b2[2]={B.x,-B.y},c2[2]={C.x,-C.y};float area=heldTriArea(a2,b2,c2);
                    if((area>=0.f)!=(cz>0.f)) {std::swap(B,C);}
                    const float sg=convention==0?-1.f:1.f;HeldTri h;P Q[3]={A,B,C};for(int k=0;k<3;++k){h.x[k]=Q[k].x;h.y[k]=-Q[k].y;h.d[k]=sg*Q[k].z;}tris.push_back(h);}
            }
            HeldOverlapVote v;heldOverlapVote(tris,v);HeldSignDecider dd;dd.add(v);
            assert(v.nearSmaller+v.nearLarger>30);
            assert(convention==0?(v.nearSmaller>4*v.nearLarger&&dd.sign==1):(v.nearLarger>4*v.nearSmaller&&dd.sign==-1));
        }
        // thin plate (spear): front face at depth -1 and back face at +1 over the same screen area, plus an unrelated distant strip of mixed faces
        std::vector<HeldTri> tris;
        for(int i=0;i<40;++i){HeldTri f{{0,10,0},{0,0,10},{-1,-1,-1}},b{{0,0,10},{0,10,10},{1,1,1}};
            f.x[0]+=i*3;f.x[1]+=i*3;f.x[2]+=i*3;b.x[0]+=i*3;b.x[1]+=i*3;b.x[2]+=i*3;
            // f is wound with area >= 0 (front), b with area < 0 (back)
            float a[2]={f.x[0],f.y[0]},bb[2]={f.x[1],f.y[1]},c[2]={f.x[2],f.y[2]};if(heldTriArea(a,bb,c)<0){std::swap(f.x[1],f.x[2]);std::swap(f.y[1],f.y[2]);std::swap(f.d[1],f.d[2]);}
            float a2[2]={b.x[0],b.y[0]},b2[2]={b.x[1],b.y[1]},c2[2]={b.x[2],b.y[2]};if(heldTriArea(a2,b2,c2)>=0){std::swap(b.x[1],b.x[2]);std::swap(b.y[1],b.y[2]);std::swap(b.d[1],b.d[2]);}
            tris.push_back(f);tris.push_back(b);}
        HeldOverlapVote v;heldOverlapVote(tris,v);assert(v.nearSmaller>0&&v.nearLarger==0);
        HeldSignDecider keep;HeldOverlapVote strong{200,5};keep.add(strong);assert(keep.sign==1);HeldOverlapVote weakOpp{10,14};keep.add(weakOpp);assert(keep.sign==1);
        for(int i=0;i<60;++i) {HeldOverlapVote o{5,100};keep.add(o);}assert(keep.sign==-1);
        HeldSignDecider few;HeldOverlapVote tiny{3,1};few.add(tiny);assert(few.sign==0);
    }
    // slope of the depth against the game's z
    {
        float z[10],d[10];for(int i=0;i<10;++i){z[i]=.1f+.003f*float(i);d[i]=6400.f*(z[i]-.1f)/2.f;}
        double s=0,c=0;assert(heldSlopeFit(z,d,10,s,c)&&std::fabs(s-3200.0)<1.0&&c>.9999);
        for(int i=0;i<10;++i) d[i]=-d[i];assert(heldSlopeFit(z,d,10,s,c)&&s<-3000.0&&c<-.9999);
        assert(!heldSlopeFit(z,d,4,s,c));float flat[10];for(int i=0;i<10;++i) flat[i]=.5f;assert(!heldSlopeFit(flat,d,10,s,c));
    }
    puts("PASS: held model shared depth, upright axes, invalid scales, complete icon split, controller transform, face-based depth direction, overlap vote, depth slope.");
}
