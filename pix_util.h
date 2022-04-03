#pragma once
#include "pix_text.h"
#include "pix_draw.h"
namespace pix::util
{
    auto node (str s, rgba outer, rgba inner, rgba text, int d = 0, font f=font{})
    {
        text::style style;
        style.color = text;
        style.font = f;
        text::token t(s, style);
        int w = t.width - t.lpadding;
        int h = t.ascent_+ t.descent_;
        int r = d>0 ? d/2: int(0.6 * sqrt(w*w + h*h) + 4);
        image image(xy(2*r+2,2*r+2), rgba{});
        frame frame = image.crop();
        frame.copy(circle{{r+1,r+1}, double(r-1)}, inner);
        frame.copy(circle{{r+1,r+1}, double(r-0)}, outer, 2.0);
        frame.copy(circle{{r+1,r+1}, double(r-1)}, outer, 2.0);
        frame.copy(circle{{r+1,r+1}, double(r-2)}, outer, 2.0);
        int dx = t.lpadding;
        int dy = t.ascent - t.ascent_;
        t.render(frame, xy(r+1-w/2-dx, r+1-h/2-dy));
        return image;
    }
}