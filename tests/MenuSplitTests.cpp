// DK2VR 0.8.95 / 0.8.96 - offline check of the menu 2D / decor separation (src/common/MenuSplit.hpp) on synthetic frames:
// decor (3D-like gradients + texture), wrapper colour path (exact 32-bit as measured in the game, or 565 quantisation / gamma),
// 2D interface drawn over it (opaque text strokes with a soft glow, translucent dark band, cursor).
// Measures: interface pixels found, decor pixels wrongly taken, the soft layer over the decor gives back the final image,
// opaque text stays opaque.
#include "../src/common/MenuSplit.hpp"
#include <cstdio>
#include <cmath>
#include <random>
#include <chrono>

static int fails=0;
static void check(bool ok,const char* what,double v,double lim) {printf("%s: %s %.4f (limit %.4f)\n",ok?"PASS":"FAIL",what,v,lim);if(!ok) ++fails;}

static uint32_t rgb(int r,int g,int b) {return uint32_t(std::clamp(r,0,255))<<16|uint32_t(std::clamp(g,0,255))<<8|uint32_t(std::clamp(b,0,255));}
static int ch(uint32_t c,int k) {return int((c>>(16-8*k))&255);}
static int q5(int v) {int q=v>>3;return (q<<3)|(q>>2);}
static int q6(int v) {int q=v>>2;return (q<<2)|(q>>4);}

struct Scene {
    int W=640,H=480;std::vector<uint32_t> decor,fin;std::vector<uint8_t> truth,core;   // core: opaque text
    // path: 0 exact (the game, log 20261006_015307), 1 = 565, 2 = 565 + gamma .8
    void make(int frame,int path,double bandCover,unsigned seed) {
        std::mt19937 rng(seed+frame*7919);std::uniform_int_distribution<int> noise(-6,6);
        decor.assign(size_t(W)*H,0);fin.assign(size_t(W)*H,0);truth.assign(size_t(W)*H,0);core.assign(size_t(W)*H,0);
        for(int y=0;y<H;++y) for(int x=0;x<W;++x) {   // moving 3D-like decor
            const double u=(x+frame*3)/640.,v=y/480.;
            int r=int(90+80*std::sin(u*7+v*3)+noise(rng)),g=int(60+50*std::cos(u*5-v*4)+noise(rng)),b=int(40+30*std::sin(u*11+v*9)+noise(rng));
            decor[size_t(y)*W+x]=rgb(r,g,b);
        }
        auto conv=[&](uint32_t c){
            if(path==0) return c;
            int r=q5(ch(c,0)),g=q6(ch(c,1)),b=q5(ch(c,2));
            if(path==2) {r=int(255*std::pow(r/255.,.8)+.5);g=int(255*std::pow(g/255.,.8)+.5);b=int(255*std::pow(b/255.,.8)+.5);}
            return rgb(r,g,b);};
        for(size_t i=0;i<fin.size();++i) fin[i]=conv(decor[i]);
        auto blend=[&](size_t i,int r,int g,int b,double a){const uint32_t c=fin[i];
            fin[i]=rgb(int(a*r+(1-a)*ch(c,0)+.5),int(a*g+(1-a)*ch(c,1)+.5),int(a*b+(1-a)*ch(c,2)+.5));truth[i]=1;};
        // translucent dark band (menu panel) covering bandCover of the height, 50 % black
        const int bh=int(H*bandCover),by=(H-bh)/2;
        for(int y=by;y<by+bh;++y) for(int x=60;x<580;++x) blend(size_t(y)*W+x,0,0,0,.5);
        // text strokes 1-3 px wide, opaque core (grey-white / gold), orange glow 2 px around (40 % -> 15 %)
        for(int line=0;line<8;++line) {const int ty=60+line*48;
            for(int k=0;k<40;++k) {const int tx=100+k*11;const int sw=1+(k%3);
                for(int y=ty-2;y<ty+16;++y) for(int x=tx-2;x<tx+sw+2;++x) {
                    const bool in=y>=ty&&y<ty+14&&x>=tx&&x<tx+sw;const size_t i=size_t(y)*W+x;
                    if(in) {if(line%2) blend(i,215,215,215,1.); else blend(i,230,190,60,1.);core[i]=1;}
                    else {const int dd=std::max(std::max(tx-x,x-(tx+sw-1)),std::max(ty-y,y-(ty+13)));blend(i,255,140,30,dd<=1?.40:.15);}
                }}}
        // cursor 20x30
        for(int y=200;y<230;++y) for(int x=300;x<320;++x) {const size_t i=size_t(y)*W+x;blend(i,200,200,180,1.);core[i]=1;}
    }
};

static void runCase(const char* name,int path,double band,int frames,bool learn,int tolerance) {
    MenuSplit m;m.learn=learn;m.tolerance=tolerance;Scene s;std::vector<uint32_t> out;double ms=0;
    for(int f=0;f<frames;++f) {s.make(f,path,band,1234);const auto t0=std::chrono::steady_clock::now();m.run(s.decor.data(),s.fin.data(),640,480,out);
        ms+=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-t0).count();}
    // interface found / decor wrongly taken
    uint64_t t=0,found=0,dn=0,fp=0,cn=0;double coreAlpha=0;int worst=0;
    for(size_t i=0;i<out.size();++i) {
        const int a=int(out[i]>>24);
        if(s.truth[i]) {++t;if(a) ++found;} else {++dn;if(a) ++fp;}
        if(s.core[i]) {++cn;coreAlpha+=a/255.;}
        // soft layer over the (mapped) decor = final image
        const uint32_t d=s.decor[i];
        for(int k=0;k<3;++k) {const int dk=int(m.lut[k][ch(d,k)]);const int comp=int(std::lround(ch(out[i],k)+(1.-a/255.)*dk));
            if(a) worst=std::max(worst,std::abs(comp-ch(s.fin[i],k)));}
    }
    const double rec=t?double(found)/t:1.,fpr=dn?double(fp)/dn:0.,ca=cn?coreAlpha/cn:1.;
    printf("-- %s: %.2f ms/frame, interface %.1f %%, mean alpha %.2f, mapping deviation %.2f\n",name,ms/frames,100*m.last.uiFraction,m.last.meanAlpha,m.last.lutDeviation);
    char a[160];
    snprintf(a,sizeof a,"%s interface found",name);check(rec>=.97,a,rec,.97);
    snprintf(a,sizeof a,"%s decor taken as interface",name);check(fpr<=.01,a,fpr,.01);
    snprintf(a,sizeof a,"%s layer over decor = final image (worst level error)",name);check(worst<=2,a,worst,2);
    snprintf(a,sizeof a,"%s opaque text alpha",name);check(ca>=.90,a,ca,.90);
}

int main() {
    runCase("exact 32-bit",0,.25,6,false,3);
    runCase("exact 32-bit, band 60 %",0,.60,6,false,3);
    runCase("565 path, tolerance 8",1,.25,6,false,8);
    runCase("learned, 565 + gamma .8",2,.25,30,true,3);
    {MenuSplit m;Scene s;s.make(0,0,0,99);std::vector<uint32_t> out;m.run(s.decor.data(),s.decor.data(),640,480,out);
     check(m.last.uiPixels==0,"identical images -> no interface",double(m.last.uiPixels),0);}
    printf(fails?"MenuSplitTests: %d FAILED\n":"PASS: MenuSplitTests (menu 2D / decor separation, soft layer)\n",fails);
    return fails?1:0;
}
