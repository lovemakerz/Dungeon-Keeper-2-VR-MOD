#pragma once
// DK2VR 0.8.84 - 360 degree first-person view as a CUBE MAP of the game's own renderer.
//
// The game draws ONE perspective view. A complete sphere (ceiling and floor included) is the union of 6 perspective views of exactly 90 degrees
// (front, right, back, left, up, down), all from the same point. Each face is a normal camera handed to the engine's own view setup (0x575A00):
// position unchanged, orientation = the face, focal f and a SQUARE screen rectangle of half size f*guard about the original centre.
//
// Engine conventions (verified on DKII-DX.exe 1.70, see tools/re/cube_engine_check.py, which runs the real engine code in an emulator):
//  * camera struct +0xC: 9 floats, triplet j = world axis j, entry i = camera axis i (camera -> world: X - C = A c). Camera axes:
//    c0 = screen right, c1 = screen DOWN, c2 = forward (depth).
//  * clip = N A^T (X - C) with N = diag(f/halfWidth, f/halfHeight, 1); the engine keeps |clip.x| <= clip.z, |clip.y| <= clip.z (0x58AF70);
//    screen x = (cx - .5) + f c0/c2, screen y = (cy - .5) + f c1/c2, rhw = K / c2 (0x58ADC0): with a square rectangle of half size f*guard
//    the face covers exactly atan(guard) about its axis.
//  * the plugin's unprojection (Math.hpp unproject) gives "ours" = k (c0, -c1, -c2) = k S c with S = diag(1,-1,-1).
// Therefore a point recorded by face k maps into the space of the FRONT camera A0 (the space of every reference record) by
//     ours_front = Q_k ours_face,   Q_k = S A0^T A_k S        (exact; no heading readout, no learned scale)
// and the face keeps only its own pyramid: in face "ours" coordinates  -z >= |x|  and  -z >= |y|  (4 planes through the camera), mapped by Q_k.
// The 6 pyramids tile the sphere exactly once: the seams are planes shared by two faces.
#include <cmath>

namespace dk2vr {

struct CubeFaces {
    float A[6][9]{};     // face matrices in the engine layout (index 3*j+i: world axis j, camera axis i)
    float Q[6][9]{};     // face "ours" -> front "ours", row major (out_r = sum_c Q[3r+c] in_c)
    float N[6][4][3]{};  // the 4 inward plane normals of each face pyramid, in FRONT "ours" coordinates (planes through the camera)
    float down[3]{};     // world down axis of the engine (+-Z), for the log
    bool valid=false;
};

namespace cube_detail {
inline void col(const float A[9],int i,float v[3]) {v[0]=A[i];v[1]=A[3+i];v[2]=A[6+i];}
inline void setCol(float A[9],int i,const float v[3]) {A[i]=v[0];A[3+i]=v[1];A[6+i]=v[2];}
inline float dot3(const float a[3],const float b[3]) {return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];}
inline void cross3(const float a[3],const float b[3],float o[3]) {o[0]=a[1]*b[2]-a[2]*b[1];o[1]=a[2]*b[0]-a[0]*b[2];o[2]=a[0]*b[1]-a[1]*b[0];}
inline float det3(const float A[9]) {return A[0]*(A[4]*A[8]-A[5]*A[7])-A[1]*(A[3]*A[8]-A[5]*A[6])+A[2]*(A[3]*A[7]-A[4]*A[6]);}
// C = A B (3x3, row major)
inline void mul(const float A[9],const float B[9],float C[9]) {for(int r=0;r<3;++r) for(int c=0;c<3;++c) C[3*r+c]=A[3*r]*B[c]+A[3*r+1]*B[3+c]+A[3*r+2]*B[6+c];}
inline void transpose(const float A[9],float T[9]) {for(int r=0;r<3;++r) for(int c=0;c<3;++c) T[3*c+r]=A[3*r+c];}
}

