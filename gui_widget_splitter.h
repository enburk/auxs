#pragma once
#include "gui_widget.h"
namespace gui
{
    struct splitter:
    widget<splitter>
    {
        property<int> lower;
        property<int> upper;
        bool touched = false;
        xy touch_point;
        int middle = 0;
        str name;

        int set (str name, int lo, int mid, int hi)
        {
            if (mid < lo or hi < mid)
            throw std::logic_error(
            "splitter " + name + ": " +
            std::to_string(lo) + ", " +
            std::to_string(mid)+ ", " +
            std::to_string(hi));

            this->name = name;
            int l = gui::metrics::line::width;
            int w = parent ? parent->coord.now.w : 0;
            int h = parent ? parent->coord.now.h : 0;
            int p = sys::settings::load(
                name + ".permyriad",
                mid*100);

            if (coord.now.size.x > coord.now.size.y)
            {
                lower = h*lo/100;
                upper = h*hi/100;
                middle = clamp<int>(h*p/100'00, lower, upper);
                coord = xyxy(
                0, middle - 10*l,
                w, middle + 10*l);
            }
            else
            {
                lower = w*lo/100;
                upper = w*hi/100;
                middle = clamp<int>(w*p/100'00, lower, upper);
                coord = xyxy(
                middle - 10*l, 0,
                middle + 10*l, h);
            }

            return middle;
        }

        void reset (str name, int mid)
        {
            sys::settings::save(
            name + ".permyriad",
            mid*100);
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size )
                mouse_image =
                coord.now.size.x >
                coord.now.size.y ?
                "horizontal splitter":
                "vertical splitter";
        }

        bool mouse_sensible (xy p) override { return true; }

        void on_mouse_click (xy p, str button, bool down) override
        {
            if (button != "left") return;
            if (down and not touched)
            touch_point = p;
            touched = down;
        }
        void on_mouse_hover (xy p) override
        {
            if (not touched) return;

            int w = parent ? parent->coord.now.w : 0;
            int h = parent ? parent->coord.now.h : 0;

            if (coord.now.size.x > coord.now.size.y)
            {
                int y = coord.now.origin.y;
                y += p.y - touch_point.y;
                y = max (y, lower.now);
                y = min (y, upper.now);
                middle = y + coord.now.h/2;
                if (name != "") sys::settings::save(
                    name + ".permyriad",
                    middle * 100'00 / h);
            }
            else
            {
                int x = coord.now.origin.x;
                x += p.x - touch_point.x;
                x = max (x, lower.now);
                x = min (x, upper.now);
                middle = x + coord.now.w/2;
                if (name != "") sys::settings::save(
                    name + ".permyriad",
                    middle * 100'00 / w);
            }

            notify();
        }
    };
}
