#pragma once
#include "doc.h"
#include "gui_widgetarium.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    using pix::text::style;
    using pix::text::style_index;
    using pix::text::metrics;
    using doc::view::format;
    using doc::place;
    using doc::range;

    struct glyph:
    widget<glyph>
    {
        unary_property<pix::glyph> value;

        Opacity opacity () override { return semitransparent; }

        void on_change (void* what) override
        {
             if (what == &value) {
                 resize(XY(
                 value.now.advance,
                 value.now.ascent +
                 value.now.descent));
                 update();
             }
        }
        void on_render (sys::window& window, XYWH r, XY offset, uint8_t alpha) override {
             window.render(r, alpha, value.now, offset, coord.now.x);
        }
    };

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
    };

    struct caret:
    widget<caret>
    {
        canvas image;
        property<time> timer;
        property<double> breadth = 0.15;
        binary_property<bool> insert_mode = true;
        static inline time time_moved;

        void on_change (void* what) override
        {
            if (timer.now == time())
                timer.go (time::infinity,
                          time::infinity);

            if (what == &timer)
            {
                if (time::now - time_moved < 200ms) {
                    image.alpha = 255;
                    return;
                }
                const int MS = 1024;
                int ms = time::now.ms % MS;
                if (ms > MS/2) ms = MS-ms;
                auto a = clamp<uint8_t>
                (256 * ms/(MS/2) + 00 - 1);
                if (true) a = a > 127 ? 255 : 0;
                image.alpha = a;
                return;
            }
            if (what == &skin
            or  what == &insert_mode)
            {
                image.color = insert_mode.now ?
                skins[skin].dark.first:
                skins[skin].heavy.first;
            }
            if (what == &coord
            or  what == &insert_mode
            or  what == &breadth)
            {
                time_moved = time::now;
                XYWH r = coord.now.local();
                if (insert_mode.now) r.w = max (1, (int)(r.w*breadth.now));
                image.coord = r;
            }
        }
    };
} 