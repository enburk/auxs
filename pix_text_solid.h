#pragma once
#include "pix_text_token.h"
namespace pix::text
{
    using token_range = array<token>::range_type;

    struct solid : metrics
    {
        str text;
        array<glyph> glyphs;
        xy offset;

        solid(token_range tokens)
        {
            for (auto& token: tokens)
            for (auto& glyph: token.glyphs)
            {
                text += glyph.text;
                glyphs += glyph;
            }
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
                auto ellipsis = glyph(u8"…",
                    glyphs.back().style_index);

                if (glyphs.back().text == " ")
                    glyphs.truncate();

                glyphs += ellipsis; layout();
                if (rborder < max_width) return;
                glyphs.truncate();
                glyphs.truncate();
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

        //xywh bar (int place)
        //{
        //    if (place < 0 or
        //        place >= length)
        //        return xywh{};
        //
        //    for (auto& token : tokens) {
        //        xywh r = token.bar(place, place+1);
        //        if (r != xywh{}) return r + token.offset;
        //        place -= token.glyphs.size(); }
        //
        //    return xywh{};
        //}
        //
        //array<xywh> bars (int from, int upto)
        //{
        //    array<xywh> bars;
        //    from = max(0, from);
        //    for (auto& token : tokens)
        //    {
        //        xywh r = token.bar(from, upto) + token.offset;
        //        from -= token.glyphs.size();
        //        upto -= token.glyphs.size();
        //        if (r.w == 0 or
        //            r.h == 0)
        //            continue;
        //        if (bars.size() > 0
        //        and bars.back().y == r.y
        //        and bars.back().h == r.h)
        //            bars.back() |= r; else
        //            bars += r;
        //    }
        //    return bars;
        //}
        //
        //int point (int x)
        //{
        //    int i = 0;
        //    for (auto& token: tokens)
        //        if (token.offset.x +
        //            token.Width() > x) return
        //            i + token.point(
        //            x - token.offset.x);
        //        else i += token.glyphs.size();
        //
        //    return length-1;
        //}
    };
} 