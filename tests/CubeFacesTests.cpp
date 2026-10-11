// DK2VR 0.8.84 - cube-map 360 view: the face builder used by the plugin (src/common/CubeFaces.hpp), pure geometry.
// The same code is also checked against the REAL engine code of DKII-DX.exe 1.70 (x86 emulation) by tools/re/cube_engine_check.py.
#include "../src/common/CubeFaces.hpp"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <random>
using namespace dk2vr;
static void expect(bool ok,const char* m) {if(!ok) {printf("FAIL: %s\n",m);exit(1);}}
static void rotAxis(const double ax[3],double a,double R[9]) {double n=std::sqrt(ax[0]*ax[0]+ax[1]*ax[1]+ax[2]*ax[2]);double x=ax[0]/n,y=ax[1]/n,z=ax[2]/n,c=std::cos(a),s=std::sin(a),C=1-c;
    double M[9]={c+x*x*C,x*y*C-z*s,x*z*C+y*s, y*x*C+z*s,c+y*y*C,y*z*C-x*s, z*x*C-y*s,z*y*C+x*s,c+z*z*C};for(int i=0;i<9;++i) R[i]=M[i];}
static void mul(const double A[9],const double B[9],double C[9]) {for(int r=0;r<3;++r) for(int c=0;c<3;++c) C[3*r+c]=A[3*r]*B[c]+A[3*r+1]*B[3+c]+A[3*r+2]*B[6+c];}
// engine projection of a world point for camera A (engine layout, columns = camera axes c0 right, c1 down, c2 forward) and the plugin's unprojection
static void ours(const float A[9],const double C[3],const double X[3],double out[3]) {
    double v[3]={X[0]-C[0],X[1]-C[1],X[2]-C[2]},c[3];for(int i=0;i<3;++i) c[i]=A[i]*v[0]+A[3+i]*v[1]+A[6+i]*v[2];
    const double f=300,cx=640,cy=360,sx=cx+f*c[0]/c[2],sy=cy+f*c[1]/c[2],rhw=1/c[2],d=1/rhw;
    out[0]=(sx-cx)*d/f;out[1]=-(sy-cy)*d/f;out[2]=-d;
}
int main() {
    std::mt19937 rng(5);std::uniform_real_distribution<double> U(-1,1);
    int cams=0;double worst=0;int tileBad=0,inside=0;
    for(int t=0;t<200;++t) {
        // a game camera: heading, downward pitch 0..60 degrees, roll +-6 degrees, both handedness conventions, world down = +Z or -Z
        const double yaw=U(rng)*3.14,pitch=(U(rng)+1)*.52,roll=U(rng)*.1,down=t%3==0?-1.:1.;
        const double fwd[3]={std::cos(yaw),std::sin(yaw),0},D[3]={0,0,down};
        double r[3]={D[1]*fwd[2]-D[2]*fwd[1],D[2]*fwd[0]-D[0]*fwd[2],D[0]*fwd[1]-D[1]*fwd[0]};
        double L[9]={r[0],D[0],fwd[0], r[1],D[1],fwd[1], r[2],D[2],fwd[2]};
        double Rp[9],Rr[9],T[9],A[9];const double ex[3]={1,0,0},ez[3]={0,0,1};rotAxis(ex,-pitch,Rp);rotAxis(ez,roll,Rr);mul(L,Rp,T);mul(T,Rr,A);
        if(t%2) for(int j=0;j<3;++j) A[3*j]=-A[3*j];   // mirrored convention
        float A0[9];for(int i=0;i<9;++i) A0[i]=float(A[i]);
        CubeFaces cf;expect(buildCubeFaces(A0,cf),"a game camera gives a cube");++cams;
        // faces: rotations with the handedness of the game camera, 4 level faces + up + down
        for(int k=0;k<6;++k) {
            const float* F=cf.A[k];double det=F[0]*(F[4]*F[8]-F[5]*F[7])-F[1]*(F[3]*F[8]-F[5]*F[6])+F[2]*(F[3]*F[7]-F[4]*F[6]);
            double d0=A[0]*(A[4]*A[8]-A[5]*A[7])-A[1]*(A[3]*A[8]-A[5]*A[6])+A[2]*(A[3]*A[7]-A[4]*A[6]);
            expect(std::fabs(det-d0)<1e-4,"face handedness = game camera handedness");
            const double fz=F[8]*down;   // forward . world down
            if(k<4) expect(std::fabs(fz)<1e-5,"faces 0..3 are level");
            if(k==4) expect(fz<-.9999,"face 4 looks straight up");
            if(k==5) expect(fz>.9999,"face 5 looks straight down");
        }
        // Q: a world point seen by face k lands where the front camera puts it
        const double C[3]={3,4,-1};
        for(int k=0;k<6;++k) for(int n=0;n<20;++n) {
            const double cc[3]={U(rng)*.95,U(rng)*.95,1.};const double s=.3+3*(U(rng)+1);
            double X[3];for(int j=0;j<3;++j) X[j]=C[j]+s*(cf.A[k][3*j]*cc[0]+cf.A[k][3*j+1]*cc[1]+cf.A[k][3*j+2]*cc[2]);
            double pf[3],p0[3];ours(cf.A[k],C,X,pf);ours(A0,C,X,p0);
            double q[3];for(int i=0;i<3;++i) q[i]=cf.Q[k][3*i]*pf[0]+cf.Q[k][3*i+1]*pf[1]+cf.Q[k][3*i+2]*pf[2];
            double e=0,m=0;for(int i=0;i<3;++i) {e=std::fmax(e,std::fabs(q[i]-p0[i]));m=std::fmax(m,std::fabs(p0[i]));}
            worst=std::fmax(worst,e/m);
            bool in=true;for(int i=0;i<4;++i) if(cf.N[k][i][0]*p0[0]+cf.N[k][i][1]*p0[1]+cf.N[k][i][2]*p0[2]<-1e-5) in=false;
            if(in) ++inside;
        }
        // tiling of the sphere (front "ours" space): every direction belongs to exactly one face pyramid
        for(int n=0;n<300;++n) {
            double v[3]={U(rng),U(rng),U(rng)};int cnt=0;
            for(int k=0;k<6;++k) {bool in=true;for(int i=0;i<4;++i) if(cf.N[k][i][0]*v[0]+cf.N[k][i][1]*v[1]+cf.N[k][i][2]*v[2]<0) in=false;if(in) ++cnt;}
            if(cnt!=1) ++tileBad;
        }
    }
    expect(worst<1e-5,"face -> front mapping exact");
    expect(inside==cams*6*20,"every face point lies in its own pyramid");
    expect(tileBad==0,"the 6 pyramids tile the sphere exactly once");
    // a camera looking straight up / down has no heading: refused (the reference view stays)
    {const float up[9]={1,0,0, 0,1,0, 0,0,1};CubeFaces cf;expect(!buildCubeFaces(up,cf),"vertical camera refused");}
    {const float bad[9]={2,0,0, 0,1,0, 0,0,1};CubeFaces cf;expect(!buildCubeFaces(bad,cf),"non-rotation refused");}
    // face lens: square inside the game's rectangle, centred, 90 degrees + guard
    {const float rect[4]={0,1280,0,720};float f,o[4];expect(cubeFaceLens(rect,1.04f,f,o),"face lens");
     expect(std::fabs(o[0]-280)<1e-3&&std::fabs(o[1]-1000)<1e-3&&std::fabs(o[2])<1e-3&&std::fabs(o[3]-720)<1e-3&&std::fabs(f*1.04f-360)<1e-2,"square rectangle centred in the screen");}
    {   // 0.8.99.1 ADAPTIVE 360: faces needed by the headset view. Game camera 45 degrees down, unmirrored, world down +Z, heading +X.
        const double pitch=.7853981634;const double fwd[3]={1,0,0},D[3]={0,0,1};
        double r[3]={D[1]*fwd[2]-D[2]*fwd[1],D[2]*fwd[0]-D[0]*fwd[2],D[0]*fwd[1]-D[1]*fwd[0]};
        double L[9]={r[0],D[0],fwd[0], r[1],D[1],fwd[1], r[2],D[2],fwd[2]};
        double Rp[9],A[9];const double ex[3]={1,0,0};rotAxis(ex,-pitch,Rp);mul(L,Rp,A);
        float A0[9];for(int i=0;i<9;++i) A0[i]=float(A[i]);
        CubeFaces cf;expect(buildCubeFaces(A0,cf),"45 degree camera gives a cube");
        const float d2r=.0174532925f;
        // eye turned by yaw (about up) and pitch (about right) from the LEVEL forward direction; front "ours" = level forward pitched 45 degrees up
        auto eyeTo=[&](float yawDeg,float pitchDeg){return [=](const float e[3],float o[3]){
            const float cy=std::cos(yawDeg*d2r),sy=std::sin(yawDeg*d2r),cp=std::cos(pitchDeg*d2r),sp=std::sin(pitchDeg*d2r);
            // pitch about x, then yaw about y (yaw > 0 = to the right: -z turns to +x)
            const float y1=e[1]*cp-e[2]*sp,z1=e[1]*sp+e[2]*cp,x1=e[0];
            const float x2=x1*cy-z1*sy,z2=x1*sy+z1*cy;
            // level frame -> front camera frame: the camera looks 45 degrees down, so a level direction is 45 degrees above its axis
            const float c45=float(std::cos(pitch)),s45=float(std::sin(pitch));
            o[0]=x2;o[1]=y1*c45-z2*s45;o[2]=y1*s45+z2*c45;};};
        const float fl=-50*d2r,fr=50*d2r,fu=48*d2r,fd=-48*d2r;
        const float tx=512.f/176.f,ty=384.f/176.f;
        auto covered=[&](const float d[3]){return frontViewCovers(d,tx,ty,.92f);};
        const unsigned all=cubeFacesReached(cf.N,fl,fr,fu,fd,15*d2r,eyeTo(0,0));
        const unsigned need=cubeFacesReached(cf.N,fl,fr,fu,fd,15*d2r,eyeTo(0,0),covered);
        const unsigned back=cubeFacesReached(cf.N,fl,fr,fu,fd,15*d2r,eyeTo(180,0),covered);
        const unsigned right=cubeFacesReached(cf.N,fl,fr,fu,fd,15*d2r,eyeTo(90,0),covered);
        const unsigned down=cubeFacesReached(cf.N,fl,fr,fu,fd,15*d2r,eyeTo(0,-60),covered);
        printf("adaptive 360 (45 degree game camera, 100x96 degree eye, margin 15): looking ahead reaches faces %02X, needs %02X; behind needs %02X; right %02X; down %02X\n",all,need,back,right,down);
        expect(!(all&4),"looking ahead never reaches the back face");
        expect((need&4)==0,"looking ahead: no back face");
        expect((need&32)==0,"looking ahead: the floor below is covered by the normal view (45 degrees down, 130 degrees tall)");
        expect(need&16,"looking ahead: the ceiling above the normal view needs the up face");
        expect((back&4)&&!(back&1),"looking behind needs the back face, not the front one");
        expect(right&2,"looking right needs the right face");
        expect(!(down&16)&&!(down&1),"looking down: neither the up face nor the front face (the normal view covers it)");
        // every direction of the sphere is in one face at least
        for(int i=0;i<500;++i) {float d[3]={float(U(rng)),float(U(rng)),float(U(rng))};int n=0;
            for(int k=0;k<6;++k) {bool in=true;for(int p=0;p<4&&in;++p) in=cf.N[k][p][0]*d[0]+cf.N[k][p][1]*d[1]+cf.N[k][p][2]*d[2]>=-1e-5f;n+=in;}
            expect(n>=1,"every direction belongs to a face");}
    }
    printf("PASS: cube map: %d game cameras, faces level / up / down with the game handedness, face->front mapping error %.1e, sphere tiled exactly once\n",cams,worst);
    return 0;
}
