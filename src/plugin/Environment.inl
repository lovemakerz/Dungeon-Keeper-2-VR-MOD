// DK2VR 0.4.1 - dungeon room and wooden table, drawn by the VR thread (device B) around the dungeon.
// Everything is generated here: tileable procedural textures with relief (normal maps), rounded meshes
// (profile sweeps with mitred corners, lathe-turned legs, tubes), per-pixel lighting shaders compiled at
// start (flickering torches, warm fill above the table, specular, soft fog). Fixed-function fallback.
// 0.4.0 (history): box meshes and per-vertex lighting.
// Room frame: origin on the floor below the recentre point, yaw of the recentre, -Z = forward.
// Table frame: TableModel.hpp. Nothing here touches the game or its device.
namespace env {
// ---------------- procedural textures ----------------
inline uint32_t hash3(int x,int y,int s) {
    uint32_t h=uint32_t(x)*374761393u+uint32_t(y)*668265263u+uint32_t(s)*2246822519u;
    h=(h^(h>>13))*1274126177u;return h^(h>>16);
}
inline float rnd(int x,int y,int s) {return float(hash3(x,y,s)&0xffffff)/float(0xffffff);}
inline float lattice(int x,int y,int px,int py,int s) {x=((x%px)+px)%px;y=((y%py)+py)%py;return rnd(x,y,s);}
inline float smooth01(float t) {return t*t*(3-2*t);}
inline float vnoise(float x,float y,int px,int py,int s) {
    int ix=int(std::floor(x)),iy=int(std::floor(y));float fx=smooth01(x-ix),fy=smooth01(y-iy);
    float a=lattice(ix,iy,px,py,s),b=lattice(ix+1,iy,px,py,s),c=lattice(ix,iy+1,px,py,s),d=lattice(ix+1,iy+1,px,py,s);
    return a+(b-a)*fx+(c-a)*fy+(a-b-c+d)*fx*fy;
}
// Tileable fractal noise: u,v in [0,1), px/py = cells across the texture for the first octave.
inline float fbm(float u,float v,int px,int py,int octaves,int s) {
    float sum=0,amp=.5f,total=0;
    for(int o=0;o<octaves;++o) {sum+=amp*vnoise(u*px,v*py,px,py,s+o*101);total+=amp;amp*=.5f;px*=2;py*=2;}
    return sum/total;
}
inline float clamp01(float v) {return v<0?0:v>1?1:v;}
inline float smoothstep(float a,float b,float x) {return smooth01(clamp01((x-a)/(b-a)));}
struct Color {float r,g,b;};
inline Color mix(Color a,Color b,float t) {return {a.r+(b.r-a.r)*t,a.g+(b.g-a.g)*t,a.b+(b.b-a.b)*t};}
inline Color mul(Color a,float k) {return {a.r*k,a.g*k,a.b*k};}
struct Image {
    int w=0,h=0;std::vector<uint32_t> px;std::vector<float> height; // height: relief for the normal map (0 = deepest)
    Image(int W,int H):w(W),h(H),px(size_t(W)*H),height(size_t(W)*H,.5f) {}
    void setHeight(int x,int y,float v) {height[size_t(y)*w+x]=v;}
    void set(int x,int y,Color c,float a=1) {
        auto b=[](float v){return uint32_t(clamp01(v)*255.f+.5f);};
        px[size_t(y)*w+x]=(b(a)<<24)|(b(c.r)<<16)|(b(c.g)<<8)|b(c.b);
    }
};
// Wood planks, grain along U (4 planks across V), tileable.
inline Image woodTexture() {
    const int N=512;Image img(N,N);
    const Color light{.47f,.31f,.18f},dark{.22f,.135f,.07f};
    for(int y=0;y<N;++y) for(int x=0;x<N;++x) {
        float u=(x+.5f)/N,v=(y+.5f)/N;
        int plank=int(v*4);float pv=v*4-plank;
        float tone=.82f+.32f*rnd(plank,1,3);
        float warp=fbm(u,v,2,4,4,11+plank*7);
        float rings=.5f+.5f*std::sin(2*Pi*(pv*3.f+warp*1.8f+rnd(plank,2,5)*6));
        rings=rings*rings;
        float streak=fbm(u,v,6,96,3,31+plank);
        float knots=smoothstep(.72f,.9f,fbm(u,v,4,4,3,41+plank));
        Color c=mix(light,dark,clamp01(.32f*rings+.55f*streak+.3f*knots));
        c=mul(c,tone*(.9f+.2f*fbm(u,v,32,32,2,51)));
        float edge=std::min(pv,1-pv)*N/4;
        float h=.62f+.1f*(1-streak)-.05f*rings;
        if(edge<1.2f) {c=mul(c,.32f);h=.1f;} else if(edge<3.f) {c=mul(c,.72f+.09f*(edge-1.2f));h=.1f+(h-.1f)*(edge-1.2f)/1.8f;}
        float joint=rnd(plank,3,7);float du=std::fabs(u-joint);du=std::min(du,1-du)*N;
        if(du<1.1f) {c=mul(c,.38f);h=.12f;}
        img.set(x,y,c);img.setHeight(x,y,h);
    }
    return img;
}
// Dungeon stone: 5 courses of large blocks of irregular length (3 per course), bevelled, rough; tileable.
inline Image stoneTexture() {
    const int N=512,R=5,B=3;Image img(N,N);
    const Color mortar{.11f,.105f,.10f};
    // Per course: random split points (periodic in U) and a random shift.
    float cuts[R][B+1];
    for(int r=0;r<R;++r) {
        float a=rnd(r,0,201)*.2f,w1=.28f+.1f*rnd(r,1,203),w2=.3f+.1f*rnd(r,2,207);
        cuts[r][0]=a;cuts[r][1]=a+w1;cuts[r][2]=a+w1+w2;cuts[r][3]=a+1;
    }
    for(int y=0;y<N;++y) for(int x=0;x<N;++x) {
        float u=(x+.5f)/N,v=(y+.5f)/N;
        float wob=.012f*(fbm(u,v,10,10,2,211)-.5f);
        float vv=v+wob;vv-=std::floor(vv);
        int row=std::min(R-1,int(vv*R));float rv=vv*R-row;
        float uu=u+.8f*wob;
        float rel=uu-cuts[row][0];rel-=std::floor(rel);rel+=cuts[row][0];
        int blk=0;while(blk<B-1&&rel>=cuts[row][blk+1]) ++blk;
        float fu0=(rel-cuts[row][blk])*N,fu1=(cuts[row][blk+1]-rel)*N;
        float e=std::min(std::min(fu0,fu1),std::min(rv,1-rv)*N/R);
        float ragged=4.f+4.f*fbm(u,v,20,20,3,61);
        Color c;float h;
        if(e<ragged) {c=mul(mortar,.7f+.6f*fbm(u,v,48,48,2,71));h=.05f+.08f*fbm(u,v,48,48,2,73);}
        else {
            h=.55f+.4f*smoothstep(0.f,22.f,e-ragged)*(.55f+.45f*fbm(u,v,10,10,5,23+blk*5+row*7));
            float t=rnd(blk,row,7),hue=rnd(blk,row,9);
            c=mix(Color{.40f,.385f,.36f},Color{.22f,.215f,.21f},t);
            c=mix(c,Color{.34f,.31f,.27f},hue*.5f);
            float surface=fbm(u,v,14,14,6,21+blk*3+row*11);
            c=mul(c,.62f+.62f*surface);
            // Relief: lit top edge, shaded bottom edge of each block.
            float fromTop=rv*N/R,fromBottom=(1-rv)*N/R;
            if(fromTop<ragged+10) c=mul(c,1.12f);
            if(fromBottom<ragged+12) c=mul(c,.72f+.28f*clamp01((fromBottom-ragged)/12));
            if(e<ragged+6) c=mul(c,.7f+.3f*(e-ragged)/6);
            float cracks=smoothstep(.02f,0.f,std::fabs(fbm(u,v,6,6,4,301+blk)-.5f));
            float crackMask=cracks*smoothstep(.55f,.7f,fbm(u,v,3,3,2,311));
            c=mul(c,1-.35f*crackMask);h-=.25f*crackMask;
        }
        img.set(x,y,c);img.setHeight(x,y,h);
    }
    return img;
}
// Irregular flagstones (periodic Voronoi cells) with dark joints, tileable.
inline Image floorTexture() {
    const int N=512,C=4;Image img(N,N);
    float pxs[C][C],pys[C][C];
    for(int i=0;i<C;++i) for(int j=0;j<C;++j) {pxs[i][j]=(i+.2f+.6f*rnd(i,j,13))/C;pys[i][j]=(j+.2f+.6f*rnd(i,j,17))/C;}
    for(int y=0;y<N;++y) for(int x=0;x<N;++x) {
        float u=(x+.5f)/N,v=(y+.5f)/N;
        float jitter=.012f*(fbm(u,v,12,12,3,91)-.5f);
        float f1=9,f2=9;int ci=0,cj=0;
        int gx=int(u*C),gy=int(v*C);
        for(int di=-1;di<=1;++di) for(int dj=-1;dj<=1;++dj) {
            int i=gx+di,j=gy+dj;int ii=((i%C)+C)%C,jj=((j%C)+C)%C;
            float cx=pxs[ii][jj]+float(i-ii)/C,cy=pys[ii][jj]+float(j-jj)/C;
            float dx=u+jitter-cx,dy=v-jitter-cy,d=std::sqrt(dx*dx+dy*dy);
            if(d<f1) {f2=f1;f1=d;ci=ii;cj=jj;} else if(d<f2) f2=d;
        }
        float edge=(f2-f1)*N;
        Color c;float h;
        if(edge<3.f) {c=mul(Color{.09f,.08f,.07f},.8f+.4f*fbm(u,v,40,40,2,95));h=.05f;}
        else {
            h=.5f+.45f*smoothstep(0.f,14.f,edge-3)*(.6f+.4f*fbm(u,v,8,8,5,33+ci*3+cj));
            float t=rnd(ci,cj,19);
            c=mix(Color{.34f,.325f,.31f},Color{.22f,.21f,.205f},t);
            c=mix(c,Color{.31f,.28f,.24f},rnd(ci,cj,23)*.45f);
            c=mul(c,.72f+.5f*fbm(u,v,20,20,5,31));
            float wear=smoothstep(.55f,.8f,fbm(u,v,6,6,3,37+ci*5+cj));
            c=mul(c,1+.12f*wear);
            if(edge<9) c=mul(c,.55f+.45f*(edge-3)/6);
        }
        img.set(x,y,c);img.setHeight(x,y,h);
    }
    return img;
}
inline Image ironTexture() {
    const int N=128;Image img(N,N);
    for(int y=0;y<N;++y) for(int x=0;x<N;++x) {
        float u=(x+.5f)/N,v=(y+.5f)/N;
        Color c=mul(Color{.26f,.26f,.27f},.7f+.6f*fbm(u,v,8,8,4,111));
        float rust=smoothstep(.62f,.8f,fbm(u,v,5,5,3,121));
        c=mix(c,Color{.30f,.17f,.09f},rust*.6f);
        img.set(x,y,c);img.setHeight(x,y,.5f+.35f*fbm(u,v,12,12,3,131));
    }
    return img;
}
// Flame sprite: hot core, orange body, soft edges (alpha = additive weight).
inline Image flameTexture() {
    const int W=64,H=128;Image img(W,H);
    for(int y=0;y<H;++y) for(int x=0;x<W;++x) {
        float fx=(x+.5f)/W*2-1,fy=1-(y+.5f)/H; // fy: 0 bottom .. 1 top
        float width=.62f*std::pow(clamp01(1-fy),.75f)*clamp01(fy*5+.35f);
        float d=width>1e-3f?std::fabs(fx)/width:9.f;
        float body=clamp01(1-d);body=body*body*(3-2*body);
        float core=clamp01(1-d*1.7f)*clamp01(1-fy*1.5f);
        Color c=mix(Color{1.f,.42f,.08f},Color{1.f,.86f,.45f},core);
        c=mix(c,Color{1.f,1.f,.85f},core*core);
        img.set(x,y,c,body*clamp01(1.15f-fy));
    }
    return img;
}
inline Image glowTexture() {
    const int N=64;Image img(N,N);
    for(int y=0;y<N;++y) for(int x=0;x<N;++x) {
        float dx=(x+.5f)/N*2-1,dy=(y+.5f)/N*2-1,r2=dx*dx+dy*dy;
        float a=std::exp(-r2*3.5f)*clamp01(1-r2);
        img.set(x,y,Color{1.f,.62f,.30f},a);
    }
    return img;
}
// Soft contact shadow under the table (multiplied onto the floor): white = no shadow.
inline Image shadowTexture() {
    const int N=128;Image img(N,N);
    for(int y=0;y<N;++y) for(int x=0;x<N;++x) {
        float u=std::fabs((x+.5f)/N*2-1),v=std::fabs((y+.5f)/N*2-1);
        float dx=std::max(0.f,u-.55f)/.45f,dy=std::max(0.f,v-.55f)/.45f;
        float r=std::sqrt(dx*dx+dy*dy);
        float s=1-.62f*(1-smoothstep(0.f,1.f,r));
        img.set(x,y,Color{s,s,s});
    }
    return img;
}
// Normal map (tangent space, +U = tangent, +V = bitangent) from the relief of an image.
inline Image normalMap(const Image& src,float strength) {
    Image out(src.w,src.h);
    auto h=[&](int x,int y){x=(x+src.w)%src.w;y=(y+src.h)%src.h;return src.height[size_t(y)*src.w+x];};
    for(int y=0;y<src.h;++y) for(int x=0;x<src.w;++x) {
        float dx=(h(x+1,y)-h(x-1,y))*.5f*strength,dy=(h(x,y+1)-h(x,y-1))*.5f*strength;
        float nx=-dx,ny=-dy,nz=1,l=std::sqrt(nx*nx+ny*ny+nz*nz);nx/=l;ny/=l;nz/=l;
        out.set(x,y,Color{nx*.5f+.5f,ny*.5f+.5f,nz*.5f+.5f});
    }
    return out;
}
// ---------------- meshes ----------------
#pragma pack(push,1)
struct Vtx {Vec3 p,n;float t[4];float u,v,ao;DWORD color;};
#pragma pack(pop)
static_assert(sizeof(Vtx)==56,"vertex layout");
struct Material {float specular=.15f,shininess=24,normal=1,wrap=.25f;Color tint{1,1,1};};
enum Tex {Wood,Stone,Floor,Iron,Flame,Glow,Shadow,TexCount};
struct Mesh {
    Tex tex=Wood;Material mat;std::vector<Vtx> v;
    ComPtr<IDirect3DVertexBuffer9> vb;UINT count=0;
    void vertex(Vec3 p,Vec3 n,Vec3 t,float sign,float u,float vv,float ao) {
        Vtx x{};x.p=p;x.n=n;x.t[0]=t.x;x.t[1]=t.y;x.t[2]=t.z;x.t[3]=sign;x.u=u;x.v=vv;x.ao=ao;x.color=0xff808080;v.push_back(x);
    }
    // Triangle pair from four corners with per-corner normals; tangent along a->b (U), sign from the V direction a->d.
    void quad4(const Vec3 p[4],const Vec3 n[4],const float uv[4][2],const float ao[4],Vec3 tangent) {
        if(!normalize(tangent)) tangent={1,0,0};
        Vec3 bit=sub(p[3],p[0]);float sign=dot(cross(n[0],tangent),bit)>=0?1.f:-1.f;
        for(int k:{0,1,2,0,2,3}) vertex(p[k],n[k],tangent,sign,uv[k][0],uv[k][1],ao[k]);
    }
    // Flat parallelogram o + a*du + b*dv, subdivided; UV = distance / tile.
    void quad(Vec3 o,Vec3 du,Vec3 dv,Vec3 n,float tileU,float tileV,float u0,float v0,int nu,int nv,const std::function<float(Vec3)>& ao=nullptr) {
        nu=std::max(1,nu);nv=std::max(1,nv);
        float lu=std::sqrt(dot(du,du)),lv=std::sqrt(dot(dv,dv));
        for(int i=0;i<nu;++i) for(int j=0;j<nv;++j) {
            Vec3 p[4];float uv[4][2],a[4];Vec3 nn[4]={n,n,n,n};
            const int di[4]={0,1,1,0},dj[4]={0,0,1,1};
            for(int k=0;k<4;++k) {
                float s=float(i+di[k])/nu,t=float(j+dj[k])/nv;
                p[k]=add(o,add(scale(du,s),scale(dv,t)));uv[k][0]=u0+lu*s/tileU;uv[k][1]=v0+lv*t/tileV;a[k]=ao?ao(p[k]):1.f;
            }
            quad4(p,nn,uv,a,du);
        }
    }
    void box(Vec3 lo,Vec3 hi,float tile,float aoBottom=1.f) {
        Vec3 s=sub(hi,lo);
        auto ao=[&](Vec3 p){return aoBottom+(1-aoBottom)*clamp01((p.y-lo.y)/std::max(1e-3f,s.y));};
        quad({lo.x,hi.y,lo.z},{s.x,0,0},{0,0,s.z},{0,1,0},tile,tile,lo.x/tile,lo.z/tile,1,1,ao);
        quad({lo.x,lo.y,lo.z},{s.x,0,0},{0,0,s.z},{0,-1,0},tile,tile,lo.x/tile,lo.z/tile,1,1,ao);
        quad({lo.x,lo.y,hi.z},{s.x,0,0},{0,s.y,0},{0,0,1},tile,tile,lo.x/tile,lo.y/tile,1,1,ao);
        quad({lo.x,lo.y,lo.z},{s.x,0,0},{0,s.y,0},{0,0,-1},tile,tile,lo.x/tile,lo.y/tile,1,1,ao);
        quad({hi.x,lo.y,lo.z},{0,0,s.z},{0,s.y,0},{1,0,0},tile,tile,lo.z/tile,lo.y/tile,1,1,ao);
        quad({lo.x,lo.y,lo.z},{0,0,s.z},{0,s.y,0},{-1,0,0},tile,tile,lo.z/tile,lo.y/tile,1,1,ao);
    }
};
// 2D profile (a = offset along the side normal, b = height), closed counter-clockwise; smooth normals
// between segments turning by less than 40 degrees.
struct Profile {
    std::vector<float> a,b;bool closed=true;
    void add(float x,float y) {a.push_back(x);b.push_back(y);}
    size_t size() const {return a.size();}
    size_t segments() const {return closed?a.size():a.size()-1;}
    void segment(size_t i,float& a0,float& b0,float& a1,float& b1) const {size_t j=(i+1)%a.size();a0=a[i];b0=b[i];a1=a[j];b1=b[j];}
    // Outward normal of segment i (counter-clockwise polygon).
    void normal(size_t i,float& na,float& nb) const {
        float a0,b0,a1,b1;segment(i,a0,b0,a1,b1);float da=a1-a0,db=b1-b0,l=std::sqrt(da*da+db*db);
        if(l<1e-7f) {na=0;nb=1;return;}
        na=db/l;nb=-da/l;
    }
    // Vertex normal at the start (end=false) or end of segment i.
    void vertexNormal(size_t i,bool end,float& na,float& nb) const {
        float a0,b0;normal(i,a0,b0);
        size_t n=segments();
        bool hasNext=closed||(end?i+1<n:i>0);
        if(!hasNext) {na=a0;nb=b0;return;}
        size_t k=end?(i+1)%n:(i+n-1)%n;
        float a1,b1;normal(k,a1,b1);
        if(a0*a1+b0*b1<.766f) {na=a0;nb=b0;return;}
        na=a0+a1;nb=b0+b1;float l=std::sqrt(na*na+nb*nb);na/=l;nb/=l;
    }
};
// Sweep along a path: point i at P[i], side normal N[i] (unit, for normals) and offset vector M[i]
// (usually N, or the mitre vector at corners), height axis Up[i].
struct PathPoint {Vec3 p,n,m,up;};
inline void sweep(Mesh& mesh,const std::vector<PathPoint>& path,const Profile& prof,float tile,const std::function<float(Vec3)>& ao=nullptr,float u0=0) {
    if(path.size()<2) return;
    std::vector<float> along(path.size(),u0);
    for(size_t i=1;i<path.size();++i) {Vec3 d=sub(path[i].p,path[i-1].p);along[i]=along[i-1]+std::sqrt(dot(d,d))/tile;}
    float vAcc=0;
    for(size_t s=0;s<prof.segments();++s) {
        float a0,b0,a1,b1;prof.segment(s,a0,b0,a1,b1);
        float len=std::sqrt((a1-a0)*(a1-a0)+(b1-b0)*(b1-b0))/tile;
        float na0,nb0,na1,nb1;prof.vertexNormal(s,false,na0,nb0);prof.vertexNormal(s,true,na1,nb1);
        for(size_t i=0;i+1<path.size();++i) {
            const PathPoint& A=path[i];const PathPoint& B=path[i+1];
            Vec3 p[4]={add(A.p,add(scale(A.m,a0),scale(A.up,b0))),add(B.p,add(scale(B.m,a0),scale(B.up,b0))),
                       add(B.p,add(scale(B.m,a1),scale(B.up,b1))),add(A.p,add(scale(A.m,a1),scale(A.up,b1)))};
            Vec3 n[4]={add(scale(A.n,na0),scale(A.up,nb0)),add(scale(B.n,na0),scale(B.up,nb0)),add(scale(B.n,na1),scale(B.up,nb1)),add(scale(A.n,na1),scale(A.up,nb1))};
            for(auto& x:n) normalize(x);
            float uv[4][2]={{along[i],vAcc},{along[i+1],vAcc},{along[i+1],vAcc+len},{along[i],vAcc+len}};
            float o[4];for(int k=0;k<4;++k) o[k]=ao?ao(p[k]):1.f;
            mesh.quad4(p,n,uv,o,sub(B.p,A.p));
        }
        vAcc+=len;
    }
}
// Rectangle path (centre c, half sizes hx,hz, in the XZ plane, up +Y) with mitred corners; one path per side
// so that normals do not bend around the corners. outward=true: offsets point away from the rectangle.
inline void sweepRect(Mesh& mesh,float hx,float hz,float y,const Profile& prof,float tile,bool outward,const std::function<float(Vec3)>& ao=nullptr) {
    const Vec3 corner[4]={{-hx,y,-hz},{hx,y,-hz},{hx,y,hz},{-hx,y,hz}};
    const Vec3 side[4]={{0,0,-1},{1,0,0},{0,0,1},{-1,0,0}}; // outward normal of side i (corner i -> i+1)
    float s=outward?1.f:-1.f;
    for(int i=0;i<4;++i) {
        int prev=(i+3)%4,next=(i+1)%4;
        Vec3 n=scale(side[i],s);
        std::vector<PathPoint> path={{corner[i],n,scale(add(side[i],side[prev]),s),{0,1,0}},{corner[next],n,scale(add(side[i],side[next]),s),{0,1,0}}};
        if(!outward) std::reverse(path.begin(),path.end());
        sweep(mesh,path,prof,tile,ao,float(i)*.37f);
    }
}
// Lathe around +Y: profile (r,y) from bottom to top (open), 'sides' segments. Frame maps local -> object.
struct Frame {Vec3 o,x,y,z;Vec3 point(Vec3 p) const {return add(o,add(scale(x,p.x),add(scale(y,p.y),scale(z,p.z))));}
                              Vec3 dir(Vec3 d) const {return add(scale(x,d.x),add(scale(y,d.y),scale(z,d.z)));}};
inline void lathe(Mesh& mesh,const std::vector<std::pair<float,float>>& ry,int sides,float tile,const Frame& f,const std::function<float(Vec3)>& ao=nullptr) {
    Profile prof;prof.closed=false;
    for(auto& q:ry) prof.add(q.first,q.second);
    float vAcc=0;
    for(size_t s=0;s+1<ry.size();++s) {
        float r0=ry[s].first,y0=ry[s].second,r1=ry[s+1].first,y1=ry[s+1].second;
        float len=std::sqrt((r1-r0)*(r1-r0)+(y1-y0)*(y1-y0))/tile;
        // Outward normal of a bottom-to-top profile in (r,y): (dy,-dr); Profile::normal gives (db,-da) for (a,b)=(r,y).
        float nr0,ny0,nr1,ny1;prof.vertexNormal(s,false,nr0,ny0);prof.vertexNormal(s,true,nr1,ny1);
        for(int k=0;k<sides;++k) {
            float t0=2*Pi*k/sides,t1=2*Pi*(k+1)/sides;
            auto P=[&](float r,float y,float t){return f.point({r*std::cos(t),y,r*std::sin(t)});};
            auto N=[&](float nr,float ny,float t){Vec3 d=f.dir({nr*std::cos(t),ny,nr*std::sin(t)});normalize(d);return d;};
            Vec3 p[4]={P(r0,y0,t0),P(r0,y0,t1),P(r1,y1,t1),P(r1,y1,t0)};
            Vec3 n[4]={N(nr0,ny0,t0),N(nr0,ny0,t1),N(nr1,ny1,t1),N(nr1,ny1,t0)};
            float uc=std::max(r0,r1)*2*Pi/tile;
            float uv[4][2]={{uc*k/sides,vAcc},{uc*(k+1)/sides,vAcc},{uc*(k+1)/sides,vAcc+len},{uc*k/sides,vAcc+len}};
            float o[4];for(int j=0;j<4;++j) o[j]=ao?ao(p[j]):1.f;
            mesh.quad4(p,n,uv,o,f.dir({-std::sin((t0+t1)/2),0,std::cos((t0+t1)/2)}));
        }
        vAcc+=len;
    }
}
// Round tube along a polyline (parallel transport frames), radius r.
inline void tube(Mesh& mesh,const std::vector<Vec3>& pts,float r,int sides,float tile) {
    if(pts.size()<2) return;
    std::vector<Vec3> T(pts.size());
    for(size_t i=0;i<pts.size();++i) {
        Vec3 d=i==0?sub(pts[1],pts[0]):i+1==pts.size()?sub(pts[i],pts[i-1]):sub(pts[i+1],pts[i-1]);
        normalize(d);T[i]=d;
    }
    Vec3 N=std::fabs(T[0].y)<.9f?cross(T[0],{0,1,0}):cross(T[0],{1,0,0});normalize(N);
    std::vector<Vec3> Ns(pts.size()),Bs(pts.size());
    for(size_t i=0;i<pts.size();++i) {
        N=sub(N,scale(T[i],dot(N,T[i])));normalize(N);
        Ns[i]=N;Bs[i]=cross(T[i],N);
    }
    float along=0;
    for(size_t i=0;i+1<pts.size();++i) {
        Vec3 d=sub(pts[i+1],pts[i]);float l=std::sqrt(dot(d,d))/tile;
        for(int k=0;k<sides;++k) {
            float t0=2*Pi*k/sides,t1=2*Pi*(k+1)/sides;
            auto dir=[&](size_t j,float t){return add(scale(Ns[j],std::cos(t)),scale(Bs[j],std::sin(t)));};
            Vec3 n[4]={dir(i,t0),dir(i+1,t0),dir(i+1,t1),dir(i,t1)};
            Vec3 p[4]={add(pts[i],scale(n[0],r)),add(pts[i+1],scale(n[1],r)),add(pts[i+1],scale(n[2],r)),add(pts[i],scale(n[3],r))};
            float uc=2*Pi*r/tile;
            float uv[4][2]={{along,uc*k/sides},{along+l,uc*k/sides},{along+l,uc*(k+1)/sides},{along,uc*(k+1)/sides}};
            float o[4]={1,1,1,1};
            mesh.quad4(p,n,uv,o,d);
        }
        along+=l;
    }
}
// Rounded corner polyline helper: arc from a (direction of travel da) turning to direction db, radius r.
inline void arcTo(std::vector<Vec3>& pts,Vec3 center,Vec3 from,Vec3 to,int steps) {
    // from/to: unit vectors from the centre; rotate in their plane.
    float ang=std::acos(std::clamp(dot(from,to),-1.f,1.f));
    Vec3 axis=cross(from,to);if(!normalize(axis)) {pts.push_back(add(center,from));return;}
    float r=1;
    for(int i=1;i<=steps;++i) {
        float t=ang*i/steps;Quat q{axis.x*std::sin(t/2),axis.y*std::sin(t/2),axis.z*std::sin(t/2),std::cos(t/2)};
        pts.push_back(add(center,scale(rotate(q,from),r)));
    }
}

struct Light {Vec3 pos;Color color;float intensity,range;float phase;};
inline float flicker(float t,float phase) {
    return 1+.07f*std::sin(t*11.3f+phase)+.05f*std::sin(t*23.7f+phase*2.1f)+.04f*std::sin(t*5.1f+phase*.7f);
}
// Baked vertex colour for the fixed-function fallback (no flicker).
inline void bake(Mesh& m,const Light* l,int n,const Color& ambient,const float* toRoom16) {
    for(auto& v:m.v) {
        Vec3 p=toRoom16?transformRow(toRoom16,v.p):v.p;
        Vec3 nn=v.n;
        if(toRoom16) {nn={v.n.x*toRoom16[0]+v.n.y*toRoom16[4]+v.n.z*toRoom16[8],v.n.x*toRoom16[1]+v.n.y*toRoom16[5]+v.n.z*toRoom16[9],v.n.x*toRoom16[2]+v.n.y*toRoom16[6]+v.n.z*toRoom16[10]};}
        Color c=mul(ambient,v.ao);
        for(int i=0;i<n;++i) {
            Vec3 d=sub(l[i].pos,p);float d2=dot(d,d),dl=std::sqrt(d2);if(dl<1e-4f) continue;
            float ndl=(dot(nn,scale(d,1/dl))+.3f)/1.3f;if(ndl<=0) continue;
            float r=dl/l[i].range,k=l[i].intensity*ndl/(1+r*r*2.2f);
            c.r+=l[i].color.r*k;c.g+=l[i].color.g*k;c.b+=l[i].color.b*k;
        }
        auto b=[&](float x,float t){return uint32_t(clamp01(x*t*.5f)*255.f+.5f);};
        v.color=0xff000000u|(b(c.r,m.mat.tint.r)<<16)|(b(c.g,m.mat.tint.g)<<8)|b(c.b,m.mat.tint.b);
    }
}

// Per-pixel lighting: 8 point lights (unrolled: the Wine compiler used for the offline preview has no loops),
// tangent-space normal map, wrapped diffuse, Blinn specular, ambient occlusion, distance fog.
const char* ShaderSource=R"(
row_major float4x4 WorldViewProj:register(c0);
row_major float4x4 World:register(c4);
struct VSIn {float3 pos:POSITION;float3 nrm:NORMAL;float4 tan:TANGENT;float2 uv:TEXCOORD0;float ao:TEXCOORD1;};
struct VSOut {float4 pos:POSITION;float2 uv:TEXCOORD0;float3 wpos:TEXCOORD1;float3 n:TEXCOORD2;float4 t:TEXCOORD3;float ao:TEXCOORD4;};
VSOut vs_main(VSIn i) {
    VSOut o;o.pos=mul(float4(i.pos,1),WorldViewProj);o.uv=i.uv;
    o.wpos=mul(float4(i.pos,1),World).xyz;o.n=mul(i.nrm,(float3x3)World);o.t=float4(mul(i.tan.xyz,(float3x3)World),i.tan.w);o.ao=i.ao;
    return o;
}
sampler2D Albedo:register(s0);
sampler2D NormalMap:register(s1);
float4 LightPos[8]:register(c0);
float4 LightCol[8]:register(c8);
float4 Ambient:register(c16);
float4 EyePos:register(c17);
float4 Material:register(c18);
float4 Tint:register(c19);
float4 Fog:register(c20);
float3 shadeOne(float4 lp,float4 lc,float3 wpos,float3 n,float3 v,float3 albedo,float ao) {
    float3 l=lp.xyz-wpos;float d2=dot(l,l);l*=rsqrt(max(d2,1e-6));
    float att=1/(1+d2*lp.w);
    float ndl=saturate((dot(n,l)+Material.w)/(1+Material.w));
    float3 h=normalize(l+v);
    float spec=pow(saturate(dot(n,h)),Material.y)*Material.x*saturate(dot(n,l)*4);
    return lc.rgb*att*(albedo*ndl*(0.55+0.45*ao)+spec);
}
struct PSIn {float2 uv:TEXCOORD0;float3 wpos:TEXCOORD1;float3 n:TEXCOORD2;float4 t:TEXCOORD3;float ao:TEXCOORD4;};
float4 ps_main(PSIn i):COLOR {
    float3 N=normalize(i.n);float3 T=normalize(i.t.xyz-N*dot(i.t.xyz,N));float3 B=cross(N,T)*i.t.w;
    float3 m=tex2D(NormalMap,i.uv).xyz*2-1;m.xy*=Material.z;
    float3 n=normalize(T*m.x+B*m.y+N*m.z);
    float3 albedo=tex2D(Albedo,i.uv).rgb*Tint.rgb;
    float3 v=normalize(EyePos.xyz-i.wpos);
    float3 c=Ambient.rgb*albedo*i.ao;
    c+=shadeOne(LightPos[0],LightCol[0],i.wpos,n,v,albedo,i.ao);
    c+=shadeOne(LightPos[1],LightCol[1],i.wpos,n,v,albedo,i.ao);
    c+=shadeOne(LightPos[2],LightCol[2],i.wpos,n,v,albedo,i.ao);
    c+=shadeOne(LightPos[3],LightCol[3],i.wpos,n,v,albedo,i.ao);
#if LIGHTS>4
    c+=shadeOne(LightPos[4],LightCol[4],i.wpos,n,v,albedo,i.ao);
    c+=shadeOne(LightPos[5],LightCol[5],i.wpos,n,v,albedo,i.ao);
    c+=shadeOne(LightPos[6],LightCol[6],i.wpos,n,v,albedo,i.ao);
    c+=shadeOne(LightPos[7],LightCol[7],i.wpos,n,v,albedo,i.ao);
#endif
    float dist=length(EyePos.xyz-i.wpos);
    c=lerp(Fog.rgb,c,exp(-dist*Fog.w));
    c=c/(1+c*0.25);
    return float4(c,1);
}
)";
// Offline preview only (Wine's HLSL compiler emits too long code for 8 lights): fewer lights.
inline const char* ShaderLights="8";
typedef HRESULT (WINAPI* CompileFn)(LPCVOID,SIZE_T,LPCSTR,const void*,void*,LPCSTR,LPCSTR,UINT,UINT,ID3DBlob**,ID3DBlob**);

