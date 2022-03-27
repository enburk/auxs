#pragma once
#include "pix_text.h"
#include "pix_draw.h"
namespace pix::util
{
    auto node (str s, rgba outer, rgba inner, rgba text, font f=font{})
    {
        glyph g(s, text::style{f, text});
        int w = g.width;
        int h = g.ascent+g.descent;
        int r = max(w, h);
        image image(xy(2*r+2,2*r+2), rgba{});
        frame frame = image.crop();
        //frame.copy(circle{{r+1,r+1}, double(r-2)}, inner);
        frame.copy(circle{{r+1,r+1}, double(r-2)}, outer, 2.0);
        frame.copy(circle{{r+1,r+1}, double(r-1)}, outer, 2.0);
        frame.copy(circle{{r+1,r+1}, double(r-0)}, outer, 2.0);
        //g.render(frame, xy(r+1-w/2, r+1-h/2));
        return image;
    }
}