#pragma once
#include "aux_abc.h"
namespace pix::coord
{
    using std::abs;
    template<class T> struct XYWH;
    template<class T> struct XYXY;
    template<class T> struct XY
    {
        T x, y;

        XY (        ) : x(0), y(0) {}
        XY (T x, T y) : x(x), y(y) {}
        template<class U> explicit
        XY (XY<U> p) :
            x(clamp<T>(p.x)),
            y(clamp<T>(p.y)) {}

        void operator *= (T n) { x *= n; y *= n; };
        void operator /= (T n) { x /= n; y /= n; };

        void operator += (XY r) { x += r.x; y += r.y; };
        void operator -= (XY r) { x -= r.x; y -= r.y; };

        bool operator == (XY v) const { return x == v.x and y == v.y; }
        bool operator != (XY v) const { return not (*this == v); }

        friend XY operator + (XY a, XY b) { a += b; return a; }
        friend XY operator - (XY a, XY b) { a -= b; return a; }

        friend XY operator * (T n, XY v) { v *= n; return v; }
        friend XY operator * (XY v, T n) { v *= n; return v; }
        friend XY operator / (XY v, T n) { v /= n; return v; }
    };
    template<class T> struct XYXY
    {
        union  {
        struct { T l, t, r, b; };
        struct { T x1, y1, x2, y2; };
        struct { T xl, yl, xh, yh; };
        struct { T left,top, right,bottom; };
        struct { XY<T> left_top, right_bottom; };
        struct { XY<T> origin, end; };
        struct { XY<T> p1, p2; };
        struct { XY<T> lt, rb; };
        };

        XYXY (                  ) : l(0), t(0), r(0), b(0) {}
        XYXY (T l, T t, T r, T b) : l(l), t(t), r(r), b(b) {}
        template<class U> explicit
        XYXY (XYXY<U> q) :
            l(clamp<T>(q.l)),
            t(clamp<T>(q.t)),
            r(clamp<T>(q.r)),
            b(clamp<T>(q.b)) {}
        XYXY (XYWH<T>);

        explicit operator bool () { return l < r and t < b; }

        friend XYXY operator & (XYXY a, XYXY q) {
            if (not a or not q) return XYXY{};
            a.l = max(a.l, q.l); a.r = min(a.r, q.r);
            a.t = max(a.t, q.t); a.b = min(a.b, q.b);
            return a ? a : XYXY();
        }
        friend XYXY operator | (XYXY a, XYXY q) {
            if (not q) return a; if (not a) return q;
            a.l = min(a.l, q.l); a.r = max(a.r, q.r);
            a.t = min(a.t, q.t); a.b = max(a.b, q.b);
            return a;
        }
        void operator &= (XYXY q) { *this = *this & q; };
        void operator |= (XYXY q) { *this = *this | q; };

        void operator += (XYXY q) { l += q.l; t += q.t; r -= q.r; b -= q.b; }
        void operator -= (XYXY q) { l -= q.l; t -= q.t; r += q.r; b += q.b; }
        friend XYXY operator + (XYXY a, XYXY q) { a += q; return a; }
        friend XYXY operator - (XYXY a, XYXY q) { a -= q; return a; }

        void operator += (XY<T> p) { l += p.x; t += p.y; r += p.x; b += p.y; }
        void operator -= (XY<T> p) { l -= p.x; t -= p.y; r -= p.x; b -= p.y; }
        friend XYXY operator + (XYXY a, XY<T> p) { a += p; return a; }
        friend XYXY operator - (XYXY a, XY<T> p) { a -= p; return a; }

        void operator *= (T n) { l *= n; t *= n; r *= n; b *= n; }
        void operator /= (T n) { l /= n; t /= n; r /= n; b /= n; }
        friend XYXY operator * (XYXY a, T n) { a *= n; return a; }
        friend XYXY operator / (XYXY a, T n) { a /= n; return a; }

        bool operator != (XYXY q) const { return not (*this == q); }
        bool operator == (XYXY q) const { return
            l == q.l and t == q.t and
            r == q.r and b == q.b; }

        bool excludes (XY<T> p) const { return not includes(p); }
        bool includes (XY<T> p) const { return
            l <= p.x and p.x < r and
            t <= p.y and p.y < b; }

        void inflate (T n) { l -= n; t -= n; r += n; b += n; }
        void deflate (T n) { l += n; t += n; r -= n; b -= n; }

        XYXY local () const { return *this - origin; }
    };
    template<class T> struct XYWH
    {
        union  {
        struct { T x, y, w, h; };
        struct { T left, top, width, height; };
        struct { XY<T> origin, size; };
        };

        XYWH (                  ) : x(0), y(0), w(0), h(0) {}
        XYWH (T x, T y, T w, T h) : x(x), y(y), w(w), h(h) {}
        template<class U> explicit
        XYWH (XYWH<U> q) :
            x(clamp<T>(q.x)),
            y(clamp<T>(q.y)),
            w(clamp<T>(q.w)),
            h(clamp<T>(q.h)) {}
        XYWH (XYXY<T>);

        explicit operator bool () { return w > 0 and h > 0; }

        void operator &= (XYWH r) { *this = XYXY<T>(*this) & XYXY<T>(r); }
        void operator |= (XYWH r) { *this = XYXY<T>(*this) | XYXY<T>(r); }
        friend XYWH operator & (XYWH a, XYWH b) { a &= b; return a; }
        friend XYWH operator | (XYWH a, XYWH b) { a |= b; return a; }

        void operator += (XY<T> p) { x += p.x; y += p.y; }
        void operator -= (XY<T> p) { x -= p.x; y -= p.y; }
        friend XYWH operator + (XYWH r, XY<T> p) { r += p; return r; }
        friend XYWH operator - (XYWH r, XY<T> p) { r -= p; return r; }

        void operator *= (T n) { x *= n; y *= n; w *= n; h *= n; }
        void operator /= (T n) { x /= n; y /= n; w /= n; h /= n; }
        friend XYWH operator * (XYWH r, T n) { r *= n; return r; }
        friend XYWH operator / (XYWH r, T n) { r /= n; return r; }

        bool operator != (XYWH r) const { return not (*this == r); }
        bool operator == (XYWH r) const { return
            x == r.x and y == r.y and
            w == r.w and h == r.h; }

        bool excludes (XY<T> p) const { return not includes(p); }
        bool includes (XY<T> p) const { return
            x <= p.x and p.x < x + w and
            y <= p.y and p.y < y + h; }

        void inflate (T n) { x -= n; y -= n; w += n+n; h += n+n; }
        void deflate (T n) { x += n; y += n; w -= n+n; h -= n+n; }

        XYWH local () const { return *this - origin; }
    };

