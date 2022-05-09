#pragma once
#include "pix_text.h"
namespace pix::text
{
    struct token: metrics
    {
        str text;
        str link;
        str info;
        style_index style;
        array<glyph> glyphs;
        xy offset;

        token () = default;
        token (str text, style_index style) : text(text), style(style)
        {
            for (auto text: aux::unicode::glyphs(text))
            glyphs += glyph(text, style);
            layout();
        }

        void layout ()
        {
            metrics::operator = (metrics{});
            for (auto& g: glyphs)
            metrics::operator += (g);

            advance = 0;
            for (auto& g: glyphs)
            {
                g.offset.x = advance;
                g.offset.y = Ascent - g.Ascent;
                advance += g.advance;
            }
        }

        void ellipt(int max_width)
        {
            while (not glyphs.empty())
            {
                while (
                glyphs.back().text == " ")
                glyphs.truncate();
                glyphs += glyph(u8"…", style);
                layout(); if (rborder <= max_width) return;
                glyphs.truncate();
                glyphs.truncate();
                layout();
            }
        }

        void render (frame<rgba> frame, xy shift=xy{}, uint8_t alpha=255)
        {
            for (auto& g: glyphs)
            {
                auto w = g.Width();
                auto h = g.Height();
                auto p = shift + offset + g.offset;
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
                    g.Width()/2 > x)
                    return i;
                    else i++;

            return glyphs.size()-1;
        }
    };
} 