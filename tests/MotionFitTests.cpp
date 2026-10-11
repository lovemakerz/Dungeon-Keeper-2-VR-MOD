// DK2VR 0.8.68 - measured rigid motion of the possession world: same sign convention as yawQuat / dungeonCorrection.
#include "../src/common/MotionFit.hpp"
#include "../src/common/Decouple.hpp"
#include <cstdio>
#include <cstdlib>
#include <random>
using namespace dk2vr;
static void expect(bool ok,const char* message) {if(!ok){printf("FAIL: %s\n",message);exit(1);}}
int main() {
    std::mt19937 rng(7);std::uniform_real_distribution<float> U(-6.f,6.f),N(-.002f,.002f);
    // 1. The fit convention equals rotate(yawQuat(phi), v) and dungeonCorrection's yaw about the pivot.
    for(float phi:{-.2f,-.03f,.0f,.011f,.15f,.6f}) {
        const Vec3 a{1.3f,0,-.7f};const Vec3 q=rotate(yawQuat(phi),a),m=planarRotate(phi,a.x,a.z);
        expect(std::fabs(q.x-m.x)<1e-5f&&std::fabs(q.z-m.z)<1e-5f,"planarRotate == rotate(yawQuat)");
    }
    // 2. Recovers a known rotation about the pivot + translation, with noise and outliers (moving creatures).
    for(int trial=0;trial<200;++trial) {
        const float phi=std::uniform_real_distribution<float>(-.12f,.12f)(rng);
        const Vec3 t{std::uniform_real_distribution<float>(-.2f,.2f)(rng),0,std::uniform_real_distribution<float>(-.2f,.2f)(rng)};
        std::vector<PlanarPair> pr;
        for(int i=0;i<120;++i) {
            const float ax=U(rng),az=U(rng);const Vec3 q=planarRotate(phi,ax,az);
            PlanarPair p{ax,az,q.x+t.x+N(rng),q.z+t.z+N(rng),1.f};
            if(i%6==0) {p.bx+=U(rng)*.1f;p.bz+=U(rng)*.1f;} // 17 % outliers
            pr.push_back(p);
        }
        PlanarFit f=fitPlanar(pr,.01f,12,.4f,.5f,.03f);
        expect(f.ok,"fit accepted");
        expect(std::fabs(f.phi-phi)<.002f,"rotation recovered");
        expect(std::fabs(f.tx-t.x)<.01f&&std::fabs(f.tz-t.z)<.01f,"translation recovered");
    }
    // 3. Sign: the world of a creature that turns right is rotated by +phi (yawQuat convention) about the pivot.
    {
        const float phi=.05f;std::vector<PlanarPair> pr;
        for(int i=0;i<40;++i) {const float ax=U(rng),az=U(rng);
            const Vec3 b=rotate(yawQuat(phi),{ax,0,az});pr.push_back({ax,az,b.x,b.z,1.f});}
        PlanarFit f=fitPlanar(pr,.01f,12,.4f,.5f,.03f);
        expect(f.ok&&std::fabs(f.phi-phi)<1e-4f,"sign matches yawQuat");
        // and dungeonCorrection with the same angle moves points exactly like the measured motion
        const Pose id{{0,0,0,1},{0,0,0}};const Similarity c=dungeonCorrection(id,id,1,{0,0,0},phi,{0,0,0});
        const Vec3 moved=apply(c,{2.f,0.4f,-1.f}),want=rotate(yawQuat(phi),{2.f,0.4f,-1.f});
        expect(std::fabs(moved.x-want.x)<1e-5f&&std::fabs(moved.z-want.z)<1e-5f&&std::fabs(moved.y-.4f)<1e-5f,"dungeonCorrection uses the same rotation");
    }
    // 4. Pivot not at the origin: positions given relative to the pivot give the same correction as dungeonCorrection about it.
    {
        const Vec3 pivot{3.f,0,-2.f};const float phi=-.07f;const Vec3 tr{.05f,0,-.02f};
        std::vector<PlanarPair> pr;const Pose id{{0,0,0,1},{0,0,0}};const Similarity c=dungeonCorrection(id,id,1,tr,phi,pivot);
        for(int i=0;i<60;++i) {const Vec3 a{U(rng)+pivot.x,0,U(rng)+pivot.z};const Vec3 b=apply(c,a);
            pr.push_back({a.x-pivot.x,a.z-pivot.z,b.x-pivot.x,b.z-pivot.z,1.f});}
        PlanarFit f=fitPlanar(pr,.01f,12,.4f,.5f,.03f);
        expect(f.ok&&std::fabs(f.phi-phi)<1e-4f&&std::fabs(f.tx-tr.x)<1e-4f&&std::fabs(f.tz-tr.z)<1e-4f,"pivot convention equals dungeonCorrection");
    }
    // 5. Rejections: too few pairs, points all near the pivot (rotation undetermined), garbage.
    {
        std::vector<PlanarPair> few;for(int i=0;i<5;++i) few.push_back({1.f*i,0.f,1.f*i,0.f,1.f});
        expect(!fitPlanar(few,.01f,12,.4f,.5f,.03f).ok,"too few pairs rejected");
        std::vector<PlanarPair> near;for(int i=0;i<30;++i) near.push_back({N(rng),N(rng),N(rng),N(rng),1.f});
        expect(!fitPlanar(near,.01f,12,.4f,.5f,.03f).ok,"degenerate spread rejected");
        std::vector<PlanarPair> junk;for(int i=0;i<60;++i) junk.push_back({U(rng),U(rng),U(rng),U(rng),1.f});
        expect(!fitPlanar(junk,.01f,12,.4f,.5f,.03f).ok,"random pairs rejected");
    }
    printf("MotionFitTests OK\n");
    return 0;
}
