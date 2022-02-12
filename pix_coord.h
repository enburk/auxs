#pragma once
#include "aux_abc.h"
namespace pix
{
    using namespace data;
    using real = double;
    using std::abs;
    struct XYWH;
    struct XYXY;
    struct XY
    {
        int x, y;

        XY (            ) : x (0), y (0) {}
        XY (int x, int y) : x (x), y (y) {}

        void operator += (XY r) { x += r.x; y += r.y; };
        void operator -= (XY r) { x -= r.x; y -= r.y; };

        void operator *= (int n) { x *= n; y *= n; };
        void operator /= (int n) { x /= n; y /= n; };

        bool operator == (XY v) const { return x == v.x && y == v.y; }
        bool operator != (XY v) const { return ! (*this == v); }

        friend XY operator + (XY a, XY b) { a += b; return a; }
        friend XY operator - (XY a, XY b) { a -= b; return a; }

        friend XY operator * (int n, XY v) { v *= n; return v; }
        friend XY operator * (XY v, int n) { v *= n; return v; }
        friend XY operator / (XY v, int n) { v /= n; return v; }
    };
    struct XYXY
    {
        union  {
        struct { int l, t, r, b; };
        struct { int x1, y1, x2, y2; };
        struct { int xl, yl, xh, yh; };
        struct { int left,top, right,bottom; };
        struct { XY  left_top, right_bottom; };
        struct { XY  origin, end; };
        struct { XY  p1, p2; };
        struct { XY  lt, rb; };
        };

        XYXY (                          ) : l (0), t (0), r (0), b (0) {}
        XYXY (int l, int t, int r, int b) : l (l), t (t), r (r), b (b) {}
        XYXY (XYWH);

        explicit operator bool () { return r > l && b > t; }

        friend XYXY operator & (XYXY a, XYXY q) {
            if (!a || !q) return XYXY();
            a.l = max(a.l, q.l); a.t = max(a.t, q.t);
            a.r = min(a.r, q.r); a.b = min(a.b, q.b);
            return a ? a : XYXY();
        }
        friend XYXY operator | (XYXY a, XYXY q) {
            if (!q) return a; if (!a) return q;
            a.l = min(a.l, q.l); a.t = min(a.t, q.t);
            a.r = max(a.r, q.r); a.b = max(a.b, q.b);
            return a;
        }
        void operator &= (XYXY q) { *this = *this & q; };
        void operator |= (XYXY q) { *this = *this | q; };

        void operator += (XYXY q) { l += q.l; t += q.t; r -= q.r; b -= q.b; }
        void operator -= (XYXY q) { l -= q.l; t -= q.t; r += q.r; b += q.b; }
        friend XYXY operator + (XYXY a, XYXY q) { a += q; return a; }
        friend XYXY operator - (XYXY a, XYXY q) { a -= q; return a; }

        void operator += (XY p) { l += p.x; t += p.y; r += p.x; b += p.y; }
        void operator -= (XY p) { l -= p.x; t -= p.y; r -= p.x; b -= p.y; }
        friend XYXY operator + (XYXY a, XY p) { a += p; return a; }
        friend XYXY operator - (XYXY a, XY p) { a -= p; return a; }

        void operator *= (int n) { l *= n; t *= n; r *= n; b *= n; }
        void operator /= (int n) { l /= n; t /= n; r /= n; b /= n; }
        friend XYXY operator * (XYXY a, int n) { a *= n; return a; }
        friend XYXY operator / (XYXY a, int n) { a /= n; return a; }

        bool operator == (XYXY q) const { return l == q.l && t == q.t && r == q.r && b == q.b; }
        bool operator != (XYXY q) const { return ! (*this == q); }

        bool includes (XY p) const { return l <= p.x && p.x <  r && t <= p.y && p.y <  b; }
        bool excludes (XY p) const { return !includes(p); }

        void inflate (int n) { l -= n; t -= n; r += n; b += n; }
        void deflate (int n) { l += n; t += n; r -= n; b -= n; }

        XYXY local () const { return *this - origin; }
    };
    struct XYWH
    {
        union  {
        struct { int x, y, w, h; };
        struct { int left, top, width, height; };
        struct { XY  origin, size; };
        };