    template<class T> XYXY<T>::XYXY(XYWH<T> q) : l (q.x), t (q.y), r (q.x+q.w), b (q.y+q.h) {}
    template<class T> XYWH<T>::XYWH(XYXY<T> q) : x (q.l), y (q.t), w (q.r-q.l), h (q.b-q.t) {}

    template<class T> XYXY<T> operator & (XYXY<T> a, XYWH<T> b) { a &= XYXY<T>(b); return a; }
    template<class T> XYXY<T> operator | (XYXY<T> a, XYWH<T> b) { a |= XYXY<T>(b); return a; }
    template<class T> XYXY<T> operator & (XYWH<T> a, XYXY<T> b) { b &= XYXY<T>(a); return b; }
    template<class T> XYXY<T> operator | (XYWH<T> a, XYXY<T> b) { b |= XYXY<T>(a); return b; }

    template<class T> XYXY<T> operator * (double k, XYXY<T> r) {
        r.l *= k; r.r *= k;
        r.t *= k; r.b *= k;
        return r; }
    template<class T> XYWH<T> operator * (double k, XYWH<T> r) {
        r.x *= k; r.w *= k;
        r.y *= k; r.h *= k;
        return r; }

    template<> XYXY<int> operator * (double k, XYXY<int> r) {
        r.l = clamp<int>(std::lround(k*r.l));
        r.t = clamp<int>(std::lround(k*r.t));
        r.r = clamp<int>(std::lround(k*r.r));
        r.b = clamp<int>(std::lround(k*r.b));
        return r; }
    template<> XYWH<int> operator * (double k, XYWH<int> r) {
        r.x = clamp<int>(std::lround(k*r.x));
        r.y = clamp<int>(std::lround(k*r.y));
        r.w = clamp<int>(std::lround(k*r.w));
        r.h = clamp<int>(std::lround(k*r.h));
        return r; }


    template<class T> struct rectifier
    {
        array<XYWH<T>> rectangles;
        void operator  = (XYWH<T> r) { rectangles.clear(); rectangles += r; }
        void operator += (XYWH<T> r) {
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
        typedef array<XYWH<T>>::iterator iterator;
        iterator begin () { return rectangles.begin(); }
        iterator end   () { return rectangles.end  (); }
        int      size  () { return rectangles.size (); }
        bool     empty () { return rectangles.empty(); }
        void     clear () { rectangles.clear(); }
    };
}

namespace pix
{
    using real = aux::fixed<32,32>;
    using XYXY = coord::XYXY<real>;
    using XYWH = coord::XYWH<real>;
    using XY   = coord::XY  <real>;

    using xyxy = coord::XYXY<int>;
    using xywh = coord::XYWH<int>;
    using xy   = coord::XY  <int>;

    ///////////////////////////////////////////////////////////////////////
                                const  int
    ///////////////////////////////////////////////////////////////////////
                                top    =-1,
    justify_left =-2, left =-1, center = 0, right = 1, justify_right = 2,
                                bottom = 1;
    ///////////////////////////////////////////////////////////////////////

    enum class geo
    {
        points,
        lines,
        line_strip,
        line_loop,
        triangles,
        triangle_strip,
        triangle_fan,
    };
}
