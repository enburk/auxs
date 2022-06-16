#pragma once
#include "doc_html_model.h"
#include "gui_widget_text_aux.h"
namespace gui::text
{
    struct box:
    widget<box>
    {
        struct text_type { box& x; text_type(box& x) : x(x) {}
        void operator =  (text_type const& tt) { *this = str(tt); }
        auto operator == (str text) { return str(*this) ==  text; }
        auto operator != (str text) { return str(*this) !=  text; }
        auto operator <=>(str text) { return str(*this) <=> text; }
        void operator  = (str text) { if (x.model.now->set_text(std::move(text))) x.on_change(this); }
        void operator += (str text) { if (x.model.now->add_text(std::move(text))) x.on_change(this); }
        operator str() const { return x.model.now->get_text(); } };

        struct html_type { box& x; html_type(box& x) : x(x) {}
        void operator =  (html_type const& tt) { *this = str(tt); }
        auto operator == (str text) { return str(*this) ==  text; }
        auto operator != (str text) { return str(*this) !=  text; }
        auto operator <=>(str text) { return str(*this) <=> text; }
        void operator  = (str html) { if (x.model.now->set_html(std::move(html))) x.on_change(this); }
        void operator += (str html) { if (x.model.now->add_html(std::move(html))) x.on_change(this); }
        operator str() const { return x.model.now->get_html(); } };

        text_type text{*this};
        html_type html{*this};

        property<rgba> color;
        binary_property<font> font;
        binary_property<style> style;
        binary_property<format> format;

        property<bool> update_colors = false;
        property<bool> update_layout = false;
        property<bool> update_text   = false;

        doc::html::model default_model;
        unary_property<doc::model*> model =
            &default_model;

        void on_change (void* what) override
        {
            if (what == &text
            or  what == &html)
            {
                update_text = true;
            }
            if (what == &skin)
            {
                style = pix::text::style{
                skins[skin.now].font,
                skins[skin.now].light
                    .second};
            }
            if (what == &font)
            {
                style.was = style.now;
                style.now.font = font.now;
                update_layout = true;
            }
            if (what == &color)
            {
                style.was = style.now;
                style.now.color = color.now;
                update_colors = true;
            }
            if (what == &style)
            {
                font.was = font.now;
                font.now = style.now.font;
                color.was = color.now;
                color.now = style.now.color;
                update_layout = true;
                update_colors = true;
            }
            if (what == &format)
            {
                update_layout = true;
            }

            if (what == &update_text   and update_text  .now
            or  what == &update_colors and update_colors.now
            or  what == &update_layout and update_layout.now)
            {
                update_text  .now = false;
                update_colors.now = false;
                update_layout.now = false;

                model.now->set(style.now);
                model.now->block.format = format.now;
                model.now->block.layout();

                if (model.now->block.size.x < 0
                or  model.now->block.size.y < 0)
                    throw std::out_of_range(
                    "bad text size");

                resize(model.now->block.size);
                notify(what);
                update();
            }
        }

        void on_render(sys::window& window,
        xywh r, xy offset, uint8_t alpha) override
        {
            for (auto& column: model.now->block.columns)
            for (auto& row: column.rows)
            {
                for (auto& solid: row->solids)
                for (auto& token: solid.tokens)
                for (auto& glyph : token.glyphs)
                {
                    xywh g {
                    glyph.offset.x,
                    glyph.offset.y,
                    glyph.Width(),
                    glyph.Height()
                    };
                    g += row->offset + solid.offset + token.offset;
                    xywh child_global = g + r.origin - offset;
                    xywh child_frame = r & child_global;
                    if (child_frame.size.x <= 0) continue;
                    if (child_frame.size.y <= 0) continue;
                    window.render(
                        child_frame, alpha, glyph,
                        child_frame.origin -
                        child_global.origin,
                        glyph.offset.x);
                }
            }
        }
    };
}
