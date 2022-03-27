#pragma once
#include "pix_frame.h"
#include "pix_draw_line.h"
namespace pix
{
    void draw (
    frame<rgba> f, auto draw,
    line line, rgba color, double width)
    {
        aa_line al;
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
    circle circle, rgba color)
    {
        aa_line al;
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
