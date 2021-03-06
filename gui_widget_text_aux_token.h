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
            xy offset;
            xywh coord() const {
                return xywh(
                    offset.x,
                    offset.y,
                    Width (),
                    Height()
                );
            }
        };
        array<glyph> glyphs;

        xy offset; // for external formatting

        void operator = (doc::view::token const& t)
        {
            bool same = text == t.text and style == t.style;

            doc::view::token::operator = (t); if (same) return;

            pix::text::metrics::operator = (pix::text::metrics{});

            glyphs.clear();

            for (auto c: aux::unicode::glyphs(text))
            {
                auto g = pix::glyph(c, style);
                Ascent  = max(Ascent,  g.Ascent);
                ascent  = max(ascent,  g.ascent);
                Descent = max(Descent, g.Descent);
                descent = max(descent, g.descent);
                glyphs += glyph{ g, xy{} };
            }
            if (glyphs.size() > 0)
            {
                lborder = glyphs.front().lborder;
                rborder = glyphs.back ().rborder;
            }
            for (auto& g: glyphs)
            {
                g.offset.x = advance;
                g.offset.y = Ascent - g.Ascent;
                advance += g.advance;
            }

            xoffset = style.style().offset.x;

            resize(xy(Width(), Height()));

            update();
        }

        int size() const { return glyphs.size(); }

        void on_render(sys::window& window,
        xywh r, xy offset, uint8_t alpha) override
        {
            for (auto& g : glyphs) {
                xywh child_global = g.coord() + r.origin - offset;
                xywh child_frame = r & child_global;
                if (child_frame.size.x <= 0) continue;
                if (child_frame.size.y <= 0) continue;
                window.render(
                    child_frame, alpha, g,
                    child_frame.origin -
                    child_global.origin,
                    g.offset.x);
            }
        }

        xywh bar (int from, int upto)
        {
            from = max(from, 0);
            upto = min(upto, glyphs.size());
            if (from >= upto) return xywh{}; upto--;
            xyxy r1 = glyphs[from].coord();
            xyxy r2 = glyphs[upto].coord();
            return xyxy(
                r1.x1, r1.y1,
                r2.x2, r2.y2);
        }

        int point (int x)
        {
            int i = 0;
            for (auto g : glyphs)
                if (g.offset.x +
                    g.Width() > x)
                    return i;
                    else i++;

            return size() - 1;
        }
    };
} 