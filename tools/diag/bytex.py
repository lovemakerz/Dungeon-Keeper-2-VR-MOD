import sys;sys.path.insert(0,'/tmp/claude-0/geo')
from load import load
import numpy as np
f=sys.argv[1];want=set(sys.argv[2].split(','))
E,U,D=load(f)
X={};B={}
for l in open(f):
    p=l.strip().split(',')
    if p[0]=='X':X[int(p[1])]=p[2]
    if p[0]=='B':B[p[2]]=p[1].lstrip('0')
for d in D:
    t=B.get(X.get(d['id']),'?')
    if t not in want: continue
    V=d['V'];P=V[:,3:6];s=np.linalg.svd(P-P.mean(0),compute_uv=False) if len(P)>2 else [0,0,0]
    print(d['id'],'tex',t,'n',d['n'],'t',len(d['T']),'blend',d['blend'],d['src'],d['dst'],'zw',d['zw'],'at',d['at'],'up',d['upright'],'org',d['origin'],'box %.0f,%.0f-%.0f,%.0f'%(V[:,0].min(),V[:,1].min(),V[:,0].max(),V[:,1].max()),'rhw %.3f-%.3f'%(V[:,2].min(),V[:,2].max()),'sv',np.round(s,3),'col %08X'%int(V[0,8]))
