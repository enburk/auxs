#pragma once
#include "pix_text.h"
namespace pix::text
{
    struct token: metrics
    {
        str text;
        style_index style;
        array<glyph> glyphs;
        xy offset;

        token () = default;
        token (str text, style_index style) : text(text), style(style)
        {
            for (auto text: aux::unicode::glyphs(text))
            {
                auto g = glyph(text, style);
                metrics::operator += (g);
                glyphs += g;
            }
            int x = 0;
            for (auto& g: glyphs)
            {
                g.offset.x = x;
                g.offset.y = Ascent - g.Ascent;
                x += g.advance;
            }
        }

        void render (frame<rgba> frame, xy shift=xy{}, uint8_t alpha=255)
        {
            for (auto& g: glyphs)
            {
                auto w = g.Width();
                auto h = g.Height();
                auto p = offset + shift + g.offset;
                auto f = frame.crop(xywh(p.x, p.y, w, h));
                g.render(f, xy{}, alpha);
            }
        }

        xywh bar (int from, int upto)
        {
            from = max(from, 0);
            upto = min(upto, glyphs.size());
            if (from >= upto) return xywh{}; upto--;
            return xyxy(
            glyphs[from].offset.x,
            glyphs[from].offset.y,
            glyphs[upto].offset.x + glyphs[upto].Width(),
            glyphs[upto].offset.y + glyphs[upto].Height());
        }

        int pointed (int x)
        {
            int i = 0;
            for (auto g : glyphs)
                if (g.offset.x +
                    g.Width() > x)
                    return i;
                    else i++;

            return glyphs.size()-1;
        }
    };
} 