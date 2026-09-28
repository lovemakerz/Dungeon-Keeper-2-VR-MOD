#include "../src/common/Tabletop.hpp"
#include <cstdio>
#include <cstdlib>
using namespace dk2vr;
void expect(bool ok,const char* message) {if(!ok){printf("FAIL: %s\n",message);exit(1);}}
bool close(float a,float b) {return std::fabs(a-b)<2e-5f;}
int main() {
    Projection lens{500,500,319.5f,239.5f};
    Vec3 up{0,.6427876f,.7660444f},ground=scale(up,-1.4f);
    auto table=makeTabletop(up,ground,lens,0,0,640,480*.78f,.92f);
    expect(table.valid,"ground footprint is valid");
    expect(close(dot(table.right,table.forward),0),"four right-angle corners");
    expect(close(dot(table.up,table.right),0)&&close(dot(table.up,table.forward),0),"rectangle lies on ground");
    auto planes=table.planes();
    for(int x:{-1,1}) for(int z:{-1,1}) {
        auto p=table.point(x*table.halfWidth,z*table.halfDepth);
        expect(close(dot(p,table.up),dot(ground,table.up)),"corners share ground height");
        float sx=lens.centerX+lens.focalX*p.x/-p.z;
        float sy=lens.centerY-lens.focalY*p.y/-p.z;
        expect(sx>=0&&sx<=640&&sy>=0&&sy<=480*.78f,"rectangle remains inside native view");
        int edges=0;
        for(auto plane:planes) {expect(plane.distance(p)>-1e-5f,"corner not cut away");if(close(plane.distance(p),0)) ++edges;}
        expect(edges==2,"exactly two edges meet at each corner");
    }
    expect(planes[0].distance(table.point(-table.halfWidth-.01f,0))<0,"left clipping");
    expect(planes[1].distance(table.point(table.halfWidth+.01f,0))<0,"right clipping");
    expect(planes[2].distance(table.point(0,-table.halfDepth-.01f))<0,"near clipping");
    expect(planes[3].distance(table.point(0,table.halfDepth+.01f))<0,"far clipping");
    Pose camera{multiply(yawOnly({0,.3f,0,.9539392f}),pitch(-.8726646f)),{1,1.7f,2}};
    for(Pose eye: {Pose{{0,0,0,1},{.968f,1.7f,2}},Pose{pitch(-.4f),{1.15f,1.5f,1.8f}}}) {
        for(auto plane:planes) for(float h:{0.f,.35f}) {
            Vec3 source=table.point(.1f,.05f,h);
            auto converted=transformPlane(plane,camera,eye);
            expect(close(plane.distance(source),converted.distance(toLocal(eye,toWorld(camera,source)))),"clip boundary invariant under eye pose and wall height");
        }
    }
    auto smaller=makeTabletop(up,scale(ground,.5f),lens,0,0,640,480*.78f,.92f);
    expect(smaller.valid&&close(smaller.halfWidth,table.halfWidth*.5f)&&close(smaller.halfDepth,table.halfDepth*.5f),"scale controls preserve ground alignment");
    expect(!makeTabletop({0,0,0},ground,lens,0,0,640,480,.9f).valid,"invalid normal rejected");
    expect(!makeTabletop(up,scale(ground,-1),lens,0,0,640,480,.9f).valid,"ground behind camera rejected");
    expect(!makeTabletop(up,ground,lens,0,0,640,480,NAN).valid,"invalid crop rejected");
    // Regression: real in-game capture, not a synthetic positive-up camera.
    // DKII's camera is at native Z=-5.8793635368 above the Z=0 floor.
    Projection nativeLens{639.921875f,639.921875f,512.25f,322.25f};
    auto native=makeNativeTabletop({0,1.241805196f,.7807372212f},{46.68688965f,-80.42286682f,39.91866302f},nativeLens,512,322,10,.92f);
    expect(native.valid,"real DKII negative-Z-up camera activates tabletop");
    expect(native.up.y>0&&native.up.z>0,"native floor normal points upwards towards camera");
    expect(std::fabs(dot(native.up,native.center)+.58793635f)<.0001f,"native ground distance matches actual camera height");
    for(auto plane:native.planes()) {
        expect(plane.distance(native.center)>0,"real native floor center lies inside rectangle");
        expect(close(plane.distance(native.center),plane.distance(add(native.center,scale(native.up,.3f)))),"real native wall and floor share edge limits");
    }
    printf("Real DKII camera footprint: %.3fm x %.3fm, ground distance %.3fm.\n",native.halfWidth*2,native.halfDepth*2,dot(native.up,native.center));
    puts("PASS: rectangular ground footprint, four clip planes, stereo/head invariance, scale and invalid inputs.");
}
