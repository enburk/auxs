#pragma once
#include "aux_abc.h"
namespace pix::coord
{
    using std::abs;
    template<class T> struct xywh;
    template<class T> struct xyxy;
    template<class T> struct xy
    {
        int x, y;

        xy (        ) : x(0), y(0) {}
        xy (T x, T y) : x(x), y(y) {}

        void operator *= (T n) { x *= n; y *= n; };
        void operator /= (T n) { x /= n; y /= n; };

        void operator += (xy r) { x += r.x; y += r.y; };
        void operator -= (xy r) { x -= r.x; y -= r.y; };

        bool operator == (xy v) const { return x == v.x and y == v.y; }
        bool operator != (xy v) const { return not (*this == v); }

        friend xy operator + (xy a, xy b) { a += b; return a; }
        friend xy operator - (xy a, xy b) { a -= b; return a; }

        friend xy operator * (T n, xy v) { v *= n; return v; }
        friend xy operator * (xy v, T n) { v *= n; return v; }
        friend xy operator / (xy v, T n) { v /= n; return v; }
    };
    template<class T> struct xyxy
    {
        union  {
        struct { T l, t, r, b; };
        struct { T x1, y1, x2, y2; };
        struct { T xl, yl, xh, yh; };
        struct { T left,top, right,bottom; };
        struct { xy<T> left_top, right_bottom; };
        struct { xy<T> origin, end; };
        struct { xy<T> p1, p2; };
        struct { xy<T> lt, rb; };
        };

        xyxy (                  ) : l(0), t(0), r(0), b(0) {}
        xyxy (T l, T t, T r, T b) : l(l), t(t), r(r), b(b) {}
        xyxy (xy<T> p1, xy<T> p2) : p1(p1), p2(p2) {}
        xyxy (xywh<T>);

        explicit operator bool () { return l < r and t < b; }

        friend xyxy operator & (xyxy a, xyxy q) {
            if (not a or not q) return xyxy{};
            a.l = max(a.l, q.l); a.r = min(a.r, q.r);
            a.t = max(a.t, q.t); a.b = min(a.b, q.b);
            return a ? a : xyxy();
        }
        friend xyxy operator | (xyxy a, xyxy q) {
            if (not q) return a; if (not a) return q;
            a.l = min(a.l, q.l); a.r = max(a.r, q.r);
            a.t = min(a.t, q.t); a.b = max(a.b, q.b);
            return a;
        }
        void operator &= (xyxy q) { *this = *this & q; };
        void operator |= (xyxy q) { *this = *this | q; };

        void operator += (xyxy q) { l += q.l; t += q.t; r -= q.r; b -= q.b; }
        void operator -= (xyxy q) { l -= q.l; t -= q.t; r += q.r; b += q.b; }
        friend xyxy operator + (xyxy a, xyxy q) { a += q; return a; }
        friend xyxy operator - (xyxy a, xyxy q) { a -= q; return a; }

        void operator += (xy<T> p) { l += p.x; t += p.y; r += p.x; b += p.y; }
        void operator -= (xy<T> p) { l -= p.x; t -= p.y; r -= p.x; b -= p.y; }
        friend xyxy operator + (xyxy a, xy<T> p) { a += p; return a; }
        friend xyxy operator - (xyxy a, xy<T> p) { a -= p; return a; }

        void operator *= (T n) { l *= n; t *= n; r *= n; b *= n; }
        void operator /= (T n) { l /= n; t /= n; r /= n; b /= n; }
        friend xyxy operator * (xyxy a, T n) { a *= n; return a; }
        friend xyxy operator / (xyxy a, T n) { a /= n; return a; }

        bool operator != (xyxy q) const { return not (*this == q); }
        bool operator == (xyxy q) const { return
            l == q.l and t == q.t and
            r == q.r and b == q.b; }

        bool excludes (xy<T> p) const { return not includes(p); }
        bool includes (xy<T> p) const { return
            l <= p.x and p.x < r and
            t <= p.y and p.y < b; }

        void inflate (T n) { l -= n; t -= n; r += n; b += n; }
        void deflate (T n) { l += n; t += n; r -= n; b -= n; }

        xyxy local () const { return *this - origin; }
    };
    template<class T> struct xywh
    {
        union  {
        struct { T x, y, w, h; };
        struct { T left, top, width, height; };
        struct { xy<T> origin, size; };
        };

        xywh (                  ) : x(0), y(0), w(0), h(0) {}
        xywh (T x, T y, T w, T h) : x(x), y(y), w(w), h(h) {}
        xywh (xy<T> oo, xy<T> wh) : origin(oo), size(wh) {}
        xywh (xyxy<T>);

        explicit operator bool () { return w > 0 and h > 0; }

        void operator &= (xywh r) { *this = xyxy<T>(*this) & xyxy<T>(r); }
        void operator |= (xywh r) { *this = xyxy<T>(*this) | xyxy<T>(r); }
        friend xywh operator & (xywh a, xywh b) { a &= b; return a; }
        friend xywh operator | (xywh a, xywh b) { a |= b; return a; }

        void operator += (xy<T> p) { x += p.x; y += p.y; }
        void operator -= (xy<T> p) { x -= p.x; y -= p.y; }
        friend xywh operator + (xywh r, xy<T> p) { r += p; return r; }
        friend xywh operator - (xywh r, xy<T> p) { r -= p; return r; }

        void operator *= (T n) { x *= n; y *= n; w *= n; h *= n; }
        void operator /= (T n) { x /= n; y /= n; w /= n; h /= n; }
        friend xywh operator * (xywh r, T n) { r *= n; return r; }
        friend xywh operator / (xywh r, T n) { r /= n; return r; }

        bool operator != (xywh r) const { return not (*this == r); }
        bool operator == (xywh r) const { return
            x == r.x and y == r.y and
            w == r.w and h == r.h; }

        bool excludes (xy<T> p) const { return not includes(p); }
        bool includes (xy<T> p) const { return
            x <= p.x and p.x < x + w and
            y <= p.y and p.y < y + h; }

        void inflate (T n) { x -= n; y -= n; w += n+n; h += n+n; }
        void deflate (T n) { x += n; y += n; w -= n+n; h -= n+n; }

        xywh local () const { return *this - origin; }
    };

