import sys,numpy as np
sys.path.insert(0,'/tmp/claude-0/geo')
from load import load
from PIL import Image
def render(f,out,target=None,fovdeg=50,W=900,H=700,eye=None,highlight=None,only=None,blendAlpha=.55):
    E,U,D=load(f)
    if eye is not None:E=np.array(eye)
    if target is None:
        P=np.concatenate([d['V'][:,3:6] for d in D if d['blend']==0 and len(d['V'])])
        target=P.mean(0)
    fw=np.array(target)-E;fw/=np.linalg.norm(fw);r=np.cross(fw,U);r/=np.linalg.norm(r);up=np.cross(r,fw)
    foc=(W/2)/np.tan(np.radians(fovdeg/2))
    zb=np.full((H,W),np.inf);img=np.zeros((H,W,3));img[:]=(40,40,60)
    def proj(P):
        q=P-E;z=q@fw;x=q@r;y=q@up
        return np.stack([W/2+foc*x/z,H/2-foc*y/z,z],1)
    def tri(a,b,c,col,alpha,zt=True,zwrite=True):
        xs=[a[0],b[0],c[0]];ys=[a[1],b[1],c[1]]
        if min(a[2],b[2],c[2])<=0.01:return
        x0=max(0,int(np.floor(min(xs))));x1=min(W-1,int(np.ceil(max(xs))));y0=max(0,int(np.floor(min(ys))));y1=min(H-1,int(np.ceil(max(ys))))
        if x0>x1 or y0>y1:return
        X,Y=np.meshgrid(np.arange(x0,x1+1)+.5,np.arange(y0,y1+1)+.5)
        den=(b[1]-c[1])*(a[0]-c[0])+(c[0]-b[0])*(a[1]-c[1])
        if abs(den)<1e-9:return
        l1=((b[1]-c[1])*(X-c[0])+(c[0]-b[0])*(Y-c[1]))/den;l2=((c[1]-a[1])*(X-c[0])+(a[0]-c[0])*(Y-c[1]))/den;l3=1-l1-l2
        m=(l1>=0)&(l2>=0)&(l3>=0)
        if not m.any():return
        iz=l1/a[2]+l2/b[2]+l3/c[2];z=1/iz
        sub=zb[y0:y1+1,x0:x1+1];m&=z<sub-1e-6 if zt else m
        if not m.any():return
        reg=img[y0:y1+1,x0:x1+1]
        reg[m]=reg[m]*(1-alpha)+np.array(col)*alpha
        if zwrite:sub[m]=z[m]
    # opaque
    for d in D:
        if d['blend'] or only is not None: 
            if only is None or d['id'] not in only or d['blend']: continue
        Pp=proj(d['V'][:,3:6]);P=d['V'][:,3:6]
        for t in d['T']:
            if max(t)>=len(P):continue
            n=np.cross(P[t[1]]-P[t[0]],P[t[2]]-P[t[0]]);nl=np.linalg.norm(n)
            sh=.35+.65*abs(n@U)/nl if nl>0 else .5
            colv=int(d["V"][t[0],8]);cc=np.array([(colv>>16)&255,(colv>>8)&255,colv&255])*.5+60
            tri(Pp[t[0]],Pp[t[1]],Pp[t[2]],cc*sh,1.)
    for d in D:
        if not d['blend']:continue
        if only is not None and d['id'] not in only:continue
        Pp=proj(d['V'][:,3:6])
        col=(255,220,40) if d['dst']==2 else (40,220,255)
        if highlight is not None and d['id'] in highlight: col=(255,0,255)
        for t in d['T']:
            if max(t)>=len(Pp):continue
            tri(Pp[t[0]],Pp[t[1]],Pp[t[2]],col,blendAlpha,zwrite=False)
    Image.fromarray(img.clip(0,255).astype(np.uint8)).save(out)
    return E,U,D,proj
if __name__=='__main__':
    render(sys.argv[1],sys.argv[2])
