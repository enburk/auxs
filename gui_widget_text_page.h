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
        str link;

        page() { focusable.now = true; }

        binary_property<xyxy> padding;
        binary_property<bool> infotip = false;

#define using(x) decltype(view.x)& x = view.x;
        using(canvas)
        using(text)
        using(html)
        using(color)
        using(font)
        using(style)
        using(wordwrap)
        using(ellipsis)
        using(alignment)
        using(lwrap)
        using(rwrap)
        using(update_text)
        using(update_colors)
        using(update_layout)
        using(highlights)
        using(selections)
        using(virtual_space)
        using(insert_mode)
        using(read_only)
        #undef using

        void on_change (void* what) override
        {
            if (what == &coord
            or  what == &padding
            or  what == &update_text
            or  what == &update_layout)
            {
                xy size = coord.now.size;
                
                bool
                scroll_x =
                scroll.x.mode == scroll::mode::permanent or
                scroll.x.mode == scroll::mode::automatic and
                padding.now.xl + padding.now.xh +
                view.cell.coord.now.size.x >
                size.x;

                bool
                scroll_y =
                scroll.y.mode == scroll::mode::permanent or
                scroll.y.mode == scroll::mode::automatic and
                padding.now.yl + padding.now.yh +
                view.cell.coord.now.size.y >
                size.y;
            
                int d = gui::metrics::text::height * 7/10 +
                    2 * gui::metrics::line::width;

                int x = scroll_y ? size.x - d : size.x;
                int y = scroll_x ? size.y - d : size.y;

                scroll.x.show(scroll_x);
                scroll.y.show(scroll_y);

                scroll.x.coord = xywh(0, size.y-d, x, d);
                scroll.y.coord = xywh(size.x-d, 0, d, y);

                scroll.x.span = view.cell.coord.now.size.x;
                scroll.y.span = view.cell.coord.now.size.y;

                scroll.x.step = gui::metrics::text::height;
                scroll.y.step = gui::metrics::text::height;

                if (scroll_x) size.y -= d;
                if (scroll_y) size.x -= d;

                size.x -= padding.now.xl + padding.now.xh;
                size.y -= padding.now.yl + padding.now.yh;

                view.coord = xywh(
                    padding.now.xl,
                    padding.now.yl,
                    size.x,
                    size.y);

                timer.go(gui::time{}, gui::time{});
                touch = false;
            }
            if (what == &selections and not view.cell.carets.empty())
            {
                xyxy r = 
                view.cell.carets.back().coord.now +
                view.shift.now;

                int d = gui::metrics::text::height;
                int w = coord.now.size.x, dx = 0;
                int h = coord.now.size.y, dy = 0;

                if (r.xl-d < 0) dx = r.xl-d; else if (r.xh+d > w) dx = r.xh+d-w;
                if (r.yl-d < 0) dy = r.yl-d; else if (r.yh+d > h) dy = r.yh+d-h;

                if (dx != 0) scroll.x.top = scroll.x.top.now + dx;
                if (dy != 0) scroll.y.top = scroll.y.top.now + dy;
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

            if (what == &scroll.x) view.shift = xy(-scroll.x.top, view.shift.now.y);
            if (what == &scroll.y) view.shift = xy(view.shift.now.x, -scroll.y.top);

            if (what == &focus_on)
            {
                if (not focus_on.now)
                view.cell.selection_bars.clear(); else
                view.cell.on_change(&selections);
            }

            notify(what);
        }

        auto rows() { return view.rows(); }
        auto row(int n) { return view.row(n); }
        auto selected () { return view.selected(); }

        void go (int where, bool selective = false)
        {
            auto ss =
            selections.now;
            int n = ss.size();
            if (n >= 2 and not selective and where == THERE)
            {
                ss.front() = ss.back();
                ss.front().from =
                ss.front().upto;
                ss.resize(1);
            }
            else
            if (n >= 2 and (where == -LINE or where == +LINE))
            {
                auto upto1 = ss.front().upto;
                auto upto2 = ss.back ().upto;
                upto1 = view.lines2rows(upto1);
                upto2 = view.lines2rows(upto2);

                int r = rows()-1;
                if((where == -LINE and upto1.line > 0 and upto2.line > 0)
                or (where == +LINE and upto1.line < r and upto2.line < r))
                for (auto& caret: ss) go(
                caret, where, false);
            }
            else
            if (n == 1 and not selective and ss.front().from != ss.front().upto)
            {
                auto& from = ss.front().from;
                auto& upto = ss.front().upto;
                if (from > upto) std::swap(from, upto);

                if (where == -GLYPH or where == -LINE
                or  where == -TOKEN or where == -PAGE)
                upto = from;
                else
                if (where == +GLYPH or where == +LINE
                or  where == +TOKEN or where == +PAGE)
                from = upto;
                else
                for (auto& caret: ss) go(
                caret, where, false);
            }
            else
            {
                if (where == TEXT_BEGIN
                or  where == TEXT_END)
                    ss.resize(1);

                for (auto& caret: ss) go(
                caret, where, selective);
            }

            selections = ss;
        }
        void go (range& caret, int where, bool selective)
        {
            if (rows() == 0) return;

            auto& [from, upto] = caret;
            auto& [r, offset] = upto;

            upto = view.lines2rows(upto);

            int rows_on_page =
                view.coord.now.h /
                pix::metrics(font.now).height;

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

            // allowed to be
            // right after the last char
            if (not virtual_space.now and
                offset > row(r).length)
                offset = row(r).length;
            if (offset < 0)
                offset = 0;

            upto = view.rows2lines(upto);

            if (not selective) from = upto;
        }

        void go (place place)
        {
            scroll.y.top = place.line *
                pix::metrics(font.now).height -
                    view.coord.now.h / 2;

            selections = array<range>{
                range{place, place}};
        }

        void see (int where)
        {
            int h = pix::metrics(font.now).height;

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

        bool on_mouse_wheel (xy, int delta) override
        {
            delta /= 20;
            delta *= gui::metrics::text::height;
            int sign = delta < 0 ? -1 : 1;
            if (sys::keyboard::shift) delta = sign * coord.now.h;
            if (sys::keyboard::ctrl) delta *= 5;
            int d = view.coord.now.h - view.cell.coord.now.h; // could be negative
            int y = view.shift.now.y + delta;
            if (y < d) y = d;
            if (y > 0) y = 0;
            scroll.y.top =-y;
            return true;
        }

        bool  touch = false;
        range touch_range;
        time  touch_time;
        xy    touch_point;
        
        property<time> timer;
        time select_delay = time{};
        time select_lapse = 100ms;
        time select_notch;
        xy   select_point;

        bool mouse_sensible (xy p) override { return true; }

        void on_mouse_click (xy p, str button, bool down) override
        {
            p -= view.coord.now.origin;

            if (button == "right") return;
            if (button != "left" ) return;

            if (down
            and not touch
            and not sys::keyboard::ctrl)
            {
                select_point = p;
                auto  place = view.pointed(p);
                auto& block = view.cell.box.model->block;

                if (touch_point == p
                and time::now < touch_time + 1000ms)
                {
                    touch_range = block.token_placed(place).range;
                }
                else
                {
                    touch_range = {place, place};
                    link = block.link(p - view.shift);
                    if (link != "") notify(&link);
                }
                touch_point = p;
                touch_time = time::now;
            }

            touch = down;
            select_notch = time::now + select_delay;
            timer.go(down? time::infinity : time(),
                     down? time::infinity : time());
        }

        void on_mouse_hover (xy p) override
        {
            p -= view.coord.now.origin;

            bool drag_and_drop = false;
            bool inside_selection = false;

            mouse_image = drag_and_drop ?
            inside_selection ? "noway" : "arrow" :
            inside_selection ? "arrow" : "editor";

            if (drag_and_drop)
            {
                return;
            }

            if (touch)
            {
                select_point =  p;
                auto place = view.pointed(p);
                selections = array<range>{
                place< touch_range.from ? range{
                place, touch_range.upto} :
                touch_range.upto< place ? range{
                touch_range.from, place} :
                touch_range};
                //info.hide();
                return;
            }

            if (infotip.now)
            {
                //if (auto token = view.target(p); token && token->info != "")
                //{
                //    xywh r = view.cell.bar(view.point(p).from);
                //    info.hide(); r.w = r.h*100;
                //    info.alignment = xy{pix::left, pix::top};
                //    info.coord = r;
                //    info.html = token->info;
                //    r.w = info.cell.coord.now.w + r.h*2; r.y += r.h;
                //    r.h = info.cell.coord.now.h + r.h/2;
                //    info.coord = r;
                //    info.alignment = xy{pix::center, pix::center};
                //    info.see();
                //}
                //else info.hide();
            }

            if (sys::keyboard::ctrl)
            {
                link = "";
                bool same = true;
                for (auto token: view.visible_tokens())
                for (auto& glyph: token->glyphs)
                if (glyph.style_index != token->style) {
                    glyph.style_index  = token->style;
                    same = false; }
                if (not same) update();
                return;
            }

            auto& block = view.cell.box.model->block;
            auto* token = block.hovered_token(p - view.shift);
            link = block.link(p - view.shift);
            bool same = true;

            mouse_image = link != "" ? "hand" :
            token or virtual_space ? "editor" : "arrow";

            for (auto token: view.visible_tokens())
            {
                auto style_index = token->style;
                if (link != "" and token->link == link)
                {
                    auto style = style_index.style();
                    style.color = skins[skin].link.first;
                    style_index = pix::text::style_index(style);
                }
                for (auto& glyph: token->glyphs)
                if (glyph.style_index != style_index) {
                    glyph.style_index  = style_index;
                    same = false; }
            }
            if (not same) update();
        }

        void on_mouse_leave () override
        {
            link = "";
            bool same = true;
            for (auto token: view.visible_tokens())
            for (auto& glyph: token->glyphs)
            if (glyph.style_index != token->style) {
                glyph.style_index  = token->style;
                same = false; }
            if (not same) update();
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
            if (key == "ctrl+A") { go(TEXT_BEGIN ); go(TEXT_END, true); } else

            if (key == "ctrl+C"     ) { sys::clipboard::set(selected()); } else
            if (key == "ctrl+insert") { sys::clipboard::set(selected()); } else
            if (key == "escape"     ) { go(THERE); } else

            {}
        }
    };
} 