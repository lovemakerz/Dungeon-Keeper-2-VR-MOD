#pragma once
// DK2VR 0.9.47.28 - made by LoVeMaKeRz
// Damage numbers: reflection of a whole NUMBER about its own centre, across the draws of one game frame.
// 0.9.47.3 mirrored the pieces of one number about the number's centre (order AND glyph shapes) but only for the pieces found in ONE draw call. The game
// draws a particle at a time when it sorts them, so a two-digit number can arrive as two draws of 4 vertices: each digit was then alone in its draw, got its
// glyph mirrored but never the reflection of the order, and '20' read '02' in the headset (report 235536). The pieces are now kept for the whole frame:
// every time a draw adds pieces, all of them are grouped again (chained, screen centres within 'radius' pixels) and each group is reflected about its centre
// along the mean reading axis. The shift of a piece is always recomputed from its ORIGINAL position, so adding a piece later gives exactly the same result
// as if all pieces had come at once, and one draw holding the whole number gives the same result as in 0.9.47.3 to 0.9.47.20.
// The vertex bytes live in the frame's vertex arena (x, y, z floats at the start of each vertex, 'stride' bytes apart). Pure logic: tests/DigitNumberTests.cpp.
#include "Math.hpp"
#include <algorithm>
#include <string>
#include <cstdio>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>
namespace dk2vr {
namespace digitnum {

struct Piece {
    size_t base = 0;                // byte offset of the draw's first vertex in the arena
    unsigned stride = 0;            // bytes between two vertices of that draw
    std::vector<uint32_t> ids;      // vertices of the piece (indices inside the draw)
    float sx = 0, sy = 0;           // centre of the piece on the game screen (pixels)
    Vec3 right{}, centre{}, shift{};// reading axis, ORIGINAL world centre, shift applied so far
    // 0.9.47.23 (screen copy): the picture of the game screen is copied on a plane that faces the headset
    std::vector<float> vsx, vsy;    // game-screen position of every vertex of the piece (same order as 'ids')
    std::vector<float> vu, vv;      // 0.9.47.28: texture coordinates of the same vertices (reading frame)
    float pw = 0.f;                 // world units per game pixel
    float cu = 0.f, cv = 0.f;       // 0.9.47.26: glyph cell (smallest u, v) - diagnostics
    float ext = 0.f;                // 0.9.47.25: size of the piece on the game screen (largest side, pixels): the gap between two digits of one number grows with it
    float pull = 0.f;               // distance the number is moved toward the headset (keeps it out of walls)
    bool lift = false;              // the number lies at floor height: its lowest point is put on the original centre
    Vec3 up{}, eye{};               // world vertical, headset position when the piece was recorded
    // 0.9.47.31: result of the reading-frame layout, kept so that the VR thread can turn the number to the CURRENT headset position at every frame
    float key = 0.f; uint64_t cohort = 0;   // 0.9.47.32: reading order remembered since the birth of the digit (see Frame::track)
    std::vector<float> ox, oy;      // offset of every vertex from the group centre along the reading right / up axes (world units)
    Vec3 gC{}, gRight{}; float gPull = 0.f; bool gHave = false;
};

// 0.9.47.31: the number was turned toward the headset position known when the game drew it (report 110701: the same '20' read '20' from one side of the table
// and as its mirror image '0 2' from another: the facing was frozen at record time). These structures travel with the frame to the VR thread, which rebuilds
// the vertices of every digit from the current headset position (same maths as the layout: right = up x toward, glyph upright).
struct DigitVertex {uint32_t index = 0; float ox = 0, oy = 0; uint32_t group = 0;};
struct DigitGroup {Vec3 centre{}, up{}, right{}; float pull = 0.f;};
struct DigitView {Vec3 right{}, up{}, face{}; bool ok = false;};
inline DigitView digitView(const DigitGroup& g, Vec3 head) {
    DigitView v; auto dot3 = [](Vec3 a, Vec3 b) {return a.x * b.x + a.y * b.y + a.z * b.z;};
    auto unit3 = [&](Vec3& a) {const float q = dot3(a, a); if(!std::isfinite(q) || q < 1e-12f) return false; a = scale(a, 1.f / std::sqrt(q)); return true;};
    Vec3 up = g.up; if(!unit3(up)) return v;
    Vec3 toward = sub(head, g.centre); Vec3 face = toward; const bool haveFace = unit3(face);
    toward = sub(toward, scale(up, dot3(toward, up)));
    Vec3 right = cross(up, toward);
    if(!unit3(right)) {right = g.right; if(!unit3(right)) return v;}
    v.right = right; v.up = up; v.face = haveFace ? face : Vec3{0, 0, 0}; v.ok = true; return v;
}
inline Vec3 digitPoint(const DigitGroup& g, const DigitView& v, float ox, float oy) {
    Vec3 q = add(g.centre, add(scale(v.right, ox), scale(v.up, oy)));
    if(g.pull > 0.f) q = add(q, scale(v.face, g.pull));
    return q;
}

// 0.9.47.23: size of the glyph cell on the game screen, independent of any rotation or mirror of the quad. Least-squares
// gradient of the screen position against (u, v): W = length of the u axis * cell width, H = same for v.
inline bool digitShape(const float* sx, const float* sy, const float* u, const float* v, size_t n, float& W, float& H) {
    if(n < 3) return false;
    double mu = 0, mv = 0, mx = 0, my = 0; float u0 = 1e9f, u1 = -1e9f, v0 = 1e9f, v1 = -1e9f;
    for(size_t i = 0; i < n; ++i) {
        mu += u[i]; mv += v[i]; mx += sx[i]; my += sy[i];
        u0 = std::min(u0, u[i]); u1 = std::max(u1, u[i]); v0 = std::min(v0, v[i]); v1 = std::max(v1, v[i]);
    }
    mu /= n; mv /= n; mx /= n; my /= n;
    double suu = 0, suv = 0, svv = 0, cux = 0, cvx = 0, cuy = 0, cvy = 0;
    for(size_t i = 0; i < n; ++i) {
        const double du = u[i] - mu, dv = v[i] - mv, dx = sx[i] - mx, dy = sy[i] - my;
        suu += du * du; suv += du * dv; svv += dv * dv; cux += du * dx; cvx += dv * dx; cuy += du * dy; cvy += dv * dy;
    }
    const double det = suu * svv - suv * suv;
    if(!(det > 1e-12)) return false;
    const double ax = (cux * svv - cvx * suv) / det, bx = (cvx * suu - cux * suv) / det;
    const double ay = (cuy * svv - cvy * suv) / det, by = (cvy * suu - cuy * suv) / det;
    W = float(std::sqrt(ax * ax + ay * ay) * (u1 - u0));
    H = float(std::sqrt(bx * bx + by * by) * (v1 - v0));
    return std::isfinite(W) && std::isfinite(H) && W > 1e-4f && H > 1e-4f;
}

struct Frame {
    std::vector<Piece> pieces;
    float radius = 26.f;
    float worldReach = 0.f;         // 0.9.47.26: > 0: two pieces are ALSO of one number when their world centres are closer than worldReach x (the larger of pw*ext): the digits of one number drift apart on the game screen as they age (rap17 geometry: 5x8 px digits 14-16 px apart in random directions) but stay a few digit sizes apart in the world
    std::vector<int> lastGroup;     // diagnostics: group of every piece after the last regroup
    float reach = 0.f;              // 0.9.47.25: 0 = fixed 'radius'; > 0 = two pieces are of one number when closer than max(radius, reach * the larger of their sizes) (report 084037: 11x16 px digits are 48 px apart, so a fixed 26 px split the number and its digits followed the game camera's yaw: right in 1 launch out of 3)
    int lastGroups = 0;             // diagnostics
    bool screenCopy = false;        // 0.9.47.23: lay the numbers out as on the game screen instead of reflecting them
    // 0.9.47.28 (reading frame): the game draws the digits turned by 180 degrees, mirrored or by 90 degrees on its screen (report 101315: '20' seen as a mirrored '02'),
    // so a copy of its screen is unreadable. Here every glyph is rebuilt from ITS OWN texture coordinates (u to the right, v downwards), the digits of a group are put in
    // one row ordered along the u axis of the quads (= the order of the text), with square glyph cells (the aged digits are 8x13 px on the game screen: 'stretched').
    bool reading = false;
    std::string lastDiag; int lastDiagM = 0;   // 0.9.47.29: layout of the biggest group of the last regroup, for the log
    uint64_t readingOk = 0, readingFallback = 0;   // 0.9.47.29: diagnostics (laid out in the reading frame / fell back)
    float sizeScale = 1.f;           // size factor of the digits (DamageDigitsScale)
    float fixedWorld = 0.f;         // > 0: world size of one glyph cell, whatever the game does (DamageDigitsFixedMM / 1000)
    size_t maxPieces = 512;         // a frame never has that many digits: beyond it new pieces are left as they are (bounded cost)
    // 0.9.47.32: ORDER OF THE DIGITS. A number is born with its digits side by side in the right order, then every digit drifts on its own in a random direction
    // (report 111947: the same '20' read '20' just after its birth and '0 2' a moment later, when the 0 had drifted to the left of the 2). The order is therefore taken
    // ONCE, when a digit is first seen, and kept: a digit is followed from one game frame to the next (same glyph cell, nearest position), and keeps the reading
    // key it had at birth. Digits born in the same frame form a cohort (one number); inside a group the order is (cohort, birth key).
    struct Track {float cu = 0, cv = 0, sx = 0, sy = 0, key = 0; uint64_t born = 0, last = 0;};
    std::vector<Track> tracks; uint64_t frameNo = 1; bool useTracks = true;
    void clear() {
        pieces.clear(); ++frameNo;
        tracks.erase(std::remove_if(tracks.begin(), tracks.end(), [&](const Track& k) {return frameNo - k.last > 150;}), tracks.end());
        if(tracks.size() > 4096) tracks.clear();
    }
    void track(Piece& p) {
        float M[4], su, sv; float key = p.sx;
        if(pieceMatrix(p, M, su, sv)) {const float l = std::hypot(M[0], M[2]); if(l > 1e-6f) key = (p.sx * M[0] + p.sy * M[2]) / l;}
        int best = -1; float bestD = 1e9f; const float gate = std::max(14.f, 3.f * p.ext);
        for(size_t i = 0; i < tracks.size(); ++i) {
            const Track& k = tracks[i];
            if(k.last == frameNo || frameNo - k.last > 8) continue;
            if(std::fabs(k.cu - p.cu) > 0.02f || std::fabs(k.cv - p.cv) > 0.02f) continue;
            const float dd = std::hypot(k.sx - p.sx, k.sy - p.sy);
            if(dd <= gate && dd < bestD) {bestD = dd; best = int(i);}
        }
        if(best < 0) {Track k; k.cu = p.cu; k.cv = p.cv; k.sx = p.sx; k.sy = p.sy; k.key = key; k.born = frameNo; k.last = frameNo; tracks.push_back(k); best = int(tracks.size()) - 1;}
        Track& k = tracks[size_t(best)]; k.sx = p.sx; k.sy = p.sy; k.last = frameNo;
        p.key = k.key; p.cohort = k.born / 4;
    }

