#pragma once
#include "pix_text.h"
namespace pix::text
{
    struct token: metrics
    {
        struct glyph: pix::glyph
        {
            xy offset;
            xywh coords() const {
                return xywh(
                    offset.x,
                    offset.y,
                    Width (),
                    Height()
                );
            }
        };
        array<glyph> glyphs;

        str text; style_index style;

        xy offset; // for external formatting

        token () = default;
        token (str text, style_index style) : text(text), style(style)
        {
            for (auto text: aux::unicode::glyphs(text))
            {
                auto g = pix::glyph(text, style);
                Ascent  = max(Ascent,  g.Ascent);
                ascent  = max(ascent,  g.ascent);
                Descent = max(Descent, g.Descent);
                descent = max(descent, g.descent);
                glyphs += glyph{g, xy{}};
            }

            if (glyphs.size() > 0) {
                lpadding = glyphs.front().lpadding;
                rpadding = glyphs.back ().rpadding;
            }

            for (auto& g: glyphs)
            {
                g.offset.x = advance;
                g.offset.y = Ascent - g.Ascent;
                advance += g.advance;
            }

            bearing = style.style().shift.x;
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
            xyxy r1 = glyphs[from].coords();
            xyxy r2 = glyphs[upto].coords();
            return xyxy(
                r1.x1, r1.y1,
                r2.x2, r2.y2);
        }

        int point (int x)
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