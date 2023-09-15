#pragma once
#include "pix_frame.h"
#include "pix_draw_gif.h"
#include "pix_draw_line.h"
namespace pix
{
    void draw (
    frame<rgba> f, auto draw,
    line line, rgba color, double width, bool ends = true)
    {
        aa_line al;
        al.capline = ends;
        al.draw(line, color, width,
        [f,draw](int x, int y, rgba color)
        {
            if (0 <= x and x < f.size.x
            and 0 <= y and y < f.size.y)
                draw(x, y, color);
        });
    }

    void draw (
    frame<rgba> f, auto draw,
    circle c, rgba color, double width)
    {
        aa_line al;
        al.capline = false;
        double r = c.radius;
        if (r < 0.5) return;
        vector v1 {r, 0};
        vector v2 {r, 0};
        auto a = 1/(r+2);
        for (int i=0; i<int(ceil(2*pi/a)); i++)
        {
            v1 = v2;
            v2 = v2.rotated(a);
            al.draw(line{v1+c.center, v2+c.center}, color, width,
            [f,draw](int x, int y, rgba color)
            {
                if (0 <= x and x < f.size.x
                and 0 <= y and y < f.size.y)
                    draw(x, y, color);
            });
        }
    }

    void draw (
    frame<rgba> f, auto draw,
    circle c, rgba color, double width, double a1, double a2)
    {
        aa_line al;
        al.capline = false;
        double r = c.radius;
        if (r < 0.5) return;
        vector v1 {r, 0}; v1 = v1.rotated(a1);
        vector v2 {r, 0}; v2 = v2.rotated(a1);
        double a = 1/(r+2);
        if (a2 < a1) a = -a;
        for (int i=0; i<int(ceil((a2-a1)/a)); i++)
        {
            v1 = v2;
            v2 = v2.rotated(a);
            al.draw(line{v1+c.center, v2+c.center}, color, width,
            [f,draw](int x, int y, rgba color)
            {
                if (0 <= x and x < f.size.x
                and 0 <= y and y < f.size.y)
                    draw(x, y, color);
            });
        }
    }

    void draw (
    frame<rgba> f, auto draw,
    circle circle, rgba color)
    {
        auto c = circle.center;
        auto r = circle.radius;
        int y1 = max(int(floor(c.y - r)), 0);
        int y2 = min(int(ceil (c.y + r)), f.size.y-1);
        for(int y=y1; y<=y2; y++)
        {
            double a = asin((y-c.y)/r);
            double d = abs(r*cos(a));
            int x1 = max(int(floor(c.x - d)), 0);
            int x2 = min(int(ceil (c.x + d)), f.size.x-1);
            for(int x=x1; x<=x2; x++)
            draw(x, y, color);
        }
    }

    void draw (
    frame<rgba> f, auto Draw,
    polygon points, rgba color, double roundness, double width)
    {
        int n =
        points.size();
        if (roundness < 1)
        {
            for (int i=0; i<n; i++) {
            auto p1 = points[(i+0)%n];
            auto p2 = points[(i+1)%n];
            draw(f, Draw, line{p1,p2},
            color, width, false); }
            return;
        }

        array<vector> pp;
        pp.reserve(n*3);

        for (int i=0; i<n; i++)
        {
            auto p1 = points[(i+0)%n];
            auto p2 = points[(i+1)%n];
            auto p3 = points[(i+2)%n];
            auto v1 = (p1 - p2).normalized();
            auto v2 = (p3 - p2).normalized();
            auto aa = acos(v1*v2);
            auto kk = roundness/tan(aa/2);
            pp += p2 + kk*v1; pp += p2;
            pp += p2 + kk*v2;
        }
        int m =
        pp.size();
        for (int i=0; i<n; i++)
        {
            auto p1 = pp[(i*3+0)%m];
            auto p2 = pp[(i*3+1)%m];
            auto p3 = pp[(i*3+2)%m];
            auto p4 = pp[(i*3+3)%m];
            // quadratic Bézier curve
            auto r = (int)(roundness);
            auto v1 = (p2 - p1)/r;
            auto v2 = (p3 - p2)/r;
            auto p = p1;
            auto a = p1;
            auto b = p2;
            for (int j=0; j<r; j++)
            {
                a += v1;
                b += v2;
                auto q = a + (b-a)*(j+1)/r;
                draw(f, Draw, line{p,q},
                color, width, false);
                p = q;
            }
            draw(f, Draw, line{p3,p4},
            color, width, false);
        }
    }

    void draw (
    frame<rgba> f, auto Draw,
    polygon points, rgba color, double roundness)
    {
        draw(f, Draw, points, color, roundness, 0.5);

        if (points.empty()) return;
        
        xy p;
        for (vector v: points)
        p += xy{(int)(v.x), (int)(v.y)};
        p /= points.
             size();

        array<xy> queue;
        Draw(p.x, p.y, color);
        queue += p;

        auto proceed = [&](int x, int y)
        {
            if (x < 0 or f.size.x <= x
            or  y < 0 or f.size.y <= y
            or  f(x,y).a != 0) return;
            Draw(x, y, color);
            queue += xy(x,y);
        };

        while(not queue.empty())
        {
            p = queue.back();
            queue.pop_back();
            proceed(p.x+1, p.y);
            proceed(p.x-1, p.y);
            proceed(p.x, p.y+1);
            proceed(p.x, p.y-1);
        }

        draw(f, Draw, points, color, roundness, 2);
    }

