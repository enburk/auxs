#pragma once
#include "gui_widget_text_aux_solid.h"
namespace gui::text
{
    struct row : metrics
    {
        xy offset;
        xy column_offset;
        array<solid> solids;
        doc::view::format format;
        bool the_last_row = true;
        int length = 0;
        int indent = -1;

        int lpadding (int height)
        {
            int d = 0;
            for (auto bar: format.lwrap) {
                d = aux::max(d, bar.x);
                height -= bar.y;
                if (height <= 0)
                    break;
            }
            return d +
            format.lpadding;
        }
        int rpadding (int height)
        {
            int d = 0;
            for (auto bar: format.rwrap) {
                d = aux::max(d, bar.x);
                height -= bar.y;
                if (height <= 0)
                    break;
            }
            return d +
            format.rpadding;
        }

        bool add (solid solid)
        {
            int height =
                max(ascent,  solid.ascent) +
                max(descent, solid.descent);

            int max_width =
                format.width -
                lpadding(height) -
                rpadding(height);

            if ((format.wordwrap or format.ellipsis)
            and advance + solid.width > max_width
            and not solids.empty()) // at least one should be accepted
            {
                the_last_row = false;
                return false;
            }

            if (format.ellipsis
            and advance + solid.width > max_width
            and solids.empty())
                solid.ellipt(max_width);

            ascent  = max(ascent,   solid.ascent);
            ascent_ = max(ascent_,  solid.ascent_);
            descent = max(descent,  solid.descent);
            descent_= max(descent_, solid.descent_);
            width = advance + solid.width;
            solid.offset.x = advance;
            advance += solid.advance;
            solids += std::move(solid);
            return true;
        }

        void ellipt()
        {
            int max_width =
                format.width -
                lpadding(ascent+descent) -
                rpadding(ascent+descent);

            the_last_row = true;

            while (solids.size() >= 2)
            {
                auto& solid = solids.back();
                advance -= solid.advance;
                solid.ellipt(max<int>());
                width = advance + solid.width;
                advance += solid.advance;
                if (width <= max_width)
                    return;

                advance -= solid.advance;
                solid.tokens.clear();
                solids.truncate();
                width = advance + solids.back().width;
            }

            if (solids.size() > 0) {
                auto& solid = solids.back();
                advance -= solid.advance;
                solid.ellipt(max_width);
                width = advance + solid.width;
                advance += solid.advance;
            }
        }

        void align ()
        {
            for (auto& solid: solids)
                solid.offset.y = ascent -
                solid.ascent;

            int align = format.alignment.x;
            int Width =
                format.width -
                lpadding(ascent+descent) -
                rpadding(ascent+descent);

            if (align == pix::left or
               (align == pix::justify_left and the_last_row)) {
                return;
            }

            if (align == pix::center and Width > width) {
                offset.x += Width/2 - width/2;
                return;
            }

            if (align == pix::right or
               (align == pix::justify_right and the_last_row)) {
                offset.x += Width - width;
                return;
            }

            int n = solids.size();
            if (n <= 0) return;
            if (n <= 1) return; //  t o k e n

            int d = (Width - width) / (n-1);
            int e = (Width - width) % (n-1);

            for (int i=0; i<n; i++)
                solids[i].offset.x +=
                d*i + (i >= n-e ? 1 : 0);

            width = Width;
        }

        xywh bar (int place)
        {
            if (place < 0 or
                place >= length)
                return xywh{};

            for (auto& solid : solids) {
                xywh r = solid.bar(place);
                if (r != xywh{}) return r + solid.offset;
                place -= solid.length; }

            return xywh{};
        }

        array<xywh> bars (int from, int upto)
        {
            array<xywh> bars;
            from = max(0, from);
            for (auto& solid : solids)
            {
                array<xywh> rr = solid.bars(from, upto);
                for (auto& r: rr) r += solid.offset;
                from -= solid.length;
                upto -= solid.length;
                if (rr.empty()) continue;
                if (bars.size() > 0 and rr.size() == 1
                and bars.back().y == rr.back().y
                and bars.back().h == rr.back().h)
                    bars.back() |= rr.back(); else
                    bars += rr;
            }
            return bars;
        }

        int point (int x)
        {
            int i = 0;
            for (auto& solid: solids)
                if (solid.offset.x +
                    solid.width > x) return
                    i + solid.point(
                    x - solid.offset.x);
                else i += solid.length;

            return length-1;
        }
    };
} 