// DK2VR 0.2.0 - table grab / two-hand rotate and zoom.
#include "../src/common/Grab.hpp"
#include <cstdio>
#include <cstdlib>
using namespace dk2vr;
static void expect(bool ok,const char* m){if(!ok){printf("FAIL: %s\n",m);exit(1);}}
static bool near3(Vec3 a,Vec3 b,float e=1e-4f){return std::fabs(a.x-b.x)<e&&std::fabs(a.y-b.y)<e&&std::fabs(a.z-b.z)<e;}
int main(){
    {   // 0.3.2 turntable: one hand on the right handle carried a quarter turn around the centre.
        TableGrab g;g.oneHandMoves=false;g.twoHandScales=false;g.oneHandTurns=true;
        Pose table{{0,0,0,1},{0,-.6f,-.8f}};float w=.8f;
        bool grip[2]={false,true};Pose hands[2]{};
        hands[1]={{0,0,0,1},{.46f,-.58f,-.8f}};
        g.update(grip,hands,table,w,.2f,6);
        hands[1].position={0,-.58f,-.8f-.46f};
        g.update(grip,hands,table,w,.2f,6);
        Vec3 right=rotate(table.orientation,{1,0,0});
        expect(near3(table.position,{0,-.6f,-.8f}),"turntable keeps the table in place");
        expect(near3(right,{0,0,-1},1e-3f),"turntable follows the hand around the centre");
    }
    TableGrab g;Pose table{{0,0,0,1},{0,-.6f,-.8f}};float width=.8f;
    bool grip[2]={false,true};Pose hands[2]={{{0,0,0,1},{-.2f,-.3f,-.4f}},{{0,0,0,1},{.2f,-.3f,-.4f}}};
    g.update(grip,hands,table,width,.2f,6);
    expect(near3(table.position,{0,-.6f,-.8f}),"grabbing does not jump");
    hands[1].position=add(hands[1].position,{.1f,.05f,-.2f});g.update(grip,hands,table,width,.2f,6);
    expect(near3(table.position,{.1f,-.55f,-1.f}),"one hand translates the table");
    hands[1].orientation=yawQuat(.5f);g.update(grip,hands,table,width,.2f,6);
    Vec3 fromHand=sub(table.position,hands[1].position),start=sub(Vec3{0,-.6f,-.8f},Vec3{.2f,-.3f,-.4f});
    expect(near3(fromHand,rotate(yawQuat(.5f),start)),"one hand turns the table around the hand");
    expect(std::fabs(yawAngle(table.orientation)-.5f)<1e-4f,"table yaw follows the hand");
    Vec3 up=rotate(table.orientation,{0,1,0});expect(near3(up,{0,1,0}),"table stays level");
    // Two hands: rotate 90 degrees and double the distance.
    grip[0]=true;table={{0,0,0,1},{0,-.6f,-.8f}};width=.8f;g.release();
    hands[0]={{0,0,0,1},{-.2f,-.3f,-.8f}};hands[1]={{0,0,0,1},{.2f,-.3f,-.8f}};
    g.update(grip,hands,table,width,.2f,6);
    hands[0].position={0,-.3f,-.8f+.4f};hands[1].position={0,-.3f,-.8f-.4f};
    g.update(grip,hands,table,width,.2f,6);
    expect(std::fabs(width-1.6f)<1e-4f,"hands twice as far apart doubles the table");
    expect(std::fabs(std::fabs(yawAngle(table.orientation))-Pi/2)<1e-3f,"two hands rotate the table");
    expect(near3(table.position,{0,-.6f,-.8f}),"table centred between the hands stays in place");
    float w=width;for(int i=0;i<3;++i){hands[0].position.z+=5;g.update(grip,hands,table,width,.2f,6);} 
    expect(width<=6.f&&width>=w,"size limit respected");
    grip[0]=grip[1]=false;expect(g.update(grip,hands,table,width,.2f,6),"release reported once");
    expect(!g.update(grip,hands,table,width,.2f,6),"no repeated release");
    // 0.2.1 mode: one hand does not move the table, two hands rotate/move without scaling.
    TableGrab m;m.oneHandMoves=false;m.twoHandScales=false;
    table={{0,0,0,1},{0,-.6f,-.8f}};width=.8f;bool one[2]={true,false};
    hands[0]={{0,0,0,1},{-.2f,-.3f,-.8f}};hands[1]={{0,0,0,1},{.2f,-.3f,-.8f}};
    m.update(one,hands,table,width,.2f,6);hands[0].position.x+=.3f;m.update(one,hands,table,width,.2f,6);
    expect(near3(table.position,{0,-.6f,-.8f}),"one hand leaves the table in place (map pan instead)");
    bool both[2]={true,true};hands[0].position={-.2f,-.3f,-.8f};
    m.update(both,hands,table,width,.2f,6);
    hands[0].position={.1f,-.2f,-.8f+.4f};hands[1].position={.1f,-.2f,-.8f-.4f};
    m.update(both,hands,table,width,.2f,6);
    expect(std::fabs(width-.8f)<1e-6f,"two hands keep the table size (native zoom instead)");
    expect(std::fabs(std::fabs(yawAngle(table.orientation))-Pi/2)<1e-3f,"two hands still rotate the table");
    expect(near3(table.position,{.1f,-.5f,-.8f}),"two hands still move the table with their midpoint");
    puts("PASS: grab translate, one-hand turn, level table, two-hand rotate/zoom, limits, release.");
}
