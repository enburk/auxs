#pragma once
#include "gui_widgetarium.h"
#include "gui_widget_text_aux.h"
namespace gui::text
{
    using token_range = widgetarium<token>::range_type;

    struct solid : metrics
    {
        XY offset;
        token_range tokens;
        solid(token_range tokens) : tokens(tokens)
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
                token.offset = XY(x, y);

                // if not space-like at the very end
                if (token.rpadding != token.advance or
                    tokens.size() == 1) // single space is ok
                    width = max(width,
                        x + token.width);
            }

            bearing = 0;
        }

        void ellipt(int max_width, token& token)
        {
            advance -= token.advance;

            doc::view::token t = token;

            str text = t.text;

            do
            {
                text.trimr();
                t.text = text + (char*)(u8"…");
                text.truncate();
                token = t;
            }
            while (advance + token.width > max_width
                and t.text != (char*)(u8"…"));

            width = advance + token.width;

            advance += token.advance;
        }

        void ellipt(int max_width)
        {
            while (tokens.size() > 0)
            {
                auto& token = tokens.back();
                ellipt(max_width, token);
                if (width <= max_width)
                    return;

                tokens.truncate();
                solid s {tokens};
                metrics::operator=(s);
            }
        }
    };

    struct row : metrics
    {
        XY offset;
        array<solid> solids;
        doc::view::format format;
        bool the_last_row = true;
        int length = 0;
        int start = -1;

        int lpadding (int height)
        {
            int d = 0;
            for (auto bar: format.lwrap) {
                d = aux::max(d, bar.x);
                height -= bar.y;
                if (height <= 0)
                    break; }
            return d + format.lpadding;
        }
        int rpadding (int height)
        {
            int d = 0;
            for (auto bar: format.rwrap) {
                d = aux::max(d, bar.x);
                height -= bar.y;
                if (height <= 0)
                    break; }
            return d + format.rpadding;
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
    };

    ///////////////////////////////////////////////////////////////////////
            
    struct line final : widgetarium<token>, doc::view::line
    {
        array<row> rows;
        int length = 0;
        int start = -1;

        void operator = (doc::view::line data)
        {
            doc::view::line::operator=(std::move(data));
            reserve(tokens.size());
            rows.clear();

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

            int width = 0; length = 0; start = -1;

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

                    if (token.text != " ")
                    {
                        if (start == -1)
                            start = length;

                        if (row.start == -1)
                            row.start = row.length;
                    }

                    row.
                    length += token.size();
                    length += token.size();
                }

                if (start == -1)
                    start = 0;

                if (row.start == -1)
                    row.start = 0;
            }

            resize(XY(width, height));
        }
    };
} 