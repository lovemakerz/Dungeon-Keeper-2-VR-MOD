// DK2VR 0.3.0 - decoupled VR rendering: the view matrix path must reproduce the 0.2.9 per-vertex path.
#include "../src/common/Decouple.hpp"
#include <cstdio>
#include <cstdlib>
using namespace dk2vr;
static void expect(bool ok,const char* message) {if(!ok){printf("FAIL: %s\n",message);exit(1);}}
static bool near3(Vec3 a,Vec3 b,float e=2e-4f) {return std::fabs(a.x-b.x)<e&&std::fabs(a.y-b.y)<e&&std::fabs(a.z-b.z)<e;}
int main() {
    const Pose poses[]={{{0,0,0,1},{0,0,0}},{yawQuat(.7f),{.1f,-.6f,-.85f}},{normalized(multiply(yawQuat(-2.1f),pitch(.4f))),{-.3f,1.6f,.2f}}};
    const Vec3 points[]={{0,0,0},{.12f,.03f,-.2f},{-.4f,-.05f,.33f},{1.2f,.7f,-2.f}};
    // Similarity algebra.
    for(auto& a:poses) for(auto& b:poses) for(auto p:points) {
        Similarity sa=similarity(a),sb=similarity(b);sa.factor=1.7f;
        expect(near3(apply(then(sa,sb),p),apply(sb,apply(sa,p))),"then() composes in order");
        expect(near3(apply(inverse(sa),apply(sa,p)),p),"inverse() undoes the transform");
        float m[16];matrixOf(then(sa,sb),m);
        expect(near3(transformRow(m,p),apply(sb,apply(sa,p)),5e-4f),"row-vector matrix matches the transform");
    }
    // 1. Unchanged table: view matrix = 0.2.9 per-vertex toLocal(eye, p).
    for(auto& table:poses) for(auto& eye:poses) for(auto p:points) {
        Similarity view=then(tableCorrection(table,table,1,{0,0,0}),inverse(similarity(eye)));
        float m[16];matrixOf(view,m);
        expect(near3(transformRow(m,p),toLocal(eye,p),5e-4f),"same table: identical to the per-vertex path");
    }
    // 2. Table moved/turned by the grab between the game frame and the headset frame.
    for(auto& rec:poses) for(auto& now:poses) for(auto& eye:poses) for(auto p:points) {
        Similarity view=then(tableCorrection(rec,now,1,{0,0,0}),inverse(similarity(eye)));
        float m[16];matrixOf(view,m);
        Vec3 expected=toLocal(eye,toWorld(now,toLocal(rec,p)));
        expect(near3(transformRow(m,p),expected,5e-4f),"moved table: dungeon follows the table at headset rate");
    }
    // 3. Width change (GrabMode 0): table centre fixed, dungeon scaled around it, floor stays level.
    {
        Pose t=poses[1];Similarity c=tableCorrection(t,t,2,{0,0,0});
        expect(near3(apply(c,t.position),t.position),"table centre fixed when the table grows");
        Vec3 onTable=toWorld(t,{.1f,0,-.05f});
        Vec3 grown=apply(c,onTable);
        expect(near3(grown,toWorld(t,{.2f,0,-.1f})),"points scale around the table centre");
    }
    // 4. Pan prediction: the dungeon follows the hand exactly, in LOCAL, whatever the table yaw and width.
    for(auto& t:poses) for(float k:{1.f,.5f,2.5f}) {
        Vec3 hand{.07f,.04f,-.03f};
        Vec3 pan=panInTable(hand,t,k,1.f);
        Similarity c=tableCorrection(t,t,k,pan),c0=tableCorrection(t,t,k,{0,0,0});
        Vec3 horizontal=rotate(t.orientation,[&]{Vec3 h=rotate(conjugate(t.orientation),hand);h.y=0;return h;}());
        for(auto p:points) expect(near3(sub(apply(c,p),apply(c0,p)),horizontal,5e-4f),"pan: dungeon follows the hand on the table plane");
        Vec3 far=panInTable({3,0,0},t,1,.3f);
        expect(std::fabs(std::sqrt(far.x*far.x+far.z*far.z)-.3f)<1e-4f,"pan prediction is limited");
    }
    // 5. Clip planes: 0.2.9 used eye-space planes with an identity view; 0.3.0 uses record-LOCAL
    // planes (fixed-function planes are in world space). Same visibility for every point.
    {
        Tabletop table;table.center={.02f,-.4f,-.6f};table.right={1,0,0};table.up={0,1,0};table.forward={0,0,-1};
        table.halfWidth=.3f;table.halfDepth=.2f;table.valid=true;
        Pose tableCamera{normalized(multiply(yawQuat(.4f),pitch(-.5f))),{.1f,.3f,-.2f}};
        Pose identity{{0,0,0,1},{0,0,0}};
        for(auto& eye:poses) for(auto plane:table.planes()) for(float x:{-.5f,-.2f,0.f,.25f,.4f}) for(float z:{-.3f,0.f,.15f,.3f}) {
            Vec3 cameraPoint=table.point(x,z,.02f);
            Vec3 local=toWorld(tableCamera,cameraPoint);
            float legacy=transformPlane(plane,tableCamera,eye).distance(toLocal(eye,local));
            float now=transformPlane(plane,tableCamera,identity).distance(local);
            expect(std::fabs(legacy-now)<1e-4f,"record-LOCAL planes equal the 0.2.9 eye planes");
            // Under any table correction, the plane moves with the dungeon.
            Similarity c=tableCorrection(poses[1],poses[2],1.8f,{.05f,0,-.02f});
            float moved=transformPlane(transformPlane(plane,tableCamera,identity),c).distance(apply(c,local));
            expect((moved>=0)==(now>=0)&&std::fabs(moved-now*1.8f)<2e-4f,"plane transform keeps visibility");
            // 0.3.0 VR thread: record-LOCAL plane -> eye space through WORLD (VIEW = identity).
            Similarity toEye=then(tableCorrection(poses[1],poses[1],1,{0,0,0}),inverse(similarity(eye)));
            float viaWorld=transformPlane(transformPlane(plane,tableCamera,identity),toEye).distance(apply(toEye,local));
            expect(std::fabs(viaWorld-legacy)<2e-4f,"eye-space planes through WORLD equal 0.2.9");
        }
    }
    // 6. 0.3.1: dungeon correction = table correction when there is no pending motion; turning keeps the
    // pivot fixed and follows the yawQuat sense used by the hands; the table itself never moves with it.
    for(auto& rec:poses) for(auto& now:poses) for(auto p:points) {
        expect(near3(apply(dungeonCorrection(rec,now,1.3f,{0,0,0},0,{.1f,0,-.2f}),p),apply(tableCorrection(rec,now,1.3f,{0,0,0}),p)),"no motion: dungeon = table");
        Vec3 pan{.03f,0,-.02f};
        expect(near3(apply(dungeonCorrection(rec,now,1,pan,0,{.4f,0,.1f}),p),apply(tableCorrection(rec,now,1,pan),p)),"pan only: same as 0.3.0");
    }
    for(auto& t:poses) for(float yaw:{.3f,-1.1f}) {
        Vec3 pivot{.12f,0,-.05f};
        Similarity c=dungeonCorrection(t,t,1,{0,0,0},yaw,pivot);
        expect(near3(apply(c,toWorld(t,pivot)),toWorld(t,pivot)),"turn keeps the pivot in place");
        Vec3 arm=add(pivot,Vec3{.2f,0,0});
        expect(near3(apply(c,toWorld(t,arm)),toWorld(t,add(pivot,rotate(yawQuat(yaw),Vec3{.2f,0,0})))),"turn follows the hands' sense");
    }
    {
        // Hands turning: angle difference is a yawQuat angle (same convention as the 0.2.x table turn).
        Vec3 l{-.2f,0,0},r{.2f,0,0};float a0=handsAngle(l,r);
        Quat q=yawQuat(.4f);float a1=handsAngle(rotate(q,l),rotate(q,r));
        expect(std::fabs(wrapRadians(a1-a0)-.4f)<1e-4f,"hands angle measured in the yawQuat sense");
    }
    printf("DecoupleTests OK\n");
    return 0;
}
