#pragma once
// made by LoVeMaKeRz
// 0.9.47.34: VR virtual keyboard (save-name field, any text field of the game). Pure geometry shared by the OpenXR host (laser hit test, key
// events) and the game DLL (drawing, key injection). Keys are PHYSICAL keys (hardware scan codes of a standard 102-key keyboard): the game
// DLL labels them with the characters of the player's Windows layout (AZERTY / QWERTY / ...) and injects the same scan codes, so the game
// receives exactly what a real keyboard would send.
#include "Math.hpp"

namespace dk2vr {
constexpr uint32_t KeyboardOpen = 1u << 0;    // Request::keyboardFlags: the panel is shown
constexpr uint32_t KeyboardShift = 1u << 1;   // one-shot SHIFT is armed (consumed by the next key)
enum KbKind : uint8_t {KbNormal = 0, KbShift, KbBackspace, KbEnter, KbEscape, KbSpace, KbClose};
struct KbKey {uint8_t scan; uint8_t kind; float x0, y0, x1, y1;};   // key units (pitch = 1), y grows downwards from the top of the key area
constexpr int KbMaxKeys = 64;
constexpr float KbPitch = .040f;           // metres per key unit
constexpr float KbWidthUnits = 14.5f, KbHeightUnits = 5.f, KbMargin = .014f;   // key area; margin around it in metres
constexpr float KbGap = .06f;              // gap between neighbouring keys, in key units
constexpr float KbMinHoldMs = 70.f;        // a tap lasts at least this long (the game polls its keyboard once per frame)
constexpr float KbRepeatDelayMs = 420.f, KbRepeatEveryMs = 70.f;
inline float kbPanelW() {return KbWidthUnits * KbPitch + 2 * KbMargin;}
inline float kbPanelH() {return KbHeightUnits * KbPitch + 2 * KbMargin;}
// Keys of the panel. Rows (top to bottom): ESC, digits, BACKSPACE / letters, ENTER / letters / SHIFT, punctuation / SPACE, CLOSE.
inline int kbLayout(KbKey out[KbMaxKeys]) {
    int n = 0;
    auto add1 = [&](uint8_t sc, uint8_t kind, float x0, float y0, float x1, float y1) {out[n++] = {sc, kind, x0, y0, x1, y1};};
    add1(0x01, KbEscape, 0, 0, 1.5f, 1);
    for (int i = 0; i < 10; ++i) add1(uint8_t(0x02 + i), KbNormal, 1.5f + float(i), 0, 2.5f + float(i), 1);   // 1 .. 0 (physical digit row)
    add1(0x0C, KbNormal, 11.5f, 0, 12.5f, 1);
    add1(0x0E, KbBackspace, 12.5f, 0, 14.5f, 1);
    for (int i = 0; i < 10; ++i) add1(uint8_t(0x10 + i), KbNormal, 1.5f + float(i), 1, 2.5f + float(i), 2);   // top letter row (Q W E R T Y U I O P / A Z E R T Y U I O P)
    for (int i = 0; i < 10; ++i) add1(uint8_t(0x1E + i), KbNormal, 1.75f + float(i), 2, 2.75f + float(i), 3);   // home row (A S D F ... / Q S D F ... M)
    add1(0x1C, KbEnter, 12.f, 1, 14.5f, 3);
    add1(0x2A, KbShift, 0, 3, 2.25f, 4);
    for (int i = 0; i < 10; ++i) add1(uint8_t(0x2C + i), KbNormal, 2.25f + float(i), 3, 3.25f + float(i), 4);   // bottom row (Z X C ... / W X C ...) + , . /
    add1(0x39, KbSpace, 3.f, 4, 11.f, 5);
    add1(0, KbClose, 11.5f, 4, 14.5f, 5);
    return n;
}
// Panel pose when it is opened: in front of the head, below the eyes, tilted back toward the face. +Z of the pose is the side the player sees.
inline Pose kbOpenPose(Vec3 head, Quat headOrientation) {
    const Quat yaw = yawOnly(headOrientation);
    Pose p{};
    p.orientation = normalized(multiply(yaw, pitch(-.50f)));
    p.position = add(head, add(rotate(yaw, {0, 0, -.62f}), Vec3{0, -.30f, 0}));
    return p;
}
// Key rectangle in panel metres (origin = centre of the panel, +y up).
inline void kbKeyRect(const KbKey& k, float& x0, float& y0, float& x1, float& y1) {
    const float ox = -KbWidthUnits * KbPitch * .5f, oy = KbHeightUnits * KbPitch * .5f;
    x0 = ox + (k.x0 + KbGap * .5f) * KbPitch; x1 = ox + (k.x1 - KbGap * .5f) * KbPitch;
    y0 = oy - (k.y1 - KbGap * .5f) * KbPitch; y1 = oy - (k.y0 + KbGap * .5f) * KbPitch;
}
// Ray (LOCAL space) against the panel. Returns the key index, -2 on the panel between keys, -1 outside. distance = hit distance.
inline int kbHit(const Pose& panel, Vec3 origin, Vec3 dir, float& distance) {
    const Vec3 o = toLocal(panel, origin), d = rotate(conjugate(panel.orientation), dir);
    if (!(o.z > 0) || !(d.z < -1e-4f)) return -1;
    const float t = -o.z / d.z;
    if (!(t > .03f && t < 5.f)) return -1;
    const float x = o.x + t * d.x, y = o.y + t * d.y;
    if (std::fabs(x) > kbPanelW() * .5f || std::fabs(y) > kbPanelH() * .5f) return -1;
    distance = t;
    KbKey keys[KbMaxKeys]; const int n = kbLayout(keys);
    for (int i = 0; i < n; ++i) {
        float x0, y0, x1, y1; kbKeyRect(keys[i], x0, y0, x1, y1);
        if (x >= x0 && x <= x1 && y >= y0 && y <= y1) return i;
    }
    return -2;
}
}
