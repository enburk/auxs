#pragma once
#include "gui_widget.h"
#include "gui_widgetarium.h"
#include "gui_widget_text_view.h"
#include "gui_widget_scroller.h"
namespace gui::text
{
    struct page:
    widget<page>
    {
        view view;
        scroll scroll;
        gui::text::view info;

        lines& lines = view.lines;
        canvas& canvas = view.canvas;
        view::text_type& text = view.text;
        view::html_type& html = view.html;
        property<RGBA>& color = view.color;
        binary_property<font>& font = view.font;
        binary_property<style>& style = view.style;
        binary_property<XY>& alignment = view.alignment;
        binary_property<int>& lpadding = view.lpadding;
        binary_property<int>& rpadding = view.rpadding;
        binary_property<array<XY>>& lwrap = view.lwrap;
        binary_property<array<XY>>& rwrap = view.rwrap;
        unary_property<array<range>>& highlights = view.highlights;
        unary_property<array<range>>& selections = view.selections;
        binary_property<bool>& wordwrap = view.wordwrap;
        binary_property<bool>& ellipsis = view.ellipsis;
        binary_property<bool>& virtual_space = view.virtual_space;
        binary_property<bool>& insert_mode = view.insert_mode;
        binary_property<bool>& focused = view.focused;
        property<bool>& update_text = view.update_text;
        property<bool>& update_colors = view.update_colors;
        property<bool>& update_layout = view.update_layout;

        binary_property<bool> infotip = false;

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                view.coord = coord.now.local();
            }
            if (what == &update_text
            or  what == &update_layout)
            {
                XY size = coord.now.size;
                bool
                    scroll_x =
                    scroll.x.mode == scroll::mode::permanent or (
                    scroll.x.mode == scroll::mode::automatic and
                        lines.coord.now.size.x > size.x );
                bool
                    scroll_y =
                    scroll.y.mode == scroll::mode::permanent or (
                    scroll.y.mode == scroll::mode::automatic and
                        lines.coord.now.size.y > size.y );
            
                int d = gui::metrics::text::height +
                    2 * gui::metrics::line::width;

                int x = scroll_y ? size.x - d : size.x;
                int y = scroll_x ? size.y - d : size.y;

                scroll.x.show(scroll_x);
                scroll.y.show(scroll_y);

                scroll.x.coord = XYWH(0, size.y-d, x, d);
                scroll.y.coord = XYWH(size.x-d, 0, d, y);

                scroll.x.span = lines.coord.now.size.x;
                scroll.y.span = lines.coord.now.size.y;

                scroll.x.step = gui::metrics::text::height;
                scroll.y.step = gui::metrics::text::height;

                if (scroll_x) size.y -= d;
                if (scroll_y) size.x -= d;

                view.resize(size);
            }
            if (what == &timer)
            {
                if (select_notch < time::now) {
                    select_notch = time::now + select_lapse;
                    int d = gui::metrics::text::height/2;
                    int x = select_point.x, w = coord.now.size.x, dx = 0;
                    int y = select_point.y, h = coord.now.size.y, dy = 0;
                    if (x < 0) dx = x; else if (x > w) dx = x-w;
                    if (y < 0) dy = y; else if (y > h) dy = y-h;
                    dx = dx/d*(int)(log(abs(dx)));
                    dy = dy/d*(int)(log(abs(dy)));
                    if (dx != 0) scroll.x.top = scroll.x.top.now + dx;
                    if (dy != 0) scroll.y.top = scroll.y.top.now + dy;
                    on_mouse_hover(select_point);
                }
            }

            if (what == &scroll.x) view.shift = XY(-scroll.x.top, view.shift.now.y);
            if (what == &scroll.y) view.shift = XY(view.shift.now.x, -scroll.y.top);

            notify(what);
        }

        bool on_mouse_wheel (XY p, int delta) override
        {
            delta /= 20;
            delta *= gui::metrics::text::height;
            int sign = delta < 0 ? -1 : 1;
            if (sys::keyboard::shift) delta = sign * coord.now.h;
            if (sys::keyboard::ctrl) delta *= 5;
            int d = view.coord.now.h - lines.coord.now.h; // could be negative
            int y = view.shift.now.y + delta;
            if (y < d) y = d;
            if (y > 0) y = 0;
            scroll.y.top =-y;
            return true;
        }

        auto selected () { return view.selected(); }

        bool  touch = false;
        range touch_range;
        time  touch_time;
        XY    touch_point;
        
        property<time> timer;
        time select_delay = time{};
        time select_lapse = 100ms;
        time select_notch;
        XY   select_point;

        bool mouse_sensible (XY p) override { return true; }

        void on_mouse_press (XY p, char button, bool down) override
        {
            if (button == 'R') return;
            if (button != 'L') return;
            if (down && !touch)
            {
                if (touch_point == p and time::now -
                    touch_time < 1000ms) // double click
                {
                    //go(-TOKEN); go(+TOKEN, true); // select token
                    //while (caret_upto > 0 &&
                    //    symbol_kind(caret_upto-1) == ' ')
                    //    caret_upto--;
                    //refresh();
                }
                else
                {
                    select_point = p;
                    touch_range = view.point(p);
                }
                touch_point = p;
                touch_time = time::now;
            }
            touch = down;

            select_notch = time::now + select_delay;
            timer.go (down ? time::infinity : time(),
                      down ? time::infinity : time());
        }

        void on_mouse_hover (XY p) override
        {
            bool drag_and_drop = false;
            bool inside_selection = false;

            mouse_image = drag_and_drop ?
            inside_selection ? "noway" : "arrow" :
            inside_selection ? "arrow" : "editor";

            if (!drag_and_drop)
            {
                if (touch)
                {
                    select_point = p;
                    range selection;
                    selection.from = touch_range.from;
                    selection.upto = view.point(p).from; // not point(p).upto !
                    selections = array<range>{selection};
                    info.hide();
                }
                else if (infotip.now)
                {
                    if (auto token = view.target(p); token && token->info != "")
                    {
                        XYWH r = view.cell.bar(view.point(p).from);
                        info.hide(); r.w = r.h*100;
                        info.alignment = XY{pix::left, pix::top};
                        info.coord = r;
                        info.html = token->info;
                        r.w = info.cell.coord.now.w + r.h*2; r.y += r.h;
                        r.h = info.cell.coord.now.h + r.h/2;
                        info.coord = r;
                        info.alignment = XY{pix::center, pix::center};
                        info.show();
                    }
                    else info.hide();
                }
            }
        }

        void on_key_pressed (str key, bool down) override
        {
            if (!down) return;
            if (touch) return; // mouse
            if (selections.now.size() != 1) return;
            auto selection = selections.now[0];
            auto & [from, upto] = selection;

            if (key == "shift+left")
            {
                if (upto.offset > 0) {
                    upto.offset--;
                    selections = array<range>{
                    selection};
                }
            }
            if (key == "shift+right")
            {
                int n = lines(from.line).length;
                if (upto.offset < n) {
                    upto.offset++;
                    selections = array<range>{
                    selection};
                }
            }
            if (key == "ctrl+left" or
                key == "ctrl+shift+left")
            {
                if (from.offset > 0) {
                    from.offset--;
                    selections = array<range>{
                    selection};
                }
            }
            if (key == "ctrl+right" or
                key == "ctrl+shift+right")
            {
                int n = lines(from.line).length;
                if (from.offset < n) {
                    from.offset++;
                    selections = array<range>{
                    selection};
                }
            }

            if (key == "ctrl+C" or
                key == "ctrl+insert") {
                sys::clipboard::set(selected());
            }
        }
    };
} 