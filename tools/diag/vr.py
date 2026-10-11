import sys,numpy as np
sys.path.insert(0,'/tmp/claude-0/geo')
from load import load
from PIL import Image
def readmaps(f):
    X={};B={}
    for l in open(f):
        p=l.strip().split(',')
        if p[0]=='X':X[int(p[1])]=p[2]
        if p[0]=='B':B[p[2]]=int(p[1])
    return X,B
def camfit(D):
    rows=[];
    for d in D:
        V=d['V']
        for v in V[::3]:
            x,y,rhw=v[0],v[1],v[2];p=v[3:6]
            if rhw<=0 or rhw>.9:continue
            w=1/rhw;P=np.r_[p,1]
            rows.append(np.r_[P,np.zeros(4),-x*P]);rows.append(np.r_[np.zeros(4),P,-y*P])
    A=np.array(rows);_,_,vt=np.linalg.svd(A,full_matrices=False);M=vt[-1].reshape(3,4)
    # camera centre = null of M
    _,_,vt2=np.linalg.svd(M);c=vt2[-1];c=c[:3]/c[3];return M,c
def render(f,texdir,tag,out,eye,target,up=np.array([0,1,0.]),fov=40,W=900,H=700):
    E,U,D=load(f);X,B=readmaps(f)
    texs={}
    def tex(k):
        if k not in texs:
            try:texs[k]=np.asarray(Image.open('%s/DK2VR_tex_%s_%04d_jeu.bmp'%(texdir,tag,k)).convert('RGBA')).astype(float)
            except Exception:texs[k]=None
        return texs[k]
    fw=target-eye;fw/=np.linalg.norm(fw);r=np.cross(fw,up);r/=np.linalg.norm(r);u2=np.cross(r,fw);foc=(W/2)/np.tan(np.radians(fov/2))
    zb=np.full((H,W),np.inf);img=np.zeros((H,W,3));img[:]=(30,30,45)
    def proj(P):
        q=P-eye;z=q@fw;return np.stack([W/2+foc*(q@r)/z,H/2-foc*(q@u2)/z,z],1)
    def raster(Pp,uv,col,mode,T):
        for t in T:
            if max(t)>=len(Pp):continue
            a,b,c=Pp[t];
            if min(a[2],b[2],c[2])<=.01:continue
            x0=max(0,int(min(a[0],b[0],c[0])));x1=min(W-1,int(max(a[0],b[0],c[0]))+1);y0=max(0,int(min(a[1],b[1],c[1])));y1=min(H-1,int(max(a[1],b[1],c[1]))+1)
            if x0>x1 or y0>y1 or (x1-x0)*(y1-y0)>W*H:continue
            Xg,Yg=np.meshgrid(np.arange(x0,x1+1)+.5,np.arange(y0,y1+1)+.5)
            den=(b[1]-c[1])*(a[0]-c[0])+(c[0]-b[0])*(a[1]-c[1])
            if abs(den)<1e-9:continue
            l1=((b[1]-c[1])*(Xg-c[0])+(c[0]-b[0])*(Yg-c[1]))/den;l2=((c[1]-a[1])*(Xg-c[0])+(a[0]-c[0])*(Yg-c[1]))/den;l3=1-l1-l2
            m=(l1>=0)&(l2>=0)&(l3>=0)
            if not m.any():continue
            iz=l1/a[2]+l2/b[2]+l3/c[2];z=1/iz
            sub=zb[y0:y1+1,x0:x1+1];m&=z<sub-1e-7
            if not m.any():continue
            reg=img[y0:y1+1,x0:x1+1]
            if mode==0:
                reg[m]=col[t[0]][:3];sub[m]=z[m];continue
            # perspective-correct uv
            w1,w2,w3=l1/a[2]/iz,l2/b[2]/iz,l3/c[2]/iz
            uu=w1*uv[t[0],0]+w2*uv[t[1],0]+w3*uv[t[2],0];vv=w1*uv[t[0],1]+w2*uv[t[1],1]+w3*uv[t[2],1]
            cc=w1[...,None]*col[t[0]]+w2[...,None]*col[t[1]]+w3[...,None]*col[t[2]]
            tx=mode[1]
            if tx is None: s=np.ones(uu.shape+(4,))*255
            else:
                th,tw=tx.shape[:2];s=tx[np.clip((vv*th).astype(int),0,th-1),np.clip((uu*tw).astype(int),0,tw-1)]
            rgb=s[...,:3]*cc[...,:3]/255.;al=s[...,3]*cc[...,3]/255./255.
            if mode[0]=='add': reg[m]=np.minimum(255,reg[m]+rgb[m])
            else: reg[m]=reg[m]*(1-al[m][:,None])+rgb[m]*al[m][:,None]
            if mode[2]: sub[m]=z[m]
    for pas in (0,1):
        for d in D:
            if (d['blend']!=0)!=(pas==1):continue
            V=d['V'];P=V[:,3:6];Pp=proj(P)
            col=np.array([[(int(c)>>16)&255,(int(c)>>8)&255,int(c)&255,(int(c)>>24)&255] for c in V[:,8]],float)
            if pas==0:
                n=None;sh=np.array([[150,140,120,255]]*len(V),float)
                raster(Pp,None,sh*0+np.c_[col[:,:3]*.6+40,col[:,3:]],0,d['T'])
            else:
                k=B.get(X.get(d['id']));t=tex(k) if k else None
                raster(Pp,V[:,6:8],col,('add' if d['dst']==2 else 'alpha',t,d['zw']),d['T'])
    Image.fromarray(img.clip(0,255).astype(np.uint8)).save(out)