    // One draw's pieces were just written to the arena at 'base': register them and regroup the frame.
    void addDraw(std::vector<unsigned char>& arena, size_t base, unsigned stride, std::vector<Piece>& draw) {
        for(auto& p : draw) {
            if(pieces.size() >= maxPieces) break;
            p.base = base; p.stride = stride; p.shift = Vec3{0, 0, 0};
            if(useTracks) track(p); else {p.key = 0.f; p.cohort = 0;}
            pieces.push_back(std::move(p));
        }
        draw.clear();
        regroup(arena);
    }

    // 0.9.47.23: every group (one number) is the COPY of its game-screen picture on a vertical plane facing the headset: a vertex goes to
    // C + right*(x - X)*pw - up*(y - Y)*pw, C/X/Y = centres of the group in the world / on the screen. Nothing depends on which way the game
    // quads turn (mirror, 180 degrees, tilt): what the game shows on its screen is what the headset shows. Positions are absolute (idempotent).
    void layoutScreen(std::vector<unsigned char>& arena, const std::vector<int>& group, int groups) {
        const size_t n = pieces.size();
        for(int g = 0; g < groups; ++g) {
            Vec3 C{0, 0, 0}, E{0, 0, 0}, F{0, 0, 0}; float X = 0, Y = 0, pw = 0, pull = 0; int m = 0; bool lift = false; Vec3 up{0, 0, 0};
            for(size_t i = 0; i < n; ++i) if(group[i] == g) {
                const Piece& p = pieces[i];
                C = add(C, p.centre); E = add(E, p.eye); F = add(F, p.right); X += p.sx; Y += p.sy; pw += p.pw; pull = std::max(pull, p.pull); lift = lift || p.lift; up = p.up; ++m;
            }
            if(m == 0) continue;
            const float k = 1.f / float(m);
            C = scale(C, k); E = scale(E, k); X *= k; Y *= k; pw *= k;
            auto dot3 = [](Vec3 a, Vec3 b) {return a.x * b.x + a.y * b.y + a.z * b.z;};
            auto unit3 = [&](Vec3& a) {const float q = dot3(a, a); if(!std::isfinite(q) || q < 1e-12f) return false; a = scale(a, 1.f / std::sqrt(q)); return true;};
            if(!unit3(up) || !(pw > 0.f) || !std::isfinite(pw)) continue;
            Vec3 toward = sub(E, C); toward = sub(toward, scale(up, dot3(toward, up)));
            Vec3 right = cross(up, toward);
            if(!unit3(right)) {right = sub(F, scale(up, dot3(F, up))); if(!unit3(right)) continue;}
            Vec3 face = toward; const bool haveFace = unit3(face);
            float low = 0.f;
            if(lift) {
                low = 1e9f;
                for(size_t i = 0; i < n; ++i) if(group[i] == g) for(size_t j = 0; j < pieces[i].vsy.size(); ++j) low = std::min(low, -(pieces[i].vsy[j] - Y) * pw);
                if(!std::isfinite(low) || low > 1e8f) low = 0.f;
            }
            for(size_t i = 0; i < n; ++i) if(group[i] == g) {
                Piece& p = pieces[i];
                for(size_t j = 0; j < p.ids.size() && j < p.vsx.size() && j < p.vsy.size(); ++j) {
                    const size_t at = p.base + size_t(p.ids[j]) * p.stride;
                    if(at + 12 > arena.size()) continue;
                    const float dx = (p.vsx[j] - X) * pw, dy = -(p.vsy[j] - Y) * pw - (lift ? low : 0.f);
                    Vec3 q = add(C, add(scale(right, dx), scale(up, dy)));
                    if(haveFace && pull > 0.f) q = add(q, scale(face, pull));
                    float v[3] = {q.x, q.y, q.z}; std::memcpy(arena.data() + at, v, 12);
                }
            }
        }
    }

