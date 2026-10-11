#pragma once
// DK2VR 0.9.47.61 - PC mirror window: ONE complete image (one eye), never stretched. Made by LoVeMaKeRz.
// fill=true  : the eye covers the whole 16:9 window (same scale on both axes, the extra top / bottom is cropped around 'centreY').
// fill=false : the whole eye is visible, black bars left and right.
#include <algorithm>
#include <cmath>
namespace dk2vr {
struct MirrorRect {float x,y,w,h;};
// Destination rectangle of the eye image inside the window (may extend past the window when fill: the window clips it).
inline MirrorRect mirrorLayout(float backW,float backH,float eyeW,float eyeH,bool fill,float centreY=.5f) {
    if(!(backW>=1.f)||!(backH>=1.f)||!(eyeW>=1.f)||!(eyeH>=1.f)) return {0,0,0,0};
    const float s=fill?std::max(backW/eyeW,backH/eyeH):std::min(backW/eyeW,backH/eyeH);
    const float w=eyeW*s,h=eyeH*s;
    float y=(backH-h)*.5f;
    if(fill&&h>backH) {   // crop: keep the window centred on 'centreY' of the eye height, never showing outside the image
        const float c=std::clamp(centreY,0.f,1.f);
        y=std::clamp(backH*.5f-c*h,backH-h,0.f);
    }
    return {(backW-w)*.5f,y,w,h};
}
// Client size that keeps 16:9 when the user drags a window edge: the width decides, except when only the height moved.
inline void mirrorKeep169(int& w,int& h,bool heightDrives) {
    if(w<320) w=320;if(h<180) h=180;
    if(heightDrives) w=(h*16+4)/9; else h=(w*9+8)/16;
}
}
