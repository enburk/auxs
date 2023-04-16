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

    struct ground:
    widget<ground>
    {
        canvas canvas;

        void on_change (void* what) override
        {
            if (what == &coord)
            canvas.coord = coord.now.local();

            if (what == &skin)
            canvas.color = skins[skin].
            ultralight.first;
        }
    };

    struct toolbar:
    widget<toolbar>
    {
        canvas canvas;

        void on_change (void* what) override
        {
            if (what == &coord)
            canvas.coord = coord.now.local();

            if (what == &skin)
            canvas.color = skins[skin].
            light.first;
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

    struct Frame:
    widget<Frame>
    {
        frame frame1;
        frame frame2;
        frame frame3;

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int d1 = frame1.thickness.now;
                int d2 = frame2.thickness.now;
                int d3 = frame3.thickness.now;
                int dd = 2*d1 + 2*d2 + 2*d3;
                xywh r = coord.now.local();
                if (r.w < dd or r.h < dd)
                {
                    frame1.coord = xywh{};
                    frame2.coord = xywh{};
                    frame3.coord = xywh{};
                }
                else
                {
                    frame1.coord = r; r.deflate(d1);
                    frame2.coord = r; r.deflate(d2);
                    frame3.coord = r; r.deflate(d3);
                }
            }
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
                auto focal = focus_on.now
                    and show_focus.now?
                    style.focused.first :
                    light;

                frame1.color = light;
                frame2.color = heavy;
                frame3.color = focal;
            }
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int d1 = frame1.thickness.now;
                int d2 = frame2.thickness.now;
                int d3 = frame3.thickness.now;
                int dd = 2*d1 + 2*d2 + 2*d3;
                xywh r = coord.now.local();
                if (r.w < dd or r.h < dd)
                {
                    frame1.coord = xywh{};
                    frame2.coord = xywh{};
                    frame3.coord = xywh{};
                    object.coord = xywh{};
                }
                else
                {
                    frame1.coord = r; r.deflate(d1);
                    frame2.coord = r; r.deflate(d2);
                    frame3.coord = r; r.deflate(d3);
                    object.coord = r;
                }
            }
            if (what == &object)
            {
                notify();
            }
            notify(what);
        }
    };
}
