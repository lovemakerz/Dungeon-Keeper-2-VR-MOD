// made by LoVeMaKeRz
#include "../src/common/UprightSprite.hpp"
#include <cassert>
#include <cstdio>
#include "../src/common/Decouple.hpp"
using namespace dk2vr;
static float dist(Vec3 a,Vec3 b){Vec3 d=sub(a,b);return std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z);}
int main(){
    int checks=0;
    for(float spin:{0.f,.7f,1.57f,-2.f}) for(float tilt:{0.f,.7f}) for(Vec3 eye:std::vector<Vec3>{{2,3,4},{-2,6,1},{0,4,0}}){
        Vec3 vertical=rotate(pitch(tilt),{0,1,0});Vec3 a=rotate(yawQuat(spin),{.6f,0,0}),b=rotate(yawQuat(spin),{0,0,.9f});
        Vec3 pivot{.2f,1.f,-.3f};std::vector<SpriteSample> s;
        for(auto uv:std::vector<std::pair<float,float>>{{.2f,.1f},{.8f,.1f},{.2f,.9f},{.2f,.9f},{.8f,.1f},{.8f,.9f},{.8f,.9f}})
            s.push_back({add(pivot,add(scale(a,uv.first-.5f),scale(b,uv.second-.5f))),uv.first,uv.second});
        UprightSprite f;assert(f.fit(s,vertical,eye,{1,0,0}));assert(dist(f.center,pivot)<1e-5f);
        Vec3 bot=add(pivot,scale(b,.4f));assert(dist(f.vertex(.5f,.9f,true),bot)<1e-5f);
        Vec3 top=f.vertex(.5f,.1f,true);assert(dist(sub(top,bot),scale(vertical,.72f))<1e-5f);
        assert(dist(f.vertex(.5f,.5f,false),pivot)<1e-5f);
        Vec3 x=sub(f.vertex(.8f,.5f,false),f.vertex(.2f,.5f,false));assert(std::fabs(x.x*vertical.x+x.y*vertical.y+x.z*vertical.z)<1e-5f);
        s.push_back(s.front());UprightSprite g;assert(g.fit(s,vertical,eye,{1,0,0}));assert(dist(f.center,g.center)<1e-5f);checks+=7;
    }
    // Integration regression: table.up is NATIVE camera space, vertices and eyes are OpenXR WORLD space.
    for(float tilt:{0.f,.35f,-.5f}) {
        Pose camera{multiply(pitch(tilt),pitch(-Pi*.5f)),{.3f,-.8f,.2f}};
        Vec3 up=spriteWorldVertical(camera,{0,0,1});
        assert(dist(up,rotate(pitch(tilt),{0,1,0}))<1e-5f);
        std::vector<SpriteSample> sample={{{-.1f,-.8f,-.2f},.7f,.2f},{{.1f,-.8f,-.2f},.8f,.2f},{{-.1f,-.8f,.2f},.7f,.3f},{{.1f,-.8f,.2f},.8f,.3f}};
        UprightSprite f;assert(f.fit(sample,up,{.5f,.5f,1.f},{1,0,0}));
        Vec3 bottom=f.vertex(.75f,.3f,true),top=f.vertex(.75f,.2f,true);
        assert(dist(sub(top,bottom),scale(up,.4f))<1e-5f);
        Vec3 normal=cross(f.right,f.up),toward=sub(Vec3{.5f,.5f,1.f},f.center);
        assert(normal.x*toward.x+normal.y*toward.y+normal.z*toward.z>0.f);checks+=4;
    }

    // One unchanged recorded frame must follow subsequent head/table motion at VR cadence.
    struct V {Vec3 p;unsigned color;float u,v;};
    V corners[]={{{-.1f,1,-.2f},0xffabcdef,0,0},{{.1f,1,-.2f},0xffabcdef,1,0},{{-.1f,.6f,-.2f},0xffabcdef,0,1},{{.1f,.6f,-.2f},0xffabcdef,1,1}};
    std::vector<unsigned char> original(sizeof(corners));memcpy(original.data(),corners,sizeof(corners));
    ReplaySprite replay;replay.up={0,1,0};for(unsigned i=0;i<4;++i)replay.corners.push_back({i,corners[i].u,corners[i].v});
    for(float angle:{0.f,1.57f,3.14f,-1.57f}) for(float height:{-3.f,0.f,1.f,10.f}) for(float scaleK:{.5f,1.f,2.f}) {
        Similarity move{yawQuat(.7f),scaleK,{.3f,-.2f,.4f}};
        Vec3 eyeWorld=add(apply(move,{0,.8f,-.2f}),rotate(yawQuat(angle),{0,height,2}));
        auto bytes=original;assert(replay.rebuild(bytes,0,sizeof(V),4,apply(inverse(move),eyeWorld)));
        V v[4];memcpy(v,bytes.data(),sizeof(v));Vec3 center=scale(add(v[0].p,v[3].p),.5f);
        assert(dist(center,{0,.8f,-.2f})<1e-5f);Vec3 right=sub(v[1].p,v[0].p),up=sub(v[0].p,v[2].p);
        Vec3 normal=rotate(move.rotation,cross(right,up)),toward=sub(eyeWorld,apply(move,center));
        assert(dot(normal,toward)>0.f);assert(std::fabs(dist(up,{0,0,0})-.4f)<1e-5f); assert(dist(scale(normal,1.f/std::sqrt(dot(normal,normal))),scale(toward,1.f/std::sqrt(dot(toward,toward))))<1e-5f);
        for(unsigned i=0;i<4;++i)assert(v[i].color==corners[i].color&&v[i].u==corners[i].u&&v[i].v==corners[i].v);
        assert(original==std::vector<unsigned char>((unsigned char*)corners,(unsigned char*)corners+sizeof(corners)));checks+=9;
    }
    // 0.9.36: glyph whose texture is turned by its UVs (u runs DOWN the screen): the screen layout is kept, the glyph reads as on the monitor
    {
        struct V {Vec3 p;unsigned color;float u,v;};
        // game screen: TL(0,0) TR(10,0) BL(0,10) BR(10,10) pixels; world quad in the plane z=-.2 facing the viewer, 0.01 m per pixel; UV rotated 90 deg
        V q[]={{{-.05f,1.05f,-.2f},1,0,1},{{.05f,1.05f,-.2f},1,0,0},{{-.05f,.95f,-.2f},1,1,1},{{.05f,.95f,-.2f},1,1,0}};
        const float sx[4]={-5,5,-5,5},sy[4]={-5,-5,5,5};
        std::vector<unsigned char> bytes(sizeof(q));memcpy(bytes.data(),q,sizeof(q));
        ReplaySprite r;r.up={0,1,0};r.screenAxes=true;r.pixelWorld=.01f;
        for(unsigned i=0;i<4;++i){SpriteCorner c{i,q[i].u,q[i].v,q[i].p};c.sx=sx[i];c.sy=sy[i];r.corners.push_back(c);}
        for(Vec3 eye:std::vector<Vec3>{{0,1,3},{1,2,3},{-2,1.5f,2}}) {
            auto b=bytes;assert(r.rebuild(b,0,sizeof(V),4,eye));V v[4];memcpy(v,b.data(),sizeof(v));
            Vec3 c=scale(add(add(v[0].p,v[1].p),add(v[2].p,v[3].p)),.25f);assert(dist(c,{0,1,-.2f})<1e-5f);
            Vec3 n=sub(eye,c);n=scale(n,1.f/std::sqrt(dot(n,n)));Vec3 right=cross({0,1,0},n);right=scale(right,1.f/std::sqrt(dot(right,right)));
            assert(dot(sub(v[1].p,v[0].p),right)>.099f);              // screen right stays right for the viewer
            assert(v[0].p.y>v[2].p.y+.07f&&v[1].p.y>v[3].p.y+.07f);   // screen top stays up
            assert(std::fabs(dot(sub(v[0].p,c),n))<1e-5f);checks+=4;  // plane faces the eye
        }
    }
    UprightSprite degenerate;assert(!degenerate.fit({{{0,0,0},0,0},{{1,0,0},0,0},{{0,0,1},0,0}},{0,1,0},{0,4,0},{1,0,0}));
    std::printf("UprightSpriteTests: %d checks PASS (roll, UV centre, repeated corners, torch base, tilted table)\n",checks+1);
}
