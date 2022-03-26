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
}
