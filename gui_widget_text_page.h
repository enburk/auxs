#pragma once
#include "gui_widget.h"
#include "gui_widgetarium.h"
#include "gui_widget_text_view.h"
#include "gui_widget_scroller.h"
namespace gui::text
{
    enum WHERE { THERE = 0,
    GLYPH, LINE, LINE_BEGIN, LINE_END, PAGE_TOP,
    TOKEN, PAGE, TEXT_BEGIN, TEXT_END, PAGE_BOTTOM,
    };

    struct page:
    widget<page>
    {
        view view;
        scroll scroll;

        page() { focusable.now = true; }

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
                        view.cell.coord.now.size.x > size.x );
                bool
                    scroll_y =
                    scroll.y.mode == scroll::mode::permanent or (
                    scroll.y.mode == scroll::mode::automatic and
                        view.cell.coord.now.size.y > size.y );
            
                real d = gui::metrics::text::height +
                     2 * gui::metrics::line::width;

                real x = scroll_y ? size.x - d : size.x;
                real y = scroll_x ? size.y - d : size.y;

                scroll.x.show(scroll_x);
                scroll.y.show(scroll_y);

                scroll.x.coord = XYWH(0, size.y-d, x, d);
                scroll.y.coord = XYWH(size.x-d, 0, d, y);

                scroll.x.span = view.cell.coord.now.size.x;
                scroll.y.span = view.cell.coord.now.size.y;

                scroll.x.step = gui::metrics::text::height;
                scroll.y.step = gui::metrics::text::height;

                if (scroll_x) size.y -= d;
                if (scroll_y) size.x -= d;