inline void mul4(const float* a,const float* b,float* out) {
    for(int r=0;r<4;++r) for(int c=0;c<4;++c) {float s=0;for(int k=0;k<4;++k) s+=a[r*4+k]*b[k*4+c];out[r*4+c]=s;}
}

struct Scene {
    ComPtr<IDirect3DTexture9> albedo[TexCount],normals[TexCount];bool ready=false,failed=false;
    ComPtr<IDirect3DVertexShader9> vs;ComPtr<IDirect3DPixelShader9> ps;ComPtr<IDirect3DVertexDeclaration9> decl;bool shaders=false;
    std::vector<Mesh> room,table;Mesh shadow,tray;
    struct Torch {Vec3 flame;float phase;};std::vector<Torch> torches;
    float tableKeyW=-1,tableKeyD=-1,tableKeyFloor=1e9f;
    int handleStateDrawn[8]{};int floorLogged=-1;
    double buildMs=0;
    // Room: X -2.4..2.4, Z -2.6..1.9, walls 2.5 m, segmental stone vault rising 0.8 m (apex 3.3 m).
    static constexpr float X0=-2.4f,X1=2.4f,Z0=-2.6f,Z1=1.9f,Hw=2.5f,Rise=.8f;
    static float vaultR() {float h=(X1-X0)*.5f;return (h*h+Rise*Rise)/(2*Rise);}
    static float vaultY(float x) {float R=vaultR();return Hw+std::sqrt(std::max(0.f,R*R-x*x))-(R-Rise);}
    static Vec3 vaultCentre() {return {0,Hw+Rise-vaultR(),0};}

