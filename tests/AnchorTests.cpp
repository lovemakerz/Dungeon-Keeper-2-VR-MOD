// DK2VR 0.2.0 - Demeo-style table anchor: native camera -> fixed table frame.
#include "../src/common/Tabletop.hpp"
#include <cstdio>
#include <cstdlib>
using namespace dk2vr;
static void expect(bool ok,const char* message) {if(!ok){printf("FAIL: %s\n",message);exit(1);}}
static bool near3(Vec3 a,Vec3 b,float e=2e-4f) {return std::fabs(a.x-b.x)<e&&std::fabs(a.y-b.y)<e&&std::fabs(a.z-b.z)<e;}
int main() {
    // Rotation from matrix rows, checked against direct matrix products.
    for(float a:{0.f,.7f,2.4f,-2.9f}) for(float b:{0.f,-.9f,1.3f}) {
        Quat q=normalized(multiply(yawQuat(a),pitch(b)));
        Vec3 r0=rotate(conjugate(q),{1,0,0}),r1=rotate(conjugate(q),{0,1,0}),r2=rotate(conjugate(q),{0,0,1});
        Quat back=quatFromRows(r0,r1,r2);
        for(Vec3 v:{Vec3{1,2,3},Vec3{-.4f,.1f,.9f}}) expect(near3(rotate(back,v),rotate(q,v),1e-4f),"quaternion from rotation rows");
    }
    // Real in-game DKII capture (camera 5.879 units above the floor, 51 degrees pitch).
    Projection lens{639.921875f,639.921875f,512.25f,322.25f};
    auto units=makeNativeTabletop({0,1.241805196f,.7807372212f},{46.68688965f,-80.42286682f,39.91866302f},lens,512,322,1,.92f);
    expect(units.valid,"native footprint in game units");
    const float width=.80f;
    const float perMeter=2*units.halfWidth/width;
    auto table=scaled(units,1/perMeter);
    expect(std::fabs(table.halfWidth*2-width)<1e-4f,"visible dungeon width equals requested table width");
    Pose inTable=nativeCameraInTable(table);
    expect(near3(toWorld(inTable,table.center),{0,0,0}),"footprint centre sits at table origin");
    expect(near3(toWorld(inTable,add(table.center,scale(table.up,.1f))),{0,.1f,0}),"floor normal becomes vertical");
    expect(near3(toWorld(inTable,add(table.center,scale(table.right,.2f))),{.2f,0,0}),"native right stays on the right");
    expect(near3(toWorld(inTable,add(table.center,scale(table.forward,.3f))),{0,0,-.3f}),"native view direction goes away from the player");
    Vec3 eye=toWorld(inTable,{0,0,0});
    expect(eye.y>0&&eye.z>0,"native camera is above the table, on the player side");
    // Place the table in LOCAL space, rotated by the grab gesture.
    Pose tablePose{yawQuat(.5f),{.1f,-.6f,-.85f}};
    Pose camera=compose(tablePose,inTable);
    expect(near3(toWorld(camera,table.center),tablePose.position),"table centre follows the table pose");
    Vec3 up=sub(toWorld(camera,add(table.center,scale(table.up,.05f))),tablePose.position);
    expect(near3(up,{0,.05f,0}),"walls stand vertically whatever the yaw");
    // Clip planes remain exact in each eye after the new composition.
    auto planes=table.planes();
    Pose eyes[2]={{yawQuat(.2f),{-.032f,0,0}},{normalized(multiply(yawQuat(-.3f),pitch(-.6f))),{.03f,.05f,.1f}}};
    for(auto& e:eyes) for(auto plane:planes) for(float h:{0.f,.04f}) {
        Vec3 source=table.point(.05f,-.07f,h);
        auto converted=transformPlane(plane,camera,e);
        expect(std::fabs(plane.distance(source)-converted.distance(toLocal(e,toWorld(camera,source))))<1e-5f,"clip plane invariance");
    }
    // Zooming out (camera twice higher) keeps the table the same physical size.
    auto zoomed=makeNativeTabletop({0,1.241805196f,.7807372212f},{46.68688965f*2,-80.42286682f*2,39.91866302f*2},lens,512,322,1,.92f);
    expect(zoomed.valid&&std::fabs(zoomed.halfWidth-2*units.halfWidth)<1e-3f*units.halfWidth,"zoom out doubles the visible dungeon area");
    auto zoomedTable=scaled(zoomed,1/(2*zoomed.halfWidth/width));
    expect(std::fabs(zoomedTable.halfWidth*2-width)<1e-4f,"auto-fit keeps a constant table size");
    // Regression 0.2.1: straight-down camera (DKII zoomed out) must remain valid and level.
    auto topDown=makeTabletop({0,0,1},{0,0,-8},lens,0,0,1024,644,.92f);
    expect(topDown.valid,"top-down camera accepted");
    Pose topPose=nativeCameraInTable(scaled(topDown,1/(2*topDown.halfWidth/width)));
    Vec3 topEye=toWorld(topPose,{0,0,0});
    expect(topEye.y>0&&std::fabs(topEye.z)<1e-3f,"top-down native eye is right above the table centre");
    expect(!makeTabletop({0,.3f,-.95f},{0,0,-8},lens,0,0,1024,644,.92f).valid,"camera looking away from the floor rejected");
    // 0.2.6: rectangle computed on the HIGH level (wall tops), base on the LOW level (pits, water).
    // Every corner of the prism between both levels must stay inside the native screen.
    {
        Vec3 up=units.up;float floorH=dot(up,units.center);
        for(float wallTop:{1.f,1.6f}) for(float pit:{.2f,.5f}) {
            float high=floorH+wallTop,low=floorH-pit;
            auto top=makeLeveledTabletop(up,low,high,lens,0,0,2*(lens.centerX+.5f),2*(lens.centerY+.5f),.92f);
            expect(top.valid,"rectangle valid between both levels");
            expect(std::fabs(dot(up,top.center)-low)<1e-4f,"table centre on the low level (base under every floor)");
            for(int cx:{-1,1}) for(int cz:{-1,1}) for(float h:{0.f,1.f}) {
                Vec3 q=top.point(cx*top.halfWidth,cz*top.halfDepth,(high-low)*h); // from the low plane up to the top plane
                float sx=lens.centerX+lens.focalX*q.x/-q.z,sy=lens.centerY-lens.focalY*q.y/-q.z;
                expect(q.z<0&&sx>=-.5f&&sx<=2*(lens.centerX+.5f)+.5f&&sy>=-.5f&&sy<=2*(lens.centerY+.5f)+.5f,"prism corner inside the native view (no diagonal cut)");
            }
        }
    }
    printf("Real camera footprint %.2f x %.2f game units, %.2f units/m on a %.2f m table; native eye at (%.3f,%.3f,%.3f) m.\n",
        units.halfWidth*2,units.halfDepth*2,perMeter,width,eye.x,eye.y,eye.z);
    puts("PASS: table anchor, orientation, width fit, zoom, clip invariance.");
}