// Builds the 6 faces from the game's own camera matrix A0 (engine layout). Faces are LEVEL (relative to the world vertical), turned with the
// heading of the game camera: face 0 looks along the horizontal direction of the game camera, 1..3 turn by 90 degrees about the vertical,
// 4 looks up, 5 looks down. Returns false if A0 is not a rotation (or looks straight up / down: no heading).
inline bool buildCubeFaces(const float A0[9],CubeFaces& out) {
    using namespace cube_detail;
    out.valid=false;
    for(int i=0;i<9;++i) if(!std::isfinite(A0[i])) return false;
    float a1[3],a2[3];col(A0,1,a1);col(A0,2,a2);
    for(int i=0;i<3;++i) {float v[3];col(A0,i,v);const float n=dot3(v,v);if(!(n>.9f&&n<1.1f)) return false;}
    const float dt=det3(A0);if(!(std::fabs(std::fabs(dt)-1.f)<.1f)) return false;
    const float sgn=dt>0?1.f:-1.f;
    // world down: the vertical axis (Z) on the side of the camera's screen-down axis
    const float D[3]={0,0,a1[2]>=0?1.f:-1.f};
    float fh[3]={a2[0],a2[1],0};const float l=std::sqrt(fh[0]*fh[0]+fh[1]*fh[1]);
    if(l<.05f) return false;
    fh[0]/=l;fh[1]/=l;
    float r[3];cross3(D,fh,r);for(float& x:r) x*=sgn;   // det [r D fh] = sgn: same handedness as the game camera
    float Lv[9];setCol(Lv,0,r);setCol(Lv,1,D);setCol(Lv,2,fh);
    // rotations in camera coordinates (columns = images of the camera axes), det +1
    const float I[9]={1,0,0, 0,1,0, 0,0,1};
    const float Ry[9]={0,0,1, 0,1,0, -1,0,0};    // about the down axis: forward -> right (columns e0->(0,0,-1), e1->e1, e2->(1,0,0))
    const float Rp[9]={1,0,0, 0,0,-1, 0,1,0};    // about the right axis: forward -> -down = UP (columns e0->e0, e1->(0,0,1), e2->(0,-1,0))
    float Rk[6][9];
    for(int i=0;i<9;++i) Rk[0][i]=I[i];
    mul(Rk[0],Ry,Rk[1]);mul(Rk[1],Ry,Rk[2]);mul(Rk[2],Ry,Rk[3]);
    for(int i=0;i<9;++i) Rk[4][i]=Rp[i];
    transpose(Rp,Rk[5]);
    const float S[9]={1,0,0, 0,-1,0, 0,0,-1};
    float A0t[9];transpose(A0,A0t);
    float SA0t[9];mul(S,A0t,SA0t);
    // face pyramid, inward normals in face "ours" coordinates (x right, y up, -z forward): -z >= |x|, -z >= |y|
    const float h=.70710678f;
    const float pn[4][3]={{-h,0,-h},{h,0,-h},{0,-h,-h},{0,h,-h}};
    for(int k=0;k<6;++k) {
        mul(Lv,Rk[k],out.A[k]);
        float t[9],q[9];mul(SA0t,out.A[k],t);mul(t,S,q);
        for(int i=0;i<9;++i) out.Q[k][i]=q[i];
        for(int p=0;p<4;++p) for(int rr=0;rr<3;++rr) out.N[k][p][rr]=q[3*rr]*pn[p][0]+q[3*rr+1]*pn[p][1]+q[3*rr+2]*pn[p][2];
    }
    for(int i=0;i<3;++i) out.down[i]=D[i];
    out.valid=true;return true;
}

// Square screen rectangle of a face: left, right, top, bottom, centred on the original rectangle's centre. The focal is the largest that keeps
// the square (with its guard band) inside the original rectangle, so every screen coordinate the engine produces stays inside the game's screen.
inline bool cubeFaceLens(const float rect[4],float guard,float& focal,float out[4]) {
    if(!(rect[1]>rect[0]&&rect[3]>rect[2])||!(guard>=1.f)) return false;
    const float cx=(rect[0]+rect[1])*.5f,cy=(rect[2]+rect[3])*.5f;
    const float half=std::fmin(rect[1]-rect[0],rect[3]-rect[2])*.5f;
    focal=half/guard;if(!(focal>16.f)) return false;
    out[0]=cx-half;out[1]=cx+half;out[2]=cy-half;out[3]=cy+half;
    return true;
}

// 0.8.99.1 ADAPTIVE 360: which cube faces a view can reach. The view is the angular rectangle [left - margin, right + margin] x [down - margin, up + margin]
// (radians, left / down negative, OpenXR field of view) about the -z axis of an eye; toFront(e, d) turns an eye direction e into the space of the face
// normals N (face pyramids, inward normals, planes through the camera). Bit k = face k (0 front, 1 right, 2 back, 3 left, 4 up, 5 down). Directions are
// sampled on a (steps+1)^2 grid: with steps 12 over at most 180 degrees a sample every 15 degrees, far finer than a 90 degree face.
// skip(d): directions that need no face (already covered by the normal view) are left out.
template<class ToFront,class Skip> inline unsigned cubeFacesReached(const float N[6][4][3],float left,float right,float up,float down,float margin,ToFront toFront,Skip skip,int steps=12) {
    unsigned mask=0;
    const float a0=left-margin,a1=right+margin,b0=down-margin,b1=up+margin;
    for(int i=0;i<=steps;++i) for(int j=0;j<=steps;++j) {
        const float a=a0+(a1-a0)*float(i)/float(steps);
        const float b=std::fmax(-1.5707f,std::fmin(1.5707f,b0+(b1-b0)*float(j)/float(steps)));
        const float e[3]={std::sin(a)*std::cos(b),std::sin(b),-std::cos(a)*std::cos(b)};
        float d[3];toFront(e,d);
        if(skip(d)) continue;
        for(int k=0;k<6;++k) {
            bool in=true;
            for(int p=0;p<4&&in;++p) in=N[k][p][0]*d[0]+N[k][p][1]*d[1]+N[k][p][2]*d[2]>=-1e-3f;
            if(in) mask|=1u<<k;
        }
    }
    return mask;
}
template<class ToFront> inline unsigned cubeFacesReached(const float N[6][4][3],float left,float right,float up,float down,float margin,ToFront toFront,int steps=12) {
    return cubeFacesReached(N,left,right,up,down,margin,toFront,[](const float*){return false;},steps);
}
// the normal (front) view of the game covers a direction d of its own "ours" space (x right, y up, -z forward) when it lies inside its frustum,
// tan half angles tx, ty, shrunk by 'inner' (< 1) so that a direction on the edge is not taken for covered
inline bool frontViewCovers(const float d[3],float tx,float ty,float inner) {
    if(!(d[2]<-1e-4f)) return false;
    return std::fabs(d[0])<=-d[2]*tx*inner&&std::fabs(d[1])<=-d[2]*ty*inner;
}

}  // namespace dk2vr
