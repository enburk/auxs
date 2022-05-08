#pragma once
#include "pix_text_row.h"
namespace pix::text
{
    struct line
    {
        array<token> tokens;
        array<row> rows;

        style_index style;
        int lpadding = 0;
        int rpadding = 0;
        int indent   = 0;

        format format;
        bool modified = true;
        int number = -1;

        generator<row*> ptrrows (pix::text::format f)
        {
            if (modified
            or  format != f) {
                format  = f;
                rows.clear();
                if (f.alignment.x == justify_left
                or  f.alignment.x == justify_right)
                format_dynamic(); else
                format_eager();
            }
            for (auto&x: rows)
            co_yield &x;
        }

        auto solids () -> generator<array<token>::range_type>
        {
            auto i = tokens.begin();
            auto j = tokens.begin();
            auto e = tokens.end();

            while (i != e)
            {
                // leading spaces yield by one,
                // one tailing space append to the word

                if    (j != e and j->text == " ") j++; else {
                while (j != e and j->text != " ") j++;
                if    (j != e and j->text == " ") j++;
                }
                    
                co_yield tokens(i, j);
                i = j;
            }
        };

        static auto skip (array<xy>& bars, int height)
        {
            while (not bars.empty()) {
                auto& bar = bars.front();
                bar.y -= height;
                if (bar.y >= 0)
                    break;
                height = -bar.y;
                bars.erase(0);
            }
        };

        void format_eager ()
        {
            auto f = format;
            rows += row{
            .style = style,
            .format = f,
            .lpadding = lpadding + indent,
            .rpadding = rpadding};

            int length = 0;

            for (auto solid: solids())
            while (not solid.empty())
            {
                rows.back().add(solid);
                if (solid.empty())
                    continue;

                int h = rows.back().Height();
                skip(f.lwrap, h);
                skip(f.rwrap, h);
                f.height -= h;

                length += rows.back().length;

                rows += row{
                .style = style,
                .format = f,
                .lpadding = lpadding,
                .rpadding = rpadding,
                .from = {0, length}};
            }

            auto m = pix::metrics(style.style().font);
            if (rows.back().solids.empty()) {
                rows.back().Ascent  = m.ascent;
                rows.back().Descent = m.descent; }
        }

        void format_dynamic ()
        {
            format_eager();
        }

        auto bars (int from, int upto, bool virtual_space)
        {
            array<xywh> bars;
            from = max(0, from);
            for (auto& row : rows)
            {
                for (xywh r: row.bars(
                from, upto, virtual_space))
                {
                    if (bars.size() > 0
                    and bars.back().x == r.x
                    and bars.back().w == r.w)
                        bars.back() |= r; else
                        bars += r;
                }
                from -= row.length;
                upto -= row.length;
            }
            return bars;
        }
    };
}
