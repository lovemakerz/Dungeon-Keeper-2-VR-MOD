import sys;sys.path.insert(0,'/tmp/claude-0/geo')
from load import load
import numpy as np
f=sys.argv[1];want=sys.argv[2]
E,U,D=load(f)
X={};B={}
for l in open(f):
    p=l.strip().split(',')
    if p[0]=='X':X[int(p[1])]=p[2]
    if p[0]=='B':B[p[2]]=p[1].lstrip('0')
ptr=[k for k,v in B.items() if v==want][0]
op=[d for d in D if not d['blend']]
bl=[d for d in D if X.get(d['id'])==ptr and d['blend']]
print('opaque draws with this texture',len(op),[ (d['id'],d['n']) for d in op][:10]);print('blended',len(bl))
tris=[]
for d in op:
    P=d['V'][:,3:6]
    for t in d['T']:
        if max(t)<len(P): tris.append(P[t])
tris=np.array(tris)
def pdist(p):
    # distance point to plane of each tri if projection inside, else large
    a,b,c=tris[:,0],tris[:,1],tris[:,2]
    n=np.cross(b-a,c-a);nl=np.linalg.norm(n,axis=1);ok=nl>1e-12;n=n/np.where(ok,nl,1)[:,None]
    dpl=np.einsum('ij,ij->i',p-a,n);q=p-dpl[:,None]*n
    def side(u,v,w):return np.einsum('ij,ij->i',np.cross(v-u,q-u),n)
    inside=(side(a,b,c)>=-1e-9)&(side(b,c,a)>=-1e-9)&(side(c,a,b)>=-1e-9)&ok
    dd=np.where(inside,np.abs(dpl),np.inf)
    i=np.argmin(dd);return dd[i],dpl[i] if np.isfinite(dd[i]) else 0
for d in bl[:25]:
    P=d['V'][:,3:6];ds=[pdist(p) for p in P]
    vals=[x[0] for x in ds]
    print(d['id'],'n',d['n'],'dist mm: min %.1f med %.1f max %.1f'%(1000*min(vals),1000*np.median(vals),1000*max(vals)),'signed',np.round([1000*x[1] for x in ds][:6],1))
