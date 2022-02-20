#pragma once
#include "doc_view.h"
#include "gui_widget_text_aux.h"
namespace gui::text
{
    struct token final:
    widget<token>, doc::view::token, metrics
    {
        struct glyph: pix::glyph
        {
            XY offset;
            XYWH coord() const {
                return XYWH(
                    offset.x,
                    offset.y,
                    width,
                    ascent+
                    descent
                );
            }
        };
        array<glyph> glyphs;

        XY offset; // for external formatting

        void operator = (doc::view::token const& t)
        {
            bool same = text == t.text and style == t.style;

            doc::view::token::operator = (t); if (same) return;

            pix::text::metrics::operator = (pix::text::metrics{});

            glyphs.clear();

            for (str text: aux::unicode::glyphs(text))
            {
                auto g = pix::glyph(text, style);
                ascent  = max(ascent,   g.ascent);
                ascent_ = max(ascent_,  g.ascent_);
                descent = max(descent,  g.descent);
                descent_= max(descent_, g.descent_);
                glyphs += glyph{ g, XY{} };
            }

            if (glyphs.size() > 0) {
                lpadding = glyphs.front().lpadding;
                rpadding = glyphs.back ().rpadding;
            }

            for (auto& g: glyphs)
            {
                g.offset.x = advance;
                g.offset.y = ascent - g.ascent;
                width = advance + g.width;
                advance += g.advance;
            }

            bearing = style.style().shift.x;

            resize(XY(width, ascent + descent));
        }

        int size() const { return glyphs.size(); }

        void on_render(sys::window& window, XYWH r, XY offset, uint8_t alpha) override
        {
            for (auto& g : glyphs) {
                XYWH child_global = g.coord() + r.origin - offset;
                XYWH child_frame = r & child_global;
                if (child_frame.size.x <= 0) continue;
                if (child_frame.size.y <= 0) continue;
                window.render(
                    child_frame, alpha, g,
                    child_frame.origin -
                    child_global.origin,
                    g.offset.x);
            }
        }

        XYWH bar (int from, int upto)
        {
            from = max(from, 0);
            upto = min(upto, glyphs.size());
            if (from >= upto) return XYWH{}; upto--;
            XYXY r1 = glyphs[from].coord();
            XYXY r2 = glyphs[upto].coord();
            return XYXY(
                r1.x1, r1.y1,
                r2.x2, r2.y2);
        }

        int point (int x)
        {
            int i = 0;
            for (auto g : glyphs)
                if (g.offset.x +
                    g.width > x)
                    return i;
                    else i++;

            return size() - 1;
        }
    };
} 