        XYWH (                          ) : x (0), y (0), w (0), h (0) {}
        XYWH (int x, int y, int w, int h) : x (x), y (y), w (w), h (h) {}
        XYWH (XYXY);

        explicit operator bool () { return w > 0 && h > 0; }

        void operator &= (XYWH r) { *this = XYXY(*this) & XYXY(r); }
        void operator |= (XYWH r) { *this = XYXY(*this) | XYXY(r); }
        friend XYWH operator & (XYWH a, XYWH b) { a &= b; return a; }
        friend XYWH operator | (XYWH a, XYWH b) { a |= b; return a; }

        void operator += (XY p) { x += p.x; y += p.y; }
        void operator -= (XY p) { x -= p.x; y -= p.y; }
        friend XYWH operator + (XYWH r, XY p) { r += p; return r; }
        friend XYWH operator - (XYWH r, XY p) { r -= p; return r; }

        void operator *= (int n) { x *= n; y *= n; w *= n; h *= n; }
        void operator /= (int n) { x /= n; y /= n; w /= n; h /= n; }
        friend XYWH operator * (XYWH r, int n) { r *= n; return r; }
        friend XYWH operator / (XYWH r, int n) { r /= n; return r; }

        bool operator != (XYWH r) const { return ! (*this == r); }
        bool operator == (XYWH r) const { return
            x == r.x and y == r.y and
            w == r.w and h == r.h; }

        bool excludes (XY p) const { return !includes(p); }
        bool includes (XY p) const { return
            x <= p.x and p.x < x + w and
            y <= p.y and p.y < y + h; }

        void inflate (int n) { x -= n; y -= n; w += n+n; h += n+n; }
        void deflate (int n) { x += n; y += n; w -= n+n; h -= n+n; }

        XYWH local () const { return *this - origin; }
    };

    inline XYXY::XYXY(XYWH q) : l (q.x), t (q.y), r (q.x+q.w), b (q.y+q.h) {}
    inline XYWH::XYWH(XYXY q) : x (q.l), y (q.t), w (q.r-q.l), h (q.b-q.t) {}

    inline XYXY operator & (XYXY a, XYWH b) { a &= XYXY(b); return a; }
    inline XYXY operator | (XYXY a, XYWH b) { a |= XYXY(b); return a; }
    inline XYXY operator & (XYWH a, XYXY b) { b &= XYXY(a); return b; }
    inline XYXY operator | (XYWH a, XYXY b) { b |= XYXY(a); return b; }

    inline XYXY operator * (double k, XYXY r) {
        r.l = clamp<int>(std::lround(k*r.l)); r.r = clamp<int>(std::lround(k*r.r));
        r.t = clamp<int>(std::lround(k*r.t)); r.b = clamp<int>(std::lround(k*r.b));
        return r; }
    inline XYWH operator * (double k, XYWH r) {
        r.x = clamp<int>(std::lround(k*r.x)); r.w = clamp<int>(std::lround(k*r.w));
        r.y = clamp<int>(std::lround(k*r.y)); r.h = clamp<int>(std::lround(k*r.h));
        return r; }

    struct rectifier
    {
        array<XYWH> rectangles;
        void operator  = (XYWH r) { rectangles.clear(); rectangles += r; }
        void operator += (XYWH r) {
            if (!r) return;
            for (auto
                i = rectangles.rbegin();
                i != rectangles.rend(); ++i)
            {
                auto & R = *i;
                XYWH u = R | r;
                auto Rsquare = R.w * R.h;
                auto rsquare = r.w * r.h;
                auto usquare = u.w * u.h;
                if  (usquare > Rsquare + rsquare) continue;
                std::swap(R, rectangles.back());
                rectangles.pop_back();
                *this += u; // recursion
                return;
            }
            rectangles += r;
        }
        typedef array<XYWH>::iterator iterator;
        iterator begin () { return rectangles.begin(); }
        iterator end   () { return rectangles.end  (); }
        int      size  () { return rectangles.size (); }
        void     clear () { rectangles.clear(); }
    };

    ///////////////////////////////////////////////////////////////////////
                                const  int
    ///////////////////////////////////////////////////////////////////////
                                top    =-1,
    justify_left =-2, left =-1, center = 0, right = 1, justify_right = 2,
                                bottom = 1;
    ///////////////////////////////////////////////////////////////////////
}