    void copy (frame<rgba> f, auto... args)
    {
        draw(f,
        [f](int x, int y, rgba color) {
        f(x, y).accumulate(color); },
        args...);
    }
    void blend (frame<rgba> f, auto... args)
    {
        draw(f,
        [f](int x, int y, rgba color) {
        f(x, y).blend(color); },
        args...);
    }

    //template<>
    //frame<rgba>&
    //frame<rgba>::copy (line line, rgba color, double width) {
    //    pix::copy(*this, line, color, width);
    //    return *this;
    //}
    //template<>
    //frame<rgba>&
    //frame<rgba>::blend (line line, rgba color, double width) {
    //    pix::blend(*this, line, color, width);
    //    return *this;
    //}
    //template<>
    //frame<rgba>&
    //frame<rgba>::copy (circle c, rgba color, double width) {
    //    pix::copy(*this, c, color, width);
    //    return *this;
    //}
    //template<>
    //frame<rgba>&
    //frame<rgba>::blend (circle c, rgba color, double width) {
    //    pix::blend(*this, c, color, width);
    //    return *this;
    //}
    //template<>
    //frame<rgba>&
    //frame<rgba>::copy (circle c, rgba color) {
    //    pix::copy(*this, c, color);
    //    return *this;
    //}
    //template<>
    //frame<rgba>&
    //frame<rgba>::blend (circle c, rgba color) {
    //    pix::blend(*this, c, color);
    //    return *this;
    //}

    template<>
    frame<rgba>&
    frame<rgba>::copy (line line, rgba color, double width)
    {
        draw(*this,
        [this](int x, int y, rgba color) {
        (*this)(x, y).accumulate(color); },
        line, color, width);
        return *this;
    }
    template<>
    frame<rgba>&
    frame<rgba>::blend (line line, rgba color, double width)
    {
        draw(*this,
        [this](int x, int y, rgba color) {
        (*this)(x, y).blend(color); },
        line, color, width);
        return *this;
    }
    template<>
    frame<rgba>&
    frame<rgba>::copy (polygon polygon, rgba color, double roundness, double width)
    {
        draw(*this,
        [this](int x, int y, rgba color) {
        (*this)(x, y).accumulate(color); },
        polygon, color, roundness, width);
        return *this;
    }
    template<>
    frame<rgba>&
    frame<rgba>::blend (polygon polygon, rgba color, double roundness, double width)
    {
        draw(*this,
        [this](int x, int y, rgba color) {
        (*this)(x, y).blend(color); },
        polygon, color, roundness, width);
        return *this;
    }
    template<>
    frame<rgba>&
    frame<rgba>::copy (polygon polygon, rgba color, double roundness)
    {
        draw(*this,
        [this](int x, int y, rgba color) {
        (*this)(x, y).accumulate(color); },
        polygon, color, roundness);
        return *this;
    }
    template<>
    frame<rgba>&
    frame<rgba>::blend (polygon polygon, rgba color, double roundness)
    {
        draw(*this,
        [this](int x, int y, rgba color) {
        (*this)(x, y).blend(color); },
        polygon, color, roundness);
        return *this;
    }
    template<>
    frame<rgba>&
    frame<rgba>::copy (circle circle, rgba color, double width, double a1, double a2)
    {
        draw(*this,
        [this](int x, int y, rgba color) {
        (*this)(x, y).accumulate(color); },
        circle, color, width, a1, a2);
        return *this;
    }
    template<>
    frame<rgba>&
    frame<rgba>::blend (circle circle, rgba color, double width, double a1, double a2)
    {
        draw(*this,
        [this](int x, int y, rgba color) {
        (*this)(x, y).blend(color); },
        circle, color, width, a1, a2);
        return *this;
    }
    template<>
    frame<rgba>&
    frame<rgba>::copy (circle circle, rgba color, double width)
    {
        draw(*this,
        [this](int x, int y, rgba color) {
        (*this)(x, y).accumulate(color); },
        circle, color, width);
        return *this;
    }
    template<>
    frame<rgba>&
    frame<rgba>::blend (circle circle, rgba color, double width)
    {
        draw(*this,
        [this](int x, int y, rgba color) {
        (*this)(x, y).blend(color); },
        circle, color, width);
        return *this;
    }
    template<>
    frame<rgba>&
    frame<rgba>::copy (circle circle, rgba color)
    {
        draw(*this,
        [this](int x, int y, rgba color) {
        (*this)(x, y).accumulate(color); },
        circle, color);
        return *this;
    }
    template<>
    frame<rgba>&
    frame<rgba>::blend (circle circle, rgba color)
    {
        draw(*this,
        [this](int x, int y, rgba color) {
        (*this)(x, y).blend(color); },
        circle, color);
        return *this;
    }
}
