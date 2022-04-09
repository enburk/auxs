#pragma once
#include "gui_widget_text_aux_token.h"
namespace gui::text
{
    using token_range = widgetarium<token>::range_type;

    struct solid : metrics
    {
        xy offset;
        int length = 0;
        token_range tokens;
        solid(token_range tokens) : tokens(tokens)
        {
            for (auto& token: tokens)
            {
                Ascent  = max(Ascent,  token.Ascent);
                ascent  = max(ascent,  token.ascent);
                Descent = max(Descent, token.Descent);
                descent = max(descent, token.descent);
                bearing = min(bearing, advance + token.bearing);
                advance += token.bearing + token.advance;
            }

            advance = 0; int width = 0;

            for (auto& token: tokens)
            {
                int y = Ascent - token.Ascent;
                int x = advance + token.bearing - bearing;
                advance += token.bearing + token.advance;
                length += token.size();
                token.offset = xy(x, y);

                // do not increase width if
                // there is space-like symbol at the very end
                // and it's not the sole symbol in the row
                if (token.rpadding != token.advance or
                    tokens.size() == 1)
                    width = max(width,
                      x + token.Width());
            }

            rpadding = advance - width;

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
            while (advance + token.Width() > max_width
                and t.text != (char*)(u8"…"));

            advance += token.Width();
        }

        void ellipt(int max_width)
        {
            while (tokens.size() > 0)
            {
                auto& token = tokens.back();
                ellipt(max_width, token);
                if (width() <= max_width)
                    return;

                tokens.truncate();
                solid s {tokens};
                metrics::operator=(s);
            }
        }

        xywh bar (int place)
        {
            if (place < 0 or
                place >= length)
                return xywh{};

            for (auto& token : tokens) {
                xywh r = token.bar(place, place+1);
                if (r != xywh{}) return r + token.offset;
                place -= token.size(); }

            return xywh{};
        }

        array<xywh> bars (int from, int upto)
        {
            array<xywh> bars;
            from = max(0, from);
            for (auto& token : tokens)
            {
                xywh r = token.bar(from, upto) + token.offset;
                from -= token.size();
                upto -= token.size();
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
                    token.Width() > x) return
                    i + token.point(
                    x - token.offset.x);
                else i += token.size();

            return length-1;
        }
    };
} 