    template<class T> xyxy<T>::xyxy(xywh<T> q) : l (q.x), t (q.y), r (q.x+q.w), b (q.y+q.h) {}
    template<class T> xywh<T>::xywh(xyxy<T> q) : x (q.l), y (q.t), w (q.r-q.l), h (q.b-q.t) {}

    template<class T> xyxy<T> operator & (xyxy<T> a, xywh<T> b) { a &= xyxy<T>(b); return a; }
    template<class T> xyxy<T> operator | (xyxy<T> a, xywh<T> b) { a |= xyxy<T>(b); return a; }
    template<class T> xyxy<T> operator & (xywh<T> a, xyxy<T> b) { b &= xyxy<T>(a); return b; }
    template<class T> xyxy<T> operator | (xywh<T> a, xyxy<T> b) { b |= xyxy<T>(a); return b; }

    template<class T> xyxy<T> operator * (double k, xyxy<T> r) {
        r.l *= k; r.r *= k;
        r.t *= k; r.b *= k;
        return r; }
    template<class T> xywh<T> operator * (double k, xywh<T> r) {
        r.x *= k; r.w *= k;
        r.y *= k; r.h *= k;
        return r; }

    template<> xyxy<int> operator * (double k, xyxy<int> r) {
        r.l = clamp<int>(std::lround(k*r.l));
        r.t = clamp<int>(std::lround(k*r.t));
        r.r = clamp<int>(std::lround(k*r.r));
        r.b = clamp<int>(std::lround(k*r.b));
        return r; }
    template<> xywh<int> operator * (double k, xywh<int> r) {
        r.x = clamp<int>(std::lround(k*r.x));
        r.y = clamp<int>(std::lround(k*r.y));
        r.w = clamp<int>(std::lround(k*r.w));
        r.h = clamp<int>(std::lround(k*r.h));
        return r; }


    template<class T> struct rectifier
    {
        array<xywh<T>> rectangles;
        void operator  = (xywh<T> r) { rectangles.clear(); rectangles += r; }
        void operator += (xywh<T> r) {
            if (!r) return;
            for (auto
                i = rectangles.rbegin();
                i != rectangles.rend(); ++i)
            {
                auto & R = *i;
                xywh u = R | r;
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
        typedef array<xywh<T>>::iterator iterator;
        iterator begin () { return rectangles.begin(); }
        iterator end   () { return rectangles.end  (); }
        int      size  () { return rectangles.size (); }
        bool     empty () { return rectangles.empty(); }
        void     clear () { rectangles.clear(); }
    };
}

namespace pix
{
    using xyxy = coord::xyxy<int>;
    using xywh = coord::xywh<int>;
    using xy   = coord::xy  <int>;

    using vector  = aux::vector<2>;
    using line    = aux::line<vector>;
    using segment = aux::segment<vector>;
    using circle  = aux::circle;

    ////////////////////////////////
              const  int
    ////////////////////////////////
              top    =-1,
    left =-1, center = 0, right = 1,
              bottom = 1,
    ////////////////////////////////

    justify_left  =-99,
    justify_right = 99,

    center_of_area =-2,
    center_of_mass = 2;

    enum class geo
    {
        none,
        points,
        lines,
        line_strip,
        line_loop,
        triangles,
        triangle_strip,
        triangle_fan,
    };
}