    static bool upload(IDirect3DDevice9Ex* dev,const Image& img,ComPtr<IDirect3DTexture9>& out) {
        ComPtr<IDirect3DTexture9> sys;
        if(FAILED(dev->CreateTexture(img.w,img.h,0,0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&sys,nullptr))) return false;
        std::vector<uint32_t> level=img.px;int w=img.w,h=img.h;
        for(UINT l=0;l<sys->GetLevelCount();++l) {
            D3DLOCKED_RECT lr{};if(FAILED(sys->LockRect(l,&lr,nullptr,0))) return false;
            for(int y=0;y<h;++y) memcpy(static_cast<unsigned char*>(lr.pBits)+size_t(y)*lr.Pitch,level.data()+size_t(y)*w,size_t(w)*4);
            sys->UnlockRect(l);
            if(w==1&&h==1) break;
            int nw=std::max(1,w/2),nh=std::max(1,h/2);std::vector<uint32_t> next(size_t(nw)*nh);
            for(int y=0;y<nh;++y) for(int x=0;x<nw;++x) {
                uint32_t acc[4]{};
                for(int k=0;k<4;++k) {
                    int sx=std::min(w-1,x*2+(k&1)),sy=std::min(h-1,y*2+(k>>1));uint32_t p=level[size_t(sy)*w+sx];
                    for(int ch=0;ch<4;++ch) acc[ch]+=(p>>(ch*8))&255;
                }
                next[size_t(y)*nw+x]=((acc[3]/4)<<24)|((acc[2]/4)<<16)|((acc[1]/4)<<8)|(acc[0]/4);
            }
            level.swap(next);w=nw;h=nh;
        }
        if(FAILED(dev->CreateTexture(img.w,img.h,0,0,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&out,nullptr))) return false;
        return SUCCEEDED(dev->UpdateTexture(sys.Get(),out.Get()));
    }
    // CPU work (textures, relief, meshes, shader compilation) runs on a worker thread started when the game
    // starts (0.4.3; 0.4.2 started it at the table, below-normal priority: 18 s measured). The result is kept
    // in a disk cache, so later launches only read ~9 MB. The VR thread then uploads one item per frame.
    volatile LONG stage=0; // 0 not started, 1 worker running, 2 data ready (uploading), -1 worker failed
    std::vector<Image> images,normalImages;std::vector<DWORD> vsCode,psCode;bool haveCode=false,fromCache=false;int uploadStep=0;double workerMs=0,uploadMs=0;
    std::wstring cacheFile;
    static constexpr uint32_t CacheMagic=0x4d4f4f52; // "ROOM"
    static constexpr uint32_t CacheFormat=5;
    bool compileCode() {
        HMODULE m=LoadLibraryW(L"d3dcompiler_47.dll");
        if(!m) {log("Environment: d3dcompiler_47.dll not found; simple lighting");return false;}
        auto compileFn=reinterpret_cast<CompileFn>(GetProcAddress(m,"D3DCompile"));
        if(!compileFn) return false;
        ComPtr<ID3DBlob> vcode,pcode,err;
        struct Macro {const char* name;const char* def;} macros[]={{"LIGHTS",ShaderLights},{nullptr,nullptr}};
        HRESULT hr=compileFn(ShaderSource,strlen(ShaderSource),"DK2VR_Environment",macros,nullptr,"vs_main","vs_3_0",1u<<15,0,&vcode,&err);
        if(FAILED(hr)) {log("Environment: vertex shader: %s",err?static_cast<const char*>(err->GetBufferPointer()):"?");return false;}
        err.Reset();
        hr=compileFn(ShaderSource,strlen(ShaderSource),"DK2VR_Environment",macros,nullptr,"ps_main","ps_3_0",1u<<15,0,&pcode,&err);
        if(FAILED(hr)) {log("Environment: pixel shader: %s",err?static_cast<const char*>(err->GetBufferPointer()):"?");return false;}
        vsCode.assign(static_cast<const DWORD*>(vcode->GetBufferPointer()),static_cast<const DWORD*>(vcode->GetBufferPointer())+vcode->GetBufferSize()/4);
        psCode.assign(static_cast<const DWORD*>(pcode->GetBufferPointer()),static_cast<const DWORD*>(pcode->GetBufferPointer())+pcode->GetBufferSize()/4);
        return true;
    }
    // Cache key: format, shader source and light count, so any change regenerates it.
    uint32_t cacheKey() const {
        uint32_t h=2166136261u;auto mix=[&](const char* p){for(;*p;++p) h=(h^uint8_t(*p))*16777619u;};
        mix(ShaderSource);mix(ShaderLights);return h^CacheFormat;
    }
    bool loadCache() {
        if(cacheFile.empty()) return false;
        FILE* f=_wfopen(cacheFile.c_str(),L"rb");if(!f) return false;
        auto rd=[&](void* p,size_t n){return fread(p,1,n,f)==n;};
        uint32_t magic=0,key=0,count=0;bool ok=rd(&magic,4)&&rd(&key,4)&&rd(&count,4)&&magic==CacheMagic&&key==cacheKey()&&count==2*TexCount;
        std::vector<Image> imgs;
        for(uint32_t i=0;ok&&i<count;++i) {
            int32_t w=0,h=0;ok=rd(&w,4)&&rd(&h,4)&&w>0&&h>0&&w<=4096&&h<=4096;
            if(!ok) break;
            Image im(w,h);ok=rd(im.px.data(),im.px.size()*4);imgs.push_back(std::move(im));
        }
        uint32_t vn=0,pn=0;
        if(ok) ok=rd(&vn,4)&&vn<65536;
        if(ok) {vsCode.resize(vn);ok=!vn||rd(vsCode.data(),vn*4);}
        if(ok) ok=rd(&pn,4)&&pn<65536;
        if(ok) {psCode.resize(pn);ok=!pn||rd(psCode.data(),pn*4);}
        fclose(f);
        if(!ok) {vsCode.clear();psCode.clear();return false;}
        images.assign(imgs.begin(),imgs.begin()+TexCount);normalImages.assign(imgs.begin()+TexCount,imgs.end());
        haveCode=!vsCode.empty()&&!psCode.empty();
        return true;
    }
    void saveCache() {
        if(cacheFile.empty()) return;
        std::wstring tmp=cacheFile+L".tmp";
        FILE* f=_wfopen(tmp.c_str(),L"wb");if(!f) {log("Environment: cache not writable (the room will be generated at each launch)");return;}
        auto wr=[&](const void* p,size_t n){return fwrite(p,1,n,f)==n;};
        uint32_t magic=CacheMagic,key=cacheKey(),count=2*TexCount;bool ok=wr(&magic,4)&&wr(&key,4)&&wr(&count,4);
        for(int pass=0;pass<2&&ok;++pass) for(auto& im:pass?normalImages:images) {int32_t w=im.w,h=im.h;ok=ok&&wr(&w,4)&&wr(&h,4)&&wr(im.px.data(),im.px.size()*4);}
        uint32_t vn=haveCode?uint32_t(vsCode.size()):0,pn=haveCode?uint32_t(psCode.size()):0;
        ok=ok&&wr(&vn,4)&&(!vn||wr(vsCode.data(),vn*4))&&wr(&pn,4)&&(!pn||wr(psCode.data(),pn*4));
        fclose(f);
        if(ok&&MoveFileExW(tmp.c_str(),cacheFile.c_str(),MOVEFILE_REPLACE_EXISTING)) return;
        DeleteFileW(tmp.c_str());log("Environment: cache write failed");
    }
    void prepareCpu() {
        fromCache=loadCache();
        if(!fromCache) {
            haveCode=compileCode();
            images={woodTexture(),stoneTexture(),floorTexture(),ironTexture(),flameTexture(),glowTexture(),shadowTexture()};
            const float strength[TexCount]={3.f,6.f,6.f,2.f,0,0,0};
            normalImages.clear();
            for(int i=0;i<TexCount;++i) normalImages.push_back(strength[i]>0?normalMap(images[i],strength[i]*images[i].w/256.f):Image(1,1));
            saveCache();
        }
        buildRoom();
    }
    static DWORD WINAPI worker(void* self) {
        auto* s=static_cast<Scene*>(self);double t0=milliseconds();
        try {s->prepareCpu();} catch(...) {InterlockedExchange(&s->stage,-1);return 0;}
        s->workerMs=milliseconds()-t0;
        log("Environment: room prepared in %.0f ms (%s)",s->workerMs,s->fromCache?"read from the cache":"generated, cache written");
        MemoryBarrier();InterlockedExchange(&s->stage,2);return 0;
    }
    // Game start (or first use): background preparation. cache: file for the generated data (may be empty).
    void startBackground(const std::wstring& cache) {
        if(InterlockedCompareExchange(&stage,1,0)!=0) return;
        cacheFile=cache;
        HANDLE t=CreateThread(nullptr,0,&worker,this,0,nullptr);
        if(!t) {log("Environment: worker thread failed; room disabled");InterlockedExchange(&stage,-1);return;}
        CloseHandle(t);log("Environment: preparing the room in the background");
    }
    // Called by the VR thread every frame until the room is ready; never blocks for long.
    bool init(IDirect3DDevice9Ex* dev) {
        if(ready) return true;
        if(failed) return false;
        LONG st=stage;
        if(st==0) {startBackground(L"");return false;}
        if(st==1) return false;
        if(st<0) {log("Environment: preparation failed; room disabled");failed=true;return false;}
        double t0=milliseconds();
        int i=uploadStep++;
        if(i==0) {
            const D3DVERTEXELEMENT9 elements[]={{0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},{0,12,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_NORMAL,0},
                {0,24,D3DDECLTYPE_FLOAT4,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TANGENT,0},{0,40,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0},
                {0,48,D3DDECLTYPE_FLOAT1,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,1},{0,52,D3DDECLTYPE_D3DCOLOR,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_COLOR,0},D3DDECL_END()};
            if(FAILED(dev->CreateVertexDeclaration(elements,&decl))) {log("Environment: vertex declaration failed; room disabled");failed=true;}
        } else if(i<=TexCount) {
            int k=i-1;
            if(!upload(dev,images[k],albedo[k])) {log("Environment: texture %d upload failed; room disabled",k);failed=true;}
        } else if(i<=2*TexCount) {
            int k=i-1-TexCount;
            if(normalImages[k].w>1&&!upload(dev,normalImages[k],normals[k])) {log("Environment: relief %d upload failed; room disabled",k);failed=true;}
        } else if(i==2*TexCount+1) {
            shaders=haveCode&&SUCCEEDED(dev->CreateVertexShader(vsCode.data(),&vs))&&SUCCEEDED(dev->CreatePixelShader(psCode.data(),&ps));
            if(haveCode&&!shaders) log("Environment: shader creation failed; simple lighting");
            if(!shaders) bakeRoom();
        } else {
            for(auto& m:room) if(!toBuffer(dev,m)) {log("Environment: room buffers failed; room disabled");failed=true;break;}
            if(!failed) {
                ready=true;images.clear();normalImages.clear();vsCode.clear();psCode.clear();
                size_t n=0;for(auto& b:room) n+=b.v.size();
                log("Environment ready: dungeon room and wooden table (uploads %.0f ms over %d frames, %zu room triangles, %s)",
                    uploadMs+milliseconds()-t0,uploadStep,n/3,shaders?"per-pixel lighting":"simple lighting");
            }
        }
        uploadMs+=milliseconds()-t0;
        return ready;
    }
    static bool toBuffer(IDirect3DDevice9Ex* dev,Mesh& m) {
        m.vb.Reset();m.count=UINT(m.v.size());
        if(!m.count) return true;
        if(FAILED(dev->CreateVertexBuffer(m.count*sizeof(Vtx),D3DUSAGE_WRITEONLY,0,D3DPOOL_DEFAULT,&m.vb,nullptr))) return false;
        void* p=nullptr;if(FAILED(m.vb->Lock(0,0,&p,0))) return false;
        memcpy(p,m.v.data(),m.count*sizeof(Vtx));m.vb->Unlock();return true;
    }
    // Static light rig (room frame) for the fallback bake and as the base of the per-frame lights.
    std::vector<Light> baseLights(Vec3 tableInRoom) const {
        std::vector<Light> l;
        for(auto& t:torches) l.push_back({t.flame,{1.f,.62f,.34f},1.35f,2.1f,t.phase});
        l.push_back({add(tableInRoom,{0,1.3f,.1f}),{1.f,.88f,.74f},.62f,1.6f,0});
        return l;
    }
    void bakeRoom() {auto l=baseLights({0,1,-.8f});for(auto& m:room) bake(m,l.data(),int(l.size()),{.06f,.06f,.07f},nullptr);}

