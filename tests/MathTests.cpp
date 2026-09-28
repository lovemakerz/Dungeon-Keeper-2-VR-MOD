#include "../src/common/Math.hpp"
#include <cstdio>
#include <cstdlib>
#include <cmath>
using namespace dk2vr;
void expect(bool condition,const char* message) {if(!condition) {printf("FAIL: %s\n",message);exit(1);}}
bool close(float a,float b){return std::fabs(a-b)<1e-5f;}
float ndcX(Vec3 p,Fov f) {float m[16];projectionMatrix(f,.01f,100,m);return (p.x*m[0]+p.z*m[8])/(-p.z);}
int main() {
    Projection lens{400,400,400,300};Vec3 p{};
    expect(unproject(600,400,.1f,lens,10,p),"unproject valid");
    expect(close(p.x,.5f)&&close(p.y,-.25f)&&close(p.z,-1),"native RHW metric reconstruction");
    expect(!unproject(0,0,0,lens,10,p),"reject zero RHW");
    expect(!unproject(0,0,NAN,lens,10,p),"reject NaN RHW");
    expect(!unproject(0,0,-1,lens,10,p),"reject negative RHW");
    Pose a{{0,0,0,1},{1,2,3}};p={.2f,.4f,-2};auto round=toLocal(a,toWorld(a,p));
    expect(close(round.x,p.x)&&close(round.y,p.y)&&close(round.z,p.z),"pose inverse translation");
    a.orientation=multiply(yawOnly({0,.38268343f,0,.92387953f}),pitch(-.7f));
    round=toLocal(a,toWorld(a,p));expect(close(round.x,p.x)&&close(round.y,p.y)&&close(round.z,p.z),"pose inverse rotation");
    Fov f{-Pi/4,Pi/4,Pi/4,-Pi/4};Vec3 target{0,0,-2};
    Pose left{{0,0,0,1},{-.032f,0,0}},right{{0,0,0,1},{.032f,0,0}};
    float lx=ndcX(toLocal(left,target),f),rx=ndcX(toLocal(right,target),f);
    expect(lx>0&&rx<0&&close(lx,-rx),"distinct stereo parallax and eye order");
    auto before=toLocal(left,target);left.position.x+=.1f;auto after=toLocal(left,target);
    expect(close(after.x,before.x-.1f),"head translation shifts geometry oppositely");
    auto down=rotate(pitch(-Pi/4),{0,0,-1});expect(down.y<0&&down.z<0,"table is below and ahead");
    Fov asymmetric{-.9f,.7f,.8f,-.6f};
    expect(close(ndcX({2*std::tan(asymmetric.left),0,-2},asymmetric),-1),"asymmetric left edge");
    expect(close(ndcX({2*std::tan(asymmetric.right),0,-2},asymmetric),1),"asymmetric right edge");
    float m[16];projectionMatrix(f,.01f,100,m);
    expect(close((-.01f*m[10]+m[14])/.01f,0),"near depth=0");
    expect(close((-100*m[10]+m[14])/100,1),"far depth=1");
    printf("PASS: stereo parallax, 6DOF transforms, native RHW, asymmetric FOV, depth, IPC layout (%zu bytes).\n",sizeof(Shared));
    return 0;
}
