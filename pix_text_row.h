#pragma once
#include "pix_text_solid.h"
namespace pix::text
{
    struct row: metrics
    {
        array<solid> solids;
        format format;
        int lpadding = 0;
        int rpadding = 0;
        xy offset;

        bool last = true;
        int length = 0;
        int indent = 0;

        int max_width (int height)
        {
            int l = 0; int h = height;
            for (auto bar: format.lwrap) {
                l = aux::max(l, bar.x);
                h -= bar.y;
                if (h <= 0)
                    break;
            }
            int r = 0; h = height;
            for (auto bar: format.rwrap) {
                r = aux::max(r, bar.x);
                h -= bar.y;
                if (h <= 0)
                    break;
            }
            return format.width
                - l - lpadding
                - r - rpadding;
        }

        void add (token_range& tokens)
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
            solids += s;

            if (m.rborder > w
            and format.ellipsis
            and solids.size() == 1)
                ellipt();
        }

        void layout ()
        {
            metrics::operator = (metrics{});
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
            length = 0;
            indent = -1;
            for (auto& solid: solids)
            for (auto& glyph: solid.glyphs)
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

            int x = 0;
            int h = Height();
            for (auto bar: format.lwrap) {
                x = aux::max(x, bar.x);
                h -= bar.y;
                if (h <= 0)
                    break;
            }
            offset.x = x + lpadding;

            int align = format.alignment.x;
            int W = max_width(Height());
            int w = width();

            if (align == left or
               (align <= justify_left and last))
                return;

            if (align == center and W > w) {
                offset.x += W/2 - w/2;
                return; }

            if (align == right or
               (align >= justify_right and last)) {
                offset.x += W - w;
                return; }

            int n = solids.size();
            if (n <= 0) return;
            if (n <= 1) return; //  t o k e n

            int d = (W - w) / (n-1);
            int e = (W - w) % (n-1);

            for (int i=0; i<n; i++)
                solids[i].offset.x +=
                d*i + (i >= n-e ? 1 : 0);

            advance = W;
            rborder = W;
        }

        void render (frame<rgba> frame, xy shift=xy{}, uint8_t alpha=255)
        {
            for (auto& s: solids)
            {
                s.render(frame, shift + offset, alpha);
            }
        }
    };
}
