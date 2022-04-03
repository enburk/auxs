#pragma once
#include "doc_html_model.h"
#include "gui_widget_text_cell.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    struct view:
    widget<view>
    {
        canvas canvas; cell cell;
        frame current_line_frame;
        frame frame;

        struct text_type { view& v; text_type(view& v) : v(v) {}
        auto operator == (str text) { return str(*this) ==  text; }
        auto operator != (str text) { return str(*this) !=  text; }
        auto operator <=>(str text) { return str(*this) <=> text; }
        void operator  = (str text) { v.model->set_text(std::move(text)); v.on_change(this); }
        void operator += (str text) { v.model->add_text(std::move(text)); v.on_change(this); }
        operator str() const { return v.model->get_text(); } };

        struct html_type { view& v; html_type(view& v) : v(v) {}
        auto operator == (str text) { return str(*this) ==  text; }
        auto operator != (str text) { return str(*this) !=  text; }
        auto operator <=>(str text) { return str(*this) <=> text; }
        void operator  = (str html) { v.model->set_html(std::move(html)); v.on_change(this); }
        void operator += (str html) { v.model->add_html(std::move(html)); v.on_change(this); }
        operator str() const { return v.model->get_html(); } };

        text_type text{*this};
        html_type html{*this};

        property<rgba> color;
        binary_property<font> font;
        binary_property<style> style;
        binary_property<int> lpadding;
        binary_property<int> rpadding;
        binary_property<array<xy>> lwrap;
        binary_property<array<xy>> rwrap;
        binary_property<xy> alignment = xy{pix::center, pix::center};
        binary_property<xy> shift;
        binary_property<bool> wordwrap = true;
        binary_property<bool> ellipsis = false;

        property<bool> update_colors = false;
        property<bool> update_layout = false;
        property<bool> update_text   = false;

        doc::html::model model_;
        doc::model* model = &model_;

        unary_property<array<range>>& highlights = cell.highlights;
        unary_property<array<range>>& selections = cell.selections;
        binary_property<bool>& virtual_space = cell.virtual_space;
        binary_property<bool>& insert_mode = cell.insert_mode;

        void on_change (void* what) override
        {
            const int l = 0; // lazy or eager ?

            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                xywh r = coord.now.local();
                canvas.coord = r;
                frame .coord = r;
                update_layout.go(true, time(l));
            }
            if (what == &text
            or  what == &html)
            {
                update_text.go(true, time(l));
            }
            if (what == &skin)
            {
                style = pix::text::style{
                    skins[skin.now].font,
                    skins[skin.now].light.second};
            }
            if (what == &font)
            {
                style.was = style.now;
                style.now.font = font.now;
                update_layout.go(true, time(l));
            }
            if (what == &color)
            {
                style.was = style.now;
                style.now.color = color.now;
                update_colors.go(true, time(l));
            }
            if (what == &style)
            {
                font.was = font.now;
                font.now = style.now.font;
                color.was = color.now;
                color.now = style.now.color;
                update_layout.go(true, time(l));
                update_colors.go(true, time(l));
            }
            if (what == &alignment
            or  what == &wordwrap
            or  what == &ellipsis
            or  what == &lpadding
            or  what == &rpadding
            or  what == &lwrap
            or  what == &rwrap)
            {
                update_layout.go(true, time(l));
            }

            if (what == &update_text   and update_text  .now
            or  what == &update_colors and update_colors.now
            or  what == &update_layout and update_layout.now)
            {
                update_text  .now = false;
                update_colors.now = false;
                update_layout.now = false;

                format format;
                format.lwrap = lwrap.now;
                format.rwrap = rwrap.now;
                format.alignment = alignment.now;
                format.lpadding = lpadding.now;
                format.rpadding = rpadding.now;
                format.wordwrap = wordwrap.now;
                format.ellipsis = ellipsis.now;
                format.width  = coord.now.size.x; if (ellipsis.now)
                format.height = coord.now.size.y;

                model->set(style.now, format);
                cell.fill(model->view_lines);
            }

            if (what == &update_text
            or  what == &update_layout
            or  what == &shift)
            {
                int H = coord.now.size.y;
                int h = cell.coord.now.size.y;

                cell.move_to(xy(0, H > h and
                    alignment.now.y == pix::center ? H/2 - h/2 :
                    alignment.now.y == pix::bottom ? H   - h   :
                    0) + shift.now);
            }
            if (what == &update_text)
            {
                highlights = array<range>{};
                selections = model->selections;
            }

            if (what == &selections
            or  what == &focus_on)
            {
                model->selections = selections.now;
                if (selections.now.size() == 1
                    and focus_on.now) {
                    xywh r = cell.carets(0).coord.now;
                    r.x = 0; r.w = coord.now.w;
                    current_line_frame.coord = r;
                    current_line_frame.show(); } else
                    current_line_frame.hide();
            }
            if (what == &focus_on)
            {
                cell.focus_on = focus_on.now;
            }

            notify(what);
        }

        auto selected () { return cell.selected(); }

        place point (xy p) { return cell.point(p - shift.now); }

        auto rows() { return cell.rows(); }
        auto row(int n) { return cell.row(n); }
        place lines2rows(place p) { return cell.lines2rows(p); }
        place rows2lines(place p) { return cell.rows2lines(p); }
    };
}
