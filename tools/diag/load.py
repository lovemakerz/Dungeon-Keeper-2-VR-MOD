import numpy as np
def load(f):
    draws=[];E=None;U=None;cur=None
    for l in open(f):
        p=l.rstrip('\n').split(',')
        t=p[0]
        if t=='E':E=np.array(list(map(float,p[1:4])));W,H=int(p[4]),int(p[5])
        elif t=='U':U=np.array(list(map(float,p[1:4])))
        elif t=='D':
            cur=dict(id=int(p[1]),n=int(p[2]),prims=int(p[3]),type=int(p[4]),blend=int(p[5]),src=int(p[6]),dst=int(p[7]),zw=int(p[8]),at=int(p[9]),origin=p[10],sprites=int(p[11]),upright=int(p[12]),V=[],T=[]);draws.append(cur)
        elif t=='V':cur['V'].append(list(map(float,p[1:9]))+[int(p[9],16)])
        elif t=='T':cur['T'].append(list(map(int,p[1:4])))
    for d in draws:
        d['V']=np.array(d['V']) if d['V'] else np.zeros((0,9));d['T']=np.array(d['T'],dtype=int) if d['T'] else np.zeros((0,3),dtype=int)
    return E,U,draws