                view.resize(size);
            }
            if (what == &selections and not selections.now.empty())
            {
                XYXY r = view.cell.carets.back().coord.now +
                         view.shift.now;

                real d = gui::metrics::text::height;
                real w = coord.now.size.x, dx = 0;
                real h = coord.now.size.y, dy = 0;

                if (r.xl-d < 0) dx = r.xl-d; else if (r.xh+d > w) dx = r.xh+d-w;
                if (r.yl-d < 0) dy = r.yl-d; else if (r.yh+d > h) dy = r.yh+d-h;

                if (dx != 0) scroll.x.top = scroll.x.top.now + dx;
                if (dy != 0) scroll.y.top = scroll.y.top.now + dy;
            }
            if (what == &timer)
            {
                if (select_notch < time::now) {
                    select_notch = time::now + select_lapse;
                    real d = gui::metrics::text::height/2;
                    real x = select_point.x, w = coord.now.size.x, dx = 0;
                    real y = select_point.y, h = coord.now.size.y, dy = 0;
                    if (x < 0) dx = x; else if (x > w) dx = x-w;
                    if (y < 0) dy = y; else if (y > h) dy = y-h;
                    dx = dx/d*real(log(abs((float)dx)));
                    dy = dy/d*real(log(abs((float)dy)));
                    if (dx != 0) scroll.x.top = scroll.x.top.now + dx;
                    if (dy != 0) scroll.y.top = scroll.y.top.now + dy;
                    on_mouse_hover(select_point);
                }
            }

            if (what == &scroll.x) view.shift = XY(-scroll.x.top.now, view.shift.now.y);
            if (what == &scroll.y) view.shift = XY(view.shift.now.x, -scroll.y.top.now);

            if (what == &focus_on)
            {
                if (not focus_on.now)
                    view.cell.selection_bars.clear();
                else view.cell.on_change(&selections);
            }

            notify(what);
        }

        auto rows() { return view.rows(); }
        auto row(int n) { return view.row(n); }
        auto selected () { return view.selected(); }

        void go (int where, bool selective = false)
        {
            auto ss = selections.now;
            int n = ss.size();
            if (n >= 2 and not selective) {
                auto b = ss[0].from; // begin of multiline caret
                auto e = ss[n-1].from; // end of multiline caret
                if ((b < e and (where == +GLYPH or where == +LINE))
                or  (b > e and (where == -GLYPH or where == -LINE)))
                ss[0] = ss[n-1];
                ss.resize(1);
            }

            for (auto& caret: ss)
                go(caret, where, selective);

            selections = ss;
        }
        void go (range& caret, int where, bool selective)
        {
            if (rows() == 0) return;

            auto& [from, upto] = caret;
            auto& [r, offset] = upto;

            upto = view.lines2rows(upto);

            int rows_on_page =
            int(view.coord.now.h /
            sys::metrics(font.now).height);

            switch(where){
            case THERE: selective = false; break;

            case-GLYPH:
                offset--;
                if (not virtual_space.now)
                if (offset < 0 and r > 0) { r--;
                    offset = row(r).length; }
                break;
            case+GLYPH:
                offset++;
                if (not virtual_space.now)
                if (offset > row(r).length
                    and r < rows()-1) {
                    r++; offset = 0; }
                break;

            //case-TOKEN: break;
            //case+TOKEN: break;

            case-LINE: r--; break;
            case+LINE: r++; break;

            case LINE_END  : offset = row(r).length; break;
            case LINE_BEGIN: offset = offset !=
                row(r).indent ?
                row(r).indent : 0;
                break;

            //case PAGE_TOP   : break;
            //case PAGE_BOTTOM: break;

            case-PAGE: r -= rows_on_page; break;
            case+PAGE: r += rows_on_page; break;

            case TEXT_BEGIN: upto = place{}; break;
            case TEXT_END  : upto = place{
                rows()-1, row(
                rows()-1).length};
                break;
            }

            if (r > rows()-1)
                r = rows()-1;
            if (r < 0)
                r = 0;

            if (not virtual_space.now and
                offset > row(r).length-1)
                offset = row(r).length-1;
            if (offset < 0)
                offset = 0;

            upto = view.rows2lines(upto);

            if (not selective) from = upto;
        }

        void go (place place)
        {
            scroll.y.top = place.line *
                sys::metrics(font.now).height -
                    view.coord.now.h / 2;

            selections = array<range>{
                range{place, place}};
        }

        void see (int where)
        {
            real h = sys::metrics(font.now).height;

            switch(where){
            case-GLYPH:
                scroll.x.top =
                scroll.x.top.now - h/2;
                break;
            case+GLYPH:
                scroll.x.top =
                scroll.x.top.now + h/2;
                break;
            case-TOKEN:
                scroll.x.top =
                scroll.x.top.now - h*5;
                break;
            case+TOKEN:
                scroll.x.top =
                scroll.x.top.now + h*5;
                break;
            case-LINE:
                scroll.y.top =
                scroll.y.top.now - h;
                break;
            case+LINE:
                scroll.y.top =
                scroll.y.top.now + h;
                break;
            case-PAGE:
                scroll.y.top =
                scroll.y.top.now -
                view.coord.now.h/h*h;
                break;
            case+PAGE:
                scroll.y.top =
                scroll.y.top.now +
                view.coord.now.h/h*h;
                break;
            }
        }

        bool on_mouse_wheel (XY p, int delta) override
        {
            delta /= 20;
            delta *= (int)gui::metrics::text::height;
            int sign = delta < 0 ? -1 : 1;
            if (sys::keyboard::shift) delta = sign * (int)coord.now.h;
            if (sys::keyboard::ctrl) delta *= 5;
            real d = view.coord.now.h - view.cell.coord.now.h; // could be negative
            real y = view.shift.now.y + delta;
            if (y < d) y = d;
            if (y > 0) y = 0;
            scroll.y.top =-y;
            return true;
        }

        bool  touch = false;
        place touch_place;
        time  touch_time;
        XY    touch_point;
        
        property<time> timer;
        time select_delay = time{};
        time select_lapse = 100ms;
        time select_notch;
        XY   select_point;

        bool mouse_sensible (XY p) override { return true; }

        void on_mouse_press (XY p, str button, bool down) override
        {
            if (button == "right") return;
            if (button != "left") return;
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
                    touch_place = view.point(p);
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
                    selection.from = touch_place;
                    selection.upto = view.point(p);
                    selections = array<range>{selection};
                    //info.hide();
                }
                else if (infotip.now)
                {
                    //if (auto token = view.target(p); token && token->info != "")
                    //{
                    //    XYWH r = view.cell.bar(view.point(p).from);
                    //    info.hide(); r.w = r.h*100;
                    //    info.alignment = XY{pix::left, pix::top};
                    //    info.coord = r;
                    //    info.html = token->info;
                    //    r.w = info.cell.coord.now.w + r.h*2; r.y += r.h;
                    //    r.h = info.cell.coord.now.h + r.h/2;
                    //    info.coord = r;
                    //    info.alignment = XY{pix::center, pix::center};
                    //    info.see();
                    //}
                    //else info.hide();
                }
            }
        }


        void on_key (str key, bool down, bool input) override
        {
            if (!down) return;
            if (touch) return; // mouse
            if (input) return; // letters

            if (key == "left" ) see(-GLYPH); else
            if (key == "right") see(+GLYPH); else
            if (key == "up"   ) see(-LINE); else
            if (key == "down" ) see(+LINE); else

            if (key == "ctrl+left" ) see(-TOKEN); else
            if (key == "ctrl+right") see(+TOKEN); else
            if (key == "ctrl+up"   ) see(-LINE); else
            if (key == "ctrl+down" ) see(+LINE); else

            if (key == "shift+left" ) go(-GLYPH, true); else
            if (key == "shift+right") go(+GLYPH, true); else
            if (key == "shift+up"   ) go(-LINE,  true); else
            if (key == "shift+down" ) go(+LINE,  true); else

            if (key == "ctrl+shift+left" ) go(-TOKEN, true); else
            if (key == "ctrl+shift+right") go(+TOKEN, true); else
            if (key == "ctrl+shift+up"   ) go(-LINE,  true); else
            if (key == "ctrl+shift+down" ) go(+LINE,  true); else

            if (key == "home"     ) go(LINE_BEGIN); else
            if (key == "end"      ) go(LINE_END  ); else
            if (key == "page up"  ) { see(-PAGE); go(-PAGE); } else
            if (key == "page down") { see(+PAGE); go(+PAGE); } else

            if (key == "ctrl+home"     ) go(TEXT_BEGIN ); else
            if (key == "ctrl+end"      ) go(TEXT_END   ); else
            if (key == "ctrl+page up"  ) go(PAGE_TOP   ); else
            if (key == "ctrl+page down") go(PAGE_BOTTOM); else

            if (key == "shift+home"     ) go(LINE_BEGIN, true); else
            if (key == "shift+end"      ) go(LINE_END,   true); else
            if (key == "shift+page up"  ) { see(-PAGE); go(-PAGE, true); } else
            if (key == "shift+page down") { see(+PAGE); go(+PAGE, true); } else

            if (key == "ctrl+shift+home"     ) go(TEXT_BEGIN , true); else
            if (key == "ctrl+shift+end"      ) go(TEXT_END   , true); else
            if (key == "ctrl+shift+page up"  ) go(PAGE_TOP   , true); else
            if (key == "ctrl+shift+page down") go(PAGE_BOTTOM, true); else

            if (key == "ctrl+C"     ) { sys::clipboard::set(selected()); } else
            if (key == "ctrl+insert") { sys::clipboard::set(selected()); } else
            if (key == "escape"     ) { go(THERE); } else

            {}
        }
    };
} 