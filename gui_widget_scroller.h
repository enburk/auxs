#pragma once
#include "gui_widget.h"
#include "gui_widget_aux.h"
#include "gui_widget_button.h"
namespace gui
{
    enum class orientation { horizontal, vertical };
    const orientation vertical = orientation::vertical;
    const orientation horizontal = orientation::horizontal;
    constexpr orientation operator ~ (orientation orientation)
    { return orientation == horizontal ? vertical : horizontal; }

    struct runner : button
    {
        void on_mouse_hover (xy p) override
        {
            button::on_mouse_hover(p);
            parent->on_mouse_hover(p +
                coord.now.origin);
        }
        void on_mouse_click (xy p, str button, bool down) override
        {
            button::on_mouse_click(p, button, down);
            parent->on_mouse_click(p + 
                coord.now.origin,
                button, down);
        }
    };

    template<orientation>
    struct scroller;template<>
    struct scroller<vertical>:
    widget<scroller<vertical>>
    {
        canvas canvas;
        runner runner;
        button up, down;
        button page_up, page_down;
        property<double> ratio = 1;
        property<int> span = 0, top = 0, step = 1;
        bool touch = false;
        int  touch_point;
        int  touch_top;

        scroller ()
        {
            up.text.text = (char*)(u8"\u25B2");
            down.text.text = (char*)(u8"\u25BC");
            up.repeat_lapse = 40ms;
            down.repeat_lapse = 40ms;
            page_up.repeat_lapse = 40ms;
            page_down.repeat_lapse = 40ms;
            page_up.on_change_state = [](){};
            page_down.on_change_state = [](){};
        }

        void on_change (void* what) override
        {
            if (what == &coord
            or  what == &ratio)
            {
                int w = coord.now.w;
                int h = coord.now.h;
                int d = clamp<int>(std::round(w/ratio.now));
                up.coord = xywh(0,0,w,d);
                down.coord = xywh(0,h-d,w,d);
                canvas.coord = xywh(0,0,w,h);
                up.text.font = pix::font{"", d*7/10};
                down.text.font = pix::font{"", d*7/10};
                refresh();
            }
            if (what == &skin)
            {
                canvas.color = gui::skins[skin].light.first;
            }
            if (what == &span
            or  what == &coord)
            {
                if (span.now < 0)
                throw std::out_of_range(
                "scroller: negative span");

                int y = max (0, min (top.now, span.now - coord.now.h));
                if (top.now != y) top = y;
                else refresh();
            }
            if (what == &top)
            {
                top.now = max (0, min (top.now, span.now - coord.now.h));
                if (top.now != top.was) {
                    refresh();
                    notify();
                }
            }
            if (what == &up) top = top.now - step.now;
            if (what == &down) top = top.now + step.now;
            if (what == &page_up) top = top.now - coord.now.h;
            if (what == &page_down) top = top.now + coord.now.h;
        }

        void refresh ()
        {
            assert(top.now >= 0);
            assert(span.now >= 0);
            assert(top.now <= span.now);

            int real_page = coord.now.h; if (real_page <= 0) return;
            int fake_span = coord.now.h - 2*up.coord.now.h;
            int fake_page = fake_span * real_page / max(1, span.now);
            int fake_top  = fake_span * top.now   / max(1, span.now);
            fake_page = min(fake_span, max(fake_page, up.coord.now.h/4));
            int w = up.coord.now.w;
            int d = up.coord.now.h;

            runner.coord = xywh(0, d+fake_top, w, fake_page);
            page_up.coord = xyxy(0, d, w, runner.coord.now.y);
            page_down.coord = xyxy(0, d+fake_top+fake_page, w, down.coord.now.y);

            up.enabled = top.now > 0;
            down.enabled = top.now < span.now - real_page;
            runner.enabled = up.enabled.now or down.enabled.now;
        }

        void on_mouse_click (xy p, str button, bool down) override
        {
            if (button != "left") return;
            if (down && !touch) touch_point = p.y;
            if (down && !touch) touch_top = top.now;
            touch = down;
        }
        void on_mouse_hover (xy p) override
        {
            if (!touch) return;
            int real_page = coord.now.h; if (real_page <= 0) return;
            int fake_span = coord.now.h - 2*up.coord.now.h;
            int fake_page = fake_span * real_page / max(1, span.now);
            fake_page = min(fake_span, fake_page); if (fake_page <= 0) return;
            top = touch_top + (p.y - touch_point) * real_page/fake_page;
        }
    };

