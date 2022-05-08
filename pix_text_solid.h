#pragma once
#include "pix_text_token.h"
namespace pix::text
{
    struct solid : metrics
    {
        str text;
        array<token> tokens;
        int length = 0;
        xy offset;

        solid(array<token>::range_type range)
        {
            for (auto token: range)
            {
                text += token.text;
                tokens += token;
            }
            layout();
        }

        void layout ()
        {
            metrics::operator = (metrics{});
            for (auto& t: tokens)
            metrics::operator += (t);

            length = 0;
            advance = 0;
            for (auto& t: tokens)
            {
                t.offset.x = advance;
                t.offset.y = Ascent - t.Ascent;
                advance += t.advance;
                length += t.glyphs.size();
            }
        }

        void ellipt(int max_width)
        {
            while (not tokens.empty())
            {
                tokens.back().ellipt(max_width);
                layout(); if (rborder <= max_width and not
                tokens.back().glyphs.empty()) return;
                tokens.truncate();
                layout();
            }
        }

        void render (frame<rgba> frame, xy shift=xy{}, uint8_t alpha=255)
        {
            for (auto& t: tokens)
            {
                t.render(frame, shift + offset, alpha);
            }
        }

        array<xywh> bars (int from, int upto)
        {
            array<xywh> bars;
            from = max(0, from);
            for (auto& token : tokens)
            {
                xywh
                r = token.bar(from, upto);
                r += token.offset;
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
        
        int pointed (int x)
        {
            int i = 0;
            for (auto& token: tokens)
                if (token.offset.x +
                    token.Width() > x) return
                    i + token.pointed(
                    x - token.offset.x);
                else i += token.glyphs.size();
        
            return length-1;
        }
    };
} 