#pragma once
// DK2VR 0.8.95 - separation of the main menu's 2D interface from its 3D decor, on the CPU, pixel exact.
//
// In the frontend the game renders its 3D decor with Direct3D into a 640x480 target; the wrapper reads it back into system memory
// (GetRenderTargetData + read-only lock) and only then the game draws its 2D interface (text, logo, buttons, cursor) in software.
// The wrapper then writes the final 640x480 image (decor + interface) into a system-memory surface that is presented.
// Both images of the SAME frame are compared here:
//   decor  = the read-back (3D only),
//   final  = the last whole 640x480 surface written before Present (decor + 2D).
// The wrapper may quantise the colours on the way (16-bit surface of the game = 565: at most 7 levels): by default the mapping is
// the identity with a tolerance of 12 levels. Optionally (learn=true, INI MenuUiLearn=1) a per-channel mapping decor value -> final
// value is learned (most frequent final value for each decor value within maxShift levels, accumulated over many frames, starting
// from the identity, kept non-decreasing). Offline test: an unrestricted learned mapping is taken over by a translucent panel that
// covers most of the screen (it then hides the panel), hence the identity default and the maxShift window.
// A pixel whose final colour is not the mapped decor colour (tolerance) is interface. The distribution of the colour differences is
// measured on every frame (Stats::errBins) so that the logs show what the wrapper really does to the colours.
// 0.8.96: the layer is SOFT (alpha recovered from the exact difference, see run()): drawn over the decor it reproduces the final image
// exactly; glows, anti-aliased edges and translucent panels keep the 3D relief visible behind them.
// Pure C++ (no Windows): checked offline by tests/MenuSplitTests.cpp.
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

struct MenuSplit {
    int tolerance=3;           // levels (0..255) between the final colour and the mapped decor colour still counted as decor (0.8.95 log: 89-99 % of the pixels differ by exactly 0)
    float alphaGain=1.5f;      // 0.8.96: soft layer, minimal alpha x gain (1 = most transparent possible, higher = more opaque text)
    int sampleStep=2;          // learning samples: one pixel in sampleStep x sampleStep
    bool learn=false;          // false: the mapping stays the identity (default)
    int maxShift=32;           // learn=true: a decor level maps to a final level at most maxShift away
    uint32_t priorWeight=48;   // identity counts put in every row at the start (a row needs that many real samples to move)
    uint32_t halveAt=1u<<16;   // a row whose total reaches this is halved (slow forgetting, the mapping is a property of the wrapper)

    struct Stats {
        double uiFraction=0;    // fraction of the image in the interface layer
        double lutDeviation=0;  // mean |mapping(d) - d| over the rows with data, all channels (0 = identity)
        int lutMaxDeviation=0;  // largest |mapping(d) - d|
        double decorResidual=0; // mean |final - mapping(decor)| over the decor pixels (sampled), levels
        uint32_t rowsWithData=0;// rows (of 768) that have real samples
        uint32_t uiPixels=0;
        double meanAlpha=0;     // mean alpha of the interface pixels (0.8.96 soft layer)
        double errBins[6]{};    // fraction of the sampled pixels whose largest channel difference (final - mapped decor) is 0, <=4, <=8, <=12, <=24, >24
        double meanDiff[3]{};   // mean signed difference final - decor per channel (R, G, B), sampled pixels
    };
    Stats last;
    uint8_t lut[3][256]{};

    void reset() {hist.clear();rowTotal.clear();}

