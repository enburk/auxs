#pragma once
#include "pix_text_solid.h"
namespace pix::text
{
    struct row: metrics
    {
        array<solid> solids;
        style_index style;
        format format;
        int lpadding = 0;
        int rpadding = 0;
        xy offset;

        place from;
        int length = 0;
        int indent = 0;
        bool last = true;
        bool ellipsis = false;

        row (style_index style = {}) : style(style)
        {
            layout();
        }

        int lwrap (int height)
        {
            int x = 0;
            for (auto bar: format.lwrap) {
                x = aux::max(x, bar.x);
                height -= bar.y;
                if (height <= 0)
                    break;
            }
            return x;
        }
        int rwrap (int height)
        {
            int x = 0;
            for (auto bar: format.rwrap) {
                x = aux::max(x, bar.x);
                height -= bar.y;
                if (height <= 0)
                    break;
            }
            return x;
        }
        int max_width (int height)
        {
            return format.width
            - lwrap(height) - lpadding
            - rwrap(height) - rpadding;
        }

        void add (array<token>::range_type& tokens)
        {
            solid s(tokens);
            metrics m = *this; m += s;
            int w = max_width(m.Height());

            if (m.rborder > w
            and format.wordwrap
            and not solids.empty()) {
                last = false;
                return; }

            tokens >>= tokens.size();
            metrics::operator = (m);
            length += s.length;
            solids += s;

            if (m.rborder > w
            and format.ellipsis
            and solids.size() == 1)
                ellipt();
        }

        void layout ()
        {
            metrics::operator = (metrics{});
            auto m = pix::metrics(style.style().font);
            Ascent  = m.ascent;
            Descent = m.descent;

            for (auto& s: solids)
            metrics::operator += (s);

            advance = 0;
            for (auto& s: solids)
            {
                s.offset.x = advance;
                s.offset.y = Ascent - s.Ascent;
                advance += s.advance;
            }
        }

        void ellipt()
        {
            if (
            ellipsis) return;
            ellipsis = true;

            last = true;

            int W = max_width(Height());

            while (solids.size() >= 2)
            {
                solids.back().ellipt(max<int>());
                layout(); if (rborder <= W) return;
                solids.truncate();
            }

            if (solids.size() > 0) {
                solids.back().ellipt(W);
                layout();
            }
        }

        void align ()
        {
            if (rborder > max_width(Height())
            and format.ellipsis and
            not format.wordwrap)
                ellipt();

            length = 0;
            indent = -1;
            for (auto& solid: solids)
            for (auto& token: solid.tokens)
            for (auto& glyph: token.glyphs)
            {
                if (glyph.text != " "
                and indent == -1)
                    indent =
                    length;

                length++;
            }
            if (indent == -1)
                indent =
                length;

            layout();

            int H = Height();
            int h = height();
            if (format.alignment.y == center_of_area
            or  format.alignment.y == center_of_mass)
            {
               for (auto& s: solids)
               s.offset.y -= (H-h)/2;
            }

            int align = format.alignment.x;
            int W = max_width(Height());
            int w = rborder - lborder;
            if (W <= w
            or  align == left
            or  align <= justify_left and last)
            {
                W = min(advance, rborder);
            }
            else
            if (align == center
            or  align == center_of_area
            or  align == center_of_mass)
            {
                for (auto& s: solids)
                s.offset.x += (W-w)/2;
            }
            else
            if (align == right
            or  align >= justify_right and last)
            {
                for (auto& s: solids)
                s.offset.x += W - w;
            }
            else // justify
            if (int n = solids.size(); n >= 2)
            {
                int d = (W - w) / (n-1);
                int e = (W - w) % (n-1);

                for (int i=0; i<n; i++)
                solids[i].offset.x += d*i;

                for (int i=0; i<e; i++)
                solids[n-e+i].offset.x += i;
            }

            advance = max(advance, W);
            rborder = max(rborder, W);

            int x = 
            lpadding + lwrap(Height());
            for (auto& s: solids)
            s.offset.x += x;
            advance += x;
            lborder += x;
            rborder += x;
        }

        void render (frame<rgba> frame, xy shift=xy{}, uint8_t alpha=255)
        {
            for (auto& s: solids)
            {
                s.render(frame, shift + offset, alpha);
            }
        }

        array<xywh> bars (int from, int upto, bool virtual_space)
        {
            if (from <= 0
            and upto == max<int>()
            and last and virtual_space)
            return array<xywh>{xywh(0,0,
                max<int>()/2, Height()) +
                offset};

            array<xywh> bars;
            from = max(0, from);
            if (from == upto)
                return bars;

            for (auto& solid : solids)
            {
                for (xywh r: solid.bars(from, upto))
                {
                    r += solid.offset;
                    if (bars.size() > 0
                    and bars.back().y == r.y
                    and bars.back().h == r.h)
                        bars.back() |= r; else
                        bars += r;
                }
                from -= solid.length;
                upto -= solid.length;
            }
            if (upto > 0 and last and virtual_space)
            {
                if (from < 0) from = 0;
                int n = min(upto - from, 10000);
                glyph space(" ", style);
                bars += xywh(advance +
                    space.advance * from, 0,
                    space.advance * n,
                    space.Height());
            }
            for (auto& bar: bars)
            bar += offset;
            return bars;
        }

        place pointed (int x, bool virtual_space)
        {
            int i = 0;
            for (auto& solid: solids)
                if (solid.offset.x +
                    solid.Width() > x)
                    return {
                    from.line,
                    from.offset +
                    i + solid.pointed(
                    x - solid.offset.x)};
                else i += solid.length;

            if (last and virtual_space)
            {
                pix::glyph space(" ", style);
                int dx = x - advance;
                int nx = dx/space.advance;
                i += nx;
            }

            return {from.line, from.offset + i};
        }
    };
}
