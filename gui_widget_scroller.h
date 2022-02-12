#pragma once
#include "gui_widget.h"
#include "gui_widget_canvas.h"
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
        void on_mouse_hover (XY p) override
        {
            button::on_mouse_hover(p);
            parent->on_mouse_hover(p +
                coord.now.origin);
        }
        void on_mouse_press (XY p, char button, bool down) override
        {
            button::on_mouse_press(p, button, down);
            parent->on_mouse_press(p + 
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
        button up, down, page_up, page_down;
        property<double> ratio = 1;
        property<int> span = 0, top = 0, step = 1;
        bool touch = false;
        int  touch_point;
        int  touch_top;

        scroller ()
        {
            up.text.text = (char*)(u8"\u25B2");
            down.text.text = (char*)(u8"\u25BC");
            up.repeating = true;
            down.repeating = true;
            page_up.on_change_state = [](){};
            page_down.on_change_state = [](){};
            page_up.repeating = true;
            page_down.repeating = true;
        }

        void on_change (void* what) override
        {
            if (what == &coord or
                what == &ratio)
            {
                int w = coord.now.w;
                int h = coord.now.h;
                int d = clamp<int>(std::round(w/ratio.now));
                up.coord = XYWH(0,0,w,d);
                down.coord = XYWH(0,h-d,w,d);
                canvas.coord = XYWH(0,0,w,h);
                up.text.font = sys::font{"", d/2};
                down.text.font = sys::font{"", d/2};
                refresh();
            }
            if (what == &skin)
            {
                canvas.color = gui::skins[skin].light.first;
            }
            if (what == &span)
            {
                if (span.now < 0) throw std::out_of_range
                    ("scroller: negative span");

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

            runner.coord = XYWH(0, d+fake_top, w, fake_page);
            page_up.coord = XYXY(0, d, w, runner.coord.now.y);
            page_down.coord = XYXY(0, d+fake_top+fake_page, w, down.coord.now.y);

            up.enabled = top.now > 0;
            down.enabled = top.now < span.now - real_page;
            runner.enabled = up.enabled.now or down.enabled.now;
        }

        void on_mouse_press (XY p, char button, bool down) override
        {
            if (button != 'L') return;
            if (down && !touch) touch_point = p.y;
            if (down && !touch) touch_top = top.now;
            touch = down;
        }
        void on_mouse_hover (XY p) override
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
        button left, right, page_left, page_right;
        property<double> ratio = 1;
        property<int> span = 0, top = 0, step = 1;
        bool touch = false;
        int  touch_point;
        int  touch_top;

        scroller ()
        {
            left.text.text = (char*)(u8"\u25C0");
            right.text.text = (char*)(u8"\u25B6");
            left.repeating = true;
            right.repeating = true;
            page_left.on_change_state = [](){};
            page_right.on_change_state = [](){};
            page_left.repeating = true;
            page_right.repeating = true;
        }

        void on_change (void* what) override
        {
            if (what == &coord or
                what == &ratio)
            {
                int w = coord.now.w;
                int h = coord.now.h;
                int d = clamp<int>(std::round(h/ratio.now));
                left.coord = XYWH(0,0,d,h);
                right.coord = XYWH(w-d,0,d,h);
                canvas.coord = XYWH(0,0,w,h);
                left.text.font = sys::font{"", d*2/3};
                right.text.font = sys::font{"", d*2/3};
                refresh();
            }
            if (what == &skin)
            {
                canvas.color = gui::skins[skin.now].light.first;
            }
            if (what == &span)
            {
                if (span.now < 0) throw std::out_of_range
                    ("scroller: negative span");

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

            runner.coord = XYWH(d+fake_top, 0, fake_page, h);
            page_left.coord = XYXY(d, 0, runner.coord.now.y, h);
            page_right.coord = XYXY(d+fake_top+fake_page, 0, right.coord.now.y, h);

            left.enabled = top.now > 0;
            right.enabled = top.now < span.now - real_page;
            runner.enabled = left.enabled.now or right.enabled.now;
        }

        void on_mouse_press (XY p, char button, bool down) override
        {
            if (button != 'L') return;
            if (down && !touch) touch_point = p.x;
            if (down && !touch) touch_top = top.now;
            touch = down;
        }
        void on_mouse_hover (XY p) override
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
}
