#pragma once
#include "pix_text_token.h"
namespace pix::text
{
    using token_range = array<token>::range_type;

    struct solid : metrics
    {
        token_range tokens;
        glyph ellipsis;
        int length = 0;
        xy offset;

        solid(token_range tokens, int max_width=max<int>()) : tokens(tokens)
        {
            for (auto& token: tokens)
            {
                ascent   = max(ascent,   token.ascent);
                ascent_  = max(ascent_,  token.ascent_);
                descent  = max(descent,  token.descent);
                descent_ = max(descent_, token.descent_);
                bearing  = min(bearing,  advance + token.bearing);
                advance += token.bearing + token.advance;
            }

            advance = 0;

            for (auto& token: tokens)
            {
                int y = ascent - token.ascent;
                int x = advance + token.bearing - bearing;
                advance += token.bearing + token.advance;
                length += token.glyphs.size();
                token.offset = xy(x, y);

                // do not increase width if
                // there is space-like symbol at the very end
                // and it's not the sole symbol in the row
                if (token.rpadding != token.advance or
                    tokens.size() == 1)
                    width = max(width,
                      x + token.width);
            }

            bearing = 0;
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

        xywh bar (int place)
        {
            if (place < 0 or
                place >= length)
                return xywh{};

            for (auto& token : tokens) {
                xywh r = token.bar(place, place+1);
                if (r != xywh{}) return r + token.offset;
                place -= token.glyphs.size(); }

            return xywh{};
        }

        array<xywh> bars (int from, int upto)
        {
            array<xywh> bars;
            from = max(0, from);
            for (auto& token : tokens)
            {
                xywh r = token.bar(from, upto) + token.offset;
                from -= token.glyphs.size();
                upto -= token.glyphs.size();
                if (r.w == 0 or
                    r.h == 0)
                    continue;
                if (bars.size() > 0
                and bars.back().y == r.y
                and bars.back().h == r.h)
                    bars.back() |= r; else
                    bars += r;
            }
            return bars;
        }

        int point (int x)
        {
            int i = 0;
            for (auto& token: tokens)
                if (token.offset.x +
                    token.width > x) return
                    i + token.point(
                    x - token.offset.x);
                else i += token.glyphs.size();

            return length-1;
        }
    };
} 