import glob,sys
from PIL import Image
W,H=256,192;uiW,uiH=1024,768
def scan(path):
    im=Image.open(path).convert('RGB').resize((uiW,uiH)).resize((W,H),Image.BILINEAR)
    px=im.load();iw,ih,ox,oy=1024,768,0,0;sx=uiW/W;sy=uiH/H
    row=[0]*H;rmin=[1e9]*H;rmax=[-1e9]*H;n=0;x0=y0=1e9;x1=y1=-1e9
    for y in range(H):
        v=(y+.5)*sy;fv=(v-oy)/ih
        if fv<.095 or fv>.765: continue
        for x in range(W):
            r,g,b=px[x,y]
            if max(r,g,b)<24: continue   # black = nothing drawn (alpha 0 in the interface layer)
            u=(x+.5)*sx;row[y]=1;rmin[y]=min(rmin[y],u);rmax[y]=max(rmax[y],u)
            if fv<=.685:
                n+=1;x0=min(x0,u-sx/2);x1=max(x1,u+sx/2);y0=min(y0,v-sy/2);y1=max(y1,v+sy/2)
    if n:
        y=int(min(H-1,y1/sy))
        while y+1<H and row[y+1] and rmin[y+1]>=x0-8 and rmax[y+1]<=x1+8: y+=1;y1=max(y1,(y+1)*sy)
    hit=n>=150 and x1-x0>=.12*iw and y1-y0>=.06*ih
    return hit,n,(round(x0),round(y0),round(x1),round(y1)) if n else None
for f in sorted(glob.glob('/home/claude/r*/*_ecran.jpg')):
    h,n,r=scan(f);print(('OPEN ' if h else 'shut ')+f.split('/')[-2]+'/'+f.split('_')[-2],n,r)
