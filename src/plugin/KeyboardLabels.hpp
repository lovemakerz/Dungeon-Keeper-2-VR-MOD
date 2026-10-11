#pragma once
// made by LoVeMaKeRz
// 0.9.47.34: labels of the VR keyboard keys, taken from the player's Windows keyboard layout (AZERTY, QWERTY, QWERTZ ...). Keys are physical
// (scan codes, Keyboard.hpp): what a key shows and types depends only on the layout, exactly as on the real keyboard.
#include <windows.h>
#include "../common/Keyboard.hpp"
#include <cstring>

namespace dk2vr {
// Character produced by a scan code with / without SHIFT in the given layout, folded to something the pixel font can draw
// (letters upper-case, accents removed). 0 = nothing printable (dead key, control).
inline char kbFold(wchar_t c) {
    if(c>=32&&c<127) {if(c>='a'&&c<='z') return char(c-'a'+'A');return char(c);}
    switch(c) {
    case 0xE0:case 0xE1:case 0xE2:case 0xE4:case 0xC0:case 0xC2:case 0xC4: return 'A';
    case 0xE7:case 0xC7: return 'C';
    case 0xE8:case 0xE9:case 0xEA:case 0xEB:case 0xC8:case 0xC9:case 0xCA:case 0xCB: return 'E';
    case 0xEE:case 0xEF:case 0xCE:case 0xCF: return 'I';
    case 0xF4:case 0xF6:case 0xD4:case 0xD6: return 'O';
    case 0xF9:case 0xFB:case 0xFC:case 0xD9:case 0xDB:case 0xDC: return 'U';
    case 0xB0: return '*';   // degree sign: drawn as a small star
    case 0xB5: return 'M';
    case 0xA3: return '#';   // pound
    case 0xA7: return '$';
    case 0xB2: return '2';
    case 0xDF: return 'S';
    default: return 0;
    }
}
inline char kbChar(HKL hkl,uint8_t scan,bool shift) {
    const UINT vk=MapVirtualKeyExW(scan,MAPVK_VSC_TO_VK,hkl);
    if(!vk) return 0;
    BYTE state[256]{};if(shift) state[VK_SHIFT]=state[VK_LSHIFT]=0x80;
    wchar_t out[4]{};
    const int n=ToUnicodeEx(vk,scan,state,out,4,0x4,hkl);   // 0x4: do not change the keyboard state (dead keys stay pending in the game)
    if(n==1) return kbFold(out[0]);
    if(n<0) {BYTE none[256]{};wchar_t flush[4]{};ToUnicodeEx(VK_SPACE,0x39,none,flush,4,0x4,hkl);}   // dead key: nothing to show
    return 0;
}
// AZERTY digit row: the unshifted key gives & e " ' ( - e _ c a and SHIFT gives the digit. These keys are sent WITH shift (and shown as digits) so that
// the player types digits; arming SHIFT on the keyboard gives the symbol instead.
inline bool kbDigitAuto(HKL hkl,uint8_t scan) {
    if(scan<0x02||scan>0x0B) return false;
    const char a=kbChar(hkl,scan,false),b=kbChar(hkl,scan,true);
    return !(a>='0'&&a<='9')&&(b>='0'&&b<='9');
}
struct KbLabels {
    HKL hkl=nullptr;char label[2][KbMaxKeys];bool autoShift[KbMaxKeys];   // label[0]: SHIFT not armed, label[1]: SHIFT armed
    void build(HKL layout) {
        hkl=layout;memset(label,0,sizeof(label));memset(autoShift,0,sizeof(autoShift));
        KbKey keys[KbMaxKeys];const int n=kbLayout(keys);
        for(int i=0;i<n;++i) if(keys[i].kind==KbNormal) {
            const char a=kbChar(hkl,keys[i].scan,false),b=kbChar(hkl,keys[i].scan,true);
            autoShift[i]=kbDigitAuto(hkl,keys[i].scan);
            label[0][i]=autoShift[i]?b:a;label[1][i]=autoShift[i]?a:b;
            if(!label[1][i]) label[1][i]=label[0][i];
        }
    }
};
}