    // decor, finalImg: w*h pixels 0x??RRGGBB (high byte ignored). out: w*h premultiplied ARGB (0 = transparent).
    bool run(const uint32_t* decor,const uint32_t* finalImg,uint32_t w,uint32_t h,std::vector<uint32_t>& out) {
        if(!decor||!finalImg||w<8||h<8) return false;
        const size_t n=size_t(w)*h;
        if(learn) {prepare();accumulate(decor,finalImg,w,h);buildLut();}
        else {for(int c=0;c<3;++c) for(int d=0;d<256;++d) lut[c][d]=uint8_t(d);last.lutDeviation=0;last.lutMaxDeviation=0;last.rowsWithData=0;}
        // 1) raw mask
        mask.assign(n,0);   // 1 = interface
        double residual=0;uint64_t residualN=0;uint64_t bins[6]{},binN=0;double sd[3]{};
        for(size_t i=0;i<n;++i) {
            const uint32_t d=decor[i],f=finalImg[i];
            const int e0=std::abs(int((f>>16)&255)-int(lut[0][(d>>16)&255]));
            const int e1=std::abs(int((f>>8)&255)-int(lut[1][(d>>8)&255]));
            const int e2=std::abs(int(f&255)-int(lut[2][d&255]));
            const int e=std::max(e0,std::max(e1,e2));
            if(e>tolerance) mask[i]=1;
            else if((i&15)==0) {residual+=e0+e1+e2;residualN+=3;}
            if((i&15)==0) {++binN;++bins[e==0?0:e<=4?1:e<=8?2:e<=12?3:e<=24?4:5];
                sd[0]+=int((f>>16)&255)-int((d>>16)&255);sd[1]+=int((f>>8)&255)-int((d>>8)&255);sd[2]+=int(f&255)-int(d&255);}
        }
        for(int k=0;k<6;++k) last.errBins[k]=binN?double(bins[k])/binN:0.;
        for(int k=0;k<3;++k) last.meanDiff[k]=binN?sd[k]/binN:0.;
        // 2) 0.8.96: SOFT interface layer (alpha matting from the exact difference). The game blends its 2D over the decor: final = a*U + (1-a)*decor.
        //    For each interface pixel the smallest alpha that keeps U a valid colour is taken (darker channel: 1 - f/d, brighter: (f-d)/(255-d)),
        //    times alphaGain (more opaque text cores), at most 1. Premultiplied colour = final - (1-a)*decor: over the decor it gives back the final
        //    image EXACTLY, and glows / anti-aliased edges / translucent panels let the 3D relief show through instead of a flat cut-out halo
        //    (0.8.95: binary mask grown by one pixel = 'texte detoure grossierement', report 20261006_015307).
        out.assign(n,0u);uint32_t ui=0;double alphaSum=0;
        for(size_t i=0;i<n;++i) {
            if(!mask[i]) continue;
            const uint32_t dp=decor[i],fp=finalImg[i];
            const int f[3]={int((fp>>16)&255),int((fp>>8)&255),int(fp&255)};
            const int d[3]={int(lut[0][(dp>>16)&255]),int(lut[1][(dp>>8)&255]),int(lut[2][dp&255])};
            float amin=0.f;
            for(int c=0;c<3;++c) {
                if(f[c]<d[c]) amin=std::max(amin,1.f-float(f[c])/float(d[c]));
                else if(f[c]>d[c]) amin=std::max(amin,float(f[c]-d[c])/float(std::max(1,255-d[c])));
            }
            const float a=std::min(1.f,amin*alphaGain);
            const int ab=int(a*255.f+.5f);if(ab<=0) continue;
            const float af=float(ab)/255.f;
            uint32_t px=uint32_t(ab)<<24;
            for(int c=0;c<3;++c) {const int v=int(std::lround(float(f[c])-(1.f-af)*float(d[c])));px|=uint32_t(std::clamp(v,0,ab))<<(16-8*c);}
            out[i]=px;++ui;alphaSum+=af;
        }
        last.meanAlpha=ui?alphaSum/ui:0.;
        last.uiPixels=ui;last.uiFraction=double(ui)/double(n);
        last.decorResidual=residualN?residual/double(residualN):0.;
        return true;
    }

private:
    std::vector<uint32_t> hist;      // 3 x 256 x 256: hist[(c*256+d)*256+f]
    std::vector<uint32_t> rowTotal;  // 3 x 256
    std::vector<uint8_t> mask;
    void prepare() {
        if(hist.size()==size_t(3)*256*256) return;
        hist.assign(size_t(3)*256*256,0);rowTotal.assign(3*256,0);
        for(int c=0;c<3;++c) for(int d=0;d<256;++d) {hist[(size_t(c)*256+d)*256+d]=priorWeight;rowTotal[size_t(c)*256+d]=priorWeight;}
    }
    void add(int c,int d,int f) {
        const size_t r=size_t(c)*256+d;uint32_t* row=&hist[r*256];
        ++row[f];
        if(++rowTotal[r]>=halveAt) {uint32_t t=0;for(int k=0;k<256;++k) {row[k]>>=1;t+=row[k];}rowTotal[r]=t;}
    }
    void accumulate(const uint32_t* decor,const uint32_t* finalImg,uint32_t w,uint32_t h) {
        const uint32_t s=uint32_t(std::max(1,sampleStep));
        for(uint32_t y=0;y<h;y+=s) for(uint32_t x=0;x<w;x+=s) {
            const size_t i=size_t(y)*w+x;const uint32_t d=decor[i],f=finalImg[i];
            add(0,int((d>>16)&255),int((f>>16)&255));add(1,int((d>>8)&255),int((f>>8)&255));add(2,int(d&255),int(f&255));
        }
    }
    void buildLut() {
        double dev=0;uint32_t devN=0;int devMax=0;uint32_t rows=0;
        for(int c=0;c<3;++c) {
            int mode[256];bool real[256];
            for(int d=0;d<256;++d) {
                const uint32_t* row=&hist[(size_t(c)*256+d)*256];
                int best=d;uint32_t bestN=0;
                const int f0=std::max(0,d-maxShift),f1=std::min(255,d+maxShift);
                for(int f=f0;f<=f1;++f) if(row[f]>bestN||(row[f]==bestN&&std::abs(f-d)<std::abs(best-d))) {bestN=row[f];best=f;}
                mode[d]=best;real[d]=rowTotal[size_t(c)*256+d]>priorWeight;
            }
            // non-decreasing: a wrapper colour conversion never inverts the order of two levels; a row that breaks it is pulled to its neighbours
            for(int d=1;d<256;++d) if(mode[d]<mode[d-1]) mode[d]=mode[d-1];
            for(int d=0;d<256;++d) {
                lut[c][d]=uint8_t(std::clamp(mode[d],0,255));
                if(real[d]) {const int e=std::abs(mode[d]-d);dev+=e;++devN;devMax=std::max(devMax,e);++rows;}
            }
        }
        last.lutDeviation=devN?dev/devN:0.;last.lutMaxDeviation=devMax;last.rowsWithData=rows;
    }
};