    // Least-squares 2x2 matrix M with (screen offset) = M * (uv offset) over the vertices of one piece. False when the quad is degenerate.
    static bool pieceMatrix(const Piece& p, float M[4], float& su, float& sv) {
        const size_t n = std::min(p.vsx.size(), std::min(p.vu.size(), std::min(p.vv.size(), p.vsy.size())));
        if(n < 3) return false;
        double mu = 0, mv = 0, mx = 0, my = 0; float u0 = 1e9f, u1 = -1e9f, v0 = 1e9f, v1 = -1e9f;
        for(size_t i = 0; i < n; ++i) {mu += p.vu[i]; mv += p.vv[i]; mx += p.vsx[i]; my += p.vsy[i]; u0 = std::min(u0, p.vu[i]); u1 = std::max(u1, p.vu[i]); v0 = std::min(v0, p.vv[i]); v1 = std::max(v1, p.vv[i]);}
        mu /= n; mv /= n; mx /= n; my /= n;
        double suu = 0, suv = 0, svv = 0, cux = 0, cvx = 0, cuy = 0, cvy = 0;
        for(size_t i = 0; i < n; ++i) {
            const double du = p.vu[i] - mu, dv = p.vv[i] - mv, dx = p.vsx[i] - mx, dy = p.vsy[i] - my;
            suu += du * du; suv += du * dv; svv += dv * dv; cux += du * dx; cvx += dv * dx; cuy += du * dy; cvy += dv * dy;
        }
        const double det = suu * svv - suv * suv;
        if(!(det > 1e-12)) return false;
        const double ax = (cux * svv - cvx * suv) / det, bx = (cvx * suu - cux * suv) / det;   // x = ax*du + bx*dv
        const double ay = (cuy * svv - cvy * suv) / det, by = (cvy * suu - cuy * suv) / det;   // y = ay*du + by*dv
        M[0] = float(ax); M[1] = float(bx); M[2] = float(ay); M[3] = float(by);
        su = float(u1 - u0); sv = float(v1 - v0);
        return std::isfinite(M[0]) && std::isfinite(M[1]) && std::isfinite(M[2]) && std::isfinite(M[3]) && std::fabs(M[0] * M[3] - M[1] * M[2]) > 1e-9f && su > 1e-6f && sv > 1e-6f;
    }
    // Reading-frame layout (see 'reading'). Positions are absolute (idempotent). Returns false when a group cannot be laid out that way (it is then left to layoutScreen).
    bool layoutReading(std::vector<unsigned char>& arena, const std::vector<int>& group, int groups) {
        const size_t n = pieces.size(); bool all = true; lastDiagM = 0; lastDiag.clear();
        for(int g = 0; g < groups; ++g) {
            Vec3 C{0, 0, 0}, E{0, 0, 0}, F{0, 0, 0}; float X = 0, Y = 0, pw = 0, pull = 0; int m = 0; bool lift = false; Vec3 up{0, 0, 0};
            std::vector<size_t> mem;
            for(size_t i = 0; i < n; ++i) if(group[i] == g) {
                const Piece& p = pieces[i];
                C = add(C, p.centre); E = add(E, p.eye); F = add(F, p.right); X += p.sx; Y += p.sy; pw += p.pw; pull = std::max(pull, p.pull); lift = lift || p.lift; up = p.up; ++m; mem.push_back(i);
            }
            for(size_t mi : mem) pieces[mi].gHave = false;
            if(m == 0) continue;
            const float k = 1.f / float(m);
            C = scale(C, k); E = scale(E, k); X *= k; Y *= k; pw *= k;
            auto dot3 = [](Vec3 a, Vec3 b) {return a.x * b.x + a.y * b.y + a.z * b.z;};
            auto unit3 = [&](Vec3& a) {const float q = dot3(a, a); if(!std::isfinite(q) || q < 1e-12f) return false; a = scale(a, 1.f / std::sqrt(q)); return true;};
            if(!unit3(up) || !(pw > 0.f) || !std::isfinite(pw)) {all = false; continue;}
            Vec3 toward = sub(E, C); toward = sub(toward, scale(up, dot3(toward, up)));
            Vec3 right = cross(up, toward);
            if(!unit3(right)) {right = sub(F, scale(up, dot3(F, up))); if(!unit3(right)) {all = false; continue;}}
            Vec3 face = toward; const bool haveFace = unit3(face);
            // matrices of the pieces; the reference one = the largest piece that has a usable matrix
            std::vector<std::array<float, 4>> Ms(m); std::vector<char> ok(m, 0); std::vector<float> su(m, 0.f), sv(m, 0.f);
            int ref = -1;
            for(int a = 0; a < m; ++a) {float M[4], u, v; if(pieceMatrix(pieces[mem[a]], M, u, v)) {ok[a] = 1; Ms[a] = {M[0], M[1], M[2], M[3]}; su[a] = u; sv[a] = v; if(ref < 0 || pieces[mem[a]].ext > pieces[mem[ref]].ext) ref = a;}}
            if(ref < 0) {all = false; continue;}
            const auto R0 = Ms[ref]; const float det0 = R0[0] * R0[3] - R0[1] * R0[2];
            const float i00 = R0[3] / det0, i01 = -R0[1] / det0, i10 = -R0[2] / det0, i11 = R0[0] / det0;   // inverse of the reference matrix
            // per piece: reading coordinate (u) and world size per uv unit
            std::vector<float> ru(m, 0.f), S(m, 0.f), wq(m, 0.f);
            for(int a = 0; a < m; ++a) {
                const Piece& p = pieces[mem[a]];
                const float dx = p.sx - X, dy = p.sy - Y;
                ru[a] = i00 * dx + i01 * dy;
                float cell;
                if(ok[a]) cell = std::sqrt(std::fabs(Ms[a][0] * Ms[a][3] - Ms[a][1] * Ms[a][2]));   // game pixels per uv unit (square cell)
                else cell = std::max(1.f, p.ext) / 0.25f;
                S[a] = fixedWorld > 0.f ? fixedWorld / 0.25f : cell * pw * sizeScale;
                wq[a] = S[a] * (ok[a] ? std::max(su[a], sv[a]) : 0.25f);
            }
            std::vector<int> order(m); for(int a = 0; a < m; ++a) order[a] = a;
            if(useTracks) std::stable_sort(order.begin(), order.end(), [&](int a, int b) {const Piece& pa = pieces[mem[a]]; const Piece& pb = pieces[mem[b]]; if(pa.cohort != pb.cohort) return pa.cohort < pb.cohort; return pa.key < pb.key;});
            else std::stable_sort(order.begin(), order.end(), [&](int a, int b) {return ru[a] < ru[b];});
            if(m >= lastDiagM) {
                lastDiagM = m; char b[400];
                const float ang = std::atan2(R0[2], R0[0]) * 57.29578f;
                snprintf(b, sizeof b, "group of %d: ref piece %d angle %.0f det %+.1f | toward %.2f %.2f %.2f right %.2f %.2f %.2f up %.2f %.2f %.2f | eye %.2f %.2f %.2f centre %.2f %.2f %.2f | order:", m, ref, ang, det0 > 0 ? 1.f : -1.f, toward.x, toward.y, toward.z, right.x, right.y, right.z, up.x, up.y, up.z, E.x, E.y, E.z, C.x, C.y, C.z);
                lastDiag = b;
                for(int q = 0; q < m; ++q) {const int a = order[q]; const Piece& pp = pieces[mem[a]]; char c[80]; snprintf(c, sizeof c, " [cell %.2f,%.2f ru %.1f px %.0f,%.0f]", pp.cu, pp.cv, ru[a], pp.sx, pp.sy); lastDiag += c;}
            }
            float total = 0.f; for(int a = 0; a < m; ++a) total += wq[a];
            std::vector<float> xc(m, 0.f); float cursor = -total * .5f;
            for(int q = 0; q < m; ++q) {const int a = order[q]; xc[a] = cursor + wq[a] * .5f; cursor += wq[a];}
            // vertex offsets in the reading frame, then the lowest point (for numbers lying at floor height)
            float low = 0.f;
            if(lift) {
                low = 1e9f;
                for(int a = 0; a < m; ++a) {
                    const Piece& p = pieces[mem[a]]; const size_t nv = std::min(p.vv.size(), p.vu.size()); if(nv == 0) continue;
                    double mv = 0; for(size_t j = 0; j < nv; ++j) mv += p.vv[j]; mv /= nv;
                    for(size_t j = 0; j < nv; ++j) low = std::min(low, -float(p.vv[j] - mv) * S[a]);
                }
                if(!std::isfinite(low) || low > 1e8f) low = 0.f;
            }
            for(int a = 0; a < m; ++a) {
                Piece& p = pieces[mem[a]];
                const size_t nv = std::min(std::min(p.vu.size(), p.vv.size()), p.ids.size()); if(nv == 0) continue;
                double mu = 0, mv = 0; for(size_t j = 0; j < nv; ++j) {mu += p.vu[j]; mv += p.vv[j];} mu /= nv; mv /= nv;
                for(size_t j = 0; j < nv; ++j) {
                    const size_t at = p.base + size_t(p.ids[j]) * p.stride;
                    if(at + 12 > arena.size()) continue;
                    const float ox = xc[a] + float(p.vu[j] - mu) * S[a], oy = -float(p.vv[j] - mv) * S[a] - (lift ? low : 0.f);
                    Vec3 q = add(C, add(scale(right, ox), scale(up, oy)));
                    if(haveFace && pull > 0.f) q = add(q, scale(face, pull));
                    float v[3] = {q.x, q.y, q.z}; std::memcpy(arena.data() + at, v, 12);
                }
                p.ox.assign(nv, 0.f); p.oy.assign(nv, 0.f);
                for(size_t j = 0; j < nv; ++j) {p.ox[j] = xc[a] + float(p.vu[j] - mu) * S[a]; p.oy[j] = -float(p.vv[j] - mv) * S[a] - (lift ? low : 0.f);}
                p.gC = C; p.gRight = right; p.gPull = (haveFace && pull > 0.f) ? pull : 0.f; p.gHave = true;
            }
        }
        return all;
    }

