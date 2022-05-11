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

        binary_property<int> lpadding;
        binary_property<int> rpadding;
        binary_property<array<xy>> lwrap;
        binary_property<array<xy>> rwrap;
        binary_property<xy> alignment = xy{pix::center, pix::center};
        binary_property<xy> shift;
        binary_property<bool> wordwrap = true;
        binary_property<bool> ellipsis = false;

        box::text_type& text = cell.text;
        box::html_type& html = cell.html;
        property<rgba>& color = cell.color;
        binary_property<font>& font = cell.font;
        binary_property<style>& style = cell.style;
        property<bool>& update_text = cell.update_text;
        property<bool>& update_colors = cell.update_colors;
        property<bool>& update_layout = cell.update_layout;
        unary_property<array<range>>& highlights = cell.highlights;
        unary_property<array<range>>& selections = cell.selections;
        binary_property<bool>& virtual_space = cell.virtual_space;
        binary_property<bool>& insert_mode = cell.insert_mode;

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                xywh r = coord.now.local();
                canvas.coord = r;
                frame .coord = r;
            }
            if (what == &alignment
            or  what == &wordwrap
            or  what == &ellipsis
            or  what == &lpadding
            or  what == &rpadding
            or  what == &lwrap
            or  what == &rwrap
            or  what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                pix::text::format f;
                f.lwrap = lwrap.now;
                f.rwrap = rwrap.now;
                f.alignment = alignment.now;
                f.wordwrap = wordwrap.now;
                f.ellipsis = ellipsis.now;
                f.width  = coord.now.size.x; if (ellipsis.now)
                f.height = coord.now.size.y; else
                f.alignment.y = pix::top;
                cell.format = f;
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

            if (what == &selections
            or  what == &focus_on)
            {
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

        place pointed (xy p) { return cell.pointed(p - shift.now); }

        auto rows() { return cell.rows(); }
        auto row(int n) { return cell.row(n); }
        place lines2rows(place p) { return cell.lines2rows(p); }
        place rows2lines(place p) { return cell.rows2lines(p); }
    };
}
