#pragma once
#include "pix_text_token.h"
namespace pix::text
{
    using token_range = array<token>::range_type;

    struct solid : metrics
    {
        token_range tokens;
        token ellipted;
        glyph ellipsis;
        int length = 0;
        xy offset;

        solid(token_range tokens) : tokens(tokens)
        {
            measure();
            int x = 0;
            for (auto& t: tokens)
            {
                t.offset.x = x;
                t.offset.y = Ascent - t.Ascent;
                x += t.advance;
            }
        }

        void measure ()
        {
            metrics::operator = (metrics{});
            for (auto& token: tokens)
            metrics::operator += (token);
            if (ellipted.text != "")
            metrics::operator += (ellipted);
            if (ellipsis.text != "")
            metrics::operator += (ellipsis);

            length = 0;
            for (auto& token: tokens)
            length += token.glyphs.size();
            if (ellipted.text != "")
            length += ellipted.glyphs.size();
        }



        //void ellipt(int max_width, token& last)
        //{
        //    advance -= last.advance;
        //
        //    token t = last;
        //
        //    str text = t.text;
        //
        //    do
        //    {
        //        text.trimr();
        //        t.text = text + (char*)(u8"…");
        //        text.truncate();
        //        last = t;
        //    }
        //    while (advance + last.width > max_width
        //        and t.text != (char*)(u8"…"));
        //
        //    width = advance + last.width;
        //
        //    advance += last.advance;
        //}
        //
        //void ellipt(int max_width)
        //{
        //    while (tokens.size() > 0)
        //    {
        //        auto& token = tokens.last();
        //        ellipt(max_width, token);
        //        if (width <= max_width)
        //            return;
        //
        //        tokens.truncate();
        //        solid s {tokens};
        //        metrics::operator=(s);
        //    }
        //}

        void render (frame<rgba> frame, xy shift=xy{}, uint8_t alpha=255)
        {
            for (auto& t: tokens)
            {
                t.render(frame, shift + offset, alpha);
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