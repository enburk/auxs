#pragma once
#include "pix_frame.h"
#include "pix_draw_line.h"
namespace pix
{
    template<>
    frame<rgba>&
    frame<rgba>::copy (line line, rgba color, double width)
    {
        aa_line al;
        al.draw(line, color, width,
        [this](int x, int y, rgba color)
        {
            if (0 <= x and x < size.x
            and 0 <= y and y < size.y)
            (*this)(x, y) = color;
        });
        return *this;

    }
    template<>
    frame<rgba>&
    frame<rgba>::blend (line line, rgba color, double width)
    {
        aa_line al;
        al.draw(line, color, width,
        [this](int x, int y, rgba color)
        {
            if (0 <= x and x < size.x
            and 0 <= y and y < size.y)
            (*this)(x, y).blend(color);
        });
        return *this;
    }
    template<>
    frame<rgba>&
    frame<rgba>::copy (circle c, rgba color, double width)
    {
        aa_line al;
        double r = c.radius;
        vector v1 {r, 0};
        vector v2 {r, 0};
        auto a = 1/(r+1);
        for (int i=0; i<int(ceil(2*pi/a)); i++) {
        v1 = v2; v2 = v2.rotated(a);
        al.draw(line{v1+c.center, v2+c.center}, color, width,
        [this](int x, int y, rgba color)
        {
            if (0 <= x and x < size.x
            and 0 <= y and y < size.y)
            (*this)(x, y) = color;
        });
        }
        return *this;
    }
    template<>
    frame<rgba>&
    frame<rgba>::blend (circle c, rgba color, double width)
    {
        aa_line al;
        double r = c.radius;
        vector v1 {r, 0};
        vector v2 {r, 0};
        auto a = 1/(r+1);
        for (int i=0; i<int(ceil(2*pi/a)); i++) {
        v1 = v2; v2 = v2.rotated(a);
        al.draw(line{v1+c.center, v2+c.center}, color, width,
        [this](int x, int y, rgba color)
        {
            if (0 <= x and x < size.x
            and 0 <= y and y < size.y)
            (*this)(x, y).blend(color);
        });
        }
        return *this;
    }
    template<>
    frame<rgba>&
    frame<rgba>::copy (circle circle, rgba color)
    {
        aa_line al;
        auto c = circle.center;
        auto r = circle.radius;
        int y1 = max(int(floor(c.y - r)), 0);
        int y2 = min(int(ceil (c.y + r)), size.y-1);
        for(int y=y1; y<=y2; y++)
        {
            double a = asin((y-c.y)/r);
            double d = abs(r*cos(a));
            int x1 = max(int(floor(c.x - d)), 0);
            int x2 = min(int(ceil (c.x + d)), size.x-1);
            for(int x=x1; x<=x2; x++)
            (*this)(x, y) = color;
        }
        return *this;
    }
    template<>
    frame<rgba>&
    frame<rgba>::blend (circle circle, rgba color)
    {
        aa_line al;
        auto c = circle.center;
        auto r = circle.radius;
        int y1 = max(int(floor(c.y - r)), 0);
        int y2 = min(int(ceil (c.y + r)), size.y-1);
        for(int y=y1; y<=y2; y++)
        {
            double a = asin((y-c.y)/r);
            double d = abs(r*cos(a));
            int x1 = max(int(floor(c.x - d)), 0);
            int x2 = min(int(ceil (c.x + d)), size.x-1);
            for(int x=x1; x<=x2; x++)
            (*this)(x, y).blend(color);
        }
        return *this;
    }
}
