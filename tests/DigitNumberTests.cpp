// DK2VR 0.9.47.21 - made by LoVeMaKeRz
#include "../src/common/DigitNumber.hpp"
#include <algorithm>
#include <cstdio>
#include <random>
using namespace dk2vr;
#define CHECK(c, ...) do {if(!(c)) {std::printf("FAIL: " __VA_ARGS__); std::printf("\n"); return 1;}} while(0)
static const unsigned STRIDE = 28;
struct Spec {float sx, sy; Vec3 centre, right;};
// One piece = 4 vertices around its centre (offsets +-0.01), laid in an arena draw of its own.
static void build(const std::vector<Spec>& specs, std::vector<unsigned char>& arena, std::vector<std::vector<digitnum::Piece>>& draws, std::vector<size_t>& bases) {
    for(const auto& s : specs) {
        const size_t base = arena.size(); arena.resize(base + 4 * STRIDE, 0); bases.push_back(base);
        digitnum::Piece p; p.sx = s.sx; p.sy = s.sy; p.right = s.right; p.centre = s.centre;
        const float dx[4] = {-.01f, .01f, -.01f, .01f}, dy[4] = {.01f, .01f, -.01f, -.01f};
        for(unsigned i = 0; i < 4; ++i) {const float v[3] = {s.centre.x + dx[i], s.centre.y + dy[i], s.centre.z}; std::memcpy(&arena[base + i * STRIDE], v, 12); p.ids.push_back(i);}
        draws.push_back({p});
    }
}
static Vec3 centreOf(const std::vector<unsigned char>& a, size_t base) {
    Vec3 c{0, 0, 0}; for(unsigned i = 0; i < 4; ++i) {float v[3]; std::memcpy(v, &a[base + i * STRIDE], 12); c.x += v[0]; c.y += v[1]; c.z += v[2];}
    return {c.x / 4, c.y / 4, c.z / 4};
}
// Independent oracle: the 0.9.47.3 rule applied once to every piece (grouping with the same chained 26 px rule, reflection about the group centre).
static std::vector<Vec3> oracle(const std::vector<Spec>& s, float radius = 26.f) {
    const size_t n = s.size(); std::vector<int> g(n, -1); int ng = 0;
    for(size_t i = 0; i < n; ++i) if(g[i] < 0) {g[i] = ng; std::vector<size_t> st{i}; while(!st.empty()) {size_t c = st.back(); st.pop_back(); for(size_t j = 0; j < n; ++j) if(g[j] < 0 && std::hypot(s[c].sx - s[j].sx, s[c].sy - s[j].sy) <= radius) {g[j] = ng; st.push_back(j);}} ++ng;}
    std::vector<Vec3> out(n); for(size_t i = 0; i < n; ++i) out[i] = s[i].centre;
    for(int k = 0; k < ng; ++k) {
        Vec3 C{0, 0, 0}, R{0, 0, 0}; int cnt = 0; for(size_t i = 0; i < n; ++i) if(g[i] == k) {C = add(C, s[i].centre); R = add(R, s[i].right); ++cnt;}
        if(cnt < 2) continue; C = scale(C, 1.f / cnt); float rl = std::sqrt(R.x * R.x + R.y * R.y + R.z * R.z); if(!(rl > 1e-6f)) continue; R = scale(R, 1.f / rl);
        for(size_t i = 0; i < n; ++i) if(g[i] == k) {Vec3 d = sub(s[i].centre, C); float along = d.x * R.x + d.y * R.y + d.z * R.z; out[i] = add(s[i].centre, scale(R, -2.f * along));}
    }
    return out;
}
static bool close(Vec3 a, Vec3 b, float e = 2e-5f) {return std::fabs(a.x - b.x) < e && std::fabs(a.y - b.y) < e && std::fabs(a.z - b.z) < e;}
static int run(const std::vector<Spec>& specs, const std::vector<size_t>& order, const char* name) {
    std::vector<unsigned char> arena; std::vector<std::vector<digitnum::Piece>> draws; std::vector<size_t> bases;
    build(specs, arena, draws, bases);
    digitnum::Frame f;
    for(size_t k : order) f.addDraw(arena, bases[k], STRIDE, draws[k]);
    const auto want = oracle(specs);
    for(size_t i = 0; i < specs.size(); ++i) CHECK(close(centreOf(arena, bases[i]), want[i]), "%s: piece %zu at %.5f,%.5f,%.5f expected %.5f,%.5f,%.5f", name, i,
        centreOf(arena, bases[i]).x, centreOf(arena, bases[i]).y, centreOf(arena, bases[i]).z, want[i].x, want[i].y, want[i].z);
    return 0;
}
int main() {
    const Vec3 R{.9f, 0, .3f};
    // 1. The report: '2' and '0' of one number in two DIFFERENT draws (game screen: '2' at x=600, '0' at x=612; world: screen right = -x).
    {
        std::vector<Spec> s = {{600, 110, {.250f, -.43f, -.90f}, R}, {612, 110, {.216f, -.43f, -.91f}, R}};
        std::vector<unsigned char> arena; std::vector<std::vector<digitnum::Piece>> draws; std::vector<size_t> bases; build(s, arena, draws, bases);
        const float before2 = centreOf(arena, bases[0]).x * R.x + centreOf(arena, bases[0]).z * R.z, before0 = centreOf(arena, bases[1]).x * R.x + centreOf(arena, bases[1]).z * R.z;
        CHECK(before2 > before0, "setup: '2' starts on the viewer's right of '0'");
        digitnum::Frame f; f.addDraw(arena, bases[0], STRIDE, draws[0]);
        CHECK(close(centreOf(arena, bases[0]), s[0].centre), "a lone piece is not moved");
        f.addDraw(arena, bases[1], STRIDE, draws[1]);
        const float after2 = centreOf(arena, bases[0]).x * R.x + centreOf(arena, bases[0]).z * R.z, after0 = centreOf(arena, bases[1]).x * R.x + centreOf(arena, bases[1]).z * R.z;
        CHECK(after2 < after0, "after the second draw the order is reflected ('2' on the left of '0')");
        const Vec3 c0 = centreOf(arena, bases[0]), c1 = centreOf(arena, bases[1]);
        const Vec3 mid{(c0.x + c1.x) / 2, (c0.y + c1.y) / 2, (c0.z + c1.z) / 2}, mid0{(s[0].centre.x + s[1].centre.x) / 2, (s[0].centre.y + s[1].centre.y) / 2, (s[0].centre.z + s[1].centre.z) / 2};
        CHECK(close(mid, mid0), "the number keeps its centre");
        CHECK(f.pieces.size() == 2, "two pieces kept");
    }
    // 2. Any arrival order gives the oracle's result (3-digit number + a far number + a lone digit).
    {
        std::vector<Spec> s = {{600, 110, {.250f, -.43f, -.90f}, R}, {612, 110, {.216f, -.43f, -.91f}, R}, {624, 111, {.182f, -.43f, -.92f}, {.88f, 0, .32f}},
                               {300, 400, {-.4f, -.43f, -.5f}, {.7f, 0, .7f}}, {311, 402, {-.43f, -.43f, -.52f}, {.7f, 0, .68f}}, {800, 50, {.6f, -.4f, -1.f}, R}};
        std::vector<size_t> ord = {0, 1, 2, 3, 4, 5}; std::mt19937 rng(7); int perms = 0;
        do {if(run(s, ord, "permutation")) return 1; ++perms;} while(std::next_permutation(ord.begin(), ord.end()));
        CHECK(perms == 720, "all 720 arrival orders tried");
        std::printf("  (%d arrival orders identical to the one-shot rule)\n", perms);
    }
    // 3. One draw holding the whole number = the old (0.9.47.3 - 0.9.47.20) result.
    {
        std::vector<Spec> s = {{600, 110, {.250f, -.43f, -.90f}, R}, {612, 110, {.216f, -.43f, -.91f}, R}};
        std::vector<unsigned char> arena; std::vector<std::vector<digitnum::Piece>> draws; std::vector<size_t> bases; build(s, arena, draws, bases);
        digitnum::Frame f; std::vector<digitnum::Piece> both = {draws[0][0], draws[1][0]}; both[1].ids = {4, 5, 6, 7};   // second piece lives in the same draw, vertices 4..7
        arena.assign(8 * STRIDE, 0);
        for(unsigned k = 0; k < 2; ++k) for(unsigned i = 0; i < 4; ++i) {const float dx[4] = {-.01f, .01f, -.01f, .01f}, dy[4] = {.01f, .01f, -.01f, -.01f};
            const float v[3] = {s[k].centre.x + dx[i], s[k].centre.y + dy[i], s[k].centre.z}; std::memcpy(&arena[(k * 4 + i) * STRIDE], v, 12);}
        both[0].ids = {0, 1, 2, 3};
        f.addDraw(arena, 0, STRIDE, both);
        const auto want = oracle(s);
        float c0[3]; Vec3 a{0, 0, 0}, b{0, 0, 0};
        for(unsigned i = 0; i < 4; ++i) {std::memcpy(c0, &arena[i * STRIDE], 12); a = add(a, {c0[0], c0[1], c0[2]}); std::memcpy(c0, &arena[(4 + i) * STRIDE], 12); b = add(b, {c0[0], c0[1], c0[2]});}
        CHECK(close(scale(a, .25f), want[0]) && close(scale(b, .25f), want[1]), "one draw with both digits equals the one-shot rule");
    }
    // 4. Idempotence and bounds: regrouping again changes nothing; a piece too far away is never touched; vertices out of the arena are ignored.
    {
        std::vector<Spec> s = {{600, 110, {.250f, -.43f, -.90f}, R}, {640, 110, {.216f, -.43f, -.91f}, R}};   // 40 px apart: two numbers
        std::vector<unsigned char> arena; std::vector<std::vector<digitnum::Piece>> draws; std::vector<size_t> bases; build(s, arena, draws, bases);
        digitnum::Frame f; f.addDraw(arena, bases[0], STRIDE, draws[0]); f.addDraw(arena, bases[1], STRIDE, draws[1]);
        CHECK(close(centreOf(arena, bases[0]), s[0].centre) && close(centreOf(arena, bases[1]), s[1].centre), "pieces 40 px apart are separate numbers and stay put");
        const auto snap = arena; f.regroup(arena); f.regroup(arena); CHECK(snap == arena, "regroup is idempotent");
        digitnum::Piece bad; bad.sx = 600; bad.sy = 112; bad.right = R; bad.centre = {.2f, -.4f, -.9f}; bad.ids = {1000000}; std::vector<digitnum::Piece> v{bad};
        f.addDraw(arena, 0, STRIDE, v); CHECK(true, "out-of-arena vertex ignored");
        digitnum::Frame g; g.maxPieces = 3; std::vector<unsigned char> a2; std::vector<std::vector<digitnum::Piece>> d2; std::vector<size_t> b2;
        build({{1, 1, {0, 0, 0}, R}, {2, 2, {.1f, 0, 0}, R}, {3, 3, {.2f, 0, 0}, R}, {4, 4, {.3f, 0, 0}, R}, {5, 5, {.4f, 0, 0}, R}}, a2, d2, b2);
        for(size_t k = 0; k < d2.size(); ++k) g.addDraw(a2, b2[k], STRIDE, d2[k]);
        CHECK(g.pieces.size() == 3, "piece count is bounded (%zu)", g.pieces.size());
        g.clear(); CHECK(g.pieces.empty(), "clear");
    }
    // 5. 0.9.47.23 - digitShape: the glyph cell size is the same whatever the quad does on the game screen (turned, mirrored, tilted).
    {
        const float u[4] = {.5f, .75f, .5f, .75f}, v[4] = {0.f, 0.f, .25f, .25f};
        auto test = [&](float ang, bool mirror, const char* name) {
            float sx[4], sy[4];
            for(int i = 0; i < 4; ++i) {
                float x = ((u[i] - .5f) / .25f) * 12.f * (mirror ? -1.f : 1.f), y = (v[i] / .25f) * 18.f;
                sx[i] = 500.f + x * std::cos(ang) - y * std::sin(ang); sy[i] = 300.f + x * std::sin(ang) + y * std::cos(ang);
            }
            float W = 0, H = 0; CHECK(digitnum::digitShape(sx, sy, u, v, 4, W, H), "%s: shape found", name);
            CHECK(std::fabs(W - 12.f) < .01f && std::fabs(H - 18.f) < .01f, "%s: W=%.3f H=%.3f", name, W, H);
            return 0;
        };
        if(test(0.f, false, "upright")) return 1; if(test(3.14159265f, false, "180")) return 1; if(test(0.f, true, "mirror")) return 1;
        if(test(.7f, false, "tilt .7")) return 1; if(test(1.5707963f, true, "90 + mirror")) return 1;
        const float d[4] = {0, 0, 0, 0}; float W, H; CHECK(!digitnum::digitShape(d, d, u, v, 4, W, H), "degenerate quad refused");
    }
    // 6. 0.9.47.23 - screen copy: a number is the picture of the game screen on a plane facing the headset, whichever side the headset is on.
    {
        const Vec3 up{0, 1, 0};
        const Vec3 eyes[5] = {{0, .3f, 1.f}, {0, .3f, -1.f}, {1.f, .3f, 0}, {-1.f, .3f, 0}, {.7f, .5f, .7f}};
        for(int e = 0; e < 5; ++e) {
            // Two glyphs of 12x18 px on the game screen: '2' at x=600, '0' at x=614. Their original world positions are screen-parallel quads (whatever).
            std::vector<unsigned char> arena(8 * STRIDE, 0);
            std::vector<digitnum::Piece> d0(1), d1(1);
            const Vec3 C0{.25f, -.43f, -.90f}, C1{.22f, -.43f, -.91f};
            const float cx[2] = {600.f, 614.f};
            for(int g = 0; g < 2; ++g) {
                digitnum::Piece& p = g ? d1[0] : d0[0];
                p.sx = cx[g]; p.sy = 110; p.pw = .001f; p.up = up; p.eye = eyes[e]; p.right = {1, 0, 0}; p.centre = g ? C1 : C0; p.lift = false; p.pull = 0;
                const float ox[4] = {-6, 6, -6, 6}, oy[4] = {-9, -9, 9, 9};   // vertex order: TL, TR, BL, BR (screen y grows downward)
                for(unsigned i = 0; i < 4; ++i) {p.ids.push_back(i); p.vsx.push_back(cx[g] + ox[i]); p.vsy.push_back(110 + oy[i]);}
            }
            digitnum::Frame f; f.screenCopy = true;
            f.addDraw(arena, 0, STRIDE, d0); f.addDraw(arena, 4 * STRIDE, STRIDE, d1);
            auto at = [&](size_t base, unsigned i) {float v[3]; std::memcpy(v, &arena[base + i * STRIDE], 12); return Vec3{v[0], v[1], v[2]};};
            Vec3 C = scale(add(C0, C1), .5f); Vec3 toward = sub(eyes[e], C); toward = sub(toward, scale(up, toward.y));
            Vec3 R = cross(up, toward); const float rl = std::sqrt(R.x * R.x + R.y * R.y + R.z * R.z); R = scale(R, 1.f / rl);
            auto along = [&](Vec3 p) {return (p.x - C.x) * R.x + (p.y - C.y) * R.y + (p.z - C.z) * R.z;};
            // screen layout kept: TL left of TR, TL above BL, '2' left of '0'
            CHECK(along(at(0, 0)) < along(at(0, 1)), "eye %d: TL left of TR", e);
            CHECK(at(0, 0).y > at(0, 2).y, "eye %d: TL above BL", e);
            CHECK(along(at(0, 1)) < along(at(4 * STRIDE, 0)), "eye %d: '2' left of '0'", e);
            CHECK(std::fabs(along(at(4 * STRIDE, 1)) - along(at(0, 0)) - 26.f * .001f) < 1e-4f, "eye %d: spacing is the screen spacing (26 px)", e);
            // plane faces the headset: normal of the plane is along 'toward'
            Vec3 n = cross(sub(at(0, 1), at(0, 0)), sub(at(0, 2), at(0, 0))); const float nl = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
            Vec3 t = toward; const float tl = std::sqrt(t.x * t.x + t.y * t.y + t.z * t.z);
            CHECK(std::fabs((n.x * t.x + n.y * t.y + n.z * t.z) / (nl * tl)) > .999f, "eye %d: plane faces the headset", e);
            // reading from the headset: TL->TR points to the viewer's right, i.e. dot with cross(up, eye-C) > 0
            CHECK(along(at(0, 1)) - along(at(0, 0)) > 0, "eye %d: u axis to the viewer's right", e);
            const auto snap = arena; f.regroup(arena); CHECK(snap == arena, "eye %d: idempotent", e);
        }
        // floor-level number: lowest vertex at the original centre height; pull moves toward the head
        std::vector<unsigned char> arena(4 * STRIDE, 0); std::vector<digitnum::Piece> d(1);
        digitnum::Piece& p = d[0]; p.sx = 600; p.sy = 110; p.pw = .001f; p.up = up; p.eye = {0, .3f, 1.f}; p.right = {1, 0, 0}; p.centre = {0, -.5f, 0}; p.lift = true; p.pull = .01f;
        const float ox[4] = {-6, 6, -6, 6}, oy[4] = {-9, -9, 9, 9}; for(unsigned i = 0; i < 4; ++i) {p.ids.push_back(i); p.vsx.push_back(600 + ox[i]); p.vsy.push_back(110 + oy[i]);}
        digitnum::Frame f; f.screenCopy = true; f.addDraw(arena, 0, STRIDE, d);
        float lo = 1e9f, z = 0; for(unsigned i = 0; i < 4; ++i) {float v[3]; std::memcpy(v, &arena[i * STRIDE], 12); lo = std::min(lo, v[1]); z = v[2];}
        CHECK(std::fabs(lo - (-.5f)) < 1e-5f, "floor-level number stands on its original centre (%.5f)", lo);
        CHECK(z > .0099f, "pulled toward the headset (z %.5f)", z);
    }
    // 7. 0.9.47.25 - big digits: 11x16 px glyphs 48 px apart belong to ONE number (report 084037); with the fixed 26 px they were split and their order followed the game camera
    {
        const Vec3 up{0, 1, 0};
        for(int mode = 0; mode < 2; ++mode) for(int swap = 0; swap < 2; ++swap) {
            std::vector<unsigned char> arena(8 * STRIDE, 0);
            std::vector<digitnum::Piece> d0(1), d1(1);
            // the world order of the two digits is the opposite of the screen order when swap == 1 (game camera on the other side)
            const Vec3 W0{swap ? -.20f : .20f, -.5f, -1.f}, W1{swap ? .20f : -.20f, -.5f, -1.f};
            const float cx[2] = {800.f, 848.f};
            for(int g = 0; g < 2; ++g) {
                digitnum::Piece& p = g ? d1[0] : d0[0];
                p.sx = cx[g]; p.sy = 140; p.pw = .001f; p.ext = 16; p.up = up; p.eye = {0, 0, 0}; p.right = {1, 0, 0}; p.centre = g ? W1 : W0; p.lift = false; p.pull = 0;
                const float ox[4] = {-5.5f, 5.5f, -5.5f, 5.5f}, oy[4] = {-8, -8, 8, 8};
                for(unsigned i = 0; i < 4; ++i) {p.ids.push_back(i); p.vsx.push_back(cx[g] + ox[i]); p.vsy.push_back(140 + oy[i]);}
            }
            digitnum::Frame f; f.screenCopy = true; f.reach = mode ? 4.5f : 0.f;
            f.addDraw(arena, 0, STRIDE, d0); f.addDraw(arena, 4 * STRIDE, STRIDE, d1);
            float a[3], b[3]; std::memcpy(a, &arena[0], 12); std::memcpy(b, &arena[4 * STRIDE], 12);
            if(mode) CHECK(f.lastGroups == 1 && b[0] > a[0], "reach 4.5: one number, first screen digit left (swap %d)", swap);
            else CHECK(f.lastGroups == 2 && ((b[0] > a[0]) == (swap == 1)), "reach 0: split, order follows the world (swap %d)", swap);
        }
    }
    // 8. 0.9.47.26 - digits of one number that drifted far apart on the game screen (old digits, 5x8 px, random directions) stay a few digit sizes apart in the world: one number
    {
        const Vec3 up{0, 1, 0};
        for(int far = 0; far < 2; ++far) {
            std::vector<unsigned char> arena(8 * STRIDE, 0);
            std::vector<digitnum::Piece> d0(1), d1(1);
            const Vec3 W0{0.f, -.5f, -1.f}, W1{far ? 0.6f : 0.04f, -.5f, -1.f};   // 4 cm or 60 cm apart (digit height 1.3 cm)
            const float cx[2] = {200.f, 380.f}, cy[2] = {265.f, 80.f};            // 250 px apart on the game screen
            for(int g = 0; g < 2; ++g) {
                digitnum::Piece& p = g ? d1[0] : d0[0];
                p.sx = cx[g]; p.sy = cy[g]; p.pw = .001f; p.ext = 13; p.up = up; p.eye = {0, 0, 0}; p.right = {1, 0, 0}; p.centre = g ? W1 : W0; p.lift = false; p.pull = 0;
                const float ox[4] = {-4, 4, -4, 4}, oy[4] = {-6.5f, -6.5f, 6.5f, 6.5f};
                for(unsigned i = 0; i < 4; ++i) {p.ids.push_back(i); p.vsx.push_back(cx[g] + ox[i]); p.vsy.push_back(cy[g] + oy[i]);}
            }
            digitnum::Frame f; f.screenCopy = true; f.reach = 4.5f; f.worldReach = 6.f;
            f.addDraw(arena, 0, STRIDE, d0); f.addDraw(arena, 4 * STRIDE, STRIDE, d1);
            CHECK(f.lastGroups == (far ? 2 : 1), "world reach: %s pieces -> %d groups", far ? "distant" : "near", f.lastGroups);
        }
    }
    // 9. 0.9.47.28 - reading frame: whatever the turn / mirror of the quads on the game screen, the first digit of the text is on the left, glyphs upright and square
    {
        const Vec3 up{0, 1, 0};
        // screen = T * (reading offset), T = identity, 180 degrees, 90 degrees, mirror
        const float Tm[4][4] = {{1, 0, 0, 1}, {-1, 0, 0, -1}, {0, -1, 1, 0}, {-1, 0, 0, 1}};
        for(int t = 0; t < 4; ++t) {
            std::vector<unsigned char> arena(8 * STRIDE, 0);
            std::vector<digitnum::Piece> d0(1), d1(1);
            const float rx[2] = {-20.f, 20.f};   // reading-frame centres: first digit (u smaller) at -20, second at +20 px
            for(int g = 0; g < 2; ++g) {
                digitnum::Piece& p = g ? d1[0] : d0[0];
                const float cx = 300.f + Tm[t][0] * rx[g], cy = 200.f + Tm[t][2] * rx[g];
                p.sx = cx; p.sy = cy; p.pw = .001f; p.ext = 15; p.up = up; p.eye = {0, 0, 0}; p.right = {1, 0, 0}; p.centre = {.01f * g, -.5f, -1.f}; p.lift = false; p.pull = 0;
                const float eu[4] = {-1, 1, -1, 1}, ev[4] = {-1, -1, 1, 1};   // glyph cell 0.25 uv, 8 px per half-cell on the reading frame but 8 x 13 on screen: stretched
                for(unsigned i = 0; i < 4; ++i) {
                    const float ou = eu[i] * 7.5f, ovv = ev[i] * 7.5f;   // reading-frame pixel offsets
                    p.ids.push_back(i); p.vsx.push_back(cx + Tm[t][0] * ou + Tm[t][1] * ovv); p.vsy.push_back(cy + Tm[t][2] * ou + Tm[t][3] * ovv);
                    p.vu.push_back(.5f + g * .25f + eu[i] * .125f); p.vv.push_back(.25f + ev[i] * .125f);
                }
            }
            digitnum::Frame f; f.screenCopy = true; f.reading = true; f.reach = 4.5f;
            f.addDraw(arena, 0, STRIDE, d0); f.addDraw(arena, 4 * STRIDE, STRIDE, d1);
            CHECK(f.lastGroups == 1, "reading frame: one group (transform %d)", t);
            float v[8][3]; for(int i = 0; i < 8; ++i) std::memcpy(v[i], &arena[size_t(i) * STRIDE], 12);
            const float ax = (v[0][0] + v[1][0] + v[2][0] + v[3][0]) / 4, bx = (v[4][0] + v[5][0] + v[6][0] + v[7][0]) / 4;
            CHECK(ax < bx, "reading frame: first digit left of the second (transform %d)", t);
            for(int g = 0; g < 2; ++g) {
                const float* a = v[g * 4]; const float* b = v[g * 4 + 1]; const float* c = v[g * 4 + 2];
                CHECK(b[0] > a[0] && std::fabs(b[1] - a[1]) < 1e-6f, "glyph u runs to the right (transform %d)", t);
                CHECK(c[1] < a[1] && std::fabs(c[0] - a[0]) < 1e-6f, "glyph v runs downwards (transform %d)", t);
                CHECK(std::fabs((b[0] - a[0]) - (a[1] - c[1])) < 1e-6f, "glyph cell is square (transform %d)", t);
            }
        }
    }
    // 10. 0.9.47.31 - billboard at replay: from any side of the number, the text reads left to right for the viewer (never the mirror image)
    {
        digitnum::DigitGroup g; g.centre = {0.3f, -0.5f, -1.2f}; g.up = {0, 1, 0}; g.right = {1, 0, 0}; g.pull = 0.f;
        const Vec3 heads[6] = {{0, 0, 3}, {0, 0, -3}, {3, 0, 0}, {-3, 0.5f, 0}, {2, 1, 2}, {-2, -0.2f, -2.5f}};
        for(const Vec3& h : heads) {
            const digitnum::DigitView v = digitnum::digitView(g, h);
            CHECK(v.ok, "digit view exists");
            const Vec3 a = digitnum::digitPoint(g, v, -0.1f, 0.f), b = digitnum::digitPoint(g, v, 0.1f, 0.f), c = digitnum::digitPoint(g, v, 0.f, 0.1f);
            Vec3 fwd = sub(g.centre, h); fwd.y = 0; const float fl = std::sqrt(fwd.x * fwd.x + fwd.z * fwd.z); fwd = scale(fwd, 1.f / fl);
            const Vec3 viewerRight = cross(fwd, Vec3{0, 1, 0});   // right-handed: x right, y up, -z forward
            const Vec3 ab = sub(b, a);
            CHECK(ab.x * viewerRight.x + ab.y * viewerRight.y + ab.z * viewerRight.z > 0.19f, "text runs to the viewer's right from head %.1f,%.1f,%.1f", h.x, h.y, h.z);
            CHECK(c.y > g.centre.y + 0.09f, "glyph top is up");
        }
    }
    // 11. 0.9.47.32 - the order of the digits is the one they had at birth, whatever the drift of each digit afterwards (report 111947: '20' became '0 2')
    {
        const Vec3 up{0, 1, 0};
        digitnum::Frame f; f.reading = true; f.screenCopy = true; f.reach = 4.5f; f.worldReach = 6.f;
        float startX[2] = {300.f, 312.f}, endX[2] = {320.f, 292.f};   // '2' born on the left then drifts right, '0' born on the right then drifts left
        for(int frame = 0; frame <= 12; ++frame) {
            f.clear();
            std::vector<unsigned char> arena(8 * STRIDE, 0);
            std::vector<digitnum::Piece> d0(1), d1(1);
            for(int g = 0; g < 2; ++g) {
                digitnum::Piece& p = g ? d1[0] : d0[0];
                const float cx = startX[g] + (endX[g] - startX[g]) * float(frame) / 12.f, cy = 200.f;
                p.sx = cx; p.sy = cy; p.pw = .001f; p.ext = 9; p.up = up; p.eye = {0, 0, 0}; p.right = {1, 0, 0}; p.centre = {.005f * (cx - 300.f), -.5f, -1.f}; p.cu = .5f + g * .25f; p.cv = .25f;
                const float eu[4] = {-1, 1, -1, 1}, ev[4] = {-1, -1, 1, 1};
                for(unsigned i = 0; i < 4; ++i) {
                    p.ids.push_back(i); p.vsx.push_back(cx + eu[i] * 4.5f); p.vsy.push_back(cy + ev[i] * 6.5f);
                    p.vu.push_back(p.cu + .125f + eu[i] * .125f); p.vv.push_back(p.cv + .125f + ev[i] * .125f);
                }
            }
            f.addDraw(arena, 0, STRIDE, d0); f.addDraw(arena, 4 * STRIDE, STRIDE, d1);
            float v[8][3]; for(int i = 0; i < 8; ++i) std::memcpy(v[i], &arena[size_t(i) * STRIDE], 12);
            const float ax = (v[0][0] + v[1][0] + v[2][0] + v[3][0]) / 4, bx = (v[4][0] + v[5][0] + v[6][0] + v[7][0]) / 4;
            CHECK(f.lastGroups == 1, "drift: one group (frame %d)", frame);
            CHECK(ax < bx, "drift: the digit born on the left stays on the left (frame %d, positions %.1f / %.1f)", frame, startX[0] + (endX[0] - startX[0]) * frame / 12.f, startX[1] + (endX[1] - startX[1]) * frame / 12.f);
        }
    }
    std::printf("PASS: DigitNumberTests (two digits in two draws are reflected as one number; 720 arrival orders = one-shot rule; one-draw number = 0.9.47.3 result; separate numbers untouched; idempotent; bounded; screen copy readable from 5 headset positions; big digits grouped by size; drifted digits grouped by world distance; glyph size independent of rotation/mirror)\n");
    return 0;
}
