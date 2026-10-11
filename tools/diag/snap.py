import sys;sys.path.insert(0,'/tmp/claude-0/geo')
from load import load
import numpy as np
f=sys.argv[1];E,U,D=load(f)
GW,GH=192,144;gx,gy=GW/1024.,GH/768.
M=np.zeros((GH,GW))
X={};B={}
for l in open(f):
    p=l.strip().split(',')
    if p[0]=='X':X[int(p[1])]=p[2]
    if p[0]=='B':B[p[2]]=int(p[1])
tot=dict(draws=0,cand=0,moved=0);bytex={}
for d in D:
    V=d['V'];T=d['T']
    if d['blend']==0:
        for t in T:
            if max(t)>=len(V):continue
            A,Bv,C=V[t[0]],V[t[1]],V[t[2]]
            if not all(0<q[2]<.9 for q in (A,Bv,C)):continue
            ax,ay,bx,by,cx,cy=A[0]*gx,A[1]*gy,Bv[0]*gx,Bv[1]*gy,C[0]*gx,C[1]*gy
            den=(by-cy)*(ax-cx)+(cx-bx)*(ay-cy)
            if abs(den)<1e-6:continue
            x0=max(0,int(np.floor(min(ax,bx,cx))));x1=min(GW-1,int(np.ceil(max(ax,bx,cx))));y0=max(0,int(np.floor(min(ay,by,cy))));y1=min(GH-1,int(np.ceil(max(ay,by,cy))))
            if x0>x1 or y0>y1:continue
            xs,ys=np.meshgrid(np.arange(x0,x1+1)+.5,np.arange(y0,y1+1)+.5)
            l1=((by-cy)*(xs-cx)+(cx-bx)*(ys-cy))/den;l2=((cy-ay)*(xs-cx)+(ax-cx)*(ys-cy))/den;l3=1-l1-l2
            m=(l1>=-.02)&(l2>=-.02)&(l3>=-.02);w=l1*A[2]+l2*Bv[2]+l3*C[2]
            sub=M[y0:y1+1,x0:x1+1];upd=m&(w>sub);sub[upd]=w[upd]
    elif d['src']==5 and d['dst']==6:
        fl=np.zeros(len(V),int)
        for t in T:
            if max(t)>=len(V):continue
            r=V[t,2];bit=2 if (r.max()-r.min())<=.004*r.max() else 1
            fl[t]|=bit
        c=m=0
        for i in range(len(V)):
            if fl[i]!=1:continue
            c+=1;xi,yi=int(V[i,0]*gx),int(V[i,1]*gy)
            if not(0<=xi<GW and 0<=yi<GH):continue
            sr=M[yi,xi]
            if sr>0 and V[i,2]>sr*1.0005 and V[i,2]<=sr*1.15:m+=1
        if m:tot['draws']+=1
        tot['cand']+=c;tot['moved']+=m
        k=B.get(X.get(d['id']));bytex.setdefault(k,[0,0]);bytex[k][0]+=c;bytex[k][1]+=m
print(tot);print(sorted(bytex.items(),key=lambda x:-x[1][1])[:15])
