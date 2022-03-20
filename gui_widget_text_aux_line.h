#pragma once
#include "gui_widget_text_aux_row.h"
namespace gui::text
{
    struct line final : widgetarium<token>, doc::view::line
    {
        array<row> rows;
        bool last = false;
        int length = 0;

        void operator = (doc::view::line data)
        {
            doc::view::line::operator=(std::move(data));
            reserve(tokens.size());
            rows.clear();
            last = false;

            if (format.height <= 0) { resize(XY()); return; }
            if (format.width  <= 0) { resize(XY()); return; }
            if (format.width  == max<int>() and
                format.alignment.x != left) throw
                std::out_of_range("text::line: "
                    "impossible format");

            auto solids = [this]() -> generator<array<doc::view::token>::range_type>
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

            auto skip = [](array<XY>& bars, int height)
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

            int total = 0; int accepted = 0; int height = 0;

            auto fmt = format; // current row format 

            for (auto solid: solids())
            {
                for (auto token : solid)
                    at(total++) = token;

                auto emplaced = from(accepted).upto(total);

                if (rows.empty() or
                not rows.back().add(emplaced)) // then new row needed
                {
                    if (not rows.empty()) // then finishing last row
                    {
                        int h =                         
                            rows.back().ascent +
                            rows.back().descent;

                        if (height + h >= fmt.height
                            or not format.wordwrap)
                            break; // emplaced tokens wouldn't be accepted

                        height += h;
                        fmt.height -= h;
                        skip(fmt.lwrap, h);
                        skip(fmt.rwrap, h);
                    }

                    rows += row{};
                    rows.back().format = fmt;
                    rows.back().offset.y = height;
                    rows.back().add(emplaced);
                    // at least one solid would be accepted
                    // and no row would be empty
                }

                accepted = total;
            }

            truncate(accepted);

            if (accepted < tokens.size()
                and format.ellipsis and
                rows.size() != 0)
                rows.back().ellipt();

            if (rows.size() != 0) height +=
                rows.back().ascent +
                rows.back().descent;

            int width = 0; length = 0;

            for (auto& row: rows)
            {
                row.align();
                row.offset.x += row.lpadding(
                    row.ascent +
                    row.descent);
                width = max (width,
                    row.offset.x +
                    row.width);

                for (auto& solid: row.solids)
                for (auto& token: solid.tokens)
                {
                    token.move_to(XY(
                    row.offset.x + solid.offset.x + token.offset.x,
                    row.offset.y + solid.offset.y + token.offset.y));

                    if (token.text.contains(one_not_of(" "))
                    and row.indent == -1)
                        row.indent =
                        row.length;

                    row.
                    length += token.size();
                    length += token.size();
                }
                if (row.indent == -1)
                    row.indent =
                    row.length;
            }

            if (rows.empty()) {
                pix::glyph space (" ", style);
                height = space.ascent +
                    space.descent; }

            resize(XY(width, height));
        }

        XYWH bar (int place)
        {
            if (place < 0 or
                place >= length)
                return XYWH{};

            for (auto& row : rows) {
                XYWH r = row.bar(place);
                if (r != XYWH{}) return r + row.offset;
                place -= row.length; }

            return XYWH{};
        }

        array<XYWH> bars (int from, int upto, bool virtual_space)
        {
            array<XYWH> bars;
            from = max(0, from);
            for (auto& row : rows)
            {
                array<XYWH> rr = row.bars(from, upto);
                for (auto& r: rr) r += row.offset;
                from -= row.length;
                upto -= row.length;
                if (rr.empty()) continue;
                if (bars.size() > 0 and rr.size() == 1
                and bars.back().x == rr.back().x
                and bars.back().w == rr.back().w)
                    bars.back() |= rr.back(); else
                    bars += rr;
            }
            if (upto > 0 and virtual_space)
            {
                if (from < 0) from = 0;
                // length was subtracted
                // from 'from' and 'upto'
                
                sys::glyph space(" ", style);

                XY  offset;
                if (rows.size() > 0) offset = 
                    rows.back().offset + XY(
                    rows.back().width +
                    from*space.advance,
                        0);

                int n = min(upto - from, 10000);

                bars += XYWH (0, 0,
                    space.advance * n,
                    space.ascent +
                    space.descent) +
                    offset;
            }
            return bars;
        }

        int point (XY p, bool virtual_space)
        {
            int i = 0;
            for (auto& row : rows)
            {
                if (row.ascent +
                    row.descent +
                    row.offset.y > p.y or
                    row.the_last_row)
                {
                    int x = p.x - row.offset.x;
                    if (x < row.width
                    or not virtual_space
                    or not row.the_last_row)
                    return i + row.point(x);
                    else break; // anyway:
                    // it's the last row
                    // and virtual space
                }
                else i += row.length;
            }

            if (virtual_space and not rows.empty())
            {
                sys::glyph space(" ", style);
                int dx = p.x - rows.back().width;
                return length + dx/space.advance;
            }

            return length-1;
        }
    };
} 