    void buildRoom() {
        room.clear();torches.clear();
        auto wallAO=[](Vec3 p) {
            float d=std::min(std::min(p.y,vaultY(p.x)-p.y+.2f),std::min(std::min(p.x-X0,X1-p.x),std::min(p.z-Z0,Z1-p.z)));
            return .45f+.55f*smoothstep(0.f,.75f,d);
        };
        Material stoneMat{.10f,14,1.f,.2f,{1,1,1}};
        // Floor
        Mesh floor;floor.tex=Floor;floor.mat={.16f,22,1.f,.2f,{1,1,1}};
        floor.quad({X0,0,Z0},{X1-X0,0,0},{0,0,Z1-Z0},{0,1,0},1.6f,1.6f,0,0,24,22,[](Vec3 p){
            float d=std::min(std::min(p.x-X0,X1-p.x),std::min(p.z-Z0,Z1-p.z));return .5f+.5f*smoothstep(0.f,.9f,d);});
        room.push_back(std::move(floor));
        // Walls: side walls rectangular, end walls follow the vault arc.
        Mesh walls;walls.tex=Stone;walls.mat=stoneMat;
        const float t=1.9f;
        walls.quad({X0,0,Z0},{0,0,Z1-Z0},{0,Hw,0},{1,0,0},t,t,0,0,18,10,wallAO);
        walls.quad({X1,0,Z0},{0,0,Z1-Z0},{0,Hw,0},{-1,0,0},t,t,0,0,18,10,wallAO);
        const int cols=24,rows=12;
        for(int endWall=0;endWall<2;++endWall) {
            float z=endWall?Z1:Z0;Vec3 n=endWall?Vec3{0,0,-1}:Vec3{0,0,1};
            for(int i=0;i<cols;++i) for(int j=0;j<rows;++j) {
                float x0=X0+(X1-X0)*i/cols,x1=X0+(X1-X0)*(i+1)/cols;
                float y00=vaultY(x0)*j/rows,y01=vaultY(x0)*(j+1)/rows,y10=vaultY(x1)*j/rows,y11=vaultY(x1)*(j+1)/rows;
                Vec3 p[4]={{x0,y00,z},{x1,y10,z},{x1,y11,z},{x0,y01,z}};Vec3 nn[4]={n,n,n,n};
                float uv[4][2]={{x0/t,-y00/t},{x1/t,-y10/t},{x1/t,-y11/t},{x0/t,-y01/t}},a[4];
                for(int k=0;k<4;++k) a[k]=wallAO(p[k]);
                walls.quad4(p,nn,uv,a,{1,0,0});
            }
        }
        room.push_back(std::move(walls));
        // Vault (stone), normals towards the vault axis.
        Mesh vault;vault.tex=Stone;vault.mat=stoneMat;vault.mat.tint={.85f,.83f,.8f};
        {
            const int nx=28,nz=18;Vec3 c=vaultCentre();
            for(int i=0;i<nx;++i) for(int j=0;j<nz;++j) {
                Vec3 p[4];Vec3 n[4];float uv[4][2],a[4];
                const int di[4]={0,1,1,0},dj[4]={0,0,1,1};
                for(int k=0;k<4;++k) {
                    float x=X0+(X1-X0)*(i+di[k])/nx,z=Z0+(Z1-Z0)*(j+dj[k])/nz;
                    p[k]={x,vaultY(x),z};n[k]=sub({0,c.y,z},p[k]);n[k].z=0;normalize(n[k]);
                    float arc=vaultR()*std::atan2(x,p[k].y-c.y);uv[k][0]=arc/t;uv[k][1]=z/t;
                    a[k]=.55f+.45f*smoothstep(0.f,.6f,std::min(z-Z0,Z1-z));
                }
                vault.quad4(p,n,uv,a,{1,0,0});
            }
        }
        room.push_back(std::move(vault));
        // Stone ribs: pilaster up each side wall then along the vault, at three depths.
        Mesh ribs;ribs.tex=Stone;ribs.mat=stoneMat;ribs.mat.tint={1.08f,1.04f,1.f};
        for(float z:{-1.95f,-.35f,1.2f}) {
            std::vector<PathPoint> path;Vec3 c=vaultCentre();
            path.push_back({{X0,0,z},{1,0,0},{1,0,0},{0,0,1}});
            path.push_back({{X0,Hw,z},{1,0,0},{1,0,0},{0,0,1}});
            const int steps=24;
            for(int i=0;i<=steps;++i) {
                float x=X0+(X1-X0)*i/steps;Vec3 p{x,vaultY(x),z};Vec3 n=sub({0,c.y,z},p);n.z=0;normalize(n);
                path.push_back({p,n,n,{0,0,1}});
            }
            path.push_back({{X1,Hw,z},{-1,0,0},{-1,0,0},{0,0,1}});
            path.push_back({{X1,0,z},{-1,0,0},{-1,0,0},{0,0,1}});
            Profile pr; // a = depth into the room, b = along Z; counter-clockwise, chamfered
            pr.add(0,-.15f);pr.add(.10f,-.15f);pr.add(.13f,-.12f);pr.add(.13f,.12f);pr.add(.10f,.15f);pr.add(0,.15f);
            sweep(ribs,path,pr,1.9f,[&](Vec3 p){return .6f+.4f*smoothstep(0.f,.8f,std::min(p.y,3.f));});
        }
        room.push_back(std::move(ribs));
        // Stone plinth along the walls (moulded).
        Mesh plinth;plinth.tex=Stone;plinth.mat=stoneMat;plinth.mat.tint={.8f,.78f,.75f};
        {
            Profile pr;pr.add(-.001f,0);pr.add(.06f,0);pr.add(.06f,.12f);pr.add(.04f,.15f);pr.add(-.001f,.16f);
            // inward sweep around the room rectangle (offset towards the room centre)
            const float cx=(X0+X1)*.5f,cz=(Z0+Z1)*.5f;
            Mesh tmp;sweepRect(tmp,(X1-X0)*.5f,(Z1-Z0)*.5f,0,pr,1.2f,false,[](Vec3){return .6f;});
            for(auto& v:tmp.v) {v.p.x+=cx;v.p.z+=cz;plinth.v.push_back(v);}
        }
        room.push_back(std::move(plinth));
        // Arched door with a moulded stone surround and iron straps (back wall).
        Mesh door;door.tex=Wood;door.mat={.12f,18,1.f,.25f,{.55f,.46f,.40f}};
        Mesh iron;iron.tex=Iron;iron.mat={.55f,36,1.f,.2f,{.72f,.7f,.7f}};
        Mesh surround;surround.tex=Stone;surround.mat=stoneMat;surround.mat.tint={1.1f,1.06f,1.f};
        {
            const float dw=.55f,dh=1.65f,zf=Z1-.035f;const int cols2=10,rows2=16;
            auto topAt=[&](float x){return dh+std::sqrt(std::max(0.f,dw*dw-x*x));};
            for(int i=0;i<cols2;++i) for(int j=0;j<rows2;++j) {
                float x0=-dw+2*dw*i/cols2,x1=-dw+2*dw*(i+1)/cols2;
                Vec3 p[4]={{x0,topAt(x0)*j/rows2,zf},{x1,topAt(x1)*j/rows2,zf},{x1,topAt(x1)*(j+1)/rows2,zf},{x0,topAt(x0)*(j+1)/rows2,zf}};
                Vec3 n[4]={{0,0,-1},{0,0,-1},{0,0,-1},{0,0,-1}};
                // grain vertical: U along Y
                float uv[4][2],a[4];for(int k=0;k<4;++k) {uv[k][0]=p[k].y/.6f;uv[k][1]=p[k].x/.5f+.1f;a[k]=.65f+.35f*smoothstep(0.f,.25f,std::min(p[k].x+dw,dw-p[k].x));}
                door.quad4(p,n,uv,a,{0,1,0});
            }
            std::vector<PathPoint> path;
            path.push_back({{-dw,0,Z1},{-1,0,0},{-1,0,0},{0,0,-1}});
            path.push_back({{-dw,dh,Z1},{-1,0,0},{-1,0,0},{0,0,-1}});
            for(int i=1;i<16;++i) {float t=Pi*i/16;Vec3 n{-std::cos(t),std::sin(t),0};path.push_back({{dw*n.x,dh+dw*n.y,Z1},n,n,{0,0,-1}});}
            path.push_back({{dw,dh,Z1},{1,0,0},{1,0,0},{0,0,-1}});
            path.push_back({{dw,0,Z1},{1,0,0},{1,0,0},{0,0,-1}});
            Profile pr; // a = outward from the opening, b = out of the wall (towards the room)
            pr.add(-.001f,-.001f);pr.add(.22f,-.001f);pr.add(.22f,.05f);pr.add(.18f,.09f);pr.add(.04f,.09f);pr.add(-.001f,.035f);
            sweep(surround,path,pr,.9f,[](Vec3 p){return .65f+.35f*smoothstep(0.f,.5f,p.y);});
            for(float y:{.35f,1.05f,1.75f}) {
                float half=y<dh?dw:std::sqrt(std::max(0.f,dw*dw-(y-dh)*(y-dh)));
                iron.box({-half+.03f,y,zf-.012f},{half-.03f,y+.055f,zf},.25f,.9f);
                for(float x=-half+.08f;x<half-.05f;x+=.14f) iron.box({x-.009f,y+.018f,zf-.02f},{x+.009f,y+.036f,zf-.012f},.1f,.9f); // rivets
            }
            std::vector<Vec3> ring;for(int i=0;i<=20;++i) {float a=2*Pi*i/20;ring.push_back({dw-.14f+.045f*std::cos(a),1.02f+.045f*std::sin(a),zf-.03f});}
            tube(iron,ring,.006f,8,.1f);
        }
        room.push_back(std::move(door));room.push_back(std::move(surround));
        // Torches: sconce (disc plate, curved arm, cup) and a turned wooden handle; six of them.
        Mesh sticks;sticks.tex=Wood;sticks.mat={.08f,12,1.f,.3f,{.5f,.4f,.34f}};
        struct Mount {Vec3 wall,out;};
        const Mount mounts[]={{{-1.45f,1.75f,Z0},{0,0,1}},{{1.45f,1.75f,Z0},{0,0,1}},{{X0,1.75f,-1.15f},{1,0,0}},{{X1,1.75f,-1.15f},{-1,0,0}},
                              {{-1.25f,1.75f,Z1},{0,0,-1}},{{1.25f,1.75f,Z1},{0,0,-1}}};
        int k=0;
        for(auto& m:mounts) {
            Vec3 side=cross(m.out,{0,1,0});
            Frame plate{add(m.wall,{0,-.02f,0}),side,m.out,{0,1,0}}; // lathe axis = out of the wall
            // lathe axis is local Y; map local (x,y,z) -> side, out, up
            Frame f{plate.o,side,m.out,{0,1,0}};
            Frame fl{plate.o,{side.x,side.y,side.z},{m.out.x,m.out.y,m.out.z},{0,1,0}};
            (void)fl;
            Frame disc{plate.o,side,m.out,cross(side,m.out)};
            lathe(iron,{{0,0},{.05f,0},{.052f,.006f},{.04f,.014f},{.012f,.02f},{0,.022f}},16,.2f,disc);
            std::vector<Vec3> arm={add(plate.o,scale(m.out,.02f)),add(add(plate.o,scale(m.out,.08f)),{0,.005f,0}),add(add(plate.o,scale(m.out,.13f)),{0,.03f,0}),add(add(plate.o,scale(m.out,.15f)),{0,.07f,0})};
            tube(iron,arm,.009f,8,.2f);
            Vec3 cupBase=add(add(plate.o,scale(m.out,.15f)),{0,.07f,0});
            Frame up{cupBase,side,{0,1,0},m.out};
            lathe(iron,{{0,0},{.02f,0},{.04f,.05f},{.045f,.06f},{.038f,.062f}},14,.2f,up);
            lathe(sticks,{{0,-.12f},{.012f,-.12f},{.016f,-.02f},{.022f,.06f},{.026f,.1f},{.02f,.11f},{0,.115f}},10,.3f,up);
            (void)f;
            torches.push_back({add(cupBase,{0,.14f,0}),float(k)*1.7f});++k;
        }
        room.push_back(std::move(iron));room.push_back(std::move(sticks));
    }
    // Table (table frame). floorT = floor height in the table frame.
    void buildTable(IDirect3DDevice9Ex* dev,const TableModel& m,float floorT) {
        table.clear();
        const float hw=m.halfWidth,hd=m.halfDepth,B=TableModel::Border,top=TableModel::Top,yb=m.bottom(),ox=m.outerX(),oz=m.outerZ();
        Mesh frame;frame.tex=Wood;frame.mat={.32f,34,1.f,.25f,{1.f,.95f,.9f}};
        {
            // Moulded top board around the opening: chamfered inner edge, flat top, rounded raised rim,
            // ogee outer edge, skirt. Counter-clockwise in (a = distance from the opening, b = height).
            Profile p;
            p.add(0,yb);p.add(B-.006f,yb);p.add(B-.006f,top-.032f);p.add(B-.002f,top-.026f);p.add(B,top-.018f);p.add(B,top-.004f);
            const float lw=TableModel::LipWidth,lh=TableModel::LipHeight;
            for(int i=0;i<=8;++i) {float t=Pi*i/8;p.add(B-lw*.5f+lw*.5f*std::cos(t),top+lh*std::sin(t)*(t<Pi*.5f?1.f:1.f));}
            p.add(B-lw-.004f,top);p.add(.006f,top);p.add(0,top-.006f);
            sweepRect(frame,hw,hd,0,p,.55f,true,[&](Vec3 q){return .75f+.25f*smoothstep(yb,top,q.y);});
        }
        table.push_back(std::move(frame));
        Mesh under;under.tex=Wood;under.mat={.18f,24,1.f,.3f,{.8f,.72f,.66f}};
        {
            // Apron with a bead at the bottom, set back 3.5 cm from the outer edge.
            Profile p;const float ah=.085f;
            p.add(-.022f,yb-ah+.004f);p.add(-.004f,yb-ah+.004f);p.add(0,yb-ah+.008f);p.add(.003f,yb-ah+.014f);p.add(0,yb-ah+.02f);p.add(0,yb);p.add(-.022f,yb);
            sweepRect(under,ox-.035f,oz-.035f,0,p,.55f,true,[&](Vec3 q){return .55f+.45f*smoothstep(yb-ah,yb,q.y);});
        }
        const float legInset=.07f;
        if(floorT<yb-.3f) {
            // Turned legs (lathe): foot, neck, vase, rings, collar under the apron.
            float L=yb-floorT;
            std::vector<std::pair<float,float>> prof;
            const float r[][2]={{0,0},{.026f,0},{.031f,.006f},{.033f,.018f},{.028f,.03f},{.021f,.05f},{.024f,.075f},{.02f,.1f},{.022f,.3f},
                                {.03f,.5f},{.032f,.58f},{.028f,.66f},{.025f,.7f},{.033f,.72f},{.034f,.74f},{.027f,.76f},{.03f,.8f},{.037f,.83f},{.037f,.995f},{0,1.f}};
            for(auto& q:r) prof.push_back({q[0],q[1]*L});
            for(float sx:{-1.f,1.f}) for(float sz:{-1.f,1.f}) {
                Frame f{{sx*(ox-legInset),floorT,sz*(oz-legInset)},{1,0,0},{0,1,0},{0,0,1}};
                lathe(under,prof,20,.6f,f,[&](Vec3 q){return .55f+.45f*smoothstep(floorT,floorT+.45f,q.y);});
            }
            // Turned stretchers: an H between the legs, low.
            float sy=floorT+.13f;
            std::vector<std::pair<float,float>> bar={{0,0},{.015f,0},{.017f,.03f},{.013f,.08f},{.014f,.5f},{.013f,.92f},{.017f,.97f},{.015f,1.f},{0,1.f}};
            auto stretch=[&](Vec3 a,Vec3 b) {
                Vec3 d=sub(b,a);float len=std::sqrt(dot(d,d));Vec3 y=scale(d,1/len);Vec3 x=std::fabs(y.y)<.9f?cross(y,{0,1,0}):Vec3{1,0,0};normalize(x);Vec3 z=cross(x,y);
                std::vector<std::pair<float,float>> p2;for(auto& q:bar) p2.push_back({q.first,q.second*len});
                lathe(under,p2,12,.6f,{a,x,y,z},[](Vec3){return .6f;});
            };
            for(float sx:{-1.f,1.f}) stretch({sx*(ox-legInset),sy,-(oz-legInset)},{sx*(ox-legInset),sy,oz-legInset});
            stretch({-(ox-legInset),sy+.02f,0},{ox-legInset,sy+.02f,0});
        }
        table.push_back(std::move(under));
        // Eight drop handles (two per side): rosette plates and a round iron bail hanging down.
        for(int i=0;i<8;++i) {
            Mesh h;h.tex=Iron;h.mat={.6f,40,1.f,.2f,{.8f,.78f,.76f}};
            int side=i/2;
            Vec3 out=TableModel::sideOut(side),lat=TableModel::sideLateral(side);
            float face=m.face(side);
            float l0=m.handleLateral(i);
            Vec3 c=add(scale(out,face),add(Vec3{0,TableModel::HandlePivot,0},scale(lat,l0)));
            for(float s:{-1.f,1.f}) {
                Vec3 pc=add(c,scale(lat,s*TableModel::HandleHalfSpan));
                Frame disc{pc,lat,out,Vec3{0,1,0}};
                lathe(h,{{0,0},{.011f,0},{.012f,.003f},{.008f,.006f},{.004f,.009f},{0,.01f}},12,.1f,disc);
            }
            // Bail: from one pivot, down with a rounded corner, across, and back up to the other pivot.
            std::vector<Vec3> pts;const float span=TableModel::HandleHalfSpan,drop=TableModel::HandleDropLength,rc=.012f,o=TableModel::HandleOut;
            auto P=[&](float l,float y){return add(scale(out,face+o),add(Vec3{0,TableModel::HandlePivot+y,0},scale(lat,l0+l)));};
            pts.push_back(P(-span,0));pts.push_back(P(-span,-(drop-rc)*.5f));pts.push_back(P(-span,-(drop-rc)));
            for(int k2=1;k2<=5;++k2) {float t=Pi*.5f*k2/5;pts.push_back(P(-span+rc-rc*std::cos(t),-(drop-rc)-rc*std::sin(t)));}
            pts.push_back(P(0,-drop));
            for(int k2=0;k2<5;++k2) {float t=Pi*.5f*k2/5;pts.push_back(P(span-rc+rc*std::sin(t),-drop+rc-rc*std::cos(t)));}
            pts.push_back(P(span,-(drop-rc)));pts.push_back(P(span,-(drop-rc)*.5f));pts.push_back(P(span,0));
            tube(h,pts,.0035f,8,.08f);
            table.push_back(std::move(h));
        }
        // 0.4.4: wooden bottom of the tray under the dungeon (replaces the dark plate): planks across the
        // width, slightly darker than the frame, just below the dungeon floor. Drawn without depth writes.
        tray=Mesh{};tray.tex=Wood;tray.mat={.14f,20,1.f,.3f,{.72f,.62f,.54f}};
        // 0.4.5: at the foot of the frame's inner chamfer and 5 mm under the frame on every side: no slit
        // between the frame and the tray at grazing angles (0.4.4 had it 1 mm above the frame -> gap).
        const float ty=top-.006f,ov=.005f;
        tray.quad({-hw-ov,ty,-hd-ov},{2*(hw+ov),0,0},{0,0,2*(hd+ov)},{0,1,0},.9f,.9f,.13f,.41f,8,6,[&](Vec3 q){
            float e=std::min(std::min(q.x+hw,hw-q.x),std::min(q.z+hd,hd-q.z));return .7f+.3f*smoothstep(0.f,.06f,e);});
        shadow=Mesh{};shadow.tex=Shadow;
        const float e=.28f;
        shadow.quad({-ox-e,floorT+.003f,-oz-e},{2*(ox+e),0,0},{0,0,2*(oz+e)},{0,1,0},2*(ox+e),2*(oz+e),0,0,1,1);
        if(!shaders) {auto l=baseLights({0,1,-.8f});for(auto& t:table) bake(t,l.data(),int(l.size()),{.06f,.06f,.07f},nullptr);bake(tray,l.data(),int(l.size()),{.06f,.06f,.07f},nullptr);}
        for(auto& t:table) toBuffer(dev,t);
        toBuffer(dev,shadow);toBuffer(dev,tray);
        log("Table built: %.2f x %.2f m, legs %.2f m to the floor",2*ox,2*oz,std::max(0.f,yb-floorT));
    }
    struct FrameState {
        Pose room{},table{};bool haveTable=false;int handleState[8]{};float time=0;
        std::vector<Light> lights;std::vector<float> strength;
        struct Sprite {Vec3 p;DWORD c;float u,v;};
        std::vector<Sprite> flames,glows;
    };
    bool prepare(IDirect3DDevice9Ex* dev,const Request& r,float aspect,float fallbackDepthRatio,FrameState& f) {
        if(!ready) return false;
        f.time=float(milliseconds()*.001);
        const bool floorKnown=r.handFlags&RequestFloorKnown;
        const float floorY=floorKnown?r.floorY:r.table.position.y-.78f;
        if(int(floorKnown)!=floorLogged) {floorLogged=int(floorKnown);log("Room floor: %s",floorKnown?"real floor from the headset (STAGE space)":"estimated 78 cm under the table");}
        f.room={r.anchor.orientation,{r.anchor.position.x,floorY,r.anchor.position.z}};
        f.table=r.table;f.haveTable=r.tableAnchor!=0;
        TableModel m=tableModel(r.tableWidthMeters,aspect>0?aspect:fallbackDepthRatio);
        float floorT=floorY-r.table.position.y;
        // Rebuilt only when the table size or its height above the floor really changes (not while carried at the same height).
        if(std::fabs(m.halfWidth-tableKeyW)>1e-4f||std::fabs(m.halfDepth-tableKeyD)>1e-4f||std::fabs(floorT-tableKeyFloor)>.015f) {
            buildTable(dev,m,floorT);tableKeyW=m.halfWidth;tableKeyD=m.halfDepth;tableKeyFloor=floorT;
        }
        for(int i=0;i<8;++i) {
            Vec3 hp=toWorld(r.table,m.handle(i));int st=0;
            for(int h=0;h<2;++h) {
                if(!(r.handFlags&(h?HandRightValid:HandLeftValid))) continue;
                Vec3 dd=sub(r.hands[h].position,hp);float d2=dot(dd,dd);
                if(r.handFlags&(h?HandRightTable:HandLeftTable)) {if(d2<.2f*.2f) st=2;}
                else if(d2<TableModel::GrabRadius*TableModel::GrabRadius&&st<1) st=1;
            }
            f.handleState[i]=st;
        }
        Vec3 tableInRoom=toLocal(f.room,r.table.position);
        f.lights=baseLights(tableInRoom);f.strength.clear();
        for(size_t i=0;i<f.lights.size();++i) f.strength.push_back(i<torches.size()?flicker(f.time,f.lights[i].phase):1.f);
        // Flames: head-facing sprites and a soft glow, in the room frame.
        f.flames.clear();f.glows.clear();
        Vec3 head=toLocal(f.room,scale(add(r.eyes[0].pose.position,r.eyes[1].pose.position),.5f));
        for(auto& t:torches) {
            float k=flicker(f.time*1.3f,t.phase*1.9f);
            Vec3 to=sub(head,t.flame);to.y=0;if(!normalize(to)) to={0,0,1};
            Vec3 right=cross({0,1,0},to);
            float w=.07f*k,h=.21f*(.85f+.3f*(k-.85f)),sway=.012f*std::sin(f.time*3.1f+t.phase);
            Vec3 base=add(t.flame,{0,-.07f,0}),topc=add(base,add(Vec3{0,h,0},scale(right,sway)));
            Vec3 a=sub(base,scale(right,w)),b=add(base,scale(right,w)),c=add(topc,scale(right,w)),d=sub(topc,scale(right,w));
            const DWORD white=0xffffffff;
            for(auto& q:{FrameState::Sprite{a,white,0,1},{b,white,1,1},{c,white,1,0},{a,white,0,1},{c,white,1,0},{d,white,0,0}}) f.flames.push_back(q);
            Vec3 up=cross(to,right);float g=.42f*k;Vec3 center=add(t.flame,{0,.01f,0});const DWORD gc=0x80ffffff;
            Vec3 ga=add(center,add(scale(right,-g),scale(up,-g))),gb=add(center,add(scale(right,g),scale(up,-g)));
            Vec3 gcn=add(center,add(scale(right,g),scale(up,g))),gd=add(center,add(scale(right,-g),scale(up,g)));
            for(auto& q:{FrameState::Sprite{ga,gc,0,1},{gb,gc,1,1},{gcn,gc,1,0},{ga,gc,0,1},{gcn,gc,1,0},{gd,gc,0,0}}) f.glows.push_back(q);
        }
        return true;
    }
    void commonStates(IDirect3DDevice9* d) {
        d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
        d->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
        d->SetRenderState(D3DRS_CLIPPLANEENABLE,0);d->SetRenderState(D3DRS_STENCILENABLE,FALSE);d->SetRenderState(D3DRS_COLORWRITEENABLE,15);
        d->SetRenderState(D3DRS_SRGBWRITEENABLE,FALSE);d->SetRenderState(D3DRS_WRAP0,0);d->SetRenderState(D3DRS_WRAP1,0);
        d->SetRenderState(D3DRS_LIGHTING,FALSE);d->SetRenderState(D3DRS_FOGENABLE,FALSE);d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
        for(DWORD s=0;s<2;++s) {
            d->SetSamplerState(s,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);d->SetSamplerState(s,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
            d->SetSamplerState(s,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);d->SetSamplerState(s,D3DSAMP_MINFILTER,D3DTEXF_ANISOTROPIC);
            d->SetSamplerState(s,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);d->SetSamplerState(s,D3DSAMP_MAXANISOTROPY,8);
            d->SetSamplerState(s,D3DSAMP_MIPMAPLODBIAS,0);d->SetSamplerState(s,D3DSAMP_MAXMIPLEVEL,0);d->SetSamplerState(s,D3DSAMP_SRGBTEXTURE,0);
        }
        d->SetTexture(2,nullptr);
    }
    void fixedStates(IDirect3DDevice9* d) {
        d->SetVertexShader(nullptr);d->SetPixelShader(nullptr);
        d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE2X);d->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);d->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
        d->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);d->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);d->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
        d->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);d->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);d->SetTextureStageState(0,D3DTSS_RESULTARG,D3DTA_CURRENT);
        d->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);d->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);d->SetTexture(1,nullptr);
    }
    // Draws meshes of one frame (room or table): objToRoom maps the mesh frame to the room (lighting) frame.
    void drawMeshes(IDirect3DDevice9* d,std::vector<Mesh*>& list,const float* objToEye,const float* proj,const float* objToRoom,const FrameState& f,const Vec3& eyeRoom) {
        float wvp[16];mul4(objToEye,proj,wvp);
        if(shaders) {
            d->SetVertexShader(vs.Get());d->SetPixelShader(ps.Get());
            d->SetVertexShaderConstantF(0,wvp,4);d->SetVertexShaderConstantF(4,objToRoom,4);
            float lp[8][4]{},lc[8][4]{};
            for(size_t i=0;i<f.lights.size()&&i<8;++i) {
                const Light& l=f.lights[i];float k=l.intensity*f.strength[i];
                lp[i][0]=l.pos.x;lp[i][1]=l.pos.y;lp[i][2]=l.pos.z;lp[i][3]=2.2f/(l.range*l.range);
                lc[i][0]=l.color.r*k;lc[i][1]=l.color.g*k;lc[i][2]=l.color.b*k;
            }
            for(size_t i=f.lights.size();i<8;++i) lp[i][1]=-100;
            d->SetPixelShaderConstantF(0,&lp[0][0],8);d->SetPixelShaderConstantF(8,&lc[0][0],8);
            const float ambient[4]={.045f,.043f,.052f,0},eye[4]={eyeRoom.x,eyeRoom.y,eyeRoom.z,1},fog[4]={.018f,.016f,.02f,.07f};
            d->SetPixelShaderConstantF(16,ambient,1);d->SetPixelShaderConstantF(17,eye,1);d->SetPixelShaderConstantF(20,fog,1);
        } else {
            fixedStates(d);D3DMATRIX id{};id._11=id._22=id._33=id._44=1;
            D3DMATRIX w;memcpy(&w,objToEye,64);d->SetTransform(D3DTS_WORLD,&w);
        }
        d->SetVertexDeclaration(decl.Get());
        for(Mesh* m:list) {
            if(!m->vb||!m->count) continue;
            if(shaders) {
                const float mat[4]={m->mat.specular,m->mat.shininess,m->mat.normal,m->mat.wrap},tint[4]={m->mat.tint.r,m->mat.tint.g,m->mat.tint.b,1};
                d->SetPixelShaderConstantF(18,mat,1);d->SetPixelShaderConstantF(19,tint,1);
                d->SetTexture(1,normals[m->tex].Get());
            }
            d->SetTexture(0,albedo[m->tex].Get());
            d->SetStreamSource(0,m->vb.Get(),0,sizeof(Vtx));
            d->DrawPrimitive(D3DPT_TRIANGLELIST,0,m->count/3);
        }
        d->SetStreamSource(0,nullptr,0,0);
    }
    // Opaque room and table, then the contact shadow. eye: eye pose in LOCAL; proj: projection.
    void drawOpaque(IDirect3DDevice9* d,const FrameState& f,const Pose& eye,const D3DMATRIX& proj) {
        commonStates(d);
        Similarity eyeInv=inverse(similarity(eye));
        float roomToEye[16],tableToEye[16],identity[16]={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1},tableToRoom[16];
        matrixOf(then(similarity(f.room),eyeInv),roomToEye);
        matrixOf(then(similarity(f.table),eyeInv),tableToEye);
        matrixOf(then(similarity(f.table),inverse(similarity(f.room))),tableToRoom);
        Vec3 eyeRoom=toLocal(f.room,eye.position);
        std::vector<Mesh*> list;for(auto& m:room) list.push_back(&m);
        drawMeshes(d,list,roomToEye,&proj._11,identity,f,eyeRoom);
        if(!f.haveTable) {d->SetVertexShader(nullptr);d->SetPixelShader(nullptr);return;}
        list.clear();
        for(size_t i=0;i<table.size();++i) {
            // Handles: tint changes with hover/held (meshes 2.. are the eight handles).
            if(i>=2) {int st=f.handleState[i-2];table[i].mat.tint=st==2?Color{2.f,1.55f,.75f}:st==1?Color{1.7f,1.7f,1.7f}:Color{.8f,.78f,.76f};}
            list.push_back(&table[i]);
        }
        drawMeshes(d,list,tableToEye,&proj._11,tableToRoom,f,eyeRoom);
        // Shadow multiplied onto the floor (fixed function).
        d->SetVertexShader(nullptr);d->SetPixelShader(nullptr);fixedStates(d);
        D3DMATRIX w;memcpy(&w,tableToEye,64);d->SetTransform(D3DTS_WORLD,&w);
        d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);d->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
        d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ZERO);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_SRCCOLOR);d->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
        d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
        d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
        if(shadow.vb) {d->SetVertexDeclaration(decl.Get());d->SetTexture(0,albedo[Shadow].Get());d->SetStreamSource(0,shadow.vb.Get(),0,sizeof(Vtx));d->DrawPrimitive(D3DPT_TRIANGLELIST,0,shadow.count/3);d->SetStreamSource(0,nullptr,0,0);}
        d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
    }
    // 0.4.4: wooden tray bottom, drawn after the table and before the dungeon, depth tested (the rim stays in
    // front) but not written (pits and lava below the table surface are drawn over it).
    bool drawTray(IDirect3DDevice9* d,const FrameState& f,const Pose& eye,const D3DMATRIX& proj) {
        if(!f.haveTable||!tray.vb) return false;
        commonStates(d);
        Similarity eyeInv=inverse(similarity(eye));
        float tableToEye[16],tableToRoom[16];
        matrixOf(then(similarity(f.table),eyeInv),tableToEye);
        matrixOf(then(similarity(f.table),inverse(similarity(f.room))),tableToRoom);
        d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
        std::vector<Mesh*> list{&tray};
        drawMeshes(d,list,tableToEye,&proj._11,tableToRoom,f,toLocal(f.room,eye.position));
        d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
        d->SetVertexShader(nullptr);d->SetPixelShader(nullptr);
        return true;
    }
    void drawFlames(IDirect3DDevice9* d,const FrameState& f,const Pose& eye) {
        if(f.flames.empty()) return;
        commonStates(d);fixedStates(d);
        float roomToEye[16];matrixOf(then(similarity(f.room),inverse(similarity(eye))),roomToEye);
        D3DMATRIX w;memcpy(&w,roomToEye,64);d->SetTransform(D3DTS_WORLD,&w);
        d->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
        d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);d->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
        d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);d->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
        d->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
        d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
        d->SetTexture(0,albedo[Glow].Get());
        d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(f.glows.size()/3),f.glows.data(),sizeof(FrameState::Sprite));
        d->SetTexture(0,albedo[Flame].Get());
        d->DrawPrimitiveUP(D3DPT_TRIANGLELIST,UINT(f.flames.size()/3),f.flames.data(),sizeof(FrameState::Sprite));
        d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);d->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
    }
};
// One scene for the process: prepared from the game start, used by the VR thread.
inline Scene& globalScene() {static Scene* s=new Scene();return *s;}
}
