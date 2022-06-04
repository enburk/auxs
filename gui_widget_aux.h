#pragma once
#include "gui_widget.h"
namespace gui
{
    struct canvas:
    widget<canvas>
    {
        property<rgba> color;

        Opacity opacity () override { return
            color.now.a == 255 ? opaque :
            color.now.a == 0 ? transparent :
                             semitransparent; }

        void on_render (sys::window& window, xywh r, xy offset, uint8_t alpha) override
        {
            window.render(r, alpha, color.now);
        }

        void on_change (void* what) override
        {
            if (what == &color)
                if (color.was.a != 0 or
                    color.now.a != 0 )
                    update();
        }
    };

    struct frame:
    widget<frame>
    {
        property<rgba> color;
        property<int> thickness = gui::metrics::line::width;

        canvas l, t, r, b; // left, top, right, bottom

        void on_change () override
        {
            int w = coord.now.w;
            int h = coord.now.h;
            int d = thickness.now;
            t.coord = xywh(0, 0,   w, d);
            b.coord = xywh(0, h-d, w, d);
            l.coord = xywh(0,   d, d, h-d-d);
            r.coord = xywh(w-d, d, d, h-d-d);
            t.color = color.now;
            b.color = color.now;
            l.color = color.now;
            r.color = color.now;
        }
    };

    template
    <class X>
    struct area :
    widget<area<X>>
    {
        frame frame1;
        frame frame2;
        frame frame3; X object;

        property<bool> show_focus = false;

        using widget<area<X>>::skin;
        using widget<area<X>>::coord;
        using widget<area<X>>::notify;
        using widget<area<X>>::focus_on;

        void on_change (void* what) override
        {
            if (what == &skin or
                what == &focus_on or
                what == &show_focus)
            {
                auto style = skins[skin];
                auto light = style.light.first;
                auto heavy = style.heavy.first;
                auto focus = focus_on.now
                    and show_focus.now?
                    style.focused.first :
                    light;

                frame1.color = light;
                frame2.color = heavy;
                frame3.color = focus;
            }
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                auto r = coord.now.local();
                frame1.coord = r; r.deflate(frame1.thickness.now);
                frame2.coord = r; r.deflate(frame2.thickness.now);
                frame3.coord = r; r.deflate(frame3.thickness.now);
                object.coord = r;
            }
            if (what == &object)
            {
                notify();
            }
            notify(what);
        }
    };

    struct splitter:
    widget<splitter>
    {
        property<int> lower;
        property<int> upper;
        bool touched = false;
        xy touch_point;
        int middle = 0;

        void on_change (void* what) override
        {
            if (what == &coord and coord.was.size != coord.now.size )
                mouse_image = coord.now.size.x > coord.now.size.y ?
                    "horizontal splitter" : "vertical splitter";
        }

        bool mouse_sensible (xy p) override { return true; }

        void on_mouse_click (xy p, str button, bool down) override
        {
            if (button != "left") return;
            if (down && !touched) touch_point = p;
            touched = down;
        }
        void on_mouse_hover (xy p) override
        {
            if (!touched) return;

            if (coord.now.size.x > coord.now.size.y)
            {
                int y = coord.now.origin.y + p.y - touch_point.y;
                y = max (y, lower.now);
                y = min (y, upper.now);
                middle = y + coord.now.h/2;
                notify();
            }
            else
            {
                int x = coord.now.origin.x + p.x - touch_point.x;
                x = max (x, lower.now);
                x = min (x, upper.now);
                middle = x + coord.now.w/2;
                notify();
            }
        }
    };
}