    bool nearInWorld(const Piece& a, const Piece& b) const {
        const float d = std::sqrt((a.centre.x - b.centre.x) * (a.centre.x - b.centre.x) + (a.centre.y - b.centre.y) * (a.centre.y - b.centre.y) + (a.centre.z - b.centre.z) * (a.centre.z - b.centre.z));
        const float size = std::max(a.pw * a.ext, b.pw * b.ext);
        return size > 0.f && std::isfinite(d) && d <= worldReach * size;
    }
    void regroup(std::vector<unsigned char>& arena) {
        const size_t n = pieces.size();
        if(n == 0) return;
        std::vector<int> group(n, -1); int groups = 0;
        for(size_t i = 0; i < n; ++i) if(group[i] < 0) {
            group[i] = groups; std::vector<size_t> stack{i};
            while(!stack.empty()) {
                const size_t c = stack.back(); stack.pop_back();
                for(size_t j = 0; j < n; ++j)
                    if(group[j] < 0 && (std::hypot(pieces[c].sx - pieces[j].sx, pieces[c].sy - pieces[j].sy) <= std::max(radius, reach * std::max(pieces[c].ext, pieces[j].ext)) || (worldReach > 0.f && nearInWorld(pieces[c], pieces[j])))) {group[j] = groups; stack.push_back(j);}
            }
            ++groups;
        }
        lastGroups = groups; lastGroup = group;
        if(reading) {if(layoutReading(arena, group, groups)) ++readingOk; else {++readingFallback; if(screenCopy) layoutScreen(arena, group, groups);} return;}
        if(screenCopy) {layoutScreen(arena, group, groups); return;}
        for(int g = 0; g < groups; ++g) {
            std::vector<size_t> members;
            Vec3 C{0, 0, 0}, R{0, 0, 0};
            for(size_t i = 0; i < n; ++i) if(group[i] == g) {members.push_back(i); C = add(C, pieces[i].centre); R = add(R, pieces[i].right);}
            std::vector<Vec3> want(members.size(), Vec3{0, 0, 0});
            if(members.size() >= 2) {
                C = scale(C, 1.f / float(members.size()));
                const float rl = std::sqrt(R.x * R.x + R.y * R.y + R.z * R.z);
                if(rl > 1e-6f) {
                    R = scale(R, 1.f / rl);
                    for(size_t m = 0; m < members.size(); ++m) {
                        const Vec3 d = sub(pieces[members[m]].centre, C);
                        const float along = d.x * R.x + d.y * R.y + d.z * R.z;
                        want[m] = scale(R, -2.f * along);
                    }
                }
            }
            for(size_t m = 0; m < members.size(); ++m) {
                Piece& p = pieces[members[m]];
                const Vec3 delta = sub(want[m], p.shift);
                if(delta.x == 0.f && delta.y == 0.f && delta.z == 0.f) continue;
                for(uint32_t id : p.ids) {
                    const size_t at = p.base + size_t(id) * p.stride;
                    if(at + 12 > arena.size()) continue;
                    float v[3]; std::memcpy(v, arena.data() + at, 12);
                    v[0] += delta.x; v[1] += delta.y; v[2] += delta.z;
                    std::memcpy(arena.data() + at, v, 12);
                }
                p.shift = want[m];
            }
        }
    }
};

}  // namespace digitnum
}  // namespace dk2vr
