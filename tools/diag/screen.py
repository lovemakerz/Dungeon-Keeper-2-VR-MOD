import sys,numpy as np
sys.path.insert(0,'/tmp/claude-0/geo')
from load import load
from PIL import Image,ImageDraw
E,U,D=load(sys.argv[1])
im=Image.new('RGB',(1024,768),(30,30,40));dr=ImageDraw.Draw(im,'RGBA')
order=sorted(D,key=lambda d:d['blend'])
for d in order:
    V=d['V']
    for t in d['T']:
        if max(t)>=len(V):continue
        pts=[(V[i,0],V[i,1]) for i in t]
        if d['blend']: col=(255,220,40,90) if d['dst']==2 else (40,220,255,90)
        else:
            c=int(V[t[0],8]);col=(((c>>16)&255)//2+60,((c>>8)&255)//2+60,(c&255)//2+60,255)
        dr.polygon(pts,fill=col)
for d in D:
    if d['blend'] and len(d['V']):
        V=d['V'];dr.text((V[:,0].mean(),V[:,1].mean()),str(d['id']),fill=(255,0,255,255)) if len(sys.argv)>3 else None
im.save(sys.argv[2])
