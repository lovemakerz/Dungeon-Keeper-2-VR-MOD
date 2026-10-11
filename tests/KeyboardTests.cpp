// made by LoVeMaKeRz
// 0.9.47.34: geometry of the VR keyboard (Keyboard.hpp): no overlapping keys, every key reachable by the laser, the panel opens in front of the face
// and looks at it, scan codes are those of a real keyboard, a ray from behind or beside the panel never hits.
#include "../src/common/Keyboard.hpp"
#include <cassert>
#include <cstdio>
#include <set>
using namespace dk2vr;
static float dot(Vec3 a, Vec3 b) {return a.x * b.x + a.y * b.y + a.z * b.z;}
int main() {
    KbKey k[KbMaxKeys]; const int n = kbLayout(k);
    assert(n > 40 && n <= KbMaxKeys);
    // 1. keys do not overlap and stay inside the key area; scan codes unique among real keys
    std::set<int> scans;
    for (int i = 0; i < n; ++i) {
        assert(k[i].x0 >= 0 && k[i].y0 >= 0 && k[i].x1 <= KbWidthUnits + 1e-4f && k[i].y1 <= KbHeightUnits + 1e-4f && k[i].x1 > k[i].x0 && k[i].y1 > k[i].y0);
        if (k[i].kind != KbClose) assert(scans.insert(k[i].scan).second);
        for (int j = i + 1; j < n; ++j) assert(k[i].x1 <= k[j].x0 + 1e-5f || k[j].x1 <= k[i].x0 + 1e-5f || k[i].y1 <= k[j].y0 + 1e-5f || k[j].y1 <= k[i].y0 + 1e-5f);
    }
    // 2. the keys every save dialog needs exist with their hardware scan codes
    auto find = [&](uint8_t kind, int sc) {for (int i = 0; i < n; ++i) if (k[i].kind == kind && (sc < 0 || k[i].scan == sc)) return i; return -1;};
    assert(find(KbEnter, 0x1C) >= 0 && find(KbBackspace, 0x0E) >= 0 && find(KbEscape, 0x01) >= 0 && find(KbSpace, 0x39) >= 0 && find(KbShift, 0x2A) >= 0 && find(KbClose, -1) >= 0);
    for (int sc = 0x02; sc <= 0x0B; ++sc) assert(find(KbNormal, sc) >= 0);                  // 10 digit-row keys
    for (int sc = 0x10; sc <= 0x19; ++sc) assert(find(KbNormal, sc) >= 0);                  // top letter row
    for (int sc = 0x1E; sc <= 0x27; ++sc) assert(find(KbNormal, sc) >= 0);                  // home row
    for (int sc = 0x2C; sc <= 0x35; ++sc) assert(find(KbNormal, sc) >= 0);                  // bottom row
    // 3. the panel opens 0.62 m in front of the head, 0.30 m lower, facing the face; any head yaw
    for (float yawDeg : {0.f, 37.f, 90.f, 180.f, -120.f}) {
        const Quat head = yawQuat(yawDeg * Pi / 180.f); const Vec3 hp{1.f, 1.6f, -2.f};
        const Pose p = kbOpenPose(hp, head);
        const Vec3 toFace = sub(hp, p.position); const float len = std::sqrt(dot(toFace, toFace));
        assert(len > .6f && len < .8f);
        const Vec3 normal = rotate(p.orientation, {0, 0, 1});
        assert(dot(normal, scale(toFace, 1.f / len)) > .97f);                             // the panel looks at the face
        const Vec3 fwd = rotate(head, {0, 0, -1}); assert(dot(sub(p.position, hp), fwd) > .5f);   // in front of the player
        assert(p.position.y < hp.y - .25f);                                                // below the eyes
        // 4. a ray from the eye through the centre of every key hits exactly that key
        for (int i = 0; i < n; ++i) {
            float x0, y0, x1, y1; kbKeyRect(k[i], x0, y0, x1, y1);
            const Vec3 target = toWorld(p, {(x0 + x1) * .5f, (y0 + y1) * .5f, 0});
            Vec3 d = sub(target, hp); d = scale(d, 1.f / std::sqrt(dot(d, d)));
            float t = 0; assert(kbHit(p, hp, d, t) == i); assert(t > .5f && t < .9f);
        }
        // 5. between keys = -2, off the panel = -1, from behind = -1
        float t = 0; { // the gap between the first two digit keys
            float a0, b0, a1, b1, c0, d0, c1, d1; kbKeyRect(k[1], a0, b0, a1, b1); kbKeyRect(k[2], c0, d0, c1, d1);
            const Vec3 gap = toWorld(p, {(a1 + c0) * .5f, (b0 + b1) * .5f, 0}); Vec3 d = sub(gap, hp); d = scale(d, 1.f / std::sqrt(dot(d, d)));
            assert(kbHit(p, hp, d, t) == -2); }
        { const Vec3 off = toWorld(p, {kbPanelW(), 0, 0}); Vec3 d = sub(off, hp); d = scale(d, 1.f / std::sqrt(dot(d, d))); assert(kbHit(p, hp, d, t) == -1); }
        { const Vec3 back = add(p.position, scale(normal, -1.f)); Vec3 d = sub(p.position, back); d = scale(d, 1.f / std::sqrt(dot(d, d))); assert(kbHit(p, back, d, t) == -1); }
    }
    // 6. text legibility: a key is at least 3.8 cm wide and the whole panel about 60 cm wide
    assert(kbPanelW() > .55f && kbPanelW() < .65f && KbPitch >= .038f);
    printf("PASS KeyboardTests: %d keys, no overlap, scan codes unique, every key hit from the eye at 5 head yaws, opens in front of the face.\n", n);
}
