#pragma once
#include "doc_html_model.h"
#include "gui_widget_text_cell.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    struct view:
    widget<view>
    {
        str brief;
        canvas canvas; cell cell;
        frame current_line_frame;
        frame frame;

        property<bool> wordwrap = true;
        property<bool> ellipsis = false;
        binary_property<array<xy>> lwrap;
        binary_property<array<xy>> rwrap;
        binary_property<xy> alignment = xy{pix::center, pix::center};
        binary_property<xy> shift;

#define using(x) decltype(cell.x)& x = cell.x;
        using(text)
        using(html)
        using(color)
        using(font)
        using(style)
        using(update_text)
        using(update_colors)
        using(update_layout)
        using(highlights)
        using(selections)
        using(virtual_space)
        using(insert_mode)
        using(read_only)
        #undef using

        doc::model*& model = cell.box.model;

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

            if (what == &update_text)
                brief = cell.box.model->brief();

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
                if (focus_on.now and
                    cell.carets.size() == 1) { xywh r =
                    cell.carets(0).coord.now;
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

        generator<pix::text::token*> visible_tokens ()
        {
            for (auto& column: model->block.columns)
            for (auto& row: column.rows)
            {
                int ry = row->offset.y;
                int rh = row->Height();
                int vy = cell.coord.now.y;
                int vh = cell.coord.now.h;
                vh = min (vh, coord.now.h - vy);

                if (ry + rh < vy
                or ry >= vy + vh)
                    continue;

                for (auto& solid: row->solids)
                for (auto& token: solid.tokens)
                co_yield & token;
            }
        }
    };
}
