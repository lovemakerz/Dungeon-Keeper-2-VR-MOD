# DK2VR 0.8.84 - offline proof of the cube-map 360 view against the REAL engine code of DKII-DX.exe 1.70 (x86 emulator, unicorn).
# Uses the plugin's own face builder (src/common/CubeFaces.hpp, compiled as cubecli) and checks, for random realistic game cameras:
#  1. the engine accepts each face camera: view setup 0x575A00 + tile selection 0x574820 give a non-empty tile hull for all 6 faces;
#  2. a world point seen by face k, projected by the engine (0x58A6F0 globals + projection 0x58ADC0), unprojected like the plugin and mapped
#     by Q_k lands exactly where the FRONT camera's own projection/unprojection puts it;
#  3. the face pyramids (planes N) contain the point, and the 6 pyramids tile the sphere exactly once.
import sys,os,subprocess,math,struct
import numpy as np
sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
from dk2emu import *
SLAB=[-9.0,0.0]   # engine world height slab (0x760AA8, 0x760B4C), set at level start by 0x574310 (called from 0x598443 with -9.0, 0.0)
CLI=os.path.join(os.path.dirname(os.path.abspath(__file__)),'cubecli')
def camstruct(e,C,A9,f,rect,far):
    p=e.HEAP+0x800000;e.f32(0x760aa8,SLAB[0]);e.f32(0x760b4c,SLAB[1])
    b=struct.pack('<12f',*(list(C)+list(A9)))+struct.pack('<f',f)+struct.pack('<4f',*rect)+struct.pack('<2f',0,0)+struct.pack('<f',far)+struct.pack('<I',0)
    e.u.mem_write(p,b);return p
def faces_cpp(A0,rect,guard):
    out=subprocess.run([CLI]+['%.9g'%x for x in A0.reshape(-1)]+['%.9g'%x for x in rect]+['%.9g'%guard],capture_output=True,text=True).stdout.split('\n')
    ok=out[0].strip()=='1';F=[]
    for k in range(6):
        v=np.array([float(x) for x in out[1+k].split()]);F.append((v[:9].reshape(3,3),v[9:18].reshape(3,3),v[18:30].reshape(4,3)))
    lo=[float(x) for x in out[7].split()]
    return ok,F,lo[1],lo[2:6]
def setup(C,A,f,rect,far=15.):
    e=Emu();p=camstruct(e,C,A.reshape(-1),f,rect,far)
    hull=e.call(0x575a00,(p,0x3f800000),stop_at=0x575c10)
    pts=[];n=hull;seen=set()
    while n and n not in seen: seen.add(n);pts.append((e.i32(n),e.i32(n+4)));n=struct.unpack('<I',e.u.mem_read(n+0xc,4))[0]
    e.u.emu_start(0x575c10,0x575c1c)   # continue to the end of the projection setup 0x58A6F0
    return e,pts
def project(e,Xw):
    M=np.array([e.f32(0x77f458+4*i) for i in range(9)]).reshape(3,3);t=np.array([e.f32(0x780940+4*i) for i in range(3)])
    clip=Xw@M+t;v=e.HEAP+0x900000;e.u.mem_write(v,struct.pack('<3f',*clip));e.call(0x58adc0,(0,v))
    return e.f32(0x76f358+0xc),e.f32(0x76f358+0x10),e.f32(0x76f358+0x18)
def unproject(e,sx,sy,rhw,units=1.):
    f,cx,cy=e.f32(0x77f430),e.f32(0x77f4f0),e.f32(0x77f930);d=1/(rhw*units);return np.array([(sx-cx)*d/f,-(sy-cy)*d/f,-d])
def rot(axis,ang):
    axis=np.asarray(axis,float);axis/=np.linalg.norm(axis);x,y,z=axis;c,s=math.cos(ang),math.sin(ang);C=1-c
    return np.array([[c+x*x*C,x*y*C-z*s,x*z*C+y*s],[y*x*C+z*s,c+y*y*C,y*z*C-x*s],[z*x*C-y*s,z*y*C+x*s,c+z*z*C]])
rng=np.random.default_rng(7);worst=0;worstPlane=float("inf");tile_bad=0;hulls=[]
rect0=(0.,1280.,0.,720.);guard=1.04
for trial in range(6):
    yaw=rng.uniform(-3.1,3.1);pitch=math.radians(rng.choice([45.,30.,10.]));roll=math.radians(rng.uniform(-5,5))
    fwd=np.array([math.cos(yaw),math.sin(yaw),0.]);D=np.array([0,0,1.]);r=np.cross(D,fwd);Lg=np.column_stack([r,D,fwd])
    A0=Lg@rot([1,0,0],-pitch)@rot([0,0,1],roll)
    if trial%2: A0=A0@np.diag([-1,1,1.])
    C=np.array([rng.uniform(5,40),rng.uniform(5,40),rng.uniform(-2.5,-.3)])
    ok,F,fF,rectF=faces_cpp(A0,rect0,guard);assert ok
    e0,_=setup(C,A0,97.,rect0)
    for k,(Ak,Qk,Nk) in enumerate(F):
        ek,pts=setup(C,Ak,fF,rectF)
        hulls.append(len(pts))
        if not pts: tile_bad+=1
        for _ in range(25):
            c=np.array([rng.uniform(-1,1),rng.uniform(-1,1),1.])*rng.uniform(.2,7)   # inside the face pyramid
            Xw=C+Ak@c
            pf=unproject(ek,*project(ek,Xw));p0=unproject(e0,*project(e0,Xw))
            worst=max(worst,np.abs(Qk@pf-p0).max()/max(1e-6,np.abs(p0).max()))
            worstPlane=min(worstPlane,(Nk@(Qk@pf)).min()/np.linalg.norm(p0))
# sphere tiling with the planes of one camera
ok,F,_,_=faces_cpp(A0,rect0,guard);count_bad=0
for _ in range(20000):
    d=rng.normal(size=3);d/=np.linalg.norm(d)
    n=sum(1 for (_,_,Nk) in F if (Nk@d).min()>=0)
    if n!=1: count_bad+=1
print('faces accepted by the engine tile selection: %d/%d non-empty hulls (sizes %d..%d)'%(len(hulls)-tile_bad,len(hulls),min(hulls),max(hulls)))
print('face -> front mapping through the REAL engine projection: worst relative error %.2e'%worst)
print('points of each face inside its own pyramid (min normalised plane distance %.2e, >= -1e-5 expected)'%worstPlane)
print('sphere tiling: %d / 20000 random directions not covered exactly once (boundary hits only)'%count_bad)
ok_all=tile_bad==0 and worst<1e-4 and worstPlane>-1e-5 and count_bad<=2
print('RESULT:', 'PASS' if ok_all else 'FAIL');sys.exit(0 if ok_all else 1)
