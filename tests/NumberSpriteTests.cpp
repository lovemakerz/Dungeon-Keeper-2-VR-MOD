// made by LoVeMaKeRz
// 0.9.47.33: a number born as ONE sprite of two touching atlas cells ("20", geometry of report 113401, draw 296 of geom_004:
// flat quads, u along +x, v along +z, the 2 on the LEFT of the 0 although its u is larger). The old path fitted ONE UV->world map over
// both cells (u axis ~ 0, direction noise) and handed it to the shared badge plane: the number came out mirrored / edge-on depending on the spacing.
#include "../src/common/BadgePlane.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
using namespace dk2vr;
static float dot(Vec3 a, Vec3 b) {return a.x * b.x + a.y * b.y + a.z * b.z;}
struct V {Vec3 p; unsigned color; float u, v;};

// Builds the sprite exactly like Plugin.cpp: corners (index, u, v, world), screen layout around the centroid, fit axes.
static ReplaySprite make(float spacingWorld, float shiftU, bool useNoJoin) {
    ReplaySprite s; s.up = {0, 1, 0};
    const float cellW = 0.0264f, y = -0.381f, z = -0.612f;       // world size of a cell (0.109 per uv unit * 0.2422)
    const float px = 12.1f;                                       // game pixels per cell
    struct Q {float cu, cv, x;};
    // piece 0 = the "0" (right), piece 1 = the "2" (left)
    Q qs[2] = {{0.5039f, 0.7539f, 0.f}, {0.7539f, 0.7539f, -spacingWorld}};
    float mx = 0, my = 0; unsigned idx = 0; std::vector<SpriteSample> samples;
    for(int q = 0; q < 2; ++q) {
        for(int c = 0; c < 4; ++c) {
            const float du = (c & 1) ? 0.2422f : 0.f, dv = (c >> 1) ? 0.2422f : 0.f;
            SpriteCorner k; k.index = idx++; k.u = qs[q].cu + du + (q ? shiftU : 0.f); k.v = qs[q].cv + dv;
            k.original = {qs[q].x + (c & 1 ? cellW * .5f : -cellW * .5f), y, z + (c >> 1 ? cellW * .5f : -cellW * .5f)};
            // game screen: the 0 at x=485, the 2 at x=473 (left), cells of 12.1 px, y=397
            const float sx = (q ? 473.f : 485.f) + (c & 1 ? px * .5f : -px * .5f), sy = 397.f + (c >> 1 ? px * .5f : -px * .5f);
            k.sx = sx; k.sy = sy; mx += sx; my += sy;
            s.corners.push_back(k); samples.push_back({k.original, k.u, k.v});
        }
    }
    mx /= 8; my /= 8; for(auto& c : s.corners) {c.sx -= mx; c.sy -= my;}
    s.screenAxes = true; s.pixelWorld = cellW / px;
    UprightSprite fr; s.hasAxes = fr.fit(samples, {0, 1, 0}, {0, 3, 3}, {1, 0, 0});
    if(s.hasAxes) {s.originalCenter = fr.center; s.sourceRight = fr.sourceRight; s.sourceDown = fr.sourceDown;}
    s.cx = mx; s.cy = my; s.width = 24.2f; s.height = px; s.rhw = 0.301f;
    if(useNoJoin) s.noJoin = glyphCellRow(s.corners);
    return s;
}
// reading check: the viewer's right/up from the head position; the 2 must be left of the 0, glyphs upright and not mirrored
static bool reads(const ReplaySprite& s, Vec3 head, const std::vector<unsigned char>& bytes) {
    Vec3 pivot{0, 0, 0}; for(auto& c : s.corners) pivot = add(pivot, c.original); pivot = scale(pivot, 1.f / 8.f);
    Vec3 n = sub(head, pivot); n = scale(n, 1.f / std::sqrt(dot(n, n)));
    Vec3 right = cross({0, 1, 0}, n); right = scale(right, 1.f / std::sqrt(dot(right, right)));
    Vec3 up = cross(n, right);
    bool ok = true;
    for(unsigned i = 0; i < 8; ++i) {
        V v; memcpy(&v, bytes.data() + i * sizeof(V), sizeof(v));
        const SpriteCorner& c = s.corners[i];
        const float X = dot(sub(v.p, pivot), right), Y = dot(sub(v.p, pivot), up);
        // game screen: x to the right, y downwards: the viewer's X must grow with sx, the viewer's Y must shrink with sy
        for(unsigned j = 0; j < 8; ++j) {
            if(i == j) continue;
            V w; memcpy(&w, bytes.data() + j * sizeof(V), sizeof(w));
            const SpriteCorner& d = s.corners[j];
            const float X2 = dot(sub(w.p, pivot), right), Y2 = dot(sub(w.p, pivot), up);
            if(c.sx > d.sx + 1.f && !(X > X2 + 1e-5f)) ok = false;
            if(c.sy > d.sy + 1.f && !(Y < Y2 - 1e-5f)) ok = false;
        }
    }
    return ok;
}
static std::vector<unsigned char> bytesOf(const ReplaySprite& s) {
    std::vector<unsigned char> b(8 * sizeof(V));
    for(unsigned i = 0; i < 8; ++i) {V v{s.corners[i].original, 0xffffffffu, s.corners[i].u, s.corners[i].v}; memcpy(b.data() + i * sizeof(V), &v, sizeof(v));}
    return b;
}
int main() {
    const Vec3 heads[] = {{0, 2, 3}, {0, 2, -3}, {3, 1.5f, 0}, {-3, 1.5f, 0}, {2, 3, 2}, {-2, 3, -2}, {0.2f, 4, 0.1f}, {2.5f, 0.6f, -2.5f}};
    // 1. the class is recognised, a single glyph or a ring is not
    {ReplaySprite s = make(0.0264f, 0.f, true); assert(glyphCellRow(s.corners)); assert(s.noJoin);}
    {ReplaySprite s = make(0.0264f, 0.f, true); std::vector<SpriteCorner> four(s.corners.begin(), s.corners.begin() + 4); assert(!glyphCellRow(four));
     std::vector<SpriteCorner> ring; for(unsigned i = 0; i < 8; ++i) {SpriteCorner k; k.index = i; k.u = 0.f; k.v = 0.f; k.original = {0, 0, 0}; ring.push_back(k);} assert(!glyphCellRow(ring));
     std::vector<SpriteCorner> big; for(unsigned i = 0; i < 8; ++i) {SpriteCorner k; k.index = i; k.u = (i & 1) ? 1.f : 0.f; k.v = (i & 2) ? 1.f : 0.f; k.original = {0, 0, 0}; big.push_back(k);} assert(!glyphCellRow(big));}
    // 2. the NEW path reads right from every head position and for every spacing / uv shift
    for(float spacing : {0.0264f, 0.0200f, 0.0400f, 0.0100f}) for(float shift : {0.f, 0.003f}) {
        ReplaySprite s = make(spacing, shift, true); assert(s.noJoin);
        std::vector<ReplaySprite*> pieces{&s}; assert(joinBadgePlanes(pieces) == 0); assert(!s.sharedPlane);
        for(Vec3 h : heads) {auto b = bytesOf(s); assert(s.rebuild(b, 0, sizeof(V), 8, h)); assert(reads(s, h, b));}
    }
    // 3. the OLD path (shared plane from the two-cell fit) is wrong for at least one head position / spacing: this is the bug of report 113401
    int bad = 0, total = 0;
    for(float spacing : {0.0264f, 0.0200f, 0.0400f, 0.0100f}) for(float shift : {0.f, 0.003f}) {
        ReplaySprite s = make(spacing, shift, false);
        if(!s.hasAxes) continue;
        std::vector<ReplaySprite*> pieces{&s}; joinBadgePlanes(pieces);
        for(Vec3 h : heads) {auto b = bytesOf(s); if(!s.rebuild(b, 0, sizeof(V), 8, h)) {++bad; ++total; continue;} ++total; if(!reads(s, h, b)) ++bad;}
    }
    printf("old shared-plane path: %d of %d (spacing, head) cases do NOT read right\n", bad, total);
    assert(bad > 0);
    puts("PASS NumberSpriteTests: two-cell number sprite recognised, no shared plane, reads right from 8 head positions x 8 spacings; the old path fails on the same cases.");
}