    template<>
    struct scroller<horizontal>:
    widget<scroller<horizontal>>
    {
        canvas canvas;
        runner runner;
        button left, right;
        button page_left, page_right;
        property<double> ratio = 1;
        property<int> span = 0, top = 0, step = 1;
        bool touch = false;
        int  touch_point;
        int  touch_top;

        scroller ()
        {
            left.text.text = (char*)(u8"\u25C0");
            right.text.text = (char*)(u8"\u25B6");
            left.repeat_lapse = 40ms;
            right.repeat_lapse = 40ms;
            page_left.repeat_lapse = 40ms;
            page_right.repeat_lapse = 40ms;
            page_left.on_change_state = [](){};
            page_right.on_change_state = [](){};
        }

        void on_change (void* what) override
        {
            if (what == &coord or
                what == &ratio)
            {
                int w = coord.now.w;
                int h = coord.now.h;
                int d = clamp<int>(std::round(h/ratio.now));
                left.coord = xywh(0,0,d,h);
                right.coord = xywh(w-d,0,d,h);
                canvas.coord = xywh(0,0,w,h);
                left.text.font = pix::font{"", d};
                right.text.font = pix::font{"", d};
                refresh();
            }
            if (what == &skin)
            {
                canvas.color = gui::skins[skin.now].light.first;
            }
            if (what == &span
            or  what == &coord)
            {
                if (span.now < 0)
                throw std::out_of_range(
                "scroller: negative span");

                int x = max (0, min (top.now, span.now - coord.now.w));
                if (top.now != x) top = x;
                else refresh();
            }
            if (what == &top)
            {
                top.now = max (0, min (top.now, span.now - coord.now.w));
                if (top.now != top.was) {
                    refresh();
                    notify();
                }
            }
            if (what == &left) top = top.now - step.now;
            if (what == &right) top = top.now + step.now;
            if (what == &page_left) top = top.now - coord.now.size.x;
            if (what == &page_right) top = top.now + coord.now.size.x;
        }

        void refresh ()
        {
            assert(top.now >= 0);
            assert(span.now >= 0);
            assert(top.now <= span.now);

            int real_page = coord.now.w; if (real_page <= 0) return;
            int fake_span = coord.now.w - 2*left.coord.now.w;
            int fake_page = fake_span * real_page / max(1, span.now);
            int fake_top  = fake_span * top.now   / max(1, span.now);
            fake_page = min(fake_span, max(fake_page, left.coord.now.h/4));
            int h = left.coord.now.h;
            int d = left.coord.now.w;

            runner.coord = xywh(d+fake_top, 0, fake_page, h);
            page_left.coord = xyxy(d, 0, runner.coord.now.x, h);
            page_right.coord = xyxy(d+fake_top+fake_page, 0, right.coord.now.x, h);

            left.enabled = top.now > 0;
            right.enabled = top.now < span.now - real_page;
            runner.enabled = left.enabled.now or right.enabled.now;
        }

        void on_mouse_click (xy p, str button, bool down) override
        {
            if (button != "left") return;
            if (down && !touch) touch_point = p.x;
            if (down && !touch) touch_top = top.now;
            touch = down;
        }
        void on_mouse_hover (xy p) override
        {
            if (!touch) return;
            int real_page = coord.now.w; if (real_page <= 0) return;
            int fake_span = coord.now.w - 2*left.coord.now.w;
            int fake_page = fake_span * real_page / max(1, span.now);
            fake_page = min(fake_span, fake_page); if (fake_page <= 0) return;
            top = (touch_top * fake_page + (p.x - touch_point) * real_page)/fake_page;
        }
    };

    struct scroll
    {
        enum class mode { none, automatic, permanent };
        struct X : scroller<horizontal> { mode mode = mode::automatic; }; X x;
        struct Y : scroller<vertical>   { mode mode = mode::automatic; }; Y y;
    };

    struct wheeler:
    widget<wheeler>
    {
        wheeler(
        base::widget& view) : view(view) { scroller.step = gui::metrics::text::height*12/10; }
        base::widget& view;

        scroller<
        vertical>
        scroller;

        void refresh()
        {
            int t =
            scroller.top;
            scroller.span = view.coord.now.h;
            scroller.top  = t;
            xywh r = view.coord;
            r.y = -scroller.top;
            view.coord = r;
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size) {
                scroller.coord = xyxy(0,0,0,coord.now.h);
                refresh();
            }

            if (what == &scroller) {
                xywh r = view.coord;
                r.y = -scroller.top;
                view.coord = r;
            }
        }

        bool on_mouse_wheel (xy, int delta) override
        {
            int h = scroller.step;
            delta /= abs(delta) < 20 ? abs(delta) : 20;
            delta *= h > 0 ? h : gui::metrics::text::height;
            if (sys::keyboard::ctrl ) delta *= 5;
            if (sys::keyboard::shift) delta *= coord.now.h;
            int d = coord.now.h - view.coord.now.h; // may be negative
            int y = view.coord.now.y + delta;
            if (y < d) y = d;
            if (y > 0) y = 0;
            scroller.top =-y;
            return true;
        }
    };

    struct list:
    widget<list>
    {
        widgetarium<
            button>
            lines;

        wheeler wheeler{lines};

        int selected = 0;

        void refresh()
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height*12/10;
            int l = gui::metrics::line::width;
            int hh = h * lines.size();

            lines.coord = xywh(0, 0, W, hh);

            int y = 0;
            for (auto & line : lines) {
            line.coord = xywh(0, y, W, h);
            y += h; }

            wheeler.refresh();
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size) { wheeler.coord =
                coord.now.local();
                refresh();
            }

            if (what == &lines) {
                selected = lines.notifier_index;
                notify();
            }
        }
    };

    struct table:
    widget<table>
    {
        widgetarium<
        widgetarium<
            button>>
            cells;

        wheeler wheeler{cells};

        xy selected;

        auto cell (int x, int y) -> gui::button&
        {
            while (
            cells.size() < y)
            cells.emplace_back();

            for (int i=
            cells[y].size(); i<x; i++)
            cells[y][i].hide();

            return cells[y][x];
        };

        void refresh()
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height*12/10;
            int Y = cells.size();
            int X = 1;
            int w = 0;

            for (auto& row: cells)
            X = max(X, row.size());
            w = W/X;

            for (int y=0; y<Y; y++)
            for (int x=0; x<X; x++)
            if (y < cells   .size())
            if (x < cells[y].size())
            cells[y][x].coord = xywh(x*w, 0, w, h),
            cells[y]   .coord = xywh(0, y*h, W, h);
            cells      .coord = xyxy(0, 0, W, Y*h);

            wheeler.refresh();
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size) { wheeler.coord =
                coord.now.local();
                refresh();
            }

            if (what == &cells) {
                selected.y = cells.notifier_index;
                selected.x = cells.notifier->notifier_index;
                notify();
            }
        }
    };
}
