#pragma once
#include "pix_draw.h"
#include "pix_text_block.h"
namespace pix::util
{
    auto node (str s, rgba outer, rgba inner, rgba text, int d = 0, font f=font{})
    {
        text::style style;
        style.color = text;
        style.font = f;
        text::token t(s, style);
        int w = t.width();
        int h = t.height();
        int r = d > 0 ? d/2 :
        int(0.6 * sqrt(w*w + h*h) + 4);
        image image(xy(2*r+2,2*r+2), rgba{});
        frame frame = image.crop();
        frame.copy(circle{{r+1,r+1}, double(r-1)}, inner);
        frame.copy(circle{{r+1,r+1}, double(r-0)}, outer, 2.0);
        frame.copy(circle{{r+1,r+1}, double(r-1)}, outer, 2.0);
        frame.copy(circle{{r+1,r+1}, double(r-2)}, outer, 2.0);
        int dx = t.lborder;
        int dy = t.Ascent - t.ascent;
        t.render(frame, xy(r+1-w/2-dx, r+1-h/2-dy));
        return image;
    }

    auto icon (str s, xy size, rgba color, int roundness = 0)
    {
        image image(size, rgba{});
        frame frame = image.crop();

        vector c {size.x/2.0, size.y/2.0};
        vector x {size.x/2.0, 0};
        vector y {0, size.y/2.0};
        
        if (s == "play.play")
        {
            frame.copy(polygon{
            c - 0.9*x - 0.9*y,
            c + 0.9*x + 0.0*y,
            c - 0.9*x + 0.9*y},
            color, roundness);
        }
        else
        if (s == "play.stop")
        {
            frame.copy(polygon{
            c - 0.8*x - 0.8*y,
            c + 0.8*x - 0.8*y, 
            c + 0.8*x + 0.8*y,
            c - 0.8*x + 0.8*y},
            color, roundness);
        }
        else
        {
            auto d = min(size.x/4, size.y/4);
            xyxy r {xy{}, size};  r.deflate(d);
            vector lt {r.l, r.t}; vector rt {r.r, r.t};
            vector lb {r.l, r.b}; vector rb {r.r, r.b};
            frame.copy(line{lt, rb}, rgba::white, d);
            frame.copy(line{rt, lb}, rgba::white, d);
            frame.copy(line{lt, rb}, rgba::red, d/2);
            frame.copy(line{rt, lb}, rgba::red, d/2);
        }
        return image;
    }
}